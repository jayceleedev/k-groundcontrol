// [0.5.0 5.6.7] specs/features/0.5.0-map.md
#pragma once
#include <QObject>
#include <QPointer>
#include <QQmlEngine>
#include <QtLocation/QGeoServiceProviderFactory>
namespace kgc::map {
/** Qt Location이 provider "kgc"로 부르는 정적 지도 플러그인이에요.
 * 입력: Qt가 넘기는 QML 엔진과 플러그인 매개변수예요. 엔진 프로퍼티 kgcTileSource에서 타일 공급을 꺼내요.
 * 출력: 타일 공급을 쓰는 지도 엔진 KgcMappingEngine이에요. Composition이 Q_IMPORT_PLUGIN(KgcGeoServiceFactory)로 한 번 넣어요.
 * 실패: 타일 공급이 없으면 NotSupportedError와 "kgc: no tile source"예요.
 */
class KgcGeoServiceFactory : public QObject, public QGeoServiceProviderFactory {
    Q_OBJECT
    Q_INTERFACES(QGeoServiceProviderFactory)
    Q_PLUGIN_METADATA(IID "org.qt-project.qt.geoservice.serviceproviderfactory/6.0" FILE "kgc_plugin.json")
public:
/** 지도 엔진을 만들어요.
 * 입력: 매개변수 parameters(쓰지 않음)와 오류를 돌려줄 error·errorString이에요.
 * 출력: 새 KgcMappingEngine이에요. Qt가 소유해요.
 * 실패: 엔진이나 타일 공급이 없으면 nullptr이고 error는 NotSupportedError예요.
 */
    QGeoMappingManagerEngine* createMappingManagerEngine(const QVariantMap& parameters, QGeoServiceProvider::Error* error,
                                                         QString* errorString) const override;
/** Qt가 지도 엔진을 만들기 전에 QML 엔진을 알려 줘요.
 * 입력: 지도를 띄우는 QML 엔진 engine이에요.
 * 출력: 엔진을 약한 포인터로 들어요.
 * 실패: 없어요.
 */
    void setQmlEngine(QQmlEngine* engine) override;
private:
    QPointer<QQmlEngine> engine_;
};
}
