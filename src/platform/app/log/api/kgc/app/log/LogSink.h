// [0.0.3 T7 T8] specs/features/0.0.3-windows.md
// [0.0.1 T1] specs/features/0.0.1-shell.md
#pragma once
#include <kgc/app/log/LogLevel.h>
#include <QStringList>
#include <QtLogging>

namespace kgc::app::log {
struct LogEvent;
class LogFeed;
namespace detail {
// 수준 매크로의 구현만 접근하도록 공개 서비스와 구분해요.
template<class... Values>
void writeLog(LogLevel level, const char* category, LogEvent event, const char* eventName,
              const char* message, const Values&... values);
}
/** 앱과 Qt의 로그를 같은 파일에 기록해요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
class LogSink {
public:
/** 로그 파일과 Qt 메시지 처리기를 준비해요.
 * 입력: 폴더와 최소 수준과 회전 크기와 보관 개수.
 * 출력: 준비 성공 여부.
 * 실패: 파일을 열 수 없으면 false.
 */
    static bool install(const QString& dir, LogLevel level, int maxFileMB, int maxFiles);
    /** 화면으로 로그를 전달할 통로를 붙이거나 떼어요.
     * 입력: 엔진보다 오래 사는 통로와 조립이 넘긴 경고 출처 접두 목록, 떼려면 nullptr.
     * 출력: 없음.
     * 실패: 없음.
     */
    static void attachFeed(LogFeed* feed, QStringList guardFilePrefixes = {});
private:
    template<class... Values>
    friend void detail::writeLog(LogLevel, const char*, LogEvent, const char*, const char*, const Values&...);
    static void write(LogLevel level, const QString& category, const QString& event, const QString& message,
                      const QMessageLogContext& context = {});
    static QString format(LogLevel level, const QString& category,
                          const QString& event, const QString& message);
    static LogLevel fromQtLevel(QtMsgType level);
};
}
