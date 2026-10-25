// [0.0.1 T1] specs/features/0.0.1-shell.md
// [0.0.3 T2] specs/features/0.0.3-windows.md
#pragma once
#include <kgc/ui/ModuleId.h>
#include <QCoreApplication>

namespace kgc::ui {
enum class UiError : std::uint16_t { DuplicateId = 1 };
/** 화면 오류 번호를 공통 코드로 바꿔요.
 * 입력: 화면 오류.
 * 출력: 공통 오류 코드.
 * 실패: 없음.
 */
inline kgc::app::errors::ErrorCode make(UiError error) { return {kModuleId, static_cast<std::uint16_t>(error)}; }
/** 화면 오류를 사용자 문구로 바꿔요.
 * 입력: 오류 코드.
 * 출력: 번역된 문구.
 * 실패: 없음.
 */
inline QString errorText(kgc::app::errors::ErrorCode error)
{
    const auto message = QT_TRANSLATE_NOOP("Ui", "같은 항목이 이미 등록되어 있어요.");
    return error == make(UiError::DuplicateId)
        ? QCoreApplication::translate("Ui", message) : QString{};
}
}
