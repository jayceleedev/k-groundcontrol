// [0.3.0 5.5 T7] specs/features/0.3.0-params.md
#pragma once
#include <QStringList>
namespace kgc::params {
/** 구성원이 하나뿐인 묶음을 기타로 모아요.
 * 입력: 행별 묶음 이름 groups예요.
 * 출력: 같은 순서의 목록이에요. 구성원이 하나인 묶음과 빈 이름은 빈 문자열이에요.
 * 실패: 없어요.
 */
QStringList mergeSingleGroups(const QStringList& groups);
}
