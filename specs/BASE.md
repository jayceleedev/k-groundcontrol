# K-GroundControl 기반 문서 (BASE)

- 상태: **초안 3 (검토 1·2 반영, 승인 대기)** — 2026-10-24, PM 작성
- 이 문서가 정하는 것: **무엇을 어떻게 구분해서 어디에 두는가.** 모듈의 모양과 경계, 설정·상태·고정값·공통 함수·저장소·주기 작업·서비스·입출력의 자리, 로그·오류·알림의 규칙, 그것을 지키는 자동 검사. 기능이 무엇을 하는지는 `specs/REQUIREMENTS.md`와 `specs/features/`가 정한다.
- 이 문서가 승인되기 전에는 기능 스펙을 쓰지 않는다. 기능 스펙의 "기반" 절은 이 문서의 이름을 그대로 쓴다. 구현 중에 여기 없는 자리가 필요해지면 만들지 않고 이 문서로 돌아온다.
- **문서의 권위**: BASE가 원본이고, 저장소 루트 `AGENTS.md`(구현자 GPT용, 영어)는 그 번역이다. 어긋나면 BASE를 따른다. 구현 주체가 GPT(D12)이므로 이 문서는 "대화 기록 없이도 같은 구조를 세울 수 있게" 쓴다.
- **요구사항과의 관계**: 이 문서는 요구사항 R-N6의 "층 0~4"를 모듈러 모놀리스로 **구체화**한다(§1.2 대응표). R-N6·R-N2의 문구 개정은 승인 사항 §17-9.

## 1. 구조의 이름 — 모듈러 모놀리스

실행 파일은 하나(K-GroundControl)이고, 안은 **경계가 분명한 모듈**로 나뉘며, 모듈끼리는 **공개 인터페이스(`api/`)로만** 말한다.

| 종류 | 뜻 | 자리 | 의존 |
|---|---|---|---|
| **바탕 모듈(platform)** | 모든 기능이 쓰는 바탕. 기능을 모른다. 넷이 층을 이룬다: `app`(0) < `comm`(1) < `vehicle`(2) < `ui`(2.5, 화면 바탕) | `src/platform/<이름>/` | 아래층만 |
| **기능 모듈(feature)** | 기능 하나 = 모듈 하나. 로직(`internal/`)과 화면(`ui/`)을 함께 갖는다 | `src/features/<id>/` | 바탕 넷 + **순위표(§3.2)에서 아래인 기능의 `api/`**만 |
| **조립(main)** | 모듈을 하나로 묶는 얇은 껍데기. 로직 없음 | `src/main/` | 전부 |

**경계는 두 겹으로 지킨다.**
- **컴파일러**: 모듈마다 CMake 정적 라이브러리 타깃 하나(`kgc_platform_app`, `kgc_feature_esc`…). `target_include_directories`에 **`api/`만 PUBLIC**, `internal/`·`ui/`는 PRIVATE. include 형식은 **`<kgc/<module>/X.h>`**(`api/kgc/<module>/` 아래에 둔다 — 헤더 이름 충돌 방지). 다른 모듈의 `internal/`은 꺾쇠로는 닿지 않는다.
- **검사(§14)**: 컴파일러가 못 보는 것 — `..`가 든 상대 경로 include(금지), `api/` 헤더가 `internal/`을 include하는 것(금지: api는 pimpl 또는 인터페이스+팩토리), **모듈 그래프의 순환**(CMake는 정적 라이브러리 순환을 허용하므로 `cmake --graphviz` 출력으로 검사), QML `import` 방향.

### 1.1 첫째 원칙 — 부분만 보고 고칠 수 있다 (R-N1~N7)

| # | 원칙 | 코드에서의 뜻 |
|---|---|---|
| 1 | 모듈 = 폴더 하나 | 밖에서 쓰는 것은 `api/`에만. 내부는 `internal/`(기능 모듈은 `ui/`도) |
| 2 | 모듈마다 자기 설명서 | 폴더 안 `MODULE.md`(§12). `api/`나 모듈 간 의존이 바뀐 커밋에서 함께 고친다(R-N2를 이렇게 좁힘 — §17-9) |
| 3 | 공유 상태는 주인 하나 | 묶음마다 주인 하나(§5 주인 표). 나머지는 읽기만 |
| 4 | 펌웨어 차이는 한 계층 | `platform/vehicle/firmware/`가 **데이터와 순수 함수**로 내놓고(§8), 실행은 기능 모듈. 전송 방식별 절차는 기능 모듈의 `internal/paths/<방식>/`(§8.1) |
| 5 | 크기 상한 | 파일 **400줄**, 함수 **60줄**, QML **300줄**(물리 줄). 제3자·생성 코드 제외 |
| 6 | 의존은 한 방향 | 바탕: app < comm < vehicle < ui. 기능: 순위표(§3.2) 아래로만. 기능 안: `ui/` → `internal/` → `api/`. 거꾸로 알릴 때는 Qt 시그널 또는 **기여 등록**(§9.8) |
| 7 | 이름은 용어집에서 | §13의 낱말만 |

