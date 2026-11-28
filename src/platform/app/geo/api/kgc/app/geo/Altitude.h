// [0.6.0 5.1] specs/features/0.6.0-fly.md
#pragma once
#include <optional>
namespace kgc::app::geo {
/** 기체가 고도 값을 읽는 기준이에요.
 * 입력: 없어요(어느 펌웨어가 어느 기준을 쓰는지는 펌웨어 플러그인이 정해요).
 * 출력: Amsl은 해수면, RelativeHome은 홈 기준, AboveCurrent는 지금 자리 위예요.
 * 실패: 없어요.
 */
enum class AltitudeFrame { Amsl, RelativeHome, AboveCurrent };
/** 홈 기준 고도를 그 틀의 값으로 바꿔요(여기로 가기).
 * 입력: 고도 틀 frame, 홈 기준 고도 relativeM(m), 홈 해수면 고도 baseAmslM(m)예요.
 * 출력: RelativeHome이면 relativeM, Amsl이면 baseAmslM + relativeM이에요.
 * 실패: 쓰는 값이 유한하지 않거나 frame이 AboveCurrent(또는 그 밖)이면 값이 없어요.
 */
std::optional<double> altitudeIn(AltitudeFrame frame, double relativeM, double baseAmslM);
/** 지금 자리 위 높이를 그 틀의 값으로 바꿔요(이륙).
 * 입력: 고도 틀 frame, 지금 자리 위 높이 heightM(m), 기체 지금 홈 기준 고도 altRelM(m)과 해수면 고도 altAmslM(m)예요.
 * 출력: AboveCurrent면 heightM, RelativeHome이면 heightM + max(altRelM, 0), Amsl이면 altAmslM + heightM이에요.
 *       땅이 홈보다 낮으면 홈 기준 고도를 0으로 봐서 홈 기준 값이 입력보다 작아지지 않게 해요.
 * 실패: 쓰는 값이 유한하지 않거나 frame이 그 밖이면 값이 없어요.
 */
std::optional<double> heightIn(AltitudeFrame frame, double heightM, double altRelM, double altAmslM);
}
