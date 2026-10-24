// [0.0.1 T1–T10] specs/features/0.0.1-shell.md
#include "Composition.h"
#include <kgc/app/boot/BootOptions.h>
#include <kgc/app/paths/Paths.h>
#include <kgc/app/log/LogSink.h>
#include <kgc/app/settings/Settings.h>
#include <kgc/app/i18n/Translator.h>
#include <kgc/appSettings/Settings.h>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

/** 부팅 순서에 맞춰 앱을 조립해요.
 * 입력: 명령줄 인자.
 * 출력: 종료 코드.
 * 실패: 로그나 창 초기화 실패는 1.
 * 흐름:
 * 1. BootOptions::parse — 부팅 값을 읽어요.
 * 2. Paths::init — 데이터 폴더를 정해요.
 * 3. LogSink::install — 로그를 열어요.
 * 4. Settings::instance — 운영 설정을 준비해요.
 * 5. Translator::setLanguage — 저장된 언어를 적용해요.
 * 6. Composition::loadShell — 조립한 창을 표시해요.
 */
int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("K-GroundControl");
    app.setApplicationVersion(KGC_VERSION);
    const auto boot = kgc::app::boot::BootOptions::parse(app.arguments());
    kgc::app::paths::Paths::init(boot.dataDir);
    if (!kgc::app::log::LogSink::install(kgc::app::paths::Paths::logsApp(), boot.logLevel, boot.maxFileMB, boot.maxFiles))
        return 1;
    kgc::app::settings::Settings::instance();
    kgc::app::i18n::Translator translator(":/kgc/i18n");
    translator.setLanguage(kgc::appSettings::settings::toString(kgc::appSettings::settings::language()));
    QQmlApplicationEngine engine;
    auto* composition = new kgc::Composition(engine,
        {KGC_VERSION, KGC_HOMEPAGE_URL, ":/kgc/LICENSE", app.applicationDirPath() + "/THIRD_PARTY_NOTICES.md"}, translator);
    if (!composition->loadShell()) return 1;
    return app.exec();
}
