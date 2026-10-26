// [0.0.1 T4–T10] specs/features/0.0.1-shell.md
#include "AppSettingsController.h"
#include <kgc/appSettings/AppSettingsService.h>
#include <kgc/appSettings/Settings.h>
#include <QFile>
#include <QLocale>
namespace kgc::appSettings {
AppSettingsController::AppSettingsController(AppSettingsService& service) : QObject(&service), service_(service)
{
    settings::onLanguageChanged(this, [this](settings::Language) { emit languageChanged(); });
}
QString AppSettingsController::version() const { return service_.info().version; }
QString AppSettingsController::sourceUrl() const { return service_.info().homepageUrl; }
QString AppSettingsController::licenseText() const
{
    QFile file(service_.info().licensePath);
    return file.open(QIODevice::ReadOnly) ? QString::fromUtf8(file.readAll()) : QString{};
}
QString AppSettingsController::noticesText() const
{
    QFile file(service_.info().noticesPath);
    return file.open(QIODevice::ReadOnly) ? QString::fromUtf8(file.readAll()) : QString{};
}
QStringList AppSettingsController::languages() const
{
    return settings::languageValues();
}
QString AppSettingsController::language() const { return settings::toString(settings::language()); }
QString AppSettingsController::languageName(const QString& code) const { return QLocale(code).nativeLanguageName(); }
void AppSettingsController::setLanguage(const QString& code)
{
    if (languages().contains(code)) settings::setLanguage(settings::languageFromString(code));
}
}
