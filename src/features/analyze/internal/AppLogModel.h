// [0.0.3 T4 T5 T6] specs/features/0.0.3-windows.md
#pragma once
#include <kgc/app/log/LogFeed.h>
#include <QAbstractListModel>
namespace kgc::analyze {
// 화면이 닫혀 있어도 같은 상한으로 기록을 보관해요.
class AppLogModel : public QAbstractListModel {
    Q_OBJECT
public:
    AppLogModel(int capacity, QObject* parent);
    void append(const kgc::app::log::LogRecord& record);
    int rowCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
private:
    const int capacity_;
    QList<kgc::app::log::LogRecord> records_;
};
}
