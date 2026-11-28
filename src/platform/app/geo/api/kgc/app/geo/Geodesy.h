// [0.6.0 5.1] specs/features/0.6.0-fly.md
#pragma once
namespace kgc::app::geo {
/** 두 위치 사이의 지표 거리를 재요(하버사인, 지구 평균 반지름은 Qt QGeoCoordinate::distanceTo와 같은 값).
 * 입력: 첫 점의 위도 lat1Deg·경도 lon1Deg와 둘째 점의 위도 lat2Deg·경도 lon2Deg(도)예요.
 * 출력: 두 점 사이 거리(m)예요. 지도 궤적이 재는 거리와 어긋나지 않아요.
 * 실패: 값 중 하나라도 유한하지 않으면 NaN이에요.
 */
double distanceMeters(double lat1Deg, double lon1Deg, double lat2Deg, double lon2Deg);
/** 아는 위치인지 판정해요.
 * 입력: 위도 latDeg와 경도 lonDeg(도)예요.
 * 출력: 둘 다 유한하고 둘 다 0은 아니면 참이에요. (0, 0)은 ArduPilot이 위치를 얻기 전에 보내는 값이라 모르는 위치로 봐요.
 * 실패: 없어요.
 */
bool isKnownPosition(double latDeg, double lonDeg);
}
