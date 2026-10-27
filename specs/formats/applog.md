# 규격: 앱 로그 (BASE §10)

- 상태: 초안 2 — 2026-10-24, 0.0.1 스펙과 함께(검토 3 반영: 수준 매크로, Qt 메시지, 예시 줄)
- 자리: `Paths::logsApp()/kgc.log`(현재), `kgc.1.log` … `kgc.N.log`(회전). 쓰는 코드는 `platform/app/log/LogSink`뿐.

## 한 줄

```
2026-10-24T18:03:15.123+09:00 INFO  kgc.shell       shell.AppStarted       App started: 0.0.1, language: ko, data folder: /home/u/.local/share/K-GroundControl
2026-10-24T18:03:15.400+09:00 DEBUG kgc.shell       shell.MenuCollapsed    Left menu collapsed: false
2026-10-24T18:03:16.002+09:00 WARN  kgc.qt          qt.Message             QML Connections: Implicitly defined onFoo properties in Connections are deprecated
```

| 칸 | 내용 |
|---|---|
| 시각 | ISO 8601, 밀리초, 지역 오프셋 |
| 수준 | `DEBUG` `INFO` `WARN` `ERROR` (고정 폭 5) — 호출 매크로 `KGC_LOG_DEBUG/INFO/WARN/ERROR`가 정함 |
| 카테고리 | 모듈 경로 `kgc.<모듈>`(고정 폭 16, 넘치면 그대로) |
| 사건 | `<module>.<EventName>` — 모듈 `Events.h` 열거의 이름(영어). Qt 자체 메시지(`qInstallMessageHandler`로 들어오는 QML 경고 등)는 카테고리 `kgc.qt`, 사건 `qt.Message`, 수준은 QtMsgType대로(Debug→DEBUG, Info→INFO, Warning→WARN, Critical·Fatal→ERROR), 메시지는 원문 그대로 |
| 메시지 | 영어(2026-10-27 사용자 결정). 짧은 구, 첫 글자 대문자, 마침표 없음. 값은 콜론 뒤, 절은 쉼표. 한글 없음 |

- 칸 사이는 공백 하나 이상. 메시지 안의 줄바꿈은 `\n`으로 이스케이프.
- 비밀값(`Secret`)은 `LogSink`가 `***`로 가린다(0.0.1에는 비밀값 없음).

## 회전
- 파일이 `[log] maxFileMB`(기본 5)를 넘으면 `kgc.log → kgc.1.log → …`로 밀고 `maxFiles`(기본 5)를 넘는 것은 지운다.
- 수준은 `[log] level`(기본 `info`) 또는 `--log-level`. 이름은 `debug`·`info`·`warn`·`error`.
- 틀린 값: `--log-level`이 틀리면 시작하지 않고 오류 메시지(Windows는 메시지 상자)를 낸다. `kgc.ini`의 `[log] level`이 틀리면 그 값을 무시하고 기본값 `info`로 시작한다(파일 오타로 앱이 안 뜨지 않게).

## `kgc.ini`(부팅 설정) — 0.0.1에서 읽는 키
```ini
[log]
level = info
maxFileMB = 5
maxFiles = 5
[paths]
dataDir =            ; 비우면 기본값(QStandardPaths::AppLocalDataLocation)
```
