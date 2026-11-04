// [0.4.1 5.1 T5] specs/features/0.4.1-board-match.md
#pragma once
#include <QString>
namespace kgc::comm::links {
/** Linux sysfs에서 시리얼 포트가 달린 USB 장치의 제품 이름을 읽어요.
 * 입력: sysfs 뿌리 sysRoot(앱은 "/sys")와 /dev/를 뺀 장치 이름 ttyName(예: ttyACM0)이에요.
 * 출력: <sysRoot>/class/tty/<ttyName>/device 링크를 푼 폴더부터 부모로 올라가며 idVendor가 있는 첫 폴더의
 *       product 내용이에요. 앞뒤 공백만 지우고 -BL 같은 꼬리는 그대로 둬요.
 * 실패: 링크가 없거나, 풀린 sysRoot 안에서 idVendor 폴더를 찾지 못하거나, product를 읽지 못하면 빈 값이에요.
 *       PTY처럼 USB가 아닌 포트도 빈 값이에요.
 */
QString usbNameFromSysfs(const QString& sysRoot, const QString& ttyName);
#ifdef Q_OS_WIN
/** Windows 장치 관리자에서 COM 포트의 USB 장치가 스스로 알린 이름을 읽어요.
 * 입력: COM 이름 portName(예: COM7)이에요.
 * 출력: 포트 장치 노드의 DEVPKEY_Device_BusReportedDeviceDesc이며, 비었으면 부모 노드의 같은 값이에요.
 *       앞뒤 공백만 지워요.
 * 실패: 포트를 찾지 못하거나 두 값이 모두 비면 빈 값이에요.
 */
QString usbNameWindows(const QString& portName);
#endif
}
