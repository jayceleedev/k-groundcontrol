// [0.0.2 T2–T9 T16 T20] specs/features/0.0.2-theme.md
#pragma once
#include <QStringView>
#include <QStringList>
#include <Qt>
namespace kgc::ui {
/** 테마를 고른 근거를 구분해요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
enum class ThemeReason { Setting, System, Unknown };
/** 적용할 테마와 선택 근거를 함께 전달해요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
struct ThemeChoice { QString id; ThemeReason reason; };
/** 사용자 선택을 우선하고 나머지는 시스템 색 모드를 따라요.
 * 입력: 저장된 선택, 시스템 색 모드, 사용 가능한 테마 목록.
 * 출력: 적용할 테마 식별자와 이유.
 * 실패: 모르는 선택은 시스템 판정으로 대체해요.
 */
ThemeChoice resolveTheme(QStringView selected, Qt::ColorScheme scheme, const QStringList& available);
}
