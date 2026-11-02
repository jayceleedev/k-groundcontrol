// [0.3.0 5.2] specs/features/0.3.0-params.md
// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <kgc/vehicle/firmware/FirmwarePlugin.h>
namespace kgc::vehicle::firmware {
class GenericPlugin final : public FirmwarePlugin {
public:
    QString displayName() const override;
    QString modeName(std::uint32_t customMode, std::uint8_t mavType) const override;
    QList<StreamRequest> streamRequests() const override;
    ParamEncoding paramEncoding() const override;
    ParamMetadataSource paramMetadataSource(int mavType, const QString& version) const override;
    QString paramGroup(const QString& name, const QString& metadataGroup) const override;
    QString paramGroupTitle(const QString& group) const override;
    QStringList paramFileSkip() const override;
};
}
