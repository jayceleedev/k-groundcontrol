// [0.5.0 5.6.8 T19 T27] specs/features/0.5.0-map.md
#pragma once
#include <QGeoCoordinate>
#include <QHash>
#include <QList>
#include <QObject>
namespace kgc::vehicle::registry { class Vehicle; class VehicleRegistry; }
namespace kgc::map {
/** 기체마다 지나온 위치(궤적)를 모아 두는 책이에요(GUI 스레드, MapService::Private 소유).
 * 입력: 기체 등록부예요. 처음 있는 기체와 새로 생기는 기체의 positionChanged를 들어요.
 * 출력: 점을 더할 때마다 pointAdded, 모두 비우면 cleared를 내고, trailOf로 한 기체의 궤적을 내줘요.
 * 실패: 위치 없음(NaN·(0, 0))과 궤적 점 간격 안의 움직임은 점으로 더하지 않아요.
 */
class TrailBook : public QObject {
    Q_OBJECT
public:
/** 등록부의 기체를 듣기 시작해요.
 * 입력: 기체 등록부 registry와 부모 parent예요.
 * 출력: 기체마다 빈 궤적으로 시작해요. 궤적 최대 점 수 설정이 줄면 넘는 점을 앞에서 바로 지워요.
 * 실패: 없어요.
 */
    TrailBook(kgc::vehicle::registry::VehicleRegistry& registry, QObject* parent = nullptr);
/** 한 기체의 궤적을 읽어요.
 * 입력: 기체 번호 sysid예요.
 * 출력: 오래된 점부터의 궤적이며, 없으면 빈 목록이에요.
 * 실패: 없어요.
 */
    QList<QGeoCoordinate> trailOf(int sysid) const;
/** 모든 기체의 궤적을 비워요.
 * 입력: 없어요.
 * 출력: 궤적을 비우고 cleared를 내요. 다음 위치부터 다시 모아요.
 * 실패: 없어요.
 */
    void clear();
signals:
/** 궤적에 점을 더했음을 알려요.
 * 입력: 기체 번호 sysid와 더한 점 coordinate예요.
 * 출력: 없어요.
 * 실패: 없어요.
 */
    void pointAdded(int sysid, const QGeoCoordinate& coordinate);
/** 모든 궤적을 비웠음을 알려요. 입력: 없어요. 출력: 없어요. 실패: 없어요. */
    void cleared();
private:
    void watch(kgc::vehicle::registry::Vehicle* vehicle);
    void forget(kgc::vehicle::registry::Vehicle* vehicle);
    void trim(int maxPoints);
    QHash<int, QList<QGeoCoordinate>> trails_;
};
}
