// [0.5.0 S1 T10] specs/features/0.5.0-map.md
#pragma once
#include <kgc/app/errors/Result.h>
#include <QByteArray>
#include <QString>
#include <memory>
#include <optional>
namespace kgc::app::store {
/** 기능을 모르는 범용 덩이 저장소예요. 열쇠마다 바이트 덩이 하나를 SQLite 파일에 두고, 고정하지 않은 것은 한도에 맞춰 지워요.
 * 입력: open에 넘기는 파일 경로와 연결 이름, 각 함수의 열쇠와 바이트예요.
 * 출력: 함수마다 Result로 값을 돌려줘요. 만든 스레드에서만 써요(Qt는 다른 스레드에서 써도 경고하지 않아요).
 * 실패: 파일을 열거나 준비하지 못하면 StoreError::OpenFailed, 열린 뒤 문장이 실패하면 StoreError::QueryFailed예요.
 */
class BlobStore {
public:
/** 저장소 파일을 열고 표와 색인을 준비해요.
 * 입력: SQLite 파일 경로 path와 이 저장소만 쓰는 연결 이름 connectionName이에요.
 * 출력: 열린 저장소예요. WAL 일지와 NORMAL 동기화로 열고, 표 blobs와 색인 blobs_evict가 없으면 만들어요.
 * 실패: 어느 단계든 실패하면(깨진 파일은 첫 PRAGMA에서 실패해요) 연결을 지운 뒤 OpenFailed예요. 같은 이름으로 다시 열 수 있어요.
 */
    static kgc::app::errors::Result<std::unique_ptr<BlobStore>> open(const QString& path, const QString& connectionName);
/** 연결을 닫고 연결 이름을 놓아요.
 * 입력: 없어요.
 * 출력: 같은 연결 이름으로 다시 열 수 있는 상태예요.
 * 실패: 없어요.
 */
    ~BlobStore();
/** 열쇠의 덩이를 읽어요.
 * 입력: 열쇠 key예요.
 * 출력: 있으면 바이트이고 그 행의 used를 지금 시각(ms)으로 바꿔요. 없으면 값 없음이에요.
 * 실패: 문장이 실패하면 QueryFailed예요.
 */
    kgc::app::errors::Result<std::optional<QByteArray>> get(const QString& key);
/** 열쇠에 덩이를 넣거나 바꿔요.
 * 입력: 열쇠 key, 바이트 data, 고정 여부 pinned예요.
 * 출력: bytes는 data 크기, used는 지금 시각(ms)이에요. 한 번 고정된 행은 put으로 풀리지 않아요.
 * 실패: 문장이 실패하면 QueryFailed예요.
 */
    kgc::app::errors::Result<void> put(const QString& key, const QByteArray& data, bool pinned);
/** 있는 덩이를 고정해 evictUnpinned가 지우지 않게 해요.
 * 입력: 열쇠 key예요.
 * 출력: 그 행이 있었으면 참, 없으면 거짓이에요.
 * 실패: 문장이 실패하면 QueryFailed예요.
 */
    kgc::app::errors::Result<bool> pin(const QString& key);
/** 모든 덩이의 바이트 합을 읽어요.
 * 입력: 없어요.
 * 출력: 바이트 합이며 비었으면 0이에요.
 * 실패: 문장이 실패하면 QueryFailed예요.
 */
    kgc::app::errors::Result<qint64> totalBytes();
/** 고정한 덩이의 바이트 합을 읽어요.
 * 입력: 없어요.
 * 출력: 바이트 합이며 없으면 0이에요.
 * 실패: 문장이 실패하면 QueryFailed예요.
 */
    kgc::app::errors::Result<qint64> pinnedBytes();
/** 고정하지 않은 덩이의 합이 한도를 넘으면 오래 안 본 것부터 지워요.
 * 입력: 고정하지 않은 덩이에 허용하는 바이트 limitBytes예요.
 * 출력: 합이 한도 이하이면 아무것도 지우지 않아요. 넘으면 최근에 본 것부터 한도까지 남기고 나머지를 지워요.
 * 실패: 문장이 실패하면 QueryFailed예요.
 */
    kgc::app::errors::Result<void> evictUnpinned(qint64 limitBytes);
/** 고정한 것까지 모든 덩이를 지우고 파일을 줄여요.
 * 입력: 없어요.
 * 출력: 빈 저장소예요.
 * 실패: 문장이 실패하면 QueryFailed예요.
 */
    kgc::app::errors::Result<void> clear();
private:
    BlobStore();
    struct Private;
    std::unique_ptr<Private> d_;
};
}
