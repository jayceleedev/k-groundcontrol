// [0.3.0 6.4 6.8 6.8-1 T2] specs/features/0.3.0-params.md
#include "ParamValue.h"
#include "Constants.h"
#include <kgc/comm/mavlink/Mavlink.h>
#include <kgc/params/Errors.h>
#include <QCoreApplication>
#include <array>
#include <bit>
#include <charconv>
#include <cmath>
#include <limits>
#include <optional>
#include <system_error>
#include <utility>
namespace kgc::params {
namespace {
template<class T> std::pair<double, double> limits()
{
    return {static_cast<double>(std::numeric_limits<T>::lowest()), static_cast<double>(std::numeric_limits<T>::max())};
}
std::optional<std::pair<double, double>> integerLimits(std::uint8_t type)
{
    switch (type) {
    case MAV_PARAM_TYPE_UINT8: return limits<std::uint8_t>();
    case MAV_PARAM_TYPE_INT8: return limits<std::int8_t>();
    case MAV_PARAM_TYPE_UINT16: return limits<std::uint16_t>();
    case MAV_PARAM_TYPE_INT16: return limits<std::int16_t>();
    case MAV_PARAM_TYPE_UINT32: return limits<std::uint32_t>();
    case MAV_PARAM_TYPE_INT32: return limits<std::int32_t>();
    case MAV_PARAM_TYPE_UINT64: return limits<std::uint64_t>();
    case MAV_PARAM_TYPE_INT64: return limits<std::int64_t>();
    default: return std::nullopt;
    }
}
// float 비트를 수직선 위의 순서로 펴요. 두 값의 차이가 곧 ULP 거리예요.
std::int64_t floatOrder(float value)
{
    const auto bits = static_cast<std::int64_t>(std::bit_cast<std::int32_t>(value));
    return bits < 0 ? std::numeric_limits<std::int32_t>::min() - bits : bits;
}
std::uint64_t widthMask(std::uint8_t type)
{
    const int width = typeBits(type);
    return width == 64 ? ~std::uint64_t{0} : (std::uint64_t{1} << width) - 1U;
}
// 음수를 부호 없는 형으로 바로 바꾸면 정의되지 않은 동작이라 부호 있는 형을 거쳐요.
std::uint64_t rawBits(double value, std::uint8_t type)
{
    if (!std::isfinite(value) || std::fabs(value) >= std::ldexp(1.0, typeBits(type))) return 0U;
    const auto raw = value < 0.0 ? static_cast<std::uint64_t>(static_cast<std::int64_t>(value)) : static_cast<std::uint64_t>(value);
    return raw & widthMask(type);
}
double fromRawBits(std::uint64_t raw, std::uint8_t type)
{
    switch (type) {
    case MAV_PARAM_TYPE_INT8: return static_cast<std::int8_t>(static_cast<std::uint8_t>(raw));
    case MAV_PARAM_TYPE_INT16: return static_cast<std::int16_t>(static_cast<std::uint16_t>(raw));
    case MAV_PARAM_TYPE_INT32: return static_cast<std::int32_t>(static_cast<std::uint32_t>(raw));
    case MAV_PARAM_TYPE_INT64: return static_cast<double>(static_cast<std::int64_t>(raw));
    default: return static_cast<double>(raw);
    }
}
}
QString formatValue(double value, std::uint8_t type)
{
    if (integerLimits(type)) return QString::number(value == 0.0 ? 0.0 : value, 'f', 0);
    std::array<char, kValueTextSize> buffer{};
    auto* first = buffer.data();
    auto* last = first + buffer.size();
    const auto end = type == MAV_PARAM_TYPE_REAL64 ? std::to_chars(first, last, value, std::chars_format::fixed)
                                                   : std::to_chars(first, last, static_cast<float>(value), std::chars_format::fixed);
    if (end.ec != std::errc{}) return QString::number(value, 'g', std::numeric_limits<double>::max_digits10);
    return QString::fromLatin1(first, end.ptr - first);
}
kgc::app::errors::Result<void> validateForType(double value, std::uint8_t type)
{
    const auto range = integerLimits(type);
    if (range) {
        if (std::trunc(value) != value) return std::unexpected(make(ParamError::NotInteger));
        if (value < range->first || value > range->second) return std::unexpected(make(ParamError::TypeLimit));
    } else if (type != MAV_PARAM_TYPE_REAL64 && !std::isfinite(static_cast<float>(value))) {
        return std::unexpected(make(ParamError::TypeLimit));
    }
    return {};
}
kgc::app::errors::Result<double> parseValue(const QString& text, std::uint8_t type)
{
    bool ok = false;
    const double value = QString(text.trimmed()).replace(QLatin1Char(','), QLatin1Char('.')).toDouble(&ok);
    if (!ok || !std::isfinite(value)) return std::unexpected(make(ParamError::NotANumber));
    if (const auto checked = validateForType(value, type); !checked) return std::unexpected(checked.error());
    return value;
}
bool sameValue(double a, double b, std::uint8_t type)
{
    if (integerLimits(type)) return a == b;
    const auto first = static_cast<float>(a);
    const auto second = static_cast<float>(b);
    if (first == second) return true;
    if (std::isnan(first) || std::isnan(second)) return false;
    return std::abs(floatOrder(first) - floatOrder(second)) <= kParamFloatUlps;
}
int typeBits(std::uint8_t type)
{
    switch (type) {
    case MAV_PARAM_TYPE_UINT8: case MAV_PARAM_TYPE_INT8: return 8;
    case MAV_PARAM_TYPE_UINT16: case MAV_PARAM_TYPE_INT16: return 16;
    case MAV_PARAM_TYPE_UINT64: case MAV_PARAM_TYPE_INT64: return 64;
    default: return 32;
    }
}
bool bitIsSet(double value, std::uint8_t type, int bit)
{
    return bit >= 0 && bit < typeBits(type) && ((rawBits(value, type) >> bit) & 1U) != 0U;
}
double withBits(double value, std::uint8_t type, const QList<int>& listed, const QList<int>& chosen)
{
    auto raw = rawBits(value, type);
    for (const int bit : listed) {
        if (bit < 0 || bit >= typeBits(type)) continue;
        const auto mask = std::uint64_t{1} << bit;
        raw = chosen.contains(bit) ? raw | mask : raw & ~mask;
    }
    return fromRawBits(raw, type);
}
bool outOfRange(double value, const ParamMeta* meta)
{
    if (!meta) return false;
    return (meta->min && value < *meta->min) || (meta->max && value > *meta->max);
}
QString formatRange(const ParamMeta& meta, std::uint8_t type)
{
    if (meta.min && meta.max) return QStringLiteral("%1 ~ %2").arg(formatValue(*meta.min, type), formatValue(*meta.max, type));
    const auto bound = [](const char* key, const QString& value) { return QCoreApplication::translate("Params", key).arg(value); };
    if (meta.min) return bound(QT_TRANSLATE_NOOP("Params", "%1 이상"), formatValue(*meta.min, type));
    if (meta.max) return bound(QT_TRANSLATE_NOOP("Params", "%1 이하"), formatValue(*meta.max, type));
    return {};
}
}
