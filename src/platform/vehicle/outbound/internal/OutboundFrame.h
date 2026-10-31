// [0.1.1 T7] specs/features/0.1.1-link-hardening.md
#pragma once
#include <kgc/vehicle/outbound/VehicleSender.h>
namespace kgc::vehicle::outbound {
/** GCS 번호로 v2 프레임을 하나 만들어 링크 워커로 넘겨요.
 * 입력: 연결 관리자 links, 대상 linkId, GCS sysid·compid·채널·메시지를 받는 pack 함수예요.
 * 출력: 워커 송신 예약에 성공하면 참이에요.
 * 실패: 열린 링크가 없거나 번호가 다르면 거짓이에요.
 */
bool packAndWrite(kgc::comm::links::LinkManager& links, int linkId, const VehicleSender::PackFn& pack);
}
