// [0.1.1 T5] specs/features/0.1.1-link-hardening.md
#pragma once
#include <QSerialPort>
namespace kgc::comm::links {
/** 시리얼 오류로 링크를 닫아야 하는지 판정해요.
 * 입력: QSerialPort가 알린 error예요.
 * 출력: NoError·TimeoutError만 거짓이고 나머지는 모두 참이에요.
 * 실패: 없어요.
 */
bool serialErrorClosesLink(QSerialPort::SerialPortError error);
}
