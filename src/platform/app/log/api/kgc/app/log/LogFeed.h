// [0.0.3 T5 T7 T8] specs/features/0.0.3-windows.md
#pragma once
#include <kgc/app/log/LogLevel.h>
#include <QObject>
#include <QList>
#include <QMutex>
#include <utility>

namespace kgc::app::log {
/** 파일에 쓴 한 줄과 전달 순번을 함께 담아요.
 * 입력: 수준과 사건 정보와 줄바꿈 없는 파일 내용.
 * 출력: 화면으로 전달할 기록.
 * 실패: 없음.
 */
struct LogRecord {
    quint64 seq = 0;
    LogLevel level = LogLevel::Info;
    QString category, event, message, text;
};
/** 여러 스레드의 로그를 순서대로 화면 스레드에 전달해요.
 * 입력: 기록과 보관 용량.
 * 출력: 최근 기록과 비동기 변경 알림.
 * 실패: 없음.
 */
class LogFeed : public QObject {
    Q_OBJECT
public:
    /** 기본 용량으로 최근 로그를 보관해요.
     * 입력: 부모 객체.
     * 출력: 없음.
     * 실패: 없음.
     */
    explicit LogFeed(QObject* parent = nullptr);
    /** 시험이나 작은 소비자에 맞게 보관 용량을 정해요.
     * 입력: 양수 용량과 부모 객체.
     * 출력: 없음.
     * 실패: 양수가 아닌 용량은 프로그래밍 오류예요.
     */
    explicit LogFeed(int capacity, QObject* parent = nullptr);
    /** 순번을 매겨 보관하고 수신 처리를 큐에 넣어요.
     * 입력: 아직 순번을 매기지 않은 기록.
     * 출력: 없음.
     * 실패: 없음.
     */
    void publish(LogRecord record);
    /** 보관 중인 기록과 마지막 순번을 한 번에 읽어요.
     * 입력: 없음.
     * 출력: 기록 목록과 마지막 순번, 빈 목록이면 0.
     * 실패: 없음.
     */
    std::pair<QList<LogRecord>, quint64> recent() const;
    /** 보관할 수 있는 최대 줄 수를 알려줘요.
     * 입력: 없음.
     * 출력: 용량.
     * 실패: 없음.
     */
    int capacity() const;
signals:
    /** 소유 스레드에서 새 기록을 알려줘요.
     * 입력: 순번이 정해진 기록.
     * 출력: 구독자 알림.
     * 실패: 없음.
     */
    void recordAdded(const kgc::app::log::LogRecord& record);
private:
    friend class LogSink;
    static thread_local bool delivering_;
    const int capacity_;
    mutable QMutex mutex_;
    QList<LogRecord> records_;
    quint64 last_ = 0;
};
}
Q_DECLARE_METATYPE(kgc::app::log::LogRecord)
