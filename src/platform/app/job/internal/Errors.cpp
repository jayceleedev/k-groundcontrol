// [0.2.0 T2] specs/features/0.2.0-fcflash.md
#include <kgc/app/job/Errors.h>
namespace kgc::app::job {
QString errorText(kgc::app::errors::ErrorCode error)
{
    // WORKER-FILL: spec §5.3 — 스펙 §7에 Cancelled 문구가 없어요. 화면에 안 나오면 빈 문자열로 둬요.
    Q_UNUSED(error);
    return {};
}
}
