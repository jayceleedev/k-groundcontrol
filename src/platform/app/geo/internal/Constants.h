// [0.6.0 5.1] specs/features/0.6.0-fly.md
#pragma once
namespace kgc::app::geo {
// Qt QGeoCoordinate::distanceTo가 쓰는 지구 평균 반지름(m)과 같은 값이에요. 지도 궤적과 거리가 어긋나지 않게 해요.
inline constexpr double kEarthMeanRadiusM = 6371007.2;
}
