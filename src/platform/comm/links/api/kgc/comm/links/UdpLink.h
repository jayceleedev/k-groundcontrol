// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <kgc/comm/links/Link.h>
#include <kgc/comm/links/LinkConfig.h>
namespace kgc::comm::links {
/** UDP 소켓과 마지막 송신자 주소를 링크 워커 안에 가둬요.
 * 입력: 송수신 데이터그램이며 수신 포트는 고정값을 사용해요.
 * 출력: Link의 열기·수신·종료 시그널을 제공해요.
 * 실패: 바인딩 실패는 failed의 UdpBindFailed로 전달해요.
 */
class UdpLink final : public Link {
public:
/** 포트를 열기 전에 연결 값을 준비해요.
 * 입력: 공통 생성 경계의 config이며 UDP는 고정 포트라 이 값을 쓰지 않아요.
 * 출력: 아직 열리지 않은 링크 객체예요.
 * 실패: 없어요.
 */
    explicit UdpLink(LinkConfig config);
/** 워커 스레드에서 자원을 정리해요.
 * 입력: 없어요.
 * 출력: 파서와 자식 포트 객체를 해제해요.
 * 실패: 없어요.
 */
    ~UdpLink() override;
/** 워커에서 UDP 수신 소켓을 열어요.
 * 입력: 고정된 수신 주소 0.0.0.0과 포트 14550을 사용해요.
 * 출력: 열기에 성공하면 opened를 내보내요.
 * 실패: 바인딩에 실패하면 UdpBindFailed를 failed로 알려요.
 */
    void open() override;
/** 포트를 닫고 종료를 알려요.
 * 입력: 없어요.
 * 출력: 소켓을 닫고 closed(nullopt)를 내보내요.
 * 실패: 없어요.
 */
    void close() override;
/** 완성된 프레임을 포트에 써요.
 * 입력: 송신할 bytes예요.
 * 출력: 마지막으로 유효한 데이터그램을 보낸 주소에 바이트를 보내요.
 * 실패: 상대 주소가 없으면 조용히 버리고, 데이터그램 쓰기 실패는 별도 신호로 알리지 않아요.
 */
    void writeBytes(const QByteArray& bytes) override;
private:
    struct Private;
    std::unique_ptr<Private> d_;
};
}
