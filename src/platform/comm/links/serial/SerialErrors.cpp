// [0.1.1 T5] specs/features/0.1.1-link-hardening.md
#include "SerialErrors.h"
namespace kgc::comm::links {
bool serialErrorClosesLink(QSerialPort::SerialPortError error)
{
    return error != QSerialPort::NoError && error != QSerialPort::TimeoutError;
}
}
