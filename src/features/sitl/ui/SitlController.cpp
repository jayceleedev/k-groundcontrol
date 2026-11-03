// [0.4.0 5.4 6.8] specs/features/0.4.0-sitl.md
#include "SitlController.h"
#include <kgc/sitl/Errors.h>
#include <kgc/sitl/Settings.h>
#include "SitlCommand.h"
#include <QCoreApplication>
namespace kgc::sitl {
SitlController::SitlController(SitlService& service)
    : QObject(&service), service_(service),
      vehicle_(sitlVehicleFromSetting(settings::vehicle())), release_(sitlReleaseFromSetting(settings::release())),
      home_{settings::homeLatitude(), settings::homeLongitude(), settings::homeAltitude(), settings::homeHeading()},
      speedup_(settings::speedup())
{
    Q_ASSERT(!instance_);
    instance_ = this;
    connect(&service_, &SitlService::stateChanged, this, [this] {
        emit stateChanged();
        emit canStartChanged();
    });
    connect(&service_, &SitlService::jobChanged, this, &SitlController::jobChanged);
    connect(&service_, &SitlService::outputChanged, this, &SitlController::outputChanged);
    connect(&service_, &SitlService::cacheChanged, this, &SitlController::cacheChanged);
    const auto link = [this] {
        emit linkChanged();
        emit canStartChanged();
    };
    connect(&service_, &SitlService::linkOpenedBySitlChanged, this, link);
    connect(&service_, &SitlService::linkBusyChanged, this, link);
}
SitlController* SitlController::create(QQmlEngine* engine, QJSEngine*)
{
    Q_ASSERT(instance_ && instance_->thread() == engine->thread());
    QQmlEngine::setObjectOwnership(instance_, QQmlEngine::CppOwnership);
    return instance_;
}
bool SitlController::editable() const
{
    return state() == SitlState::Idle || state() == SitlState::Failed;
}
int SitlController::speedupMin() const { return settings::speedupMin(); }
int SitlController::speedupMax() const { return settings::speedupMax(); }
void SitlController::changeInputs()
{
    emit inputsChanged();
    emit canStartChanged();
}
void SitlController::setHomeLatitude(double value) { home_.latitude = value; changeInputs(); }
void SitlController::setHomeLongitude(double value) { home_.longitude = value; changeInputs(); }
void SitlController::setHomeAltitude(double value) { home_.altitude = value; changeInputs(); }
void SitlController::setHomeHeading(int value) { home_.heading = value; changeInputs(); }
void SitlController::setSpeedup(int value) { speedup_ = value; changeInputs(); }
void SitlController::setWipe(bool value) { wipe_ = value; changeInputs(); }
QVariantList SitlController::badHomeFields() const
{
    QVariantList fields;
    for (const auto field : checkHome(home_)) fields.append(int(field));
    return fields;
}
bool SitlController::canStart() const
{
    return editable() && checkHome(home_).isEmpty() && !linkBusy()
        && speedup_ >= settings::speedupMin() && speedup_ <= settings::speedupMax();
}
QString SitlController::cachedAt() const
{
    const auto time = service_.cachedAt(vehicle_, release_);
    return time.isValid() && service_.cached(vehicle_, release_) ? time.toString(QStringLiteral("yyyy-MM-dd HH:mm")) : QString{};
}
QString SitlController::badHomeText() const { return errorText(make(SitlError::BadHome)); }
QString SitlController::failureText() const
{
    const auto error = service_.lastError();
    if (!error) return {};
    auto text = errorText(*error);
    if (*error == make(SitlError::Crashed) && service_.lastExitCode() >= 0)
        text += QStringLiteral(" (") + QCoreApplication::translate("Sitl", QT_TRANSLATE_NOOP("Sitl", "종료 코드 %1"))
            .arg(service_.lastExitCode()) + QLatin1Char(')');
    return text;
}
void SitlController::chooseVehicle(int value)
{
    if (!editable() || value < int(SitlVehicle::Copter) || value > int(SitlVehicle::Rover)) return;
    vehicle_ = SitlVehicle(value);
    changeInputs();
    emit cacheChanged();
}
void SitlController::chooseRelease(int value)
{
    if (!editable() || value < int(SitlRelease::Stable) || value > int(SitlRelease::Latest)) return;
    release_ = SitlRelease(value);
    changeInputs();
    emit cacheChanged();
}
void SitlController::resetHome()
{
    if (!editable()) return;
    home_ = {settings::homeLatitudeDefault(), settings::homeLongitudeDefault(), settings::homeAltitudeDefault(),
             settings::homeHeadingDefault()};
    changeInputs();
}
void SitlController::startOrStop()
{
    if (state() == SitlState::Starting || state() == SitlState::Running) service_.stop();
    else static_cast<void>(service_.start(vehicle_, release_, home_, speedup_, wipe_));
}
void SitlController::redownload() { static_cast<void>(service_.redownload(vehicle_, release_)); }
void SitlController::reconnect() { static_cast<void>(service_.reconnect()); }
}
