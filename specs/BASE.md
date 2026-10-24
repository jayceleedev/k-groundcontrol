# K-GroundControl 기반 문서 (BASE)

- 상태: **초안 2 (검토 1 반영, 모듈러 모놀리스로 재편, 승인 대기)** — 2026-10-24, PM 작성
- 이 문서가 정하는 것: **무엇을 어떻게 구분해서 어디에 두는가.** 모듈의 모양과 경계, 설정·상태·고정값·공통 함수·저장소·주기 작업·서비스·입출력의 자리, 로그·오류·알림의 규칙, 그것을 지키는 자동 검사. 기능이 무엇을 하는지는 `specs/REQUIREMENTS.md`와 `specs/features/`가 정한다.
- 이 문서가 승인되기 전에는 기능 스펙을 쓰지 않는다. 기능 스펙의 "기반" 절은 이 문서의 이름을 그대로 쓴다. 구현 중에 여기 없는 자리가 필요해지면 만들지 않고 이 문서로 돌아온다.
- **문서의 권위**: BASE가 원본이고, 저장소 루트 `AGENTS.md`(구현자 GPT용, 영어)는 그 번역이다. 둘이 어긋나면 BASE를 따른다. 구현 주체가 GPT(D12)이므로 이 문서는 "대화 기록 없이도 같은 구조를 세울 수 있게" 쓴다.

## 1. 구조의 이름 — 모듈러 모놀리스

실행 파일은 하나(K-GroundControl)이고, 안은 **경계가 분명한 모듈**로 나뉘며, 모듈끼리는 **공개 인터페이스(`api/`)로만** 말한다. 모듈은 두 종류다.

| 종류 | 뜻 | 자리 |
|---|---|---|
| **바탕 모듈(platform)** | 모든 기능이 쓰는 바탕. 기능을 모른다. 셋이 층을 이룬다: `app`(0) < `comm`(1) < `vehicle`(2) | `src/platform/<이름>/` |
| **기능 모듈(feature)** | 기능 하나 = 모듈 하나. 로직(`internal/`)과 화면(`ui/`)을 함께 갖는다 | `src/features/<id>/` |
| **조립(main)** | 모듈을 하나로 묶는 얇은 껍데기. 로직 없음 | `src/main/` |

**경계는 컴파일러가 지킨다.** 모듈마다 CMake 정적 라이브러리 타깃 하나(`kgc_platform_app`, `kgc_feature_esc`…). `target_include_directories`에 **`api/`만 PUBLIC**, `internal/`·`ui/`는 PRIVATE. 다른 모듈의 `internal/`을 include하면 컴파일이 실패한다. 스크립트 검사(§14)는 컴파일러가 못 보는 것만 맡는다.

### 1.1 첫째 원칙 — 부분만 보고 고칠 수 있다 (R-N1~N7)

| # | 원칙 | 코드에서의 뜻 |
|---|---|---|
| 1 | 모듈 = 폴더 하나 | 밖에서 쓰는 것은 `api/`에만. 내부는 `internal/`(기능 모듈은 `ui/`도) |
| 2 | 모듈마다 자기 설명서 | 폴더 안 `MODULE.md`(§12). 공개 헤더나 모듈 간 의존이 바뀐 커밋에서 함께 고친다 |
| 3 | 공유 상태는 주인 하나 | 묶음마다 주인 하나(§5 주인 표). 나머지는 읽기만 |
| 4 | 펌웨어 차이는 한 계층 | `platform/vehicle/firmware/`의 플러그인이 **데이터와 순수 함수**로 내놓고(§8), 실행은 기능 모듈이 그 데이터를 해석해서 한다 |
| 5 | 크기 상한 | 파일 **400줄**, 함수 **60줄**, QML 파일 **300줄**(물리 줄, 빈 줄·주석 포함). 넘으면 쪼갠다. 제3자·생성 코드는 제외 |
| 6 | 의존은 한 방향 | 바탕: app < comm < vehicle. 기능 모듈은 바탕 셋과 다른 기능의 `api/`를 쓴다. 기능 안: `ui/` → `internal/` → `api/` 한 방향. 바탕은 기능을 모른다. 순환 금지(CMake가 잡음). 아래가 위로 알릴 때는 Qt 시그널 |
| 7 | 이름은 용어집에서 | §13의 낱말만. 새 낱말은 용어집에 먼저 |

## 2. 기술 기반 (D3·D4·R-N18)

