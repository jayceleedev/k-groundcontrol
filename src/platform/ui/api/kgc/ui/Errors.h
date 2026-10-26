// [0.0.1 T1] specs/features/0.0.1-shell.md
// [0.0.3 T2] specs/features/0.0.3-windows.md
// [0.0.4 T2] specs/features/0.0.4-righttabs.md
#pragma once
#include <kgc/ui/ModuleId.h>
#include <QCoreApplication>

namespace kgc::ui {
/** 화면 등록부가 거부한 까닭이에요.
 * DuplicateId: 같은 식별자가 이미 있어요. TopTaken: 우 사이드바 계기 자리(Top)를 이미 다른 항목이 쓰고 있어요.
 */
enum class UiError : std::uint16_t { DuplicateId = 1, TopTaken = 2 };
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
    const auto duplicate = QT_TRANSLATE_NOOP("Ui", "같은 항목이 이미 등록되어 있어요.");
    const auto topTaken = QT_TRANSLATE_NOOP("Ui", "우 사이드바 계기 자리는 하나만 쓸 수 있어요.");
    if (error == make(UiError::DuplicateId)) return QCoreApplication::translate("Ui", duplicate);
    if (error == make(UiError::TopTaken)) return QCoreApplication::translate("Ui", topTaken);
    return {};
}
}
