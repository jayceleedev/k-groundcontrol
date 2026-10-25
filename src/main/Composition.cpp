// [0.0.1 T1–T10] specs/features/0.0.1-shell.md
#include "Composition.h"
#include <kgc/ui/UiRegistry.h>
#include <kgc/ui/ThemeSwitcher.h>
#include <kgc/ui/UiSettingValues.h>
#include <kgc/shell/ShellService.h>
#include <kgc/shell/Ui.h>
#include <kgc/appSettings/AppSettingsService.h>
#include <kgc/appSettings/Ui.h>
#include <kgc/app/i18n/Translator.h>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQuickWindow>
#include <QtQml/QQmlExtensionPlugin>

Q_IMPORT_QML_PLUGIN(Kgc_UiPlugin)
Q_IMPORT_QML_PLUGIN(Kgc_ShellPlugin)
Q_IMPORT_QML_PLUGIN(Kgc_AppSettingsPlugin)

// 정적 라이브러리의 자원을 링커가 버리지 않도록 전역에서 참조해요.
static void initKgcResources() { Q_INIT_RESOURCE(kgc); }

namespace kgc {
Composition::Composition(QQmlEngine& engine, appSettings::AppInfo info, app::i18n::Translator& translator)
    : QObject(&engine), engine_(engine)
{
    initKgcResources();
    auto* panels = new ui::PanelRegistry(this);
    auto* topbar = new ui::TopbarSlot(this);
    auto* statusChips = new ui::StatusChipSlot(this);
    auto* rightTabs = new ui::RightTabSlot(this);
    auto* mapLayers = new ui::MapLayerSlot(this);
    auto* navigator = new ui::Navigator(this);
    auto* context = engine.rootContext();
    context->setContextProperty("panelRegistry", panels);
    context->setContextProperty("topbarSlot", topbar);
    context->setContextProperty("statusChipSlot", statusChips);
    context->setContextProperty("rightTabSlot", rightTabs);
    context->setContextProperty("mapLayerSlot", mapLayers);
    context->setContextProperty("navigator", navigator);
    context->setContextProperty("translator", &translator);
    context->setContextProperty("themeSwitcher", new ui::ThemeSwitcher(engine));
    context->setContextProperty("uiSettingValues", new ui::UiSettingValues(this));
    ui::UiRegistry registry{*panels, *topbar, *statusChips, *rightTabs, *mapLayers, *navigator, engine};
    auto* shellService = new shell::ShellService(translator, this);
    auto* appSettingsService = new appSettings::AppSettingsService(std::move(info), translator, this);
    shell::registerUi(registry, *shellService);
    appSettings::registerUi(registry, *appSettingsService);
    const auto registerPlaceholder = [panels](const char* id, int order, int group,
                                               const char* titleKey, const QUrl& icon) {
        const auto result = panels->registerPlaceholder(id, order, group, titleKey, icon);
        Q_ASSERT(result.has_value());
    };
    registerPlaceholder("fcFlash", 1, 1, QT_TRANSLATE_NOOP("Menu", "펌웨어 세팅"), QUrl("qrc:/kgc/icons/menu/fcFlash.svg"));
    registerPlaceholder("layout", 2, 1, QT_TRANSLATE_NOOP("Menu", "기체 구성도"), QUrl("qrc:/kgc/icons/menu/layout.svg"));
    registerPlaceholder("sensors", 3, 1, QT_TRANSLATE_NOOP("Menu", "센서 보정"), QUrl("qrc:/kgc/icons/menu/sensors.svg"));
    registerPlaceholder("radio", 4, 1, QT_TRANSLATE_NOOP("Menu", "조종기"), QUrl("qrc:/kgc/icons/menu/radio.svg"));
    registerPlaceholder("flightModes", 5, 1, QT_TRANSLATE_NOOP("Menu", "비행 모드"), QUrl("qrc:/kgc/icons/menu/flightModes.svg"));
    registerPlaceholder("power", 6, 1, QT_TRANSLATE_NOOP("Menu", "배터리 · 전원"), QUrl("qrc:/kgc/icons/menu/power.svg"));
    registerPlaceholder("failsafe", 7, 1, QT_TRANSLATE_NOOP("Menu", "안전 · 페일세이프"), QUrl("qrc:/kgc/icons/menu/failsafe.svg"));
    registerPlaceholder("motors", 8, 1, QT_TRANSLATE_NOOP("Menu", "모터 시험"), QUrl("qrc:/kgc/icons/menu/motors.svg"));
    registerPlaceholder("tuning", 9, 1, QT_TRANSLATE_NOOP("Menu", "튜닝"), QUrl("qrc:/kgc/icons/menu/tuning.svg"));
    registerPlaceholder("logs", 10, 1, QT_TRANSLATE_NOOP("Menu", "로그 분석"), QUrl("qrc:/kgc/icons/menu/logs.svg"));
    registerPlaceholder("esc", 11, 2, QT_TRANSLATE_NOOP("Menu", "ESC 설정"), QUrl("qrc:/kgc/icons/menu/esc.svg"));
    registerPlaceholder("elrs", 12, 2, QT_TRANSLATE_NOOP("Menu", "ELRS 설정"), QUrl("qrc:/kgc/icons/menu/elrs.svg"));
    registerPlaceholder("tracker", 13, 2, QT_TRANSLATE_NOOP("Menu", "안테나 트래커"), QUrl("qrc:/kgc/icons/menu/tracker.svg"));
    registerPlaceholder("params", 14, 3, QT_TRANSLATE_NOOP("Menu", "수동 기체 설정"), QUrl("qrc:/kgc/icons/menu/params.svg"));
    connect(&translator, &app::i18n::Translator::languageChanged, &engine, &QQmlEngine::retranslate);
}
QQuickWindow* Composition::loadShell()
{
    QQmlComponent component(&engine_, QUrl("qrc:/qt/qml/Kgc/Shell/Shell.qml"));
    auto* object = component.create();
    auto* window = qobject_cast<QQuickWindow*>(object);
    if (!window) { delete object; return nullptr; }
    window->QObject::setParent(&engine_);
    window->show();
    return window;
}
}