- 언어 **C++23**(`std::expected`를 쓰기 위해 — 승인 사항 §17-1. 대안: C++20 + `tl::expected`). 컴파일러 MSVC 2022 17.8+, GCC 13+.
- Qt 6.12 LTS. 바탕 `app`이 쓸 수 있는 Qt 모듈: Core, Network, Sql, Positioning, TextToSpeech. `comm`: + SerialPort. `vehicle`: Core만 추가 없음. 기능 모듈: + Quick/Qml, Location, Multimedia, Graphs, Quick3D(필요한 기능만). GPL-3.0이므로 모듈 제한 없음.
- 빌드 CMake ≥ 3.28 + Ninja, `CMakePresets.json`(linux-debug·linux-release·windows-release). Qt는 aqtinstall로 패치 버전 고정, C/C++ 의존은 CPM(태그·체크섬 고정), vcpkg 안 씀. 제3자 코드는 CPM 캐시에 두고 **모든 구조 검사에서 제외**.
- MAVLink: `mavlink/c_library_v2`를 CPM으로 커밋 고정. 방언 **ardupilotmega**(common 포함; MAG_CAL_PROGRESS·ACCELCAL_VEHICLE_POS·FLASH_BOOTLOADER가 여기 있음) — 승인 사항 §17-2(대안 `all`). include는 `platform/comm/mavlink/api/Mavlink.h` **한 곳**을 통해서만(설정 매크로를 정한 뒤 include). 1~2층과 기능 모듈 `internal/`만 허용, `ui/`는 금지. PX4 이벤트 해석은 libevents(CPM).
- 네임스페이스 `kgc::<모듈>` (`kgc::app`, `kgc::comm`, `kgc::vehicle`, `kgc::esc`, `kgc::tracker`…). 기능 모듈끼리 같은 이름(`Protocol`)이 부딪히지 않게 모듈 네임스페이스를 뗀다.
- 버전은 `CMakeLists.txt`의 `project(VERSION)` 한 곳. 정보 창·로그·산출물 이름이 읽는다. `0.0.1`부터.

## 3. 폴더

```
k-groundcontrol/
├─ CMakeLists.txt  CMakePresets.json  cmake/(CPM, deploy, notices)
├─ src/
│  ├─ platform/
│  │  ├─ app/        0층  boot settings secrets paths store scheduler job net log errors notify i18n units geo crc util
│  │  ├─ comm/       1층  links(serial udp tcp mock) mavlink(parser channel signing) lease(PortLease) tlog joystick
│  │  └─ vehicle/    2층  registry Vehicle states firmware(px4 ardupilot generic) outbound(VehicleSender) safety(ArmGuard WorkLock ArmedPolicy) command(CommandQueue)
│  ├─ features/
│  │  ├─ shell/      메인 창·틀(상단바·좌 메뉴·우 사이드바·하단바)·테마·공용 부품
│  │  ├─ fly/        지도·비행 조작 띠·유도 명령·연결 팝업
│  │  ├─ mission/    미션 계획·전송(지도 패널 안)
│  │  ├─ firmware/   펌웨어 세팅(FC 굽기)
│  │  ├─ layout/     기체 구성도
│  │  ├─ sensors/  radio/  flightModes/  power/  safety/  motors/  tuning/  logs/
│  │  ├─ esc/  elrs/  tracker/
│  │  ├─ params/     수동 기체 설정
│  │  ├─ appSettings/
│  │  ├─ ai/         AI 세팅 패널
│  │  ├─ notify/     알림 벨·토스트·TTS 화면(알림 저장은 platform/app/notify)
│  │  ├─ terrain/    온라인 고도·지형 요청 응답
│  │  ├─ gcsposition/ GCS GPS 수신기
│  │  └─ video/
│  └─ main/          main.cpp Composition(모듈 등록·조립)
├─ tracker/          ESP32 스케치 + protocol/ 공용 헤더 (§11)
├─ firmware/tracker/<변종>/  CI가 넣는 트래커 이미지(설치본 포함)
├─ i18n/             kgc_en.ts 등
├─ resources/        qrc, icons/, silhouettes/, data/<이름>/ (원본 데이터)
├─ tools/check/      구조 검사(개발용 Python 3) + hooks/
├─ tests/            프로토콜 시험 벡터·순수 함수 단위 테스트 (§17-3 승인 사항)
├─ docs/  specs/  .github/workflows/  AGENTS.md  LICENSE  THIRD_PARTY_NOTICES.md  README.md
└─ .pm/              PM 작업 파일(.gitignore)
```

모듈 폴더 안의 모양:

```
features/esc/
├─ api/        EscService.h  EscStates.h  (밖에서 보는 전부)
├─ internal/   로직·프로토콜·Constants.h  Events.h  Errors.h
├─ ui/         EscPanel.qml  EscController.{h,cpp}  (화면)
├─ CMakeLists.txt
└─ MODULE.md
```

바탕 모듈은 `ui/`가 없다. 기능 모듈 중 화면이 없는 것(terrain·gcsposition)은 `ui/`가 없다.

### 3.1 기능 모듈 식별자 (좌 메뉴·설정 카테고리·폴더가 같은 이름을 쓴다)

