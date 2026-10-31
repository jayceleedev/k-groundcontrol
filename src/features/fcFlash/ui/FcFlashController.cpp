// [0.2.0 T14–T23] specs/features/0.2.0-fcflash.md
#include "FcFlashController.h"
namespace kgc::fcFlash {
FcFlashController::FcFlashController(FcFlashService& service) : QObject(&service), service_(service)
{
    Q_ASSERT(!instance_);
    instance_ = this;
    connect(&service_, &FcFlashService::changed, this, &FcFlashController::changed);
}
FcFlashController* FcFlashController::create(QQmlEngine* engine, QJSEngine*)
{
    Q_ASSERT(instance_ && instance_->thread() == engine->thread());
    QQmlEngine::setObjectOwnership(instance_, QQmlEngine::CppOwnership);
    return instance_;
}
}
