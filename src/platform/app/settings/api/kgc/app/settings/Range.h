// [0.0.2 T20] specs/features/0.0.2-theme.md
#pragma once
#include <QJsonObject>
#include <QVariant>
namespace kgc::app::settings {
/** 숫자 설정의 값이 정의된 범위 안에 있는지 판단해요.
 * 입력: 설정 정의와 검사할 값.
 * 출력: 경계를 포함한 범위 안이면 참, 숫자가 아닌 설정은 참.
 * 실패: 숫자로 변환할 수 없거나 유한하지 않은 숫자는 거짓.
 */
bool withinRange(const QJsonObject& definition, const QVariant& value);
}
