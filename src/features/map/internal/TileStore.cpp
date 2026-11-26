// [0.5.0 5.6.5] specs/features/0.5.0-map.md
#include "TileStore.h"
#include "Constants.h"
#include "Events.h"
#include "TileSource.h"
#include <kgc/app/log/Log.h>
#include <kgc/map/Errors.h>
#include <kgc/map/ModuleId.h>
#include <QFile>
namespace kgc::map {
TileStore::TileStore() = default;
TileStore::~TileStore() = default;
void TileStore::open(const QString& path, qint64 limitBytes)
{
    limitBytes_ = limitBytes;
    const auto connection = QStringLiteral("mapTiles");
    auto opened = kgc::app::store::BlobStore::open(path, connection);
    if (!opened) {
        // 깨진 파일은 한 번 지우고 새로 만들어요. 미리 받은 타일도 함께 사라지고, 폴더는 지우지 않아요.
        for (const auto& file : {path, path + QStringLiteral("-wal"), path + QStringLiteral("-shm")}) QFile::remove(file);
        KGC_LOG_WARN(kCategory, MapEvent::StoreReset, "Map store reset: %1", path);
        opened = kgc::app::store::BlobStore::open(path, connection);
    }
    if (!opened) {
        KGC_LOG_WARN(kCategory, MapEvent::StoreFailed, "Map store failed: %1", tileErrorName(opened.error()));
        emit failed(make(MapError::StoreFailed));
        return;
    }
    store_ = std::move(*opened);
    if (!store_->evictUnpinned(limitBytes_)) emit failed(make(MapError::StoreFailed));
}
void TileStore::setLimit(qint64 limitBytes)
{
    limitBytes_ = limitBytes;
    if (store_ && !store_->evictUnpinned(limitBytes_)) emit failed(make(MapError::StoreFailed));
}
void TileStore::lookup(quint64 requestId, const QString& key)
{
    if (!store_) { emit missing(requestId); return; }
    const auto bytes = store_->get(key);
    if (!bytes) {
        emit missing(requestId);
        emit failed(make(MapError::StoreFailed));
        return;
    }
    if (*bytes) emit found(requestId, **bytes);
    else emit missing(requestId);
}
void TileStore::save(const QString& key, const QByteArray& bytes, bool pinned)
{
    if (!store_) return;
    if (!store_->put(key, bytes, pinned)) emit failed(make(MapError::StoreFailed));
    if (++saves_ % kEvictEverySaves == 0 && !store_->evictUnpinned(limitBytes_)) emit failed(make(MapError::StoreFailed));
}
void TileStore::pin(quint64 requestId, const QString& key)
{
    if (!store_) { emit pinned(requestId, false); return; }
    const auto result = store_->pin(key);
    emit pinned(requestId, result.value_or(false));
    if (!result) emit failed(make(MapError::StoreFailed));
}
void TileStore::size()
{
    if (!store_) { emit failed(make(MapError::StoreFailed)); return; }
    const auto total = store_->totalBytes();
    const auto fixed = store_->pinnedBytes();
    if (!total || !fixed) { emit failed(make(MapError::StoreFailed)); return; }
    emit sized(*total, *fixed);
}
void TileStore::clear()
{
    if (!store_) { emit failed(make(MapError::StoreFailed)); return; }
    if (!store_->clear()) { emit failed(make(MapError::StoreFailed)); return; }
    emit cleared();
}
}
