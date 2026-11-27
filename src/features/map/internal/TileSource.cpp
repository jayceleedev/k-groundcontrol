// [0.5.1 5.4] specs/features/0.5.1-map-sharp.md
// [0.5.0 5.6.6] specs/features/0.5.0-map.md
#include "TileSource.h"
#include "Constants.h"
#include "Events.h"
#include "TileImage.h"
#include "TileStore.h"
#include "TileUrl.h"
#include <kgc/app/log/Log.h>
#include <kgc/app/net/Errors.h>
#include <kgc/map/Errors.h>
#include <kgc/map/ModuleId.h>
#include <algorithm>
namespace kgc::map {
using kgc::app::errors::ErrorCode;
using kgc::app::errors::Result;
using kgc::app::net::HttpError;
using kgc::app::net::HttpRequest;
namespace {
// 받은 목록을 지역 사본으로 돌며 불러요. 콜백 안에서 fetch·cancel이 다시 불려도 대기표가 흔들리지 않아요.
template<class Waiters>
void deliver(const Waiters& waiters, const Result<QByteArray>& result)
{
    for (const auto& waiter : waiters) waiter.done(result);
}
}
TileSource::TileSource(kgc::app::net::Http& http, TileStore& store, QString host, QObject* parent)
    : QObject(parent), http_(http), store_(store), host_(std::move(host)),
      probeTimeout_(QStringLiteral("mapOnlineProbe"), this)
{
    connect(&store_, &TileStore::found, this, &TileSource::storeFound);
    connect(&store_, &TileStore::missing, this, &TileSource::storeMissing);
    connect(&store_, &TileStore::pinned, this, &TileSource::storePinned);
    connect(&probeTimeout_, &kgc::app::scheduler::Timeout::expired, this, &TileSource::probe);
}
QString TileSource::language() const { return language_; }
void TileSource::setLanguage(const QString& language)
{
    if (language == language_) return;
    language_ = language;
    emit languageChanged();
}
int TileSource::tilePixelRatio() const { return tilePixelRatio_; }
void TileSource::setTilePixelRatio(int ratio)
{
    if (ratio == tilePixelRatio_) return;
    tilePixelRatio_ = ratio;
    emit tilePixelRatioChanged();
}
TileTicket TileSource::fetch(settings::MapType type, const QString& language, int ratio, int z, int x, int y, bool pin, Done done)
{
    const TileTicket ticket = ++nextTicket_;
    const auto key = tileKey(type, language, ratio, z, x, y);
    if (const auto it = pending_.find(key); it != pending_.end()) {
        it->waiters.append({ticket, pin, std::move(done)});
        return ticket;
    }
    Pending entry;
    entry.stage = pin ? Stage::Pin : Stage::Lookup;
    entry.requestId = ++nextRequestId_;
    // 지도 요청은 Qt가 다시 요청하므로 0번, 영역 받기는 한 번 더 보내요. 나중에 낀 사람은 이 값을 따라요.
    entry.retries = pin ? 1 : 0;
    entry.waiters.append({ticket, pin, std::move(done)});
    entry.type = type;
    entry.language = language;
    entry.ratio = ratio;
    entry.z = z;
    entry.x = x;
    entry.y = y;
    const auto id = entry.requestId;
    pending_.insert(key, std::move(entry));
    auto* store = &store_;
    if (pin) QMetaObject::invokeMethod(store, [store, id, key] { store->pin(id, key); });
    else QMetaObject::invokeMethod(store, [store, id, key] { store->lookup(id, key); });
    return ticket;
}
void TileSource::cancel(TileTicket ticket)
{
    if (ticket == 0) return;
    for (auto it = pending_.begin(); it != pending_.end(); ++it) {
        auto& waiters = it->waiters;
        const auto found = std::find_if(waiters.begin(), waiters.end(), [ticket](const Waiter& waiter) { return waiter.ticket == ticket; });
        if (found == waiters.end()) continue;
        waiters.erase(found);
        if (!waiters.isEmpty()) return;
        // 대기를 먼저 지운 뒤 멈춰요. 늦게 오는 Aborted 답이 같은 타일의 새 대기에 섞이지 않아요.
        const QPointer<HttpRequest> request = it->request;
        const bool http = it->stage == Stage::Http;
        pending_.erase(it);
        if (http && request) request->abort();
        return;
    }
}
QHash<QString, TileSource::Pending>::iterator TileSource::storeEntry(quint64 requestId)
{
    return std::find_if(pending_.begin(), pending_.end(), [requestId](const Pending& entry) {
        return entry.stage != Stage::Http && entry.requestId == requestId;
    });
}
void TileSource::storeFound(quint64 requestId, const QByteArray& bytes)
{
    const auto it = storeEntry(requestId);
    if (it == pending_.end()) return;
    const auto key = it.key();
    if (it->stage == Stage::Fallback) {
        // 대신 쓰기 3: 찾는 동안 다시 닿았으면 찾은 바이트는 버리고 지금 배수로 서버에 물어요.
        if (!it->fallbackError && online_) { startHttp(key); return; }
        QList<Waiter> pinners;
        QList<Waiter> others;
        for (const auto& waiter : std::as_const(it->waiters)) (waiter.pin ? pinners : others).append(waiter);
        const auto error = it->fallbackError;
        if (!pinners.isEmpty() && !error) {
            // 영역 받기는 대신 쓰지 않아요. 아직 서버에 묻지 않았으니 그 사람만 남겨 서버에 물어요.
            it->waiters = pinners;
            startHttp(key);
        } else {
            pending_.erase(it);
        }
        // 다른 배수 그림을 찾았으니 다시 닿을 때 판을 올려 지금 배수로 다시 청하게 해요.
        fallbackServed_ = true;
        // 대신 쓴 그림은 저장하거나 고정하지 않아요.
        deliver(others, bytes);
        if (!pinners.isEmpty() && error) deliverFailure(key, pinners, *error);
        return;
    }
    if (it->stage != Stage::Lookup) return;
    const auto waiters = it->waiters;
    pending_.erase(it);
    // 저장소에서 찾은 타일도 영역 받기가 기다리면 고정해요(답은 버려요).
    if (std::any_of(waiters.cbegin(), waiters.cend(), [](const Waiter& waiter) { return waiter.pin; })) {
        auto* store = &store_;
        QMetaObject::invokeMethod(store, [store, key] { store->pin(0, key); });
    }
    deliver(waiters, bytes);
}
void TileSource::storeMissing(quint64 requestId)
{
    const auto it = storeEntry(requestId);
    if (it == pending_.end()) return;
    const auto key = it.key();
    if (it->stage == Stage::Lookup) {
        // 대신 쓰기 1: 오프라인이면 서버에 묻기 전에 다른 배수의 저장된 타일을 찾아요(영역 받기가 끼면 묻지 않음).
        const bool pin = std::any_of(it->waiters.cbegin(), it->waiters.cend(), [](const Waiter& waiter) { return waiter.pin; });
        if (!online_ && !pin) {
            it->fallbackError.reset();
            startFallback(key);
        } else {
            startHttp(key);
        }
        return;
    }
    if (it->stage != Stage::Fallback) return;
    // 대신 쓰기 4: 서버에 아직 묻지 않았으면 묻고, 서버가 이미 실패했으면 그 실패로 끝내요.
    if (!it->fallbackError) { startHttp(key); return; }
    const auto error = *it->fallbackError;
    const auto waiters = it->waiters;
    pending_.erase(it);
    deliverFailure(key, waiters, error);
}
void TileSource::storePinned(quint64 requestId, bool pinned)
{
    const auto it = storeEntry(requestId);
    if (it == pending_.end() || it->stage != Stage::Pin) return;
    const auto key = it.key();
    if (!pinned) { startHttp(key); return; }
    QList<Waiter> pinners;
    QList<Waiter> others;
    for (const auto& waiter : std::as_const(it->waiters)) (waiter.pin ? pinners : others).append(waiter);
    if (others.isEmpty()) {
        pending_.erase(it);
    } else {
        // 고정만 원한 사람은 끝났고, 그림이 필요한 사람을 위해 저장소에서 다시 읽어요.
        it->waiters = others;
        it->stage = Stage::Lookup;
        it->requestId = ++nextRequestId_;
        auto* store = &store_;
        QMetaObject::invokeMethod(store, [store, id = it->requestId, key] { store->lookup(id, key); });
    }
    deliver(pinners, QByteArray{});
}
void TileSource::startHttp(const QString& key)
{
    const auto it = pending_.find(key);
    if (it == pending_.end()) return;
    it->stage = Stage::Http;
    // 머리글은 Http 기본값(User-Agent)만 써요.
    const QUrl url(tileUrl(it->type, it->language, it->ratio, it->z, it->x, it->y, host_));
    auto* request = http_.get(url, {kTileTimeout, it->retries, {}});
    it->request = request;
    connect(request, &HttpRequest::finished, this, [this, key, request](const Result<QByteArray>& result) {
        httpFinished(key, request, result);
    });
}
void TileSource::startFallback(const QString& key)
{
    const auto it = pending_.find(key);
    if (it == pending_.end()) return;
    it->stage = Stage::Fallback;
    it->request.clear();
    it->requestId = ++nextRequestId_;
    // 다른 배수의 열쇠는 이 대기의 값으로 만들어요(지금 공급 배수가 아니에요).
    const auto other = tileKey(it->type, it->language, it->ratio == 1 ? 2 : 1, it->z, it->x, it->y);
    auto* store = &store_;
    QMetaObject::invokeMethod(store, [store, id = it->requestId, other] { store->lookup(id, other); });
}
void TileSource::httpFinished(const QString& key, const HttpRequest* request, const Result<QByteArray>& result)
{
    // 지운 대기나 다른 요청의 답(멈춘 요청의 Aborted 포함)은 버려요.
    const auto it = pending_.find(key);
    if (it == pending_.end() || it->stage != Stage::Http || it->request.data() != request) return;
    if (result && !tileImageFormat(*result).isEmpty()) {
        const Pending entry = *it;
        pending_.erase(it);
        const bool pin = std::any_of(entry.waiters.cbegin(), entry.waiters.cend(), [](const Waiter& waiter) { return waiter.pin; });
        auto* store = &store_;
        QMetaObject::invokeMethod(store, [store, key, bytes = *result, pin] { store->save(key, bytes, pin); });
        deliver(entry.waiters, result);
        reach(true, {});
        return;
    }
    // 200이지만 그림이 아닌 답은 저장하지 않아요. 서버에는 닿은 거예요.
    const ErrorCode error = result ? make(MapError::TileFailed) : result.error();
    // 서버 다시 묻기 주소는 이 대기의 배수로 만들어요.
    const QUrl url(tileUrl(it->type, it->language, it->ratio, it->z, it->x, it->y, host_));
    if (std::all_of(it->waiters.cbegin(), it->waiters.cend(), [](const Waiter& waiter) { return waiter.pin; })) {
        // 영역 받기만 기다리면 대신 쓰지 않고 바로 실패예요.
        const auto waiters = it->waiters;
        pending_.erase(it);
        deliverFailure(key, waiters, error);
    } else {
        // 대신 쓰기 2: 지도가 기다리면 대기를 지우지 않고 다른 배수의 저장된 타일을 찾아요.
        it->fallbackError = error;
        startFallback(key);
    }
    if (result) reach(true, {});
    else if (error == kgc::app::net::make(HttpError::NetworkFailed) || error == kgc::app::net::make(HttpError::TimedOut))
        reach(false, url);
    else if (error == kgc::app::net::make(HttpError::HttpStatus))
        reach(true, {});
}
void TileSource::deliverFailure(const QString& key, const QList<Waiter>& waiters, ErrorCode error)
{
    if (!failedKeys_.contains(key)) {
        failedKeys_.insert(key);
        KGC_LOG_WARN(kCategory, MapEvent::TileFailed, "Tile failed: %1, %2", key, tileErrorName(error));
    }
    deliver(waiters, std::unexpected(error));
}
void TileSource::reach(bool reached, const QUrl& url)
{
    if (!reached) {
        probeUrl_ = url;
        if (!online_) return;
        online_ = false;
        probeTimeout_.start(kOnlineProbeInterval);
        return;
    }
    probeTimeout_.stop();
    if (online_) return;
    online_ = true;
    if (fallbackServed_) {
        // 판 올림이 보이는 칸을 모두 지금 배수로 다시 청하므로 빈 칸 다시 청하기(onlineAgain)는 따로 내지 않아요.
        fallbackServed_ = false;
        emit fallbackStale();
        return;
    }
    emit onlineAgain();
}
void TileSource::probe()
{
    // Qt가 다시 요청을 포기한 뒤에는 이 요청 말고는 서버에 닿았는지 알 길이 없어요. 본문은 저장하지 않아요.
    auto* request = http_.get(probeUrl_, {kTileTimeout, 0, {}});
    connect(request, &HttpRequest::finished, this, [this](const Result<QByteArray>& result) {
        if (!result && result.error() == kgc::app::net::make(HttpError::Aborted)) return;
        if (result || result.error() == kgc::app::net::make(HttpError::HttpStatus)) { reach(true, {}); return; }
        if (!online_) probeTimeout_.start(kOnlineProbeInterval);
    });
}
QString tileErrorName(ErrorCode error)
{
    return QStringLiteral("%1:%2").arg(error.module.value).arg(error.code);
}
}
