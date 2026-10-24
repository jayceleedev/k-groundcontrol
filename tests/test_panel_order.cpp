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
            {"appSettings", 15, 3, {}, {}, {}}, {"fcFlash", 1, 1, {}, {}, {}},
            {"esc", 11, 2, {}, {}, {}}, {"logs", 10, 1, {}, {}, {}}};
        const auto sorted = sortByOrder(entries);
        QStringList ids;
        for (const auto& entry : sorted) ids.append(entry.id);
        QCOMPARE(ids, (QStringList{"fcFlash", "logs", "esc", "appSettings"}));
        QCOMPARE(entries.front().id, QString("appSettings"));
        QVERIFY(sortByOrder({}).empty());
    }

    /** T15: 구분선은 로그 뒤와 트래커 뒤에만 생겨요. */
    void test_T15_group_separators()
    {
        const std::vector<PanelEntry> entries{
            {"fcFlash", 1, 1, {}, {}, {}}, {"layout", 2, 1, {}, {}, {}},
            {"sensors", 3, 1, {}, {}, {}}, {"radio", 4, 1, {}, {}, {}},
            {"flightModes", 5, 1, {}, {}, {}}, {"power", 6, 1, {}, {}, {}},
            {"failsafe", 7, 1, {}, {}, {}}, {"motors", 8, 1, {}, {}, {}},
            {"tuning", 9, 1, {}, {}, {}}, {"logs", 10, 1, {}, {}, {}},
            {"esc", 11, 2, {}, {}, {}}, {"elrs", 12, 2, {}, {}, {}},
            {"tracker", 13, 2, {}, {}, {}}, {"params", 14, 3, {}, {}, {}},
            {"appSettings", 15, 3, {}, {}, {}}};
        const std::vector<bool> expected{false, false, false, false, false,
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
