// [0.2.0 T7 T8] specs/features/0.2.0-fcflash.md
#pragma once
#include <kgc/fcFlash/ModuleId.h>
#include <QString>
namespace kgc::fcFlash {
/** 펌웨어 굽기가 시작되지 못했거나 멈춘 까닭을 나타내요.
 * 입력: 스펙 §6·§7의 실패 상황이에요.
 * 출력: 각 상황에 대응하는 FcFlashError 값이에요. Aborted는 중단 플래그로 멈춘 경우예요.
 * 실패: 없어요.
 */
enum class FcFlashError : std::uint16_t {
    Armed = 1, OtherLink, BootloaderNotFound, BadBootloaderRevision, WrongBoard, TooLarge, BadImage,
    DownloadFailed, EraseFailed, ProgramFailed, VerifyFailed, PortLost, Aborted
};
/** 굽기 오류를 공통 코드로 바꿔요.
 * 입력: 변환할 FcFlashError인 error예요.
 * 출력: fcFlash 모듈 번호와 오류 번호를 담은 ErrorCode예요.
 * 실패: 없어요.
 */
inline kgc::app::errors::ErrorCode make(FcFlashError error)
{
    return {kModuleId, static_cast<std::uint16_t>(error)};
}
/** 굽기 오류를 화면 문구로 바꿔요.
 * 입력: 표시할 공통 오류 코드 error예요.
 * 출력: 현재 언어의 문구이며, 값이 필요한 WrongBoard와 다른 모듈의 코드는 빈 문자열이에요.
 * 실패: 없어요.
 */
QString errorText(kgc::app::errors::ErrorCode error);
}
