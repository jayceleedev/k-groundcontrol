// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include <kgc/comm/links/Link.h>
#include <kgc/comm/mavlink/MavlinkParser.h>
namespace kgc::comm::links {
struct Link::Private { kgc::comm::mavlink::MavlinkParser parser; };
Link::Link(QObject* parent) : QObject(parent), d_(std::make_unique<Private>()) {}
Link::~Link() = default;
void Link::receiveBytes(const QByteArray& bytes)
{
    for (const auto byte : bytes) {
        mavlink_message_t message{};
        bool isV1 = false;
        if (d_->parser.parse(static_cast<std::uint8_t>(byte), message, isV1)
            && !kgc::comm::mavlink::dropV1NonHeartbeat(isV1, message.msgid))
            emit messageReceived(QVariant::fromValue(message));
    }
}
}
