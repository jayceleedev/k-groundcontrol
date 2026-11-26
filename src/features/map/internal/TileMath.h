// [0.5.0 5.6.3 T2] specs/features/0.5.0-map.md
#pragma once
#include <QGeoRectangle>
#include <QtGlobal>
namespace kgc::map {
/** 한 확대에서 타일 한 장의 번호예요.
 * 입력: 없어요.
 * 출력: 열 x와 행 y예요.
 * 실패: 없어요.
 */
struct TileXY { int x; int y; };
/** 한 확대에서 영역을 덮는 타일 묶음이에요.
 * 입력: 없어요.
 * 출력: 왼쪽 위 타일 x·y와 열 수 columns, 행 수 rows예요. 열 i(0부터)의 타일 x는 (x + i) mod 2^z예요.
 * 실패: 없어요.
 */
struct TileRange { int x; int y; int columns; int rows; };
/** 위도·경도가 든 웹 메르카토르 타일을 찾아요.
 * 입력: 위도 lat, 경도 lon(도), 확대 z예요. 위도는 ±kMaxMercatorLatitude로 잘라요.
 * 출력: 0 ~ 2^z − 1로 자른 타일 번호예요.
 * 실패: 없어요.
 */
TileXY tileOf(double lat, double lon, int z);
/** 영역을 덮는 타일 묶음을 구해요.
 * 입력: 영역 area와 확대 z예요. 왼쪽 위 경도가 오른쪽 아래 경도보다 크면 날짜 변경선을 넘는 영역으로 봐요.
 * 출력: 왼쪽 위 타일부터의 열·행 수예요. 날짜 변경선을 넘으면 열 수는 2^z를 넘지 않아요.
 * 실패: 없어요.
 */
TileRange tileRange(const QGeoRectangle& area, int z);
/** 확대 범위의 타일 수를 세요.
 * 입력: 영역 area와 확대 범위 minZoom~maxZoom이에요.
 * 출력: 확대마다 columns × rows의 합이에요.
 * 실패: 없어요.
 */
qint64 tileCount(const QGeoRectangle& area, int minZoom, int maxZoom);
/** 지도 1 px이 땅에서 몇 m인지 구해요.
 * 입력: 위도 lat(도)과 확대 zoom(소수 가능)이에요.
 * 출력: kMetersPerPixelAtZoom0 · cos(lat) / 2^zoom(m)이에요.
 * 실패: 없어요.
 */
double metersPerPixel(double lat, double zoom);
}
