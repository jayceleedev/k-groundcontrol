// [0.0.1 T1] specs/features/0.0.1-shell.md
#pragma once
#include <kgc/app/errors/ErrorCode.h>
#include <kgc/app/log/LogSink.h>

namespace kgc::app::log {
/** 모듈과 사건의 번호를 담아요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
struct LogEvent { kgc::app::errors::ModuleId module; std::uint16_t code; };
namespace detail {
// 파일 형식은 숫자 LogEvent 대신 사건 이름을 써요. 쓰지 않는 번호를 인자로 넘기지 않아요.
// 매크로가 #event로 만든 열거 표기에서 마지막 이름을 얻고, 카테고리의 kgc.를 떼어 모듈 이름을 붙여요.
template<class... Values>
void writeLog(LogLevel level, const char* category, const char* eventName,
              const char* message, const Values&... values)
{
    QString text = QString::fromUtf8(message);
    ((text = text.arg(values)), ...);
    const auto module = QString::fromUtf8(category).mid(4);
    const auto name = QString::fromUtf8(eventName).section("::", -1);
    LogSink::write(level, QString::fromUtf8(category), module + '.' + name, text);
}
}
}
#define KGC_LOG_DEBUG(category, event, message, ...) \
    ::kgc::app::log::detail::writeLog(::kgc::app::log::LogLevel::Debug, category, \
        #event, message __VA_OPT__(,) __VA_ARGS__)
#define KGC_LOG_INFO(category, event, message, ...) \
    ::kgc::app::log::detail::writeLog(::kgc::app::log::LogLevel::Info, category, \
        #event, message __VA_OPT__(,) __VA_ARGS__)
#define KGC_LOG_WARN(category, event, message, ...) \
    ::kgc::app::log::detail::writeLog(::kgc::app::log::LogLevel::Warn, category, \
        #event, message __VA_OPT__(,) __VA_ARGS__)
#define KGC_LOG_ERROR(category, event, message, ...) \
    ::kgc::app::log::detail::writeLog(::kgc::app::log::LogLevel::Error, category, \
        #event, message __VA_OPT__(,) __VA_ARGS__)