| id | 한국어 | 좌 메뉴 | 요구 |
|---|---|---|---|
| `shell` | 틀 | — | R-N24, UI 그림 |
| `fly` | 비행 | (바탕 화면) | R-Q4~R-Q6 |
| `mission` | 미션 | (지도 패널) | R-Q7·R-Q8 |
| `firmware` | 펌웨어 세팅 | 1 | R-Q10 |
| `layout` | 기체 구성도 | 2 | 5.5절 |
| `sensors` | 센서 보정 | 3 | S-38~48 |
| `radio` | 조종기 | 4 | S-49~52 |
| `flightModes` | 비행 모드 | 5 | S-53·54 |
| `power` | 배터리·전원 | 6 | S-55~57 |
| `safety` | 안전·페일세이프 | 7 | S-62~64 |
| `motors` | 모터 시험 | 8 | S-58·59 |
| `tuning` | 튜닝 | 9 | S-65~68 |
| `logs` | 로그 분석 | 10 | E1·E2 |
| `esc` | ESC 설정 | 11 | 5.2절 |
| `elrs` | ELRS 설정 | 12 | 5.3절 |
| `tracker` | 안테나 트래커 | 13 | 5.4절 |
| `params` | 수동 기체 설정 | 14 | R-D1·D2, S-10~14 |
| `appSettings` | 앱 설정 | 15 | R-N17 |
| `ai` | AI 세팅 | (상단 오른쪽) | R-D4~D8 |
| `notify` | 알림 | (상단 벨) | §10 |
| `terrain` `gcsposition` `video` | 지형 · GCS GPS · 영상 | — | C6, R-Q3, R-Q6 |

## 4. 설정 — 세 가지를 섞지 않는다

| 종류 | 뜻 | 자리 | 바꾸는 사람 | 저장 |
|---|---|---|---|---|
| **부팅 설정** | 뜨기 전에 정해지는 것: 데이터 폴더, 로그 수준·회전, 개발 플래그(`--sitl`, `--display`) | `platform/app/boot/BootOptions`: 명령줄 + `kgc.ini`(`QStandardPaths::AppConfigLocation`) | 개발자·고급 사용자 | ini |
| **운영 설정** | 쓰면서 바꾸는 값 | `platform/app/settings/`: 정의는 각 기능 모듈의 `internal/settings/<id>.settings.json`(모듈이 소유, 빌드 때 모음), 값은 `Settings`가 `QSettings`로 | 사용자 | QSettings(플랫폼 기본 위치, 데이터 폴더와 무관 — 승인 사항 §17-5) |
| **고정값** | 바꾸면 안 되는 수 | 모듈 `internal/Constants.h`; 여러 모듈이 쓰는 값만 `platform/app/constants/`(§6) | 코드 수정 | 코드 |

- 운영 설정은 **메타데이터 먼저**(R-N17). JSON 한 항목: `id`(= `<모듈>.<이름>`), `type`(`bool|int|double|string|enum|list<object>`), `default`, `min/max`, `enum`, `labelKey`(번역 키), `category`(모듈 id). 빌드 때 생성기가 `Settings::<모듈>::<이름>()` 접근자를 만들어 **없는 키는 컴파일 오류**가 되게 한다. 코드에 설정 키 문자열을 쓰지 않는다. 앱 설정 화면의 검색은 이 JSON을 읽는다.
- 설정 기본값은 JSON에만 있다(코드 상수와 두 곳 금지). 예: 길게 누르기 500ms, 유도 거리 1000m, 고도 2~121.92m.
- **비밀값**(API 키·서명 키·바인드 문구·WiFi 비밀번호): `platform/app/secrets/SecretStore`(QtKeychain, BSD-3, CPM — 승인 사항 §17-4). 값은 `Secret` 형으로만 다룬다: `QString` 암시 변환 없음, `QDebug <<` 없음, `reveal()`은 허용 파일에서만(검사). 설정 JSON에는 "있음/없음"만.
- **파일 자리**(`platform/app/paths/Paths`가 정한다. 데이터 폴더 아래):

| 하위 폴더 | 내용 |
|---|---|
| `logs/app` | 앱 로그(회전) |
| `logs/tlog` | 텔레메트리 로그 |
| `logs/onboard` | 내려받은 기체 로그 |
| `missions` | .plan |
| `layouts` | 기체 구성도 + 파라미터 묶음 |
| `params/backups` | 쓰기 전 전체 백업(R-D5 ⑤) |
| `ai/changes` | AI·구성도 변경 사유 기록(R-D5 ⑩) |
| `video` | 녹화 |
| `cache/firmware` | 내려받은 펌웨어(해시 기록) |
| `cache/tiles` `cache/metadata` | 지도 타일, 파라미터 메타데이터 |

## 5. 상태(states) — 묶음마다 주인 하나

- 묶음은 `Q_GADGET` 값 구조체. 기체마다 `VehicleStates`(QObject) 하나가 묶음을 `Q_PROPERTY … NOTIFY <bundle>Changed`로 갖는다. QML은 활성 기체의 `VehicleStates`에 직접 바인딩해도 된다.
- **주인 표** — 주인만 쓴다. 나머지는 읽기만. 같은 값을 자기 캐시로 들지 않는다.

