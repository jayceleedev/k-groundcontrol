// [0.6.0 5.11.3 T9] specs/features/0.6.0-fly.md
#pragma once
#include <kgc/fly/Errors.h>
#include <kgc/fly/FlyTypes.h>
#include <QString>
namespace kgc::fly {
/** 거리를 화면 글로 만들어요(단위는 번역하지 않아요).
 * 입력: 미터 meters예요.
 * 출력: 반올림해 1000 m 아래면 "259 m"처럼 정수, 그 위면 km 소수 한 자리에서 끝 ".0"을 뺀 "1.2 km"·"25 km"예요.
 * 실패: NaN이면 "—"예요.
 */
QString distanceText(double meters);
/** 길이를 소수 한 자리 숫자 글로 만들어요.
 * 입력: 미터 meters예요.
 * 출력: 소수 한 자리에서 끝 ".0"을 뺀 글이에요(50 → "50", 30.46 → "30.5").
 * 실패: 없어요.
 */
QString lengthNumber(double meters);
/** 좌표를 화면 글로 만들어요.
 * 입력: 위도 latDeg·경도 lonDeg예요.
 * 출력: 소수 여섯 자리 둘을 ", "로 이은 글이에요("35.158700, 129.160400").
 * 실패: 없어요.
 */
QString positionText(double latDeg, double lonDeg);
/** 로그에 쓰는 동작 이름을 만들어요.
 * 입력: 동작 종류 kind예요.
 * 출력: 열거 이름 그대로의 영어("GoTo")예요.
 * 실패: 없어요.
 */
QString actionName(FlyActionKind kind);
/** 로그에 쓰는 오류 이름을 만들어요.
 * 입력: 비행 오류 error예요.
 * 출력: 열거 이름 그대로의 영어("ModeNotChanged")예요.
 * 실패: 없어요.
 */
QString flyErrorName(FlyError error);
/** 실패 토스트의 앞 글(무엇을 못 했는지)을 만들어요(번역 문맥 "Fly").
 * 입력: 동작 종류 kind예요.
 * 출력: 지금 언어의 글이에요("그곳으로 가지 못했어요").
 * 실패: 없어요.
 */
QString actionFailureText(FlyActionKind kind);
}
