// [0.0.3 T3 T4 T5 T12 T13] specs/features/0.0.3-windows.md
#pragma once
#include <QQuickWindow>
#include <QAbstractListModel>
#include <kgc/ui/SeparateWindow.h>
class QQmlEngine;
namespace kgc::analyze {
class AppLogService;
/** 별도 창의 수명과 화면 표시를 관리해요.
 * 입력: 전역 서비스와 화면 엔진.
 * 출력: 창과 화면용 값.
 * 실패: 창 생성 실패 시 열기를 중단해요.
 */
class AppLogController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QQuickWindow* window READ window NOTIFY windowChanged)
    Q_PROPERTY(int saveDelayMs READ saveDelayMs CONSTANT)
    Q_PROPERTY(QAbstractListModel* model READ model CONSTANT)
public:
    /** 서비스와 함께 살며 앱 종료 때 창 자리를 저장해요.
     * 입력: 서비스와 엔진.
     * 출력: 없음.
     * 실패: 없음.
     */
    AppLogController(AppLogService& service, QQmlEngine& engine);
    /** 만들어진 창을 알려줘요.
     * 입력: 없음.
     * 출력: 창 포인터, 열기 전에는 nullptr.
     * 실패: 없음.
     */
    QQuickWindow* window() const;
    /** 자리 저장을 늦출 시간을 알려줘요.
     * 입력: 없음.
     * 출력: 밀리초 간격.
     * 실패: 없음.
     */
    int saveDelayMs() const;
    /** 최근 로그를 화면에 연결해요.
     * 입력: 없음.
     * 출력: 서비스의 목록 객체.
     * 실패: 없음.
     */
    QAbstractListModel* model() const;
    /** 창을 처음 만들거나 기존 창을 다시 보여줘요.
     * 입력: 없음.
     * 출력: 없음.
     * 실패: 화면을 만들지 못하면 창은 없어요.
     * 흐름:
     * 1. SeparateWindow::open — 처음에 자리를 복원하고 같은 창을 다시 열어요.
     * 2. KGC_LOG_INFO — 창을 열었다는 사건을 기록해요.
     */
    Q_INVOKABLE void open();
    /** 보통 상태인 창의 화면 기준 자리를 저장해요.
     * 입력: 없음.
     * 출력: 없음.
     * 실패: 창이 없거나 최소화·최대화 상태면 무시해요.
     * 흐름:
     * 1. SeparateWindow::savePlacement — 화면 기준 자리로 바꾸고 기능 설정에 저장해요.
     */
    Q_INVOKABLE void savePlacement();
signals:
    /** 창이 처음 만들어졌음을 알려줘요.
     * 입력: 없음.
     * 출력: 변경 알림.
     * 실패: 없음.
     */
    void windowChanged();
private:
    AppLogService& service_;
    kgc::ui::SeparateWindow window_;
};
}
