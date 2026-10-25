// [0.0.3 T5 T6 T7 T8] specs/features/0.0.3-windows.md
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
        LogSink::attachFeed(&feed, {"qrc:/test/log-window/", "qrc:/test/log-view.qml"});
        connect(&feed, &LogFeed::recordAdded, this, [](const LogRecord&) {
            QMessageLogger("consumer.cpp", 1, "receive").warning("during-delivery");
        });
        QMessageLogger("producer.cpp", 1, "produce").info("original-record");
        QCOMPARE(feed.recent().second, quint64(1));
        QCoreApplication::sendPostedEvents(&feed, QEvent::MetaCall);
        QMessageLogger("qrc:/test/log-window/Window.qml", 1, "binding").warning("window-warning");
        QMessageLogger("qrc:/test/log-view.qml", 1, "binding").warning("delegate-warning");
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
        // 새로 붙일 때 접두 목록을 비우면 이전 제외 규칙이 남지 않아야 해요.
        LogSink::attachFeed(&feed);
        QMessageLogger("qrc:/test/log-view.qml", 1, "binding").warning("unguarded-warning");
        QCOMPARE(feed.recent().second, quint64(2));
        LogSink::attachFeed(nullptr);
    }

    /** T6: 수준 이름은 소문자이며 이름 해석과 같은 대응을 쓰는지 확인해요. */
    void test_T6_level_names()
    {
        const std::array names{"debug", "info", "warn", "error"};
        const std::array levels{LogLevel::Debug, LogLevel::Info, LogLevel::Warn, LogLevel::Error};
        for (std::size_t i = 0; i < levels.size(); ++i) {
            QCOMPARE(logLevelName(levels[i]), QString::fromLatin1(names[i]));
            QCOMPARE(parseLogLevel(logLevelName(levels[i])), levels[i]);
        }
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
