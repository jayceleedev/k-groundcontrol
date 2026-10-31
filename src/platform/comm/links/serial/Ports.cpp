// [0.1.1 T4 T5 T13 T15 T16] specs/features/0.1.1-link-hardening.md
// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include <kgc/comm/links/LinkManager.h>
#include <kgc/comm/links/Errors.h>
#include "LinkManagerPrivate.h"
#include <QFile>
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
    if (d_->open && !d_->closing && !d_->cancelled && d_->config.kind == LinkConfig::Kind::Serial) {
        const bool present = std::any_of(ports.begin(), ports.end(), [this](const auto& port) {
            return port.first == d_->config.portName;
        });
        // 독점으로 연 8250 포트는 목록에서 빠질 수 있어서 장치 파일도 확인해요.
        if (!present && !QFile::exists(QStringLiteral("/dev/") + d_->config.portName)) {
            d_->error = make(LinkError::PortLost);
            d_->closing = true;
            QMetaObject::invokeMethod(d_->worker, [worker = d_->worker] {
                if (worker) worker->close();
            }, Qt::QueuedConnection);
        }
    }
    if (ports == d_->ports) return;
    d_->ports = std::move(ports);
    emit portsChanged();
}
}
