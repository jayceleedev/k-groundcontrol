// [0.3.0 6.4 T1] specs/features/0.3.0-params.md
#include "ParamCodec.h"
#include <kgc/comm/mavlink/Mavlink.h>
#include <bit>
namespace kgc::params {
using kgc::vehicle::firmware::ParamEncoding;
double decodeParam(float wire, std::uint8_t type, ParamEncoding encoding)
{
    if (encoding == ParamEncoding::CCast) return wire;
    const auto bits = std::bit_cast<std::uint32_t>(wire);
    switch (type) {
    case MAV_PARAM_TYPE_UINT8: return static_cast<std::uint8_t>(bits);
    case MAV_PARAM_TYPE_INT8: return static_cast<std::int8_t>(bits);
    case MAV_PARAM_TYPE_UINT16: return static_cast<std::uint16_t>(bits);
    case MAV_PARAM_TYPE_INT16: return static_cast<std::int16_t>(bits);
    case MAV_PARAM_TYPE_UINT32: return bits;
    case MAV_PARAM_TYPE_INT32: return static_cast<std::int32_t>(bits);
    default: return wire;
    }
}
float encodeParam(double value, std::uint8_t type, ParamEncoding encoding)
{
    if (encoding == ParamEncoding::CCast) return static_cast<float>(value);
    // 8·16비트 정수는 낮은 바이트에만 담아야 decodeParam과 짝이 맞아요.
    switch (type) {
    case MAV_PARAM_TYPE_UINT8:
        return std::bit_cast<float>(std::uint32_t{static_cast<std::uint8_t>(value)});
    case MAV_PARAM_TYPE_INT8:
        return std::bit_cast<float>(std::uint32_t{static_cast<std::uint8_t>(static_cast<std::int8_t>(value))});
    case MAV_PARAM_TYPE_UINT16:
        return std::bit_cast<float>(std::uint32_t{static_cast<std::uint16_t>(value)});
    case MAV_PARAM_TYPE_INT16:
        return std::bit_cast<float>(std::uint32_t{static_cast<std::uint16_t>(static_cast<std::int16_t>(value))});
    case MAV_PARAM_TYPE_UINT32: return std::bit_cast<float>(static_cast<std::uint32_t>(value));
    case MAV_PARAM_TYPE_INT32: return std::bit_cast<float>(static_cast<std::int32_t>(value));
    default: return static_cast<float>(value);
    }
}
}
