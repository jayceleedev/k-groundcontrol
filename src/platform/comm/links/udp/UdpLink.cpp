// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include <kgc/comm/links/UdpLink.h>
#include <kgc/comm/links/Errors.h>
#include "Constants.h"
#include <QUdpSocket>
#include <QNetworkDatagram>
namespace kgc::comm::links {
struct UdpLink::Private {
    QUdpSocket* socket = nullptr;
    QHostAddress peer;
    quint16 port = 0;
};
UdpLink::UdpLink(LinkConfig) : d_(std::make_unique<Private>()) {}
UdpLink::~UdpLink() = default;
void UdpLink::open()
{
    d_->socket = new QUdpSocket(this);
    connect(d_->socket, &QUdpSocket::readyRead, this, [this] {
        while (d_->socket->hasPendingDatagrams()) {
            const auto datagram = d_->socket->receiveDatagram();
            if (!datagram.isValid()) break;
            d_->peer = datagram.senderAddress();
            d_->port = datagram.senderPort();
            receiveBytes(datagram.data());
        }
    });
    if (!d_->socket->bind(QHostAddress::AnyIPv4, kUdpPort, QUdpSocket::DontShareAddress)) {
        emit failed(make(LinkError::UdpBindFailed));
        return;
    }
    emit opened();
}
void UdpLink::close()
{
    if (d_->socket) d_->socket->close();
    emit closed(std::nullopt);
}
void UdpLink::writeBytes(const QByteArray& bytes)
{
    if (d_->socket && !d_->peer.isNull()) d_->socket->writeDatagram(bytes, d_->peer, d_->port);
}
}
