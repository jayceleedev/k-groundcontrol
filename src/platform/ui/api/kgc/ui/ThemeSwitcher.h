// [0.0.2 T2–T9 T16 T20] specs/features/0.0.2-theme.md
#pragma once
#include <QObject>
#include <QVariantList>
#include <QHash>
#include <QSet>
#include <QUrl>
class QQmlEngine;
namespace kgc::ui {
    /** 테마 객체의 수명과 현재 테마를 한 곳에서 관리해요.
     * 입력: 화면 엔진.
     * 출력: 없음.
     * 실패: 없음.
     */
class ThemeSwitcher : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString themeId READ themeId NOTIFY themeIdChanged)
    Q_PROPERTY(int currentIndex READ currentIndex NOTIFY themeIdChanged)
    Q_PROPERTY(QVariantList themes READ themes CONSTANT)
    Q_PROPERTY(QString selected READ selected NOTIFY selectedChanged)
public:
    /** 테마 목록을 읽고 시작 테마를 적용해요.
     * 입력: 화면 엔진.
     * 출력: 없음.
     * 실패: 없음.
     */
    explicit ThemeSwitcher(QQmlEngine& engine);
    /** 적용된 테마를 알려줘요.
     * 입력: 없음.
     * 출력: 테마 식별자.
     * 실패: 없음.
     */
    QString themeId() const;
    /** 표시 순서로 정렬한 테마 목록을 알려줘요.
     * 입력: 없음.
     * 출력: 식별자, 이름표, 밝기와 유리 여부 목록.
     * 실패: 없음.
     */
    QVariantList themes() const;
    /** 적용된 테마가 목록에서 놓인 위치를 알려줘요.
     * 입력: 없음.
     * 출력: 정렬된 themes 목록의 위치예요.
     * 실패: 적용된 테마가 없으면 -1이에요.
     */
    int currentIndex() const;
    /** 저장된 사용자 선택을 읽어요.
     * 입력: 없음.
     * 출력: 설정 값.
     * 실패: 없음.
     */
    QString selected() const;
    /** 고른 테마를 저장해서 화면에 적용해요.
     * 입력: 테마 식별자.
     * 출력: 없음.
     * 실패: 없음.
     * 흐름:
     * 1. settings::setTheme — 선택을 저장해요. 같은 값이면 변경 알림 없이 끝나요.
     * 2. settings::onThemeChanged — 저장값 변경을 알려 주고 apply를 불러요.
     * 3. resolveTheme — 저장값과 운영체제 색 모드로 적용할 테마를 정해요.
     * 4. apply — 모르는 설정 값은 한 번 경고하고, 적용된 식별자가 같으면 끝나요.
     * 5. QQmlComponent::create — 새 테마 객체를 만들어요.
     * 6. Theme.current — 새 객체를 넣고 이전 객체를 deleteLater로 정리해요.
     * 7. themeIdChanged — 적용된 식별자와 목록 위치의 변경을 알려요.
     * 8. KGC_LOG_INFO — ThemeApplied 사건으로 적용 이유를 기록해요.
     */
    Q_INVOKABLE void choose(const QString& id);
signals:
    /** 적용된 테마의 변경을 알려요.
     * 입력: 없음.
     * 출력: 없음.
     * 실패: 없음.
     */
    void themeIdChanged();
    /** 저장된 선택의 변경을 알려요.
     * 입력: 없음.
     * 출력: 없음.
     * 실패: 없음.
     */
    void selectedChanged();
private:
    void apply();
    QQmlEngine& engine_;
    QVariantList themes_;
    QHash<QString, QUrl> sources_;
    QString themeId_;
    QSet<QString> warnedSelections_;
    QObject* current_ = nullptr;
};
}
