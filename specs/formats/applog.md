# 규격: 앱 로그 (BASE §10)

- 상태: 초안 1 — 2026-10-24, 0.0.1 스펙과 함께
- 자리: `Paths::logsApp()/kgc.log`(현재), `kgc.1.log` … `kgc.N.log`(회전). 쓰는 코드는 `platform/app/log/LogSink`뿐.

## 한 줄

```
2026-10-24T18:03:15.123+09:00 INFO  kgc.app.boot    boot.Started           앱 시작: 0.0.1, 언어: ko, 데이터 폴더: /home/u/.local/share/K-GroundControl
```

| 칸 | 내용 |
|---|---|
| 시각 | ISO 8601, 밀리초, 지역 오프셋 |
| 수준 | `DEBUG` `INFO` `WARN` `ERROR` `FATAL` (고정 폭 5) |
| 카테고리 | 모듈 경로 `kgc.<모듈>`(고정 폭 16, 넘치면 그대로) |
| 사건 | `<module>.<EventName>` — 모듈 `Events.h` 열거의 이름(영어) |
| 메시지 | 한국어 명사형 전보체. 값은 콜론 뒤, 절은 쉼표. 문장 어미 없음 |

- 칸 사이는 공백 하나 이상. 메시지 안의 줄바꿈은 `\n`으로 이스케이프.
- 비밀값(`Secret`)은 `LogSink`가 `***`로 가린다(0.0.1에는 비밀값 없음).

## 회전
- 파일이 `[log] maxFileMB`(기본 5)를 넘으면 `kgc.log → kgc.1.log → …`로 밀고 `maxFiles`(기본 5)를 넘는 것은 지운다.
- 수준은 `[log] level`(기본 `info`) 또는 `--log-level`.

## `kgc.ini`(부팅 설정) — 0.0.1에서 읽는 키
```ini
[log]
level = info
maxFileMB = 5
maxFiles = 5
[paths]
dataDir =            ; 비우면 기본값(QStandardPaths::AppLocalDataLocation)
```