| 묶음 | 담는 것 | 주인 | 출처 |
|---|---|---|---|
| `Link` | 연결·주 링크·손실률·하트비트 시각·끊김 | `vehicle/states/VehicleStateOwner` | HEARTBEAT, RADIO_STATUS, 시퀀스 |
| `Arming` | armed, 모드(id+표시 이름), 준비 상태, **기체 쪽 시동 불가 사유** | VehicleStateOwner | HEARTBEAT, SYS_STATUS, EVENT/STATUSTEXT(플러그인 해석 함수) |
| `Attitude` `Position` `Gps` `Battery[]` `Sensors` `Rc` `Flight` `Messages` `Esc[]` | (초안 1과 같음) | VehicleStateOwner | 각 MAVLink 메시지 |
| `Capabilities` | 기능 비트마스크, 펌웨어 버전·UID, 지원 범위(D17) 판정 | VehicleStateOwner — **단계별로 채움**: HEARTBEAT → AUTOPILOT_VERSION → 파라미터 완료 → 탐침. 파라미터·탐침이 필요한 비트(D11 ESC 패스스루 등)는 기능 모듈이 `VehicleStateOwner::reportProbe(cap, ok)` 한 입구로 알린다 | HEARTBEAT, AUTOPILOT_VERSION, 파라미터, 탐침 |
| `Params` | 파라미터 값 전체, 내려받기 진행, PX4 해시 캐시 | `features/params`의 `ParamService` — 값을 드는 **유일한 곳** | PARAM_VALUE, FTP |
| `MissionSync` | 기체 미션 해시·동기 여부·현재 항목 | `features/mission`의 `MissionService` | MISSION_CURRENT, 전송 결과 |
| `ArmBlockers` | **GCS 쪽** 시동 불가 사유(보정 뒤 재부팅 전, 방향 변경 뒤 재보정 전, PASSTHRU_EN 확인 전, 작업 잠금 중…) | `vehicle/safety/ArmGuard` — 기능 모듈은 `block(reason) → 토큰`, `release(토큰)`으로만 | 기능 모듈의 요청 |

- 메시지 해석은 `vehicle/states/internal/updaters/<Bundle>Updater`의 **순수 함수**(메시지 → 묶음 변경). `VehicleStateOwner`는 배선만 해서 400줄을 넘지 않는다.
- 기체가 아닌 것의 상태도 같은 규칙: `features/tracker/api/TrackerStates`(주인 TrackerService), `features/gcsposition/api/GcsPositionStates`.
- 스레드: 링크 워커가 `LinkManager::messageReceived(linkId, msg)`를 **큐드 시그널**로 GUI 스레드에 올리고, Owner는 GUI 스레드에서 갱신한다. 묶음은 `Q_DECLARE_METATYPE`.

## 6. 고정값과 정해진 값의 이름

- 모듈의 고정값은 그 모듈 `internal/Constants.h`. **여러 모듈이 쓰는 값만** `platform/app/constants/`: `mavlink.h`(ACK 1200ms·3회, 하트비트 1Hz, 끊김 3500ms), `safety.h`(비상 정지 21196, 강제 시동 2989, 모터 시험 3초), `mission.h`(1500ms·250ms·5회). CRC 다항식은 `platform/app/crc/`에, 트래커 프로토콜 상수는 공용 헤더에만(§11).
- 검사(§14)는 **시간·횟수·주소·매직값**만 본다: `chrono` 리터럴, 타이머·Timeout 인자, 4자리 이상 16진수, 3자리 이상 10진수(허용 360·180·90·1000·60). 256 미만의 오프셋·시프트·인덱스는 대상이 아니다. QML 수치는 `features/shell/ui/theme/Theme.qml`에만.
- 열거(`enum class`)는 모듈 `api/`에. **펌웨어가 정하는 열거**(모드·회전·서보 기능)는 플러그인이 메타데이터에서 읽어 제공하고 코드에 박지 않는다.
- 로그 사건·오류 코드는 **모듈마다** `internal/Events.h`·`Errors.h`(모듈 접두사, `LogEvent::EscFlashFailed`). 검사가 전부 모아 중복·미등록을 본다.

## 7. 자료 규격(파일 형식)과 원본 데이터

| 형식 | 자리(코드) | 문서 |
|---|---|---|
| 설정 메타데이터 `*.settings.json` | `platform/app/settings` | `specs/formats/settings.md` |
| 미션 `.plan`(QGC v1 호환), `.waypoints` 읽기, KML 내보내기, KML/SHP 가져오기, 명령 카탈로그 JSON | `features/mission` | `specs/formats/plan.md`, `mission-commands.md` |
| 기체 구성도 `.kgclayout.json` + `.params`, 실루엣 | `features/layout`, `resources/silhouettes/` | `specs/formats/layout.md` |
| 파라미터 파일(QGC 형식 + MP 2열 읽기), 묶음 이름·작업 단위 묶음 정의(펌웨어·버전별) | `features/params`, `resources/data/params/` | `specs/formats/params.md`, `taskset.md` |
| 비행 전 점검표 정의 | `features/fly`, `resources/data/preflight/` | `specs/formats/preflight.md` |
| 텔레메트리 로그 `.tlog`, CSV 텔레메트리 | `platform/comm/tlog` | `specs/formats/tlog.md` |
| 기체 로그 `.ulg` `.bin` 읽기 | `features/logs` | `specs/formats/onboard-log.md` |
| 트래커 프로토콜 | `tracker/protocol/kgc_tracker_protocol.h`(원본, §11) | `specs/formats/tracker-protocol.md` |
| AI 변경 기록, 파라미터 백업 | `features/ai`, `features/params` | `specs/formats/changes.md` |
| 번역 `.ts/.qm` | `i18n/` | — |

