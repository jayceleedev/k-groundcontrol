// [0.5.0 5.6.8 T19 T27] specs/features/0.5.0-map.md
#include "TrailBook.h"
#include "Trail.h"
#include <kgc/map/Settings.h>
#include <kgc/vehicle/registry/VehicleRegistry.h>
#include <kgc/vehicle/states/VehicleStates.h>
#include <QPointer>
namespace kgc::map {
using kgc::vehicle::registry::Vehicle;
using kgc::vehicle::registry::VehicleRegistry;
TrailBook::TrailBook(VehicleRegistry& registry, QObject* parent) : QObject(parent)
{
    for (const auto& vehicle : registry.vehicles()) {
        if (vehicle) watch(vehicle);
    }
    connect(&registry, &VehicleRegistry::vehicleAdded, this, &TrailBook::watch);
    connect(&registry, &VehicleRegistry::vehicleRemoved, this, &TrailBook::forget);
    // 최대 점 수를 줄이면 넘는 점을 바로 지워요. 화면의 궤적 레이어도 같은 규칙으로 스스로 지워서 신호는 내지 않아요.
    settings::onTrailMaxPointsChanged(this, [this](int maxPoints) { trim(maxPoints); });
}
QList<QGeoCoordinate> TrailBook::trailOf(int sysid) const { return trails_.value(sysid); }
void TrailBook::clear()
{
    for (auto& points : trails_) points.clear();
    emit cleared();
}
void TrailBook::watch(Vehicle* vehicle)
{
    const int sysid = vehicle->sysid();
    trails_.insert(sysid, {});
    auto* states = vehicle->states();
    connect(states, &kgc::vehicle::states::VehicleStates::positionChanged, this,
            [this, sysid, states = QPointer(states)] {
        if (!states || !trails_.contains(sysid)) return;
        const auto position = states->position();
        const QGeoCoordinate next(position.latDeg, position.lonDeg);
        // 간격은 지금 설정을 쓰므로, 간격을 바꾸면 다음 점부터 적용돼요.
        auto& points = trails_[sysid];
        if (appendTrailPoint(points, next, settings::trailMinMeters(), settings::trailMaxPoints()))
            emit pointAdded(sysid, points.constLast());
    });
}
void TrailBook::forget(Vehicle* vehicle)
{
    // 지워지는 기체가 마지막으로 보내는 위치가 같은 번호로 다시 생긴 기체의 궤적에 섞이지 않게 끊어요.
    vehicle->states()->disconnect(this);
    trails_.remove(vehicle->sysid());
}
void TrailBook::trim(int maxPoints)
{
    for (auto& points : trails_) {
        if (points.size() > maxPoints) points.remove(0, points.size() - maxPoints);
    }
}
}