### 1.2 요구사항 층과의 대응

| 요구사항 R-N6 | 이 문서 |
|---|---|
| 0층 앱 기반 | `platform/app` |
| 1층 통신 | `platform/comm` |
| 2층 기체 모델 | `platform/vehicle` |
| (화면 바탕) | `platform/ui` — 요구사항에 없던 층. 테마·공용 부품·패널 등록부·슬롯 |
| 3층 기체 서비스 | 각 기능 모듈의 `internal/` |
| 4층 화면 | 각 기능 모듈의 `ui/` + `features/shell` |

## 2. 기술 기반 (D3·D4·R-N18, §17-1~2 확정)

- 언어 **C++23**(`std::expected`). GCC 13+, MSVC 2022 17.8+(CMake가 `/std:c++latest`로 넘긴다).
- Qt 6.12 LTS. 층별로 쓸 수 있는 Qt 모듈: `app` Core·Network·Sql·Positioning·TextToSpeech / `comm` + SerialPort / `vehicle` 추가 없음 / `platform/ui` + Quick·Qml·QuickControls / 기능 모듈 + Location·Multimedia·Graphs·Quick3D(필요한 기능만). GPL-3.0이라 모듈 제한 없음.
- 빌드 CMake ≥ 3.28 + Ninja, `CMakePresets.json`. Qt는 aqtinstall 패치 고정, C/C++ 의존은 CPM(태그·체크섬 고정), vcpkg 안 씀. 제3자 코드는 CPM 캐시, 모든 검사에서 제외.
- MAVLink: `mavlink/c_library_v2` CPM 커밋 고정, 방언 **ardupilotmega**(common 포함). include는 `<kgc/comm/Mavlink.h>` 한 곳을 통해서만(설정 매크로 뒤 include). 허용: `comm`·`vehicle`·기능 `internal/`. 금지: `app`, `platform/ui`, 모든 `ui/`. PX4 이벤트 해석은 libevents(CPM).
- QML 모듈: 바탕 `Kgc.Ui`, 기능마다 `Kgc.<Id>`(`qt_add_qml_module`, STATIC). `src/main`이 `Q_IMPORT_QML_PLUGIN`으로 전부 링크하고 `Composition`이 기능마다 `registerUi(registry)`를 **직접 호출**한다(정적 초기화 객체에 기대지 않는다 — 링커가 버린다).
- 네임스페이스 `kgc::<모듈>` (`kgc::app`, `kgc::esc`, `kgc::tracker`…).
- 버전은 `project(VERSION)` 한 곳. `0.0.1`부터.

## 3. 폴더

```
k-groundcontrol/
├─ CMakeLists.txt  CMakePresets.json  cmake/(CPM, deploy, notices/extra.json)
├─ src/
│  ├─ platform/
│  │  ├─ app/       0   boot settings secrets paths store scheduler job net log errors notify i18n units geo crc util
│  │  ├─ comm/      1   links(serial udp tcp mock) rawport(RawPort·PortRegistry) lease(PortLease) mavlink(parser channel signing) tlog joystick
│  │  ├─ vehicle/   2   registry Vehicle states firmware(px4 ardupilot generic) outbound(VehicleSender) safety(ArmGuard WorkLock ArmedPolicy RebootWindow) command(CommandQueue)
│  │  └─ ui/        2.5 Theme components(HoldToConfirm JobView Pill …) PanelRegistry Slots(Topbar StatusChip RightTab MapLayer) Navigator
│  ├─ features/<id>/   api/kgc/<id>/  internal/  ui/  CMakeLists.txt  MODULE.md
│  └─ main/            main.cpp Composition.cpp
├─ tracker/            ESP32 스케치 + protocol/ 공용 헤더 (§11)
├─ firmware/tracker/<변종>/   CI가 넣는 트래커 이미지
├─ i18n/  resources/(qrc icons silhouettes data/<이름>/)  tools/check/(+hooks/)  tests/  docs/  specs/
├─ .github/workflows/  AGENTS.md  LICENSE  THIRD_PARTY_NOTICES.md  README.md
└─ .pm/   (gitignore)
```

