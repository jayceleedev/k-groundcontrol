// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <QObject>
#include <QString>
#include <limits>
namespace kgc::vehicle::states {
/** 시동 여부와 비행 모드를 함께 전달해요.
 * 입력: HEARTBEAT의 base_mode·custom_mode와 플러그인의 모드 이름이에요.
 * 출력: armed·modeId·modeName이며, 수신 전은 거짓·-1·빈 문자열이에요.
 * 실패: 없어요.
 */
struct ArmingBundle {
    Q_GADGET
    Q_PROPERTY(bool armed MEMBER armed)
    Q_PROPERTY(qint64 modeId MEMBER modeId)
    Q_PROPERTY(QString modeName MEMBER modeName)
public:
    bool armed = false;
    qint64 modeId = -1;
    QString modeName = {};
};
}
Q_DECLARE_METATYPE(kgc::vehicle::states::ArmingBundle)
