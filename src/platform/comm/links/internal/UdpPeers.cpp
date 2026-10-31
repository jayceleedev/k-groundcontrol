// [0.1.1 T4 T13] specs/features/0.1.1-link-hardening.md
#include "UdpPeers.h"
#include "Constants.h"
namespace kgc::comm::links {
void UdpPeers::note(const QHostAddress& address, quint16 port, qint64 nowMs)
{
    for (auto& peer : peers_) {
        if (peer.address == address && peer.port == port) {
            peer.lastMs = nowMs;
            return;
        }
    }
    peers_.append({address, port, nowMs});
}
QList<QPair<QHostAddress, quint16>> UdpPeers::active(qint64 nowMs)
{
    peers_.removeIf([nowMs](const Peer& peer) {
        return nowMs - peer.lastMs >= kUdpPeerExpiry.count();
    });
    QList<QPair<QHostAddress, quint16>> result;
    for (const auto& peer : peers_) result.append({peer.address, peer.port});
    return result;
}
}