바탕 모듈의 하위 영역(`comm/mavlink`, `vehicle/firmware` 등)은 **각각 타깃 하나·`api/` 하나·`MODULE.md` 하나**이고, 같은 층 안의 순서는 각 바탕 모듈 `MODULE.md`가 적는다.

### 3.1 기능 모듈 표 — id · 수명 · 자리

| id | 한국어 | 수명 | 어디에 보이나 | 요구 |
|---|---|---|---|---|
| `shell` | 틀 | 전역 | 메인 창(1920×900), 상단·좌·우·하단 바의 **틀**과 슬롯 배치만 | R-N24, UI 그림 |
| `map` | 지도 | 전역 | 가운데 바탕, 레이어 등록 api | B10, F-48~F-58 |
| `fly` | 비행 조작 | 기체별 | 상단바 조작 띠(슬롯 기여), 유도 명령(지도 클릭), 연결 팝업(하단 기체 칩) | R-Q4·R-Q5 |
| `vehicleInfo` | 기체 정보 | 기체별 | 우 사이드바 계기 + 개요·상세·메시지 탭 | B2~B5, F-9 |
| `preflight` | 비행 전 점검 | 기체별 | 우 사이드바 점검 탭 | F-46·47 |
| `mission` | 미션 | 기체별 | 지도 패널(편집), 상단바 미션 선택·시작 슬롯 | R-Q7·R-Q8 |
| `fcFlash` | 펌웨어 세팅 | 전역 | 좌 메뉴 1 | R-Q10 |
| `layout` | 기체 구성도(+기체 종류) | 기체별 | 좌 메뉴 2 | 5.5절, S-34~37 |
| `sensors` | 센서 보정 | 기체별 | 좌 메뉴 3 | S-38~48 |
| `radio` | 조종기(+조이스틱 설정) | 기체별 | 좌 메뉴 4 | S-49~52 |
| `flightModes` | 비행 모드 | 기체별 | 좌 메뉴 5 | S-53·54 |
| `power` | 배터리·전원 | 기체별 | 좌 메뉴 6 | S-55~57 |
| `failsafe` | 안전·페일세이프 | 기체별 | 좌 메뉴 7 | S-62~64 |
| `motors` | 모터 시험 | 기체별 | 좌 메뉴 8 | S-58·59 |
| `tuning` | 튜닝 | 기체별 | 좌 메뉴 9 | S-65~68 |
| `logs` | 로그 분석(+tlog 재생, 우 사이드바 로그 탭) | 전역(내려받기는 기체별) | 좌 메뉴 10, 우 탭 | E1·E2, C-12 |
| `esc` | ESC 설정 | 전역 | 좌 메뉴 11 | 5.2절 |
| `elrs` | ELRS 설정 | 전역 | 좌 메뉴 12 | 5.3절 |
| `tracker` | 안테나 트래커 | 전역 | 좌 메뉴 13, 하단 칩 | 5.4절 |
| `params` | 수동 기체 설정 | 기체별 | 좌 메뉴 14 | R-D1·D2, S-10~14 |
| `appSettings` | 앱 설정(링크 목록·언어·단위·AI 제공자·**정보 창**·고지) | 전역 | 좌 메뉴 15, 정보 창 | R-N16·17·22 |
| `ai` | AI 세팅(+첫 사용 동의) | 기체별 | 상단 오른쪽 패널 | R-D4~D8, R-N23 |
| `notify` | 알림 화면 | 전역 | 상단 벨·토스트·TTS | §10 |
| `terrain` | 지형 | 전역(요청 응답은 기체별) | — | C6, F-56 |
| `gcsposition` | GCS GPS | 전역 | 하단 칩 | R-Q3, C-47 |
| `video` | 영상 | 전역 | 지도 PiP(map 레이어) | B11 |
| `analyze` | MAVLink 인스펙터·콘솔·앱 로그 뷰어·진동 | 기체별 | 좌 메뉴(나중 순서) | E3, C-55·57 |
| `extras` | GeoTag·ADS-B·Remote ID·Flight Review·RTK/NTRIP·포워딩·서명·카메라·짐벌·사용자 동작·SiK | — | 기능 스펙 때 **각각 모듈로** 쪼갬 | E5, R-Q3·Q6·Q10 |

