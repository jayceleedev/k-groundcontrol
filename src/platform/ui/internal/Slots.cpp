// [0.0.1 T1] specs/features/0.0.1-shell.md
#include <kgc/ui/Slots.h>
namespace kgc::ui {
TopbarSlot::TopbarSlot(QObject* parent) : SlotModel(parent) {}
StatusChipSlot::StatusChipSlot(QObject* parent) : SlotModel(parent) {}
RightTabSlot::RightTabSlot(QObject* parent) : SlotModel(parent) {}
MapLayerSlot::MapLayerSlot(QObject* parent) : SlotModel(parent) {}
}
