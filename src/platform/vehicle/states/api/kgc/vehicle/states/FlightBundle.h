// [0.6.0 5.4] specs/features/0.6.0-fly.md
#pragma once
#include <QObject>
namespace kgc::vehicle::states {
/** 기체의 착륙 상태와 VTOL 상태를 MAVLink 값 그대로 전달해요.
 * 입력: EXTENDED_SYS_STATE의 landed_state·vtol_state예요.
 * 출력: landedState는 0 모름·1 땅·2 공중·3 이륙 중·4 착륙 중, vtolState는 0 VTOL 아님·1 고정익으로 전환 중·2 멀티콥터로 전환 중·3 멀티콥터·4 고정익이에요.
 *       받기 전과, 5초 동안 오지 않거나 링크가 끊긴 뒤에는 둘 다 -1이에요(오래된 값이 단추를 정하지 않게).
 * 실패: 없어요.
 */
struct FlightBundle {
    Q_GADGET
    Q_PROPERTY(int landedState MEMBER landedState)
    Q_PROPERTY(int vtolState MEMBER vtolState)
public:
    int landedState = -1;
    int vtolState = -1;
};
}
Q_DECLARE_METATYPE(kgc::vehicle::states::FlightBundle)
