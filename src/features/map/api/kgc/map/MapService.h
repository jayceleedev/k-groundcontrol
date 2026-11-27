// [0.5.0 5.6.2 5.6.5 5.6.8] specs/features/0.5.0-map.md
#pragma once
#include <kgc/app/errors/Result.h>
#include <kgc/app/job/Job.h>
#include <kgc/map/Settings.h>
#include <QGeoCoordinate>
#include <QGeoRectangle>
#include <QList>
#include <QObject>
#include <QString>
#include <memory>
namespace kgc::app::net { class Http; }
namespace kgc::app::i18n { class Translator; }
namespace kgc::vehicle::registry { class VehicleRegistry; }
namespace kgc::map {
class TileSource;
/** 지도 부품이 시작하는 곳(가운데 위도·경도와 확대)이에요.
 * 입력: 없어요.
 * 출력: 위도 lat, 경도 lon(도), 확대 zoom이에요.
 * 실패: 없어요.
 */
struct SavedView { double lat; double lon; double zoom; };
/** 모든 지도 부품이 같이 쓰는 타일 받기·저장·지도 종류·궤적·영역 받기를 맡는 전역 서비스예요.
 * 입력: Http, 기체 등록부, 번역기, 시험용 타일 서버 주소(비면 Google)예요. Composition이 하나 만들어요.
 * 출력: 지도 종류·언어·저장소 크기·궤적·영역 받기 작업과 그 바뀜 시그널이에요. 타일 저장소는 워커 스레드에서 돌아요.
 * 실패: 저장소를 쓰지 못하면 storeFailed를 내고 저장 없이 서버에서만 타일을 받아요.
 */
class MapService : public QObject {
    Q_OBJECT
public:
/** 타일 저장소 스레드·타일 공급·궤적 책(TrailBook)을 만들고 번역기 언어를 공급에 이어요.
 * 입력: http, vehicles, translator, 시험용 타일 서버 주소 tileHost(parseTileHost를 거친 값이나 빈 값), 부모 parent예요.
 * 출력: 저장소 스레드가 cache/tiles/tiles.sqlite를 본 타일 저장 한도로 여는 서비스예요.
 *       TrailBook은 vehicles의 기체 위치를 모으고, 그 pointAdded·cleared를 trailPointAdded·trailsCleared로 넘겨요.
 * 실패: 저장소를 열지 못하면 storeFailed를 내요(열기는 워커에서 나중에 끝나요).
 */
    MapService(kgc::app::net::Http& http, kgc::vehicle::registry::VehicleRegistry& vehicles,
               kgc::app::i18n::Translator& translator, const QString& tileHost, QObject* parent);
/** 저장소 시그널을 끊고 저장소 스레드를 끝낼 때까지 기다려요.
 * 입력: 없어요.
 * 출력: 저장소는 자기 스레드에서 닫혀요.
 * 실패: 없어요.
 */
    ~MapService() override;
/** 지금 지도 종류를 읽어요.
 * 입력: 없어요.
 * 출력: 운영 설정 map.mapType의 값이에요.
 * 실패: 없어요.
 */
    settings::MapType mapType() const;
/** 지도 종류를 바꿔요.
 * 입력: 새 지도 종류 type이에요.
 * 출력: 설정에 쓰고, 값이 바뀌었으면 mapTypeChanged를 내요.
 * 실패: 없어요.
 * 흐름:
 * 1. settings::mapType — 지금 값과 같으면 아무 일도 하지 않아요.
 * 2. settings::setMapType — 운영 설정 map.mapType에 써요.
 * 3. mapTypeChanged — 지도 종류가 바뀌었음을 알려요.
 */
    void setMapType(settings::MapType type);
/** 타일 지명 언어를 읽어요.
 * 입력: 없어요.
 * 출력: "ko" 또는 "en"(타일 공급의 값 그대로)이에요.
 * 실패: 없어요.
 */
    QString language() const;
/** 기체 등록부를 내줘요.
 * 입력: 없어요.
 * 출력: 생성자에 받은 등록부예요.
 * 실패: 없어요.
 */
    kgc::vehicle::registry::VehicleRegistry& vehicles() const;
/** 영역을 받을 때의 타일 수를 세요.
 * 입력: 영역 area와 확대 범위 minZoom~maxZoom이에요.
 * 출력: 확대마다 그 영역을 덮는 타일 수의 합이에요.
 * 실패: 없어요.
 */
    qint64 tileCount(const QGeoRectangle& area, int minZoom, int maxZoom) const;
/** 영역 받기를 시작해요.
 * 입력: 영역 area와 확대 범위 minZoom~maxZoom이에요. 지금 지도 종류와 언어로 받아요.
 * 출력: 새 영역 받기 작업이에요. 시작하면 downloadChanged를 내고, 작업이 끝날 때도 downloadChanged를 내요.
 * 실패: 범위가 틀리면 BadZoomRange, 너무 많으면 TooManyTiles, 저장소 고장이면 StoreFailed, 받는 중이면 DownloadBusy예요.
 * 흐름:
 * 1. planDownload — 확대 범위와 타일 수(100,000개까지)를 확인해요.
 * 2. 저장소 고장(storeBroken)이나 받는 중인 작업이 있으면 멈춰요.
 * 3. 남은 MapDownloadJob을 deleteLater하고, 지금 지도 종류·언어로 새 MapDownloadJob을 만들어요.
 * 4. MapDownloadJob::start — TileSource::fetch(pin)으로 타일을 저장소에 고정하거나 서버에서 받아 TileStore::save로 고정해요.
 * 5. downloadChanged — 화면이 새 작업을 보게 알려요.
 */
    kgc::app::errors::Result<kgc::app::job::Job*> startDownload(const QGeoRectangle& area, int minZoom, int maxZoom);
/** 마지막 영역 받기 작업을 내줘요.
 * 입력: 없어요.
 * 출력: 작업이며, 없으면 nullptr예요. 끝난 작업은 다음 시작이나 dismissDownload까지 남아요.
 * 실패: 없어요.
 */
    kgc::app::job::Job* downloadJob() const;
/** 끝난 영역 받기 작업을 치워요.
 * 입력: 없어요.
 * 출력: 작업을 지우고 downloadChanged를 내요.
 * 실패: 없어요.
 * 흐름:
 * 1. MapDownloadJob::deleteLater — 남은 작업을 지우고 downloadJob이 nullptr를 내게 해요.
 * 2. downloadChanged — 화면이 작업이 없어졌음을 보게 알려요.
 */
    void dismissDownload();
/** 저장소 크기를 물어요.
 * 입력: 없어요.
 * 출력: 저장소 스레드가 답하면 storeSizeReady를 내요.
 * 실패: 저장소를 쓰지 못하면 storeFailed를 내요.
 * 흐름:
 * 1. QMetaObject::invokeMethod — 저장소 스레드에서 TileStore::size를 불러요.
 * 2. TileStore::sized — 받은 두 크기로 storeSizeReady를 내요(실패면 TileStore::failed → storeFailed).
 */
    void requestStoreSize();
/** 저장소의 모든 타일(미리 받은 것 포함)을 지워요.
 * 입력: 없어요.
 * 출력: 다 지우면 storeCleared를 내요.
 * 실패: 저장소를 쓰지 못하면 storeFailed를 내요.
 * 흐름:
 * 1. QMetaObject::invokeMethod — 저장소 스레드에서 TileStore::clear를 불러요.
 * 2. TileStore::cleared — storeCleared를 내요(실패면 TileStore::failed → storeFailed).
 */
    void clearStore();
/** 기체 한 대의 궤적을 읽어요.
 * 입력: 기체 번호 sysid예요.
 * 출력: 오래된 점부터의 궤적이며, 없으면 빈 목록이에요.
 * 실패: 없어요.
 */
    QList<QGeoCoordinate> trailOf(int sysid) const;
/** 모든 기체의 궤적을 지워요.
 * 입력: 없어요.
 * 출력: 궤적을 비우고 trailsCleared를 내요.
 * 실패: 없어요.
 * 흐름:
 * 1. TrailBook::clear — 모든 기체의 궤적을 비워요. 다음 위치부터 다시 모아요.
 * 2. TrailBook::cleared → trailsCleared — 화면이 궤적이 비었음을 보게 알려요.
 */
    void clearTrails();
/** 지도 부품이 시작할 곳을 읽어요.
 * 입력: 없어요.
 * 출력: 가운데 지도를 마지막으로 본 곳이며, 처음이면 설정 기본값(36.5, 127.8, 확대 7)이에요.
 * 실패: 없어요.
 */
    SavedView lastView() const;
/** 가운데 지도를 본 곳을 저장해요.
 * 입력: 본 곳 view예요.
 * 출력: 세 값을 각 설정의 범위로 잘라 운영 설정에 써요.
 * 실패: 유한하지 않은 값은 저장되지 않아요.
 */
    void saveView(SavedView view);
signals:
/** 지도 종류가 바뀌었음을 알려요. 입력: 없어요. 출력: 없어요. 실패: 없어요. */
    void mapTypeChanged();
/** 타일 지명 언어가 바뀌었음을 알려요. 입력: 없어요. 출력: 없어요. 실패: 없어요. */
    void languageChanged();
/** 영역 받기 작업이 생기거나 끝나거나 치워졌음을 알려요. 입력: 없어요. 출력: 없어요. 실패: 없어요. */
    void downloadChanged();
/** 저장소 크기를 알려요.
 * 입력: 모든 타일의 바이트 totalBytes와 미리 받은 타일의 바이트 pinnedBytes예요.
 * 출력: 없어요.
 * 실패: 없어요.
 */
    void storeSizeReady(qint64 totalBytes, qint64 pinnedBytes);
/** 저장소를 다 비웠음을 알려요. 입력: 없어요. 출력: 없어요. 실패: 없어요. */
    void storeCleared();
/** 저장소를 쓰지 못했음을 알려요.
 * 입력: 오류 코드 error(MapError::StoreFailed)예요.
 * 출력: 없어요.
 * 실패: 없어요.
 */
    void storeFailed(kgc::app::errors::ErrorCode error);
/** 기체 궤적에 점이 더해졌음을 알려요.
 * 입력: 기체 번호 sysid와 더한 점 coordinate예요.
 * 출력: 없어요.
 * 실패: 없어요.
 */
    void trailPointAdded(int sysid, const QGeoCoordinate& coordinate);
/** 모든 궤적을 지웠음을 알려요. 입력: 없어요. 출력: 없어요. 실패: 없어요. */
    void trailsCleared();
private:
    // register.cpp가 플러그인에 넘길 타일 공급을 꺼내요.
    friend TileSource& tileSource(MapService& service);
    struct Private;
    std::unique_ptr<Private> d_;
};
}
