// [0.5.1 5.6] specs/features/0.5.1-map-sharp.md
// [0.5.0 5.6.8 6 T22] specs/features/0.5.0-map.md
#include "MapDownloadJob.h"
#include "Constants.h"
#include "Events.h"
#include <kgc/app/job/Job.h>
#include <kgc/app/log/Log.h>
#include <kgc/map/Errors.h>
#include <QCoreApplication>
#include <utility>
namespace kgc::map {
using kgc::app::errors::Result;
using kgc::app::job::Job;
MapDownloadJob::MapDownloadJob(TileSource& source, settings::MapType type, QString language, int ratio, QGeoRectangle area,
                               int minZoom, int maxZoom, qint64 total, QObject* parent)
    : QObject(parent), source_(&source), type_(type), language_(std::move(language)), ratio_(ratio), area_(std::move(area)),
      maxZoom_(maxZoom), total_(total), z_(minZoom),
      job_(new Job(kgc::app::job::OperationKind::MapDownload, this))
{
    range_ = tileRange(area_, z_);
    connect(job_, &Job::cancelRequested, this, &MapDownloadJob::cancel);
}
MapDownloadJob::~MapDownloadJob()
{
    // 끝나기 전에 지워지면 기다리는 표를 거둬 다른 지도가 기다리지 않는 요청은 멈춰요.
    stopped_ = true;
    if (source_) {
        for (const auto ticket : std::as_const(open_)) source_->cancel(ticket);
    }
    open_.clear();
}
Job* MapDownloadJob::job() const { return job_; }
void MapDownloadJob::start()
{
    job_->setStep(QT_TRANSLATE_NOOP("Job", "타일 받는 중"));
    job_->setCancellable(true);
    job_->setProgress(0);
    KGC_LOG_INFO(kCategory, MapEvent::DownloadStarted, "Map download started: %1 tiles", total_);
    fill();
    if (open_.isEmpty()) finish();
}
bool MapDownloadJob::nextTile(int& z, int& x, int& y)
{
    // 작은 확대부터, 그 안에서 행(y) 오름차순, 열 i 순서예요. 열 i의 x는 (x + i) mod 2^z라 날짜 변경선을 넘어도 맞아요.
    while (z_ <= maxZoom_) {
        if (row_ < range_.rows && range_.columns > 0) {
            z = z_;
            x = (range_.x + column_) % (1 << z_);
            y = range_.y + row_;
            if (++column_ >= range_.columns) {
                column_ = 0;
                ++row_;
            }
            return true;
        }
        ++z_;
        row_ = 0;
        column_ = 0;
        if (z_ <= maxZoom_) range_ = tileRange(area_, z_);
    }
    return false;
}
void MapDownloadJob::fill()
{
    int z = 0;
    int x = 0;
    int y = 0;
    while (!stopped_ && source_ && open_.size() < kDownloadParallel && nextTile(z, x, y)) {
        const quint64 serial = ++nextSerial_;
        // 콜백은 언제나 나중에 불리므로, fetch가 돌려준 표를 먼저 적어 둘 수 있어요.
        // 영역 받기는 시작할 때의 배수로만 받아요(다른 배수를 대신 쓰지 않아요).
        const auto ticket = source_->fetch(type_, language_, ratio_, z, x, y, true,
            [self = QPointer<MapDownloadJob>(this), serial](const Result<QByteArray>& result) {
                if (self) self->tileDone(serial, result);
            });
        open_.insert(serial, ticket);
    }
}
void MapDownloadJob::tileDone(quint64 serial, const Result<QByteArray>& result)
{
    if (stopped_ || !open_.remove(serial)) return;
    ++done_;
    // 받지 못한 타일(TileFailed 포함)도 끝난 수에 넣어 진행선이 1에서 끝나요.
    if (!result) ++failed_;
    job_->setProgress(total_ > 0 ? double(done_) / double(total_) : 1.0);
    fill();
    if (open_.isEmpty()) finish();
}
void MapDownloadJob::cancel()
{
    if (stopped_) return;
    // 새 요청을 멈추고 열린 표를 거둬요. 이미 저장한 타일은 남아요.
    stopped_ = true;
    if (source_) {
        for (const auto ticket : std::as_const(open_)) source_->cancel(ticket);
    }
    open_.clear();
    job_->finishCancelled();
    KGC_LOG_INFO(kCategory, MapEvent::DownloadCancelled, "Map download cancelled: %1 of %2 tiles", done_, total_);
}
void MapDownloadJob::finish()
{
    stopped_ = true;
    KGC_LOG_INFO(kCategory, MapEvent::DownloadFinished, "Map download finished: %1 tiles, %2 failed", total_, failed_);
    if (failed_ == 0) {
        job_->succeed();
        return;
    }
    job_->fail(make(MapError::TileFailed),
               QCoreApplication::translate("Map", QT_TRANSLATE_NOOP("Map", "타일 %1개를 받지 못했어요")).arg(failed_));
}
}