규격 문서는 기능 스펙과 함께 쓰고, 읽기·쓰기 함수는 그 모듈 하나에만. 펌웨어별 데이터는 플러그인 아래 `data/`.

## 8. 펌웨어 플러그인 계층 (R-Q12·R-N4) — 데이터와 순수 함수만

- 자리: `platform/vehicle/firmware/api/FirmwarePlugin.h` + `internal/{px4,ardupilot,generic}/`.
- **선택**: 첫 HEARTBEAT의 `autopilot` 값으로 `FirmwarePluginFactory`가 고르고(모르면 generic) 바꾸지 않는다. 값이 바뀌면 기체를 새로 만든다.
- 플러그인이 **내놓는 것(데이터·순수 함수)**: 기능 비트마스크 초기값과 단계별 보강 규칙; 모드 표(id ↔ 표시 이름); 이륙·일시정지·고도 변경·속도 변경의 **`CommandStep` 목록**(어떤 메시지를 어떤 순서로, 어떤 ACK를 기다리는지 — 실행은 기능 모듈); 시동 불가 사유 해석 함수(PX4 events / AP PreArm 텍스트 → 사유 목록); 파라미터 메타데이터 출처와 이름 바꿈표; 미션 번호 규칙(PX4 홈 없음); 보정 방식 기술자(STATUSTEXT 파싱 / 역방향 명령); 구성도 대응표(R-D33); AI 거부 목록(R-D5 ⑥); 지원 버전 범위(D17); 고도 틀 값(`AltitudeFrame`) — `app/geo/Altitude`는 펌웨어를 모르고 frame을 인자로 받는다(R-N11).
- 플러그인은 **실행하지 않는다**(기체에 보내지 않고, 상태를 쓰지 않는다). 기능 모듈이 `plugin.takeoffSteps()`를 받아 §9의 VehicleSender로 보낸다.
- 규칙: `vehicle/firmware/` 밖에서 `MAV_AUTOPILOT_PX4`·`MAV_AUTOPILOT_ARDUPILOTMEGA` 비교와 `"PX4"`·`"ArduPilot"`·`"APM"` 문자열 비교는 검사가 잡는다. 화면은 비트마스크만 본다. 기능 모듈 안에 펌웨어별 하위 폴더를 두는 것은 **금지**(한 계층 원칙).

## 9. 조립·수명·송신·공통 틀

### 9.1 조립과 수명
- `src/main/Composition`이 모든 모듈의 `api/`를 include해 조립한다(층 밖, 로직 없음). `main.cpp`는 BootOptions → Composition → 창 띄우기.
- 링크는 `comm/links/LinkManager`가, 기체는 `vehicle/registry/VehicleRegistry`가 갖는다(sysid 키; compid 1이 자동조종기, 그 밖의 compid는 그 기체의 `Component`). **활성 기체**는 `VehicleRegistry::active` 하나뿐 — 상단바 기체 표시, 트래커 기본 대상, 구성도 편집 대상이 이것을 본다.
- 기체별 서비스(params·mission·calibration·safety 판정 등)는 Vehicle이 생길 때 Composition이 만들고 **부모를 Vehicle로** 둔다. 앱 전역 서비스(firmware·esc·elrs·tracker·ai·video·gcsposition·terrain)는 앱과 수명이 같다.
- 기체 삭제: `vehicleRemoved` 시그널 → `deleteLater`. QML·컨트롤러는 기체를 `QPointer` 또는 활성 기체 프로퍼티로만 잡는다.

### 9.2 기체로 나가는 문 하나
- `vehicle/outbound/VehicleSender`가 기체로 가는 **모든 MAVLink 송신**의 유일한 문(주 링크 선택, sysid·compid 채움). 송신마다 `OutboundKind`(Arm, Motion, Command, Param, Mission, Other).
- 모든 송신은 `vehicle/safety/WorkLock`(R-Q1a (나)형: 토큰을 쥔 작업의 송신만 통과, 그 작업 자신의 단계는 예외)과 `ArmedPolicy`(R-N10 표를 그대로 옮긴 한 표)를 거친다. 거부되면 `ErrorCode`로 돌려주고 알림.
- `vehicle/command/CommandQueue`는 VehicleSender 위의 **재시도 계층**(ACK·타임아웃·재시도). 보정 시작처럼 ACK가 늦는 명령은 `{retries=0, timeout=설정값}` 모드로 큐를 탄다 — 예외 목록 없음. AP 가속도 보정의 "다음"(COMMAND_ACK 송신)도 Sender를 지난다.
- 보내기 스레드: Sender가 `QMetaObject::invokeMethod(link, …, Qt::QueuedConnection)`으로 링크 워커에 넘긴다.
- 검사: 링크 쓰기 API 호출은 `comm/`과 `vehicle/outbound/` 밖에서 금지. `command_(long|int)_(pack|encode)`는 `vehicle/command/` 밖에서 금지.
- `comm/lease/PortLease`는 (가)형(원시 포트 독점)만. FC·ESC·ELRS·트래커 굽기는 임대 없이 포트를 열 수 없다.

