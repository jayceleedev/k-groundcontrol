// [0.0.3 T4 T5] specs/features/0.0.3-windows.md
#include <kgc/analyze/AppLogService.h>
#include <kgc/app/log/LogFeed.h>
#include "AppLogModel.h"
namespace kgc::analyze {
AppLogService::AppLogService(kgc::app::log::LogFeed& feed, QObject* parent)
    : QObject(parent), model_(new AppLogModel(feed.capacity(), this))
{
    connect(&feed, &kgc::app::log::LogFeed::recordAdded, this, [this](const auto& record) {
        if (record.seq <= last_) return;
        last_ = record.seq;
        model_->append(record);
    });
    const auto [records, last] = feed.recent();
    for (const auto& record : records) model_->append(record);
    last_ = last;
}
QAbstractListModel* AppLogService::model() const { return model_; }
}
