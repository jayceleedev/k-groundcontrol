// [0.6.0 5.1 T3] specs/features/0.6.0-fly.md
#include <kgc/app/geo/Altitude.h>
#include <algorithm>
#include <cmath>
namespace kgc::app::geo {
std::optional<double> altitudeIn(AltitudeFrame frame, double relativeM, double baseAmslM)
{
    switch (frame) {
    case AltitudeFrame::RelativeHome:
        if (std::isfinite(relativeM)) return relativeM;
        return std::nullopt;
    case AltitudeFrame::Amsl:
        if (std::isfinite(relativeM) && std::isfinite(baseAmslM)) return baseAmslM + relativeM;
        return std::nullopt;
    case AltitudeFrame::AboveCurrent:
        break;
    }
    return std::nullopt;
}
std::optional<double> heightIn(AltitudeFrame frame, double heightM, double altRelM, double altAmslM)
{
    if (!std::isfinite(heightM)) return std::nullopt;
    switch (frame) {
    case AltitudeFrame::AboveCurrent:
        return heightM;
    case AltitudeFrame::RelativeHome:
        // 땅이 홈보다 낮으면 0으로 봐서 홈 기준 값이 입력한 높이보다 작아지지 않게 해요.
        if (std::isfinite(altRelM)) return heightM + std::max(altRelM, 0.0);
        return std::nullopt;
    case AltitudeFrame::Amsl:
        if (std::isfinite(altAmslM)) return altAmslM + heightM;
        return std::nullopt;
    }
    return std::nullopt;
}
}
