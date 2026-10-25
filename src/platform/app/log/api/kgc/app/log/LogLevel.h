// [BASE 6] specs/BASE.md
// [0.0.3 T6] specs/features/0.0.3-windows.md
#pragma once
#include <QString>
#include <QStringView>
#include <optional>

namespace kgc::app::log {
/** 로그의 최소 출력 수준과 각 사건의 수준을 같은 형으로 나타내요.
 * 입력: 없음.
 * 출력: 낮은 수준부터 Debug, Info, Warn, Error.
 * 실패: 없음.
 */
enum class LogLevel { Debug, Info, Warn, Error };
/** 부팅 설정의 수준 이름을 로그 수준으로 바꿔요.
 * 입력: 소문자 수준 이름.
 * 출력: 이름에 대응하는 수준.
 * 실패: 지원하지 않는 이름이면 빈 값.
 */
std::optional<LogLevel> parseLogLevel(QStringView name);
/** 로그 수준을 설정과 화면이 함께 쓰는 이름으로 바꿔요.
 * 입력: 로그 수준.
 * 출력: 소문자 수준 이름.
 * 실패: 정의되지 않은 수준은 빈 문자열.
 */
QString logLevelName(LogLevel level);
}
