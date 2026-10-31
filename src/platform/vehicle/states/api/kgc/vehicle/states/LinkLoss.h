// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <QtGlobal>
namespace kgc::vehicle::states {
/** 하트비트 간격이 끊김 기준에 도달했는지 판정해요.
 * 입력: 같은 단조 시계에서 측정한 마지막 수신 lastHeartbeatMs와 현재 nowMs예요.
 * 출력: 차이가 3500ms 이상이면 참이며, 수신 전이나 현재보다 미래인 수신 시각은 거짓이에요.
 * 실패: 없어요.
 */
bool isLinkLost(qint64 lastHeartbeatMs, qint64 nowMs);
}
