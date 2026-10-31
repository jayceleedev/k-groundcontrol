// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include <kgc/comm/links/SerialLink.h>
#include <kgc/comm/links/Errors.h>
#include <QSerialPort>
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
        if (!d_->opened || d_->closed || error == QSerialPort::NoError
            || error == QSerialPort::TimeoutError) return;
        d_->closed = true;
        d_->port->close();
        emit closed(make(LinkError::PortLost));
    });
    if (!d_->port->open(QIODevice::ReadWrite)) {
        auto error = LinkError::PortOpenFailed;
        if (d_->port->error() == QSerialPort::PermissionError) error = LinkError::PermissionDenied;
        if (d_->port->error() == QSerialPort::OpenError) error = LinkError::PortBusy;
        emit failed(make(error));
        return;
    }
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
