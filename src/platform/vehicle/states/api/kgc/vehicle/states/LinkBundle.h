// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <QObject>
#include <QString>
#include <limits>
namespace kgc::vehicle::states {
/** 연결 상태와 마지막 하트비트 시각을 전달해요.
 * 입력: HEARTBEAT 수신 여부와 단조 시계의 밀리초 시각이에요.
 * 출력: connected·lost와 lastHeartbeatMs이며, 수신 전은 거짓·거짓·-1이에요.
 * 실패: 없어요.
 */
struct LinkBundle {
    Q_GADGET
    Q_PROPERTY(bool connected MEMBER connected)
    Q_PROPERTY(bool lost MEMBER lost)
    Q_PROPERTY(qint64 lastHeartbeatMs MEMBER lastHeartbeatMs)
public:
    bool connected = false;
    bool lost = false;
    qint64 lastHeartbeatMs = -1;
};
}
Q_DECLARE_METATYPE(kgc::vehicle::states::LinkBundle)
