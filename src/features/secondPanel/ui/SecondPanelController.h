// [0.0.3 T9 T12 T13] specs/features/0.0.3-windows.md
#pragma once
#include <QQuickWindow>
#include <QAbstractListModel>
#include <kgc/ui/SeparateWindow.h>
class QQmlEngine;
namespace kgc::secondPanel {
class SecondPanelService;
/** 별도 창의 수명과 화면 표시를 관리해요.
 * 입력: 전역 서비스와 화면 엔진.
 * 출력: 창과 화면용 값.
 * 실패: 창 생성 실패 시 열기를 중단해요.
 */
class SecondPanelController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QQuickWindow* window READ window NOTIFY windowChanged)
    Q_PROPERTY(int saveDelayMs READ saveDelayMs CONSTANT)
    Q_PROPERTY(int arrangementIndex READ arrangementIndex NOTIFY arrangementChanged)
    Q_PROPERTY(QVariantList cells READ cells NOTIFY arrangementChanged)
public:
    /** 서비스와 함께 살며 앱 종료 때 창 자리를 저장해요.
     * 입력: 서비스와 엔진.
     * 출력: 없음.
     * 실패: 없음.
     */
    SecondPanelController(SecondPanelService& service, QQmlEngine& engine);
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
    /** 고른 배치의 목록 위치를 읽어요.
     * 입력: 없음.
     * 출력: 설정 목록 안의 위치.
     * 실패: 없음.
     */
    int arrangementIndex() const;
    /** 화면에 쓸 칸의 비율 좌표를 읽어요.
     * 입력: 없음.
     * 출력: 사각형 목록.
     * 실패: 없음.
     */
    QVariantList cells() const;
    /** 고른 배치를 서비스에 넘겨요.
     * 입력: 설정 목록의 위치.
     * 출력: 없음.
     * 실패: 범위 밖 위치는 무시해요.
     * 흐름:
     * 1. SecondPanelService::setArrangement — 배치를 저장해요.
     */
    Q_INVOKABLE void chooseArrangement(int index);
    /** 창을 처음 만들거나 기존 창을 다시 보여줘요.
     * 입력: 없음.
     * 출력: 없음.
     * 실패: 화면을 만들지 못하면 창은 없어요.
     * 흐름:
     * 1. SeparateWindow::open — 처음에 자리를 복원하고 같은 창을 다시 열어요.
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
    /** 저장된 배치의 변경을 알려줘요.
     * 입력: 없음.
     * 출력: 변경 알림.
     * 실패: 없음.
     */
    void arrangementChanged();
private:
    SecondPanelService& service_;
    kgc::ui::SeparateWindow window_;
};
}