- **기체별**: Vehicle이 생길 때 Composition이 만들고 부모를 Vehicle로. 다른 모듈은 `XService::of(Vehicle*)`로 얻는다(api의 정적 조회, 내부는 Vehicle의 자식 검색). 컨트롤러는 `VehicleRegistry::activeChanged`에 **다시 묶는다**.
- **전역**: 앱 수명. `Composition`이 하나 만든다.

### 3.2 기능 순위표 (의존은 위에서 아래로만)

```
shell
  ├─ notify  ai  appSettings  analyze
  ├─ fly  mission  vehicleInfo  preflight  logs
  ├─ layout → motors sensors radio flightModes power failsafe tuning params
  ├─ fcFlash  esc  elrs  tracker
  ├─ map → video  terrain  gcsposition
  └─ (바탕) platform/ui < vehicle < comm < app
```
- 화살표 왼쪽이 오른쪽의 `api/`를 쓴다. 같은 줄의 형제끼리는 **쓰지 않는다**(쓸 일이 생기면 §9.8 기여 등록·Navigator·states로).
- `layout`이 `motors` 등의 api를 쓰는 것(표식 → 그 설정 페이지)과 `motors`가 구성도를 읽는 것(R-D35 양방향)은 이렇게 푼다: 구성도 **데이터**(`LayoutStates`)는 `layout`이 내놓고, `motors`는 `Navigator::open("layout", marker)`로 열기만 한다. 아래가 위를 부르지 않는다.
- 순환은 `tools/check`가 `cmake --graphviz`로 잡는다.

## 4. 설정 — 세 가지를 섞지 않는다 (§17-5 확정)

| 종류 | 뜻 | 자리 | 바꾸는 사람 | 저장 |
|---|---|---|---|---|
| **부팅 설정** | 뜨기 전에 정해지는 것 | `platform/app/boot/BootOptions`: 명령줄 + `kgc.ini`(`QStandardPaths::AppConfigLocation`) | 개발자·고급 사용자 | ini |
| **운영 설정** | 쓰면서 바꾸는 값 | 정의는 각 모듈 `internal/settings/<id>.settings.json`; 저장·검색은 `platform/app/settings/Settings`(QSettings) | 사용자 | QSettings(플랫폼 기본) |
| **고정값** | 바꾸면 안 되는 수 | 모듈 `internal/Constants.h`; 여러 모듈이 쓰는 값만 `platform/app/constants/` | 코드 | 코드 |

- 운영 설정은 **메타데이터 먼저**(R-N17). 항목: `id`(`<모듈>.<이름>`), `type`(`bool|int|double|string|enum|list<object>`), `default`, `min/max`, `enum`, `labelKey`, `category`(모듈 id). 빌드 때 생성기가 **모듈마다** `api/kgc/<id>/Settings.h`에 형이 있는 접근자를 만든다(`esc::settings::flashChunk()`). 없는 키는 컴파일 오류. 코드에 설정 키 문자열을 쓰지 않는다. 바탕 `app/settings`는 모듈을 모르고 등록부만 갖는다.
- **다른 모듈의 설정을 읽을 때**는 그 모듈 `api/`의 접근자를 쓴다(순위표 방향으로만). 여러 기능이 같이 쓰는 안전 설정(길게 누르기 시간, 유도 거리·고도 한계)은 `platform/ui`(길게 누르기)와 `platform/vehicle/safety`(유도 한계) 설정으로 둔다.
- 설정 기본값은 JSON에만.
- **비밀값**: `platform/app/secrets/SecretStore`(QtKeychain, §17-4). `Secret` 형으로만(QString 암시 변환·`QDebug <<` 없음, `reveal()`은 허용 파일만).
- **파일 자리**(`platform/app/paths/Paths`, 데이터 폴더 아래): `logs/app`, `logs/tlog`, `logs/onboard`, `missions`, `layouts`, `params/backups`, `params/cache`(PX4 해시 캐시), `ai/changes`, `video`, `cache/firmware`, `cache/tiles`, `cache/metadata`, `cache/terrain`.

## 5. 상태(states) — 묶음마다 주인 하나

