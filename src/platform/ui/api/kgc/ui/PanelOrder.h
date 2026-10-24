// [0.0.1 T1] specs/features/0.0.1-shell.md
#pragma once
#include <kgc/app/errors/Result.h>
#include <QString>
#include <QUrl>
#include <vector>

namespace kgc::ui {
/** 패널 등록과 메뉴 표시 정보를 함께 담아요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
struct PanelEntry {
    QString id;
    int order;
    int group;
    QString titleKey;
    QUrl icon;
    QUrl component;
};
/** 등록 순서와 무관하게 메뉴 순서를 정해요.
 * 입력: 패널 목록.
 * 출력: 순서로 정렬된 목록.
 * 실패: 없음.
 */
std::vector<PanelEntry> sortByOrder(std::vector<PanelEntry> entries);
/** 묶음이 바뀌는 항목 뒤에 구분선을 표시해요.
 * 입력: 정렬된 목록.
 * 출력: 항목별 구분선 여부.
 * 실패: 없음.
 */
std::vector<bool> separatorAfter(const std::vector<PanelEntry>& sorted);
/** 이미 등록된 식별자인지 확인해요.
 * 입력: 패널 목록과 식별자.
 * 출력: 성공 또는 오류.
 * 실패: 식별자 중복이면 DuplicateId.
 */
kgc::app::errors::Result<void> checkDuplicate(const std::vector<PanelEntry>& entries, const QString& id);
}
