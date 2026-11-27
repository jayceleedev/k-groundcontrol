// [0.5.1 5.1 T1] specs/features/0.5.1-map-sharp.md
#pragma once
#include <QtGlobal>
namespace kgc::map {
/** 휠 사건 하나를 더한 뒤의 휠 모음이에요.
 * 입력: 없어요.
 * 출력: 이번에 옮길 층 수 steps(양수는 확대, 음수는 축소), 이 몸짓에서 남은 모은 값 sum, 이 몸짓이 잔 값 흐름인지 fine이에요.
 * 실패: 없어요.
 */
struct WheelStep { int steps = 0; double sum = 0; bool fine = false; };
/** 휠 사건 하나를 모은 값에 더해 몇 층 갈지 정해요.
 * 입력: 이 몸짓에서 모은 값 sum, 이 몸짓이 잔 값 흐름인지 fine, 휠 사건의 angleDelta.y인 delta,
 *       이 지도 부품이 마지막으로 층을 넘긴 뒤 지난 시간 sinceStepMs(ms, 음수면 시계가 뒤로 간 것으로 보고 아주 큰 값으로 써요)예요.
 * 출력: 120(마우스 한 칸)마다 한 층이에요. 120의 배수가 아닌 값이 한 번이라도 온 몸짓(잔 값 흐름)은 한 번에 한 층만,
 *       그것도 마지막 층에서 kWheelStepIntervalMs가 지난 뒤에만 가고, 그 전에 넘친 값은 한 칸에서 잘라 버려요.
 *       방향이 바뀌면 모은 값을 버려요. delta가 0이면 아무것도 바꾸지 않아요.
 * 실패: 없어요.
 */
WheelStep wheelStep(double sum, bool fine, int delta, qint64 sinceStepMs);
/** 지금 확대에서 steps만큼 정수 층을 옮긴 확대를 구해요.
 * 입력: 지금 확대 current, 옮길 층 수 steps, 지도의 가장 작은 확대 minZoom과 가장 큰 확대 maxZoom이에요.
 * 출력: 확대는 floor(current + kZoomEpsilon) + steps, 축소는 ceil(current - kZoomEpsilon) + steps, 0이면 current예요(14.6에서 확대 15, 축소 14).
 *       끝에 [minZoom, maxZoom]으로 잘라요. 지도 폭에 맞춘 가장 작은 확대는 소수일 수 있어요.
 * 실패: 없어요.
 */
double zoomStepTarget(double current, int steps, double minZoom, double maxZoom);
/** 핀치가 끝났을 때 맞출 정수 층을 구해요.
 * 입력: 핀치를 시작할 때의 확대 startZoom, 놓을 때의 확대 endZoom, 가장 작은 확대 minZoom과 가장 큰 확대 maxZoom이에요.
 * 출력: endZoom을 반올림한 층이에요. 다만 kPinchSnapMin 넘게 움직였는데 반올림이 시작 층으로 돌아가면 움직인 방향으로 한 층 가요.
 *       끝에 [minZoom, maxZoom]으로 잘라요.
 * 실패: 없어요.
 */
double pinchSnapTarget(double startZoom, double endZoom, double minZoom, double maxZoom);
/** 화면 배율에 맞는 타일 화소 배수를 정해요.
 * 입력: 연결된 화면 가운데 가장 큰 devicePixelRatio인 screenRatio예요.
 * 출력: kHighDpiMinRatio(1.15) 이상이면 2(512 px 타일), 아니면 1(256 px 타일)이에요.
 * 실패: 없어요.
 */
int tilePixelRatioFor(double screenRatio);
}
