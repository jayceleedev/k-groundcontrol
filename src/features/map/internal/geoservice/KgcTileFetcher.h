// [0.5.0 5.6.7] specs/features/0.5.0-map.md
#pragma once
#include "TileSource.h"
#include <QPointer>
#include <QtLocation/private/qgeotiledmapreply_p.h>
#include <QtLocation/private/qgeotilefetcher_p.h>
#include <QtLocation/private/qgeotilespec_p.h>
namespace kgc::map {
/** Qt Location의 타일 요청 하나를 타일 공급의 대기표 하나로 잇는 답이에요.
 * 입력: 타일 사양과 타일 공급이에요. 만들 때 공급의 지금 언어로 fetch(pin 거짓)해요.
 * 출력: 받은 바이트와 그림 형식을 넣고 끝나요.
 * 실패: 오류면 CommunicationError와 tileErrorName 글자(영어만)예요. Qt가 다시 요청하다 다섯 번 뒤 포기해요.
 */
class KgcTileReply : public QGeoTiledMapReply {
    Q_OBJECT
public:
/** 타일을 청해요.
 * 입력: 타일 사양 spec(지도 id 1~3), 타일 공급 source, 부모 parent예요.
 * 출력: 대기표를 들고 답을 기다리는 객체예요.
 * 실패: 없어요.
 */
    KgcTileReply(const QGeoTileSpec& spec, TileSource& source, QObject* parent);
/** 끝나지 않은 대기표를 거둬요.
 * 입력: 없어요.
 * 출력: 공급에 cancel을 불러요.
 * 실패: 없어요.
 */
    ~KgcTileReply() override;
/** Qt가 더는 필요 없는 타일을 멈춰요.
 * 입력: 없어요.
 * 출력: 대기표를 거두고 Qt 기본 abort를 불러요.
 * 실패: 없어요.
 */
    void abort() override;
private:
    void deliver(const kgc::app::errors::Result<QByteArray>& result);
    QPointer<TileSource> source_;
    TileTicket ticket_ = 0;
};
/** Qt Location의 타일 요청을 타일 공급으로 넘기는 fetcher예요.
 * 입력: 타일 공급과 지도 엔진이에요.
 * 출력: 타일마다 KgcTileReply예요.
 * 실패: 공급이 없거나 지도 id가 1~3이 아니면 이미 끝난 UnknownError 답이에요.
 */
class KgcTileFetcher : public QGeoTileFetcher {
    Q_OBJECT
public:
/** 공급을 약한 포인터로 들어요.
 * 입력: 타일 공급 source와 지도 엔진 parent예요.
 * 출력: 엔진이 소유하는 fetcher예요.
 * 실패: 없어요.
 */
    KgcTileFetcher(TileSource& source, QGeoMappingManagerEngine* parent);
private:
    QGeoTiledMapReply* getTileImage(const QGeoTileSpec& spec) override;
    QPointer<TileSource> source_;
};
}
