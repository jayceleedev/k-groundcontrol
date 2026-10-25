// [0.0.3 T11 T12 T13] specs/features/0.0.3-windows.md
#pragma once
#include <QString>
#include <QRect>
#include <QList>
namespace kgc::ui {
/** 화면 이름과 창을 놓을 수 있는 영역을 담아요.
 * 입력: 화면 이름과 전역 논리 픽셀 영역.
 * 출력: 복원에 쓰는 화면 정보.
 * 실패: 없음.
 */
struct ScreenInfo { QString name; QRect available; };
/** 창을 표시할 화면과 전역 좌표를 담아요.
 * 입력: 화면 목록 위치와 창 영역.
 * 출력: 창에 적용할 자리.
 * 실패: 없음.
 */
struct Placement { int screenIndex; QRect geometry; };
/** 저장한 자리를 복원하고 화면 안으로 끌어와요.
 * 입력: 저장 문자열, 주 화면이 첫째인 비어 있지 않은 화면 목록, 기본 크기.
 * 출력: setScreen 다음 setGeometry에 쓸 화면과 전역 영역.
 * 실패: 저장값이 잘못되면 주 화면 가운데 기본 크기를 사용해요.
 */
Placement restorePlacement(const QString& saved, const QList<ScreenInfo>& screens, QSize defaultSize);
/** 창 자리를 화면 기준 문자열로 바꿔요.
 * 입력: 화면 이름과 그 화면 가용 영역 왼쪽 위 기준의 창 영역.
 * 출력: 화면 이름과 좌표·크기를 세로줄로 이은 문자열.
 * 실패: 없음.
 */
QString savePlacement(const QString& screenName, QRect geometryInScreen);
}
