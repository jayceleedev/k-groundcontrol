// [0.6.0 5.11.3 T9] specs/features/0.6.0-fly.md
#pragma once
#include <kgc/fly/FlyTypes.h>
#include <kgc/vehicle/states/VehicleMessage.h>
#include <QtGlobal>
namespace kgc::fly {
/** 누를 수 있는 단추를 정하는 데 쓰는 값을 모아요.
 * 입력: FlyService가 상태 묶음과 실행기에서 채워요.
 * 출력: 능력 capabilities, 링크 linkOk, 시동 armed, 착륙 상태 landedState(MAVLink 값, 모름 -1), 기체 위치 hasPosition,
 *       홈(위치와 고도) hasHome, 실행 중 busy, 지금 모드 modeId(ArmingBundle.modeId, 모름 -1)예요.
 * 실패: 없어요.
 */
struct FlyInputs {
    FlyCapabilities capabilities;
    bool linkOk = false;
    bool armed = false;
    int landedState = -1;
    bool hasPosition = false;
    bool hasHome = false;
    bool busy = false;
    qint64 modeId = -1;
};
/** 지금 누를 수 있는 비행 단추를 정해요(순수 함수).
 * 입력: in이에요. 착륙 상태 2·3·4는 공중, 1은 땅, 그 밖은 모름이에요.
 * 출력: 링크가 없으면 모두 거짓이에요. 시동 끄기·비상 정지는 실행 중이어도 참일 수 있어요(끼어들어 나가요).
 *       이륙 모드(launchOnArmModes)에서는 '시동'이 거짓이에요. 종류를 모르면 이륙·착륙·RTL·일시정지·모드·여기로 가기가 거짓이에요.
 * 실패: 없어요.
 */
FlyReadiness readinessOf(const FlyInputs& in);
/** 모드 창에서 그 모드를 고를 수 있는지 정해요(결정 5).
 * 입력: 모드 mode와 착륙 상태 landedState예요.
 * 출력: 스틱을 따르는 모드가 아니거나 땅(1)이면 참이에요. 모름은 공중처럼 봐요.
 * 실패: 없어요.
 */
bool modeChoosable(const kgc::vehicle::firmware::FlightMode& mode, int landedState);
/** 기체 메시지가 실패 까닭으로 덧붙일 만한지 정해요.
 * 입력: 기체 메시지 message예요.
 * 출력: 중요도가 알림(5) 이상(값이 5 이하)이고 글이 비지 않으면 참이에요.
 * 실패: 없어요.
 */
bool isReason(const kgc::vehicle::states::VehicleMessage& message);
}
