// [0.6.0 5.9 T26] specs/features/0.6.0-fly.md
// [0.5.0 5.6.9 T18 T20] specs/features/0.5.0-map.md
#pragma once
#include <QAbstractListModel>
#include <QList>
#include <QPointer>
namespace kgc::vehicle::registry { class Vehicle; class VehicleRegistry; }
namespace kgc::map {
/** 지도 레이어가 그릴 기체 표식 목록이에요(기체마다 한 줄, 등록부의 기체 순서).
 * 입력: 기체 등록부예요. 기체의 위치·자세 상태와 활성 기체를 들어요.
 * 출력: 역할 sysid·coordinate·heading·active·homeCoordinate·hasHome·hasPosition·altitudeText예요.
 *       heading은 자세의 yaw, 없으면 위치의 기수 방향, 그것도 없으면 0이에요. 위치 없음((0, 0)·NaN)은 hasPosition·hasHome이 거짓이에요.
 *       altitudeText는 홈 기준 고도를 정수로 반올림한 "30 m"이고, 고도를 모르면 빈 값이에요.
 * 실패: 없어요. 기체 상태 값은 바꾸지 않아요.
 */
class VehicleMarkerModel : public QAbstractListModel {
    Q_OBJECT
public:
/** 등록부의 지금 기체로 줄을 만들고 바뀜을 듣기 시작해요.
 * 입력: 기체 등록부 registry와 부모 parent예요.
 * 출력: 기체가 생기거나 없어지면 줄을 넣고 빼고, 위치·자세가 바뀌면 그 줄의 dataChanged를 내요.
 * 실패: 없어요.
 */
    VehicleMarkerModel(kgc::vehicle::registry::VehicleRegistry& registry, QObject* parent);
    /** 입력: 부모 색인 parent(목록이라 쓰지 않아요). 출력: 기체 수예요. 실패: 없어요. */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    /** 입력: 줄 index와 역할 role이에요. 출력: 그 기체의 표식 값이며, 없는 줄은 빈 값이에요. 실패: 없어요. */
    QVariant data(const QModelIndex& index, int role) const override;
    /** 입력: 없어요. 출력: Qt::UserRole + 1부터의 여덟 역할 이름이에요. 실패: 없어요. */
    QHash<int, QByteArray> roleNames() const override;
private:
    void add(kgc::vehicle::registry::Vehicle* vehicle);
    void remove(kgc::vehicle::registry::Vehicle* vehicle);
    void changed(const kgc::vehicle::registry::Vehicle* vehicle, const QList<int>& roles);
    kgc::vehicle::registry::VehicleRegistry& registry_;
    QList<QPointer<kgc::vehicle::registry::Vehicle>> rows_;
};
}
