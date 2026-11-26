# app/store (바탕 0층)
역할: 기능을 모르는 범용 덩이 저장소 BlobStore를 SQLite 파일 하나로 제공해요. QtSql은 이 모듈에서만 써요(BASE §9.3).
수명: 저장소를 연 모듈이 BlobStore 하나를 쥐고, 만든 스레드에서만 써요.
받는 것: open의 파일 경로와 연결 이름, 열쇠·바이트·고정 여부, 고정하지 않은 덩이의 한도 바이트.
주는 것: BlobStore(open·get·put·pin·totalBytes·pinnedBytes·evictUnpinned·clear), StoreError(OpenFailed·QueryFailed), make, 번역 문맥 "Store"의 errorText.
기여: 없음.
의존: app/errors, app/log, Qt Core·Sql.
안 하는 것: 열쇠의 뜻(타일·메타데이터)과 깨진 파일을 지우고 다시 만드는 판단은 쓰는 모듈이 맡아요. 스레드를 만들지 않아요.
표: blobs(key 기본 열쇠, data, bytes, pinned, used ms), 색인 blobs_evict(pinned, used). WAL 일지, synchronous NORMAL.
지우기: put은 고정을 풀지 않아요. evictUnpinned는 고정하지 않은 합이 한도를 넘을 때만 최근에 본 것부터 한도까지 남겨요. clear는 모두 지운 뒤 VACUUM이에요.
로그: kgc.app.store — Opened(일지 모드), OpenFailed, QueryFailed(드라이버 글).
