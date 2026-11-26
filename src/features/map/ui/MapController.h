// [0.5.0 5.6.9] specs/features/0.5.0-map.md
#pragma once
#include <kgc/map/MapService.h>
#include <QAbstractItemModel>
#include <QPointer>
#include <QQmlEngine>
#include <QStringList>
#include <QVariantMap>
namespace kgc::ui { class MapLayerSlot; }
namespace kgc::map {
/** 지도 서비스의 지도 종류·언어·확대 범위·축척·저장소 크기를 지도 부품(QML)에 옮겨요.
 * 입력: 전역 지도 서비스와 지도 레이어 슬롯이에요.
 * 출력: 프로퍼티와 요청 함수예요. 판정은 서비스가 맡아요.
 * 실패: 저장소 실패는 storeError 글자로 내놓아요.
 */
class MapController : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(int mapType READ mapType WRITE setMapType NOTIFY mapTypeChanged)
    Q_PROPERTY(QStringList mapTypeNames READ mapTypeNames CONSTANT)
    Q_PROPERTY(QString language READ language NOTIFY languageChanged)
    Q_PROPERTY(QAbstractItemModel* layers READ layers CONSTANT)
    Q_PROPERTY(int minZoom READ minZoom CONSTANT)
    Q_PROPERTY(int maxZoom READ maxZoom CONSTANT)
    Q_PROPERTY(QString storeTotalText READ storeTotalText NOTIFY storeChanged)
    Q_PROPERTY(QString storePinnedText READ storePinnedText NOTIFY storeChanged)
    Q_PROPERTY(QString storeError READ storeError NOTIFY storeChanged)
public:
/** 서비스를 부모로 삼고 서비스 시그널을 이어요.
 * 입력: 지도 서비스 service와 레이어 슬롯 layers예요.
 * 출력: 서비스가 소유하는 컨트롤러예요.
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
    /** 입력: 없어요. 출력: 지도 레이어 슬롯이에요(지도 부품이 모든 항목을 얹어요). 실패: 없어요. */
    QAbstractItemModel* layers() const;
    /** 입력: 없어요. 출력: 가장 작은 확대(kMinZoom)예요. 실패: 없어요. */
    int minZoom() const;
    /** 입력: 없어요. 출력: 가장 큰 확대(kMaxZoom)예요. 실패: 없어요. */
    int maxZoom() const;
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
/** 축척 막대의 길이와 글자를 구해요.
 * 입력: 지도 가운데 위도 latitude, 확대 zoom, 막대의 최대 폭 maxPixels(px)예요.
 * 출력: {"pixels": 막대 폭, "label": "200 m" 꼴 글자}예요.
 * 실패: 없어요.
 */
    Q_INVOKABLE QVariantMap scaleFor(double latitude, double zoom, int maxPixels) const;
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
    /** 입력: 없어요. 출력: 저장소 글자(storeTotalText·storePinnedText·storeError)를 다시 읽도록 알려요. 실패: 없어요. */
    void storeChanged();
private:
    MapService& service_;
    kgc::ui::MapLayerSlot& layers_;
    QString storeTotalText_;
    QString storePinnedText_;
    QString storeError_;
    inline static QPointer<MapController> instance_;
};
}
