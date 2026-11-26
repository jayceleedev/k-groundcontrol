// [0.5.0 5.6.2 5.6.5 5.6.8] specs/features/0.5.0-map.md
#pragma once
#include <kgc/map/MapService.h>
#include <QThread>
namespace kgc::map {
class TileStore;
class TileSource;
// 서비스가 가진 저장소 스레드와 타일 공급이에요. 저장소는 그 스레드에서 지워져요(FcFlashService 선례).
struct MapService::Private {
    explicit Private(kgc::vehicle::registry::VehicleRegistry& registry) : vehicles(registry) {}
    kgc::vehicle::registry::VehicleRegistry& vehicles;
    QThread storeThread;
    TileStore* store = nullptr;
    TileSource* source = nullptr;
    // 저장소가 failed를 내면 참, sized를 내면 거짓이에요(고장 동안 영역 받기를 시작하지 않아요).
    bool storeBroken = false;
};
/** 서비스의 타일 공급을 꺼내요.
 * 입력: 지도 서비스 service예요.
 * 출력: 서비스가 소유하는 타일 공급이에요(register.cpp가 지도 플러그인에 넘겨요).
 * 실패: 없어요.
 */
TileSource& tileSource(MapService& service);
}
