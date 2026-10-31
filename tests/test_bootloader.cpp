// [0.2.0 T7] specs/features/0.2.0-fcflash.md
#include "FakeBootStream.h"
#include "bootloader/Bootloader.h"
#include <kgc/fcFlash/Errors.h>
#include <QElapsedTimer>
#include <QtTest>
#include <atomic>
using namespace std::chrono_literals;
using kgc::fcFlash::Bootloader;
using kgc::fcFlash::BootloaderStage;
using kgc::fcFlash::FcFlashError;
using kgc::fcFlash::FirmwareImage;
using kgc::fcFlash::make;
using Op = FakeBootStream::Op;
namespace {
QByteArray patterned(int size)
{
    QByteArray bytes(size, Qt::Uninitialized);
    for (int index = 0; index < size; ++index) bytes[index] = static_cast<char>(index * 7 + 1);
    return bytes;
}
FirmwareImage imageOf(const QByteArray& internal, const QByteArray& external = {})
{
    FirmwareImage image;
    image.boardId = 1013;
    image.image = internal;
    image.extfImage = external;
    return image;
}
const Bootloader::ProgressFn kNoProgress = [](double) {};
const Bootloader::StageFn kNoStage = [](BootloaderStage) {};
}
class BootloaderTest : public QObject {
    Q_OBJECT
    std::atomic_bool abort_{false};
    /** sync와 identify를 마친 상태를 만들어요. flashImage와 verify는 그 뒤에 불러요. */
    bool identified(Bootloader& loader)
    {
        return loader.sync().has_value() && loader.identify().has_value();
    }
private slots:
    void init() { abort_ = false; }
    /** T7: sync는 0x21 0x20을 보내고 0x12 0x10을 받으면 성공이에요. */
    void test_T7_sync_bytes()
    {
        FakeBootStream fake;
        Bootloader loader(fake, abort_);
        QVERIFY(loader.sync().has_value());
        QCOMPARE(fake.sent, QByteArray::fromHex("2120"));
    }
    /** T7: 응답이 없으면 세 번 해 보고 BootloaderNotFound로 끝나요. */
    void test_T7_sync_tries_three_times()
    {
        FakeBootStream fake;
        fake.silent = true;
        Bootloader loader(fake, abort_);
        const auto result = loader.sync();
        QVERIFY(!result.has_value());
        QVERIFY(result.error() == make(FcFlashError::BootloaderNotFound));
        QCOMPARE(fake.count(Op::Sync), 3);
    }
    /** T7: identify는 1, 2, 3, 4, 6 순서로 묻고 값을 풀어요. */
    void test_T7_identify_bytes_and_values()
    {
        FakeBootStream fake;
        fake.revision = 5;
        fake.boardRevision = 2;
        fake.flashSize = 0x1E0000;
        fake.extfSize = 0x800000;
        Bootloader loader(fake, abort_);
        const auto info = loader.identify();
        QVERIFY(info.has_value());
        QCOMPARE(fake.sent, QByteArray::fromHex("220120220220220320220420220620"));
        QCOMPARE(info->bootloaderRev, 5);
        QCOMPARE(info->boardId, 1013);
        QCOMPARE(info->boardRev, 2);
        QCOMPARE(info->flashSize, qint64(0x1E0000));
        QCOMPARE(info->extfSize, qint64(0x800000));
    }
    /** T7: 판이 5 미만이면 외부 플래시를 묻지 않고 0으로 둬요. */
    void test_T7_identify_skips_extf_below_revision_5()
    {
        FakeBootStream fake;
        fake.revision = 4;
        fake.extfSize = 0x800000;
        Bootloader loader(fake, abort_);
        const auto info = loader.identify();
        QVERIFY(info.has_value());
        QCOMPARE(fake.deviceArgs, QList<int>({1, 2, 3, 4}));
        QCOMPARE(info->extfSize, qint64(0));
    }
    /** T7: 외부 플래시 질문이 실패해도 identify는 성공하고 크기는 0이에요. */
    void test_T7_identify_extf_failure_means_zero()
    {
        FakeBootStream fake;
        fake.extfInfoFails = true;
        Bootloader loader(fake, abort_);
        const auto info = loader.identify();
        QVERIFY(info.has_value());
        QCOMPARE(info->extfSize, qint64(0));
    }
    /** T7: 판이 3~20 밖이면 거부해요(2와 21). */
    void test_T7_rejects_bootloader_revision()
    {
        for (const quint32 revision : {2U, 21U}) {
            FakeBootStream fake;
            fake.revision = revision;
            Bootloader loader(fake, abort_);
            const auto info = loader.identify();
            QVERIFY(!info.has_value());
            QVERIFY(info.error() == make(FcFlashError::BadBootloaderRevision));
        }
    }
    /** T7: 판 3과 20은 받아들여요. */
    void test_T7_accepts_revision_limits()
    {
        for (const quint32 revision : {3U, 20U}) {
            FakeBootStream fake;
            fake.revision = revision;
            Bootloader loader(fake, abort_);
            QVERIFY(loader.identify().has_value());
        }
    }
    /** T7: 지우기는 0x23 0x20을 보내고, 읽기 조각마다 0~1의 늘어나는 진행률을 내요. */
    void test_T7_erase_bytes_and_progress()
    {
        FakeBootStream fake;
        Bootloader loader(fake, abort_);
        QVERIFY(identified(loader));
        fake.sent.clear();
        fake.holdReads = 5;
        QList<double> values;
        QVERIFY(loader.erase([&](double value) { values << value; }).has_value());
        QVERIFY(fake.sent.contains(QByteArray::fromHex("2320")));
        QCOMPARE(fake.count(Op::Erase), 1);
        QVERIFY(!values.isEmpty());
        for (qsizetype index = 0; index < values.size(); ++index) {
            QVERIFY(values[index] >= 0.0 && values[index] <= 1.0);
            if (index > 0) QVERIFY(values[index] >= values[index - 1]);
        }
    }
    /** T7: 쓰기는 252바이트 묶음이고 묶음은 4의 배수이며, 내용이 그대로 도착해요. */
    void test_T7_program_chunks()
    {
        FakeBootStream fake;
        Bootloader loader(fake, abort_);
        const auto image = patterned(1000);
        QList<double> values;
        QVERIFY(loader.program(image, [&](double value) { values << value; }).has_value());
        QCOMPARE(fake.programSizes, QList<int>({252, 252, 252, 244}));
        QCOMPARE(fake.flash, image);
        QVERIFY(!values.isEmpty());
        QCOMPARE(values.last(), 1.0);
    }
    /** T7: 4의 배수가 아닌 이미지는 0xFF를 덧붙여 보내요. */
    void test_T7_program_pads_with_ff()
    {
        FakeBootStream fake;
        Bootloader loader(fake, abort_);
        const auto image = patterned(253);
        QVERIFY(loader.program(image, kNoProgress).has_value());
        QCOMPARE(fake.programSizes, QList<int>({252, 4}));
        QCOMPARE(fake.flash, image + QByteArray(3, static_cast<char>(0xFF)));
    }
    /** T7: 쓰기 실패는 재시도하지 않아요. */
    void test_T7_program_no_retry()
    {
        FakeBootStream fake;
        fake.failProgramAt = 1;
        Bootloader loader(fake, abort_);
        const auto result = loader.program(patterned(1000), kNoProgress);
        QVERIFY(!result.has_value());
        QVERIFY(result.error() == make(FcFlashError::ProgramFailed));
        QCOMPARE(fake.programSizes.size(), 2);
    }
    /** T7: 검증은 0x29 0x20이고, 이미지 + 0xFF를 플래시 크기까지 셈한 값으로 비교해요. */
    void test_T7_verify_crc_success()
    {
        FakeBootStream fake;
        Bootloader loader(fake, abort_);
        QVERIFY(identified(loader));
        const auto image = patterned(1001);
        QVERIFY(loader.program(image, kNoProgress).has_value());
        fake.sent.clear();
        QVERIFY(loader.verify(image).has_value());
        QVERIFY(fake.sent.contains(QByteArray::fromHex("2920")));
        QCOMPARE(fake.count(Op::Crc), 1);
    }
    /** T7: 값이 다르면 VerifyFailed예요. */
    void test_T7_verify_detects_mismatch()
    {
        FakeBootStream fake;
        fake.corruptCrc = true;
        Bootloader loader(fake, abort_);
        QVERIFY(identified(loader));
        const auto image = patterned(1000);
        QVERIFY(loader.program(image, kNoProgress).has_value());
        const auto result = loader.verify(image);
        QVERIFY(!result.has_value());
        QVERIFY(result.error() == make(FcFlashError::VerifyFailed));
    }
    /** T7: 내부 이미지만 있으면 지우기·쓰기·검증 순서이고, 단계를 차례로 알리며 스스로 부팅하지 않아요. */
    void test_T7_flash_image_internal_order()
    {
        FakeBootStream fake;
        Bootloader loader(fake, abort_);
        QVERIFY(identified(loader));
        const auto image = patterned(1000);
        QList<BootloaderStage> stages;
        QVERIFY(loader.flashImage(imageOf(image), [&](BootloaderStage stage) { stages << stage; }, kNoProgress).has_value());
        QCOMPARE(fake.flashOps(), QList<Op>({Op::Erase, Op::Program, Op::Crc}));
        QCOMPARE(fake.flash, image);
        QCOMPARE(stages, QList<BootloaderStage>({BootloaderStage::Erase, BootloaderStage::Program, BootloaderStage::Verify}));
        QVERIFY(!fake.bootSeen());
    }
    /** T7: 쓰기가 실패하면 검증도 부팅도 하지 않아요. */
    void test_T7_no_boot_after_program_failure()
    {
        FakeBootStream fake;
        fake.failProgramAt = 2;
        Bootloader loader(fake, abort_);
        QVERIFY(identified(loader));
        const auto result = loader.flashImage(imageOf(patterned(1000)), kNoStage, kNoProgress);
        QVERIFY(!result.has_value());
        QVERIFY(result.error() == make(FcFlashError::ProgramFailed));
        QCOMPARE(fake.programSizes.size(), 3);
        QCOMPARE(fake.count(Op::Crc), 0);
        QVERIFY(!fake.bootSeen());
    }
    /** T7: 검증이 실패하면 끝까지 쓰고 CRC를 물은 뒤에도 boot 바이트가 나가지 않아요. */
    void test_T7_no_boot_after_verify_failure()
    {
        FakeBootStream fake;
        fake.corruptCrc = true;
        Bootloader loader(fake, abort_);
        QVERIFY(identified(loader));
        const auto result = loader.flashImage(imageOf(patterned(1000)), kNoStage, kNoProgress);
        QVERIFY(!result.has_value());
        QVERIFY(result.error() == make(FcFlashError::VerifyFailed));
        QCOMPARE(fake.count(Op::Crc), 1);
        QVERIFY(!fake.bootSeen());
        QVERIFY(!fake.sent.contains(QByteArray::fromHex("3020")));
    }
    /** T7: boot는 0x30 0x20을 보내고 응답을 기다리지 않아요. */
    void test_T7_boot_bytes_without_reply()
    {
        FakeBootStream fake;
        Bootloader loader(fake, abort_);
        const auto readsBefore = fake.reads;
        QVERIFY(loader.boot().has_value());
        QCOMPARE(fake.sent, QByteArray::fromHex("3020"));
        QCOMPARE(fake.reads, readsBefore);
    }
    /** T7: 외부 이미지는 내부보다 먼저 지우기·쓰기·검증을 해요. */
    void test_T7_extf_before_internal()
    {
        FakeBootStream fake;
        fake.extfSize = 0x8000;
        Bootloader loader(fake, abort_);
        QVERIFY(identified(loader));
        const auto internal = patterned(1000);
        const auto external = patterned(600);
        QList<BootloaderStage> stages;
        QVERIFY(loader.flashImage(imageOf(internal, external), [&](BootloaderStage stage) { stages << stage; }, kNoProgress).has_value());
        QCOMPARE(fake.flashOps(), QList<Op>({Op::ExtfErase, Op::ExtfProgram, Op::ExtfCrc, Op::Erase, Op::Program, Op::Crc}));
        QCOMPARE(stages.size(), 6);
        QCOMPARE(fake.extfEraseSize, 600U);
        QCOMPARE(fake.extfCrcSize, 600U);
        QCOMPARE(fake.extf, external);
        QCOMPARE(fake.extfProgramSizes, QList<int>({252, 252, 96}));
        QCOMPARE(fake.flash, internal);
    }
    /** T7: 외부 검증이 실패하면 내부를 건드리지 않고 부팅하지 않아요. */
    void test_T7_extf_verify_failure_stops()
    {
        FakeBootStream fake;
        fake.extfSize = 0x8000;
        fake.corruptExtfCrc = true;
        Bootloader loader(fake, abort_);
        QVERIFY(identified(loader));
        const auto result = loader.flashImage(imageOf(patterned(1000), patterned(600)), kNoStage, kNoProgress);
        QVERIFY(!result.has_value());
        QVERIFY(result.error() == make(FcFlashError::VerifyFailed));
        QCOMPARE(fake.count(Op::ExtfCrc), 1);
        QCOMPARE(fake.count(Op::Erase), 0);
        QVERIFY(!fake.bootSeen());
    }
    /** T7: image_size가 0이면 내부 지우기·쓰기·검증을 건너뛰어요. */
    void test_T7_zero_image_size_skips_internal()
    {
        FakeBootStream fake;
        fake.extfSize = 0x8000;
        Bootloader loader(fake, abort_);
        QVERIFY(identified(loader));
        QVERIFY(loader.flashImage(imageOf({}, patterned(600)), kNoStage, kNoProgress).has_value());
        QCOMPARE(fake.flashOps(), QList<Op>({Op::ExtfErase, Op::ExtfProgram, Op::ExtfCrc}));
    }
    /** T7: 외부 이미지가 없으면 외부 명령을 보내지 않아요. */
    void test_T7_no_extf_commands_without_image()
    {
        FakeBootStream fake;
        fake.extfSize = 0x8000;
        Bootloader loader(fake, abort_);
        QVERIFY(identified(loader));
        QVERIFY(loader.flashImage(imageOf(patterned(100)), kNoStage, kNoProgress).has_value());
        QCOMPARE(fake.flashOps(), QList<Op>({Op::Erase, Op::Program, Op::Crc}));
    }
    /** T7: 외부 이미지가 외부 플래시보다 크면 굽기 전에 TooLarge예요. */
    void test_T7_extf_too_large_before_erase()
    {
        FakeBootStream fake;
        fake.extfSize = 100;
        Bootloader loader(fake, abort_);
        QVERIFY(identified(loader));
        const auto result = loader.flashImage(imageOf(patterned(100), patterned(600)), kNoStage, kNoProgress);
        QVERIFY(!result.has_value());
        QVERIFY(result.error() == make(FcFlashError::TooLarge));
        QVERIFY(fake.flashOps().isEmpty());
    }
    /** T7: 지우기 대기 중 중단 플래그가 서면 곧바로 Aborted로 돌아와요. */
    void test_T7_abort_flag_stops_erase()
    {
        FakeBootStream fake;
        Bootloader loader(fake, abort_);
        QVERIFY(identified(loader));
        fake.eraseSilent = true;
        const auto readsBefore = fake.reads;
        fake.onRead = [&] {
            if (fake.reads - readsBefore >= 3) abort_ = true;
        };
        QElapsedTimer clock;
        clock.start();
        const auto result = loader.erase(kNoProgress);
        QVERIFY(!result.has_value());
        QVERIFY(result.error() == make(FcFlashError::Aborted));
        QVERIFY2(clock.elapsed() < 2000, qPrintable(QString::number(clock.elapsed())));
    }
    /** T7: 모든 읽기는 100ms 조각을 넘지 않아요. */
    void test_T7_reads_stay_within_slices()
    {
        FakeBootStream fake;
        Bootloader loader(fake, abort_);
        QVERIFY(identified(loader));
        fake.holdReads = 3;
        QVERIFY(loader.flashImage(imageOf(patterned(100)), kNoStage, kNoProgress).has_value());
        QVERIFY(fake.reads > 0);
        QVERIFY(fake.longestRead <= 100ms);
    }
    /** T7: 쓰던 중 포트를 잃으면 PortLost로 끝나고 부팅하지 않아요. */
    void test_T7_port_loss_during_program()
    {
        FakeBootStream fake;
        Bootloader loader(fake, abort_);
        QVERIFY(identified(loader));
        fake.loseAfterWrites = 2;
        const auto result = loader.flashImage(imageOf(patterned(5000)), kNoStage, kNoProgress);
        QVERIFY(!result.has_value());
        QVERIFY(result.error() == make(FcFlashError::PortLost));
        QVERIFY(!fake.bootSeen());
    }
};
QTEST_GUILESS_MAIN(BootloaderTest)
#include "test_bootloader.moc"
