// [0.4.1 5.1 T5] specs/features/0.4.1-board-match.md
#include "UsbName.h"
#include <QFile>
#include <QFileInfo>
#ifdef Q_OS_WIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
// initguid.h를 먼저 넣어야 GUID_DEVCLASS_PORTS와 DEVPKEY 값이 이 파일 안에 정의돼요.
#include <initguid.h>
#include <devguid.h>
#include <devpkey.h>
#include <setupapi.h>
#include <cfgmgr32.h>
#include <vector>
#endif
namespace kgc::comm::links {
QString usbNameFromSysfs(const QString& sysRoot, const QString& ttyName)
{
    const auto root = QFileInfo(sysRoot).canonicalFilePath();
    if (root.isEmpty() || ttyName.isEmpty()) return {};
    // device는 USB 인터페이스 폴더를 가리키는 링크라서 풀린 실제 경로에서 부모로 올라가요.
    auto folder = QFileInfo(sysRoot + QStringLiteral("/class/tty/") + ttyName + QStringLiteral("/device")).canonicalFilePath();
    // Windows 경로는 대소문자를 가리지 않아서 sysRoot 안인지도 대소문자 없이 봐요.
#ifdef Q_OS_WIN
    constexpr auto sensitivity = Qt::CaseInsensitive;
#else
    constexpr auto sensitivity = Qt::CaseSensitive;
#endif
    while (!folder.isEmpty() && folder.startsWith(root + '/', sensitivity)) {
        if (QFile::exists(folder + QStringLiteral("/idVendor"))) {
            QFile product(folder + QStringLiteral("/product"));
            if (!product.open(QIODevice::ReadOnly)) return {};
            return QString::fromUtf8(product.readAll()).trimmed();
        }
        const auto parent = QFileInfo(folder).path();
        if (parent == folder) break;
        folder = parent;
    }
    return {};
}
#ifdef Q_OS_WIN
namespace {
// 버퍼 끝에 0을 하나 더 둬서 값이 0으로 끝나지 않아도 안전하게 읽어요.
QString wideText(const std::vector<BYTE>& buffer)
{
    return QString::fromWCharArray(reinterpret_cast<const wchar_t*>(buffer.data())).trimmed();
}
QString portNameOf(HDEVINFO set, SP_DEVINFO_DATA& data)
{
    const HKEY key = SetupDiOpenDevRegKey(set, &data, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
    if (key == INVALID_HANDLE_VALUE) return {};
    QString name;
    DWORD type = 0;
    DWORD size = 0;
    if (RegQueryValueExW(key, L"PortName", nullptr, &type, nullptr, &size) == ERROR_SUCCESS && type == REG_SZ && size > 0) {
        std::vector<BYTE> buffer(size + sizeof(wchar_t), 0);
        if (RegQueryValueExW(key, L"PortName", nullptr, &type, buffer.data(), &size) == ERROR_SUCCESS) name = wideText(buffer);
    }
    RegCloseKey(key);
    return name;
}
QString reportedName(HDEVINFO set, SP_DEVINFO_DATA& data)
{
    DEVPROPTYPE type = DEVPROP_TYPE_EMPTY;
    DWORD size = 0;
    SetupDiGetDevicePropertyW(set, &data, &DEVPKEY_Device_BusReportedDeviceDesc, &type, nullptr, 0, &size, 0);
    if (size == 0) return {};
    std::vector<BYTE> buffer(size + sizeof(wchar_t), 0);
    if (!SetupDiGetDevicePropertyW(set, &data, &DEVPKEY_Device_BusReportedDeviceDesc, &type, buffer.data(), size, nullptr, 0)
        || type != DEVPROP_TYPE_STRING) return {};
    return wideText(buffer);
}
QString parentReportedName(DEVINST child)
{
    DEVINST parent = 0;
    if (CM_Get_Parent(&parent, child, 0) != CR_SUCCESS) return {};
    DEVPROPTYPE type = DEVPROP_TYPE_EMPTY;
    ULONG size = 0;
    if (CM_Get_DevNode_PropertyW(parent, &DEVPKEY_Device_BusReportedDeviceDesc, &type, nullptr, &size, 0) != CR_BUFFER_SMALL
        || size == 0) return {};
    std::vector<BYTE> buffer(size + sizeof(wchar_t), 0);
    if (CM_Get_DevNode_PropertyW(parent, &DEVPKEY_Device_BusReportedDeviceDesc, &type, buffer.data(), &size, 0) != CR_SUCCESS
        || type != DEVPROP_TYPE_STRING) return {};
    return wideText(buffer);
}
}
QString usbNameWindows(const QString& portName)
{
    const HDEVINFO set = SetupDiGetClassDevsW(&GUID_DEVCLASS_PORTS, nullptr, nullptr, DIGCF_PRESENT);
    if (set == INVALID_HANDLE_VALUE) return {};
    QString result;
    SP_DEVINFO_DATA data{};
    data.cbSize = sizeof(data);
    for (DWORD index = 0; SetupDiEnumDeviceInfo(set, index, &data); ++index) {
        if (portNameOf(set, data).compare(portName, Qt::CaseInsensitive) != 0) continue;
        // 복합 장치는 COM 노드가 이름을 모르고 부모 USB 장치가 알아서 한 단계 올라가 봐요.
        result = reportedName(set, data);
        if (result.isEmpty()) result = parentReportedName(data.DevInst);
        break;
    }
    SetupDiDestroyDeviceInfoList(set);
    return result;
}
#endif
}
