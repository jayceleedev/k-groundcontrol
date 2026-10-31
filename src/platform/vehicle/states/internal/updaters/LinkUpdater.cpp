// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include "LinkUpdater.h"
namespace kgc::vehicle::states {
LinkBundle updateLink(const mavlink_message_t&, qint64 nowMs) { return {true, false, nowMs}; }
}
