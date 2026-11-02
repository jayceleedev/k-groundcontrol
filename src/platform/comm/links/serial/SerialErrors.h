// [0.3.0 F5] specs/features/0.3.0-params.md
// [0.1.1 T5] specs/features/0.1.1-link-hardening.md
#pragma once
#include <QSerialPort>
#include <QString>
namespace kgc::comm::links {
/** 쓰기가 잠깐 바쁜 것뿐인지 판정해요.
 * 입력: QSerialPort가 알린 error와 그 글 text예요.
 * 출력: ResourceError이고 글이 qt_error_string(EAGAIN)과 같으면 참이에요. Qt가 EAGAIN을 ResourceError로 알려서 글로 구분해요.
 * 실패: 없어요.
 */
bool serialWriteBusy(QSerialPort::SerialPortError error, const QString& text);
/** 시리얼 오류로 링크를 닫아야 하는지 판정해요.
 * 입력: QSerialPort가 알린 error와 그 글 text예요.
 * 출력: NoError·TimeoutError와 serialWriteBusy인 경우만 거짓이고 나머지는 모두 참이에요.
 * 실패: 없어요.
 */
bool serialErrorClosesLink(QSerialPort::SerialPortError error, const QString& text);
}
