// [0.0.2 T2–T9 T16 T20] specs/features/0.0.2-theme.md
// [0.0.2 T24] specs/features/0.0.2-theme.md
#include <kgc/ui/UiSettingValues.h>
#include <kgc/ui/Settings.h>
#include <kgc/ui/ModuleId.h>
#include <kgc/app/log/Log.h>
#include <QFileInfo>
#include "Events.h"
namespace kgc::ui {
UiSettingValues::UiSettingValues(QObject* parent) : QObject(parent)
{
    settings::onBackgroundKindChanged(this, [this](auto) { emit backgroundKindChanged(); });
    settings::onBackgroundImageChanged(this, [this](const QString&) { emit backgroundImageChanged(); });
    settings::onHoldToConfirmMsChanged(this, [this](int) { emit holdToConfirmMsChanged(); });
}
int UiSettingValues::holdToConfirmMs() const { return settings::holdToConfirmMs(); }
int UiSettingValues::holdToConfirmMsMin() const { return settings::holdToConfirmMsMin(); }
int UiSettingValues::holdToConfirmMsMax() const { return settings::holdToConfirmMsMax(); }
void UiSettingValues::setHoldToConfirmMs(int value) { settings::setHoldToConfirmMs(value); }
QString UiSettingValues::backgroundKind() const { return settings::toString(settings::backgroundKind()); }
QString UiSettingValues::backgroundImage() const { return settings::backgroundImage(); }
QStringList UiSettingValues::backgroundKinds() const { return settings::backgroundKindValues(); }
QUrl UiSettingValues::backgroundImageSource() const
{
    const auto path = backgroundImage();
    return path.isEmpty() ? QUrl{} : QUrl::fromLocalFile(path);
}
void UiSettingValues::setBackgroundKind(const QString& value)
{
    if (backgroundKinds().contains(value)) settings::setBackgroundKind(settings::fromString(value));
}
void UiSettingValues::setBackgroundImage(const QString& value)
{
    const QUrl url(value);
    if (!url.isLocalFile() && !url.scheme().isEmpty() && !QFileInfo(value).isAbsolute()) return;
    const auto path = url.isLocalFile() ? url.toLocalFile() : value;
    settings::setBackgroundImage(path.isEmpty() ? path : QFileInfo(path).absoluteFilePath());
}
void UiSettingValues::reportBackgroundImageMissing(const QString& path)
{
    if (warnedImagePaths_.contains(path)) return;
    warnedImagePaths_.insert(path);
    KGC_LOG_WARN(kCategory, UiEvent::BackgroundImageMissing, "배경 사진 못 읽음: %1", path);
}
}
