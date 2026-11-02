// [0.3.0 5.2 6.3 7.1] specs/features/0.3.0-params.md
#include "ArduPilotPlugin.h"
#include <kgc/comm/mavlink/Mavlink.h>
#include <QCoreApplication>
#include <QRegularExpression>
#include <algorithm>
#include <array>
#include <utility>
namespace kgc::vehicle::firmware {
namespace {
// §7.1 표예요. 원문은 번역 문맥 ParamGroup에 있어요.
constexpr std::array<std::pair<const char*, const char*>, 41> kGroupTitles{{
    {"ACRO", QT_TRANSLATE_NOOP("ParamGroup", "아크로 모드")},
    {"ADSB", QT_TRANSLATE_NOOP("ParamGroup", "ADS-B")},
    {"AHRS", QT_TRANSLATE_NOOP("ParamGroup", "자세 추정")},
    {"ARMING", QT_TRANSLATE_NOOP("ParamGroup", "시동 검사")},
    {"ATC", QT_TRANSLATE_NOOP("ParamGroup", "자세 제어")},
    {"AUTOTUNE", QT_TRANSLATE_NOOP("ParamGroup", "자동 튜닝")},
    {"BARO", QT_TRANSLATE_NOOP("ParamGroup", "기압계")},
    {"BATT", QT_TRANSLATE_NOOP("ParamGroup", "배터리")},
    {"BRD", QT_TRANSLATE_NOOP("ParamGroup", "보드")},
    {"CAM", QT_TRANSLATE_NOOP("ParamGroup", "카메라")},
    {"CAN", QT_TRANSLATE_NOOP("ParamGroup", "CAN 버스")},
    {"CIRCLE", QT_TRANSLATE_NOOP("ParamGroup", "서클 모드")},
    {"COMPASS", QT_TRANSLATE_NOOP("ParamGroup", "나침반")},
    {"EK", QT_TRANSLATE_NOOP("ParamGroup", "위치 추정 필터")},
    {"FENCE", QT_TRANSLATE_NOOP("ParamGroup", "지오펜스")},
    {"FLOW", QT_TRANSLATE_NOOP("ParamGroup", "옵티컬 플로")},
    {"FLTMODE", QT_TRANSLATE_NOOP("ParamGroup", "비행 모드")},
    {"FRAME", QT_TRANSLATE_NOOP("ParamGroup", "기체 구성")},
    {"FS", QT_TRANSLATE_NOOP("ParamGroup", "페일세이프")},
    {"GPS", QT_TRANSLATE_NOOP("ParamGroup", "GPS")},
    {"INS", QT_TRANSLATE_NOOP("ParamGroup", "관성 센서")},
    {"LAND", QT_TRANSLATE_NOOP("ParamGroup", "착륙")},
    {"LOG", QT_TRANSLATE_NOOP("ParamGroup", "로그")},
    {"MNT", QT_TRANSLATE_NOOP("ParamGroup", "짐벌")},
    {"MOT", QT_TRANSLATE_NOOP("ParamGroup", "모터")},
    {"NTF", QT_TRANSLATE_NOOP("ParamGroup", "알림 장치")},
    {"OSD", QT_TRANSLATE_NOOP("ParamGroup", "OSD")},
    {"PILOT", QT_TRANSLATE_NOOP("ParamGroup", "조종 입력")},
    {"PSC", QT_TRANSLATE_NOOP("ParamGroup", "위치 제어")},
    {"RC", QT_TRANSLATE_NOOP("ParamGroup", "조종기")},
    {"RCMAP", QT_TRANSLATE_NOOP("ParamGroup", "조종기 채널 배정")},
    {"RELAY", QT_TRANSLATE_NOOP("ParamGroup", "릴레이")},
    {"RNGFND", QT_TRANSLATE_NOOP("ParamGroup", "거리 센서")},
    {"RTL", QT_TRANSLATE_NOOP("ParamGroup", "복귀")},
    {"SCHED", QT_TRANSLATE_NOOP("ParamGroup", "스케줄러")},
    {"SERIAL", QT_TRANSLATE_NOOP("ParamGroup", "시리얼 포트")},
    {"SERVO", QT_TRANSLATE_NOOP("ParamGroup", "서보 출력")},
    {"SR", QT_TRANSLATE_NOOP("ParamGroup", "텔레메트리 속도")},
    {"SYSID", QT_TRANSLATE_NOOP("ParamGroup", "시스템 번호")},
    {"TERRAIN", QT_TRANSLATE_NOOP("ParamGroup", "지형")},
    {"WPNAV", QT_TRANSLATE_NOOP("ParamGroup", "웨이포인트 비행")},
}};
QString vehicleFamily(int mavType)
{
    switch (mavType) {
    case MAV_TYPE_QUADROTOR: case MAV_TYPE_HEXAROTOR: case MAV_TYPE_OCTOROTOR: case MAV_TYPE_TRICOPTER:
    case MAV_TYPE_COAXIAL: case MAV_TYPE_HELICOPTER: case MAV_TYPE_DODECAROTOR: case MAV_TYPE_DECAROTOR:
    case MAV_TYPE_GENERIC:
        return QStringLiteral("Copter");
    case MAV_TYPE_FIXED_WING: case MAV_TYPE_VTOL_TAILSITTER_DUOROTOR: case MAV_TYPE_VTOL_TAILSITTER_QUADROTOR:
    case MAV_TYPE_VTOL_TILTROTOR: case MAV_TYPE_VTOL_FIXEDROTOR: case MAV_TYPE_VTOL_TAILSITTER:
    case MAV_TYPE_VTOL_TILTWING: case MAV_TYPE_VTOL_RESERVED5: case MAV_TYPE_VTOL_GYRODYNE:
        return QStringLiteral("Plane");
    case MAV_TYPE_GROUND_ROVER: case MAV_TYPE_SURFACE_BOAT: return QStringLiteral("Rover");
    case MAV_TYPE_SUBMARINE: return QStringLiteral("Sub");
    case MAV_TYPE_ANTENNA_TRACKER: return QStringLiteral("Tracker");
    default: return {};
    }
}
}
ParamEncoding ArduPilotPlugin::paramEncoding() const { return ParamEncoding::CCast; }
ParamMetadataSource ArduPilotPlugin::paramMetadataSource(int mavType, const QString& version) const
{
    static const QRegularExpression form(QStringLiteral("^(\\d+)\\.(\\d+)"));
    const auto family = vehicleFamily(mavType);
    const auto match = form.match(version);
    if (family.isEmpty() || !match.hasMatch()) return {ParamMetadataFormat::None, {}, {}};
    const auto major = match.captured(1);
    const int minor = match.captured(2).toInt();
    QList<QUrl> urls;
    for (int candidate = minor; candidate >= std::max(0, minor - 2); --candidate)
        urls.append(QUrl(QStringLiteral("https://raw.githubusercontent.com/ArduPilot/ParameterRepository/main/%1-%2.%3/apm.pdef.json")
                             .arg(family, major).arg(candidate)));
    return {ParamMetadataFormat::Grouped, urls, QStringLiteral("%1-%2.%3").arg(family, major).arg(minor)};
}
QString ArduPilotPlugin::paramGroup(const QString& name, const QString& metadataGroup) const
{
    if (metadataGroup == QStringLiteral("Lua Script")) return metadataGroup;
    const auto end = name.indexOf('_');
    if (end < 0) return {};
    auto prefix = name.left(end);
    while (!prefix.isEmpty() && prefix.back().isDigit()) prefix.chop(1);
    return prefix;
}
QString ArduPilotPlugin::paramGroupTitle(const QString& group) const
{
    for (const auto& [key, title] : kGroupTitles)
        if (group == QLatin1StringView(key)) return QCoreApplication::translate("ParamGroup", title);
    return {};
}
QStringList ArduPilotPlugin::paramFileSkip() const
{
    return {QStringLiteral("SYSID_SW_MREV"), QStringLiteral("WP_TOTAL"), QStringLiteral("CMD_TOTAL"),
            QStringLiteral("FENCE_TOTAL"), QStringLiteral("SYS_NUM_RESETS"), QStringLiteral("ARSPD_OFFSET"),
            QStringLiteral("GND_ABS_PRESS"), QStringLiteral("GND_TEMP"), QStringLiteral("CMD_INDEX"),
            QStringLiteral("LOG_LASTFILE"), QStringLiteral("FORMAT_VERSION")};
}
}
