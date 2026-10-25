// [0.0.2 T26] specs/features/0.0.2-theme.md
#include <kgc/ui/SurfaceFormat.h>
#include <QSurfaceFormat>

namespace kgc::ui {
void applyDefaultSurfaceFormat()
{
    // 차트 선과 곡선의 가장자리가 매끄럽게 보이도록 멀티샘플을 켜요.
    auto format = QSurfaceFormat::defaultFormat();
    format.setSamples(4);
    QSurfaceFormat::setDefaultFormat(format);
}
}
