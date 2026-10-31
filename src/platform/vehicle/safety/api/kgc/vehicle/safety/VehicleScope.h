// [0.2.0 T5] specs/features/0.2.0-fcflash.md
#pragma once
#include <kgc/vehicle/registry/Vehicle.h>
#include <QPointer>
namespace kgc::vehicle::safety {
/** 시동 판정이 볼 기체의 범위를 나타내요.
 * 입력: One은 기체 하나, Any는 연결된 기체 전부예요.
 * 출력: ArmedPolicy::check에 넘기는 값이에요. 기체 없는 작업은 Any로 판정해요.
 * 실패: 없어요.
 */
class VehicleScope {
public:
/** 기체 하나만 보는 범위를 만들어요.
 * 입력: 볼 vehicle이에요.
 * 출력: One 범위예요.
 * 실패: 없어요.
 */
    static VehicleScope One(kgc::vehicle::registry::Vehicle* vehicle) { return VehicleScope(vehicle, false); }
/** 연결된 기체 전부를 보는 범위를 만들어요.
 * 입력: 없어요.
 * 출력: Any 범위예요.
 * 실패: 없어요.
 */
    static VehicleScope Any() { return VehicleScope(nullptr, true); }
/** Any 범위인지 읽어요.
 * 입력: 없어요.
 * 출력: Any이면 참이에요.
 * 실패: 없어요.
 */
    bool any() const { return any_; }
/** One 범위의 기체를 읽어요.
 * 입력: 없어요.
 * 출력: 기체의 비소유 포인터이며, Any이거나 기체가 지워졌으면 nullptr예요.
 * 실패: 없어요.
 */
    kgc::vehicle::registry::Vehicle* vehicle() const { return vehicle_; }
private:
    VehicleScope(kgc::vehicle::registry::Vehicle* vehicle, bool any) : vehicle_(vehicle), any_(any) {}
    QPointer<kgc::vehicle::registry::Vehicle> vehicle_;
    bool any_;
};
}
