// [0.4.0 5.4 6.2] specs/features/0.4.0-sitl.md
#pragma once
#include <kgc/sitl/SitlTypes.h>
#include <QList>
#include <QString>
namespace kgc::sitl {
/** 운영체제·기체·배포판에 맞는 내려받을 파일 목록을 만들어요.
 * 입력: 운영체제 platform, 기체 vehicle, 배포판 release예요.
 * 출력: 첫 항목이 실행 파일, 마지막 항목이 defaults.parm인 목록이에요. Windows는 12개, Linux는 2개예요.
 * 실패: 없어요.
 */
QList<SitlFile> sitlFiles(SitlPlatform platform, SitlVehicle vehicle, SitlRelease release);
/** 캐시 폴더 이름을 만들어요.
 * 입력: 운영체제 platform, 기체 vehicle, 배포판 release예요.
 * 출력: windows-copter-stable 꼴의 이름이에요.
 * 실패: 없어요.
 */
QString sitlFolderName(SitlPlatform platform, SitlVehicle vehicle, SitlRelease release);
/** 시뮬레이터의 --model 값을 정해요.
 * 입력: 기체 vehicle이에요.
 * 출력: +, heli, plane, rover 중 하나예요.
 * 실패: 없어요.
 */
QString sitlModel(SitlVehicle vehicle);
/** 기체별 실행 폴더 이름을 정해요.
 * 입력: 기체 vehicle이에요.
 * 출력: copter, heli, plane, rover 중 하나예요.
 * 실패: 없어요.
 */
QString sitlRunFolderName(SitlVehicle vehicle);
}
