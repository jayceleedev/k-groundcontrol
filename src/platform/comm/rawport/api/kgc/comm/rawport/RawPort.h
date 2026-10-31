// [0.2.0 T14 T19] specs/features/0.2.0-fcflash.md
#pragma once
#include <kgc/comm/rawport/ByteStream.h>
#include <QString>
#include <memory>
namespace kgc::comm::rawport {
/** 실제 시리얼 포트를 ByteStream으로 내놓아요.
 * 입력: 포트 이름과 속도예요. 임대(PortLease)를 쥔 쪽만 열어요.
 * 출력: 만든 스레드에서만 쓰는 막는 바이트 통로예요. 8N1, 흐름 제어 없음으로 열어요.
 * 실패: 포트 오류는 PortError::Lost, 시간 초과는 PortError::Timeout이에요.
 */
class RawPort final : public ByteStream {
public:
/** 포트를 여는 스레드에서 RawPort를 만들어요.
 * 입력: 포트 이름 portName과 속도 baud예요.
 * 출력: 열린 RawPort예요.
 * 실패: 열 수 없으면 PortError::OpenFailed예요.
 */
    static kgc::app::errors::Result<std::unique_ptr<RawPort>> open(QString portName, int baud);
/** 포트를 닫아요.
 * 입력: 없어요.
 * 출력: 만든 스레드에서 포트를 닫아요.
 * 실패: 없어요.
 */
    ~RawPort() override;
    kgc::app::errors::Result<void> write(QByteArray bytes) override;
    kgc::app::errors::Result<QByteArray> read(int count, std::chrono::milliseconds timeout) override;
    void clearInput() override;
    bool canControlLine() const override;
    kgc::app::errors::Result<void> setBaud(int baud) override;
    kgc::app::errors::Result<void> setDtrRts(bool dtr, bool rts) override;
private:
    RawPort();
    struct Private;
    std::unique_ptr<Private> d_;
};
}
