// [0.5.0 5.6.7] specs/features/0.5.0-map.md
#include "KgcTileFetcher.h"
#include "TileImage.h"
namespace kgc::map {
KgcTileReply::KgcTileReply(const QGeoTileSpec& spec, TileSource& source, QObject* parent)
    : QGeoTiledMapReply(spec, parent), source_(&source)
{
    // 공급의 콜백은 언제나 나중에 와서, 표를 받은 뒤에 deliver가 불려요. 답이 먼저 지워지면 콜백은 아무것도 하지 않아요.
    ticket_ = source.fetch(settings::MapType(spec.mapId() - 1), source.language(), spec.zoom(), spec.x(), spec.y(), false,
                           [self = QPointer<KgcTileReply>(this)](const kgc::app::errors::Result<QByteArray>& result) {
                               if (self) self->deliver(result);
                           });
}
KgcTileReply::~KgcTileReply()
{
    if (source_ && ticket_) source_->cancel(ticket_);
    ticket_ = 0;
}
void KgcTileReply::abort()
{
    if (source_ && ticket_) source_->cancel(ticket_);
    ticket_ = 0;
    QGeoTiledMapReply::abort();
}
void KgcTileReply::deliver(const kgc::app::errors::Result<QByteArray>& result)
{
    ticket_ = 0;
    if (isFinished()) return;
    if (!result) {
        // Qt가 이 글자를 앱 로그에 남기므로 번역 문구 대신 영어 번호 이름을 넘겨요.
        setError(QGeoTiledMapReply::CommunicationError, tileErrorName(result.error()));
        return;
    }
    setMapImageData(*result);
    setMapImageFormat(tileImageFormat(*result));
    setFinished(true);
}
KgcTileFetcher::KgcTileFetcher(TileSource& source, QGeoMappingManagerEngine* parent)
    : QGeoTileFetcher(parent), source_(&source)
{
}
QGeoTiledMapReply* KgcTileFetcher::getTileImage(const QGeoTileSpec& spec)
{
    // 지도 id는 settings::MapType 순서 + 1(1·2·3)이에요.
    if (!source_ || spec.mapId() < 1 || spec.mapId() > 3)
        return new QGeoTiledMapReply(QGeoTiledMapReply::UnknownError, QStringLiteral("unknown map type"), this);
    return new KgcTileReply(spec, *source_, this);
}
}
