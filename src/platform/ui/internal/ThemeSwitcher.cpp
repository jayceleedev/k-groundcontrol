// [0.0.2 T2–T9 T16 T20] specs/features/0.0.2-theme.md
#include <kgc/ui/ThemeSwitcher.h>
#include <kgc/ui/ThemeChoice.h>
#include <kgc/ui/Settings.h>
#include <kgc/ui/ModuleId.h>
#include <kgc/app/log/Log.h>
#include "Events.h"
#include <QDir>
#include <QGuiApplication>
#include <QStyleHints>
#include <QQmlComponent>
#include <QQmlEngine>
#include <algorithm>
namespace kgc::ui {
ThemeSwitcher::ThemeSwitcher(QQmlEngine& engine) : QObject(&engine), engine_(engine)
{
    const QDir directory(":/qt/qml/Kgc/Ui/themes");
    for (const auto& file : directory.entryList({"*.qml"}, QDir::Files)) {
        if (file == "ThemeBase.qml") continue;
        auto id = file.chopped(4);
        id[0] = id[0].toLower();
        const QUrl source("qrc:/qt/qml/Kgc/Ui/themes/" + file);
        QQmlComponent component(&engine_, source);
        auto* object = component.create();
        Q_ASSERT_X(object, "ThemeSwitcher", qPrintable(component.errorString()));
        QVariantMap entry{{"id", id}};
        for (const auto* name : {"order", "titleKey", "dark", "glass"}) entry[name] = object->property(name);
        themes_.append(entry);
        sources_[id] = source;
        delete object;
    }
    std::sort(themes_.begin(), themes_.end(), [](const QVariant& a, const QVariant& b) {
        const auto left = a.toMap(), right = b.toMap();
        return left["order"] == right["order"] ? left["id"].toString() < right["id"].toString()
            : left["order"].toInt() < right["order"].toInt();
    });
    settings::onThemeChanged(this, [this](const QString&) { emit selectedChanged(); apply(); });
    connect(qGuiApp->styleHints(), &QStyleHints::colorSchemeChanged, this, [this] { apply(); });
    apply();
}
QString ThemeSwitcher::themeId() const { return themeId_; }
QVariantList ThemeSwitcher::themes() const { return themes_; }
int ThemeSwitcher::currentIndex() const
{
    for (qsizetype index = 0; index < themes_.size(); ++index)
        if (themes_[index].toMap()["id"].toString() == themeId_) return static_cast<int>(index);
    return -1;
}
QString ThemeSwitcher::selected() const { return settings::theme(); }
void ThemeSwitcher::choose(const QString& id) { settings::setTheme(id); }
void ThemeSwitcher::apply()
{
    const auto choice = resolveTheme(settings::theme(), qGuiApp->styleHints()->colorScheme(), sources_.keys());
    if (choice.reason == ThemeReason::Unknown && !warnedSelections_.contains(settings::theme())) {
        warnedSelections_.insert(settings::theme());
        KGC_LOG_WARN(kCategory, UiEvent::ThemeUnknown, "테마 없음: %1, 시스템 따라감", settings::theme());
    }
    if (choice.id == themeId_) return;
    QQmlComponent component(&engine_, sources_.value(choice.id));
    auto* next = component.create();
    Q_ASSERT_X(next, "ThemeSwitcher", qPrintable(component.errorString()));
    next->setParent(this);
    auto* theme = engine_.singletonInstance<QObject*>("Kgc.Ui", "Theme");
    Q_ASSERT(theme);
    const bool changed = theme->setProperty("current", QVariant::fromValue(next));
    Q_ASSERT(changed);
    if (current_) current_->deleteLater();
    current_ = next;
    themeId_ = choice.id;
    emit themeIdChanged();
    KGC_LOG_INFO(kCategory, UiEvent::ThemeApplied, "테마 적용: %1, 이유: %2", themeId_,
        choice.reason == ThemeReason::Setting ? "설정" : "시스템");
}
}
