// [0.0.3 T10] specs/features/0.0.3-windows.md
#pragma once
#include <kgc/secondPanel/Settings.h>
#include <QRectF>
#include <QList>
namespace kgc::secondPanel {
/** 고른 배치를 가로 우선 순서의 비율 사각형으로 바꿔요.
 * 입력: 설정에 정의한 칸 배치.
 * 출력: 전체를 채우는 0~1 좌표 사각형 목록, 큰 칸 배치는 큰 칸이 첫째예요.
 * 실패: 정의되지 않은 열거 값은 빈 목록.
 */
QList<QRectF> cellRects(settings::Arrangement arrangement);
}
