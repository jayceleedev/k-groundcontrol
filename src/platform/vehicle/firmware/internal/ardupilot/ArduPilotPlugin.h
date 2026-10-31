// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <kgc/vehicle/firmware/FirmwarePlugin.h>
namespace kgc::vehicle::firmware {
class ArduPilotPlugin final : public FirmwarePlugin {
public:
    QString displayName() const override;
    QString modeName(std::uint32_t customMode, std::uint8_t mavType) const override;
    QList<StreamRequest> streamRequests() const override;
};
}
