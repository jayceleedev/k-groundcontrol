// [0.4.1 5.5] specs/features/0.4.1-board-match.md
// [0.2.0 T14–T23] specs/features/0.2.0-fcflash.md
#pragma once
#include <kgc/fcFlash/FcFlashService.h>
#include <QPointer>
#include <QVariantList>
#include <QQmlEngine>
namespace kgc::fcFlash {
/** 굽기 흐름의 값을 QML에 옮기고 서비스를 불러요.
 * 입력: 전역 서비스예요.
 * 출력: 서비스 값을 그대로 낸 표시값이에요. 판정은 서비스가 맡아요.
 * 실패: 없어요.
 */
class FcFlashController : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(kgc::fcFlash::Step step READ step NOTIFY changed)
    Q_PROPERTY(kgc::fcFlash::Phase phase READ phase NOTIFY changed)
    Q_PROPERTY(int secondsLeft READ secondsLeft NOTIFY changed)
    Q_PROPERTY(bool canRetry READ canRetry NOTIFY changed)
    Q_PROPERTY(QStringList pickPorts READ pickPorts NOTIFY changed)
    Q_PROPERTY(QString boardManufacturer READ boardManufacturer NOTIFY changed)
    Q_PROPERTY(QString boardProduct READ boardProduct NOTIFY changed)
    Q_PROPERTY(int boardId READ boardId NOTIFY changed)
    Q_PROPERTY(int flashKb READ flashKb NOTIFY changed)
    Q_PROPERTY(int bootloaderRevision READ bootloaderRevision NOTIFY changed)
    Q_PROPERTY(kgc::fcFlash::FirmwareSource source READ source NOTIFY changed)
    Q_PROPERTY(QString vehicle READ vehicle NOTIFY changed)
    Q_PROPERTY(QStringList versions READ versions NOTIFY changed)
    Q_PROPERTY(int versionIndex READ versionIndex NOTIFY changed)
    Q_PROPERTY(QStringList candidates READ candidates NOTIFY changed)
    Q_PROPERTY(int candidateIndex READ candidateIndex NOTIFY changed)
    Q_PROPERTY(QString usbName READ usbName NOTIFY changed)
    Q_PROPERTY(kgc::fcFlash::BoardMatch matchKind READ matchKind NOTIFY changed)
    Q_PROPERTY(QList<int> matchedCandidates READ matchedCandidates NOTIFY changed)
    Q_PROPERTY(QVariantList otherBoards READ otherBoards NOTIFY changed)
    Q_PROPERTY(bool showOtherBoards READ showOtherBoards WRITE setShowOtherBoards NOTIFY showOtherBoardsChanged)
    Q_PROPERTY(bool listLoading READ listLoading NOTIFY changed)
    Q_PROPERTY(bool listFailed READ listFailed NOTIFY changed)
    Q_PROPERTY(bool canContinue READ canContinue NOTIFY changed)
    Q_PROPERTY(QString filePath READ filePath NOTIFY changed)
    Q_PROPERTY(QString firmwareName READ firmwareName NOTIFY changed)
    Q_PROPERTY(QString expectedVersion READ expectedVersion NOTIFY changed)
    Q_PROPERTY(QString firmwareBoard READ firmwareBoard NOTIFY changed)
    Q_PROPERTY(kgc::app::job::Job* job READ job NOTIFY changed)
    Q_PROPERTY(QString vehicleVersion READ vehicleVersion NOTIFY changed)
    Q_PROPERTY(QString vehicleFirmwareName READ vehicleFirmwareName NOTIFY changed)
    Q_PROPERTY(int flashStage READ flashStage NOTIFY changed)
    Q_PROPERTY(QVariantList ports READ ports NOTIFY portsChanged)
public:
/** 서비스를 부모로 삼아 화면 수명을 묶어요.
 * 입력: 서비스예요.
 * 출력: 서비스가 소유하는 컨트롤러예요.
 * 실패: 중복 등록은 프로그래밍 오류예요.
 */
    explicit FcFlashController(FcFlashService& service);
/** 등록된 객체를 QML 엔진에 제공해요.
 * 입력: QML 엔진과 스크립트 엔진이에요.
 * 출력: C++가 소유하는 싱글턴 포인터예요.
 * 실패: 등록 전 호출이나 다른 스레드의 엔진은 프로그래밍 오류예요.
 */
    static FcFlashController* create(QQmlEngine* engine, QJSEngine* scriptEngine);
    /** 입력: 없어요. 출력: 서비스의 단계예요. 실패: 없어요. */
    Step step() const { return service_.step(); }
    /** 입력: 없어요. 출력: 서비스의 Phase예요. 실패: 없어요. */
    Phase phase() const { return service_.phase(); }
/** 실패 글자를 현재 언어로 읽어요.
 * 입력: 없어요.
 * 출력: 서비스의 실패 글자이며, 없으면 빈 값이에요.
 * 실패: 없어요.
 */
    Q_INVOKABLE QString failureText() const { return service_.failureText(); }
    /** 화면 값을 전달해요. 입력: 없어요. 출력: 서비스의 secondsLeft 값이에요. 실패: 없어요. */
    int secondsLeft() const { return service_.secondsLeft(); }
    /** 화면 값을 전달해요. 입력: 없어요. 출력: 서비스의 canRetry 값이에요. 실패: 없어요. */
    bool canRetry() const { return service_.canRetry(); }
    /** 화면 값을 전달해요. 입력: 없어요. 출력: 서비스의 pickPorts 값이에요. 실패: 없어요. */
    QStringList pickPorts() const { return service_.pickPorts(); }
    /** 화면 값을 전달해요. 입력: 없어요. 출력: 서비스의 boardManufacturer 값이에요. 실패: 없어요. */
    QString boardManufacturer() const { return service_.boardManufacturer(); }
    /** 화면 값을 전달해요. 입력: 없어요. 출력: 서비스의 boardProduct 값이에요. 실패: 없어요. */
    QString boardProduct() const { return service_.boardProduct(); }
    /** 화면 값을 전달해요. 입력: 없어요. 출력: 서비스의 boardId 값이에요. 실패: 없어요. */
    int boardId() const { return service_.boardId(); }
    /** 화면 값을 전달해요. 입력: 없어요. 출력: 서비스의 flashKb 값이에요. 실패: 없어요. */
    int flashKb() const { return service_.flashKb(); }
    /** 화면 값을 전달해요. 입력: 없어요. 출력: 서비스의 bootloaderRevision 값이에요. 실패: 없어요. */
    int bootloaderRevision() const { return service_.bootloaderRevision(); }
    /** 화면 값을 전달해요. 입력: 없어요. 출력: 서비스의 source 값이에요. 실패: 없어요. */
    FirmwareSource source() const { return service_.source(); }
    /** 화면 값을 전달해요. 입력: 없어요. 출력: 서비스의 vehicle 값이에요. 실패: 없어요. */
    QString vehicle() const { return service_.vehicle(); }
    /** 화면 값을 전달해요. 입력: 없어요. 출력: 서비스의 versions 값이에요. 실패: 없어요. */
    QStringList versions() const { return service_.versions(); }
    /** 화면 값을 전달해요. 입력: 없어요. 출력: 서비스의 versionIndex 값이에요. 실패: 없어요. */
    int versionIndex() const { return service_.versionIndex(); }
    /** 화면 값을 전달해요. 입력: 없어요. 출력: 서비스의 candidates 값이에요. 실패: 없어요. */
    QStringList candidates() const { return service_.candidates(); }
    /** 화면 값을 전달해요. 입력: 없어요. 출력: 서비스의 candidateIndex 값이에요. 실패: 없어요. */
    int candidateIndex() const { return service_.candidateIndex(); }
    /** 화면 값을 전달해요. 입력: 없어요. 출력: 서비스의 usbName 값이에요. 실패: 없어요. */
    QString usbName() const { return service_.usbName(); }
    /** 화면 값을 전달해요. 입력: 없어요. 출력: 서비스의 matchKind 값이에요. 실패: 없어요. */
    BoardMatch matchKind() const { return service_.matchKind(); }
    /** 화면 값을 전달해요. 입력: 없어요. 출력: 서비스의 matchedCandidates 값이에요. 실패: 없어요. */
    QList<int> matchedCandidates() const { return service_.matchedCandidates(); }
    /** 화면 값을 전달해요. 입력: 없어요. 출력: 서비스의 otherBoards 값이에요. 실패: 없어요. */
    QVariantList otherBoards() const { return service_.otherBoards(); }
    /** 화면 상태를 읽어요. 입력: 없어요. 출력: "다른 보드 ID도 보기"가 켜졌으면 참이에요. 기본은 거짓이에요. 실패: 없어요. */
    bool showOtherBoards() const { return showOtherBoards_; }
/** "다른 보드 ID도 보기"를 켜거나 꺼요.
 * 입력: 새 값 value예요.
 * 출력: 값이 바뀌면 showOtherBoardsChanged를 내요. 컨트롤러가 앱 끝까지 살아 있어서 패널을 닫았다 열어도 값이 남아요.
 * 실패: 없어요.
 */
    void setShowOtherBoards(bool value);
    /** 화면 값을 전달해요. 입력: 없어요. 출력: 서비스의 listLoading 값이에요. 실패: 없어요. */
    bool listLoading() const { return service_.listLoading(); }
    /** 화면 값을 전달해요. 입력: 없어요. 출력: 서비스의 listFailed 값이에요. 실패: 없어요. */
    bool listFailed() const { return service_.listFailed(); }
    /** 화면 값을 전달해요. 입력: 없어요. 출력: 서비스의 canContinue 값이에요. 실패: 없어요. */
    bool canContinue() const { return service_.canContinue(); }
    /** 화면 값을 전달해요. 입력: 없어요. 출력: 서비스의 filePath 값이에요. 실패: 없어요. */
    QString filePath() const { return service_.filePath(); }
    /** 화면 값을 전달해요. 입력: 없어요. 출력: 서비스의 firmwareName 값이에요. 실패: 없어요. */
    QString firmwareName() const { return service_.firmwareName(); }
    /** 화면 값을 전달해요. 입력: 없어요. 출력: 서비스의 expectedVersion 값이에요. 실패: 없어요. */
    QString expectedVersion() const { return service_.expectedVersion(); }
    /** 화면 값을 전달해요. 입력: 없어요. 출력: 서비스의 firmwareBoard 값이에요. 실패: 없어요. */
    QString firmwareBoard() const { return service_.firmwareBoard(); }
    /** 화면 값을 전달해요. 입력: 없어요. 출력: 서비스의 job 값이에요. 실패: 없어요. */
    kgc::app::job::Job* job() const { return service_.job(); }
    /** 화면 값을 전달해요. 입력: 없어요. 출력: 서비스의 vehicleVersion 값이에요. 실패: 없어요. */
    QString vehicleVersion() const { return service_.vehicleVersion(); }
    /** 화면 값을 전달해요. 입력: 없어요. 출력: 서비스의 vehicleFirmwareName 값이에요. 실패: 없어요. */
    QString vehicleFirmwareName() const { return service_.vehicleFirmwareName(); }
    /** 화면 값을 전달해요. 입력: 없어요. 출력: 서비스의 flashStage 값이에요. 실패: 없어요. */
    int flashStage() const { return service_.flashStage(); }
    /** 포트 목록을 화면 형식으로 바꿔요. 입력: 없어요. 출력: 이름과 설명 목록이에요. 실패: 없어요. */
    QVariantList ports() const;
    /** 보드 확인을 요청해요.
     * 입력: 화면에서 고른 값이에요.
     * 출력: 서비스 상태가 바뀌면 changed로 알려요.
     * 실패: 서비스의 실패 문구로 알려요.
     * 흐름:
     * 1. FcFlashService::beginProbe — 보드 확인을 요청해요.
     */
    Q_INVOKABLE void beginProbe(const QString& portName) { service_.beginProbe(portName); }
    /** 응답한 포트를 골라요.
     * 입력: 화면에서 고른 값이에요.
     * 출력: 서비스 상태가 바뀌면 changed로 알려요.
     * 실패: 서비스의 실패 문구로 알려요.
     * 흐름:
     * 1. FcFlashService::pickPort — 응답한 포트를 골라요.
     */
    Q_INVOKABLE void pickPort(const QString& portName) { service_.pickPort(portName); }
    /** 보드 확인을 다시 요청해요.
     * 입력: 없어요.
     * 출력: 서비스 상태가 바뀌면 changed로 알려요.
     * 실패: 서비스의 실패 문구로 알려요.
     * 흐름:
     * 1. FcFlashService::retryProbe — 보드 확인을 다시 요청해요.
     */
    Q_INVOKABLE void retryProbe() { service_.retryProbe(); }
    /** 펌웨어 출처를 골라요.
     * 입력: 화면에서 고른 값이에요.
     * 출력: 서비스 상태가 바뀌면 changed로 알려요.
     * 실패: 서비스의 실패 문구로 알려요.
     * 흐름:
     * 1. FcFlashService::chooseSource — 펌웨어 출처를 골라요.
     */
    Q_INVOKABLE void chooseSource(FirmwareSource source) { service_.chooseSource(source); }
    /** 기체 종류를 골라요.
     * 입력: 화면에서 고른 값이에요.
     * 출력: 서비스 상태가 바뀌면 changed로 알려요.
     * 실패: 서비스의 실패 문구로 알려요.
     * 흐름:
     * 1. FcFlashService::chooseVehicle — 기체 종류를 골라요.
     */
    Q_INVOKABLE void chooseVehicle(const QString& name) { service_.chooseVehicle(name); }
    /** 펌웨어 버전을 골라요.
     * 입력: 화면에서 고른 값이에요.
     * 출력: 서비스 상태가 바뀌면 changed로 알려요.
     * 실패: 서비스의 실패 문구로 알려요.
     * 흐름:
     * 1. FcFlashService::chooseVersion — 펌웨어 버전을 골라요.
     */
    Q_INVOKABLE void chooseVersion(int index) { service_.chooseVersion(index); }
    /** 펌웨어 후보를 골라요.
     * 입력: 화면에서 고른 값이에요.
     * 출력: 서비스 상태가 바뀌면 changed로 알려요.
     * 실패: 서비스의 실패 문구로 알려요.
     * 흐름:
     * 1. FcFlashService::chooseCandidate — 펌웨어 후보를 골라요.
     */
    Q_INVOKABLE void chooseCandidate(int index) { service_.chooseCandidate(index); }
    /** 펌웨어 파일을 골라요.
     * 입력: 화면에서 고른 값이에요.
     * 출력: 서비스 상태가 바뀌면 changed로 알려요.
     * 실패: 서비스의 실패 문구로 알려요.
     * 흐름:
     * 1. FcFlashService::chooseFile — 펌웨어 파일을 골라요.
     */
    Q_INVOKABLE void chooseFile(const QString& path) { service_.chooseFile(path); }
    /** 확인 화면으로 이동해요.
     * 입력: 없어요.
     * 출력: 서비스 상태가 바뀌면 changed로 알려요.
     * 실패: 서비스의 실패 문구로 알려요.
     * 흐름:
     * 1. FcFlashService::toConfirm — 확인 화면으로 이동해요.
     */
    Q_INVOKABLE void toConfirm() { service_.toConfirm(); }
    /** 선택 화면으로 돌아가요.
     * 입력: 없어요.
     * 출력: 서비스 상태가 바뀌면 changed로 알려요.
     * 실패: 서비스의 실패 문구로 알려요.
     * 흐름:
     * 1. FcFlashService::backToFirmware — 선택 화면으로 돌아가요.
     */
    Q_INVOKABLE void backToFirmware() { service_.backToFirmware(); }
    /** 굽기를 요청해요.
     * 입력: 없어요.
     * 출력: 서비스 상태가 바뀌면 changed로 알려요.
     * 실패: 서비스의 실패 문구로 알려요.
     * 흐름:
     * 1. FcFlashService::startFlash — 굽기를 요청해요.
     */
    Q_INVOKABLE void startFlash() { service_.startFlash(); }
    /** 지우기 전의 굽기를 취소해요.
     * 입력: 없어요.
     * 출력: 서비스 상태가 바뀌면 changed로 알려요.
     * 실패: 서비스의 실패 문구로 알려요.
     * 흐름:
     * 1. FcFlashService::cancelFlash — 지우기 전의 굽기를 취소해요.
     */
    Q_INVOKABLE void cancelFlash() { service_.cancelFlash(); }
    /** 굽기를 다시 요청해요.
     * 입력: 없어요.
     * 출력: 서비스 상태가 바뀌면 changed로 알려요.
     * 실패: 서비스의 실패 문구로 알려요.
     * 흐름:
     * 1. FcFlashService::retryFlash — 굽기를 다시 요청해요.
     */
    Q_INVOKABLE void retryFlash() { service_.retryFlash(); }
    /** FC를 부팅하고 돌아가요.
     * 입력: 없어요.
     * 출력: 서비스 상태가 바뀌면 changed로 알려요.
     * 실패: 서비스의 실패 문구로 알려요.
     * 흐름:
     * 1. FcFlashService::bootAndReset — FC를 부팅하고 돌아가요.
     */
    Q_INVOKABLE void bootAndReset() { service_.bootAndReset(); }
    /** 부팅 없이 처음으로 돌아가요.
     * 입력: 없어요.
     * 출력: 서비스 상태가 바뀌면 changed로 알려요.
     * 실패: 서비스의 실패 문구로 알려요.
     * 흐름:
     * 1. FcFlashService::reset — 부팅 없이 처음으로 돌아가요.
     */
    Q_INVOKABLE void reset() { service_.reset(); }
signals:
/** 표시값을 다시 읽도록 알려요.
 * 입력: 없어요.
 * 출력: 변경 알림이에요.
 * 실패: 없어요.
 */
    void changed();
    /** 포트 목록 갱신을 알려요. 입력: 없어요. 출력: 목록 변경 알림이에요. 실패: 없어요. */
    void portsChanged();
    /** 화면 상태 변경을 알려요. 입력: 없어요. 출력: showOtherBoards 변경 알림이에요. 실패: 없어요. */
    void showOtherBoardsChanged();
private:
    FcFlashService& service_;
    bool showOtherBoards_ = false;
    inline static QPointer<FcFlashController> instance_;
};
}
