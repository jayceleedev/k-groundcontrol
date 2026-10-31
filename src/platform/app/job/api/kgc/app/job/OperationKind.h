// [0.2.0 T2 T5] specs/features/0.2.0-fcflash.md
#pragma once
namespace kgc::app::job {
/** 작업의 종류를 밝혀 시동 중 규칙을 판정할 수 있게 해요.
 * 입력: BASE §9.2 표의 작업 이름이에요.
 * 출력: 표의 값 전부를 담은 이름표 열거예요. 기능 이름이 들어 있지만 코드 의존은 아니에요.
 * 실패: 없어요.
 */
enum class OperationKind {
    SetupPage, FcFlash, EscFlash, ElrsFlash, TrackerFlash, EscConfig, ElrsConfig, AiWrite, ParamBulk,
    LayoutWrite, MotorTest,
    ParamSingle, Tuning, Autotune,
    TrackerConfig, TrackerCalib, TrackerManual,
    FlyAction, MissionStartStop, Read
};
}
