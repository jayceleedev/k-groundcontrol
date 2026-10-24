// [0.0.1 T4–T10] specs/features/0.0.1-shell.md
#pragma once
#include <QString>
namespace kgc::appSettings {
/** 버전과 배포 자료의 위치를 묶어요.
 * 입력: 조립이 넘기는 배포 정보.
 * 출력: 정보 값 묶음.
 * 실패: 없음.
 */
struct AppInfo { QString version; QString homepageUrl; QString licensePath; QString noticesPath; };
}
