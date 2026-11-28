// [0.6.0 5.4] specs/features/0.6.0-fly.md
#pragma once
#include <QObject>
#include <QString>
namespace kgc::vehicle::states {
/** 기체가 보낸 글 메시지(STATUSTEXT) 하나예요.
 * 입력: STATUSTEXT의 severity와 text예요.
 * 출력: severity는 MAV_SEVERITY 값(0 비상 ~ 7 디버그, 기본 7), text는 UTF-8로 푼 글이에요.
 * 실패: 없어요.
 */
struct VehicleMessage {
    Q_GADGET
    Q_PROPERTY(int severity MEMBER severity)
    Q_PROPERTY(QString text MEMBER text)
public:
    int severity = 7;
    QString text;
};
}
Q_DECLARE_METATYPE(kgc::vehicle::states::VehicleMessage)
