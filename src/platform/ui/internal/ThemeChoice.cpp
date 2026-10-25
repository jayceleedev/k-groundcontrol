// [0.0.2 T2–T9 T16 T20] specs/features/0.0.2-theme.md
#include <kgc/ui/ThemeChoice.h>
namespace kgc::ui {
ThemeChoice resolveTheme(QStringView selected, Qt::ColorScheme scheme, const QStringList& available)
{
    if (available.contains(selected.toString())) return {selected.toString(), ThemeReason::Setting};
    return {scheme == Qt::ColorScheme::Dark ? QStringLiteral("dark") : QStringLiteral("light"),
        selected == u"system" ? ThemeReason::System : ThemeReason::Unknown};
}
}
