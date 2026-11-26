// [0.5.0 5.6.3 T6] specs/features/0.5.0-map.md
#pragma once
#include <kgc/app/errors/Result.h>
#include <QGeoRectangle>
namespace kgc::map {
/** 영역 받기를 시작해도 되는지 보고 받을 타일 수를 세요.
 * 입력: 영역 area와 확대 범위 minZoom~maxZoom이에요.
 * 출력: 받을 타일 수(tileCount)예요.
 * 실패: kMinZoom ≤ minZoom ≤ maxZoom ≤ kMaxZoom가 아니면 BadZoomRange, kMaxDownloadTiles를 넘으면 TooManyTiles예요.
 */
kgc::app::errors::Result<qint64> planDownload(const QGeoRectangle& area, int minZoom, int maxZoom);
}
