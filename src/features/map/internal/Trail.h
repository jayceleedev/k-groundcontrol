// [0.5.0 5.6.3 T4] specs/features/0.5.0-map.md
#pragma once
#include <QGeoCoordinate>
#include <QList>
namespace kgc::map {
/** 지도에 그릴 수 있는 위치인지 판정해요.
 * 입력: 위도 latDeg와 경도 lonDeg(도)예요.
 * 출력: 둘 다 유한하고 둘 다 0은 아니면 참이에요. (0, 0)은 ArduPilot이 위치를 얻기 전에 보내는 값이라 "위치 없음"으로 봐요.
 * 실패: 없어요.
 */
bool isMapPosition(double latDeg, double lonDeg);
/** 궤적에 새 점을 더할지 정하고 더해요.
 * 입력: 궤적 points, 새 위치 next, 마지막 점과의 최소 거리 minMeters(m), 최대 점 수 maxPoints예요.
 * 출력: 지도 위치가 아니거나 마지막 점과의 거리가 minMeters 이하면 거짓이에요. 아니면 더하고, maxPoints를 넘는 만큼 앞에서 지운 뒤 참이에요.
 * 실패: 없어요.
 */
bool appendTrailPoint(QList<QGeoCoordinate>& points, const QGeoCoordinate& next, double minMeters, int maxPoints);
}
