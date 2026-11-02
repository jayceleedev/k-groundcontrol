// [0.3.0 6.4 T1] specs/features/0.3.0-params.md
#pragma once
#include <kgc/vehicle/firmware/FirmwarePlugin.h>
#include <cstdint>
namespace kgc::params {
/** PARAM_VALUE의 float 칸을 값으로 풀어요.
 * 입력: float 칸 wire, MAV_PARAM_TYPE 번호 type, 펌웨어의 encoding이에요.
 * 출력: 값이에요. Bytewise 정수형은 float 비트를 그 형으로 읽어요.
 * 실패: 없어요.
 */
double decodeParam(float wire, std::uint8_t type, kgc::vehicle::firmware::ParamEncoding encoding);
/** 값을 PARAM_SET의 float 칸으로 담아요.
 * 입력: 값 value, MAV_PARAM_TYPE 번호 type, 펌웨어의 encoding이에요.
 * 출력: float 칸이에요. Bytewise 정수형은 비트를 그대로 담아요.
 * 실패: 없어요.
 */
float encodeParam(double value, std::uint8_t type, kgc::vehicle::firmware::ParamEncoding encoding);
}
