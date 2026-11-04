// [0.4.1 T5] specs/features/0.4.1-board-match.md
#include "UsbName.h"
#include <QtTest>
#include <filesystem>
using kgc::comm::links::usbNameFromSysfs;
namespace {
void writeFile(const QString& path, const QByteArray& bytes)
{
    QVERIFY(QDir().mkpath(QFileInfo(path).path()));
    QFile file(path);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write(bytes);
}
/** 폴더를 가리키는 심볼릭 링크를 만들어요. 권한이 없어 못 만들면 거짓이에요(Windows 시험 환경). */
bool linkFolder(const QString& target, const QString& link)
{
    if (!QDir().mkpath(target) || !QDir().mkpath(QFileInfo(link).path())) return false;
    std::error_code error;
    std::filesystem::create_directory_symlink(std::filesystem::path(target.toStdWString()), std::filesystem::path(link.toStdWString()), error);
    return !error && QFileInfo(link).isSymLink();
}
}
class UsbNameSysfsTest : public QObject {
    Q_OBJECT
private:
    QTemporaryDir temp;
    QString sys() const { return temp.path() + "/sys"; }
    QString usbDevice() const { return sys() + "/devices/pci0000:00/0000:00:14.0/usb1/1-6"; }
private slots:
    /** 실제 Linux와 같은 모양의 가짜 sysfs를 만들어요. 링크를 못 만드는 환경에서는 이 시험 전체를 건너뛰어요. */
    void initTestCase()
    {
        QVERIFY(temp.isValid());
        writeFile(usbDevice() + "/idVendor", "3162\n");
        writeFile(usbDevice() + "/product", "  MatekH743-BL \n");
        if (!linkFolder(usbDevice() + "/1-6:1.0/tty/ttyACM0", sys() + "/class/tty/ttyACM0/device"))
            QSKIP("이 환경에서는 심볼릭 링크를 만들 수 없어요");
    }
    /** T5: device 링크를 풀고 부모로 올라가 idVendor가 있는 폴더의 product를 앞뒤 공백만 지워 읽어요. -BL은 그대로예요. */
    void test_T5_reads_product_of_usb_device()
    {
        QCOMPARE(usbNameFromSysfs(sys(), "ttyACM0"), QString("MatekH743-BL"));
    }
    /** T5: sysRoot 자체가 링크여도 풀린 경로를 기준으로 봐요. */
    void test_T5_sys_root_link_is_resolved()
    {
        if (!linkFolder(sys(), temp.path() + "/syslink")) QSKIP("이 환경에서는 심볼릭 링크를 만들 수 없어요");
        QCOMPARE(usbNameFromSysfs(temp.path() + "/syslink", "ttyACM0"), QString("MatekH743-BL"));
    }
    /** T5: device 링크가 없으면 빈 값이에요(PTY나 내장 포트). */
    void test_T5_missing_link_is_empty()
    {
        QVERIFY(QDir().mkpath(sys() + "/class/tty/ttyS0"));
        QVERIFY(usbNameFromSysfs(sys(), "ttyS0").isEmpty());
        QVERIFY(usbNameFromSysfs(sys(), "ttyACM9").isEmpty());
        QVERIFY(usbNameFromSysfs(sys(), "").isEmpty());
        QVERIFY(usbNameFromSysfs(temp.path() + "/missing", "ttyACM0").isEmpty());
    }
    /** T5: idVendor가 sysRoot 안에 없으면 빈 값이에요. 링크가 밖을 가리키거나, idVendor가 sysRoot 위에만 있는 경우예요. */
    void test_T5_id_vendor_outside_root_is_empty()
    {
        const auto outside = temp.path() + "/outside/1-7";
        writeFile(outside + "/idVendor", "3162\n");
        writeFile(outside + "/product", "Outside\n");
        QVERIFY(linkFolder(outside + "/1-7:1.0/tty/ttyACM1", sys() + "/class/tty/ttyACM1/device"));
        QVERIFY(usbNameFromSysfs(sys(), "ttyACM1").isEmpty());
        writeFile(temp.path() + "/idVendor", "3162\n");
        writeFile(temp.path() + "/product", "Above\n");
        QVERIFY(linkFolder(sys() + "/devices/virtual/tty/ttyACM2", sys() + "/class/tty/ttyACM2/device"));
        QVERIFY(usbNameFromSysfs(sys(), "ttyACM2").isEmpty());
    }
};
QTEST_APPLESS_MAIN(UsbNameSysfsTest)
#include "test_T5_usb_name_sysfs.moc"
