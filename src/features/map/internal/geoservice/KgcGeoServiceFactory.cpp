// [0.5.0 5.6.7] specs/features/0.5.0-map.md
#include "KgcGeoServiceFactory.h"
#include "Constants.h"
#include "KgcMappingEngine.h"
#include "TileSource.h"
namespace kgc::map {
QGeoMappingManagerEngine* KgcGeoServiceFactory::createMappingManagerEngine(const QVariantMap& parameters,
    QGeoServiceProvider::Error* error, QString* errorString) const
{
    Q_UNUSED(parameters);
    // 전역 변수 대신 register.cpp가 QML 엔진 프로퍼티에 둔 공급을 꺼내요.
    auto* source = engine_ ? qobject_cast<TileSource*>(engine_->property(kTileSourceProperty).value<QObject*>()) : nullptr;
    if (!source) {
        *error = QGeoServiceProvider::NotSupportedError;
        *errorString = QStringLiteral("kgc: no tile source");
        return nullptr;
    }
    return new KgcMappingEngine(*source, error, errorString);
}
void KgcGeoServiceFactory::setQmlEngine(QQmlEngine* engine)
{
    engine_ = engine;
}
}