### 9.3 바깥과 닿는 자리
| 무엇 | 유일한 자리 | 검사 |
|---|---|---|
| HTTP(S) | `platform/app/net/Http`: HTTPS만, 타임아웃·재시도, 다운로드 캐시+해시 기록, `onlineChanged`(R-N13) | `QNetworkAccessManager`가 밖에 있으면 잡음 |
| 환경 변수 | `platform/app/boot/BootOptions` | `qEnvironmentVariable`·`getenv` 위치 |
| 시리얼 포트 | `platform/comm/links/serial` | `QSerialPort` 생성 위치 |
| 조이스틱(SDL3) | `platform/comm/joystick` | SDL include 위치 |
| SQL | `platform/app/store`(SQLite: 타일·메타데이터·펌웨어 캐시) | QtSql include·`QSqlQuery` 위치 |
| TTS | `platform/app/notify/Speech` | — |

### 9.4 오래 걸리는 작업과 오류
- `platform/app/job/Job`(QObject): 상태(대기·진행·성공·실패·취소), 단계 이름, 진행률, 남은 시간, `cancel()`, 끝날 때 `finished(Result<void>)` 한 번. 임대·잠금·ArmGuard 토큰은 Job이 **RAII**로 쥔다. 화면은 공용 부품 `features/shell/ui/components/JobView.qml`로 보여 준다. FC·ESC·ELRS·트래커 굽기, 미션 전송, 파라미터 내려받기, 보정, 로그 내려받기가 전부 Job이다.
- `Result<T> = std::expected<T, ErrorCode>`(§17-1). 함수는 예외 대신 Result. 예외·`Q_ASSERT`·`qFatal`은 프로그램 버그에만. 워커 스레드의 오류는 `ErrorCode`를 큐드 시그널로 소유 객체에 올리고, 알림은 소유 객체만 낸다.

### 9.5 주기 작업과 타임아웃
- `platform/app/scheduler/Scheduler`에는 **이름 있는 반복 작업만** 등록: `register(name, period, owner, context)` — context 스레드에서 돈다. 초기 목록: GCS 하트비트 1Hz(링크 스레드), 끊김 검사 1Hz, 시리얼 포트 목록 1Hz, tlog 플러시, 트래커 각도(기체 위치를 받을 때마다, 최대 5Hz).
- 일회성 타임아웃은 이름이 붙는 `app/scheduler/Timeout`으로만. QML `Timer`는 `shell/ui/components/` 안에서만.
- 검사 대상: `QTimer`, `singleShot`, `startTimer(`, `QBasicTimer`, QML `Timer {`가 위 자리 밖에 있는 것.

### 9.6 공통 함수
- `platform/app/geo`: `Altitude`(AMSL·상대·지형·타원체고↔MSL, frame 인자) **한 묶음**, `Geodesy`(방위·고각·거리 — R-D23의 한 함수). `app/units`(표시 단위), `app/crc`(CRC16-XMODEM, CRC-0xA001, CRC8-0xD5 — 시험 벡터는 `tests/`), `app/util`.
- 네임스페이스 범위의 자유 함수가 두 모듈에 같은 이름이면 검사가 잡는다(멤버 함수는 대상 아님).

### 9.7 화면
- `features/shell/ui/`: `Shell.qml`(메인 창, 최소 1920×900 R-N24), `topbar/`, `leftbar/`, `rightbar/tabs/{overview,detail,messages,logs,preflight}/`, `bottombar/`, `theme/Theme.qml`, `components/`(HoldToConfirm, JobView, Pill…).
- 각 기능 모듈의 `ui/`: 패널 QML + `<Id>Controller`. 컨트롤러가 할 수 있는 일은 **상태를 표시값으로 옮기는 것과 서비스 호출**뿐. 안전 판정은 서비스·ArmedPolicy가 하고 화면은 결과만 보여 준다. 패널은 shell이 `panelRegistry`로 모아 좌 메뉴 순서(§3.1)대로 띄운다.
- `features/fly/ui/map/`: 지도와 레이어(기체·궤적·미션·펜스·랠리·ADS-B·트래커 부채꼴). 미션 계획 화면은 미션 기능 스펙 때 이 절을 보강한다.
- 영상 디코드·타일 로드는 워커 스레드(R-N15).

## 10. 로그·오류·알림·번역

