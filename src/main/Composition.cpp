// [0.0.3 T1 T13] specs/features/0.0.3-windows.md
// [0.0.1 T1–T10 T17] specs/features/0.0.1-shell.md
#include "Composition.h"
#include <kgc/ui/UiRegistry.h>
#include <kgc/ui/ThemeSwitcher.h>
#include <kgc/ui/UiSettingValues.h>
#include <kgc/shell/ShellService.h>
#include <kgc/shell/Ui.h>
#include <kgc/appSettings/AppSettingsService.h>
#include <kgc/appSettings/Ui.h>
#include <kgc/uiGallery/UiGalleryService.h>
#include <kgc/uiGallery/Ui.h>
#include <kgc/app/i18n/Translator.h>
#include <kgc/analyze/AppLogService.h>
#include <kgc/analyze/Ui.h>
#include <kgc/secondPanel/SecondPanelService.h>
#include <kgc/secondPanel/Ui.h>
#include <QCoreApplication>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQuickWindow>
#include <QtQml/QQmlExtensionPlugin>

Q_IMPORT_QML_PLUGIN(Kgc_UiPlugin)
Q_IMPORT_QML_PLUGIN(Kgc_ShellPlugin)
Q_IMPORT_QML_PLUGIN(Kgc_AppSettingsPlugin)
Q_IMPORT_QML_PLUGIN(Kgc_UiGalleryPlugin)
Q_IMPORT_QML_PLUGIN(Kgc_AnalyzePlugin)
Q_IMPORT_QML_PLUGIN(Kgc_SecondPanelPlugin)

// 정적 라이브러리의 자원을 링커가 버리지 않도록 전역에서 참조해요.
static void initKgcResources() { Q_INIT_RESOURCE(kgc); Q_INIT_RESOURCE(tabler_icons); }

