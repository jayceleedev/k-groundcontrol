// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <kgc/comm/links/Link.h>
#include <kgc/comm/links/LinkConfig.h>
namespace kgc::comm::links {
/** 시리얼 입출력을 링크 워커 안에 가둬요.
 * 입력: 시리얼 연결 값과 송수신 바이트예요.
 * 출력: Link의 열기·수신·종료 시그널을 제공해요.
 * 실패: 열기 오류는 failed, 열린 포트의 오류는 closed의 PortLost로 전달해요.
 */
class SerialLink final : public Link {
public:
/** 포트를 열기 전에 연결 값을 준비해요.
 * 입력: 포트 이름과 속도가 담긴 config예요.
 * 출력: 아직 열리지 않은 링크 객체예요.
 * 실패: 없어요.
 */
    explicit SerialLink(LinkConfig config);
/** 워커 스레드에서 자원을 정리해요.
 * 입력: 없어요.
 * 출력: 파서와 자식 포트 객체를 해제해요.
 * 실패: 없어요.
 */
    ~SerialLink() override;
/** 워커에서 시리얼 포트를 열어요.
 * 입력: 생성자에 넘긴 포트 이름과 속도예요.
 * 출력: 열기에 성공하면 opened를 내보내요.
 * 실패: 권한 오류는 PermissionDenied, 점유 오류는 PortBusy, 나머지는 PortOpenFailed를 failed로 알려요.
 */
    void open() override;
/** 포트를 닫고 종료를 알려요.
 * 입력: 없어요.
 * 출력: closed(nullopt)를 내보내요. 이미 닫힌 시리얼은 다시 알리지 않아요.
 * 실패: 없어요.
 */
    void close() override;
/** 완성된 프레임을 포트에 써요.
 * 입력: 송신할 bytes예요.
 * 출력: 열린 포트에 바이트를 넘겨요.
 * 실패: 쓰기 오류는 closed의 PortLost로 알려요. 열리지 않았거나 닫힌 포트는 쓰지 않아요.
 */
    void writeBytes(const QByteArray& bytes) override;
private:
    struct Private;
    std::unique_ptr<Private> d_;
};
}
