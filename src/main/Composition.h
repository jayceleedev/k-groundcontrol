// [0.1.0 T10 T13 T17] specs/features/0.1.0-link.md
// [0.0.3 T13] specs/features/0.0.3-windows.md
// [0.0.1 T1–T10] specs/features/0.0.1-shell.md
#pragma once
#include <QObject>
#include <QStringList>
#include <kgc/appSettings/AppInfo.h>
class QQmlEngine;
class QQuickWindow;
namespace kgc::app::log { class LogFeed; }
namespace kgc::app::i18n { class Translator; }
namespace kgc {
/** 엔진 수명에 맞춰 전역 모듈을 조립해요.
 * 입력: 엔진과 배포 정보와 번역기와 로그 통로, 시험용 시리얼 경로예요.
 * 출력: 없음.
 * 실패: 없음.
 */
class Composition : public QObject {
public:
/** 등록부와 서비스를 만들어 화면에 연결해요.
 * 입력: 엔진과 배포 정보와 번역기와 로그 통로, 시험용 시리얼 경로예요.
 * 출력: 없음.
 * 실패: 없음.
 */
    Composition(QQmlEngine& engine, appSettings::AppInfo info, app::i18n::Translator& translator,
                app::log::LogFeed& feed, const QString& extraSerial = {});
/** 조립된 화면의 창을 만들고 표시해요.
 * 입력: 없음.
 * 출력: 창 포인터.
 * 실패: 화면을 만들 수 없으면 nullptr.
 * 흐름:
 * 1. QQmlComponent::create — 등록된 문맥으로 창을 만들어요.
 * 2. QObject::connect — 셸 닫기를 앱 종료에 연결해요.
 * 3. QQuickWindow::show — 창을 표시해요.
 */
    QQuickWindow* loadShell();
/** 로그 통로가 다시 내보내지 않을 경고 출처 접두를 모아요.
 * 입력: 없음.
 * 출력: 기능 모듈이 낸 QML 경로 접두 목록.
 * 실패: 없음.
 */
    static QStringList logGuardPrefixes();
private:
    QQmlEngine& engine_;
};
}
