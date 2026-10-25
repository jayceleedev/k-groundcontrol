// [0.0.2 T2–T9 T16 T20] specs/features/0.0.2-theme.md
#include <kgc/ui/UiSettingValues.h>
#include <kgc/ui/Settings.h>
namespace kgc::ui {
UiSettingValues::UiSettingValues(QObject* parent) : QObject(parent)
{
    settings::onHoldToConfirmMsChanged(this, [this](int) { emit holdToConfirmMsChanged(); });
}
int UiSettingValues::holdToConfirmMs() const { return settings::holdToConfirmMs(); }
int UiSettingValues::holdToConfirmMsMin() const { return settings::holdToConfirmMsMin(); }
int UiSettingValues::holdToConfirmMsMax() const { return settings::holdToConfirmMsMax(); }
void UiSettingValues::setHoldToConfirmMs(int value) { settings::setHoldToConfirmMs(value); }
}
