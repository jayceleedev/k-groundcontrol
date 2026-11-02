// [0.2.0 S3 S4] specs/features/0.2.0-fcflash.md
// [0.1.1 T4 T5 T13 T15 T16] specs/features/0.1.1-link-hardening.md
// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include <kgc/comm/links/LinkManager.h>
#include <kgc/comm/links/Errors.h>
#include <kgc/app/log/Log.h>
#include "Events.h"
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
    for (const auto& extra : d_->extraSerial) {
        if (!QFile::exists(extra)) continue;
        const bool present = std::any_of(ports.begin(), ports.end(), [&extra](const auto& port) {
            return port.first == extra;
        });
        if (!present) ports.append({extra, {}});
    }
    if (d_->open && !d_->closing && !d_->cancelled && d_->config.kind == LinkConfig::Kind::Serial) {
        const bool present = std::any_of(ports.begin(), ports.end(), [this](const auto& port) {
            return port.first == d_->config.portName;
        });
        // 독점으로 연 8250 포트는 목록에서 빠질 수 있어서 장치 파일도 확인해요.
        if (!present && !QFile::exists(d_->config.portName.startsWith('/')
                ? d_->config.portName : QStringLiteral("/dev/") + d_->config.portName)) {
            KGC_LOG_WARN(kCategory, LinksEvent::SerialPortLost, "Serial port lost: port list, port: %1", d_->config.portName);
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
PortInfo LinkManager::portInfo(QString name)
{
    for (const auto& port : QSerialPortInfo::availablePorts())
        if (port.portName() == name || port.systemLocation() == name)
            return {port.manufacturer(), port.description()};
    return {};
}
}
