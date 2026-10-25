// [0.0.3 T7 T13] specs/features/0.0.3-windows.md
// [0.0.1 T1–T10] specs/features/0.0.1-shell.md
// [0.0.2 T26 T27] specs/features/0.0.2-theme.md
#include "Composition.h"
#include <kgc/app/boot/BootOptions.h>
#include <kgc/app/paths/Paths.h>
#include <kgc/app/log/LogSink.h>
#include <kgc/app/log/LogFeed.h>
#include <kgc/app/settings/Settings.h>
#include <kgc/app/i18n/Translator.h>
#include <kgc/appSettings/Settings.h>
#include <QGuiApplication>
#include <QIcon>
#include <kgc/ui/SurfaceFormat.h>
#include <QQuickStyle>
#include <QQmlApplicationEngine>

/** 부팅 순서에 맞춰 앱을 조립해요.
 * 입력: 명령줄 인자.
 * 출력: 종료 코드.
 * 실패: 로그나 창 초기화 실패는 1.
 * 흐름:
 * 1. kgc::ui::applyDefaultSurfaceFormat — 창 생성 전에 공용 표면 형식을 정해요.
 * 2. BootOptions::parse — 부팅 값을 읽어요.
 * 3. Paths::init — 데이터 폴더를 정해요.
 * 4. LogSink::install·attachFeed — 로그를 열고 화면 통로를 붙여요.
 * 5. Settings::instance — 운영 설정을 준비해요.
 * 6. Translator::setLanguage — 저장된 언어를 적용해요.
 * 7. QQuickStyle::setStyle — 화면 부품의 바탕 스타일을 정해요.
 * 8. new Composition — 서비스와 등록부를 조립해요.
 * 9. QGuiApplication::setWindowIcon — 등록된 자원에서 창 아이콘을 정해요.
 * 10. Composition::loadShell·app.exec — 조립한 창과 이벤트 처리를 시작해요.
 * 11. LogSink::attachFeed — 엔진 파괴 전에 화면 통로를 떼어요.
 */
int main(int argc, char* argv[])
{
    kgc::ui::applyDefaultSurfaceFormat();
    QGuiApplication app(argc, argv);
    kgc::app::log::LogFeed feed;
    app.setApplicationName("K-GroundControl");
    app.setApplicationVersion(KGC_VERSION);
    const auto boot = kgc::app::boot::BootOptions::parse(app.arguments());
    kgc::app::paths::Paths::init(boot.dataDir);
    if (!kgc::app::log::LogSink::install(kgc::app::paths::Paths::logsApp(), boot.logLevel, boot.maxFileMB, boot.maxFiles))
        return 1;
    kgc::app::log::LogSink::attachFeed(&feed, kgc::Composition::logGuardPrefixes());
    kgc::app::settings::Settings::instance();
    kgc::app::i18n::Translator translator(":/kgc/i18n");
    translator.setLanguage(kgc::appSettings::settings::toString(kgc::appSettings::settings::language()));
    QQuickStyle::setStyle("Basic");
    QQmlApplicationEngine engine;
    auto* composition = new kgc::Composition(engine,
        {KGC_VERSION, KGC_HOMEPAGE_URL, ":/kgc/LICENSE", app.applicationDirPath() + "/THIRD_PARTY_NOTICES.md"}, translator, feed);
    QGuiApplication::setWindowIcon(QIcon(":/kgc/icons/app-tile.svg"));
    const int result = composition->loadShell() ? app.exec() : 1;
    kgc::app::log::LogSink::attachFeed(nullptr);
    return result;
}
