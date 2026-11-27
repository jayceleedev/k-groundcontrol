// [0.5.0 5.6.9 5.9] specs/features/0.5.0-map.md
#pragma once
#include <kgc/map/MapService.h>
#include <kgc/app/job/Job.h>
#include <QAbstractItemModel>
#include <QGeoCoordinate>
#include <QGeoRectangle>
#include <QMetaObject>
#include <QPointer>
#include <QQmlEngine>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>
namespace kgc::ui { class MapLayerSlot; }
namespace kgc::map {
class VehicleMarkerModel;
/** 지도 서비스의 지도 종류·기체·궤적·지도 설정·저장소·영역 받기를 지도 부품(QML)과 앱 설정 "지도" 칸에 옮겨요.
 * 입력: 전역 지도 서비스와 지도 레이어 슬롯이에요.
 * 출력: 프로퍼티와 요청 함수예요. 판정은 서비스가 맡아요.
 * 실패: 저장소 실패는 storeError, 영역 받기 실패는 downloadError·downloadResultText 글자로 내놓아요.
 */
class MapController : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(int mapType READ mapType WRITE setMapType NOTIFY mapTypeChanged)
    Q_PROPERTY(QStringList mapTypeNames READ mapTypeNames CONSTANT)
    Q_PROPERTY(QString language READ language NOTIFY languageChanged)
    Q_PROPERTY(QAbstractItemModel* vehicles READ vehicles CONSTANT)
    Q_PROPERTY(QAbstractItemModel* layers READ layers CONSTANT)
    Q_PROPERTY(int activeSysid READ activeSysid NOTIFY activeSysidChanged)
    Q_PROPERTY(QGeoCoordinate activeCoordinate READ activeCoordinate NOTIFY activePositionChanged)
    Q_PROPERTY(bool activeHasPosition READ activeHasPosition NOTIFY activePositionChanged)
    Q_PROPERTY(int minZoom READ minZoom CONSTANT)
    Q_PROPERTY(int maxZoom READ maxZoom CONSTANT)
    Q_PROPERTY(int saveViewDelayMs READ saveViewDelayMs CONSTANT)
    Q_PROPERTY(int tileStoreLimitMb READ tileStoreLimitMb WRITE setTileStoreLimitMb NOTIFY tileStoreLimitMbChanged)
    Q_PROPERTY(int tileStoreLimitMbMin READ tileStoreLimitMbMin CONSTANT)
    Q_PROPERTY(int tileStoreLimitMbMax READ tileStoreLimitMbMax CONSTANT)
    Q_PROPERTY(double trailMinMeters READ trailMinMeters WRITE setTrailMinMeters NOTIFY trailMinMetersChanged)
    Q_PROPERTY(double trailMinMetersMin READ trailMinMetersMin CONSTANT)
    Q_PROPERTY(double trailMinMetersMax READ trailMinMetersMax CONSTANT)
    Q_PROPERTY(int trailMaxPoints READ trailMaxPoints WRITE setTrailMaxPoints NOTIFY trailMaxPointsChanged)
    Q_PROPERTY(int trailMaxPointsMin READ trailMaxPointsMin CONSTANT)
    Q_PROPERTY(int trailMaxPointsMax READ trailMaxPointsMax CONSTANT)
    Q_PROPERTY(bool autoFollow READ autoFollow WRITE setAutoFollow NOTIFY autoFollowChanged)
    Q_PROPERTY(kgc::app::job::Job* downloadJob READ downloadJob NOTIFY downloadChanged)
    Q_PROPERTY(bool downloadRunning READ downloadRunning NOTIFY downloadChanged)
    Q_PROPERTY(bool downloadFailed READ downloadFailed NOTIFY downloadChanged)
    Q_PROPERTY(QString downloadResultText READ downloadResultText NOTIFY downloadChanged)
    Q_PROPERTY(QString downloadError READ downloadError NOTIFY downloadChanged)
    Q_PROPERTY(QString storeTotalText READ storeTotalText NOTIFY storeChanged)
    Q_PROPERTY(QString storePinnedText READ storePinnedText NOTIFY storeChanged)
    Q_PROPERTY(QString storeError READ storeError NOTIFY storeChanged)
public:
/** 서비스를 부모로 삼고 서비스·등록부·설정 시그널을 이어요.
 * 입력: 지도 서비스 service와 레이어 슬롯 layers예요.
 * 출력: 서비스가 소유하는 컨트롤러예요. 기체 표식 목록을 함께 만들어요.
 * 실패: 두 번 만드는 것은 프로그래밍 오류예요.
 */
    MapController(MapService& service, kgc::ui::MapLayerSlot& layers);
/** 등록된 객체를 QML 엔진에 제공해요.
 * 입력: QML 엔진과 스크립트 엔진이에요.
 * 출력: C++가 소유하는 싱글턴 포인터예요.
 * 실패: 등록 전 호출이나 다른 스레드의 엔진은 프로그래밍 오류예요.
 */
    static MapController* create(QQmlEngine* engine, QJSEngine* scriptEngine);
    /** 입력: 없어요. 출력: 지도 종류 번호(0 위성, 1 하이브리드, 2 일반)예요. 실패: 없어요. */
    int mapType() const;
/** 지도 종류를 바꿔요.
 * 입력: 지도 종류 번호 value(0·1·2)예요.
 * 출력: 서비스에 넘겨 설정에 쓰고, 바뀌면 mapTypeChanged가 나요.
 * 실패: 범위 밖 번호는 무시해요.
 * 흐름:
 * 1. MapService::setMapType — 운영 설정에 새 지도 종류를 써요.
 * 2. MapService::mapTypeChanged — mapTypeChanged로 넘겨 모든 지도 부품이 엔진 지도 종류를 바꿔요.
 */
    void setMapType(int value);
    /** 입력: 없어요. 출력: 번역 문맥 "Map"의 지도 종류 원문 셋이에요(QML이 qsTranslate로 그려요). 실패: 없어요. */
    QStringList mapTypeNames() const;
    /** 입력: 없어요. 출력: 서비스의 지명 언어("ko"·"en")예요. 실패: 없어요. */
    QString language() const;
    /** 입력: 없어요. 출력: 기체 표식 목록(VehicleMarkerModel)이에요. 실패: 없어요. */
    QAbstractItemModel* vehicles() const;
    /** 입력: 없어요. 출력: 지도 레이어 슬롯이에요(지도 부품이 모든 항목을 얹어요). 실패: 없어요. */
    QAbstractItemModel* layers() const;
    /** 입력: 없어요. 출력: 활성 기체의 sysid이며, 없으면 -1이에요. 실패: 없어요. */
    int activeSysid() const;
    /** 입력: 없어요. 출력: 활성 기체의 위치이며, 없거나 지도 위치가 아니면 무효 좌표예요. 실패: 없어요. */
    QGeoCoordinate activeCoordinate() const;
    /** 입력: 없어요. 출력: 활성 기체가 있고 isMapPosition이 참이면 참이에요. 실패: 없어요. */
    bool activeHasPosition() const;
    /** 입력: 없어요. 출력: 가장 작은 확대(kMinZoom)예요. 실패: 없어요. */
    int minZoom() const;
    /** 입력: 없어요. 출력: 가장 큰 확대(kMaxZoom)예요. 실패: 없어요. */
    int maxZoom() const;
    /** 입력: 없어요. 출력: 지도가 멈춘 뒤 본 곳을 저장하기까지의 시간(kSaveViewDelayMs, ms)이에요. 실패: 없어요. */
    int saveViewDelayMs() const;
    /** 입력: 없어요. 출력: 운영 설정 map.tileStoreLimitMb(MB)예요. 실패: 없어요. */
    int tileStoreLimitMb() const;
    /** 입력: 새 한도 value(MB)예요. 출력: 생성 setter로 써요. 실패: 범위 밖 값은 저장되지 않아요. */
    void setTileStoreLimitMb(int value);
    /** 입력: 없어요. 출력: 본 타일 저장 한도의 가장 작은 값이에요. 실패: 없어요. */
    int tileStoreLimitMbMin() const;
    /** 입력: 없어요. 출력: 본 타일 저장 한도의 가장 큰 값이에요. 실패: 없어요. */
    int tileStoreLimitMbMax() const;
    /** 입력: 없어요. 출력: 운영 설정 map.trailMinMeters(m)예요. 실패: 없어요. */
    double trailMinMeters() const;
    /** 입력: 새 간격 value(m)예요. 출력: 생성 setter로 써요. 실패: 범위 밖 값은 저장되지 않아요. */
    void setTrailMinMeters(double value);
    /** 입력: 없어요. 출력: 궤적 점 간격의 가장 작은 값이에요. 실패: 없어요. */
    double trailMinMetersMin() const;
    /** 입력: 없어요. 출력: 궤적 점 간격의 가장 큰 값이에요. 실패: 없어요. */
    double trailMinMetersMax() const;
    /** 입력: 없어요. 출력: 운영 설정 map.trailMaxPoints예요. 실패: 없어요. */
    int trailMaxPoints() const;
    /** 입력: 새 최대 점 수 value예요. 출력: 생성 setter로 써요. 실패: 범위 밖 값은 저장되지 않아요. */
    void setTrailMaxPoints(int value);
    /** 입력: 없어요. 출력: 궤적 최대 점 수의 가장 작은 값이에요. 실패: 없어요. */
    int trailMaxPointsMin() const;
    /** 입력: 없어요. 출력: 궤적 최대 점 수의 가장 큰 값이에요. 실패: 없어요. */
    int trailMaxPointsMax() const;
    /** 입력: 없어요. 출력: 운영 설정 map.autoFollow예요. 실패: 없어요. */
    bool autoFollow() const;
    /** 입력: 켤지 value예요. 출력: 생성 setter로 써요(지금의 따라가기는 바꾸지 않아요). 실패: 없어요. */
    void setAutoFollow(bool value);
    /** 입력: 없어요. 출력: 마지막 영역 받기 작업이며, 없으면 nullptr예요. 실패: 없어요. */
    kgc::app::job::Job* downloadJob() const;
    /** 입력: 없어요. 출력: 작업이 있고 Pending·Running이면 참이에요. 실패: 없어요. */
    bool downloadRunning() const;
    /** 입력: 없어요. 출력: 작업이 Failed일 때만 참이에요(취소는 거짓). 실패: 없어요. */
    bool downloadFailed() const;
    /** 입력: 없어요. 출력: 끝난 작업의 결과 글자이며, 진행 중이거나 작업이 없으면 빈 값이에요. 실패: 없어요. */
    QString downloadResultText() const;
    /** 입력: 없어요. 출력: 마지막 startDownload 실패 문구이며, 성공·dismissDownload 때 비워요. 실패: 없어요. */
    QString downloadError() const { return downloadError_; }
    /** 입력: 없어요. 출력: 저장한 지도 크기 글자이며, 처음은 빈 값이에요. 실패: 없어요. */
    QString storeTotalText() const { return storeTotalText_; }
    /** 입력: 없어요. 출력: 미리 받은 지도 크기 글자이며, 처음은 빈 값이에요. 실패: 없어요. */
    QString storePinnedText() const { return storePinnedText_; }
    /** 입력: 없어요. 출력: 저장소를 쓰지 못했을 때의 문구이며, 크기를 받으면 빈 값이에요. 실패: 없어요. */
    QString storeError() const { return storeError_; }
/** 지도 부품이 시작할 곳을 읽어요.
 * 입력: 없어요.
 * 출력: {"lat", "lon", "zoom"}이에요.
 * 실패: 없어요.
 */
    Q_INVOKABLE QVariantMap lastView() const;
/** 가운데 지도를 본 곳을 저장해요.
 * 입력: 가운데 위도 lat, 경도 lon, 확대 zoom이에요.
 * 출력: 서비스가 설정 범위로 잘라 써요.
 * 실패: 없어요.
 */
    Q_INVOKABLE void saveView(double lat, double lon, double zoom);
/** 기체 한 대의 궤적을 읽어요(궤적 레이어가 처음 그릴 때 써요).
 * 입력: 기체 번호 sysid예요.
 * 출력: 오래된 점부터의 궤적(좌표 목록)이며, 없으면 빈 목록이에요.
 * 실패: 없어요.
 */
    Q_INVOKABLE QVariantList trailOf(int sysid) const;
/** 모든 기체의 궤적을 지워요.
 * 입력: 없어요.
 * 출력: 서비스가 궤적을 비우면 trailsCleared가 나요.
 * 실패: 없어요.
 * 흐름:
 * 1. MapService::clearTrails — TrailBook::clear로 모든 궤적을 비워요.
 * 2. MapService::trailsCleared — trailsCleared로 넘겨 궤적 레이어가 선을 비워요.
 */
    Q_INVOKABLE void clearTrails();
/** 축척 막대의 길이와 글자를 구해요.
 * 입력: 지도 가운데 위도 latitude, 확대 zoom, 막대의 최대 폭 maxPixels(px)예요.
 * 출력: {"pixels": 막대 폭, "label": "200 m" 꼴 글자}예요.
 * 실패: 없어요.
 */
    Q_INVOKABLE QVariantMap scaleFor(double latitude, double zoom, int maxPixels) const;
/** 영역을 받을 때의 타일 수를 세요.
 * 입력: 영역 area와 확대 범위 minZoom~maxZoom이에요.
 * 출력: 받을 타일 수예요(서비스의 tileCount 그대로).
 * 실패: 없어요.
 */
    Q_INVOKABLE qint64 tileCount(QGeoRectangle area, int minZoom, int maxZoom) const;
/** 받을 타일의 크기를 어림해요.
 * 입력: 타일 수 tiles예요.
 * 출력: 지금 지도 종류의 평균 크기(kAvgTileBytes)로 어림한 크기 글자("28 MB" 꼴)예요.
 *       지도 종류가 바뀌면 값이 달라지므로, QML 바인딩은 mapType도 읽어 다시 구해요.
 * 실패: 없어요.
 */
    Q_INVOKABLE QString estimateText(qint64 tiles) const;
/** 영역 받기 계획이 받을 수 있는지 봐요.
 * 입력: 영역 area와 확대 범위 minZoom~maxZoom이에요.
 * 출력: planDownload 오류(BadZoomRange·TooManyTiles)의 errorText이며, 받을 수 있으면 빈 값이에요.
 * 실패: 없어요.
 */
    Q_INVOKABLE QString planError(QGeoRectangle area, int minZoom, int maxZoom) const;
/** 영역 받기를 시작해요.
 * 입력: 영역 area와 확대 범위 minZoom~maxZoom이에요.
 * 출력: 시작했으면 참이고 downloadError를 비워요.
 * 실패: 거짓이면 downloadError에 errorText(BadZoomRange·TooManyTiles·StoreFailed·DownloadBusy)를 둬요.
 * 흐름:
 * 1. MapService::startDownload — 계획을 확인하고 MapDownloadJob을 만들어 시작해요.
 * 2. MapService::downloadChanged — downloadChanged로 넘겨 카드가 JobView로 진행을 보여요.
 */
    Q_INVOKABLE bool startDownload(QGeoRectangle area, int minZoom, int maxZoom);
/** 끝난 영역 받기 작업을 치워요.
 * 입력: 없어요.
 * 출력: downloadError를 비우고, 서비스가 작업을 지우면 downloadChanged가 나요.
 * 실패: 없어요.
 * 흐름:
 * 1. downloadError를 비워요.
 * 2. MapService::dismissDownload — 남은 MapDownloadJob을 지워요.
 * 3. MapService::downloadChanged — downloadChanged로 넘겨 카드가 작업 없음을 보게 해요.
 */
    Q_INVOKABLE void dismissDownload();
/** 저장소 크기를 다시 물어요.
 * 입력: 없어요.
 * 출력: 답이 오면 storeTotalText·storePinnedText가 바뀌고 storeChanged가 나요.
 * 실패: 저장소를 쓰지 못하면 storeError가 바뀌어요.
 * 흐름:
 * 1. MapService::requestStoreSize — 저장소 스레드에 크기를 물어요.
 * 2. MapService::storeSizeReady — 받은 바이트를 sizeText로 바꿔요.
 */
    Q_INVOKABLE void refreshStoreSize();
/** 저장소를 비워요.
 * 입력: 없어요.
 * 출력: 다 비우면 스스로 refreshStoreSize를 불러요.
 * 실패: 저장소를 쓰지 못하면 storeError가 바뀌어요.
 * 흐름:
 * 1. MapService::clearStore — 저장소 스레드에서 모든 타일을 지워요.
 * 2. MapService::storeCleared — 크기를 다시 물어요.
 */
    Q_INVOKABLE void clearStore();
/** 지도 레이어를 만들지 못했음을 앱 로그에 남겨요.
 * 입력: 레이어 id와 Qt의 오류 글자 error예요.
 * 출력: WARN "Map layer failed: <id>, <error>"예요.
 * 실패: 없어요.
 */
    Q_INVOKABLE void reportLayerFailed(const QString& id, const QString& error);
signals:
    /** 입력: 없어요. 출력: mapType을 다시 읽도록 알려요. 실패: 없어요. */
    void mapTypeChanged();
    /** 입력: 없어요. 출력: language를 다시 읽도록 알려요. 실패: 없어요. */
    void languageChanged();
    /** 입력: 없어요. 출력: activeSysid를 다시 읽도록 알려요(활성 기체가 바뀜). 실패: 없어요. */
    void activeSysidChanged();
    /** 입력: 없어요. 출력: activeCoordinate·activeHasPosition을 다시 읽도록 알려요. 실패: 없어요. */
    void activePositionChanged();
    /** 입력: 없어요. 출력: tileStoreLimitMb를 다시 읽도록 알려요. 실패: 없어요. */
    void tileStoreLimitMbChanged();
    /** 입력: 없어요. 출력: trailMinMeters를 다시 읽도록 알려요. 실패: 없어요. */
    void trailMinMetersChanged();
    /** 입력: 없어요. 출력: trailMaxPoints를 다시 읽도록 알려요(궤적 레이어가 넘는 점을 지워요). 실패: 없어요. */
    void trailMaxPointsChanged();
    /** 입력: 없어요. 출력: autoFollow를 다시 읽도록 알려요. 실패: 없어요. */
    void autoFollowChanged();
    /** 입력: 기체 번호 sysid와 더한 점 coordinate예요. 출력: 궤적 레이어가 그 선에 점을 더해요. 실패: 없어요. */
    void trailPointAdded(int sysid, QGeoCoordinate coordinate);
    /** 입력: 없어요. 출력: 궤적 레이어가 모든 선을 비워요. 실패: 없어요. */
    void trailsCleared();
    /** 입력: 없어요. 출력: 영역 받기 프로퍼티(downloadJob·downloadRunning·…)를 다시 읽도록 알려요. 실패: 없어요. */
    void downloadChanged();
    /** 입력: 없어요. 출력: 저장소 글자(storeTotalText·storePinnedText·storeError)를 다시 읽도록 알려요. 실패: 없어요. */
    void storeChanged();
private:
    void bindActive();
    MapService& service_;
    kgc::ui::MapLayerSlot& layers_;
    VehicleMarkerModel* vehicles_;
    QMetaObject::Connection activePosition_;
    QString downloadError_;
    QString storeTotalText_;
    QString storePinnedText_;
    QString storeError_;
    inline static QPointer<MapController> instance_;
};
}
