// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <kgc/vehicle/states/CapabilitiesBundle.h>
#include <kgc/comm/mavlink/Mavlink.h>
#include <kgc/vehicle/firmware/FirmwarePlugin.h>
namespace kgc::vehicle::states {
CapabilitiesBundle updateCapabilities(const mavlink_message_t& message, const kgc::vehicle::firmware::FirmwarePlugin& plugin);
}