- **로그**: 호출은 `KGC_LOG(category, LogEvent::X, "메시지", 값…)` 매크로 **하나**로만. `qDebug`·`qInfo`·`qWarning` 직접 호출 금지(검사). 카테고리는 모듈 이름(`kgc.comm.serial`, `kgc.esc`). 메시지는 **한국어 명사형 전보체**(값은 콜론 뒤, 절은 쉼표, 문장 어미 없음). 사건 이름은 모듈 `Events.h`의 영어 식별자. `platform/app/log/LogSink`가 Qt 메시지 핸들러로 파일 회전(부팅 설정)과 비밀값 마스킹을 한다. tlog·포워딩에는 `comm/mavlink/signing`의 필터가 SETUP_SIGNING을 뺀다.
- **오류**: 모듈 `Errors.h`의 `ErrorCode`(영어). 사용자 문구(한국어, 번역 대상)는 `tr()`.
- **알림**: `platform/app/notify/Notify`(저장·수준 `Info/Warning/Critical`·출처·시각·읽음) — 상단 벨·토스트·TTS가 읽는 한 입구. 기체 메시지 중 무엇을 알림으로 올릴지는 **기능 스펙(B4)이 정한다**(BASE는 자리만).
- **번역**: 원문 **한국어**, `i18n/kgc_en.ts`가 영어 — 승인 사항 §17-6. `platform/app/i18n/Translator.retranslate()`로 재시작 없는 전환(R-N16). 검사: C++의 한글 리터럴이 `tr(`·`QT_TR_NOOP(`·`KGC_LOG(` 밖에 있으면, QML의 `text:`·`title:`·`placeholderText:`·`ToolTip.text:` 리터럴이 `qsTr(` 없이 있으면 잡는다.

## 11. 트래커 스케치 `tracker/` (R-D27)

- `tracker/`에 둔다(별도 프로그램, 층 규칙 없음, 크기 검사는 받음). 빌드 도구(PlatformIO/ESP-IDF)는 **트래커 기능 스펙에서** 정한다. `.github/workflows/tracker.yml`이 변종별 이미지를 만들고 패키징이 `firmware/tracker/<변종>/`에 넣는다(R-D28).
- 공용 헤더 `tracker/protocol/kgc_tracker_protocol.h`: **C99 헤더 전용**, `static inline` 함수가 바이트를 리틀 엔디언으로 직접 읽고 쓴다(패킹 구조체 금지 — MSVC/GCC 차이). 프레임 CRC·매직값·프로토콜 버전·명령 id·절대 한계는 **이 헤더에만**. GCS 쪽은 `features/tracker/internal/protocol/`이 이 파일 하나를 include한다(복사 금지; src 밖 include 허용은 이곳뿐).
- 장치 설정(서보 범위·반전·중립·슬루·히스테리시스·편각·생존 시간)의 **주인은 ESP32 NVS**. GCS는 `TrackerStates`로 읽고 "설정 쓰기" 명령으로만 바꾼다. GCS 운영 설정에는 추적 대상·위치 출처·고정 좌표만.

## 12. 모듈 설명서 `MODULE.md` 틀

```
# <모듈 id> (<바탕 n층 | 기능>)
역할: 한 문장.
받는 것: 읽는 상태 묶음·시그널·설정(이름으로).
주는 것: api/의 클래스·함수·시그널·상태 묶음.
의존: 쓰는 모듈(api 이름).
안 하는 것: 일부러 하지 않는 일과 그것을 하는 모듈.
흐름(사용자가 따라가는 진입점이 있으면): 1. … 2. …
```
- 100줄 이하. 공개 헤더나 모듈 간 의존이 바뀐 커밋에서 함께 고친다(검사).

## 13. 용어집 (한국어 ↔ 식별자)

| 한국어 | 식별자 | 뜻 |
|---|---|---|
| 바탕 모듈 / 기능 모듈 | `platform` / `feature` | §1 |
| 공개 인터페이스 | `api` | 모듈 밖에서 보는 전부 |
| 기체 / 활성 기체 | `Vehicle` / `ActiveVehicle` | MAVLink 시스템 / 화면이 보는 기체 |
| 컴포넌트 | `Component` | 기체 안 카메라·짐벌 등 |
| 링크 / 주 링크 | `Link` / `PrimaryLink` | |
| 상태(묶음) / 주인 | `States`(`Bundle`) / `Owner` | §5 |
| 기능 비트마스크 | `Capability` | |
| 펌웨어 플러그인 / 명령 단계 | `FirmwarePlugin` / `CommandStep` | §8 |
| 송신 문 / 명령 큐 | `VehicleSender` / `CommandQueue` | §9.2 |
| 작업 잠금 / 포트 임대 / 시동 지킴이 / 시동 중 규칙 | `WorkLock` / `PortLease` / `ArmGuard` / `ArmedPolicy` | |
| 작업 | `Job` | §9.4 |
| 서비스 / 컨트롤러 / 패널 / 메뉴 | `Service` / `Controller` / `Panel` / `Menu` | 기능 모듈의 로직 / 화면 접점 / 가운데 화면 / 좌 항목 |
| 파라미터 / 묶음(계열) / 작업 단위 묶음 | `Param` / `ParamGroup` / `TaskSet` | |
| 미션 / 유도 명령 | `Mission` / `GuidedAction` | |
| 시동 / 해제 / 비상 정지 / 길게 눌러 확인 | `Arm` / `Disarm` / `EmergencyStop` / `HoldToConfirm` | |
| 기체 구성도 / 부품 표식 | `Layout` / `LayoutMarker` | 5.5절 |
| 안테나 트래커 / 받침 정면 방위 | `Tracker` / `BaseHeading` | 5.4절 |
| 알림 / 기체 메시지 | `Notification` / `VehicleMessage` | §10 / STATUSTEXT |
| 앱 로그 / 텔레메트리 로그 / 기체 로그 | `AppLog` / `TelemetryLog` / `OnboardLog` | |
| 로그 사건 / 기체 이벤트 | `LogEvent` / `VehicleEvent` | 우리 로그 / PX4 events 프로토콜 |
| 운영 설정 / 부팅 설정 / 고정값 / 비밀값 | `Setting` / `BootOption` / `Constant` / `Secret` | §4 |

