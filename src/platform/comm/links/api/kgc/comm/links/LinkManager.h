// [0.4.1 5.1] specs/features/0.4.1-board-match.md
// [0.2.0 5.5] specs/features/0.2.0-fcflash.md
// [0.1.1 T4 T5 T13 T15 T16] specs/features/0.1.1-link-hardening.md
// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <kgc/comm/links/LinkConfig.h>
#include <kgc/comm/mavlink/Mavlink.h>
#include <kgc/app/errors/Result.h>
#include <QObject>
#include <QStringList>
#include <memory>
#include <optional>
namespace kgc::app::scheduler { class Scheduler; }
namespace kgc::comm::rawport { class PortRegistry; }
namespace kgc::comm::links {
/** 포트 목록의 제조사와 제품 설명, USB 장치가 알린 이름을 묶어요.
 * 입력: 운영체제가 알려 준 시리얼 포트 정보예요.
 * 출력: manufacturer와 product, 그리고 USB 장치가 스스로 알린 제품 이름 usbName(앞뒤 공백만 지우고 그대로)이며, 모르면 빈 값이에요.
 * 실패: 없어요. USB가 아닌 포트의 usbName은 빈 값이에요.
 */
struct PortInfo { QString manufacturer; QString product; QString usbName; };
/** 하나의 링크와 포트 목록을 GUI 스레드에서 관리해요.
 * 입력: 포트 조회용 스케줄러와 연결·해제 요청이에요.
 * 출력: 포트 목록과 링크의 열기·수신·종료 결과를 제공하며, 사용 중인 포트가 목록과 /dev/ 장치 파일 양쪽에서 사라지면 종료해요.
 * 실패: 중복 연결은 반환 오류이며 비동기 열기·통신 오류는 시그널로 알려요.
 */
class LinkManager : public QObject {
    Q_OBJECT
public:
/** 포트 목록 갱신 작업과 큐드 전달 형식을 준비해요.
 * 입력: scheduler, 포트 소유자를 기록하는 registry, 목록에 추가할 extraSerial 경로 목록, 선택적인 parent예요.
 * 출력: 초기 포트 목록과 1Hz 갱신·포트 유실 확인 작업을 준비해요.
 * 실패: 같은 스케줄러에 serialPorts 이름이 이미 있으면 프로그래밍 오류예요.
 */
    explicit LinkManager(kgc::app::scheduler::Scheduler& scheduler,
                         kgc::comm::rawport::PortRegistry& registry,
                         QStringList extraSerial = {}, QObject* parent = nullptr);
/** 워커가 끝난 뒤 링크 자원을 정리해요.
 * 입력: 없어요.
 * 출력: 이벤트 루프를 멈추고 워커 스레드의 종료를 기다려요.
 * 실패: 없어요.
 */
    ~LinkManager() override;
/** 워커에서 링크 열기를 시작해요.
 * 입력: UDP 또는 시리얼 연결 값 config예요.
 * 출력: 시작을 접수하면 성공을 반환하며, 이후 linkClosed 또는 linkFailed가 정확히 한 번 나와요.
 * 실패: 워커가 이미 있으면 AlreadyConnected, 시리얼 포트에 주인이 있으면 PortBusy를 바로 반환해요. 실제 열기 실패는 linkFailed로 알려요.
 * 흐름:
 * 1. LinkManager::connectLink — 기존 워커 유무를 확인하고 PortRegistry::claim으로 시리얼 포트를 점유해요.
 * 2. QThread::start → Link::open — 새 워커에서 통로를 열고 결과를 GUI 스레드로 받아요.
 * 3. QThread::finished — 워커를 정리하고 PortRegistry::release로 점유를 놓은 뒤 linkClosed 또는 linkFailed로 알려요.
 */
    kgc::app::errors::Result<void> connectLink(const LinkConfig& config);
/** 워커에 닫기를 요청하고 열기 중인 요청도 취소해요.
 * 입력: 없어요.
 * 출력: 반환값 없이 취소를 기록하며, 진행 중인 연결은 linkClosed(id, nullopt)로 한 번 끝나요.
 * 실패: 종료 전 도착한 열기 오류도 취소 결과로 바꾸며 linkFailed는 내보내지 않아요. 연결이 없으면 아무 일도 하지 않아요.
 * 흐름:
 * 1. LinkManager::disconnectLink — 취소를 기록하고 대기 중인 실패 결과를 지워요.
 * 2. QMetaObject::invokeMethod → Link::close — 아직 종료 중이 아닌 워커에만 닫기를 예약해요.
 * 3. QThread::finished — 워커가 멈춘 뒤 linkClosed로 정상 취소를 알려요.
 */
    void disconnectLink();
/** 송신 문이 넘긴 바이트를 워커로 예약해요.
 * 입력: 현재 연결 번호 linkId와 프레임 bytes예요.
 * 출력: 큐드 호출을 예약하면 참이에요.
 * 실패: 열린 링크가 없거나 번호가 다르거나 종료 중이거나 예약이 실패하면 거짓이에요.
 */
    bool writeBytes(int linkId, const QByteArray& bytes);
/** 마지막으로 접수한 링크의 연결 값을 복사해요.
 * 입력: 없어요.
 * 출력: 마지막 연결의 LinkConfig이며, 아직 접수한 적이 없으면 기본값이에요.
 * 실패: 없어요.
 */
    LinkConfig linkConfig() const;
/** 현재 링크 번호를 읽어요.
 * 입력: 없어요.
 * 출력: 열기·연결·종료 중인 링크의 번호이며, 워커가 없으면 -1이에요.
 * 실패: 없어요.
 */
    int linkId() const;
/** 송신 패킹에 사용할 링크 채널을 읽어요.
 * 입력: 없어요.
 * 출력: 단일 링크 송신에 배정한 MAVLink 채널 번호예요.
 * 실패: 없어요.
 */
    std::uint8_t channel() const;
/** 포트 이름과 설명의 쌍을 읽어요.
 * 입력: 없어요.
 * 출력: 마지막 조회에서 얻은 포트 목록이며, extraSerial의 각 경로는 파일이 있을 때만 끝에 포함해요.
 * 실패: 없어요.
 */
    QList<QPair<QString, QString>> ports() const;
/** 목록을 즉시 갱신해 포트의 등장과 유실을 확인해요.
 * 입력: 없어요.
 * 출력: 목록이 바뀌면 portsChanged를 내요.
 * 실패: 사용 중인 포트가 사라지면 닫기를 예약해요.
 */
    void refreshPorts();
/** 포트의 제조사와 제품 설명, USB 장치 이름을 읽어요.
 * 입력: 포트 이름 name이에요.
 * 출력: 운영체제가 알려 준 PortInfo이며, 없으면 빈 값이에요. usbName은 Linux는 usbNameFromSysfs("/sys", 포트 이름),
 *       Windows는 usbNameWindows(COM 이름)로 읽고, 그 밖의 운영체제는 빈 값이에요.
 * 실패: 없어요.
 */
    PortInfo portInfo(QString name);
signals:
/** 취소되지 않은 링크가 열렸음을 알려요.
 * 입력: 열린 연결의 linkId예요.
 * 출력: linkOpened 시그널이며, 열기 중 취소하면 생략할 수 있어요.
 * 실패: 없어요.
 */
    void linkOpened(int linkId);
/** 취소하지 않은 연결의 열기 실패를 한 번 알려요.
 * 입력: 실패한 열기의 ErrorCode인 error예요.
 * 출력: 워커 종료 뒤 나오는 끝 신호이며 같은 연결에 linkClosed는 나오지 않아요.
 * 실패: 열기 중 사용자가 끊었다면 이 신호 대신 linkClosed(id, nullopt)가 나와요.
 */
    void linkFailed(kgc::app::errors::ErrorCode error);
/** 연결의 정상 종료나 통신 오류를 한 번 알려요.
 * 입력: 종료한 linkId와 선택적인 오류 error예요.
 * 출력: 워커 종료 뒤 나오는 끝 신호이며 같은 연결에 linkFailed는 나오지 않아요.
 * 실패: 사용자 취소는 nullopt예요. 열기 중 취소하면 linkOpened 없이 나올 수 있어요. 시리얼의 NoError·TimeoutError 이외 오류, 음수 쓰기 결과, 목록과 /dev/ 장치 파일 양쪽에서의 유실은 PortLost를 담아요.
 */
    void linkClosed(int linkId, std::optional<kgc::app::errors::ErrorCode> error);
/** 워커에서 해석한 메시지를 GUI 스레드로 전달해요.
 * 입력: 수신한 linkId와 완성 메시지 message예요.
 * 출력: 열려 있고 종료 중이 아닌 링크의 messageReceived 시그널이에요.
 * 실패: 없어요.
 */
    void messageReceived(int linkId, mavlink_message_t message);
/** 선택 가능한 포트 목록이 달라졌음을 알려요.
 * 입력: 없어요.
 * 출력: ports를 다시 읽도록 알리는 portsChanged 시그널이에요.
 * 실패: 없어요.
 */
    void portsChanged();
private:
    struct Private;
    std::unique_ptr<Private> d_;
};
}
