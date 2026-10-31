// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include <kgc/comm/links/LinkManager.h>
#include "LinkManagerPrivate.h"
#include <QSerialPortInfo>
#include <algorithm>
namespace kgc::comm::links {
void LinkManager::refreshPorts()
{
    QList<QPair<QString, QString>> ports;
    for (const auto& port : QSerialPortInfo::availablePorts()) ports.append({port.portName(), port.description()});
    std::sort(ports.begin(), ports.end());
    if (!d_->extraSerial.isEmpty()) {
        const bool present = std::any_of(ports.begin(), ports.end(), [this](const auto& port) {
            return port.first == d_->extraSerial;
        });
        if (!present) ports.append({d_->extraSerial, {}});
    }
    if (ports == d_->ports) return;
    d_->ports = std::move(ports);
    emit portsChanged();
}
}
