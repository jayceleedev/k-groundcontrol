// [0.0.2 T2–T9 T16 T20] specs/features/0.0.2-theme.md
// [0.0.2 T24] specs/features/0.0.2-theme.md
// [0.0.4 T18 T27] specs/features/0.0.4-righttabs.md
#include <kgc/ui/UiSettingValues.h>
#include <kgc/ui/Settings.h>
#include <kgc/ui/ModuleId.h>
#include <kgc/app/log/Log.h>
#include <QFileInfo>
#include <QImageReader>
#include "Events.h"
namespace kgc::ui {
UiSettingValues::UiSettingValues(QObject* parent) : QObject(parent)
{
    settings::onBackgroundKindChanged(this, [this](auto) {
        reportBackgroundImageMissing(backgroundImage());
        emit backgroundKindChanged();
    });
    settings::onBackgroundImageChanged(this, [this](const QString& path) {
        reportBackgroundImageMissing(path);
        emit backgroundImageChanged();
    });
    settings::onHoldToConfirmMsChanged(this, [this](int) { emit holdToConfirmMsChanged(); });
    settings::onInstrumentStyleChanged(this, [this](auto) { emit instrumentStyleChanged(); });
    reportBackgroundImageMissing(backgroundImage());
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
    if (backgroundKinds().contains(value)) settings::setBackgroundKind(settings::backgroundKindFromString(value));
}
void UiSettingValues::setBackgroundImage(const QString& value)
{
    const QUrl url(value);
    if (!url.isLocalFile() && !url.scheme().isEmpty() && !QFileInfo(value).isAbsolute()) return;
    const auto path = url.isLocalFile() ? url.toLocalFile() : value;
    settings::setBackgroundImage(path.isEmpty() ? path : QFileInfo(path).absoluteFilePath());
}
void UiSettingValues::chooseBackgroundImage(const QString& path)
{
    setBackgroundImage(path);
    settings::setBackgroundKind(settings::BackgroundKind::Image);
}
void UiSettingValues::reportBackgroundImageMissing(const QString& path)
{
    if (settings::backgroundKind() != settings::BackgroundKind::Image
        || path != backgroundImage() || warnedImagePaths_.contains(path)) return;
    // 머리 정보만 멀쩡한 손상 파일도 놓치지 않도록 사진 전체를 읽어요.
    if (!path.isEmpty() && !QImageReader(path).read().isNull()) return;
    warnedImagePaths_.insert(path);
    KGC_LOG_WARN(kCategory, UiEvent::BackgroundImageMissing, "Background image not readable: %1", path);
}
// 생성 접근자가 목록 밖 저장값을 기본값 pfd로 돌려주므로 여기서 따로 거르지 않아요.
QString UiSettingValues::instrumentStyle() const { return settings::toString(settings::instrumentStyle()); }
QStringList UiSettingValues::instrumentStyles() const { return settings::instrumentStyleValues(); }
void UiSettingValues::setInstrumentStyle(const QString& value)
{
    if (instrumentStyles().contains(value)) settings::setInstrumentStyle(settings::instrumentStyleFromString(value));
}
}
