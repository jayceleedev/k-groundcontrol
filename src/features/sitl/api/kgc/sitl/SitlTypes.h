// [0.4.0 5.4] specs/features/0.4.0-sitl.md
#pragma once
#include <QObject>
#include <QString>
#include <QUrl>
#include <QtQml/qqmlregistration.h>
namespace kgc::sitl {
Q_NAMESPACE
QML_NAMED_ELEMENT(Sitl)
/** 시뮬레이션할 기체 종류예요.
 * 입력: 없어요.
 * 출력: Copter·Heli·Plane·Rover 중 하나예요. QML에서는 Sitl.Copter 꼴이에요.
 * 실패: 없어요.
 */
enum class SitlVehicle { Copter, Heli, Plane, Rover };
Q_ENUM_NS(SitlVehicle)
/** 내려받을 배포판이에요.
 * 입력: 없어요.
 * 출력: Stable·Beta·Latest 중 하나예요.
 * 실패: 없어요.
 */
enum class SitlRelease { Stable, Beta, Latest };
Q_ENUM_NS(SitlRelease)
/** 시뮬레이터의 지금 상태예요(스펙 §6.1).
 * 입력: 없어요.
 * 출력: Idle·Downloading·Starting·Running·Failed 중 하나예요.
 * 실패: 없어요.
 */
enum class SitlState { Idle, Downloading, Starting, Running, Failed };
Q_ENUM_NS(SitlState)
/** 홈 위치의 칸 하나를 가리켜요.
 * 입력: 없어요.
 * 출력: Latitude·Longitude·Altitude·Heading 중 하나예요.
 * 실패: 없어요.
 */
enum class SitlHomeField { Latitude, Longitude, Altitude, Heading };
Q_ENUM_NS(SitlHomeField)
/** 실행 파일을 고를 운영체제예요. QML에는 내지 않아요.
 * 입력: 없어요.
 * 출력: Windows·Linux 중 하나예요.
 * 실패: 없어요.
 */
enum class SitlPlatform { Windows, Linux };
/** 시뮬레이터를 띄울 홈 위치예요.
 * 입력: 위도·경도(도), 고도(m), 기수(도, 정수)예요.
 * 출력: 값을 그대로 담아요.
 * 실패: 범위는 검사하지 않아요. checkHome이 봐요.
 */
struct SitlHome { double latitude; double longitude; double altitude; int heading; };
/** 내려받을 파일 하나예요.
 * 입력: 받을 주소 url과 캐시에 저장할 이름 fileName이에요.
 * 출력: 값을 그대로 담아요.
 * 실패: 없어요.
 */
struct SitlFile { QUrl url; QString fileName; };
}
