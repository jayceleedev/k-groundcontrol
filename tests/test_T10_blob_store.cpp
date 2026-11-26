// [0.5.0 T10] specs/features/0.5.0-map.md
#include <kgc/app/store/BlobStore.h>
#include <kgc/app/store/Errors.h>
#include <QFile>
#include <QRegularExpression>
#include <QTemporaryDir>
#include <QThread>
#include <QtTest>
using kgc::app::store::BlobStore;
using kgc::app::store::StoreError;
namespace {
// used 시각이 작업마다 달라지도록 조금 기다려요.
void waitTick() { QThread::msleep(2); }
// 저장소에 그 열쇠가 남아 있는지 봐요(읽으면 used가 바뀌므로 지우기 확인 뒤에만 써요).
bool contains(BlobStore& store, const QString& key)
{
    const auto value = store.get(key);
    return value.has_value() && value->has_value();
}
}
class BlobStoreTest : public QObject {
    Q_OBJECT
private slots:
    /** T10: 넣기·읽기·고정·합계·오래 안 본 것부터 지우기·비우기가 스펙 §5.2대로 동작해요. */
    void test_T10_put_get_pin_evict_clear()
    {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        auto opened = BlobStore::open(dir.filePath("blobs.sqlite"), "t10Blobs");
        QVERIFY(opened.has_value());
        auto& store = **opened;
        QVERIFY(store.put("a", QByteArray(100, 'a'), false).has_value());
        waitTick();
        QVERIFY(store.put("d", QByteArray(200, 'd'), false).has_value());
        waitTick();
        QVERIFY(store.put("e", QByteArray(300, 'e'), false).has_value());
        waitTick();
        QVERIFY(store.put("c", QByteArray(300, 'c'), true).has_value());
        waitTick();
        QCOMPARE(*store.totalBytes(), qint64(900));
        QCOMPARE(*store.pinnedBytes(), qint64(300));
        const auto a = store.get("a");
        QVERIFY(a.has_value() && a->has_value());
        QCOMPARE((*a)->size(), 100);
        waitTick();
        const auto missing = store.get("zzz");
        QVERIFY(missing.has_value() && !missing->has_value());
        QVERIFY(store.put("c", QByteArray(300, 'c'), false).has_value());
        QCOMPARE(*store.pinnedBytes(), qint64(300));
        const auto pinned = store.pin("zzz");
        QVERIFY(pinned.has_value() && !*pinned);
        waitTick();
        QVERIFY(contains(store, "e"));
        waitTick();
        QVERIFY(store.evictUnpinned(350).has_value());
        QCOMPARE(*store.totalBytes(), qint64(600));
        QVERIFY(!contains(store, "a"));
        QVERIFY(!contains(store, "d"));
        QVERIFY(contains(store, "c"));
        QVERIFY(contains(store, "e"));
        QVERIFY(store.clear().has_value());
        QCOMPARE(*store.totalBytes(), qint64(0));
        QCOMPARE(*store.pinnedBytes(), qint64(0));
    }
    /** T10: 데이터베이스가 아닌 파일은 OpenFailed이고, 그 파일을 지우면 같은 연결 이름으로 Qt 경고 없이 다시 열려요. */
    void test_T10_bad_file_then_reopen()
    {
        QTest::failOnWarning(QRegularExpression("connection"));
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto path = dir.filePath("broken.sqlite");
        {
            QFile file(path);
            QVERIFY(file.open(QIODevice::WriteOnly));
            QCOMPARE(file.write(QByteArray("not a database").leftJustified(64, ' ')), qint64(64));
        }
        const auto broken = BlobStore::open(path, "t10Broken");
        QVERIFY(!broken.has_value());
        QVERIFY(broken.error() == kgc::app::store::make(StoreError::OpenFailed));
        QVERIFY(QFile::remove(path));
        auto reopened = BlobStore::open(path, "t10Broken");
        QVERIFY(reopened.has_value());
        QCOMPARE(*(*reopened)->totalBytes(), qint64(0));
    }
};
QTEST_GUILESS_MAIN(BlobStoreTest)
#include "test_T10_blob_store.moc"
