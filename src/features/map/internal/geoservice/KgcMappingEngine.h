// [0.5.1 5.5] specs/features/0.5.1-map-sharp.md
// [0.5.0 5.6.7] specs/features/0.5.0-map.md
#pragma once
#include <QList>
#include <QPointer>
#include <QtLocation/QGeoServiceProvider>
#include <QtLocation/private/qgeotiledmap_p.h>
#include <QtLocation/private/qgeotiledmappingmanagerengine_p.h>
namespace kgc::map {
class TileSource;
/** 타일 공급으로 그리는 Qt Location 타일 지도 엔진이에요(GUI 스레드).
 * 입력: 타일 공급이에요. 지도 종류는 settings::MapType 순서로 id 1·2·3이에요.
 * 출력: 확대 2~20, 256 px 칸(2배 타일의 512 px 그림도 같은 칸에 그려요), 방위·기울기 없음, 메모리 캐시만 쓰는 지도예요.
 *       언어나 타일 화소 배수가 바뀌거나, 다른 배수를 대신 보여 준 뒤 서버에 다시 닿으면(fallbackStale) 타일 판을 올려
 *       보이는 타일을 지금 배수로 다시 받아요(저장소에 있으면 저장소에서).
 * 실패: 없어요.
 */
class KgcMappingEngine : public QGeoTiledMappingManagerEngine {
    Q_OBJECT
public:
/** 카메라 능력·지도 종류·캐시·fetcher를 정하고 공급의 시그널을 이어요.
 * 입력: 타일 공급 source와 Qt가 넘기는 error·errorString이에요.
 * 출력: error는 NoError, errorString은 비워요. 캐시 예산은 이때의 타일 화소 배수로 정해요.
 * 실패: 없어요.
 */
    KgcMappingEngine(TileSource& source, QGeoServiceProvider::Error* error, QString* errorString);
/** 미리 받기를 끈 타일 지도를 만들어요.
 * 입력: 없어요.
 * 출력: 새 QGeoTiledMap이에요(Qt의 지도 항목이 지워요). 만든 지도를 약한 포인터로 기억해요.
 * 실패: 없어요.
 */
    QGeoMap* createMap() override;
/** 살아 있는 지도마다 보이는 장면을 비워 그림도 캐시도 없는 타일만 다시 청하게 해요.
 * 입력: 없어요.
 * 출력: 서버에 다시 닿았을 때 빈 칸을 채워요.
 * 실패: 없어요.
 */
    void refreshVisible();
private:
    QList<QPointer<QGeoTiledMap>> maps_;
};
}
