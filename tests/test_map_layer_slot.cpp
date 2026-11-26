// [0.5.0 T7] specs/features/0.5.0-map.md
#include <kgc/ui/Slots.h>
#include <kgc/ui/Errors.h>
#include <QtTest>
class MapLayerSlotTest : public QObject {
    Q_OBJECT
private slots:
    /** T7: 등록 순서와 상관없이 order 순서로 줄을 세우고, component 역할에 넣은 주소를 돌려주며, 같은 id는 DuplicateId로 거부해요. */
    void test_T7_register_layer_order_and_duplicate()
    {
        kgc::ui::MapLayerSlot slot;
        const QUrl vehicles("qrc:/qt/qml/Kgc/Map/VehicleLayer.qml");
        const QUrl trails("qrc:/qt/qml/Kgc/Map/TrailLayer.qml");
        const QUrl homes("qrc:/qt/qml/Kgc/Map/HomeLayer.qml");
        QVERIFY(slot.registerLayer("mapVehicles", 30, vehicles).has_value());
        QVERIFY(slot.registerLayer("mapTrails", 10, trails).has_value());
        QVERIFY(slot.registerLayer("mapHomes", 20, homes).has_value());
        QCOMPARE(slot.rowCount(), 3);
        const auto roles = slot.roleNames();
        const QStringList ids{"mapTrails", "mapHomes", "mapVehicles"};
        const QList<QUrl> components{trails, homes, vehicles};
        for (int row = 0; row < 3; ++row) {
            QCOMPARE(slot.data(slot.index(row), roles.key("id")).toString(), ids[row]);
            QCOMPARE(slot.data(slot.index(row), roles.key("component")).toUrl(), components[row]);
        }
        const auto again = slot.registerLayer("mapTrails", 5, homes);
        QVERIFY(!again.has_value());
        QVERIFY(again.error() == kgc::ui::make(kgc::ui::UiError::DuplicateId));
        QCOMPARE(slot.rowCount(), 3);
    }
};
QTEST_APPLESS_MAIN(MapLayerSlotTest)
#include "test_map_layer_slot.moc"
