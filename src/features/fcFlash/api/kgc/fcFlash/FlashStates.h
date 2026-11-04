// [0.4.1 5.3] specs/features/0.4.1-board-match.md
// [0.2.0 T14–T22] specs/features/0.2.0-fcflash.md
#pragma once
#include <QObject>
#include <QtQml/qqmlregistration.h>
namespace kgc::fcFlash {
Q_NAMESPACE
QML_NAMED_ELEMENT(FlashStates)
/** 화면의 네 단계를 나타내요.
 * 입력: 없어요.
 * 출력: Board·Firmware·Confirm·Flash 중 하나예요.
 * 실패: 없어요.
 */
enum class Step { Board, Firmware, Confirm, Flash };
Q_ENUM_NS(Step)
/** 굽기 흐름의 지금 상태를 나타내요(스펙 §6.1 표).
 * 입력: 없어요.
 * 출력: Idle·Probing·Rebooting·WaitReplug·PickPort는 Board, Ready는 Firmware·Confirm,
 *       Flashing·Reconnecting·Done·Mismatch·NoReturn은 Flash, Failed는 Board 또는 Flash예요.
 * 실패: 없어요.
 */
enum class Phase {
    Idle, Probing, Rebooting, WaitReplug, PickPort, Ready, Flashing, Reconnecting, Done, Mismatch, NoReturn, Failed
};
Q_ENUM_NS(Phase)
/** 펌웨어를 어디서 가져올지 나타내요.
 * 입력: 없어요.
 * 출력: Px4Official·ArduPilotOfficial·LocalFile 중 하나예요.
 * 실패: 없어요.
 */
enum class FirmwareSource { Px4Official, ArduPilotOfficial, LocalFile };
Q_ENUM_NS(FirmwareSource)
/** USB 보드 이름과 같은 보드 ID 후보의 이름을 맞춰 본 결과예요.
 * 입력: 없어요.
 * 출력: NoName(USB 이름을 모름), Matched(하나 이상 맞음), NoMatch(이름은 있지만 맞는 후보가 없음) 중 하나예요.
 * 실패: 없어요.
 */
enum class BoardMatch { NoName, Matched, NoMatch };
Q_ENUM_NS(BoardMatch)
}
