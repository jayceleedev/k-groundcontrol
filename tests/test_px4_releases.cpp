// [0.2.0 T10] specs/features/0.2.0-fcflash.md
#include "sources/Px4Releases.h"
#include <QtTest>
using kgc::fcFlash::Px4Candidate;
using kgc::fcFlash::parsePx4Releases;
using kgc::fcFlash::px4Candidates;
using kgc::fcFlash::px4Prefixes;
namespace {
/** 손으로 만든 표본을 읽어요. 릴리스 표본은 순서를 섞어 두었고, 날짜와 태그 어느 쪽으로 보아도 최신은 v1.17.0이에요. */
QByteArray sample(const QString& name)
{
    QFile file(QStringLiteral(KGC_TEST_DATA "/") + name);
    return file.open(QIODevice::ReadOnly) ? file.readAll() : QByteArray();
}
QStringList names(const QList<Px4Candidate>& candidates)
{
    QStringList result;
    for (const auto& candidate : candidates) result << candidate.prefix + "|" + candidate.label;
    result.sort();
    return result;
}
}
class Px4ReleasesTest : public QObject {
    Q_OBJECT
private slots:
    /** T10: 초안과 프리릴리스를 빼고 최신 순으로 내놓아요. */
    void test_T10_excludes_draft_and_prerelease()
    {
        const auto releases = parsePx4Releases(sample("px4-releases.json"));
        QStringList tags;
        for (const auto& release : releases) tags << release.tag;
        QCOMPARE(tags, QStringList({"v1.17.0", "v1.16.1", "v1.15.4"}));
    }
    /** T10: 태그의 숫자 버전은 기체 값과 견줄 수 있게 v 없이 나와요. */
    void test_T10_version_has_no_v_prefix()
    {
        const auto releases = parsePx4Releases(sample("px4-releases.json"));
        QVERIFY(!releases.isEmpty());
        QCOMPARE(releases.first().version, QString("1.17.0"));
    }
    /** T10: 자산의 이름과 내려받을 주소를 읽어요. */
    void test_T10_reads_assets()
    {
        const auto releases = parsePx4Releases(sample("px4-releases.json"));
        QVERIFY(!releases.isEmpty());
        QCOMPARE(releases.first().assets.size(), 7);
        QCOMPARE(releases.first().assets.first().name, QString("matek_h743_default.px4"));
        QCOMPARE(releases.first().assets.first().url, QString("https://example.invalid/v1.17.0/matek_h743_default.px4"));
    }
    /** T10: 깨진 응답은 빈 목록이에요. */
    void test_T10_broken_json_is_empty()
    {
        QVERIFY(parsePx4Releases("not json").isEmpty());
        QVERIFY(parsePx4Releases("{}").isEmpty());
    }
    /** T10: 보드 ID가 같은 prefix를 전부 찾아요. */
    void test_T10_prefixes_share_board_id()
    {
        auto prefixes = px4Prefixes(sample("px4-boards.json"), 1013);
        prefixes.sort();
        QCOMPARE(prefixes, QStringList({"hkust_nxt-dual", "hkust_nxt-v1", "matek_h743", "matek_h743-mini", "matek_h743-slim"}));
        QVERIFY(px4Prefixes(sample("px4-boards.json"), 4242).isEmpty());
    }
    /** T10: 보드 ID가 겹치면 prefix마다의 꼬리표 전부가 후보이고 bootloader는 빠져요. */
    void test_T10_all_candidates_without_bootloader()
    {
        const auto releases = parsePx4Releases(sample("px4-releases.json"));
        QVERIFY(!releases.isEmpty());
        const auto candidates = px4Candidates(releases.first(), px4Prefixes(sample("px4-boards.json"), 1013));
        QCOMPARE(names(candidates), QStringList({"hkust_nxt-v1|default", "hkust_nxt-v1|rover",
                                                  "matek_h743-mini|default", "matek_h743|default"}));
    }
    /** T10: 후보는 고른 파일의 주소를 들고 있어요. */
    void test_T10_candidate_keeps_url()
    {
        const auto releases = parsePx4Releases(sample("px4-releases.json"));
        QVERIFY(!releases.isEmpty());
        const auto candidates = px4Candidates(releases.first(), {"matek_h743"});
        QCOMPARE(candidates.size(), 1);
        QCOMPARE(candidates.first().url, QString("https://example.invalid/v1.17.0/matek_h743_default.px4"));
    }
    /** T10: 맞는 파일이 없으면 후보가 없어요. */
    void test_T10_no_candidates_for_other_board()
    {
        const auto releases = parsePx4Releases(sample("px4-releases.json"));
        QVERIFY(!releases.isEmpty());
        QVERIFY(px4Candidates(releases.first(), {"cubepilot_cubeorange"}).isEmpty());
    }
};
QTEST_APPLESS_MAIN(Px4ReleasesTest)
#include "test_px4_releases.moc"