## 14. 자동 검사 `tools/check/` (R-N21)

커밋 훅(`core.hooksPath=tools/check/hooks`)과 CI(`before..after` 범위 커밋마다)가 같은 스크립트를 돈다. 함수 길이는 lizard(개발용, 버전 고정 — §17-7). 제3자·생성 코드 제외.

| 검사 | 규칙 | 도입 |
|---|---|---|
| 경계 | (컴파일러) 다른 모듈 `internal/`·`ui/` include 불가; 스크립트는 바탕 층 순서(app<comm<vehicle), 바탕→기능 include 금지, 기능 안 ui→internal→api 방향 | 0.0.1 |
| 크기 | 파일 400 / 함수 60 / QML 300(물리 줄) | 0.0.1 |
| MODULE.md | 모듈마다 있고 100줄 이하; api·의존 변경 커밋에 함께 | 0.0.1 |
| MAVLink include | `comm/mavlink/api/Mavlink.h` 밖의 `mavlink/*.h` include, `ui/`의 MAVLink include | 0.1.x |
| 펌웨어 분기 | §8 규칙 | 0.1.x |
| 송신 | §9.2 규칙 | 0.1.x |
| 바깥 접점 | §9.3 표 | 각 자리가 처음 쓰일 때 |
| 타이머 | §9.5 | 0.1.x |
| 고정값 | §6 범위 | 0.1.x |
| 설정 키 | 설정 키 문자열 리터럴(생성 접근자 밖) | 0.1.x |
| 로그·오류 | `qDebug` 계열 직접 호출; 등록부 밖 사건·오류 이름; `KGC_LOG` 메시지에 문장 어미(…다/…요) | 0.1.x |
| 문자열 | §10 번역 규칙 | 0.0.1(QML), 0.1.x(C++) |
| 비밀값 | `Secret::reveal()`이 허용 파일 밖 | 비밀값이 처음 쓰일 때 |
| 이름 중복 | 자유 함수 이름이 두 모듈에 | 0.1.x |

## 15. 빌드·CI·배포 (R-N18~R-N22)

- `.github/workflows/`: `check.yml`(구조 검사·`tests/`), `linux.yml`(Ubuntu 24.04 → AppImage), `windows.yml`(MSVC 2022, Qt 6.12.x aqtinstall → zip), `tracker.yml`. 태그에서 Release.
- 산출물에 `LICENSE`, `THIRD_PARTY_NOTICES.md`(CPM 목록 + `cmake/notices/extra.json`의 CPM 밖 의존 — Qt·GStreamer·MSVC 런타임·트래커 라이브러리 — 를 CI가 합쳐 생성), 소스 위치. SBOM은 0.1.x부터.
- 0.0.1 산출물: Windows zip(windeployqt·MSVC 런타임) + AppImage. GStreamer·NSIS는 영상 버전부터.

## 16. 이 문서가 정하지 않는 것

각 기능의 동작(기능 스펙), 화면의 모양(`specs/ui/`), 바이트 명세(규격 문서), 테스트 내용, 알림 승격 기준(B4 스펙), 트래커 빌드 도구(트래커 스펙), 성능 수치.

## 17. 승인 사항 (사용자가 정할 것)

| # | 물음 | PM 제안 | 대안 |
|---|---|---|---|
| 1 | C++ 표준과 Result 형 | **C++23 + `std::expected`** | C++20 + `tl::expected`(CPM) |
| 2 | MAVLink 방언 | **ardupilotmega**(common 포함) | `all` |
| 3 | 테스트 코드 자리 | **프로토콜 시험 벡터·순수 함수 단위 테스트만 `tests/`**(R-D13·R-N19가 요구), 기능 확인 테스트는 `.pm/tests/` | 전부 `.pm/` |
| 4 | 비밀값 저장 라이브러리 | **QtKeychain**(BSD-3, CPM) | 직접 OS API |
| 5 | 운영 설정 저장 | **QSettings**(플랫폼 기본 위치) | 데이터 폴더 안 JSON |
| 6 | 번역 원문 언어 | **한국어**(영어는 `.ts`) | 영어 원문 |
| 7 | 개발용 Python 의존 | **Python 3 + lizard**(검사 전용, 배포물 제외) | C++로 검사 작성 |
| 8 | 폴더·네임스페이스·크기 상한 | §3·§2·§1.1 그대로 | — |

## 18. 검토·승인 기록

| 날짜 | 누가 | 무엇 |
|---|---|---|
| 2026-10-24 | reviewer-1 | 설계 검토 1: fail, 지적 20개(`.pm/base/reviewer-1.md`) → 초안 2에 반영(모듈러 모놀리스 재편 포함) |
| | reviewer-2 | (대기) |
| | 사용자 | (대기) |
