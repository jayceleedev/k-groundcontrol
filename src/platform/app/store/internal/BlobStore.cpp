// [0.5.0 S1 T10] specs/features/0.5.0-map.md
#include <kgc/app/store/BlobStore.h>
#include <kgc/app/store/Errors.h>
#include <kgc/app/log/Log.h>
#include "Events.h"
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QVariantList>
namespace kgc::app::store {
namespace {
using kgc::app::errors::Result;
// 연결을 준비하는 문장들이에요. 하나라도 실패하면 저장소를 열지 않아요.
const QString kSynchronousSql = QStringLiteral("PRAGMA synchronous=NORMAL");
const QString kCreateTableSql = QStringLiteral(
    "CREATE TABLE IF NOT EXISTS blobs(key TEXT PRIMARY KEY, data BLOB NOT NULL, bytes INTEGER NOT NULL, "
    "pinned INTEGER NOT NULL, used INTEGER NOT NULL)");
const QString kCreateIndexSql = QStringLiteral("CREATE INDEX IF NOT EXISTS blobs_evict ON blobs(pinned, used)");
// 최근에 본 것부터 바이트를 쌓아 한도를 넘는 행만 고르므로, 한도 안의 최근 덩이는 남아요.
const QString kEvictSql = QStringLiteral(
    "DELETE FROM blobs WHERE key IN (SELECT key FROM (SELECT key, SUM(bytes) OVER "
    "(ORDER BY used DESC, key ROWS UNBOUNDED PRECEDING) AS kept FROM blobs WHERE pinned=0) WHERE kept > ?)");
// 고정은 한 번 걸리면 풀리지 않게 기존 값과 새 값을 OR로 합쳐요.
const QString kPutSql = QStringLiteral(
    "INSERT INTO blobs(key, data, bytes, pinned, used) VALUES(?, ?, ?, ?, ?) ON CONFLICT(key) DO UPDATE SET "
    "data=excluded.data, bytes=excluded.bytes, used=excluded.used, pinned=(blobs.pinned OR excluded.pinned)");
qint64 nowMs() { return QDateTime::currentMSecsSinceEpoch(); }
}
struct BlobStore::Private {
    QSqlDatabase db;
    QString connectionName;

