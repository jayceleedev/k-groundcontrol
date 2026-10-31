// [0.1.0 T7] specs/features/0.1.0-link.md
#include <kgc/ui/Slots.h>
#include <kgc/ui/Errors.h>
#include <QtTest>
class LinkSlotsTest : public QObject {
    Q_OBJECT
private slots:
    /** T7: 실제 기여도 순서·식별자로 정렬하고 주소를 보존해요. */
    void test_T7_registration()
    {
        kgc::ui::TopbarSlot top;
        kgc::ui::StatusChipSlot chips;
        const QUrl component("qrc:/Link.qml");
        QVERIFY(top.registerItem("z", 20, "right", 2, component));
        QVERIFY(top.registerItem("b", 10, "right", 1, component));
        QVERIFY(top.registerItem("a", 10, "right", 1, component));
        QVERIFY(chips.registerItem("z", 20, "left", component));
        QVERIFY(chips.registerItem("b", 10, "left", component));
        QVERIFY(chips.registerItem("a", 10, "left", component));
        const auto duplicateTop = top.registerItem("a", 0, "left", 0, QUrl("qrc:/Other.qml"));
        const auto duplicateChip = chips.registerItem("a", 0, "right", QUrl("qrc:/Other.qml"));
        QVERIFY(!duplicateTop && duplicateTop.error() == kgc::ui::make(kgc::ui::UiError::DuplicateId));
        QVERIFY(!duplicateChip && duplicateChip.error() == kgc::ui::make(kgc::ui::UiError::DuplicateId));
        for (kgc::ui::SlotModel* model : QList<kgc::ui::SlotModel*>{&top, &chips}) {
            QCOMPARE(model->rowCount(), 3);
            const auto roles = model->roleNames();
            for (int row = 0; row < 3; ++row) {
                QCOMPARE(model->data(model->index(row), roles.key("id")).toString(), QStringList({"a", "b", "z"})[row]);
                QCOMPARE(model->data(model->index(row), roles.key("component")).toUrl(), component);
            }
        }
        const auto separator = top.roleNames().key("separatorBefore");
        QVERIFY(!top.data(top.index(1), separator).toBool());
        QVERIFY(top.data(top.index(2), separator).toBool());
    }
    /** T7: 자리 표시와 실제 기여도 같은 식별자를 두 번 쓰지 못해요. */
    void test_T7_placeholder_collision()
    {
        kgc::ui::TopbarSlot top;
        kgc::ui::StatusChipSlot chips;
        QVERIFY(top.registerPlaceholder("a", 1, "right", 1, "name", ""));
        QVERIFY(chips.registerPlaceholder("a", 1, "left", "name"));
        QVERIFY(!top.registerItem("a", 2, "right", 1, QUrl("qrc:/Link.qml")));
        QVERIFY(!chips.registerItem("a", 2, "left", QUrl("qrc:/Link.qml")));
        QVERIFY(top.data(top.index(0), top.roleNames().key("component")).toUrl().isEmpty());
        QVERIFY(chips.data(chips.index(0), chips.roleNames().key("component")).toUrl().isEmpty());
    }
};
QTEST_GUILESS_MAIN(LinkSlotsTest)
#include "test_link_slots.moc"
