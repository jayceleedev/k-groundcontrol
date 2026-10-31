// [0.2.0 T14–T22] specs/features/0.2.0-fcflash.md
#include <kgc/fcFlash/Errors.h>
namespace kgc::fcFlash {
QString errorText(kgc::app::errors::ErrorCode error)
{
    // WORKER-FILL: spec §7 — QT_TRANSLATE_NOOP("FcFlash", …)로 §6.8·§7의 문구를 돌려줘요.
    Q_UNUSED(error);
    return {};
}
}
