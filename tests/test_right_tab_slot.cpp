// [0.0.4 T2] specs/features/0.0.4-righttabs.md
#include <kgc/ui/Slots.h>
#include <kgc/ui/Errors.h>
#include <QFile>
#include <QXmlStreamReader>
#include <QtTest>

namespace {
using kgc::ui::RightTabPlacement;
using kgc::ui::UiError;
// 등록 결과가 기대한 오류인지 한 줄로 확인하려고 둔 도우미예요.
bool failedWith(const kgc::app::errors::Result<void>& result, UiError error)
{
    return !result.has_value() && result.error() == kgc::ui::make(error);
}
// 번역 파일에서 문맥과 원문이 같은 항목의 영어를 찾아요. 없으면 빈 글자예요.
QString translationOf(const QString& context, const QString& source)
{
    QFile file(QStringLiteral(KGC_TS_FILE));
    if (!file.open(QIODevice::ReadOnly)) return {};
    QXmlStreamReader xml(&file);
    QString name, currentSource;
    while (!xml.atEnd()) {
        if (!xml.readNextStartElement()) continue;
        if (xml.name() == u"name") name = xml.readElementText();
        else if (xml.name() == u"source") currentSource = xml.readElementText();
        else if (xml.name() == u"translation" && name == context && currentSource == source)
            return xml.readElementText();
    }
    return {};
}
}
class RightTabSlotTest : public QObject {
    Q_OBJECT
private slots:
    /** T2: 우 탭 슬롯이 order 순서로 정렬하고, 같은 id·두 번째 계기 자리를 정해진 오류로 거부하는지 확인해요. */
    void test_T2_register_order_and_errors()
    {
        kgc::ui::RightTabSlot slot;
        const QUrl url("qrc:/x.qml");
        QVERIFY(slot.registerTab("logs", 40, "로그", url, RightTabPlacement::Tab).has_value());
        QVERIFY(slot.registerTab("overview", 10, "개요", url, RightTabPlacement::Tab).has_value());
        QVERIFY(slot.registerTab("instrument", 0, "계기", url, RightTabPlacement::Top).has_value());
        QVERIFY(failedWith(slot.registerTab("logs", 5, "로그", url, RightTabPlacement::Tab), UiError::DuplicateId));
        QVERIFY(failedWith(slot.registerTab("other", 1, "다른 계기", url, RightTabPlacement::Top), UiError::TopTaken));
        // 같은 id로 Top을 다시 넣으면 Top 중복보다 id 중복을 먼저 알려요.
        QVERIFY(failedWith(slot.registerTab("instrument", 0, "계기", url, RightTabPlacement::Top),
            UiError::DuplicateId));
        QCOMPARE(slot.rowCount(), 3);
        const auto roles = slot.roleNames();
        const QStringList ids{"instrument", "overview", "logs"};
        const QStringList placements{"top", "tab", "tab"};
        for (int row = 0; row < 3; ++row) {
            QCOMPARE(slot.data(slot.index(row), roles.key("id")).toString(), ids[row]);
            QCOMPARE(slot.data(slot.index(row), roles.key("placement")).toString(), placements[row]);
        }
        QCOMPARE(slot.data(slot.index(1), roles.key("titleKey")).toString(), QString("개요"));
        QCOMPARE(slot.data(slot.index(1), roles.key("component")).toUrl(), url);
    }
    /** T2: 계기 자리 오류 문구의 원문이 스펙 §5와 같고 번역 파일 문맥 "Ui"에 그 영어가 있는지 확인해요. */
    void test_T2_top_taken_text()
    {
        const QString source = "우 사이드바 계기 자리는 하나만 쓸 수 있어요.";
        QCOMPARE(kgc::ui::errorText(kgc::ui::make(UiError::TopTaken)), source);
        QCOMPARE(translationOf("Ui", source),
            QString("Only one item can use the right sidebar instrument area."));
    }
};
QTEST_GUILESS_MAIN(RightTabSlotTest)
#include "test_right_tab_slot.moc"
