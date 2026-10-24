// [0.0.1 T4–T10] specs/features/0.0.1-shell.md
#include <kgc/appSettings/AppSettingsService.h>
#include <kgc/appSettings/Settings.h>
#include <kgc/appSettings/ModuleId.h>
#include <kgc/app/i18n/Translator.h>
#include <kgc/app/log/Log.h>
#include "Events.h"
namespace kgc::appSettings {
AppSettingsService::AppSettingsService(AppInfo info, kgc::app::i18n::Translator& translator, QObject* parent)
    : QObject(parent), info_(std::move(info))
{
    settings::onLanguageChanged(this, [&translator](settings::Language language) {
        const auto previous = translator.language();
        translator.setLanguage(settings::toString(language));
        KGC_LOG_INFO(kCategory, AppSettingsEvent::LanguageChanged, "언어 변경: %1→%2", previous, translator.language());
    });
}
const AppInfo& AppSettingsService::info() const { return info_; }
}
