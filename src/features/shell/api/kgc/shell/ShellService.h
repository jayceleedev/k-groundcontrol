// [0.0.1 T4–T10] specs/features/0.0.1-shell.md
#pragma once
#include <QObject>
namespace kgc::app::i18n { class Translator; }
namespace kgc::shell {
/** 창에서 일어나는 사건을 앱 로그에 남겨요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
class ShellService : public QObject {
    Q_OBJECT
public:
/** 앱 시작과 초기 메뉴 상태를 기록해요.
 * 입력: 번역기와 부모.
 * 출력: 없음.
 * 실패: 없음.
 */
    explicit ShellService(kgc::app::i18n::Translator& translator, QObject* parent = nullptr);
/** 앱 종료를 기록해요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
    ~ShellService() override;
/** 창이 표시된 크기를 기록해요.
 * 입력: 창 너비와 높이.
 * 출력: 없음.
 * 실패: 없음.
 * 흐름:
 * 1. KGC_LOG_INFO — 창 표시 사건을 기록해요.
 */
    void windowShown(int width, int height);
/** 열린 패널을 기록해요.
 * 입력: 패널 식별자.
 * 출력: 없음.
 * 실패: 없음.
 * 흐름:
 * 1. KGC_LOG_INFO — 패널 열림 사건을 기록해요.
 */
    void panelOpened(const QString& id);
/** 패널 닫힘을 기록해요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 * 흐름:
 * 1. KGC_LOG_INFO — 패널 닫힘 사건을 기록해요.
 */
    void panelClosed();
/** 좌 메뉴 접힘 상태를 기록해요.
 * 입력: 접힘 여부.
 * 출력: 없음.
 * 실패: 없음.
 * 흐름:
 * 1. KGC_LOG_DEBUG — 메뉴 접힘 사건을 기록해요.
 */
    void menuCollapsed(bool collapsed);
};
}