- **바탕 묶음**(기체 자체의 값)은 `platform/vehicle/states/VehicleStates`(QObject, 기체마다 하나)가 `Q_GADGET` 값 구조체로 `Q_PROPERTY … NOTIFY <bundle>Changed` 한다. 주인은 `VehicleStateOwner`. 해석은 `internal/updaters/<Bundle>Updater` 순수 함수. QML은 활성 기체의 VehicleStates에 직접 바인딩해도 된다.
- **기능 묶음**(기능이 만드는 값)은 그 기능의 **기체별 서비스가 `api/`로 내놓는다**. 바탕은 기능 묶음을 모른다.
- **큰 목록**(파라미터·메시지·ESC 배열)은 값 묶음이 아니라 **`QAbstractListModel`**로 행 단위 변경을 알린다.

| 묶음 | 담는 것 | 주인 | 모양 |
|---|---|---|---|
| `Link` `Arming` `Attitude` `Position` `Gps` `Battery[]` `Sensors` `Rc` `Flight` | 초안 1과 같음(Arming에 **기체 쪽** 시동 불가 사유) | `VehicleStateOwner` | Q_GADGET |
| `Messages` | STATUSTEXT·이벤트 목록 | `VehicleStateOwner` | ListModel |
| `Esc[]` | rpm·전압·전류·온도 | `VehicleStateOwner` | ListModel |
| `Capabilities` | 기능 비트마스크, 버전·UID, 지원 범위 판정 | `VehicleStateOwner` — 단계별: HEARTBEAT → AUTOPILOT_VERSION → `reportParamsReady()` → `reportProbe(cap, ok)`(기능이 알림) | Q_GADGET |
| `ArmBlockers` | **GCS 쪽** 시동 불가 사유 | `vehicle/safety/ArmGuard` — `block(reason) → 토큰`, `release(토큰)` | ListModel |
| `Params` | 값 전체·내려받기 진행 | `features/params` `ParamService`(기체별) — 값을 드는 유일한 곳 | ListModel |
| `MissionSync` | 기체 미션 해시·동기·현재 항목 | `features/mission` `MissionService`(기체별) | Q_GADGET |
| `LayoutStates` | 구성도 표식·기체값 차이 | `features/layout`(기체별) | ListModel |
| `TrackerStates` `GcsPositionStates` | 트래커·GCS GPS | 각 전역 서비스 | Q_GADGET |

- 스레드: 링크 워커 → `LinkManager::messageReceived(linkId, msg)` 큐드 시그널 → GUI 스레드의 Owner. 묶음은 `Q_DECLARE_METATYPE`.

## 6. 고정값과 정해진 값의 이름

- 모듈 고정값 `internal/Constants.h`; 공유 값만 `platform/app/constants/`(`mavlink.h` ACK 1200ms·3회·하트비트 1Hz·끊김 3500ms, `safety.h` 21196·2989·3초, `mission.h` 1500·250ms·5회). CRC 다항식은 `app/crc`, 트래커 상수는 공용 헤더(§11).
- 검사 범위: `chrono` 리터럴, 타이머·Timeout 인자, 4자리 이상 16진수, 3자리 이상 10진수(허용 360·180·90·1000·60). 오프셋·시프트·인덱스(<256)는 대상 아님. QML에서는 `width`·`height`·`padding`·`margins`·`spacing`·`radius`·색 리터럴만 `Theme`에 있어야 한다(anchors·z·opacity 등은 대상 아님).
- 열거는 모듈 `api/`. 펌웨어가 정하는 열거는 플러그인이 메타데이터에서.
- **오류 코드**: `platform/app/errors/ErrorCode = {ModuleId, uint16 code}` 하나의 형. 각 모듈은 `api/kgc/<id>/Errors.h`에 자기 열거(`enum class EscError : uint16_t`)와 `ModuleId`를 둔다. `Result<T> = std::expected<T, ErrorCode>`. `Job`은 `errorCode`·`errorText` 프로퍼티로 QML에 낸다.
- **로그 사건**: `platform/app/log/LogEvent = {ModuleId, uint16}`; 모듈 열거는 `internal/Events.h`. 검사가 모아 중복·미등록을 본다.

## 7. 자료 규격(파일 형식)과 원본 데이터

(초안 2 표 유지) + 앱 로그 형식(`specs/formats/applog.md`: 한 줄 = 시각·수준·카테고리·사건·메시지), 지형 캐시, PX4 해시 캐시(`params/cache`). 규격 문서는 기능 스펙과 함께, 읽기·쓰기 함수는 그 모듈 하나에만. 펌웨어별 데이터는 플러그인 아래 `data/`.

## 8. 펌웨어 플러그인 계층 (R-Q12·R-N4) — 데이터와 순수 함수만

