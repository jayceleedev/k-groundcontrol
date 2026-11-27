// [0.5.1 5.6] specs/features/0.5.1-map-sharp.md
// [0.5.0 5.6.8 T22] specs/features/0.5.0-map.md
#pragma once
#include "TileMath.h"
#include "TileSource.h"
#include <kgc/app/errors/Result.h>
#include <kgc/map/Settings.h>
#include <QGeoRectangle>
#include <QHash>
#include <QObject>
#include <QPointer>
#include <QString>
namespace kgc::app::job { class Job; }
namespace kgc::map {
/** 고른 영역의 타일을 확대 범위만큼 받아 저장소에 고정하는 영역 받기 작업 하나예요(Job을 가져요, SitlDownload 선례).
 * 입력: 타일 공급, 지도 종류·언어·타일 화소 배수, 영역과 확대 범위, 받을 타일 수(planDownload의 값)예요.
 * 출력: job()으로 단계·진행률·남은 시간·취소를 보여요. 받은 타일은 미리 받은 것(pinned)으로 남아요.
 * 실패: 받지 못한 타일이 있으면 TileFailed로, 취소하면 공용 취소로 끝나요. 이미 받은 것은 어느 쪽이든 남아요.
 */
class MapDownloadJob : public QObject {
    Q_OBJECT
public:
/** 아직 시작하지 않은 영역 받기를 만들어요.
 * 입력: 타일 공급 source, 지도 종류 type, 지명 언어 language, 타일 화소 배수 ratio(받는 동안 배수가 바뀌어도 이 배수로 끝까지 받아요),
 *       영역 area, 확대 범위 minZoom~maxZoom, 타일 수 total, parent예요.
 * 출력: Pending 상태의 Job(OperationKind::MapDownload)을 가진 객체예요.
 * 실패: 없어요.
 */
    MapDownloadJob(TileSource& source, settings::MapType type, QString language, int ratio, QGeoRectangle area,
                   int minZoom, int maxZoom, qint64 total, QObject* parent);
    /** 기다리는 타일 표를 모두 거둬요. 입력: 없어요. 출력: 없어요. 실패: 없어요. */
    ~MapDownloadJob() override;
    /** 입력: 없어요. 출력: 이 영역 받기의 Job이에요. 소유권은 넘기지 않아요. 실패: 없어요. */
    kgc::app::job::Job* job() const;
/** 받기를 시작해요.
 * 입력: 없어요.
 * 출력: Job이 "타일 받는 중" 단계로 Running이 되고, 작은 확대부터 타일을 kDownloadParallel개까지 동시에 청해요.
 * 실패: 없어요(타일마다의 실패는 세었다가 끝에 알려요).
 * 흐름:
 * 1. Job::setStep·setCancellable·setProgress — 단계 "타일 받는 중", 취소 가능, 진행 0으로 둬요.
 * 2. TileSource::fetch(pin) — 확대·행·열 순서로 다음 타일을 저장소에 고정하거나 서버에서 받아 고정해요.
 * 3. 콜백마다 끝난 수와 실패 수를 세고 Job::setProgress, 빈 자리만큼 다음 타일을 청해요.
 * 4. 다 끝나면 Job::succeed 또는 Job::fail(TileFailed)이에요.
 */
    void start();
private:
    bool nextTile(int& z, int& x, int& y);
    void fill();
    void tileDone(quint64 serial, const kgc::app::errors::Result<QByteArray>& result);
    void cancel();
    void finish();
    QPointer<TileSource> source_;
    settings::MapType type_;
    QString language_;
    int ratio_;
    QGeoRectangle area_;
    int maxZoom_;
    qint64 total_;
    qint64 done_ = 0;
    qint64 failed_ = 0;
    bool stopped_ = false;
    int z_;
    int row_ = 0;
    int column_ = 0;
    TileRange range_{};
    quint64 nextSerial_ = 0;
    QHash<quint64, TileTicket> open_;
    kgc::app::job::Job* job_;
};
}
