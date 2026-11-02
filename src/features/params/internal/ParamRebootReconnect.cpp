// [0.3.0 6.8-2] specs/features/0.3.0-params.md
#include <kgc/params/ParamRebootReconnect.h>
#include "Constants.h"
#include "Events.h"
#include <kgc/app/log/Log.h>
#include <kgc/app/scheduler/Timeout.h>
#include <kgc/comm/links/LinkManager.h>
#include <kgc/params/ModuleId.h>
#include <kgc/vehicle/registry/VehicleRegistry.h>
namespace kgc::params {
using kgc::app::scheduler::Timeout;
ParamRebootReconnect::ParamRebootReconnect(kgc::comm::links::LinkManager& links,
                                           kgc::vehicle::registry::VehicleRegistry& vehicles, QObject* parent)
    : QObject(parent), links_(links), window_(new Timeout(QStringLiteral("paramRebootWindow"), this)),
      retry_(new Timeout(QStringLiteral("paramRebootRetry"), this))
{
    connect(window_, &Timeout::expired, this, &ParamRebootReconnect::stop);
    connect(retry_, &Timeout::expired, this, &ParamRebootReconnect::attempt);
    connect(&links, &kgc::comm::links::LinkManager::linkFailed, this, &ParamRebootReconnect::linkEnded);
    connect(&links, &kgc::comm::links::LinkManager::linkClosed, this,
            [this](int, const std::optional<kgc::app::errors::ErrorCode>& error) {
                if (error) linkEnded();
                else stop();
            });
    connect(&vehicles, &kgc::vehicle::registry::VehicleRegistry::vehicleAdded, this, &ParamRebootReconnect::stop);
}
void ParamRebootReconnect::watch(const kgc::comm::links::LinkConfig& config)
{
    config_ = config;
    watching_ = true;
    attempted_ = false;
    retry_->stop();
    window_->start(kRebootReconnectWait);
    // 재부팅 응답을 처리하기 전에 포트가 먼저 사라졌으면 끝 신호를 더 기다릴 수 없어요.
    if (links_.linkId() < 0) linkEnded();
}
void ParamRebootReconnect::linkEnded()
{
    if (watching_) retry_->start(attempted_ ? kRebootRetry : kRebootOpenDelay);
}
void ParamRebootReconnect::attempt()
{
    // 사용자가 먼저 연결했거나 이전 시도가 열려 있으면 그 링크의 결과를 기다려요.
    if (!watching_ || links_.linkId() >= 0) return;
    attempted_ = true;
    KGC_LOG_INFO(kCategory, ParamEvent::RebootReconnect, "Reconnecting after reboot: %1", config_.portName);
    if (!links_.connectLink(config_)) retry_->start(kRebootRetry);
}
void ParamRebootReconnect::stop()
{
    watching_ = false;
    window_->stop();
    retry_->stop();
}
}
