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
/** 사건 이름과 인자들을 로그 메시지에 붙여요.
 * 입력: 수준과 카테고리와 사건과 메시지와 값.
 * 출력: 없음.
 * 실패: 없음.
 */
template<class... Values>
void writeLog(QtMsgType level, const char* category, LogEvent event,
              const char* eventName, const char* message, const Values&... values)
{
    Q_UNUSED(event);
    QString text = QString::fromUtf8(message);
    ((text = text.arg(values)), ...);
    const auto module = QString::fromUtf8(category).mid(4);
    const auto name = QString::fromUtf8(eventName).section("::", -1);
    LogSink::write(level, QString::fromUtf8(category), module + '.' + name, text);
}
}
#define KGC_LOG_DEBUG(category, event, message, ...) \
    ::kgc::app::log::writeLog(QtDebugMsg, category, \
        {kModuleId, static_cast<std::uint16_t>(event)}, #event, message __VA_OPT__(,) __VA_ARGS__)
#define KGC_LOG_INFO(category, event, message, ...) \
    ::kgc::app::log::writeLog(QtInfoMsg, category, \
        {kModuleId, static_cast<std::uint16_t>(event)}, #event, message __VA_OPT__(,) __VA_ARGS__)
#define KGC_LOG_WARN(category, event, message, ...) \
    ::kgc::app::log::writeLog(QtWarningMsg, category, \
        {kModuleId, static_cast<std::uint16_t>(event)}, #event, message __VA_OPT__(,) __VA_ARGS__)
#define KGC_LOG_ERROR(category, event, message, ...) \
    ::kgc::app::log::writeLog(QtCriticalMsg, category, \
        {kModuleId, static_cast<std::uint16_t>(event)}, #event, message __VA_OPT__(,) __VA_ARGS__)
