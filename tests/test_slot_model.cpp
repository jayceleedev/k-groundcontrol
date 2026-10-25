// [0.0.2 T21] specs/features/0.0.2-theme.md
#include <kgc/ui/SlotModel.h>
#include <kgc/ui/Slots.h>
#include <type_traits>
#include <kgc/ui/Errors.h>
#include <QtTest>
// T21: 네 기여 목록이 같은 바탕을 써서 동작이 갈라지지 않게 확인해요.
static_assert(std::is_base_of_v<kgc::ui::SlotModel, kgc::ui::TopbarSlot>);
static_assert(std::is_base_of_v<kgc::ui::SlotModel, kgc::ui::StatusChipSlot>);
static_assert(std::is_base_of_v<kgc::ui::SlotModel, kgc::ui::RightTabSlot>);
static_assert(std::is_base_of_v<kgc::ui::SlotModel, kgc::ui::MapLayerSlot>);
// [0.0.3 T2 T15] specs/features/0.0.3-windows.md
class SlotModelTest : public QObject {
    Q_OBJECT
private slots:
    /** T15: 모든 슬롯이 같은 정렬과 중복 거부 규칙을 지키는지 확인해요. */
    void test_T15_registration()
    {
        kgc::ui::TopbarSlot top;
        kgc::ui::StatusChipSlot status;
        kgc::ui::RightTabSlot right;
        kgc::ui::MapLayerSlot map;
        kgc::ui::BottomActionSlot bottom;
        for (kgc::ui::SlotModel* model : QList<kgc::ui::SlotModel*>{&top, &status, &right, &map, &bottom}) {
            QVERIFY(model->add({"z", 20}).has_value());
            QVERIFY(model->add({"b", 10}).has_value());
            QVERIFY(model->add({"a", 10}).has_value());
            const auto duplicate = model->add({"a", 0});
            QVERIFY(!duplicate.has_value());
            QVERIFY(duplicate.error() == kgc::ui::make(kgc::ui::UiError::DuplicateId));
            QCOMPARE(model->rowCount(), 3);
            const auto role = model->roleNames().key("id");
            QCOMPARE(model->data(model->index(0), role).toString(), "a");
            QCOMPARE(model->data(model->index(1), role).toString(), "b");
            QCOMPARE(model->data(model->index(2), role).toString(), "z");
        }
    }
    /** T2: 중복 등록이 기존 단추의 수신 대상을 바꾸지 않는지 확인해요. */
    void test_T2_action_receiver()
    {
        kgc::ui::BottomActionSlot model;
        QSignalSpy spy(this, &SlotModelTest::opened);
        QVERIFY(model.registerAction("a", 10, "logs", "hint", this, "open").has_value());
        QVERIFY(!model.registerAction("a", 20, {}, {}, nullptr, "missing").has_value());
        model.trigger("missing");
        model.trigger("a");
        QCOMPARE(spy.size(), 1);
        QCOMPARE(model.data(model.index(0), model.roleNames().key("icon")).toString(), "logs");
    }
public slots:
    void open() { emit opened(); }
signals:
    void opened();
};
QTEST_GUILESS_MAIN(SlotModelTest)
#include "test_slot_model.moc"

