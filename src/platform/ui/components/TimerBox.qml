// [0.0.2 T11] specs/features/0.0.2-theme.md
/** 시간 처리를 한 자리에서 제공해요.
 * 입력: interval, running, repeat와 start(), stop(), restart(), triggered를 사용해요.
 * 출력: 보이지 않는 타이머가 interval 뒤 triggered를 내며 repeat이면 반복해요.
 * 실패: stop을 부르면 대기 중인 triggered를 내지 않아요. 시간 오차는 화면 갱신 주기에 따라요.
 */
import QtQuick

Timer {}
