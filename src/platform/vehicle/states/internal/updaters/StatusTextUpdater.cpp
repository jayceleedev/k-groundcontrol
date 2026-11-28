// [0.6.0 5.4 T2] specs/features/0.6.0-fly.md
#include "StatusTextUpdater.h"
#include <QByteArray>
namespace kgc::vehicle::states {
VehicleMessage statusTextOf(const mavlink_message_t& message)
{
    mavlink_statustext_t status{};
    mavlink_msg_statustext_decode(&message, &status);
    VehicleMessage result;
    result.severity = status.severity;
    // 50자를 꽉 채운 글에는 끝 NUL이 없어서 길이를 칸 크기로 막아요.
    result.text = QString::fromUtf8(status.text, qstrnlen(status.text, MAVLINK_MSG_STATUSTEXT_FIELD_TEXT_LEN));
    return result;
}
}
