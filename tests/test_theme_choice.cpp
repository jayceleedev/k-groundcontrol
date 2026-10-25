// [0.0.2 T2] specs/features/0.0.2-theme.md
#include <kgc/ui/ThemeChoice.h>
#include <QtTest>
using namespace kgc::ui;
/** 테마 판정의 설정 우선순위와 시스템 대체를 확인해요.
 * 입력: 테마 식별자와 운영체제 색 모드.
 * 출력: 시험 결과.
 * 실패: 선택한 테마나 이유가 다르면 실패해요.
 */
class ThemeChoiceTest : public QObject {
    Q_OBJECT
private slots:
    /** T2: 사용자가 고른 테마는 시스템보다 우선해요. */
    void test_T2_resolve_theme()
    {
        const QStringList available{"light", "dark", "aeroLight", "aeroDark"};
        const auto check = [&available](QStringView selected, Qt::ColorScheme scheme,
                                        const QString& id, ThemeReason reason) {
            const auto choice = resolveTheme(selected, scheme, available);
            QCOMPARE(choice.id, id);
            QCOMPARE(choice.reason, reason);
        };
        check(u"system", Qt::ColorScheme::Dark, "dark", ThemeReason::System);
        check(u"system", Qt::ColorScheme::Light, "light", ThemeReason::System);
        check(u"system", Qt::ColorScheme::Unknown, "light", ThemeReason::System);
        check(u"aeroDark", Qt::ColorScheme::Light, "aeroDark", ThemeReason::Setting);
        check(u"nope", Qt::ColorScheme::Dark, "dark", ThemeReason::Unknown);
        check(u"aeroLight", Qt::ColorScheme::Dark, "aeroLight", ThemeReason::Setting);
    }
};
QTEST_GUILESS_MAIN(ThemeChoiceTest)
#include "test_theme_choice.moc"
