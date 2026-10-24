// [0.0.1 T1] specs/features/0.0.1-shell.md
#include <kgc/ui/Slots.h>
namespace kgc::ui {
TopbarSlot::TopbarSlot(QObject* parent) : QAbstractListModel(parent) {}
int TopbarSlot::rowCount(const QModelIndex&) const { return 0; }
QVariant TopbarSlot::data(const QModelIndex&, int) const { return {}; }
StatusChipSlot::StatusChipSlot(QObject* parent) : QAbstractListModel(parent) {}
int StatusChipSlot::rowCount(const QModelIndex&) const { return 0; }
QVariant StatusChipSlot::data(const QModelIndex&, int) const { return {}; }
RightTabSlot::RightTabSlot(QObject* parent) : QAbstractListModel(parent) {}
int RightTabSlot::rowCount(const QModelIndex&) const { return 0; }
QVariant RightTabSlot::data(const QModelIndex&, int) const { return {}; }
MapLayerSlot::MapLayerSlot(QObject* parent) : QAbstractListModel(parent) {}
int MapLayerSlot::rowCount(const QModelIndex&) const { return 0; }
QVariant MapLayerSlot::data(const QModelIndex&, int) const { return {}; }
}
