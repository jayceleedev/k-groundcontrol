// [0.3.0 F5] specs/features/0.3.0-params.md
// [0.1.1 T4 T5 T13 T15 T16] specs/features/0.1.1-link-hardening.md
// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include <kgc/comm/links/SerialLink.h>
#include <kgc/comm/links/Errors.h>
#include <QElapsedTimer>
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
    bool busyLogged = false;
    QElapsedTimer sinceOpen;
    qint64 reads = 0;
    qint64 lastRead = 0;
    qint64 writes = 0;
    // 열린 직후 끊김의 원인을 가리려고 포트를 닫기 전의 상태를 글로 모아요.
    QString portState() const
    {
        return QStringLiteral("error: %1, text: %2, after open: %3 ms, reads: %4, last read: %5, writes: %6, unwritten: %7, unread: %8")
            .arg(int(port->error())).arg(port->errorString()).arg(sinceOpen.elapsed()).arg(reads).arg(lastRead)
            .arg(writes).arg(port->bytesToWrite()).arg(port->bytesAvailable());
    }
};
SerialLink::SerialLink(LinkConfig config) : d_(std::make_unique<Private>()) { d_->config = std::move(config); }
SerialLink::~SerialLink() = default;
void SerialLink::open()
{
    d_->port = new QSerialPort(this);
    d_->port->setPortName(d_->config.portName);
    d_->port->setBaudRate(d_->config.baudRate);
    connect(d_->port, &QSerialPort::readyRead, this, [this] {
        const auto bytes = d_->port->readAll();
        ++d_->reads;
        d_->lastRead = bytes.size();
        receiveBytes(bytes);
    });
    connect(d_->port, &QSerialPort::errorOccurred, this, [this](QSerialPort::SerialPortError error) {
        if (!d_->opened || d_->closed) return;
        if (serialWriteBusy(error, d_->port->errorString())) {
            // 포트가 잠시 바쁜 것뿐이라 링크를 살려 둬요. 시끄럽지 않게 링크마다 한 번만 남겨요.
            if (!d_->busyLogged)
                KGC_LOG_INFO(kCategory, LinksEvent::SerialWriteBusy, "Serial write busy, keeping link: %1", d_->portState());
            d_->busyLogged = true;
            d_->port->clearError();
            return;
        }
        if (serialErrorClosesLink(error, d_->port->errorString())) {
            d_->closed = true;
            KGC_LOG_WARN(kCategory, LinksEvent::SerialPortLost, "Serial port lost: errorOccurred, %1", d_->portState());
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
    d_->sinceOpen.start();
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
    if (!d_->opened || d_->closed) return;
    const auto written = d_->port->write(bytes);
    ++d_->writes;
    if (written < 0 && !d_->closed) {
        d_->closed = true;
        KGC_LOG_WARN(kCategory, LinksEvent::SerialPortLost, "Serial port lost: write, returned: %1, %2", written, d_->portState());
        d_->port->close();
        emit closed(make(LinkError::PortLost));
    }
}
}
