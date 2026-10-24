// [0.0.1 T1] specs/features/0.0.1-shell.md
#pragma once
#include <kgc/ui/PanelOrder.h>
#include <QAbstractListModel>

namespace kgc::ui {
/** 패널들을 정렬된 메뉴 모델로 제공해요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
class PanelRegistry : public QAbstractListModel {
    Q_OBJECT
public:
    enum class Role { Id = Qt::UserRole + 1, Order, Group, TitleKey, Icon, Component, SeparatorAfter };
/** 빈 패널 등록부를 만들어요.
 * 입력: 부모.
 * 출력: 없음.
 * 실패: 없음.
 */
    explicit PanelRegistry(QObject* parent = nullptr);
/** 패널을 중복 없이 등록해요.
 * 입력: 패널 항목.
 * 출력: 성공 또는 오류.
 * 실패: 식별자가 겹치면 DuplicateId.
 */
    kgc::app::errors::Result<void> registerPanel(PanelEntry entry);
/** 아직 없는 패널의 메뉴 자리를 등록해요.
 * 입력: 식별자와 순서와 묶음과 제목과 아이콘.
 * 출력: 성공 또는 오류.
 * 실패: 식별자가 겹치면 DuplicateId.
 */
    kgc::app::errors::Result<void> registerPlaceholder(const QString& id, int order, int group,
        const QString& titleKey, const QUrl& icon);
/** 메뉴 항목 수를 알려줘요.
 * 입력: 부모 인덱스.
 * 출력: 행 수.
 * 실패: 없음.
 */
    int rowCount(const QModelIndex& parent = {}) const override;
/** 메뉴 표시 값을 읽어요.
 * 입력: 인덱스와 역할.
 * 출력: 표시 값.
 * 실패: 유효하지 않은 위치는 빈 값.
 */
    QVariant data(const QModelIndex& index, int role) const override;
/** 화면에서 사용할 역할 이름을 알려줘요.
 * 입력: 없음.
 * 출력: 역할 이름 목록.
 * 실패: 없음.
 */
    QHash<int, QByteArray> roleNames() const override;
private:
    std::vector<PanelEntry> entries_;
    std::vector<bool> separators_;
};
}
