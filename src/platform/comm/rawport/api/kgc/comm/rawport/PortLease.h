// [0.2.0 T4] specs/features/0.2.0-fcflash.md
#pragma once
#include <kgc/comm/rawport/PortRegistry.h>
#include <QPointer>
namespace kgc::comm::rawport {
/** 비어 있는 포트를 잠시 빌린 기록이에요.
 * 입력: 등록부와 포트 이름이에요. GUI 스레드에서만 써요.
 * 출력: 살아 있는 동안 그 포트의 주인이 Lease예요. 이동만 되고, 소멸하면 놓아요.
 * 실패: 링크가 쥔 포트는 PortError::Busy예요. 링크 일시 정지는 뒤 버전이에요.
 */
class PortLease {
public:
/** 포트를 빌려요.
 * 입력: 등록부 registry와 포트 이름 portName이에요.
 * 출력: 주인이 Lease가 된 임대예요.
 * 실패: 이미 주인이 있으면 PortError::Busy예요.
 */
    static kgc::app::errors::Result<PortLease> acquire(PortRegistry& registry, QString portName);
    PortLease(PortLease&& other) noexcept;
    PortLease& operator=(PortLease&& other) noexcept;
    PortLease(const PortLease&) = delete;
    PortLease& operator=(const PortLease&) = delete;
/** 임대를 놓아요.
 * 입력: 없어요.
 * 출력: 등록부의 주인을 None으로 되돌려요. 이동해 나간 객체는 아무 일도 하지 않아요.
 * 실패: 없어요.
 */
    ~PortLease();
private:
    PortLease(PortRegistry& registry, PortRegistry::Token token);
    QPointer<PortRegistry> registry_;
    PortRegistry::Token token_;
};
}
