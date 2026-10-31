// [0.2.0 T3] specs/features/0.2.0-fcflash.md
#include <kgc/app/net/Errors.h>
namespace kgc::app::net {
QString errorText(kgc::app::errors::ErrorCode error)
{
    // WORKER-FILL: spec §5.4 — 스펙에 HttpError 문구가 없어요. 화면은 기능 모듈의 문구를 쓰므로 빈 문자열로 둬요.
    Q_UNUSED(error);
    return {};
}
}