- 자리 `platform/vehicle/firmware/`(`api/kgc/vehicle/FirmwarePlugin.h`, `internal/{px4,ardupilot,generic}/`).
- **선택**: **compid 1이고 `autopilot ≠ MAV_AUTOPILOT_INVALID`인 첫 HEARTBEAT**로 `FirmwarePluginFactory`가 고른다(짐벌·컴패니언 하트비트는 거른다). 바꾸지 않는다; 값이 바뀌면 기체를 새로 만든다.
- 내놓는 것(초안 2와 같음): 비트마스크 초기값·보강 규칙, 모드 표, `CommandStep` 목록, 시동 불가 사유 해석 함수, 메타데이터 출처·이름 바꿈표, 미션 번호 규칙, 보정 방식 기술자, 구성도 대응표, AI 거부 목록, 지원 버전 범위, `AltitudeFrame`. **실행하지 않는다.**
- 검사: `vehicle/firmware/` 밖에서 `MAV_AUTOPILOT_PX4`·`MAV_AUTOPILOT_ARDUPILOTMEGA` 비교, `"PX4"`·`"ArduPilot"`·`"APM"` 문자열 비교를 잡되, **허용 목록**: `features/fcFlash`(펌웨어 선택 화면), `features/logs`(.ulg/.bin), `features/params`(메타데이터 출처 표시), 번역 파일.

### 8.1 전송 방식별 절차의 자리
- ESC·ELRS처럼 **펌웨어가 아니라 전송 방식**에 따라 절차가 다른 것은 기능 모듈 `internal/paths/<방식>/`에 둔다: `esc/internal/paths/{blhPassthrough, serialControl, direct}`, `elrs/internal/paths/{uart, wifi, bfPassthrough, etxPassthrough, fcPassthrough}`. 어느 방식을 쓸지는 `Capabilities`와 사용자 선택으로 고른다. 폴더 이름은 전송 방식이지 펌웨어 이름이 아니다.

## 9. 조립·수명·송신·공통 틀

### 9.1 조립과 수명
- `src/main/Composition`이 모든 `api/`를 include해 조립한다. `main.cpp`: BootOptions → Composition → Shell 창.
- `comm/links/LinkManager`(링크), `vehicle/registry/VehicleRegistry`(기체, sysid 키; compid 1이 자동조종기, 다른 compid는 그 기체의 `Component`). **활성 기체** `VehicleRegistry::active` 하나.
- 기체별 모듈은 §3.1 표대로 Vehicle 자식; 전역 모듈은 앱 수명. 기체 삭제: `vehicleRemoved` → `deleteLater`. 참조는 `QPointer`/활성 기체 프로퍼티.
- **포트 소유 등록부** `comm/rawport/PortRegistry`: 모든 시리얼 포트의 현재 소유자(MAVLink 링크 / `RawPort`(트래커·GCS GPS처럼 오래 쥐는 비-MAVLink 포트) / `PortLease`(굽기)). USB 자동 연결은 등록부에 있는 포트를 건드리지 않는다.
- **의도한 재부팅 구간** `vehicle/safety/RebootWindow`: 기능이 `begin(vehicle, timeout)`으로 열면 그동안 끊김 판정이 "끊김"이 아니라 "재부팅 중"이 된다(R-Q1a (나)).

### 9.2 기체로 나가는 문 하나
- `vehicle/outbound/VehicleSender`가 기체로 가는 **모든 MAVLink 송신**의 유일한 문. 송신마다 `OutboundKind`(Arm, Motion, Command, Param, Mission, Other).
- **시동 중 규칙(R-N10)은 두 곳에서 판정**: ① 작업을 시작할 때 — 호출자가 `OperationKind`(ParamSingle, ParamBulk, AiWrite, LayoutWrite, Tuning, Autotune, SetupPage, FcFlash, EscConfig, ElrsConfig, TrackerFlash, MotorTest…)를 밝히고 `vehicle/safety/ArmedPolicy::check(kind, vehicle)`로 허용·경고·거부를 받는다(표는 R-N10을 그대로 옮긴 것). ② Sender에서 — `OutboundKind` 수준의 마지막 방어(시동 중 Arm·Motion 외 금지된 종류). 원시 포트 작업(굽기)은 Sender를 지나지 않으므로 ①만 거치며, **트래커 굽기는 연결된 기체 중 하나라도 시동 중이면 거부**.
- `WorkLock`(R-Q1a (나)): 토큰을 쥔 작업의 송신만 통과, 그 작업 자신의 단계는 예외.
- `vehicle/command/CommandQueue`는 Sender 위의 재시도 계층. 보정 시작은 `{retries=0, timeout=설정값}` 모드. 예외 목록 없음.
- 보내기 스레드: `QMetaObject::invokeMethod(link, …, Qt::QueuedConnection)`.
- 검사: 링크 쓰기 API는 `comm/`·`vehicle/outbound/` 밖 금지; `command_(long|int)_(pack|encode)`는 `vehicle/command/` 밖 금지.

