// [0.4.2 5.1] specs/features/0.4.2-param-tree.md
// [0.3.0 5.2] specs/features/0.3.0-params.md
// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include "GenericPlugin.h"
namespace kgc::vehicle::firmware {
QString GenericPlugin::displayName() const { return "MAVLink"; }
QString GenericPlugin::modeName(std::uint32_t, std::uint8_t) const { return {}; }
QList<StreamRequest> GenericPlugin::streamRequests() const { return {}; }
ParamEncoding GenericPlugin::paramEncoding() const { return ParamEncoding::CCast; }
ParamMetadataSource GenericPlugin::paramMetadataSource(int, const QString&) const
{
    return {ParamMetadataFormat::None, {}, {}};
}
QString GenericPlugin::paramGroupTitle(const QString&) const { return {}; }
QStringList GenericPlugin::paramFileSkip() const { return {}; }
}
