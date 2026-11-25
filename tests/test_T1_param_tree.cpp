// [0.4.2 T1 T2 T3 T4 T5] specs/features/0.4.2-param-tree.md
#include "ParamTree.h"
#include <QFile>
#include <QtTest>
#include <algorithm>
using kgc::params::ParamNode;
using kgc::params::buildParamTree;
using kgc::params::paramNodeText;
using kgc::params::underParamNode;
using kgc::params::visibleParamNodes;
namespace {
// 스펙 §5.2 표처럼 들여쓰기(깊이마다 두 칸)와 가지의 개수로 한 줄씩 적어요. 잎의 개수는 따로 1인지 봐요.
QStringList outline(const QList<ParamNode>& tree)
{
    QStringList lines;
    for (const auto& node : tree) {
        auto line = QString(node.depth * 2, u' ') + node.key;
        if (node.branch) line += QStringLiteral(" (%1)").arg(node.count);
        lines.append(line);
    }
    return lines;
}
// 마디의 모양이 key와 어긋나지 않는지 봐요: 가지는 "_*"로 끝나고, 잎은 개수가 1이에요.
bool consistent(const QList<ParamNode>& tree)
{
    return std::all_of(tree.cbegin(), tree.cend(), [](const ParamNode& node) {
        return node.branch == node.key.endsWith(QStringLiteral("_*")) && (node.branch || node.count == 1);
    });
}
QStringList keys(const QList<ParamNode>& tree)
{
    QStringList result;
    for (const auto& node : tree) result.append(node.key);
    return result;
}
const ParamNode* find(const QList<ParamNode>& tree, const QString& key)
{
    const auto found = std::find_if(tree.cbegin(), tree.cend(), [&](const ParamNode& node) { return node.key == key; });
    return found == tree.cend() ? nullptr : &*found;
}
// 이름 파일을 읽어요. Windows에서 줄 끝이 CRLF여도 같도록 글자 모드로 읽고 앞뒤 공백을 떼요.
QStringList readNames(const QString& file)
{
    QFile input(QStringLiteral(KGC_TEST_DATA "/") + file);
    QStringList names;
    if (!input.open(QIODevice::ReadOnly | QIODevice::Text)) return names;
    while (!input.atEnd()) {
        const auto line = QString::fromUtf8(input.readLine()).trimmed();
        if (!line.isEmpty()) names.append(line);
    }
    return names;
}
const QStringList kSample{"ACRO_BAL_PITCH", "ACRO_BAL_ROLL", "ACRO_OPTIONS", "AHRS_COMP_BETA", "AHRS_EKF_TYPE",
    "BATT2_CAPACITY", "BATT2_MONITOR", "BATT_CAPACITY", "BATT_MONITOR", "FORMAT_VERSION", "SERVO1_MAX", "SERVO1_MIN",
    "SERVO2_MAX", "SERVO2_MIN", "SERVO_RATE", "SR0_EXT_STAT", "SR0_POSITION", "SR1_EXT_STAT", "WPNAV_SPEED",
    "WPNAV_SPEED_DN", "WPNAV_SPEED_UP"};
// 실제 목록의 크기를 한 번에 재요.
struct TreeStats {
    int nodes = 0, first = 0, firstBranches = 0, firstLeaves = 0, branches = 0, deepestLeaf = 0;
};
TreeStats measure(const QList<ParamNode>& tree)
{
    TreeStats stats;
    stats.nodes = tree.size();
    for (const auto& node : tree) {
        stats.branches += node.branch;
        if (node.depth == 0) {
            ++stats.first;
            (node.branch ? stats.firstBranches : stats.firstLeaves) += 1;
        }
        if (!node.branch) stats.deepestLeaf = std::max(stats.deepestLeaf, node.depth);
    }
    return stats;
}
// 모든 마디의 count가 underParamNode가 참인 이름 수와 같은지 봐요. 다르면 그 key를 돌려줘요.
QString countMismatch(const QList<ParamNode>& tree, const QStringList& names)
{
    for (const auto& node : tree) {
        const auto matched = std::count_if(names.cbegin(), names.cend(),
                                           [&](const QString& name) { return underParamNode(name, node.key); });
        if (matched != node.count) return node.key;
    }
    return {};
}
}
class ParamTreeTest : public QObject {
    Q_OBJECT
private slots:
    /** T1: 스펙 §5.2의 21개 이름은 그 표와 key·branch·depth·count·순서가 모두 같아요. */
    void test_T1_sample_tree()
    {
        const auto tree = buildParamTree(kSample);
        const QStringList expected{
            "ACRO_* (3)", "  ACRO_BAL_* (2)", "    ACRO_BAL_PITCH", "    ACRO_BAL_ROLL", "  ACRO_OPTIONS",
            "AHRS_* (2)", "  AHRS_COMP_BETA", "  AHRS_EKF_TYPE",
            "BATT2_* (2)", "  BATT2_CAPACITY", "  BATT2_MONITOR",
            "BATT_* (2)", "  BATT_CAPACITY", "  BATT_MONITOR",
            "FORMAT_VERSION",
            "SERVO1_* (2)", "  SERVO1_MAX", "  SERVO1_MIN",
            "SERVO2_* (2)", "  SERVO2_MAX", "  SERVO2_MIN",
            "SERVO_RATE",
            "SR0_* (2)", "  SR0_EXT_STAT", "  SR0_POSITION",
            "SR1_EXT_STAT",
            "WPNAV_* (3)", "  WPNAV_SPEED", "  WPNAV_SPEED_* (3)", "    WPNAV_SPEED_DN", "    WPNAV_SPEED_UP"};
        QCOMPARE(outline(tree), expected);
        QVERIFY(consistent(tree));
    }
    /** T2: 빈 목록, "_" 없는 이름, 한 개짜리 접두어, 숫자 인스턴스, 자식 하나 단계, 빈 조각, 이름 = 접두어의 경계예요. */
    void test_T2_edges()
    {
        QVERIFY(buildParamTree({}).isEmpty());
        QCOMPARE(outline(buildParamTree({"FORMAT"})), QStringList({"FORMAT"}));
        QCOMPARE(outline(buildParamTree({"X_A"})), QStringList({"X_A"}));
        QCOMPARE(outline(buildParamTree({"BATT2_A", "BATT2_B", "BATT_A", "BATT_B"})),
                 QStringList({"BATT2_* (2)", "  BATT2_A", "  BATT2_B", "BATT_* (2)", "  BATT_A", "  BATT_B"}));
        QCOMPARE(outline(buildParamTree({"X_A_B", "X_A_C"})),
                 QStringList({"X_* (2)", "  X_A_* (2)", "    X_A_B", "    X_A_C"}));
        QCOMPARE(outline(buildParamTree({"A__B", "A__C"})), QStringList({"A_* (2)", "  A__B", "  A__C"}));
        const auto tune = buildParamTree({"TUNE", "TUNE_MAX", "TUNE_MIN"});
        QCOMPARE(outline(tune), QStringList({"TUNE", "TUNE_* (3)", "  TUNE_MAX", "  TUNE_MIN"}));
        QVERIFY(consistent(tune));
    }
    /** T3: underParamNode는 "*"면 모두, 가지면 접두어 + "_"와 접두어와 같은 이름, 잎이면 그 이름만 참이고 대소문자를 가려요. */
    void test_T3_under_node()
    {
        for (const char* name : {"BATT", "BATT_MONITOR", "BATT2_MONITOR", "x", "_HASH"})
            QVERIFY(underParamNode(QString::fromLatin1(name), "*"));
        QVERIFY(underParamNode("BATT", "BATT_*"));
        QVERIFY(underParamNode("BATT_MONITOR", "BATT_*"));
        QVERIFY(!underParamNode("BATT2_MONITOR", "BATT_*"));
        QVERIFY(!underParamNode("BATTERY", "BATT_*"));
        QVERIFY(underParamNode("WPNAV_SPEED", "WPNAV_SPEED"));
        QVERIFY(!underParamNode("WPNAV_SPEED_DN", "WPNAV_SPEED"));
        QVERIFY(!underParamNode("WPNAV_SPEE", "WPNAV_SPEED"));
        QVERIFY(!underParamNode("batt_monitor", "BATT_*"));
        QVERIFY(!underParamNode("batt", "BATT_*"));
        QVERIFY(!underParamNode("wpnav_speed", "WPNAV_SPEED"));
        QCOMPARE(paramNodeText("ATC_RAT_*"), QString("ATC_RAT"));
        QCOMPARE(paramNodeText("WPNAV_SPEED"), QString("WPNAV_SPEED"));
        QCOMPARE(paramNodeText("*"), QString("*"));
    }
    /** T4: 펼친 가지가 없으면 첫 단계만, 펼친 가지는 바로 아래까지, 조상이 접혀 있으면 안 보이고, 없는 key는 무시해요. */
    void test_T4_visible_nodes()
    {
        const auto tree = buildParamTree(kSample);
        const QStringList first{"ACRO_*", "AHRS_*", "BATT2_*", "BATT_*", "FORMAT_VERSION", "SERVO1_*", "SERVO2_*",
                                "SERVO_RATE", "SR0_*", "SR1_EXT_STAT", "WPNAV_*"};
        QCOMPARE(keys(visibleParamNodes(tree, {})), first);
        auto acro = first;
        acro.insert(1, "ACRO_BAL_*");
        acro.insert(2, "ACRO_OPTIONS");
        QCOMPARE(keys(visibleParamNodes(tree, {"ACRO_*"})), acro);
        QCOMPARE(keys(visibleParamNodes(tree, {"ACRO_BAL_*"})), first);
        QCOMPARE(keys(visibleParamNodes(tree, {"ACRO_*", "NOPE_*", "FORMAT_VERSION"})), acro);
        auto deep = acro;
        deep.insert(2, "ACRO_BAL_PITCH");
        deep.insert(3, "ACRO_BAL_ROLL");
        QCOMPARE(keys(visibleParamNodes(tree, {"ACRO_*", "ACRO_BAL_*"})), deep);
        for (const auto& node : visibleParamNodes(tree, {"ACRO_*"}))
            QCOMPARE(node.depth, node.key.startsWith("ACRO_") && node.key != "ACRO_*" ? 1 : 0);
    }
    /** T5: ArduCopter 4.7.2 1369개는 마디 1693, 첫 단계 203(가지 105, 잎 98), 가지 324, 가장 깊은 잎 3이고 개수가 맞아요. */
    void test_T5_copter_list()
    {
        const auto names = readNames(QStringLiteral("copter-4.7.2-names.txt"));
        QCOMPARE(names.size(), 1369);
        const auto tree = buildParamTree(names);
        const auto stats = measure(tree);
        QCOMPARE(stats.nodes, 1693);
        QCOMPARE(stats.first, 203);
        QCOMPARE(stats.firstBranches, 105);
        QCOMPARE(stats.firstLeaves, 98);
        QCOMPARE(stats.branches, 324);
        QCOMPARE(stats.deepestLeaf, 3);
        const QList<QPair<QString, int>> counts{{"SIM_*", 355}, {"ATC_*", 61}, {"ATC_RAT_*", 39}, {"ATC_RAT_RLL_*", 13},
            {"EK3_*", 83}, {"EK3_SRC1_*", 5}, {"SERVO5_*", 5}, {"SERVO_*", 14}, {"BATT_*", 20}, {"TUNE_*", 3}};
        for (const auto& [key, count] : counts) {
            const auto* node = find(tree, key);
            QVERIFY2(node, qPrintable(key));
            QCOMPARE(node->count, count);
        }
        for (const char* key : {"BATT2_MONITOR", "FLTMODE1"}) {
            const auto* node = find(tree, QString::fromLatin1(key));
            QVERIFY2(node && !node->branch && node->depth == 0, key);
        }
        QVERIFY(consistent(tree));
        QCOMPARE(countMismatch(tree, names), QString());
    }
    /** T5: PX4 1.16 944개는 마디 1153, 첫 단계 68(가지 57, 잎 11), 가지 209, 가장 깊은 잎 4이고 개수가 맞아요. */
    void test_T5_px4_list()
    {
        const auto names = readNames(QStringLiteral("px4-1.16-names.txt"));
        QCOMPARE(names.size(), 944);
        const auto tree = buildParamTree(names);
        const auto stats = measure(tree);
        QCOMPARE(stats.nodes, 1153);
        QCOMPARE(stats.first, 68);
        QCOMPARE(stats.firstBranches, 57);
        QCOMPARE(stats.firstLeaves, 11);
        QCOMPARE(stats.branches, 209);
        QCOMPARE(stats.deepestLeaf, 4);
        for (const auto& [key, count] : QList<QPair<QString, int>>{{"EKF2_*", 141}, {"CA_*", 121}}) {
            const auto* node = find(tree, key);
            QVERIFY2(node, qPrintable(key));
            QCOMPARE(node->count, count);
        }
        QVERIFY(consistent(tree));
        QCOMPARE(countMismatch(tree, names), QString());
    }
};
QTEST_APPLESS_MAIN(ParamTreeTest)
#include "test_T1_param_tree.moc"
