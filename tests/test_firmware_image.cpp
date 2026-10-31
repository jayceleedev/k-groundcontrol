// [0.2.0 T8] specs/features/0.2.0-fcflash.md
#include "image/FirmwareImage.h"
#include <kgc/fcFlash/Errors.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtTest>
using kgc::fcFlash::FcFlashError;
using kgc::fcFlash::FirmwareImage;
using kgc::fcFlash::FirmwareKind;
using kgc::fcFlash::checkImage;
using kgc::fcFlash::compatible;
using kgc::fcFlash::make;
using kgc::fcFlash::parseFirmwareImage;
using kgc::fcFlash::versionInText;
namespace {
constexpr auto kEmptyStream = "eNoDAAAAAAE=";
/** 파일이 쓰는 모양(zlib 스트림의 base64)으로 묶어요. qCompress의 앞 4바이트는 Qt만의 길이 표시라 떼어내요. */
QString packed(const QByteArray& bytes)
{
    return QString::fromLatin1(qCompress(bytes).sliced(4).toBase64());
}
QByteArray px4Sample(const QByteArray& image, const QJsonObject& changes = {})
{
    QJsonObject sample{{"magic", "PX4FWv1"}, {"board_id", 1013}, {"image", packed(image)},
                       {"image_size", image.size()}, {"summary", "KoaFC"},
                       {"git_identity", "v1.16.2-1.0.0"}, {"version", "0.1"}, {"image_maxsize", 1835008}};
    for (auto it = changes.begin(); it != changes.end(); ++it) sample[it.key()] = it.value();
    return QJsonDocument(sample).toJson();
}
QByteArray apjSample(const QByteArray& image, const QByteArray& external = {})
{
    return QJsonDocument(QJsonObject{{"magic", "APJFWv1"}, {"board_id", 140}, {"image", packed(image)},
                                     {"image_size", image.size()}, {"summary", "CubeOrange"},
                                     {"extf_image", packed(external)}, {"extf_image_size", external.size()},
                                     {"git_identity", "e7fdad94"}, {"USBID", "0x2dae/0x1016"}}).toJson();
}
FirmwareImage sized(int internal, int external = 0)
{
    FirmwareImage image;
    image.boardId = 1013;
    image.image = QByteArray(internal, 'a');
    image.extfImage = QByteArray(external, 'b');
    return image;
}
}
class FirmwareImageTest : public QObject {
    Q_OBJECT
private slots:
    /** T8: PX4 표본을 풀어 종류, 보드, 내용, summary, 버전을 읽어요. */
    void test_T8_parses_px4_sample()
    {
        const QByteArray body("px4 image body");
        const auto image = parseFirmwareImage(px4Sample(body));
        QVERIFY(image.has_value());
        QCOMPARE(image->kind, FirmwareKind::Px4);
        QCOMPARE(image->boardId, 1013);
        QCOMPARE(image->image, body);
        QCOMPARE(image->summary, QString("KoaFC"));
        QCOMPARE(image->expectedVersion, QString("1.16.2"));
    }
    /** T8: PX4 파일에 extf 키가 없으면 외부 이미지는 비어요. */
    void test_T8_px4_without_extf_keys()
    {
        const auto image = parseFirmwareImage(px4Sample("body"));
        QVERIFY(image.has_value());
        QVERIFY(image->extfImage.isEmpty());
    }
    /** T8: ArduPilot 표본을 풀고, 사람이 쓴 버전이 없으니 비교 버전은 비어요. */
    void test_T8_parses_apj_sample_without_version()
    {
        const QByteArray body("apj image body");
        const auto image = parseFirmwareImage(apjSample(body));
        QVERIFY(image.has_value());
        QCOMPARE(image->kind, FirmwareKind::ArduPilot);
        QCOMPARE(image->boardId, 140);
        QCOMPARE(image->image, body);
        QCOMPARE(image->summary, QString("CubeOrange"));
        QVERIFY(image->expectedVersion.isEmpty());
    }
    /** T8: 외부 이미지가 있으면 함께 풀어요. */
    void test_T8_parses_extf_image()
    {
        const QByteArray external("external flash body");
        const auto image = parseFirmwareImage(apjSample("internal", external));
        QVERIFY(image.has_value());
        QCOMPARE(image->extfImage, external);
    }
    /** T8: 내부가 0바이트여도(SPRacingH7 꼴) 해석해요. */
    void test_T8_parses_zero_internal_image()
    {
        const QByteArray external("external only");
        const auto image = parseFirmwareImage(apjSample({}, external));
        QVERIFY(image.has_value());
        QVERIFY(image->image.isEmpty());
        QCOMPARE(image->extfImage, external);
    }
    /** T8: .apj마다 있는 빈 스트림 eNoDAAAAAAE=는 크기 0으로 받아들여요. */
    void test_T8_accepts_literal_empty_stream()
    {
        QJsonObject sample = QJsonDocument::fromJson(apjSample("internal")).object();
        sample["extf_image"] = kEmptyStream;
        sample["extf_image_size"] = 0;
        const auto image = parseFirmwareImage(QJsonDocument(sample).toJson());
        QVERIFY(image.has_value());
        QVERIFY(image->extfImage.isEmpty());
    }
    /** T8: JSON이 아니거나 magic을 모르면 BadImage예요. */
    void test_T8_rejects_unknown_magic()
    {
        for (const auto& bad : {QByteArray("not json"), QByteArray("[]"), px4Sample("body", {{"magic", "HEXFWv1"}})}) {
            const auto image = parseFirmwareImage(bad);
            QVERIFY(!image.has_value());
            QVERIFY(image.error() == make(FcFlashError::BadImage));
        }
    }
    /** T8: 풀린 길이가 image_size와 다르면 BadImage예요. */
    void test_T8_rejects_length_mismatch()
    {
        const auto image = parseFirmwareImage(px4Sample("body", {{"image_size", 5}}));
        QVERIFY(!image.has_value());
        QVERIFY(image.error() == make(FcFlashError::BadImage));
    }
    /** T8: 풀린 길이가 extf_image_size와 다르면 BadImage예요. */
    void test_T8_rejects_extf_length_mismatch()
    {
        QJsonObject sample = QJsonDocument::fromJson(apjSample("internal", "external")).object();
        sample["extf_image_size"] = 3;
        const auto image = parseFirmwareImage(QJsonDocument(sample).toJson());
        QVERIFY(!image.has_value());
        QVERIFY(image.error() == make(FcFlashError::BadImage));
    }
    /** T8: 깨진 base64와 깨진 zlib 스트림은 BadImage예요. */
    void test_T8_rejects_broken_encoding()
    {
        for (const QString& text : {QString("@@@ not base64 @@@"), QString("AAAAAAAA")}) {
            const auto image = parseFirmwareImage(px4Sample("body", {{"image", text}}));
            QVERIFY(!image.has_value());
            QVERIFY(image.error() == make(FcFlashError::BadImage));
        }
    }
    /** T8: 버전은 git_identity에서 첫 x.y.z를 뽑아요. */
    void test_T8_extracts_version_from_identity()
    {
        QCOMPARE(versionInText("v1.16.2-1.0.0"), QString("1.16.2"));
        QCOMPARE(versionInText("v1.18.0-beta1-325-ge8ea98b404"), QString("1.18.0"));
        QCOMPARE(versionInText("e7fdad94e0"), QString());
        const auto image = parseFirmwareImage(px4Sample("body", {{"git_identity", "v1.15.4-rc2-7-gabc"}}));
        QVERIFY(image.has_value());
        QCOMPARE(image->expectedVersion, QString("1.15.4"));
    }
    /** T8: 보드가 같으면 호환이고, 9와 255는 서로 호환이며, 그 밖은 아니에요. */
    void test_T8_compatible_table()
    {
        QVERIFY(compatible(1013, 1013));
        QVERIFY(compatible(9, 9));
        QVERIFY(compatible(255, 255));
        QVERIFY(compatible(255, 9));
        QVERIFY(compatible(9, 255));
        QVERIFY(!compatible(1013, 54));
        QVERIFY(!compatible(9, 10));
        QVERIFY(!compatible(255, 1013));
    }
    /** T8: 보드가 다르면 WrongBoard이고, 크기가 맞으면 통과해요. */
    void test_T8_check_image_board()
    {
        QVERIFY(checkImage(sized(100), 1013, 1000, 0).has_value());
        const auto wrong = checkImage(sized(100), 54, 1000, 0);
        QVERIFY(!wrong.has_value());
        QVERIFY(wrong.error() == make(FcFlashError::WrongBoard));
    }
    /** T8: image_size가 플래시 크기를 넘으면 TooLarge이고, 같으면 통과해요. */
    void test_T8_check_image_too_large()
    {
        QVERIFY(checkImage(sized(1000), 1013, 1000, 0).has_value());
        const auto large = checkImage(sized(1001), 1013, 1000, 0);
        QVERIFY(!large.has_value());
        QVERIFY(large.error() == make(FcFlashError::TooLarge));
    }
    /** T8: 외부 이미지가 외부 플래시를 넘으면 TooLarge이고, 같으면 통과해요. */
    void test_T8_check_image_extf_too_large()
    {
        QVERIFY(checkImage(sized(10, 500), 1013, 1000, 500).has_value());
        const auto large = checkImage(sized(10, 501), 1013, 1000, 500);
        QVERIFY(!large.has_value());
        QVERIFY(large.error() == make(FcFlashError::TooLarge));
    }
};
QTEST_APPLESS_MAIN(FirmwareImageTest)
#include "test_firmware_image.moc"
