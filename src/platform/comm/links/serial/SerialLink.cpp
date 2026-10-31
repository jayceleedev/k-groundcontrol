// [0.1.1 T4 T5 T13 T15 T16] specs/features/0.1.1-link-hardening.md
// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include <kgc/comm/links/SerialLink.h>
#include <kgc/comm/links/Errors.h>
#include <QSerialPort>
#include <kgc/app/log/Log.h>
#include "SerialErrors.h"
#include "Events.h"
namespace kgc::comm::links {
struct SerialLink::Private {
    LinkConfig config;
    QSerialPort* port = nullptr;
    bool opened = false;
    bool closed = false;
};
SerialLink::SerialLink(LinkConfig config) : d_(std::make_unique<Private>()) { d_->config = std::move(config); }
SerialLink::~SerialLink() = default;
void SerialLink::open()
{
    d_->port = new QSerialPort(this);
    d_->port->setPortName(d_->config.portName);
    d_->port->setBaudRate(d_->config.baudRate);
    connect(d_->port, &QSerialPort::readyRead, this, [this] { receiveBytes(d_->port->readAll()); });
    connect(d_->port, &QSerialPort::errorOccurred, this, [this](QSerialPort::SerialPortError error) {
        if (!d_->opened || d_->closed) return;
        if (serialErrorClosesLink(error)) {
            d_->closed = true;
            d_->port->close();
            emit closed(make(LinkError::PortLost));
        }
    });
    if (!d_->port->open(QIODevice::ReadWrite)) {
        auto error = LinkError::PortOpenFailed;
        if (d_->port->error() == QSerialPort::PermissionError) error = LinkError::PermissionDenied;
        if (d_->port->error() == QSerialPort::OpenError) error = LinkError::PortBusy;
        emit failed(make(error));
        return;
    }
    // 제어선을 거부하는 드라이버도 통신에는 쓸 수 있어서 연결을 계속해요.
    const auto acceptControlLine = [this](bool success) {
        if (success) return;
        const auto error = d_->port->error();
        d_->port->clearError();
        if (error != QSerialPort::UnsupportedOperationError)
            KGC_LOG_WARN(kCategory, LinksEvent::SerialErrorIgnored, "Serial error ignored: %1", int(error));
    };
    acceptControlLine(d_->port->setDataTerminalReady(true));
    acceptControlLine(d_->port->setRequestToSend(true));
    d_->opened = true;
    emit opened();
}
void SerialLink::close()
{
    if (d_->closed) return;
    d_->closed = true;
    if (d_->port) d_->port->close();
    emit closed(std::nullopt);
}
void SerialLink::writeBytes(const QByteArray& bytes)
{
    if (d_->opened && !d_->closed && d_->port->write(bytes) < 0 && !d_->closed) {
        d_->closed = true;
        d_->port->close();
        emit closed(make(LinkError::PortLost));
    }
}
}
