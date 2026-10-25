// [0.0.3 T4 T5 T6] specs/features/0.0.3-windows.md
#include "AppLogModel.h"
namespace kgc::analyze {
AppLogModel::AppLogModel(int capacity, QObject* parent) : QAbstractListModel(parent), capacity_(capacity) {}
void AppLogModel::append(const kgc::app::log::LogRecord& record)
{
    if (records_.size() == capacity_) {
        beginRemoveRows({}, 0, 0);
        records_.removeFirst();
        endRemoveRows();
    }
    const int row = int(records_.size());
    beginInsertRows({}, row, row);
    records_.append(record);
    endInsertRows();
}
int AppLogModel::rowCount(const QModelIndex& parent) const { return parent.isValid() ? 0 : int(records_.size()); }
QVariant AppLogModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= records_.size()) return {};
    const auto& record = records_[index.row()];
    switch (role) {
    case Qt::UserRole + 1: return record.text;
    case Qt::UserRole + 2: return kgc::app::log::logLevelName(record.level);
    case Qt::UserRole + 3: return record.category;
    case Qt::UserRole + 4: return record.event;
    case Qt::UserRole + 5: return record.message;
    case Qt::UserRole + 6: return QVariant::fromValue(record.seq);
    default: return {};
    }
}
QHash<int, QByteArray> AppLogModel::roleNames() const
{
    return {{Qt::UserRole + 1, "text"}, {Qt::UserRole + 2, "level"},
        {Qt::UserRole + 3, "category"}, {Qt::UserRole + 4, "event"},
        {Qt::UserRole + 5, "message"}, {Qt::UserRole + 6, "seq"}};
}
}
