// [0.0.3 T3 T9 T12 T13] specs/features/0.0.3-windows.md
#pragma once
#include <QPointer>
#include <QQuickWindow>
#include <QUrl>
#include <functional>
class QQmlEngine;

namespace kgc::ui {
/** 기능 컨트롤러 아래에서 별도 창의 수명과 자리를 함께 관리해요.
 * 입력: 화면 엔진, 창 주소, 기본 크기와 설정 읽기·쓰기 함수.
 * 출력: 같은 객체로 다시 열리는 별도 창.
 * 실패: 창을 만들 수 없거나 화면이 없으면 열기에 실패해요.
 */
class SeparateWindow : public QObject {
    Q_OBJECT
public:
    /** 기능의 설정 접근자를 받아 창 종료까지 자리 저장을 맡아요.
     * 컨트롤러의 값 멤버로 두는 것을 전제로 해서 QObject 부모를 따로 받지 않아요(소유자는 컨트롤러 하나).
     * 입력: 엔진, 창 주소, 기본 크기, 저장값 읽기·쓰기 함수.
     * 출력: 없음.
     * 실패: 없음.
     */
    SeparateWindow(QQmlEngine& engine, QUrl source, QSize defaultSize,
        std::function<QString()> readPlacement, std::function<void(const QString&)> writePlacement);
    /** 이미 만들어진 창을 알려줘요.
     * 입력: 없음.
     * 출력: 창 포인터, 열기 전에는 nullptr.
     * 실패: 없음.
     */
    QQuickWindow* window() const;
    /** 처음에는 자리를 복원하고 이후에는 같은 창을 앞으로 가져와요.
     * 입력: 없음.
     * 출력: 창을 표시했으면 true.
     * 실패: 창 생성 실패나 화면 없음은 false.
     * 흐름:
     * 1. QGuiApplication::screens — 사용할 화면이 있는지 확인해요.
     * 2. QQmlComponent::createWithInitialProperties — 루트 문맥으로 숨겨진 창을 만들어요.
     * 3. restorePlacement — 설정 읽기 함수와 화면 정보로 자리를 정해요.
     * 4. QQuickWindow::setScreen·setGeometry·show — 화면과 자리를 적용한 뒤 보여줘요.
     * 5. QQuickWindow::showNormal·raise·requestActivate — 기존 창도 앞으로 가져와요.
     */
    bool open();
    /** 보통 상태인 창의 자리를 기능의 설정에 저장해요.
     * 입력: 없음.
     * 출력: 없음.
     * 실패: 창이 없거나 최소화·최대화 상태면 무시해요.
     * 흐름:
     * 1. kgc::ui::savePlacement — 화면 기준 좌표를 문자열로 바꿔요.
     * 2. writePlacement_ — 생성자에서 받은 설정 쓰기 함수로 저장해요.
     */
    void savePlacement();
signals:
    /** 창이 처음 만들어졌음을 컨트롤러에 알려줘요.
     * 입력: 없음.
     * 출력: 변경 알림.
     * 실패: 없음.
     */
    void windowChanged();
private:
    QQmlEngine& engine_;
    QUrl source_;
    QSize defaultSize_;
    std::function<QString()> readPlacement_;
    std::function<void(const QString&)> writePlacement_;
    QPointer<QQuickWindow> window_;
};
}
