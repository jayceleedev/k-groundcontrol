// [0.0.1 T15] specs/features/0.0.1-shell.md
#include <kgc/ui/PanelOrder.h>
#include <kgc/ui/Errors.h>
#include <QtTest>

using namespace kgc::ui;

/** 메뉴 순수 함수가 등록 순서와 묶음을 올바르게 다루는지 확인해요.
 * 입력: 시험용 패널 목록.
 * 출력: 시험 결과.
 * 실패: 메뉴 순서나 구분선이나 중복 판정이 다르면 실패해요.
 */
class PanelOrderTest : public QObject {
    Q_OBJECT
private slots:
    /** T15: 등록 순서가 뒤섞여도 메뉴 순서로 정렬해요. */
    void test_T15_sort_by_order()
    {
        const std::vector<PanelEntry> entries{
            {"appSettings", 15, 4, {}, {}, {}}, {"fcFlash", 1, 1, {}, {}, {}},
            {"esc", 11, 3, {}, {}, {}}, {"logs", 10, 2, {}, {}, {}}};
        const auto sorted = sortByOrder(entries);
        QStringList ids;
        for (const auto& entry : sorted) ids.append(entry.id);
        QCOMPARE(ids, (QStringList{"fcFlash", "logs", "esc", "appSettings"}));
        QCOMPARE(entries.front().id, QString("appSettings"));
        QVERIFY(sortByOrder({}).empty());
    }

    /** T15: 구분선은 펌웨어 세팅 뒤, 로그 뒤, 트래커 뒤에만 생겨요. */
    void test_T15_group_separators()
    {
        const std::vector<PanelEntry> entries{
            {"fcFlash", 1, 1, {}, {}, {}}, {"layout", 2, 2, {}, {}, {}},
            {"sensors", 3, 2, {}, {}, {}}, {"radio", 4, 2, {}, {}, {}},
            {"flightModes", 5, 2, {}, {}, {}}, {"power", 6, 2, {}, {}, {}},
            {"failsafe", 7, 2, {}, {}, {}}, {"motors", 8, 2, {}, {}, {}},
            {"tuning", 9, 2, {}, {}, {}}, {"logs", 10, 2, {}, {}, {}},
            {"esc", 11, 3, {}, {}, {}}, {"elrs", 12, 3, {}, {}, {}},
            {"tracker", 13, 3, {}, {}, {}}, {"params", 14, 4, {}, {}, {}},
            {"appSettings", 15, 4, {}, {}, {}}};
        const std::vector<bool> expected{true, false, false, false, false,
            false, false, false, false, true, false, false, true, false, false};
        QVERIFY(separatorAfter(entries) == expected);
        QVERIFY(separatorAfter({}).empty());
        QVERIFY(separatorAfter({entries.front()}) == std::vector<bool>{false});
    }

    /** T15: 같은 식별자는 중복 오류로 거부하고 새 식별자는 받아요. */
    void test_T15_duplicate_id()
    {
        const std::vector<PanelEntry> entries{{"fcFlash", 1, 1, {}, {}, {}},
            {"logs", 10, 1, {}, {}, {}}};
        const auto result = checkDuplicate(entries, "logs");
        QVERIFY(!result.has_value());
        QVERIFY(result.error() == make(UiError::DuplicateId));
        QVERIFY(checkDuplicate(entries, "params").has_value());
        QVERIFY(checkDuplicate({}, "logs").has_value());
    }
};
QTEST_GUILESS_MAIN(PanelOrderTest)
#include "test_panel_order.moc"
