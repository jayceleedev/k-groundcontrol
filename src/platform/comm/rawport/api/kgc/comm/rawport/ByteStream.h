// [0.2.0 T7] specs/features/0.2.0-fcflash.md
#pragma once
#include <kgc/app/errors/Result.h>
#include <QByteArray>
#include <chrono>
namespace kgc::comm::rawport {
/** 작업 스레드에서 막아서 쓰는 바이트 통로예요.
 * 입력: 구현체가 잡은 포트나 패스스루 통로예요.
 * 출력: 바이트 읽기·쓰기와 선택적인 제어선 함수예요.
 * 실패: 포트 오류는 PortError::Lost, 시간 초과는 PortError::Timeout이에요.
 */
class ByteStream {
public:
    virtual ~ByteStream() = default;
/** 바이트를 모두 보낼 때까지 막아요.
 * 입력: 보낼 bytes예요.
 * 출력: 다 보내면 성공이에요.
 * 실패: 포트를 잃으면 PortError::Lost예요.
 */
    virtual kgc::app::errors::Result<void> write(QByteArray bytes) = 0;
/** 바이트가 올 때까지 막아요.
 * 입력: 많아야 받을 count와 기다릴 timeout이에요.
 * 출력: count를 다 채우거나 timeout이 지나면 돌아와요. 시간이 지났어도 하나라도 받았으면 그만큼을 돌려줘요.
 *       그래서 결과는 count보다 짧을 수 있고, 부르는 쪽이 이어 붙여요.
 * 실패: 한 바이트도 못 받으면 PortError::Timeout, 포트를 잃으면 PortError::Lost예요.
 */
    virtual kgc::app::errors::Result<QByteArray> read(int count, std::chrono::milliseconds timeout) = 0;
/** 받아 둔 바이트를 버려요.
 * 입력: 없어요.
 * 출력: 다음 read는 이 뒤에 온 바이트부터 돌려줘요.
 * 실패: 없어요.
 */
    virtual void clearInput() = 0;
/** DTR·RTS를 움직일 수 있는 통로인지 알려요.
 * 입력: 없어요.
 * 출력: 움직일 수 있으면 참이에요. 패스스루 래퍼는 거짓이에요.
 * 실패: 없어요.
 */
    virtual bool canControlLine() const = 0;
/** 통신 속도를 바꿔요.
 * 입력: 새 속도 baud예요.
 * 출력: 바꾸면 성공이에요.
 * 실패: 포트를 잃으면 PortError::Lost예요.
 */
    virtual kgc::app::errors::Result<void> setBaud(int baud) = 0;
/** DTR·RTS 선을 바꿔요.
 * 입력: 각 선의 새 값 dtr·rts예요.
 * 출력: 바꾸면 성공이에요.
 * 실패: 포트를 잃으면 PortError::Lost예요.
 */
    virtual kgc::app::errors::Result<void> setDtrRts(bool dtr, bool rts) = 0;
};
}
