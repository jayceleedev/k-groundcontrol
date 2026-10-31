// [0.2.0 T14 T19] specs/features/0.2.0-fcflash.md
#include <kgc/comm/rawport/RawPort.h>
#include <kgc/comm/rawport/Errors.h>
#include <QSerialPort>
#include <QElapsedTimer>
#include <algorithm>
#include <limits>
namespace kgc::comm::rawport {
struct RawPort::Private { QSerialPort port; };
RawPort::RawPort() : d_(std::make_unique<Private>()) {}
RawPort::~RawPort() = default;
kgc::app::errors::Result<std::unique_ptr<RawPort>> RawPort::open(QString portName, int baud)
{
    auto raw = std::unique_ptr<RawPort>(new RawPort);
    auto& port = raw->d_->port;
    port.setPortName(portName);
    if (!port.setBaudRate(baud) || !port.setDataBits(QSerialPort::Data8)
        || !port.setParity(QSerialPort::NoParity) || !port.setStopBits(QSerialPort::OneStop)
        || !port.setFlowControl(QSerialPort::NoFlowControl) || !port.open(QIODevice::ReadWrite))
        return std::unexpected(make(PortError::OpenFailed));
    return raw;
}
kgc::app::errors::Result<void> RawPort::write(QByteArray bytes)
{
    auto& port = d_->port;
    if (!port.isOpen()) return std::unexpected(make(PortError::Lost));
    if (port.write(bytes) != bytes.size()) return std::unexpected(make(PortError::Lost));
    while (port.bytesToWrite() > 0) {
        if (!port.waitForBytesWritten())
            return std::unexpected(make(port.error() == QSerialPort::TimeoutError ? PortError::Timeout : PortError::Lost));
    }
    return {};
}
kgc::app::errors::Result<QByteArray> RawPort::read(int count, std::chrono::milliseconds timeout)
{
    auto& port = d_->port;
    QByteArray bytes;
    QElapsedTimer clock;
    clock.start();
    if (!port.isOpen()) return std::unexpected(make(PortError::Lost));
    while (bytes.size() < count) {
        bytes += port.read(count - bytes.size());
        if (port.error() != QSerialPort::NoError && port.error() != QSerialPort::TimeoutError)
            return std::unexpected(make(PortError::Lost));
        if (bytes.size() == count) break;
        const auto remaining = timeout.count() - clock.elapsed();
        if (remaining <= 0) break;
        const auto wait = static_cast<int>(std::min<qint64>(remaining, std::numeric_limits<int>::max()));
        if (!port.waitForReadyRead(wait)) {
            if (port.error() != QSerialPort::TimeoutError && port.error() != QSerialPort::NoError)
                return std::unexpected(make(PortError::Lost));
            break;
        }
    }
    if (bytes.isEmpty() && count > 0) return std::unexpected(make(PortError::Timeout));
    return bytes;
}
void RawPort::clearInput() { d_->port.clear(QSerialPort::Input); }
bool RawPort::canControlLine() const { return true; }
kgc::app::errors::Result<void> RawPort::setBaud(int baud)
{
    if (!d_->port.setBaudRate(baud)) return std::unexpected(make(PortError::Lost));
    return {};
}
kgc::app::errors::Result<void> RawPort::setDtrRts(bool dtr, bool rts)
{
    if (!d_->port.setDataTerminalReady(dtr) || !d_->port.setRequestToSend(rts))
        return std::unexpected(make(PortError::Lost));
    return {};
}
}
