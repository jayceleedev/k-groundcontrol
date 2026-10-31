// [0.1.1 T4 T13] specs/features/0.1.1-link-hardening.md
#pragma once
#include <QHostAddress>
#include <QList>
#include <QPair>
namespace kgc::comm::links {
/** UDP로 최근에 데이터를 보내온 주소를 기억해요.
 * 입력: 수신한 주소·포트와 같은 단조 시계의 밀리초 시각이에요.
 * 출력: 아직 살아 있다고 볼 주소 목록이에요.
 * 실패: 없어요.
 */
class UdpPeers {
public:
/** 주소를 더하거나 마지막 수신 시각을 갱신해요.
 * 입력: 보낸 쪽 address·port와 현재 시각 nowMs예요.
 * 출력: 없어요. 처음 보는 주소는 목록 끝에 붙어요.
 * 실패: 없어요.
 */
    void note(const QHostAddress& address, quint16 port, qint64 nowMs);
/** 오래된 주소를 지우고 남은 주소를 돌려줘요.
 * 입력: 현재 시각 nowMs예요.
 * 출력: 등록 순서대로의 주소 목록이며, 마지막 수신에서 10000ms 이상 지난 주소는 빠져요.
 * 실패: 없어요.
 */
    QList<QPair<QHostAddress, quint16>> active(qint64 nowMs);
private:
    struct Peer {
        QHostAddress address;
        quint16 port = 0;
        qint64 lastMs = 0;
    };
    QList<Peer> peers_;
};
}