### 9.3 바깥과 닿는 자리 (초안 2 표 유지 + RawPort)
HTTP → `app/net/Http`; 환경 변수 → `app/boot`; 시리얼 → `comm/links/serial`·`comm/rawport`; 조이스틱 → `comm/joystick`; SQL → `app/store`; TTS → `app/notify/Speech`.

### 9.4 오래 걸리는 작업과 오류
`platform/app/job/Job`(초안 2와 같음) + `OperationKind`를 Job이 갖는다(§9.2 ①). `Result<T>` §6. 워커 오류는 큐드 시그널로 소유 객체에.

### 9.5 주기 작업과 타임아웃
`Scheduler`에 이름 있는 반복 작업만(GCS 하트비트 1Hz, 끊김 검사 1Hz, 포트 목록 1Hz, tlog 플러시). 트래커 각도 송신은 반복 작업이 **아니라** 위치 수신에 따라(최대 5Hz 제한은 `Timeout`으로). 일회성은 `Timeout`. QML `Timer`는 `platform/ui/components/`만. 검사 대상 `QTimer`·`singleShot`·`startTimer(`·`QBasicTimer`·QML `Timer {`.

### 9.6 공통 함수
`app/geo/Altitude`(frame 인자)·`Geodesy`, `app/units`, `app/crc`, `app/util`. 자유 함수 이름 중복 검사.

### 9.7 화면 바탕 `platform/ui` (QML 모듈 `Kgc.Ui`)
- `Theme.qml`(크기·색·글꼴 토큰), `components/`(HoldToConfirm, JobView, Pill, Chip, Timer 래퍼), `PanelRegistry`(좌 메뉴 패널: id·순서·QML 컴포넌트), `Slots`(기여 지점: `TopbarSlot`, `StatusChipSlot`(하단), `RightTabSlot`, `MapLayerSlot`), `Navigator`(`open(panelId, arg)` — 아래 모듈이 위 모듈 화면을 **이름으로** 연다).
- 기능 모듈의 `registerUi(registry)`가 패널·슬롯 기여·레이어를 등록한다. `shell`은 **잎 모듈**: 틀을 그리고 등록부를 읽어 배치만 한다.
- 컨트롤러(`<Id>Controller`)는 상태를 표시값으로 옮기고 서비스를 호출만. 판정은 서비스·ArmedPolicy.
- QML `import` 검사: 기능 QML은 `QtQuick*`·`Kgc.Ui`·자기 `Kgc.<Id>`만 import.

### 9.8 기여 등록(아래가 위로 보이는 법)
| 기여 | 누가 | 어디에 |
|---|---|---|
| 상단바 조작 띠·미션 슬롯 | fly, mission | `TopbarSlot` |
| 하단 상태 칩 | fly(기체 링크·센서), tracker, gcsposition | `StatusChipSlot` |
| 우 사이드바 탭 | vehicleInfo(개요·상세·메시지), logs(로그), preflight(점검) | `RightTabSlot` |
| 지도 레이어 | mission, tracker(부채꼴), terrain(단면), video(PiP), fly(유도 표식) | `MapLayerSlot`(map api) |
| 좌 메뉴 패널 | §3.1 좌 메뉴 모듈 | `PanelRegistry` |

## 10. 로그·오류·알림·번역 (§17-6 확정)
초안 2와 같음(`KGC_LOG` 하나, 한국어 전보체, `LogSink` 회전·마스킹, `comm/mavlink/signing` 필터, `Notify` 한 입구 — 승격 기준은 B4 스펙, 원문 한국어·`kgc_en.ts`, `Translator.retranslate()`). QML `console.log`도 검사 대상(금지, `KGC_LOG` 바인딩 사용).

