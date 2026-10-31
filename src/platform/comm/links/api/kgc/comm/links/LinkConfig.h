// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <QString>
namespace kgc::comm::links {
/** 링크를 열 때 필요한 값을 복사해서 전달해요.
 * 입력: 전송 종류 kind, 시리얼의 portName·baudRate예요.
 * 출력: 기본값은 UDP이며 포트 이름은 비어 있고 속도는 0이에요.
 * 실패: 이 구조체는 값을 검증하지 않고, 실제 열기 실패는 링크가 알려요.
 */
struct LinkConfig {
    enum class Kind { Udp, Serial };
    Kind kind = Kind::Udp;
    QString portName;
    int baudRate = 0;
    bool operator==(const LinkConfig&) const = default;
};
}
