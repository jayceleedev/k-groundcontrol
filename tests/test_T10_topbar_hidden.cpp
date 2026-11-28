// [0.6.0 T10] specs/features/0.6.0-fly.md
#include <kgc/ui/Slots.h>
#include <kgc/ui/Errors.h>
#include <QSignalSpy>
#include <QtTest>
using kgc::ui::TopbarSlot;
namespace {
// Composition이 지금 등록하는 fly 자리 표시 여덟과 mission.select예요.
void registerFlyPlaceholders(TopbarSlot& top)
{
    const auto add = [&top](const char* id, int order, int group, const char* labelKey, const char* icon,
                            const char* shape = "button") {
        QVERIFY(top.registerPlaceholder(id, order, "centre", group, labelKey, icon, false, shape));
    };
    add("fly.vehicle", 1, 1, "기체", "", "display");
    add("fly.mode", 2, 2, "모드: —", "", "dropdown");
    add("fly.arm", 3, 2, "ARM", "");
    add("fly.takeoff", 4, 3, "이륙", "plane-departure");
    add("fly.land", 5, 3, "착륙", "plane-arrival");
    add("fly.rtl", 6, 3, "RTL", "home");
    add("fly.pause", 7, 3, "일시정지", "player-pause");
    add("fly.emergencyStop", 8, 4, "비상 정지", "hand-stop");
    add("mission.select", 9, 5, "미션: —", "", "dropdown");
}
int rowOf(const TopbarSlot& top, const QString& id)
{
    const auto role = top.roleNames().key("id");
    for (int row = 0; row < top.rowCount(); ++row)
        if (top.data(top.index(row), role).toString() == id) return row;
    return -1;
}
bool roleOf(const TopbarSlot& top, const QString& id, const char* name)
{
    return top.data(top.index(rowOf(top, id)), top.roleNames().key(name)).toBool();
}
QStringList separatorsOf(const TopbarSlot& top)
{
    QStringList ids;
    const auto idRole = top.roleNames().key("id");
    const auto separator = top.roleNames().key("separatorBefore");
    for (int row = 0; row < top.rowCount(); ++row)
        if (top.data(top.index(row), separator).toBool()) ids << top.data(top.index(row), idRole).toString();
    return ids;
}
// 0행부터 끝 행까지를 덮는 dataChanged가 있었는지 봐요.
bool coversAllRows(const QSignalSpy& spy, int rows)
{
    for (const auto& arguments : spy) {
        const auto first = arguments.at(0).value<QModelIndex>();
        const auto last = arguments.at(1).value<QModelIndex>();
        if (first.row() == 0 && last.row() == rows - 1) return true;
    }
    return false;
}
}
class TopbarHiddenTest : public QObject {
    Q_OBJECT
private slots:
    /** T10: 구분선은 보이는 항목끼리 세고, 숨은 항목은 hidden 역할이 참이며 구분선이 없어요. */
    void test_T10_hidden_skips_separator()
    {
        TopbarSlot top;
        registerFlyPlaceholders(top);
        QCOMPARE(separatorsOf(top), QStringList({"fly.mode", "fly.takeoff", "fly.emergencyStop", "mission.select"}));
        QVERIFY(!roleOf(top, "fly.takeoff", "hidden"));
        QSignalSpy changed(&top, &TopbarSlot::dataChanged);
        QVERIFY(top.setHidden("fly.takeoff", true));
        QVERIFY(roleOf(top, "fly.takeoff", "hidden"));
        QVERIFY(!roleOf(top, "fly.takeoff", "separatorBefore"));
        QVERIFY(roleOf(top, "fly.land", "separatorBefore"));
        QVERIFY(coversAllRows(changed, top.rowCount()));
        QVERIFY(top.setHidden("fly.land", true));
        QVERIFY(top.setHidden("fly.rtl", true));
        QVERIFY(top.setHidden("fly.pause", true));
        // 앞의 보이는 행은 arm(묶음 2)이라 emergencyStop(묶음 4) 앞 구분선은 그대로예요.
        QVERIFY(roleOf(top, "fly.emergencyStop", "separatorBefore"));
        QCOMPARE(separatorsOf(top), QStringList({"fly.mode", "fly.emergencyStop", "mission.select"}));
        QVERIFY(top.setHidden("fly.takeoff", false));
        QVERIFY(roleOf(top, "fly.takeoff", "separatorBefore"));
        QVERIFY(!roleOf(top, "fly.land", "separatorBefore"));
    }
    /** T10: 숨은 행 바로 앞에 항목이 끼어들면 그 뒤 보이는 행의 앞 구분선이 바뀌고, 그 행까지 덮는 dataChanged가 나와요. */
    void test_T10_insert_before_hidden_row()
    {
        TopbarSlot top;
        registerFlyPlaceholders(top);
        QVERIFY(top.setHidden("fly.takeoff", true));
        QVERIFY(roleOf(top, "fly.land", "separatorBefore"));
        QSignalSpy changed(&top, &TopbarSlot::dataChanged);
        // 묶음 3인 항목이 arm(묶음 2)과 숨은 takeoff 사이에 끼면 land 앞 구분선은 그 항목 앞으로 옮겨 가요.
        QVERIFY(top.registerPlaceholder("fly.extra", 3, "centre", 3, "추가", ""));
        QVERIFY(roleOf(top, "fly.extra", "separatorBefore"));
        QVERIFY(!roleOf(top, "fly.land", "separatorBefore"));
        const int land = rowOf(top, "fly.land");
        bool covered = false;
        for (const auto& arguments : changed) {
            const auto first = arguments.at(0).value<QModelIndex>().row();
            const auto last = arguments.at(1).value<QModelIndex>().row();
            covered = covered || (first <= land && land <= last);
        }
        QVERIFY(covered);
    }
    /** T10: 같은 값을 두 번 쓰면 둘째에는 dataChanged가 없어요. */
    void test_T10_same_value_is_silent()
    {
        TopbarSlot top;
        registerFlyPlaceholders(top);
        QVERIFY(top.setHidden("fly.takeoff", true));
        QSignalSpy changed(&top, &TopbarSlot::dataChanged);
        QVERIFY(top.setHidden("fly.takeoff", true));
        QCOMPARE(changed.count(), 0);
    }
    /** T10: 등록되지 않은 id를 숨기면 UnknownId예요. */
    void test_T10_unknown_id()
    {
        TopbarSlot top;
        registerFlyPlaceholders(top);
        const auto result = top.setHidden("nope", true);
        QVERIFY(!result);
        QCOMPARE(result.error(), kgc::ui::make(kgc::ui::UiError::UnknownId));
        QVERIFY(!kgc::ui::errorText(result.error()).isEmpty());
    }
};
QTEST_GUILESS_MAIN(TopbarHiddenTest)
#include "test_T10_topbar_hidden.moc"
