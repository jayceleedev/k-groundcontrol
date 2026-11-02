// [0.3.0 6.3] specs/features/0.3.0-params.md
#pragma once
#include <kgc/params/ParamMeta.h>
#include <kgc/vehicle/firmware/FirmwarePlugin.h>
#include <QHash>
#include <QObject>
namespace kgc::app::net { class Http; }
namespace kgc::params {
/** 메타데이터를 캐시에서 읽거나 내려받아 해석해요.
 * 입력: Http와 플러그인이 낸 출처예요.
 * 출력: loaded 또는 failed를 한 번 내요.
 * 실패: 주소를 다 시도해도 못 받거나 해석 결과가 비면 failed예요.
 */
class ParamMetadataLoader : public QObject {
    Q_OBJECT
public:
/** 멈춘 로더를 만들어요.
 * 입력: 수명 동안 유지되는 http와 parent예요.
 * 출력: start 전까지 아무 일도 하지 않아요.
 * 실패: 없어요.
 */
    ParamMetadataLoader(kgc::app::net::Http& http, QObject* parent);
/** 읽기를 시작해요.
 * 입력: 출처 source이며 format은 None이 아니어야 해요.
 * 출력: 캐시가 쓸 만하면 바로 loaded를 내고, 아니면 주소를 차례로 내려받아요.
 * 실패: failed로 알려요.
 */
    void start(const kgc::vehicle::firmware::ParamMetadataSource& source);
signals:
/** 메타데이터를 얻었음을 알려요. 입력: 해석 결과 metadata예요. 출력: 표에 넣어요. 실패: 없어요. */
    void loaded(const QHash<QString, kgc::params::ParamMeta>& metadata);
/** 메타데이터를 얻지 못했음을 알려요. 입력: 없어요. 출력: 상태를 Failed로 바꿔요. 실패: 없어요. */
    void failed();
private:
    QHash<QString, ParamMeta> parseFile() const;
    void tryNext();
    kgc::app::net::Http& http_;
    kgc::vehicle::firmware::ParamMetadataSource source_;
    QString path_;
    qsizetype next_ = 0;
};
}
