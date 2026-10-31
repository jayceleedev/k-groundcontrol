// [0.1.1 T1 T2 T3] specs/features/0.1.1-link-hardening.md
// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <QObject>
#include <QString>
#include <limits>
namespace kgc::vehicle::states {
/** 연결 상태와 마지막 수신 시각을 전달해요.
 * 입력: 자동조종기 메시지 수신 여부와 단조 시계의 밀리초 시각이에요.
 * 출력: connected·lost와 lastMessageMs이며, 수신 전은 거짓·거짓·-1이에요.
 * 실패: 없어요.
 */
struct LinkBundle {
    Q_GADGET
    Q_PROPERTY(bool connected MEMBER connected)
    Q_PROPERTY(bool lost MEMBER lost)
    Q_PROPERTY(qint64 lastMessageMs MEMBER lastMessageMs)
public:
    bool connected = false;
    bool lost = false;
    qint64 lastMessageMs = -1;
};
}
Q_DECLARE_METATYPE(kgc::vehicle::states::LinkBundle)
