// [0.6.0 5.9 T26] specs/features/0.6.0-fly.md
// [0.5.0 5.6.9 T18 T20] specs/features/0.5.0-map.md
#include "VehicleMarkerModel.h"
#include "Trail.h"
#include <kgc/vehicle/registry/VehicleRegistry.h>
#include <kgc/vehicle/states/VehicleStates.h>
#include <QGeoCoordinate>
#include <cmath>
namespace kgc::map {
using kgc::vehicle::registry::Vehicle;
using kgc::vehicle::registry::VehicleRegistry;
using kgc::vehicle::states::VehicleStates;
namespace {
enum Role { Sysid = Qt::UserRole + 1, Coordinate, Heading, Active, HomeCoordinate, HasHome, HasPosition, AltitudeText };
// 자세의 yaw가 먼저이고, 없으면 위치의 기수 방향, 그것도 없으면 위(0°)예요.
double headingOf(const VehicleStates& states)
{
    const double yaw = states.attitude().yawDeg;
    if (std::isfinite(yaw)) return yaw;
    const double heading = states.position().headingDeg;
    return std::isfinite(heading) ? heading : 0.0;
}
}
VehicleMarkerModel::VehicleMarkerModel(VehicleRegistry& registry, QObject* parent)
    : QAbstractListModel(parent), registry_(registry)
{
    for (const auto& vehicle : registry_.vehicles()) {
        if (vehicle) add(vehicle);
    }
    connect(&registry_, &VehicleRegistry::vehicleAdded, this, &VehicleMarkerModel::add);
    connect(&registry_, &VehicleRegistry::vehicleRemoved, this, &VehicleMarkerModel::remove);
    connect(&registry_, &VehicleRegistry::activeChanged, this, [this] {
        if (!rows_.isEmpty()) emit dataChanged(index(0), index(int(rows_.size()) - 1), {Active});
    });
}
int VehicleMarkerModel::rowCount(const QModelIndex& parent) const { return parent.isValid() ? 0 : int(rows_.size()); }
QVariant VehicleMarkerModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= rows_.size() || !rows_[index.row()]) return {};
    const Vehicle* vehicle = rows_[index.row()];
    const auto* states = vehicle->states();
    const auto position = states->position();
    switch (role) {
    case Sysid: return vehicle->sysid();
    case Coordinate: return QVariant::fromValue(QGeoCoordinate(position.latDeg, position.lonDeg));
    case Heading: return headingOf(*states);
    case Active: return vehicle == registry_.active();
    case HomeCoordinate: return QVariant::fromValue(QGeoCoordinate(position.homeLatDeg, position.homeLonDeg));
    case HasHome: return isMapPosition(position.homeLatDeg, position.homeLonDeg);
    case HasPosition: return isMapPosition(position.latDeg, position.lonDeg);
    // 홈 기준 고도를 정수 m로 반올림한 글이에요(번역하지 않아요). 모르면 빈 값이라 글이 숨어요.
    case AltitudeText: return std::isfinite(position.altRelM) ? QStringLiteral("%1 m").arg(qRound(position.altRelM)) : QString();
    default: return {};
    }
}
QHash<int, QByteArray> VehicleMarkerModel::roleNames() const
{
    return {{Sysid, "sysid"}, {Coordinate, "coordinate"}, {Heading, "heading"}, {Active, "active"},
            {HomeCoordinate, "homeCoordinate"}, {HasHome, "hasHome"}, {HasPosition, "hasPosition"},
            {AltitudeText, "altitudeText"}};
}
void VehicleMarkerModel::add(Vehicle* vehicle)
{
    // 등록부의 기체 목록은 sysid 순서라서 같은 순서로 끼워 넣어요.
    int row = 0;
    while (row < rows_.size() && rows_[row] && rows_[row]->sysid() < vehicle->sysid()) ++row;
    beginInsertRows({}, row, row);
    rows_.insert(row, vehicle);
    endInsertRows();
    auto* states = vehicle->states();
    connect(states, &VehicleStates::positionChanged, this, [this, vehicle] {
        changed(vehicle, {Coordinate, Heading, HomeCoordinate, HasHome, HasPosition, AltitudeText});
    });
    connect(states, &VehicleStates::attitudeChanged, this, [this, vehicle] { changed(vehicle, {Heading}); });
}
void VehicleMarkerModel::remove(Vehicle* vehicle)
{
    vehicle->states()->disconnect(this);
    const auto row = rows_.indexOf(vehicle);
    if (row < 0) return;
    beginRemoveRows({}, int(row), int(row));
    rows_.removeAt(row);
    endRemoveRows();
}
void VehicleMarkerModel::changed(const Vehicle* vehicle, const QList<int>& roles)
{
    const auto row = rows_.indexOf(vehicle);
    if (row >= 0) emit dataChanged(index(int(row)), index(int(row)), roles);
}
}
