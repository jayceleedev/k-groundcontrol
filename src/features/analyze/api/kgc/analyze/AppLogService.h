// [0.0.3 T4 T5] specs/features/0.0.3-windows.md
#pragma once
#include <QObject>
#include <QAbstractListModel>
namespace kgc::app::log { class LogFeed; }
namespace kgc::analyze {
class AppLogModel;
/** 앱 수명 동안 최근 로그 목록을 유지해요.
 * 입력: 로그 통로.
 * 출력: 화면에서 읽는 목록.
 * 실패: 없음.
 */
class AppLogService : public QObject {
    Q_OBJECT
public:
    /** 먼저 구독한 뒤 최근 기록을 읽어 빠짐과 겹침을 막아요.
     * 입력: 로그 통로와 부모 객체.
     * 출력: 없음.
     * 실패: 없음.
     */
    explicit AppLogService(kgc::app::log::LogFeed& feed, QObject* parent = nullptr);
    /** 화면에 제공할 읽기 전용 목록을 알려줘요.
     * 입력: 없음.
     * 출력: 로그 목록 객체.
     * 실패: 없음.
     */
    QAbstractListModel* model() const;
private:
    AppLogModel* model_;
    quint64 last_ = 0;
};
}
