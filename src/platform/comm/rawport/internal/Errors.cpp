// [0.2.0 T4] specs/features/0.2.0-fcflash.md
#include <kgc/comm/rawport/Errors.h>
namespace kgc::comm::rawport {
QString errorText(kgc::app::errors::ErrorCode error)
{
    // WORKER-FILL: spec §5.5 — 스펙에 PortError 문구가 없어요. 화면은 fcFlash 문구를 쓰므로 빈 문자열로 둬요.
    Q_UNUSED(error);
    return {};
}
}
