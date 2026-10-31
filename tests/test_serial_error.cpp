// [0.1.1 T5] specs/features/0.1.1-link-hardening.md
#include "SerialErrors.h"
#include <QtTest>
using kgc::comm::links::serialErrorClosesLink;
class SerialErrorTest : public QObject {
    Q_OBJECT
private slots:
    /** T5: 오류 없음과 시간 초과를 뺀 모든 오류가 링크를 닫아요. */
    void test_T5_every_real_error_closes()
    {
        QVERIFY(serialErrorClosesLink(QSerialPort::ResourceError));
        QVERIFY(serialErrorClosesLink(QSerialPort::DeviceNotFoundError));
        QVERIFY(serialErrorClosesLink(QSerialPort::PermissionError));
        QVERIFY(serialErrorClosesLink(QSerialPort::WriteError));
        QVERIFY(serialErrorClosesLink(QSerialPort::ReadError));
        QVERIFY(serialErrorClosesLink(QSerialPort::UnknownError));
        QVERIFY(!serialErrorClosesLink(QSerialPort::TimeoutError));
        QVERIFY(!serialErrorClosesLink(QSerialPort::NoError));
    }
};
QTEST_GUILESS_MAIN(SerialErrorTest)
#include "test_serial_error.moc"
