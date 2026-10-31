// [0.2.0 T14 T19] specs/features/0.2.0-fcflash.md
#include <kgc/comm/rawport/RawPort.h>
#include <kgc/comm/rawport/Errors.h>
namespace kgc::comm::rawport {
struct RawPort::Private {};
RawPort::RawPort() : d_(std::make_unique<Private>()) {}
RawPort::~RawPort() = default;
kgc::app::errors::Result<std::unique_ptr<RawPort>> RawPort::open(QString portName, int baud)
{
    // WORKER-FILL: spec §5.5 — 이 스레드에서 QSerialPort를 8N1, 흐름 제어 없음으로 열어요. 실패는 OpenFailed예요.
    Q_UNUSED(portName);
    Q_UNUSED(baud);
    return std::unexpected(make(PortError::OpenFailed));
}
kgc::app::errors::Result<void> RawPort::write(QByteArray bytes)
{
    // WORKER-FILL: spec §5.5 — waitForBytesWritten으로 막아 보내요. 포트 오류는 Lost예요.
    Q_UNUSED(bytes);
    return std::unexpected(make(PortError::Lost));
}
kgc::app::errors::Result<QByteArray> RawPort::read(int count, std::chrono::milliseconds timeout)
{
    // WORKER-FILL: spec §5.5 — count를 채우거나 timeout까지 waitForReadyRead로 막아요. 한 바이트도 없으면 Timeout이에요.
    Q_UNUSED(count);
    Q_UNUSED(timeout);
    return std::unexpected(make(PortError::Lost));
}
void RawPort::clearInput()
{
    // WORKER-FILL: spec §5.5 — 포트와 Qt 버퍼의 받은 바이트를 버려요.
}
bool RawPort::canControlLine() const
{
    // WORKER-FILL: spec §5.5 — 실제 시리얼 포트이므로 참이에요.
    return false;
}
kgc::app::errors::Result<void> RawPort::setBaud(int baud)
{
    // WORKER-FILL: spec §5.5 — QSerialPort::setBaudRate를 불러요.
    Q_UNUSED(baud);
    return std::unexpected(make(PortError::Lost));
}
kgc::app::errors::Result<void> RawPort::setDtrRts(bool dtr, bool rts)
{
    // WORKER-FILL: spec §5.5 — QSerialPort의 DTR·RTS를 바꿔요.
    Q_UNUSED(dtr);
    Q_UNUSED(rts);
    return std::unexpected(make(PortError::Lost));
}
}
