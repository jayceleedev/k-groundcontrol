// [0.3.0 F5] specs/features/0.3.0-params.md
// [0.1.1 T5] specs/features/0.1.1-link-hardening.md
#include "SerialErrors.h"
#include <cerrno>
namespace kgc::comm::links {
bool serialWriteBusy(QSerialPort::SerialPortError error, const QString& text)
{
    // Qt가 같은 함수로 글을 만들어서 언어와 상관없이 비교할 수 있어요.
    return error == QSerialPort::ResourceError && text == qt_error_string(EAGAIN);
}
bool serialErrorClosesLink(QSerialPort::SerialPortError error, const QString& text)
{
    return error != QSerialPort::NoError && error != QSerialPort::TimeoutError && !serialWriteBusy(error, text);
}
}
