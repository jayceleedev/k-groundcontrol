// [0.0.2 T2–T9 T16 T20] specs/features/0.0.2-theme.md
#pragma once
#include <QObject>
#include <QVariantList>
#include <QHash>
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
     * 1. settings::setTheme — 선택을 저장하고 변경 구독으로 테마를 적용해요.
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
    QObject* current_ = nullptr;
};
}
