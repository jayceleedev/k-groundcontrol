// [0.2.0 T14–T23] specs/features/0.2.0-fcflash.md
#pragma once
#include <kgc/app/errors/Result.h>
#include "bootloader/Bootloader.h"
#include "image/FirmwareImage.h"
#include "sources/ArduPilotManifest.h"
#include <QList>
#include <QObject>
#include <QStringList>
#include <atomic>
#include <memory>
namespace kgc::fcFlash {
/** 포트 하나의 확인 결과예요.
 * 입력: FlashWorker가 채워요.
 * 출력: 응답한 포트 이름과 보드 정보예요.
 * 실패: 없어요.
 */
struct ProbeResult {
    QString portName;
    BoardInfo board;
};
/** 자기 스레드에서 RawPort와 Bootloader를 쥐고 막는 일을 해요.
 * 입력: 서비스가 큐드 호출하는 슬롯과 원자 중단 플래그예요.
 * 출력: 결과는 전부 큐드 시그널이에요. GUI 스레드를 막지 않아요.
 * 실패: 오류는 ErrorCode로 시그널에 실어 서비스에 올려요.
 */
class FlashWorker : public QObject {
    Q_OBJECT
public:
/** 작업자를 만들어요.
 * 입력: 서비스가 든 중단 플래그 abort예요.
 * 출력: moveToThread를 기다리는 작업자예요.
 * 실패: 없어요.
 */
    explicit FlashWorker(const std::atomic_bool& abort);
    ~FlashWorker() override;
public slots:
/** 후보 포트를 차례로 열어 sync·identify를 해요.
 * 입력: 임대를 쥔 portNames예요.
 * 출력: probed 시그널이에요. 응답한 포트는 열어 둬요.
 * 실패: 응답하지 않은 포트는 닫고 결과에서 빼요.
 */
    void probePorts(const QStringList& portNames);
/** 포트 하나만 남기고 부팅 없이 닫아요.
 * 입력: 남길 portName이에요.
 * 출력: 나머지 포트를 닫고 portClosed를 내요.
 * 실패: 없어요.
 */
    void keepPort(const QString& portName);
/** 쥔 포트를 부팅 없이 모두 닫아요.
 * 입력: 없어요.
 * 출력: 닫은 뒤 portClosed를 내요.
 * 실패: 없어요.
 */
    void closePorts();
/** 쥔 포트에 이미지를 구워요.
 * 입력: 해석한 image예요.
 * 출력: stageChanged·progressChanged를 내고 끝나면 flashFinished를 내요.
 * 실패: 실패해도 BOOT를 보내지 않아요.
 */
    void flash(const kgc::fcFlash::FirmwareImage& image);
/** BOOT를 보내고 포트를 닫아요.
 * 입력: 없어요.
 * 출력: 닫은 뒤 portClosed를 내요.
 * 실패: 없어요.
 */
    void bootAndClose();
/** 펌웨어 파일을 해석해요.
 * 입력: 파일 경로 path예요.
 * 출력: imageParsed 시그널이에요.
 * 실패: BadImage는 시그널의 결과로 알려요.
 */
    void parseImage(const QString& path);
/** gzip 목록을 풀고 보드에 맞는 항목을 골라요.
 * 입력: 받은 바이트 gzipped와 보드 ID boardId예요.
 * 출력: manifestScanned 시그널이에요.
 * 실패: 풀 수 없으면 빈 목록이에요.
 */
    void parseManifest(const QByteArray& gzipped, int boardId);
signals:
    /** 입력: 응답한 포트들. 출력: 후보 전부를 확인한 뒤 한 번 나와요. 실패: 없어요. */
    void probed(const QList<kgc::fcFlash::ProbeResult>& found);
    /** 입력: 시작한 단계. 출력: 단계가 바뀔 때 나와요. 실패: 없어요. */
    void stageChanged(kgc::fcFlash::BootloaderStage stage);
    /** 입력: 단계 안 진행률 0~1. 출력: 진행 중 여러 번 나와요. 실패: 없어요. */
    void progressChanged(double value);
    /** 입력: 굽기 결과. 출력: flash마다 한 번 나와요. 실패: 없어요. */
    void flashFinished(kgc::app::errors::Result<void> result);
    /** 입력: 없어요. 출력: 포트를 닫은 뒤 나와요. 서비스는 이 뒤에 임대를 놓아요. 실패: 없어요. */
    void portClosed();
    /** 입력: 없어요. 출력: 쥔 포트를 잃으면 나와요. 실패: 없어요. */
    void portLost();
    /** 입력: 해석 결과. 출력: parseImage마다 한 번 나와요. 실패: 없어요. */
    void imageParsed(kgc::app::errors::Result<kgc::fcFlash::FirmwareImage> image);
    /** 입력: 맞는 항목들. 출력: parseManifest마다 한 번 나와요. 실패: 없어요. */
    void manifestScanned(const QList<kgc::fcFlash::ManifestEntry>& entries);
private:
    struct Private;
    std::unique_ptr<Private> d_;
};
}
