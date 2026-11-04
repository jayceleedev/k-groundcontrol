// [0.4.1 T3] specs/features/0.4.1-board-match.md
#include "sources/ArduPilotManifest.h"
#include <QtTest>
using kgc::fcFlash::ArduPilotVehicle;
using kgc::fcFlash::ManifestScan;
using kgc::fcFlash::OtherBoard;
using kgc::fcFlash::manifestCandidates;
using kgc::fcFlash::manifestOtherBoards;
using kgc::fcFlash::scanManifest;
namespace {
/** 손으로 만든 ArduPilot 목록 표본을 읽어요. 다른 보드 항목에는 겹친 항목, 두 보드 ID의 KakuteF4,
 *  후보와 같은 이름의 MatekH743(1150), 보드 255 항목, 안쪽 객체에 다른 board_id를 숨긴 항목이 들어 있어요. */
QByteArray sampleManifest()
{
    QFile file(QStringLiteral(KGC_TEST_DATA "/ardupilot-manifest.json"));
    return file.open(QIODevice::ReadOnly) ? file.readAll() : QByteArray();
}
QStringList rows(const QList<OtherBoard>& boards)
{
    QStringList result;
    for (const auto& board : boards) result << QStringLiteral("%1|%2").arg(board.name).arg(board.boardId);
    return result;
}
}
class ManifestOtherBoardsTest : public QObject {
    Q_OBJECT
private slots:
    /** T3: 같은 보드 ID 항목은 bootloader_str을 bootloaderNames로 들고 있어요. */
    void test_T3_bootloader_names_filled()
    {
        const auto candidates = manifestCandidates(scanManifest(sampleManifest(), 1013).entries, ArduPilotVehicle::Copter, "OFFICIAL");
        QCOMPARE(candidates.size(), 2);
        for (const auto& entry : candidates) {
            if (entry.platform == "MatekH743") QCOMPARE(entry.bootloaderNames, QStringList({"MatekH743-BL"}));
            else QCOMPARE(entry.bootloaderNames, QStringList({"MatekH743-BL", "Matek H743 Wing"}));
        }
        const auto heli = manifestCandidates(scanManifest(sampleManifest(), 1013).entries, ArduPilotVehicle::Heli, "OFFICIAL");
        QCOMPARE(heli.size(), 1);
        QVERIFY(heli.first().bootloaderNames.isEmpty());
    }
    /** T3: entries는 0.2.0과 같아요. 다른 보드 항목과 안쪽 객체의 board_id는 섞이지 않아요. */
    void test_T3_entries_unchanged()
    {
        const auto scan = scanManifest(sampleManifest(), 1013);
        QStringList platforms;
        for (const auto& entry : scan.entries) platforms << entry.platform + "|" + entry.buildType;
        platforms.sort();
        QCOMPARE(platforms, QStringList({"MatekH743-bdshot|OFFICIAL", "MatekH743-heli|OFFICIAL", "MatekH743|OFFICIAL",
                                         "MatekH743|OFFICIAL", "MatekH743|OFFICIAL", "MatekH743|OFFICIAL",
                                         "MatekH743|STABLE-4.10.0", "MatekH743|STABLE-4.6.3", "MatekH743|STABLE-4.9.1"}));
        QCOMPARE(scanManifest(sampleManifest(), 140).entries.size(), 1);
        QCOMPARE(scanManifest(sampleManifest(), 255).entries.size(), 1);
    }
    /** T3: 다른 보드 항목은 기체·빌드 종류별로 한 번씩이고, 베타·hex·다섯 기체 밖은 빠져요. */
    void test_T3_boards_once_per_vehicle_and_build()
    {
        const auto scan = scanManifest(sampleManifest(), 1013);
        int kakute = 0;
        for (const auto& board : scan.boards) {
            QVERIFY(board.platform != "Fake");
            QVERIFY(board.boardId != 1013);
            QVERIFY(!board.platform.endsWith("-beta") && !board.platform.endsWith("-hex") && !board.platform.endsWith("-tracker"));
            if (board.platform == "KakuteF4" && board.boardId == 70 && board.vehicle == ArduPilotVehicle::Copter
                && board.buildType == "OFFICIAL") ++kakute;
        }
        QCOMPARE(kakute, 1);
        QCOMPARE(scan.boards.size(), 11);
        QCOMPARE(rows(manifestOtherBoards(scan, ArduPilotVehicle::Heli, "OFFICIAL")), QStringList({"KakuteF4-heli|70"}));
        QCOMPARE(rows(manifestOtherBoards(scan, ArduPilotVehicle::Plane, "OFFICIAL")), QStringList({"KakuteF4|70"}));
        QCOMPARE(rows(manifestOtherBoards(scan, ArduPilotVehicle::Copter, "STABLE-4.6.3")), QStringList({"KakuteF4|70"}));
        QVERIFY(manifestOtherBoards(scan, ArduPilotVehicle::Copter, "BETA").isEmpty());
    }
    /** T3: 한 platform이 두 보드 ID에 있으면 두 행이고, 후보와 이름이 같은 행은 빠지며, 대소문자 무시로 정렬해요. */
    void test_T3_rows_split_filtered_and_sorted()
    {
        const auto others = manifestOtherBoards(scanManifest(sampleManifest(), 1013), ArduPilotVehicle::Copter, "OFFICIAL");
        QCOMPARE(rows(others), QStringList({"CubeOrange|140", "fmuv3|9", "fmuv3-like|255", "KakuteF4|70", "KakuteF4|122",
                                            "matekF405|125", "MatekH7A3|1150"}));
    }
    /** T3: 보드 9와 255는 같은 계열이라 서로 다른 보드로 나오지 않아요. */
    void test_T3_board_9_and_255_are_not_other()
    {
        for (const int boardId : {9, 255}) {
            const auto scan = scanManifest(sampleManifest(), boardId);
            for (const auto& board : scan.boards) QVERIFY(board.boardId != 9 && board.boardId != 255);
            const auto others = rows(manifestOtherBoards(scan, ArduPilotVehicle::Copter, "OFFICIAL"));
            QVERIFY(others.contains("MatekH743|1013"));
            QVERIFY(others.contains("MatekH743|1150"));
            QVERIFY(!others.contains("fmuv3|9") && !others.contains("fmuv3-like|255"));
        }
    }
    /** T3: 깨진 입력은 두 목록 모두 비어요. */
    void test_T3_broken_input_is_empty()
    {
        const ManifestScan scan = scanManifest("not json", 1013);
        QVERIFY(scan.entries.isEmpty() && scan.boards.isEmpty());
        QVERIFY(manifestOtherBoards(scan, ArduPilotVehicle::Copter, "OFFICIAL").isEmpty());
    }
};
QTEST_APPLESS_MAIN(ManifestOtherBoardsTest)
#include "test_T3_manifest_other_boards.moc"
