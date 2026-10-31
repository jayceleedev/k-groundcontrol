// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <cstdint>
namespace kgc::vehicle::registry {
/** 기체 발견과 재생성 조건을 순수 함수로 제공해요.
 * 입력: HEARTBEAT의 컴포넌트·자동조종기·기체 종류와 이전 자동조종기 값이에요.
 * 출력: 새 기체로 받을지와 기존 기체를 다시 만들지의 판정이에요.
 * 실패: 없어요.
 */
class VehicleSelection {
public:
/** 자동조종기가 보낸 기체 하트비트인지 판정해요.
 * 입력: 송신 compid와 HEARTBEAT의 autopilot·mavType이에요.
 * 출력: compid가 1이고 유효한 자동조종기이며 GCS·짐벌이 아니면 참이에요.
 * 실패: 없어요.
 */
    static bool accepts(std::uint8_t compid, std::uint8_t autopilot, std::uint8_t mavType);
/** 같은 시스템의 자동조종기 값이 바뀌었는지 판정해요.
 * 입력: 이전 값 previous와 새 하트비트의 current예요.
 * 출력: 두 값이 다르면 기체를 다시 만들도록 참을 돌려줘요.
 * 실패: 없어요.
 */
    static bool recreate(std::uint8_t previous, std::uint8_t current);
};
}
