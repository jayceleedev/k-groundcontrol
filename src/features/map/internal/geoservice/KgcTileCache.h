// [0.5.1 5.5] specs/features/0.5.1-map-sharp.md
// [0.5.0 5.6.7] specs/features/0.5.0-map.md
#pragma once
#include <QtLocation/private/qgeofiletilecache_p.h>
namespace kgc::map {
/** 디스크를 건드리지 않는 Qt Location 타일 캐시예요(메모리와 텍스처만).
 * 입력: 엔진이 만들어질 때의 타일 화소 배수예요. 디스크 저장은 TileStore가 맡아요.
 * 출력: 메모리 kTileMemoryCacheBytes × 배수², 화면 밖 텍스처 kTileExtraTextureBytes × 배수²의 캐시예요(2배 그림은 바이트가 약 4배라서).
 *       실행 중에 배수가 바뀌어도 이 예산은 그대로예요.
 * 실패: 없어요.
 * 주의: clearMapId()는 부르지 않아요. 가상 함수가 아니라 막을 수 없고, 빈 폴더 이름이면 작업 폴더의 *-*-*-*.* 파일을 지워요.
 */
class KgcTileCache : public QGeoFileTileCache {
    Q_OBJECT
public:
/** 폴더 없는 캐시를 만들어요.
 * 입력: 타일 화소 배수 pixelRatio(1 또는 2)와 부모 parent예요.
 * 출력: 디스크 폴더 이름이 빈 캐시예요. 크기 한도는 init()에서 pixelRatio로 정해요.
 * 실패: 없어요.
 */
    explicit KgcTileCache(int pixelRatio, QObject* parent = nullptr);
/** 메모리와 텍스처 캐시만 비워요.
 * 입력: 없어요.
 * 출력: 세 캐시를 비워요. 기본 구현처럼 작업 폴더의 파일을 지우지 않아요.
 * 실패: 없어요.
 */
    void clearAll() override;
protected:
/** 크기만 정해요.
 * 입력: 없어요.
 * 출력: 디스크 0, 메모리와 텍스처 한도(배수²배)예요. 기본 구현을 부르지 않아 캐시 폴더를 만들거나 지우지 않아요.
 * 실패: 없어요.
 */
    void init() override;
private:
    int pixelRatio_;
};
}
