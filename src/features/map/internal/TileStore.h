// [0.5.0 5.6.5] specs/features/0.5.0-map.md
#pragma once
#include <kgc/app/errors/ErrorCode.h>
#include <kgc/app/store/BlobStore.h>
#include <QByteArray>
#include <QObject>
#include <QString>
#include <memory>
namespace kgc::map {
/** 타일 바이트를 SQLite 덩이 저장소에 두는 워커 스레드 객체예요.
 * 입력: 함수 호출이에요. 모두 저장소 스레드에서 불러요(GUI는 QMetaObject::invokeMethod로 넘겨요).
 * 출력: 답은 시그널로 내요. 부모가 없고 생성자는 일하지 않아요.
 * 실패: 열지 못하면 파일을 한 번 지우고 다시 열고, 그래도 안 되면 failed를 내고 저장 없이 답해요
 *       (lookup은 missing, pin은 pinned(id, false), save는 버림, size·clear는 failed).
 */
class TileStore : public QObject {
    Q_OBJECT
public:
/** 아직 열지 않은 저장소를 만들어요.
 * 입력: 없어요.
 * 출력: 저장 없이 답하는 상태예요. open을 불러야 저장해요.
 * 실패: 없어요.
 */
    TileStore();
/** 저장소 파일을 닫아요.
 * 입력: 없어요.
 * 출력: 연결 이름을 놓아요. 저장소 스레드에서 지워져야 해요.
 * 실패: 없어요.
 */
    ~TileStore() override;
/** 저장소 파일을 열고 본 타일을 한도에 맞춰요.
 * 입력: SQLite 파일 경로 path와 본 타일의 바이트 한도 limitBytes예요.
 * 출력: 열리면 evictUnpinned를 한 번 해요.
 * 실패: 열지 못하면 path·-wal·-shm 파일을 지우고(폴더는 두고) 다시 열어요. 또 실패하면 failed(StoreFailed)를 내고 저장 없이 답해요.
 */
    void open(const QString& path, qint64 limitBytes);
/** 본 타일의 한도를 바꾸고 바로 맞춰요.
 * 입력: 새 바이트 한도 limitBytes예요.
 * 출력: 열려 있으면 evictUnpinned를 한 번 해요. 저장 없이 답하는 동안은 한도만 바꿔요.
 * 실패: 지우기가 실패하면 failed를 내요.
 */
    void setLimit(qint64 limitBytes);
/** 열쇠의 타일을 찾아요.
 * 입력: 요청 번호 requestId와 열쇠 key예요.
 * 출력: 있으면 found(requestId, bytes)(그 타일을 본 시각을 지금으로), 없으면 missing(requestId)이에요.
 * 실패: 읽기가 실패하면 missing과 failed를 내요.
 */
    void lookup(quint64 requestId, const QString& key);
/** 타일을 저장해요.
 * 입력: 열쇠 key, 바이트 bytes, 고정 여부 pinned예요.
 * 출력: 저장하고, kEvictEverySaves번마다 그때의 한도로 evictUnpinned를 해요.
 * 실패: 쓰기가 실패하면 failed를 내요. 저장 없이 답하는 동안은 버려요.
 */
    void save(const QString& key, const QByteArray& bytes, bool pinned);
/** 있는 타일을 고정해요.
 * 입력: 요청 번호 requestId와 열쇠 key예요.
 * 출력: pinned(requestId, 그 타일이 있었는지)예요.
 * 실패: 쓰기가 실패하면 pinned(requestId, false)와 failed를 내요.
 */
    void pin(quint64 requestId, const QString& key);
/** 저장소 크기를 재요.
 * 입력: 없어요.
 * 출력: sized(모든 타일 바이트, 고정한 타일 바이트)예요.
 * 실패: 읽기가 실패하거나 저장 없이 답하는 동안은 failed를 내요.
 */
    void size();
/** 모든 타일을 지워요.
 * 입력: 없어요.
 * 출력: cleared예요.
 * 실패: 지우기가 실패하거나 저장 없이 답하는 동안은 failed를 내요.
 */
    void clear();
signals:
/** 찾은 타일을 알려요. 입력: 요청 번호 requestId와 바이트 bytes예요. 출력: 없어요. 실패: 없어요. */
    void found(quint64 requestId, QByteArray bytes);
/** 타일이 없음을 알려요. 입력: 요청 번호 requestId예요. 출력: 없어요. 실패: 없어요. */
    void missing(quint64 requestId);
/** 고정 결과를 알려요. 입력: 요청 번호 requestId와 고정했는지 pinned예요. 출력: 없어요. 실패: 없어요. */
    void pinned(quint64 requestId, bool pinned);
/** 저장소 크기를 알려요. 입력: totalBytes와 pinnedBytes예요. 출력: 없어요. 실패: 없어요. */
    void sized(qint64 totalBytes, qint64 pinnedBytes);
/** 모두 지웠음을 알려요. 입력: 없어요. 출력: 없어요. 실패: 없어요. */
    void cleared();
/** 저장소를 쓰지 못했음을 알려요. 입력: 오류 error(MapError::StoreFailed)예요. 출력: 없어요. 실패: 없어요. */
    void failed(kgc::app::errors::ErrorCode error);
private:
    std::unique_ptr<kgc::app::store::BlobStore> store_;
    qint64 limitBytes_ = 0;
    int saves_ = 0;
};
}
