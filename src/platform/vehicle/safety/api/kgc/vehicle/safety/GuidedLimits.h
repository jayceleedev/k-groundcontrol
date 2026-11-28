// [0.6.0 5.7] specs/features/0.6.0-fly.md
#pragma once
namespace kgc::vehicle::safety {
/** 유도 비행(여기로 가기·이륙 높이)의 한계예요.
 * 입력: 없어요.
 * 출력: 기체 지금 위치에서 잰 수평 최대 거리 maxDistanceM(m)과 홈 기준 최저·최고 고도 minAltitudeM·maxAltitudeM(m)이에요.
 * 실패: 없어요.
 */
struct GuidedLimits { double maxDistanceM; double minAltitudeM; double maxAltitudeM; };
/** 운영 설정 셋(safety.guidedMaxDistanceM·guidedMinAltitudeM·guidedMaxAltitudeM)에서 유도 한계를 읽어요.
 * 입력: 없어요(설정을 읽어요).
 * 출력: 설정 값의 GuidedLimits예요. 최저가 최고보다 크면 둘을 바꿔 돌려줘요.
 * 실패: 없어요(범위 밖 저장값은 설정이 기본값으로 읽어요).
 */
GuidedLimits guidedLimits();
/** 유도 목표 판정 결과예요.
 * 입력: 없어요.
 * 출력: Ok(보내도 됨), TooFar(너무 멂), TooLow(너무 낮음), TooHigh(너무 높음)예요.
 * 실패: 없어요.
 */
enum class GuidedCheck { Ok, TooFar, TooLow, TooHigh };
/** 유도 목표가 한계 안인지 판정해요(순수 함수).
 * 입력: 한계 limits, 기체 지금 위치에서 목표까지 수평 거리 distanceM(m), 목표의 홈 기준 고도 altitudeRelM(m)이에요.
 * 출력: 거리를 먼저 보고, 거리가 최대보다 크거나 NaN이면 TooFar, 고도가 최저보다 낮거나 NaN이면 TooLow, 최고보다 높으면 TooHigh, 그 밖 Ok예요. 경계값은 Ok예요.
 * 실패: 없어요.
 */
GuidedCheck checkGuided(const GuidedLimits& limits, double distanceM, double altitudeRelM);
}
