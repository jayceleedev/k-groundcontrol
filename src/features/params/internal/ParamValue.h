// [0.3.0 6.4 6.8-1 T2] specs/features/0.3.0-params.md
#pragma once
#include <kgc/app/errors/Result.h>
#include <kgc/params/ParamMeta.h>
#include <QList>
#include <QString>
#include <cstdint>
namespace kgc::params {
/** 값을 형에 맞는 글자로 바꿔요.
 * 입력: 값 value와 MAV_PARAM_TYPE 번호 type이에요.
 * 출력: 정수형은 정수 표기, 실수형은 그 형이 왕복하는 가장 짧은 고정 소수점 표기예요. 100000은 100000으로 나와요.
 * 실패: 없어요.
 */
QString formatValue(double value, std::uint8_t type);
/** 값이 형에 담기는지 검사해요.
 * 입력: 값 value와 MAV_PARAM_TYPE 번호 type이에요.
 * 출력: 담기면 성공 결과예요.
 * 실패: 정수형에 소수면 NotInteger, 형의 범위 밖이면 TypeLimit이에요.
 */
kgc::app::errors::Result<void> validateForType(double value, std::uint8_t type);
/** 사용자가 넣은 글자를 값으로 바꿔요.
 * 입력: 글자 text와 MAV_PARAM_TYPE 번호 type이에요.
 * 출력: 앞뒤 공백을 지우고 쉼표를 점으로 읽은 값이에요.
 * 실패: 숫자가 아니거나 무한대면 NotANumber, 그 밖은 validateForType과 같아요.
 */
kgc::app::errors::Result<double> parseValue(const QString& text, std::uint8_t type);
/** 두 값이 FC에서 같은 값인지 판정해요.
 * 입력: 값 a·b와 MAV_PARAM_TYPE 번호 type이에요.
 * 출력: 정수형은 같을 때, 실수형은 float로 바꿔 kParamFloatUlps ULP 안일 때 참이에요.
 * 실패: 없어요.
 */
bool sameValue(double a, double b, std::uint8_t type);
/** 값의 형이 담는 비트 수를 읽어요.
 * 입력: MAV_PARAM_TYPE 번호 type이에요.
 * 출력: 8·16·32·64예요. 실수형은 32예요.
 * 실패: 없어요.
 */
int typeBits(std::uint8_t type);
/** 값의 비트 하나가 켜져 있는지 읽어요.
 * 입력: 값 value, MAV_PARAM_TYPE 번호 type, 비트 번호 bit예요.
 * 출력: 형의 폭 안에서 켜져 있으면 참이에요. 음수 INT32의 비트 31도 읽어요.
 * 실패: 폭 밖의 비트는 거짓이에요.
 */
bool bitIsSet(double value, std::uint8_t type, int bit);
/** 목록에 있는 비트만 바꾼 새 값을 만들어요.
 * 입력: 지금 값 value, MAV_PARAM_TYPE 번호 type, 목록의 비트 listed, 그중 켤 비트 chosen이에요.
 * 출력: 형의 폭 안에서 계산한 값이에요. 목록에 없는 비트는 그대로예요.
 * 실패: 폭 밖의 비트는 무시해요.
 */
double withBits(double value, std::uint8_t type, const QList<int>& listed, const QList<int>& chosen);
/** 값이 메타데이터의 범위 밖인지 판정해요.
 * 입력: 값 value와 메타데이터 meta이며, meta는 없어도 돼요.
 * 출력: min보다 작거나 max보다 크면 참이에요.
 * 실패: 없어요.
 */
bool outOfRange(double value, const ParamMeta* meta);
/** 범위를 사람이 읽는 글자로 바꿔요.
 * 입력: 메타데이터 meta와 MAV_PARAM_TYPE 번호 type이에요.
 * 출력: `min ~ max`, `min 이상`, `max 이하`이며, 범위가 없으면 빈 문자열이에요.
 * 실패: 없어요.
 */
QString formatRange(const ParamMeta& meta, std::uint8_t type);
}
