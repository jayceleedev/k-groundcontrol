// [0.0.3 T9] specs/features/0.0.3-windows.md
#include <kgc/secondPanel/SecondPanelService.h>
namespace kgc::secondPanel {
SecondPanelService::SecondPanelService(QObject* parent) : QObject(parent)
{
    settings::onArrangementChanged(this, [this](auto) { emit arrangementChanged(); });
}
settings::Arrangement SecondPanelService::arrangement() const { return settings::arrangement(); }
QList<QRectF> SecondPanelService::cells() const { return cellRects(arrangement()); }
void SecondPanelService::setArrangement(settings::Arrangement arrangement) { settings::setArrangement(arrangement); }
}
