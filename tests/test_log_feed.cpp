// [0.0.3 T5 T7 T8] specs/features/0.0.3-windows.md
#include <kgc/app/log/LogFeed.h>
#include <kgc/app/log/LogSink.h>
#include <QtTest>
#include <thread>
#include <array>
using namespace kgc::app::log;

class LogFeedTest : public QObject {
    Q_OBJECT
private slots:
    /** T7: 화면 경고는 파일에 남지만 다시 화면으로 들어오지 않는지 확인해요. */
    void test_T7_warning_guard()
    {
        QTemporaryDir dir;
        QVERIFY(LogSink::install(dir.path(), LogLevel::Debug, 5, 5));
        LogFeed feed;
        LogSink::attachFeed(&feed);
        connect(&feed, &LogFeed::recordAdded, this, [](const LogRecord&) {
            QMessageLogger("consumer.cpp", 1, "receive").warning("during-delivery");
        });
        QMessageLogger("producer.cpp", 1, "produce").info("original-record");
        QCOMPARE(feed.recent().second, quint64(1));
        QCoreApplication::sendPostedEvents(&feed, QEvent::MetaCall);
        QMessageLogger("qrc:/qt/qml/Kgc/Analyze/AppLogWindow.qml", 1, "binding").warning("window-warning");
        QMessageLogger("qrc:/qt/qml/Kgc/Ui/components/LogView.qml", 1, "binding").warning("delegate-warning");
        QCOMPARE(feed.recent().second, quint64(1));
        LogSink::attachFeed(nullptr);
        QFile file(dir.filePath("kgc.log"));
        QVERIFY(file.open(QIODevice::ReadOnly));
        const auto text = file.readAll();
        QVERIFY(text.contains("during-delivery"));
        QVERIFY(text.contains("window-warning"));
        QVERIFY(text.contains("delegate-warning"));
        const auto record = feed.recent().first.first();
        QVERIFY(!record.text.endsWith('\n'));
        QVERIFY(text.startsWith(record.text.toUtf8() + '\n'));
    }

    /** T8: 여러 스레드가 동시에 보내도 순서와 받는 스레드가 일정한지 확인해요. */
    void test_T8_concurrent_order()
    {
        LogFeed feed(4000);
        QList<quint64> received;
        bool correctThread = true;
        connect(&feed, &LogFeed::recordAdded, this, [&](const LogRecord& record) {
            correctThread &= QThread::currentThread() == feed.thread();
            received.append(record.seq);
        });
        std::array<std::thread, 4> workers;
        for (auto& worker : workers) worker = std::thread([&] {
            for (int i = 0; i < 1000; ++i)
                feed.publish({0, LogLevel::Info, "test", "test.Record", "record", "record"});
        });
        for (auto& worker : workers) worker.join();
        QVERIFY(received.isEmpty());
        const auto [records, last] = feed.recent();
        QCOMPARE(last, quint64(4000));
        QCOMPARE(records.size(), 4000);
        QTRY_COMPARE(received.size(), 4000);
        QVERIFY(correctThread);
        for (int i = 0; i < 4000; ++i) {
            QCOMPARE(records[i].seq, quint64(i + 1));
            QCOMPARE(received[i], quint64(i + 1));
        }
    }
    /** T5: 오래된 줄만 버리고 마지막 순번은 유지하는지 확인해요. */
    void test_T5_capacity()
    {
        LogFeed feed(2);
        for (int i = 0; i < 3; ++i) feed.publish({0, LogLevel::Info, {}, {}, {}, {}});
        const auto [records, last] = feed.recent();
        QCOMPARE(feed.capacity(), 2);
        QCOMPARE(records.size(), 2);
        QCOMPARE(records.first().seq, quint64(2));
        QCOMPARE(last, quint64(3));
    }
};
QTEST_GUILESS_MAIN(LogFeedTest)
#include "test_log_feed.moc"
