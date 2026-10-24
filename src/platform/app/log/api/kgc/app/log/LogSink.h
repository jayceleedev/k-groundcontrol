// [0.0.1 T1] specs/features/0.0.1-shell.md
#pragma once
#include <QString>
#include <QtLogging>

namespace kgc::app::log {
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
    static bool install(const QString& dir, const QString& level, int maxFileMB, int maxFiles);
/** 등록된 사건 한 줄을 기록해요.
 * 입력: 수준과 카테고리와 사건과 메시지.
 * 출력: 없음.
 * 실패: 없음.
 */
    static void write(QtMsgType level, const QString& category, const QString& event, const QString& message);
private:
    static QString format(QtMsgType level, const QString& category,
                          const QString& event, const QString& message);
};
}
