// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <kgc/vehicle/states/LinkBundle.h>
#include <kgc/comm/mavlink/Mavlink.h>
#include <kgc/vehicle/firmware/FirmwarePlugin.h>
namespace kgc::vehicle::states {
LinkBundle updateLink(const mavlink_message_t& message, qint64 nowMs);
}
