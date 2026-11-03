// [0.4.0 5.2] specs/features/0.4.0-sitl.md
#pragma once
#include <QtGlobal>
namespace kgc::comm::links {
// UDP 링크가 듣는 포트예요. 시뮬레이터도 이 번호로 보내도록 한 곳에만 둬요.
inline constexpr quint16 kUdpPort = 14550;
}
