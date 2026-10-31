// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <kgc/app/errors/ErrorCode.h>
#include <QObject>
#include <QVariant>
#include <memory>
#include <optional>
namespace kgc::comm::links {
/** 전송 방식에 관계없이 워커가 여닫고 쓰는 경계를 제공해요.
 * 입력: 워커 스레드의 열기·닫기 요청과 송수신 바이트예요.
 * 출력: 열기·실패·종료 시그널과 해석한 수신 메시지를 제공해요.
 * 실패: 전송 방식의 열기 오류는 failed, 열린 포트의 유실은 closed로 전달해요.
 */
class Link : public QObject {
    Q_OBJECT
public:
/** 링크별 파싱 상태를 준비해요.
 * 입력: 수명을 함께할 부모 객체이며, 없으면 호출자가 소유해요.
 * 출력: 다른 링크와 수신 상태를 공유하지 않는 파서예요.
 * 실패: 없어요.
 */
    explicit Link(QObject* parent = nullptr);
/** 링크별 파싱 상태를 정리해요.
 * 입력: 없어요.
 * 출력: 파서와 자식 객체의 자원을 해제해요.
 * 실패: 없어요.
 */
    ~Link() override;
/** 워커 스레드에서 전송 통로를 열어요.
 * 입력: 생성 시 전송 방식에 전달한 연결 값이에요.
 * 출력: 성공하면 opened를 내보내요.
 * 실패: 열 수 없으면 공통 오류 코드를 failed로 내보내요.
 */
    virtual void open() = 0;
/** 워커 스레드에서 전송 통로를 닫아요.
 * 입력: 없어요.
 * 출력: 입출력을 멈추고 closed로 종료를 알려요.
 * 실패: 이미 닫힌 통로의 처리는 전송 방식이 맡으며 반환 오류는 없어요.
 */
    virtual void close() = 0;
/** 완성된 프레임을 워커 스레드에서 써요.
 * 입력: 전송할 바이트 배열 bytes예요.
 * 출력: 전송 방식의 출력 통로에 바이트를 넘겨요.
 * 실패: 시리얼 쓰기 오류는 closed의 PortLost로 알리고, UDP 상대가 없으면 버려요.
 */
    virtual void writeBytes(const QByteArray& bytes) = 0;
signals:
/** 열기에 성공했음을 소유자에게 알려요.
 * 입력: 없어요.
 * 출력: 입출력을 시작할 수 있음을 알리는 opened 시그널이에요.
 * 실패: 없어요.
 */
    void opened();
/** 열기 실패의 공통 오류 코드를 전달해요.
 * 입력: 전송 방식에서 얻은 열기 오류 error예요.
 * 출력: 관리자가 종료 결과를 결정할 수 있도록 failed 시그널을 내보내요.
 * 실패: 없어요.
 */
    void failed(kgc::app::errors::ErrorCode error);
/** 닫힘과 선택적인 오류를 전달해요.
 * 입력: 정상 닫힘이면 nullopt, 통신 유실이면 오류를 담은 error예요.
 * 출력: 관리자가 워커를 정지하도록 closed 시그널을 내보내요.
 * 실패: 없어요.
 */
    void closed(std::optional<kgc::app::errors::ErrorCode> error);
/** 방언 헤더를 노출하지 않고 완성 메시지를 전달해요.
 * 입력: mavlink_message_t를 담은 QVariant인 message예요.
 * 출력: 워커에서 해석하고 v1 제한을 통과한 메시지 시그널이에요.
 * 실패: 없어요.
 */
    void messageReceived(QVariant message);
protected:
/** 수신 바이트를 링크별 파서로 해석하고 허용된 프레임만 내보내요.
 * 입력: 전송 방식에서 받은 bytes예요.
 * 출력: 완성된 허용 프레임마다 messageReceived를 내보내요.
 * 실패: 손상 프레임과 v1의 하트비트 이외 메시지는 버려요.
 */
    void receiveBytes(const QByteArray& bytes);
private:
    struct Private;
    std::unique_ptr<Private> d_;
};
}
