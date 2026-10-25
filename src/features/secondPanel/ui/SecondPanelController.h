// [0.0.3 T1 T2 T3 T9 T12 T13 T14] specs/features/0.0.3-windows.md
#pragma once
#include <QQuickWindow>
#include <QAbstractListModel>
#include <QPointer>
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
     * 1. QQmlComponent::create — 처음에만 루트 문맥으로 창을 만들어요.
     * 2. restorePlacement — 저장값과 화면 목록으로 자리를 정해요.
     * 3. QQuickWindow::setScreen·setGeometry·show — 복원한 창을 보여줘요.
     * 4. QQuickWindow::showNormal·raise·requestActivate — 기존 창도 앞으로 가져와요.
     */
    Q_INVOKABLE void open();
    /** 보통 상태인 창의 화면 기준 자리를 저장해요.
     * 입력: 없음.
     * 출력: 없음.
     * 실패: 창이 없거나 최소화·최대화 상태면 무시해요.
     * 흐름:
     * 1. kgc::ui::savePlacement — 화면 기준 좌표를 문자열로 바꿔요.
     * 2. settings::setWindowPlacement — 운영 설정에 즉시 저장해요.
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
    QQmlEngine& engine_;
    QPointer<QQuickWindow> window_;
};
}