namespace kgc {
namespace {
void registerPlaceholders(ui::PanelRegistry* panels)
{
    const auto registerPlaceholder = [panels](const char* id, int order, int group,
                                               const char* titleKey, const QUrl& icon) {
        const auto result = panels->registerPlaceholder(id, order, group, titleKey, icon);
        Q_ASSERT(result.has_value());
    };
    registerPlaceholder("fcFlash", 1, 1, QT_TRANSLATE_NOOP("Menu", "펌웨어 세팅"), QUrl("qrc:/kgc/icons/menu/fcFlash.svg"));
    registerPlaceholder("layout", 2, 2, QT_TRANSLATE_NOOP("Menu", "기체 구성도"), QUrl("qrc:/kgc/icons/menu/layout.svg"));
    registerPlaceholder("sensors", 3, 2, QT_TRANSLATE_NOOP("Menu", "센서 보정"), QUrl("qrc:/kgc/icons/menu/sensors.svg"));
    registerPlaceholder("radio", 4, 2, QT_TRANSLATE_NOOP("Menu", "조종기"), QUrl("qrc:/kgc/icons/menu/radio.svg"));
    registerPlaceholder("flightModes", 5, 2, QT_TRANSLATE_NOOP("Menu", "비행 모드"), QUrl("qrc:/kgc/icons/menu/flightModes.svg"));
    registerPlaceholder("power", 6, 2, QT_TRANSLATE_NOOP("Menu", "배터리 · 전원"), QUrl("qrc:/kgc/icons/menu/power.svg"));
    registerPlaceholder("failsafe", 7, 2, QT_TRANSLATE_NOOP("Menu", "안전 · 페일세이프"), QUrl("qrc:/kgc/icons/menu/failsafe.svg"));
    registerPlaceholder("motors", 8, 2, QT_TRANSLATE_NOOP("Menu", "모터 시험"), QUrl("qrc:/kgc/icons/menu/motors.svg"));
    registerPlaceholder("tuning", 9, 2, QT_TRANSLATE_NOOP("Menu", "튜닝"), QUrl("qrc:/kgc/icons/menu/tuning.svg"));
    registerPlaceholder("logs", 10, 2, QT_TRANSLATE_NOOP("Menu", "로그 분석"), QUrl("qrc:/kgc/icons/menu/logs.svg"));
    registerPlaceholder("esc", 11, 3, QT_TRANSLATE_NOOP("Menu", "ESC 설정"), QUrl("qrc:/kgc/icons/menu/esc.svg"));
    registerPlaceholder("elrs", 12, 3, QT_TRANSLATE_NOOP("Menu", "ELRS 설정"), QUrl("qrc:/kgc/icons/menu/elrs.svg"));
    registerPlaceholder("tracker", 13, 3, QT_TRANSLATE_NOOP("Menu", "안테나 트래커"), QUrl("qrc:/kgc/icons/menu/tracker.svg"));
    registerPlaceholder("params", 14, 4, QT_TRANSLATE_NOOP("Menu", "수동 기체 설정"), QUrl("qrc:/kgc/icons/menu/params.svg"));
}
// 시안(specs/ui/main-screen.html)의 순서대로 상단바·하단바 자리를 잡아 두고, 기능이 생기면 같은 id·순서로 바꿔 넣어요.
void registerBarPlaceholders(ui::TopbarSlot* topbar, ui::StatusChipSlot* chips)
{
    const auto top = [topbar](const char* id, int order, const char* area, int group, const char* labelKey,
                              const char* icon, bool iconOnly = false) {
        const auto result = topbar->registerPlaceholder(id, order, area, group, labelKey, icon, iconOnly);
        Q_ASSERT(result.has_value());
    };
    top("fly.vehicle", 1, "centre", 1, QT_TRANSLATE_NOOP("Bar", "기체"), "");
    top("fly.mode", 2, "centre", 2, QT_TRANSLATE_NOOP("Bar", "모드"), "");
    top("fly.arm", 3, "centre", 2, QT_TRANSLATE_NOOP("Bar", "ARM"), "");
    top("fly.takeoff", 4, "centre", 3, QT_TRANSLATE_NOOP("Bar", "이륙"), "plane-departure");
    top("fly.land", 5, "centre", 3, QT_TRANSLATE_NOOP("Bar", "착륙"), "plane-arrival");
    top("fly.rtl", 6, "centre", 3, QT_TRANSLATE_NOOP("Bar", "RTL"), "home");
    top("fly.pause", 7, "centre", 3, QT_TRANSLATE_NOOP("Bar", "일시정지"), "player-pause");
    top("mission.select", 8, "centre", 4, QT_TRANSLATE_NOOP("Bar", "미션"), "");
    top("mission.start", 9, "centre", 4, QT_TRANSLATE_NOOP("Bar", "미션 시작"), "player-play");
    top("mission.progress", 10, "centre", 4, QT_TRANSLATE_NOOP("Bar", "WP"), "");
    top("fly.emergencyStop", 11, "right", 5, QT_TRANSLATE_NOOP("Bar", "비상 정지"), "hand-stop");
    top("notify.bell", 12, "right", 5, QT_TRANSLATE_NOOP("Bar", "알림"), "bell", true);
    top("ai.panel", 13, "right", 5, QT_TRANSLATE_NOOP("Bar", "AI"), "sparkles", true);
    const auto chip = [chips](const char* id, int order, const char* side, const char* labelKey) {
        const auto result = chips->registerPlaceholder(id, order, side, labelKey);
        Q_ASSERT(result.has_value());
    };
    chip("link.connection", 1, "left", QT_TRANSLATE_NOOP("Bar", "기체 연결"));
    chip("vehicleInfo.gps", 2, "left", QT_TRANSLATE_NOOP("Bar", "GPS"));
    chip("vehicleInfo.compass", 3, "left", QT_TRANSLATE_NOOP("Bar", "나침반"));
    chip("vehicleInfo.imu", 4, "left", QT_TRANSLATE_NOOP("Bar", "IMU"));
    chip("vehicleInfo.baro", 5, "left", QT_TRANSLATE_NOOP("Bar", "기압계"));
    chip("vehicleInfo.rc", 6, "left", QT_TRANSLATE_NOOP("Bar", "RC"));
    chip("vehicleInfo.telemetry", 7, "left", QT_TRANSLATE_NOOP("Bar", "TLM"));
    chip("vehicleInfo.battery", 8, "left", QT_TRANSLATE_NOOP("Bar", "배터리"));
    chip("tracker.status", 9, "right", QT_TRANSLATE_NOOP("Bar", "트래커"));
    chip("gcsposition.gps", 10, "right", QT_TRANSLATE_NOOP("Bar", "GCS GPS"));
}
}
Composition::Composition(QQmlEngine& engine, appSettings::AppInfo info, app::i18n::Translator& translator,
                         app::log::LogFeed& feed)
    : QObject(&engine), engine_(engine)
{
    initKgcResources();
    auto* panels = new ui::PanelRegistry(this);
    auto* topbar = new ui::TopbarSlot(this);
    auto* statusChips = new ui::StatusChipSlot(this);
    auto* rightTabs = new ui::RightTabSlot(this);
    auto* mapLayers = new ui::MapLayerSlot(this);
    auto* bottomActions = new ui::BottomActionSlot(this);
    auto* navigator = new ui::Navigator(this);
    auto* context = engine.rootContext();
    context->setContextProperty("panelRegistry", panels);
    context->setContextProperty("topbarSlot", topbar);
    context->setContextProperty("statusChipSlot", statusChips);
    context->setContextProperty("rightTabSlot", rightTabs);
    context->setContextProperty("mapLayerSlot", mapLayers);
    context->setContextProperty("navigator", navigator);
    context->setContextProperty("bottomActionSlot", bottomActions);
    context->setContextProperty("translator", &translator);
    context->setContextProperty("themeSwitcher", new ui::ThemeSwitcher(engine));
    context->setContextProperty("uiSettingValues", new ui::UiSettingValues(this));
    ui::UiRegistry registry{*panels, *topbar, *statusChips, *rightTabs, *mapLayers, *bottomActions, *navigator, engine};
    auto* shellService = new shell::ShellService(translator, this);
    auto* appSettingsService = new appSettings::AppSettingsService(std::move(info), translator, this);
    shell::registerUi(registry, *shellService);
    appSettings::registerUi(registry, *appSettingsService);
    auto* galleryService = new uiGallery::UiGalleryService(this);
    uiGallery::registerUi(registry, *galleryService);
    auto* appLogService = new analyze::AppLogService(feed, this);
    analyze::registerUi(registry, *appLogService);
    auto* secondPanelService = new secondPanel::SecondPanelService(this);
    secondPanel::registerUi(registry, *secondPanelService);
    registerPlaceholders(panels);
    registerBarPlaceholders(topbar, statusChips);
    connect(&translator, &app::i18n::Translator::languageChanged, &engine, &QQmlEngine::retranslate);
}
QQuickWindow* Composition::loadShell()
{
    QQmlComponent component(&engine_, QUrl("qrc:/qt/qml/Kgc/Shell/Shell.qml"));
    auto* object = component.create();
    auto* window = qobject_cast<QQuickWindow*>(object);
    if (!window) { delete object; return nullptr; }
    window->QObject::setParent(&engine_);
    connect(window, &QQuickWindow::closing, qApp, &QCoreApplication::quit);
    window->show();
    return window;
}
// 기능 api를 아는 곳은 조립 하나라서, main은 이 함수로만 경고 출처를 받아요.
QStringList Composition::logGuardPrefixes()
{
    return analyze::AppLogService::loopGuardPrefixes();
}
}