## 11. 트래커 스케치 `tracker/` (R-D27)
초안 2와 같음(C99 헤더 전용 `kgc_tracker_protocol.h`, static inline LE 직렬화, CRC·상수는 헤더에만, src 밖 include 허용은 `features/tracker/internal/protocol/`뿐, 장치 설정 주인은 ESP32 NVS, `tracker.yml` → `firmware/tracker/<변종>/`). 평상시 연결은 `comm/rawport/RawPort`로 쥔다.

## 12. 모듈 설명서 `MODULE.md` 틀
초안 2와 같음(역할·받는 것·주는 것·의존·안 하는 것·흐름, 100줄 이하, `api/`·의존 변경 커밋에 함께). **수명(기체별/전역)과 기여(슬롯·패널·레이어)** 줄을 더한다.

## 13. 용어집
초안 2 표 + `platform/ui`(화면 바탕), `Slot`(기여 지점), `Navigator`(이름으로 열기), `RawPort`(오래 쥐는 비-MAVLink 포트), `PortRegistry`(포트 소유 등록부), `RebootWindow`(의도한 재부팅 구간), `OperationKind`(작업 종류 — 시동 중 규칙 판정용), `OutboundKind`(송신 종류), `ListModel`(큰 목록 상태), `fcFlash`(펌웨어 세팅 모듈), `failsafe`(안전·페일세이프 모듈).

## 14. 자동 검사 `tools/check/` (R-N21, §17-7 확정)
초안 2 표 + 추가: **모듈 그래프 순환**(`cmake --graphviz`, 0.0.1), **상대 경로 `..` include**(0.0.1), **api→internal include**(0.0.1), **QML import 방향**(0.0.1), **QML `console.log`**(0.1.x), 펌웨어 문자열 검사 허용 목록(§8). 훅 `core.hooksPath=tools/check/hooks`, CI는 `before..after` 커밋마다, 함수 길이 lizard 고정.

## 15. 빌드·CI·배포
초안 2와 같음(`check.yml`·`linux.yml`·`windows.yml`·`tracker.yml`, 고지 = CPM 목록 + `cmake/notices/extra.json`, 0.0.1 산출물 zip+AppImage, 정보 창은 `appSettings`가 그린다).

## 16. 이 문서가 정하지 않는 것
각 기능의 동작, 화면의 모양, 바이트 명세, 테스트 내용, 알림 승격 기준(B4), 트래커 빌드 도구(트래커 스펙), 성능 수치, `extras`의 모듈 분할(각 기능 스펙).

## 17. 승인 사항

| # | 물음 | 상태 |
|---|---|---|
| 1 | C++23 + `std::expected` | **확정** 2026-10-24 |
| 2 | MAVLink 방언 ardupilotmega(c_library_v2 CPM) | **확정** |
| 3 | 테스트 자리: 프로토콜 시험 벡터·순수 함수 단위 테스트만 `tests/`; 기능 확인 테스트는 `.pm/tests/`(저장소·CI 밖에 남음) | **확정** |
| 4 | 비밀값 QtKeychain | **확정** |
| 5 | 운영 설정 QSettings | **확정** |
| 6 | 번역 원문 한국어 | **확정** |
| 7 | 검사용 Python 3 + lizard 고정 | **확정** |
| 8 | 폴더·네임스페이스·크기 상한 400/60/300 | **확정** |
| **9** | **요구사항 개정**: R-N6을 "모듈러 모놀리스(§1.2 대응표)"로, R-N2를 "api·의존 변경 커밋에서 함께"로 좁힘. 기능 간 의존은 **순위표(DAG)**로 허용하고 역방향은 기여 등록·Navigator·states로 | **승인 대기** |
| **10** | 기능 id 변경: `firmware`→`fcFlash`, `safety`→`failsafe`; 와이어프레임 id도 맞춤 | **승인 대기** |

## 18. 검토·승인 기록
| 날짜 | 누가 | 무엇 |
|---|---|---|
| 2026-10-24 | reviewer-1 | 설계 검토 1: fail, 지적 20개(`.pm/base/reviewer-1.md`) → 초안 2 |
| 2026-10-24 | 사용자 | §17 1~8 PM 제안값으로 확정 |
| 2026-10-24 | reviewer-2 | 설계 검토 2: fail, 이전 15 해결·5 부분, 새 지적 12개(`.pm/base/reviewer-2.md`) → 초안 3(platform/ui 신설, 순위표, 기여 등록, OperationKind, 기능 묶음 분리, ErrorCode 형, include 형식, paths/, 모듈 표 확장) |
| | reviewer-3 | (대기) |
| | 사용자 | (대기) |
