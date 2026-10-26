// [0.0.1 T4–T10] specs/features/0.0.1-shell.md
// [0.0.4 T25a T25b T25c] specs/features/0.0.4-righttabs.md
#pragma once
#include <QObject>
#include <QSize>
class QQuickWindow;
class QScreen;
// 메타 객체의 호출 인자에는 완전한 형이 필요하므로 생성 코드에서만 헤더를 읽어요.
Q_MOC_INCLUDE("QQuickWindow")
namespace kgc::ui { class Navigator; }
namespace kgc::shell {
class ShellService;
/** 창 상태와 사용자 동작을 서비스에 이어 줘요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
class ShellController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString activePanelId READ activePanelId NOTIFY activePanelChanged)
    Q_PROPERTY(QString panelArg READ panelArg NOTIFY activePanelChanged)
    Q_PROPERTY(bool leftMenuCollapsed READ leftMenuCollapsed WRITE setLeftMenuCollapsed NOTIFY leftMenuCollapsedChanged)
    Q_PROPERTY(QSize availableScreenSize READ availableScreenSize NOTIFY availableScreenSizeChanged)
    Q_PROPERTY(int leftMenuWidth READ leftMenuWidth WRITE setLeftMenuWidth NOTIFY leftMenuWidthChanged)
    Q_PROPERTY(int leftMenuWidthMin READ leftMenuWidthMin CONSTANT)
    Q_PROPERTY(int leftMenuWidthMax READ leftMenuWidthMax CONSTANT)
public:
/** 탐색 요청과 저장된 메뉴 설정을 화면에 연결해요.
 * 입력: 서비스와 탐색기.
 * 출력: 없음.
 * 실패: 없음.
 */
    ShellController(ShellService& service, kgc::ui::Navigator& navigator);
/** 열린 패널의 식별자를 읽어요.
 * 입력: 없음.
 * 출력: 패널 식별자.
 * 실패: 없음.
 */
    QString activePanelId() const;
/** 열린 패널의 인자를 읽어요.
 * 입력: 없음.
 * 출력: 패널 인자.
 * 실패: 없음.
 */
    QString panelArg() const;
/** 저장된 메뉴 접힘 상태를 읽어요.
 * 입력: 없음.
 * 출력: 접힘 여부.
 * 실패: 없음.
 */
    bool leftMenuCollapsed() const;
/** 창이 있는 화면의 가용 크기를 읽어요.
 * 입력: 없음.
 * 출력: 논리 픽셀 크기.
 * 실패: 없음.
 */
    QSize availableScreenSize() const;
/** 접힘 설정을 저장해요.
 * 입력: 접힘 여부.
 * 출력: 없음.
 * 실패: 없음.
 * 흐름:
 * 1. settings::setLeftMenuCollapsed — 설정을 저장해요.
 * 2. ShellService::menuCollapsed — 설정 구독에서 사건을 기록해요.
 */
    void setLeftMenuCollapsed(bool collapsed);
/** 저장된 좌 메뉴 폭을 읽어요.
 * 입력: 없음.
 * 출력: 배율 1 기준 논리 픽셀 폭(실제 폭은 × Theme.uiScale).
 * 실패: 저장값이 범위 밖이면 Settings가 기본값 200을 돌려줘요.
 */
    int leftMenuWidth() const;
/** 좌 메뉴 폭의 최솟값을 읽어요.
 * 입력: 없음.
 * 출력: 설정 메타데이터의 min(배율 1 기준 논리 픽셀).
 * 실패: 없음.
 */
    int leftMenuWidthMin() const;
/** 좌 메뉴 폭의 최댓값을 읽어요.
 * 입력: 없음.
 * 출력: 설정 메타데이터의 max(배율 1 기준 논리 픽셀).
 * 실패: 없음.
 */
    int leftMenuWidthMax() const;
/** 손잡이를 놓을 때 끈 좌 메뉴 폭을 저장해요.
 * 입력: 배율 1 기준 논리 픽셀 폭.
 * 출력: 없음. 저장되면 leftMenuWidthChanged가 나가요.
 * 실패: 범위 밖 값은 Settings가 거부해 저장하지 않아요.
 * 흐름:
 * 1. settings::setLeftMenuWidth — 메타데이터 범위 안의 폭만 저장해요.
 */
    void setLeftMenuWidth(int width);
/** 창이 옮겨 가는 화면의 크기를 구독해요.
 * 입력: 창.
 * 출력: 없음.
 * 실패: 없음.
 * 흐름:
 * 1. QWindow::screenChanged — 창이 옮겨 가면 화면 구독을 바꿔요.
 * 2. QScreen::availableGeometryChanged — 가용 크기를 갱신해요.
 */
    Q_INVOKABLE void attachWindow(QQuickWindow* window);
/** 같은 메뉴는 닫고 다른 메뉴는 열어요.
 * 입력: 패널 식별자.
 * 출력: 없음.
 * 실패: 없음.
 * 흐름:
 * 1. ShellService::panelClosed — 이전 패널이 있으면 닫힘을 기록해요.
 * 2. ShellService::panelOpened — 새 패널이 있으면 열림을 기록해요.
 */
    Q_INVOKABLE void togglePanel(const QString& id);
/** 표시된 창 크기를 서비스에 넘겨요.
 * 입력: 너비와 높이.
 * 출력: 없음.
 * 실패: 없음.
 * 흐름:
 * 1. ShellService::windowShown — 창 표시 사건을 기록해요.
 */
    Q_INVOKABLE void windowShown(int width, int height);
signals:
/** 열린 패널의 변경을 알려요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
    void activePanelChanged();
/** 접힘 설정의 변경을 알려요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
    void leftMenuCollapsedChanged();
/** 좌 메뉴 폭 설정의 변경을 알려요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
    void leftMenuWidthChanged();
/** 화면 가용 크기의 변경을 알려요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
    void availableScreenSizeChanged();
/** 같은 인자의 반복 요청도 패널에 전달해요.
 * 입력: 패널 식별자와 인자.
 * 출력: 없음.
 * 실패: 없음.
 */
    void panelArgRequested(const QString& id, const QString& arg);
private:
    void openPanel(const QString& id, const QString& arg);
    void bindScreen(QScreen* screen);
    ShellService& service_;
    QString activePanelId_;
    QString panelArg_;
    QSize availableScreenSize_;
    QMetaObject::Connection screenConnection_;
    QMetaObject::Connection geometryConnection_;
};
}
