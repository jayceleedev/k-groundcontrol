// [0.2.0 T1] specs/features/0.2.0-fcflash.md
#include <kgc/app/crc/Crc.h>
#include <QtTest>
using kgc::app::crc::crc32Bootloader;
class BootloaderCrcTest : public QObject {
    Q_OBJECT
private slots:
    /** T1: "123456789"의 값이 부트로더 방식의 0x2DFD2D88이에요. */
    void test_T1_check_value()
    {
        QCOMPARE(crc32Bootloader("123456789"), 0x2DFD2D88u);
    }
    /** T1: 빈 입력은 초기값 0을 그대로 돌려줘요(마지막 XOR이 없어요). */
    void test_T1_empty_is_initial_state()
    {
        QCOMPARE(crc32Bootloader(""), 0u);
    }
    /** T1: 나눠서 이어 계산해도 한 번에 계산한 값과 같아요. */
    void test_T1_continued_matches_whole()
    {
        const QByteArray whole("123456789");
        const auto first = crc32Bootloader(whole.first(4));
        QCOMPARE(crc32Bootloader(whole.sliced(4), first), crc32Bootloader(whole));
        QCOMPARE(crc32Bootloader(whole), 0x2DFD2D88u);
    }
};
QTEST_APPLESS_MAIN(BootloaderCrcTest)
#include "test_bootloader_crc.moc"
