// [0.4.0 5.4 6.3] specs/features/0.4.0-sitl.md
#pragma once
#include <kgc/sitl/SitlTypes.h>
#include <kgc/sitl/Settings.h>
#include <QList>
#include <QStringList>
namespace kgc::sitl {
/** 시뮬레이터 명령줄 인자를 만들어요(스펙 §6.3).
 * 입력: 기체 vehicle, 홈 home, 속도 배수 speedup, 값 초기화 wipe예요.
 * 출력: 값마다 따로인 인자 목록이에요. 소수점은 시스템 언어와 상관없이 늘 .이에요. wipe면 끝에 --wipe가 붙어요.
 * 실패: 범위는 검사하지 않아요.
 */
QStringList sitlArguments(SitlVehicle vehicle, const SitlHome& home, int speedup, bool wipe);
/** 홈 위치에서 범위를 벗어난 칸을 찾아요.
 * 입력: 홈 home이에요. 범위는 설정 정의의 min·max(생성 접근자)를 읽어요.
 * 출력: 벗어난 칸의 목록이며, 비면 통과예요. 경계값은 통과예요.
 * 실패: 숫자가 아닌 값(NaN)도 벗어난 칸으로 봐요.
 */
QList<SitlHomeField> checkHome(const SitlHome& home);
/** 저장된 기체 설정을 기체 종류로 바꿔요. 입력: 설정 값 value예요. 출력: 같은 기체예요. 실패: 없어요. */
SitlVehicle sitlVehicleFromSetting(settings::Vehicle value);
/** 기체 종류를 저장할 설정 값으로 바꿔요. 입력: 기체 vehicle이에요. 출력: 같은 설정 값이에요. 실패: 없어요. */
settings::Vehicle sitlVehicleToSetting(SitlVehicle vehicle);
/** 저장된 배포판 설정을 배포판으로 바꿔요. 입력: 설정 값 value예요. 출력: 같은 배포판이에요. 실패: 없어요. */
SitlRelease sitlReleaseFromSetting(settings::Release value);
/** 배포판을 저장할 설정 값으로 바꿔요. 입력: 배포판 release예요. 출력: 같은 설정 값이에요. 실패: 없어요. */
settings::Release sitlReleaseToSetting(SitlRelease release);
}
