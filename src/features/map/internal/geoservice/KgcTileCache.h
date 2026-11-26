// [0.5.0 5.6.7] specs/features/0.5.0-map.md
#pragma once
#include <QtLocation/private/qgeofiletilecache_p.h>
namespace kgc::map {
/** 디스크를 건드리지 않는 Qt Location 타일 캐시예요(메모리와 텍스처만).
 * 입력: 없어요. 디스크 저장은 TileStore가 맡아요.
 * 출력: 메모리 kTileMemoryCacheBytes, 화면 밖 텍스처 kTileExtraTextureBytes의 캐시예요.
 * 실패: 없어요.
 * 주의: clearMapId()는 부르지 않아요. 가상 함수가 아니라 막을 수 없고, 빈 폴더 이름이면 작업 폴더의 *-*-*-*.* 파일을 지워요.
 */
class KgcTileCache : public QGeoFileTileCache {
    Q_OBJECT
public:
/** 폴더 없는 캐시를 만들어요.
 * 입력: 부모 parent예요.
 * 출력: 디스크 폴더 이름이 빈 캐시예요.
 * 실패: 없어요.
 */
    explicit KgcTileCache(QObject* parent = nullptr);
/** 메모리와 텍스처 캐시만 비워요.
 * 입력: 없어요.
 * 출력: 세 캐시를 비워요. 기본 구현처럼 작업 폴더의 파일을 지우지 않아요.
 * 실패: 없어요.
 */
    void clearAll() override;
protected:
/** 크기만 정해요.
 * 입력: 없어요.
 * 출력: 디스크 0, 메모리와 텍스처 한도예요. 기본 구현을 부르지 않아 캐시 폴더를 만들거나 지우지 않아요.
 * 실패: 없어요.
 */
    void init() override;
};
}
