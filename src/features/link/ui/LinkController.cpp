// [0.1.0 T9 T14 T15 T16 T17 T20] specs/features/0.1.0-link.md
#include "LinkController.h"
#include "Constants.h"
namespace kgc::link {
LinkController::LinkController(LinkService& service)
    : QObject(&service), service_(service), baudRate_(kDefaultBaudRate)
{
    Q_ASSERT(!instance_);
    instance_ = this;
    connect(&service_, &LinkService::connectionStateChanged, this, &LinkController::connectionStateChanged);
    connect(&service_, &LinkService::portsChanged, this, &LinkController::refreshPorts);
    connect(&service_.vehicles(), &kgc::vehicle::registry::VehicleRegistry::activeChanged, this, &LinkController::rebind);
    rebind();
}
LinkController* LinkController::create(QQmlEngine* engine, QJSEngine*)
{
    Q_ASSERT(instance_ && instance_->thread() == engine->thread());
    QQmlEngine::setObjectOwnership(instance_, QQmlEngine::CppOwnership);
    return instance_;
}
void LinkController::rebind()
{
    if (vehicle_) disconnect(vehicle_->states(), nullptr, this, nullptr);
    vehicle_ = service_.vehicles().active();
    if (vehicle_) {
        using kgc::vehicle::states::VehicleStates;
        connect(vehicle_->states(), &VehicleStates::linkChanged, this, &LinkController::vehicleChanged);
        connect(vehicle_->states(), &VehicleStates::capabilitiesChanged, this, &LinkController::vehicleChanged);
    }
    emit vehicleChanged();
}
QVariantList LinkController::ports() const
{
    QVariantList result{QVariantMap{{"portName", QString{}}, {"description", QString{}}}};
    for (const auto& [name, description] : service_.ports())
        result.append(QVariantMap{{"portName", name}, {"description", description}});
    return result;
}
int LinkController::portIndex() const
{
    const auto ports = service_.ports();
    for (qsizetype i = 0; i < ports.size(); ++i) if (ports[i].first == portName_) return int(i + 1);
    return 0;
}
void LinkController::refreshPorts()
{
    if (portIndex() == 0) portName_.clear();
    emit portsChanged();
}
void LinkController::selectPort(int index)
{
    if (!service_.canConnect()) return;
    const auto ports = service_.ports();
    if (index < 0 || index > ports.size()) return;
    portName_ = index == 0 ? QString{} : ports[index - 1].first;
    emit portsChanged();
}
QVariantList LinkController::baudRates() const
{
    QVariantList result;
    for (int baud : kBaudRates) result.append(baud);
    return result;
}
void LinkController::selectBaud(int index)
{
    if (!service_.canConnect()) return;
    if (udp() || index < 0 || index >= int(kBaudRates.size())) return;
    baudRate_ = kBaudRates[index];
    emit baudRateChanged();
}
int LinkController::sysid() const { return vehicle_ ? vehicle_->sysid() : -1; }
QString LinkController::firmwareName() const { return vehicle_ ? vehicle_->states()->capabilities().firmwareName : QString{}; }
bool LinkController::lost() const { return vehicle_ && vehicle_->states()->link().lost; }
void LinkController::toggleConnection()
{
    if (service_.canConnect()) {
        using kgc::comm::links::LinkConfig;
        service_.connectLink({udp() ? LinkConfig::Kind::Udp : LinkConfig::Kind::Serial, portName_, baudRate_});
    } else service_.disconnectLink();
}
}
