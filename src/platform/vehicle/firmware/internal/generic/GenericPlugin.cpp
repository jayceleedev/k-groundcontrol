// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include "GenericPlugin.h"
namespace kgc::vehicle::firmware {
QString GenericPlugin::displayName() const { return "MAVLink"; }
QString GenericPlugin::modeName(std::uint32_t, std::uint8_t) const { return {}; }
QList<StreamRequest> GenericPlugin::streamRequests() const { return {}; }
}
