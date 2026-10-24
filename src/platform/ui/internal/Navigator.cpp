// [0.0.1 T1] specs/features/0.0.1-shell.md
#include <kgc/ui/Navigator.h>
namespace kgc::ui {
Navigator::Navigator(QObject* parent) : QObject(parent) {}
void Navigator::open(const QString& panelId, const QString& arg) { emit openRequested(panelId, arg); }
}
