// [0.3.0 F5] specs/features/0.3.0-params.md
#include "SerialErrors.h"
#include <QtTest>
#include <cerrno>
using kgc::comm::links::serialErrorClosesLink;
using kgc::comm::links::serialWriteBusy;
class SerialEagainTest : public QObject {
    Q_OBJECT
private slots:
    /** F5: 쓰기가 잠깐 바쁜 EAGAIN은 링크를 닫지 않아요. */
    void test_serial_eagain_keeps_link()
    {
        const auto text = qt_error_string(EAGAIN);
        QVERIFY(serialWriteBusy(QSerialPort::ResourceError, text));
        QVERIFY(!serialErrorClosesLink(QSerialPort::ResourceError, text));
    }
    /** F5: 같은 ResourceError라도 EIO는 진짜 끊김이라 닫아요. */
    void test_serial_eio_closes_link()
    {
        const auto text = qt_error_string(EIO);
        QVERIFY(!serialWriteBusy(QSerialPort::ResourceError, text));
        QVERIFY(serialErrorClosesLink(QSerialPort::ResourceError, text));
        QVERIFY(serialErrorClosesLink(QSerialPort::ResourceError, {}));
    }
    /** F5: EAGAIN 글이어도 다른 오류 종류는 그대로 닫아요. */
    void test_serial_eagain_text_needs_resource_error()
    {
        const auto text = qt_error_string(EAGAIN);
        QVERIFY(!serialWriteBusy(QSerialPort::WriteError, text));
        QVERIFY(serialErrorClosesLink(QSerialPort::WriteError, text));
    }
    /** F5: 오류 없음과 시간 초과는 바뀌지 않고 닫지 않아요. */
    void test_serial_no_error_and_timeout_unchanged()
    {
        QVERIFY(!serialErrorClosesLink(QSerialPort::NoError, {}));
        QVERIFY(!serialErrorClosesLink(QSerialPort::TimeoutError, {}));
        QVERIFY(!serialWriteBusy(QSerialPort::NoError, {}));
    }
};
QTEST_GUILESS_MAIN(SerialEagainTest)
#include "test_serial_eagain_keeps_link.moc"
