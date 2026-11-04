// [0.4.1 T4] specs/features/0.4.1-board-match.md
#include "sources/Px4Releases.h"
#include <QtTest>
using kgc::fcFlash::OtherBoard;
using kgc::fcFlash::Px4Release;
using kgc::fcFlash::parsePx4Releases;
using kgc::fcFlash::px4Boards;
using kgc::fcFlash::px4OtherBoards;
using kgc::fcFlash::px4Prefixes;
namespace {
/** 손으로 만든 표본을 읽어요. v1.15.4에는 다른 보드의 파일, bootloader, .bin, 표에 없는 ark 파일을 섞어 두었어요. */
QByteArray sample(const QString& name)
{
    QFile file(QStringLiteral(KGC_TEST_DATA "/") + name);
    return file.open(QIODevice::ReadOnly) ? file.readAll() : QByteArray();
}
Px4Release release(const QString& tag)
{
    for (const auto& value : parsePx4Releases(sample("px4-releases.json")))
        if (value.tag == tag) return value;
    return {};
}
QStringList rows(const QList<OtherBoard>& boards)
{
    QStringList result;
    for (const auto& board : boards) result << QStringLiteral("%1|%2").arg(board.name).arg(board.boardId);
    return result;
}
}
class Px4OtherBoardsTest : public QObject {
    Q_OBJECT
private slots:
    /** T4: px4Boards는 표의 항목 전부를 파일 순서대로 읽고 names를 채워요. 없거나 빈 names는 빈 목록이에요. */
    void test_T4_reads_boards_with_names()
    {
        const auto boards = px4Boards(sample("px4-boards.json"));
        QCOMPARE(boards.size(), 10);
        QCOMPARE(boards.first().boardId, 9);
        QCOMPARE(boards.first().prefix, QString("px4_fmu-v2"));
        QCOMPARE(boards.first().names, QStringList({"PX4 FMU v2.x"}));
        QVERIFY(boards.at(2).names.isEmpty());
        QCOMPARE(boards.at(7).prefix, QString("matek_h743"));
        QCOMPARE(boards.at(7).names, QStringList({"Matek H743-slim", "MatekH743"}));
        QVERIFY(px4Boards("not json").isEmpty());
    }
    /** T4: px4Prefixes의 결과는 0.2.0과 같아요. 255는 9로 봐요. */
    void test_T4_prefixes_unchanged()
    {
        QCOMPARE(px4Prefixes(sample("px4-boards.json"), 1013),
                 QStringList({"hkust_nxt-dual", "hkust_nxt-v1", "matek_h743", "matek_h743-mini", "matek_h743-slim"}));
        QCOMPARE(px4Prefixes(sample("px4-boards.json"), 255), QStringList({"px4_fmu-v2", "px4_fmu-v3"}));
        QVERIFY(px4Prefixes(sample("px4-boards.json"), 4242).isEmpty());
    }
    /** T4: 다른 보드 ID의 파일만 나오고, bootloader·.bin·표에 없는 파일은 빠지며, 이름순이에요. */
    void test_T4_other_board_assets_only()
    {
        const auto boards = px4Boards(sample("px4-boards.json"));
        QCOMPARE(rows(px4OtherBoards(release("v1.15.4"), boards, 1013)),
                 QStringList({"auterion_fmu-v6x · default|53", "cubepilot_cubeorange · default|140",
                              "px4_fmu-v2 · default|9", "px4_fmu-v3 · default|9", "px4_fmu-v6x · default|53"}));
        QCOMPARE(rows(px4OtherBoards(release("v1.17.0"), boards, 1013)), QStringList({"px4_fmu-v6x · default|53"}));
    }
    /** T4: 보드 255인 FC에는 9의 파일이 다른 보드로 나오지 않고, 1013의 파일은 나와요. */
    void test_T4_board_255_keeps_9_as_same()
    {
        const auto others = rows(px4OtherBoards(release("v1.15.4"), px4Boards(sample("px4-boards.json")), 255));
        QVERIFY(!others.contains("px4_fmu-v2 · default|9") && !others.contains("px4_fmu-v3 · default|9"));
        QVERIFY(others.contains("matek_h743 · default|1013"));
    }
    /** T4: 표가 비면 다른 보드도 없어요. */
    void test_T4_empty_boards_give_nothing()
    {
        QVERIFY(px4OtherBoards(release("v1.15.4"), {}, 1013).isEmpty());
    }
};
QTEST_APPLESS_MAIN(Px4OtherBoardsTest)
#include "test_T4_px4_other_boards.moc"