    // 문장을 준비하고 값을 차례로 묶어 실행해요. 질의 객체는 부르는 쪽이 들고 있어 결과를 읽을 수 있어요.
    bool run(QSqlQuery& query, const QString& sql, const QVariantList& values)
    {
        if (!query.prepare(sql)) return false;
        for (const auto& value : values) query.addBindValue(value);
        return query.exec();
    }
    // 열린 뒤 문장이 실패하면 드라이버 글을 남기고 공통 오류로 바꿔요.
    std::unexpected<kgc::app::errors::ErrorCode> failed(const QSqlQuery& query)
    {
        KGC_LOG_WARN(kCategory, StoreEvent::QueryFailed, "Store query failed: %1", query.lastError().text());
        return std::unexpected(make(StoreError::QueryFailed));
    }
    // 한 값만 돌려주는 문장(합계)을 실행해요.
    Result<qint64> scalar(const QString& sql)
    {
        QSqlQuery query(db);
        if (!run(query, sql, {}) || !query.next()) return failed(query);
        return query.value(0).toLongLong();
    }
    // 열기 단계를 차례로 실행해요. 질의는 이 함수 안에서만 살아서 실패 뒤 연결을 지울 때 남지 않아요.
    std::optional<QString> prepare(QString& journalMode)
    {
        if (!db.open()) return db.lastError().text();
        {
            QSqlQuery query(db);
            if (!query.exec(QStringLiteral("PRAGMA journal_mode=WAL")) || !query.next())
                return query.lastError().text();
            journalMode = query.value(0).toString();
        }
        for (const auto& sql : {kSynchronousSql, kCreateTableSql, kCreateIndexSql}) {
            QSqlQuery query(db);
            if (!query.exec(sql)) return query.lastError().text();
        }
        return std::nullopt;
    }
    // 마지막 손잡이까지 비운 뒤 연결을 지워야 Qt가 "아직 쓰는 중" 경고를 내지 않아요.
    void release()
    {
        if (connectionName.isEmpty()) return;
        db.close();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(connectionName);
        connectionName.clear();
    }
};
BlobStore::BlobStore() : d_(std::make_unique<Private>()) {}
BlobStore::~BlobStore() { d_->release(); }
Result<std::unique_ptr<BlobStore>> BlobStore::open(const QString& path, const QString& connectionName)
{
    std::unique_ptr<BlobStore> store(new BlobStore);
    auto& d = *store->d_;
    d.connectionName = connectionName;
    d.db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connectionName);
    d.db.setDatabaseName(path);
    QString journalMode;
    if (const auto error = d.prepare(journalMode)) {
        KGC_LOG_WARN(kCategory, StoreEvent::OpenFailed, "Store open failed: %1, %2", path, *error);
        d.release();
        return std::unexpected(make(StoreError::OpenFailed));
    }
    KGC_LOG_INFO(kCategory, StoreEvent::Opened, "Store opened: %1, journal %2", path, journalMode);
    return store;
}
Result<std::optional<QByteArray>> BlobStore::get(const QString& key)
{
    QByteArray data;
    {
        QSqlQuery query(d_->db);
        if (!d_->run(query, QStringLiteral("SELECT data FROM blobs WHERE key=?"), {key})) return d_->failed(query);
        if (!query.next()) return std::optional<QByteArray>();
        data = query.value(0).toByteArray();
    }
    // 본 시각을 남겨야 한도를 넘을 때 오래 안 본 것부터 지울 수 있어요.
    QSqlQuery touch(d_->db);
    if (!d_->run(touch, QStringLiteral("UPDATE blobs SET used=? WHERE key=?"), {nowMs(), key})) return d_->failed(touch);
    return std::optional<QByteArray>(std::move(data));
}
Result<void> BlobStore::put(const QString& key, const QByteArray& data, bool pinned)
{
    QSqlQuery query(d_->db);
    if (!d_->run(query, kPutSql, {key, data, qint64(data.size()), pinned ? 1 : 0, nowMs()})) return d_->failed(query);
    return {};
}
Result<bool> BlobStore::pin(const QString& key)
{
    QSqlQuery query(d_->db);
    if (!d_->run(query, QStringLiteral("UPDATE blobs SET pinned=1 WHERE key=?"), {key})) return d_->failed(query);
    return query.numRowsAffected() > 0;
}
Result<qint64> BlobStore::totalBytes()
{
    return d_->scalar(QStringLiteral("SELECT COALESCE(SUM(bytes), 0) FROM blobs"));
}
Result<qint64> BlobStore::pinnedBytes()
{
    return d_->scalar(QStringLiteral("SELECT COALESCE(SUM(bytes), 0) FROM blobs WHERE pinned=1"));
}
Result<void> BlobStore::evictUnpinned(qint64 limitBytes)
{
    const auto unpinned = d_->scalar(QStringLiteral("SELECT COALESCE(SUM(bytes), 0) FROM blobs WHERE pinned=0"));
    if (!unpinned) return std::unexpected(unpinned.error());
    if (*unpinned <= limitBytes) return {};
    QSqlQuery query(d_->db);
    if (!d_->run(query, kEvictSql, {limitBytes})) return d_->failed(query);
    return {};
}
Result<void> BlobStore::clear()
{
    QSqlQuery query(d_->db);
    if (!d_->run(query, QStringLiteral("DELETE FROM blobs"), {})) return d_->failed(query);
    // VACUUM은 트랜잭션 안에서 돌 수 없어서 지우기가 끝난 뒤 따로 실행해요.
    QSqlQuery vacuum(d_->db);
    if (!vacuum.exec(QStringLiteral("VACUUM"))) return d_->failed(vacuum);
    return {};
}
}
