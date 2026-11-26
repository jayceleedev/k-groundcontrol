// [0.5.0 5.6.3 T3] specs/features/0.5.0-map.md
#pragma once
#include <QString>
namespace kgc::map {
/** 축척 막대 하나의 길이예요.
 * 입력: 없어요.
 * 출력: 땅의 길이 meters(m)와 화면 길이 pixels(px)예요.
 * 실패: 없어요.
 */
struct ScaleBarLength { double meters; int pixels; };
/** 화면 폭 안에 드는 가장 긴 축척 막대를 골라요.
 * 입력: 1 px의 길이 metersPerPixel(m)과 막대의 최대 폭 maxPixels(px)예요.
 * 출력: {1, 2, 5} × 10^k m(k ≥ 0) 가운데 maxPixels 안에 드는 가장 긴 것과 반올림한 px이에요. 1 m도 들지 않으면 1 m예요.
 * 실패: 없어요.
 */
ScaleBarLength scaleBar(double metersPerPixel, int maxPixels);
/** 축척 막대의 글자를 만들어요.
 * 입력: 막대의 길이 meters(m)예요.
 * 출력: 1000 미만은 "N m", 이상은 "N km"(정수가 아니면 소수 한 자리, "1.5 km")예요. 단위 글자는 번역하지 않아요.
 * 실패: 없어요.
 */
QString scaleLabel(double meters);
}
