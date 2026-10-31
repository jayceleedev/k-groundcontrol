// [0.0.1 T1] specs/features/0.0.1-shell.md
#pragma once
#include <cstdint>
#include <optional>
#include <QMetaType>

namespace kgc::app::errors {
/** 모듈의 고유 번호를 담아요.
 * 입력: 없음.
 * 출력: 16비트 번호.
 * 실패: 없음.
 */
struct ModuleId {
    std::uint16_t value;
    constexpr bool operator==(const ModuleId&) const = default;
};
/** 모듈과 오류 번호를 함께 담아요.
 * 입력: 없음.
 * 출력: 비교할 수 있는 오류 코드.
 * 실패: 없음.
 */
struct ErrorCode {
    ModuleId module;
    std::uint16_t code;
    constexpr bool operator==(const ErrorCode&) const = default;
};
}

// [BASE 9.4] specs/BASE.md
Q_DECLARE_METATYPE(kgc::app::errors::ErrorCode)
Q_DECLARE_METATYPE(std::optional<kgc::app::errors::ErrorCode>)
