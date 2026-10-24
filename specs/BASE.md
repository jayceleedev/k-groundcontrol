# K-GroundControl 기반 문서 (BASE)

- 상태: **초안 5 (검토 1~4 반영, 승인 대기)** — 2026-10-24, PM 작성
- 이 문서가 정하는 것: **무엇을 어떻게 구분해서 어디에 두는가.** 모듈의 모양과 경계, 설정·상태·고정값·공통 함수·저장소·주기 작업·서비스·입출력의 자리, 로그·오류·알림의 규칙, 그것을 지키는 자동 검사. 기능이 무엇을 하는지는 `specs/REQUIREMENTS.md`와 `specs/features/`가 정한다.
- 이 문서가 승인되기 전에는 기능 스펙을 쓰지 않는다. 기능 스펙의 "기반" 절은 이 문서의 이름을 그대로 쓴다. 구현 중에 여기 없는 자리가 필요해지면 만들지 않고 이 문서로 돌아온다.
- **문서의 권위**: BASE가 원본이고, 저장소 루트 `AGENTS.md`(구현자 GPT용, 영어)는 그 번역이다. 어긋나면 BASE를 따른다. 구현 주체가 GPT(D12)이므로 이 문서만 읽고도 같은 구조를 세울 수 있게 **다른 판이나 문서를 참조하지 않고** 쓴다.
- **요구사항과의 관계**: 요구사항 R-N6의 "층 0~4"를 모듈러 모놀리스로 구체화한다(§1.2). R-N6·R-N2의 문구 개정은 승인 사항 §17-9.

## 1. 구조의 이름 — 모듈러 모놀리스

실행 파일은 하나(K-GroundControl)이고, 안은 **경계가 분명한 모듈**로 나뉘며, 모듈끼리는 **공개 인터페이스(`api/`)로만** 말한다.

| 종류 | 뜻 | 자리 | 의존 |
|---|---|---|---|
| **바탕 모듈(platform)** | 모든 기능이 쓰는 바탕. 기능을 모른다. 넷이 층을 이룬다: `app`(0) < `comm`(1) < `vehicle`(2) < `ui`(2.5) | `src/platform/<이름>/` | 아래층만 |
| **기능 모듈(feature)** | 기능 하나 = 모듈 하나. 로직(`internal/`)과 화면(`ui/`)을 함께 갖는다 | `src/features/<id>/` | 바탕 넷 + 순위표(§3.2)에서 **더 아래 순위**인 기능의 `api/` |
| **조립(main)** | 모듈을 하나로 묶는 얇은 껍데기. 로직 없음 | `src/main/` | 전부 |

**경계는 두 겹으로 지킨다.**
- **컴파일러**: 모듈마다 CMake 정적 라이브러리 타깃 하나. `target_include_directories`에 **`api/`만 PUBLIC**, `internal/`·`ui/`는 PRIVATE. include 형식은 **`<kgc/<모듈 경로>/X.h>`**(`api/kgc/<모듈 경로>/`에 둔다). 바탕 하위 영역은 부모 경로를 잇는다: `<kgc/vehicle/firmware/FirmwarePlugin.h>`, `<kgc/comm/mavlink/Mavlink.h>`. 다른 모듈의 `internal/`은 꺾쇠 include로는 닿지 않는다.
- **검사(§14)**: 컴파일러가 못 보는 것 — `..`가 든 상대 경로 include(금지), `api/` 헤더가 `internal/`을 include하는 것(금지: api는 pimpl 또는 인터페이스+팩토리), **모듈 그래프의 순환**(CMake는 정적 라이브러리 순환을 허용하므로 `cmake --graphviz` 출력으로 순위표와 대조), QML `import` 방향.

### 1.1 첫째 원칙 — 부분만 보고 고칠 수 있다 (R-N1~N7)

| # | 원칙 | 코드에서의 뜻 |
|---|---|---|
| 1 | 모듈 = 폴더 하나 | 밖에서 쓰는 것은 `api/`에만. 내부는 `internal/`(기능 모듈은 `ui/`도) |
| 2 | 모듈마다 자기 설명서 | 폴더 안 `MODULE.md`(§12). `api/`나 모듈 간 의존이 바뀐 커밋에서 함께 고친다(R-N2를 이렇게 좁힘 — §17-9) |
| 3 | 공유 상태는 주인 하나 | 묶음마다 주인 하나(§5). 나머지는 읽기만 |
| 4 | 펌웨어 차이는 한 계층 | `platform/vehicle/firmware/`가 **데이터와 순수 함수**로 내놓고(§8), 실행은 기능 모듈. 전송 방식별 절차는 기능 모듈의 `internal/paths/<방식>/`(§8.1) |
| 5 | 크기 상한 | 파일 **400줄**, 함수 **60줄**, QML **300줄**(물리 줄, 빈 줄·주석 포함). 제3자·생성 코드 제외 |
| 6 | 의존은 한 방향 | 바탕: app < comm < vehicle < ui. 기능: 순위표 아래로만(줄을 건너뛰어 아래로 가는 것은 허용, 같은 순위끼리는 금지). 기능 안: `ui/` → `internal/` → `api/`. 아래가 위에 알릴 때는 Qt 시그널·상태·**기여 등록**(§9.8)·`Navigator`(이름으로 열기) |
| 7 | 이름은 용어집에서 | §13의 낱말만. 새 낱말은 용어집에 먼저 |

### 1.2 요구사항 층과의 대응

| 요구사항 R-N6 | 이 문서 |
|---|---|
| 0층 앱 기반 | `platform/app` |
| 1층 통신 | `platform/comm` |
| 2층 기체 모델 | `platform/vehicle` |
| (요구사항에 없던 층) 화면 바탕 | `platform/ui` — 테마·공용 부품·패널 등록부·슬롯·Navigator |
| 3층 기체 서비스 | 각 기능 모듈의 `internal/` |
| 4층 화면 | 각 기능 모듈의 `ui/` + `features/shell` |

## 2. 기술 기반 (D3·D4·R-N18; §17-1~2 확정)

- 언어 **C++23**(`std::expected`). GCC 13+, MSVC 2022 17.8+(CMake가 `/std:c++latest`로 넘긴다).
- Qt 6.12 LTS. 층별로 쓸 수 있는 Qt 모듈: `app` Core·Network·Sql·Positioning·TextToSpeech / `comm` + SerialPort / `vehicle` 추가 없음 / `platform/ui` + Quick·Qml·QuickControls / 기능 모듈 + Location·Multimedia·Graphs·Quick3D(필요한 기능만). GPL-3.0이라 모듈 제한 없음.
- 빌드 CMake ≥ 3.28 + Ninja, `CMakePresets.json`(linux-debug·linux-release·windows-release). Qt는 aqtinstall 패치 버전 고정, C/C++ 의존은 CPM(태그·체크섬 고정), vcpkg 안 씀. 제3자 코드는 CPM 캐시에 두고 모든 검사에서 제외.
- MAVLink: `mavlink/c_library_v2` CPM 커밋 고정, 방언 **ardupilotmega**(common 포함). include는 `<kgc/comm/mavlink/Mavlink.h>` **한 곳**을 통해서만(설정 매크로 뒤 include). 허용: `comm`·`vehicle`·기능 `internal/`. 금지: `app`, `platform/ui`, 모든 `ui/`. PX4 이벤트 해석은 libevents(CPM).
- QML 모듈: 바탕 `Kgc.Ui`, 기능마다 `Kgc.<Id>`(`qt_add_qml_module`, STATIC). `src/main`이 `Q_IMPORT_QML_PLUGIN`으로 전부 링크하고 `Composition`이 기능마다 `registerUi(registry)`를 **직접 호출**한다(정적 초기화 객체에 기대지 않는다 — 링커가 버린다).
- 네임스페이스는 모듈 경로를 따른다: `kgc::app`, `kgc::comm::mavlink`, `kgc::vehicle::firmware`, `kgc::esc`, `kgc::tracker`.
- 버전은 `CMakeLists.txt`의 `project(VERSION)` 한 곳. 정보 창·로그·산출물 이름이 읽는다. `0.0.1`부터.

## 3. 폴더

```
k-groundcontrol/
├─ CMakeLists.txt  CMakePresets.json  cmake/(CPM, deploy, notices/extra.json)
├─ src/
│  ├─ platform/
│  │  ├─ app/       0   boot settings secrets paths store scheduler job net log errors notify i18n units geo crc util
│  │  ├─ comm/      1   links(serial udp tcp mock) rawport(RawPort PortRegistry) lease(PortLease) mavlink(parser channel signing) espflash(ESP 시리얼 부트로더 클라이언트) tlog joystick
│  │  ├─ vehicle/   2   registry Vehicle states firmware(px4 ardupilot generic) outbound(VehicleSender) safety(ArmGuard WorkLock ArmedPolicy RebootWindow) command(CommandQueue)
│  │  └─ ui/        2.5 Theme components(HoldToConfirm JobView Pill Chip TimerBox) PanelRegistry Slots(Topbar StatusChip RightTab MapLayer) Navigator
│  ├─ features/<id>/   api/kgc/<id>/  internal/  ui/  CMakeLists.txt  MODULE.md
│  └─ main/            main.cpp Composition.cpp
├─ tracker/            ESP32 스케치 + protocol/ 공용 헤더 (§11)
├─ firmware/tracker/<변종>/   CI가 넣는 트래커 이미지
├─ i18n/  resources/(qrc icons silhouettes data/<이름>/)  tools/check/(+hooks/)  tests/  docs/  specs/
├─ .github/workflows/  AGENTS.md  LICENSE  THIRD_PARTY_NOTICES.md  README.md
└─ .pm/   (gitignore)
```

- 바탕 모듈의 하위 영역(`comm/mavlink`, `vehicle/firmware` 등)은 **각각 타깃 하나·`api/` 하나·`MODULE.md` 하나·네임스페이스 하나**. 같은 바탕 안의 순서는 그 바탕 모듈 `MODULE.md`가 적는다.
- 모듈 폴더 안: `api/kgc/<id>/`(공개 헤더 — `<Id>Service.h`, `<Id>States.h`, `Errors.h`, `ModuleId.h`, 생성 `Settings.h`), `internal/`(로직, `Constants.h`, `Events.h`, `settings/<id>.settings.json`, `paths/`), `ui/`(`<Id>Panel.qml`, `<Id>Controller.{h,cpp}`, `register.cpp`), `CMakeLists.txt`, `MODULE.md`. 바탕 모듈과 화면 없는 기능은 `ui/`가 없다.

### 3.1 기능 모듈 표 — id · 수명 · 보이는 곳

| id | 한국어 | 수명 | 보이는 곳 | 요구 |
|---|---|---|---|---|
| `shell` | 틀 | 전역 | 메인 창(최소 1920×900), 상단·좌·우·하단 바의 틀, 슬롯 배치. 앱 아이콘 → `Navigator.open("appSettings","about")` | R-N24 |
| `notify` | 알림 화면 | 전역 | 상단 벨·목록·토스트·TTS 호출 | §10 |
| `ai` | AI 세팅 | 전역 패널 + 기체별 서비스 | 상단 오른쪽 아이콘·패널, 첫 사용 동의 | R-D4~D8, R-N23 |
| `appSettings` | 앱 설정 | 전역 | 좌 메뉴 15, **정보 창**(버전·라이선스·고지·소스) | R-N16·17·22 |
| `analyze` | 인스펙터·콘솔·앱 로그 뷰어·진동 | 기체별 + 전역(앱 로그) | 좌 메뉴(나중 순서) | E3, C-55·57 |
| `link` | 연결 | 전역 | 하단 기체 칩(링크 상태) + 연결 팝업. 저장된 링크 목록의 편집 화면은 `appSettings`가 갖고, 둘 다 바탕 `comm/links/LinkManager` api를 직접 쓴다(같은 순위라 서로 부르지 않음) | R-Q1, C-1~C-8 |
| `fly` | 비행 조작 | 기체별 | 상단바 조작 띠(슬롯), 유도 명령(지도 클릭 팝업·표식 레이어) | R-Q4·R-Q5 |
| `mission` | 미션 | **전역 편집기**(`MissionEditor`: .plan 편집·저장, 기체 없이도) + **기체별** `MissionService`(전송·동기·현재 항목) | 지도 패널 편집 레이어, 상단바 미션 선택·시작 슬롯 | R-Q7·R-Q8, R-N13 |
| `vehicleInfo` | 기체 정보 | 기체별 | 우 사이드바 계기 + 개요·상세·메시지 탭, 하단 센서 ✓/✕·배터리·RSSI 칩 | B2~B5, F-7, F-9 |
| `preflight` | 비행 전 점검 | 기체별 | 우 사이드바 점검 탭; 점검표 정의 `resources/data/preflight/` | F-46·47 |
| `logs` | 로그 분석 | 전역 뷰어 + 기체별 내려받기 | 좌 메뉴 10, 우 사이드바 로그 탭, tlog 재생 | E1·E2, C-12 |
| `layout` | 기체 구성도(+기체 종류) | 기체별 | 좌 메뉴 2 | 5.5절, S-34~37 |
| `sensors` | 센서 보정 | 기체별 | 좌 메뉴 3 | S-38~48 |
| `radio` | 조종기(+조이스틱 설정) | 기체별 | 좌 메뉴 4 | S-49~52 |
| `flightModes` | 비행 모드 | 기체별 | 좌 메뉴 5 | S-53·54 |
| `power` | 배터리·전원 | 기체별 | 좌 메뉴 6 | S-55~57 |
| `failsafe` | 안전·페일세이프 | 기체별 | 좌 메뉴 7 | S-62~64 |
| `motors` | 모터 시험 | 기체별 | 좌 메뉴 8 | S-58·59 |
| `tuning` | 튜닝 | 기체별 | 좌 메뉴 9 | S-65~68 |
| `fcFlash` | 펌웨어 세팅 | 전역 | 좌 메뉴 1 | R-Q10 |
| `esc` | ESC 설정 | 전역 | 좌 메뉴 11 | 5.2절 |
| `elrs` | ELRS 설정 | 전역 | 좌 메뉴 12 | 5.3절 |
| `tracker` | 안테나 트래커 | 전역 | 좌 메뉴 13, 하단 칩, 지도 부채꼴 레이어 | 5.4절 |
| `map` | 지도 | 전역 | 가운데 바탕(타일·축척·기체 아이콘·궤적·홈), 레이어 슬롯의 그릇. 펜스·랠리·ADS-B는 그리지 않는다(기여 레이어) | B10 |
| `video` | 영상 | 전역 | 지도 PiP 레이어 | B11 |
| `terrain` | 지형 | 전역 조회 + 기체별 응답(TERRAIN_REQUEST) | 지도 단면 레이어 | C6, F-56 |
| `gcsposition` | GCS GPS | 전역 | 하단 칩 | R-Q3, C-47 |
| `params` | 수동 기체 설정(파라미터) | 기체별 | 좌 메뉴 14 | R-D1·D2, S-10~14 |
| `extras` | GeoTag·ADS-B·Remote ID·Flight Review·RTK/NTRIP·포워딩·서명·카메라·짐벌·사용자 동작·SiK | 기능 스펙 때 **각각 모듈로** 쪼갬 | — | E5, R-Q3·Q6·Q10 |

- **수명 규칙**: 전역 모듈은 `Composition`이 하나 만든다(앱 수명). 기체별 모듈은 Vehicle이 생길 때 `Composition`이 만들고 **부모를 Vehicle로** 둔다. **혼합 수명** 모듈은 전역 서비스와 기체별 서비스를 **따로** 두고(`LogsViewer` 전역 / `LogDownloadService::of(vehicle)`), 이름으로 구분한다.
- **얻는 법**: 기체별 서비스는 그 모듈 `api/`의 정적 조회 `XService::of(Vehicle*)`(내부는 Vehicle의 자식 검색). 컨트롤러는 `VehicleRegistry::activeChanged`에 **다시 묶는다**.

### 3.2 기능 순위표 (숫자가 클수록 아래; 의존은 아래로만, 줄 건너뛰기 허용, 같은 순위끼리 금지)

| 순위 | 기능 |
|---|---|
| 1 | `shell` |
| 2 | `notify` `ai` `appSettings` `analyze` `link` |
| 3 | `fly` `mission` `vehicleInfo` `preflight` `logs` |
| 4 | `layout` |
| 5 | `sensors` `radio` `flightModes` `power` `failsafe` `motors` `tuning` `fcFlash` `esc` `elrs` `tracker` |
| 6 | `map` |
| 7 | `video` `terrain` `gcsposition` |
| 8 | `params` |
| 9 | `extras`(쪼개질 때 각 모듈의 순위를 기능 스펙이 정한다; 정하기 전에는 9) |
| — | 바탕(모든 기능보다 아래): `app`(0) < `comm`(1) < `vehicle`(2) < `platform/ui`(2.5) — 왼쪽이 더 아래 |

- 위가 아래의 `api/`를 쓴다. **아래가 위를 쓰지 않는다** — 위에 보이려면 §9.8 기여 등록, 위를 열려면 `Navigator::open(id, arg)`, 위의 값이 필요하면 그 값을 더 아래 모듈이나 상태로 내린다.
- 예: `layout`(4)이 `motors`(5)의 api를 불러 **그림 모터 시험**을 한다(주인은 layout, OperationKind `MotorTest`). R-D35의 "표식≠기체값이면 시험 잠금" 판정은 `motors`가 할 수 없으므로(위의 `LayoutStates`를 읽을 수 없다) **`layout`이 판정해 그 결과를 `motors` api 호출의 인자로 넘긴다**.
- `esc`·`elrs`·`tracker`(5)가 파라미터(`SERVO_BLH_*`, `PASSTHRU_EN`)를 읽고 쓰는 것은 `params`(8)가 아래라서 허용.
- `map`(6)은 레이어 슬롯의 그릇이고 `video`·`terrain`(7)·`tracker`·`mission`·`fly`는 `platform/ui`의 `MapLayerSlot`에 등록한다(§9.8). `map`은 그들을 모른다.
- 순환은 `tools/check`가 `cmake --graphviz` 출력을 이 표와 대조해 잡는다.

## 4. 설정 — 세 가지를 섞지 않는다 (§17-5 확정)

| 종류 | 뜻 | 자리 | 바꾸는 사람 | 저장 |
|---|---|---|---|---|
| **부팅 설정** | 뜨기 전에 정해지는 것: 데이터 폴더, 로그 수준·회전, 개발 플래그(`--sitl`, `--display`) | `platform/app/boot/BootOptions`: 명령줄 + `kgc.ini`(`QStandardPaths::AppConfigLocation`) | 개발자·고급 사용자 | ini |
| **운영 설정** | 쓰면서 바꾸는 값 | 정의는 각 모듈 `internal/settings/<id>.settings.json`; 저장·검색은 `platform/app/settings/Settings`(QSettings) | 사용자 | QSettings(플랫폼 기본 위치, 데이터 폴더와 무관) |
| **고정값** | 바꾸면 안 되는 수 | 모듈 `internal/Constants.h`; 여러 모듈이 쓰는 값만 `platform/app/constants/`(§6) | 코드 | 코드 |

- 운영 설정은 **메타데이터 먼저**(R-N17). 항목: `id`(`<모듈>.<이름>`), `type`(`bool|int|double|string|enum|list<object>`), `default`, `min/max`, `enum`, `labelKey`(번역 키), `category`(모듈 id). 빌드 때 생성기가 **모듈마다** `api/kgc/<id>/Settings.h`에 형이 있는 접근자를 만든다(`kgc::esc::settings::flashChunk()`). 없는 키는 컴파일 오류. 코드에 설정 키 문자열을 쓰지 않는다. 바탕 `app/settings`는 모듈을 모르고 등록·저장·검색만 한다.
- **다른 모듈의 설정을 읽을 때**는 그 모듈 `api/`의 접근자를 순위표 방향으로만. 여러 기능이 함께 쓰는 안전 설정은 바탕에 둔다: 길게 누르기 시간 → `platform/ui`, 유도 거리·고도 한계·강제 시동 보이기 → `platform/vehicle/safety`.
- 설정 기본값은 JSON에만(코드 상수와 두 곳 금지).
- **비밀값**(API 키·서명 키·바인드 문구·WiFi 비밀번호): `platform/app/secrets/SecretStore`(QtKeychain, §17-4). 값은 `Secret` 형으로만: `QString` 암시 변환 없음, `QDebug <<` 없음, `reveal()`은 허용 파일에서만(검사). 설정 JSON에는 "있음/없음"만.
- **파일 자리**(`platform/app/paths/Paths`, 데이터 폴더 아래): `logs/app`, `logs/tlog`, `logs/onboard`, `missions`, `layouts`, `params/backups`(쓰기 전 전체 백업), `params/cache`(PX4 해시 캐시), `ai/changes`(AI·구성도 변경 사유), `video`, `cache/firmware`(해시 기록), `cache/tiles`, `cache/metadata`, `cache/terrain`.

## 5. 상태(states) — 묶음마다 주인 하나

- **바탕 묶음**(기체 자체의 값)은 `platform/vehicle/states/VehicleStates`(QObject, 기체마다 하나)가 갖는다. 값 묶음은 `Q_GADGET` 구조체 + `Q_PROPERTY … NOTIFY <bundle>Changed`; 큰 목록은 `QAbstractListModel`(행 단위 변경). 주인은 `VehicleStateOwner` 하나. 메시지 해석은 `internal/updaters/<Bundle>Updater`의 **순수 함수**(메시지 → 묶음 변경)라 Owner는 배선만 한다. QML은 활성 기체의 VehicleStates에 직접 바인딩해도 된다.
- **기능 묶음**(기능이 만드는 값)은 그 기능의 기체별 서비스가 `api/`로 내놓는다. 바탕은 기능 묶음을 모른다.
- 같은 값을 자기 캐시로 들지 않는다. 쓰려면 주인에게 요청하지 않는다 — 쓸 일은 상태가 아니라 명령(§9.2).

| 묶음 | 담는 것 | 출처 | 주인 | 모양 |
|---|---|---|---|---|
| `Link` | 연결 여부·주 링크·손실률·하트비트 시각·끊김·재부팅 중 | HEARTBEAT, RADIO_STATUS, 시퀀스, RebootWindow | `VehicleStateOwner` | Q_GADGET |
| `Arming` | armed, 모드(id+표시 이름), 준비 상태, **기체 쪽** 시동 불가 사유 | HEARTBEAT, SYS_STATUS, EVENT/STATUSTEXT(플러그인 해석 함수) | Owner | Q_GADGET |
| `Attitude` | roll·pitch·yaw·각속도 | ATTITUDE | Owner | Q_GADGET |
| `Position` | 위도·경도·AMSL·상대 고도·속도·헤딩·홈·GCS 거리 | GLOBAL_POSITION_INT, HOME_POSITION | Owner | Q_GADGET |
| `Gps` | fix·위성·HDOP·인스턴스 | GPS_RAW_INT, GPS2_RAW | Owner | Q_GADGET |
| `Battery[]` | 전압·전류·잔량·소모·셀 | BATTERY_STATUS, SYS_STATUS | Owner | ListModel |
| `Sensors` | 센서별 켜짐·건강 | SYS_STATUS 비트 | Owner | Q_GADGET |
| `Rc` | 채널 값·RSSI | RC_CHANNELS | Owner | Q_GADGET |
| `Flight` | 비행 시간·거리·착륙 상태·VTOL 상태 | EXTENDED_SYS_STATE, 계산 | Owner | Q_GADGET |
| `Messages` | STATUSTEXT·이벤트 목록(심각도·시각·읽음) | STATUSTEXT, EVENT | Owner | ListModel |
| `Esc[]` | rpm·전압·전류·온도 | ESC_STATUS/INFO, AP ESC_TELEMETRY | Owner | ListModel |
| `Capabilities` | 기능 비트마스크, 버전·UID, 지원 범위(D17) 판정 | **단계별**: HEARTBEAT → AUTOPILOT_VERSION → `reportParamsReady()`(params가 알림) → `reportProbe(cap, ok)`(기능이 알림) | Owner | Q_GADGET |
| `ArmBlockers` | **GCS 쪽** 시동 불가 사유(보정 뒤 재부팅 전, 방향 변경 뒤 재보정 전, PASSTHRU_EN 확인 전, 작업 잠금 중…) | 기능의 요청 | `vehicle/safety/ArmGuard` — `block(reason) → 토큰`, `release(토큰)` | ListModel |
| `Params` | 값 전체·내려받기 진행·메타데이터 | PARAM_VALUE, FTP | `features/params` `ParamService`(기체별) — 값을 드는 **유일한 곳** | ListModel |
| `MissionSync` | 기체 미션 해시·동기 여부·현재 항목 | MISSION_CURRENT, 전송 결과 | `features/mission` `MissionService`(기체별) | Q_GADGET |
| `LayoutStates` | 구성도 표식·기체값과의 차이 | 구성도 파일, Params | `features/layout`(기체별) | ListModel |
| `TrackerStates` | 연결·모드·헤딩·각도·GPS·전압·리셋 원인 | 트래커 프로토콜 | `features/tracker` `TrackerService`(전역) | Q_GADGET |
| `GcsPositionStates` | GCS 위치·fix | NMEA/UBX | `features/gcsposition`(전역) | Q_GADGET |

- 스레드: 링크 워커 → `LinkManager::messageReceived(linkId, msg)` **큐드 시그널** → GUI 스레드의 Owner가 갱신. 묶음은 `Q_DECLARE_METATYPE`.

## 6. 고정값·열거·오류 코드·로그 사건

- **고정값**: 모듈 `internal/Constants.h`. 여러 모듈이 쓰는 값만 `platform/app/constants/`: `mavlink.h`(ACK 1200ms·3회, GCS 하트비트 1Hz, 끊김 3500ms), `safety.h`(비상 정지 21196, 강제 시동 2989, 모터 시험 3초), `mission.h`(1500ms·250ms·5회). CRC 다항식은 `app/crc`, 트래커 프로토콜 상수는 공용 헤더(§11)에만.
- 검사 범위: `chrono` 리터럴, 타이머·Timeout 인자, 4자리 이상 16진수, 3자리 이상 10진수(허용 360·180·90·1000·60). 오프셋·시프트·인덱스(<256)는 대상 아님. QML은 `width`·`height`·`padding`·`margins`·`spacing`·`radius`·색 리터럴만 `Theme`에(anchors·z·opacity 등은 대상 아님).
- **열거**: 모듈 `api/`의 `enum class`. 펌웨어가 정하는 열거(모드·회전·서보 기능)는 플러그인이 메타데이터에서 읽어 제공.
- **모듈 id**: 모듈마다 `api/kgc/<id>/ModuleId.h`에 `inline constexpr kgc::app::errors::ModuleId kModuleId{18};`(16비트, 모듈별 고유, 10진). 바탕 `app/errors/ModuleId`는 형만 정의하고 값을 모른다. 중복은 검사가 모아 본다. `ModuleId.h`는 고정값 검사에서 제외한다.
- **오류 코드**: `platform/app/errors/ErrorCode = {ModuleId module; uint16 code;}` 하나의 형. 모듈은 `api/kgc/<id>/Errors.h`에 `enum class EscError : uint16_t`와 `ErrorCode make(EscError)`를 둔다. `Result<T> = std::expected<T, ErrorCode>`. 사용자 문구는 모듈이 `errorText(ErrorCode)`(번역 대상)로.
- **로그 사건**: `platform/app/log/LogEvent = {ModuleId, uint16}` 형. 모듈은 `internal/Events.h`에 `enum class EscEvent : uint16_t`. 검사가 전부 모아 중복·미등록을 본다.

## 7. 자료 규격(파일 형식)과 원본 데이터

| 형식 | 자리(코드) | 문서 |
|---|---|---|
| 설정 메타데이터 `*.settings.json` | `platform/app/settings` | `specs/formats/settings.md` |
| 앱 로그(한 줄 = 시각·수준·카테고리·사건·메시지) | `platform/app/log` | `specs/formats/applog.md` |
| 미션 `.plan`(QGC v1 호환), `.waypoints` 읽기, KML 내보내기, KML/SHP 가져오기, 명령 카탈로그 JSON | `features/mission`, `resources/data/mission/` | `specs/formats/plan.md`, `mission-commands.md` |
| 기체 구성도 `.kgclayout.json` + `.params` 묶음, 실루엣 | `features/layout`, `resources/silhouettes/` | `specs/formats/layout.md` |
| 파라미터 파일(QGC 형식 + MP 2열 읽기), 묶음 이름·작업 단위 묶음 정의(펌웨어·버전별), PX4 해시 캐시 | `features/params`, `resources/data/params/` | `specs/formats/params.md`, `taskset.md` |
| 비행 전 점검표 정의 | `features/preflight`, `resources/data/preflight/` | `specs/formats/preflight.md` |
| 텔레메트리 로그 `.tlog`(8바이트 BE µs + 패킷), CSV 텔레메트리 | `platform/comm/tlog` | `specs/formats/tlog.md` |
| 기체 로그 `.ulg` `.bin` 읽기 | `features/logs` | `specs/formats/onboard-log.md` |
| 지형 캐시 | `features/terrain` | `specs/formats/terrain-cache.md` |
| 트래커 프로토콜 | `tracker/protocol/kgc_tracker_protocol.h`(원본, §11) | `specs/formats/tracker-protocol.md` |
| AI·구성도 변경 기록, 파라미터 백업 | `features/ai`, `features/layout`, `features/params` | `specs/formats/changes.md` |
| 번역 `.ts/.qm` | `i18n/` | — |

규격 문서는 기능 스펙과 함께 쓰고, 읽기·쓰기 함수는 그 모듈 하나에만. 펌웨어별 데이터는 플러그인 아래 `data/`.

## 8. 펌웨어 플러그인 계층 (R-Q12·R-N4) — 데이터와 순수 함수만

- 자리 `platform/vehicle/firmware/`: `api/kgc/vehicle/firmware/FirmwarePlugin.h`, `internal/{px4,ardupilot,generic}/`.
- **선택**: **compid 1이고 `autopilot ≠ MAV_AUTOPILOT_INVALID`인 첫 HEARTBEAT**로 `FirmwarePluginFactory`가 고른다(짐벌·컴패니언 하트비트는 거른다). 모르는 autopilot이면 `generic`. 그 뒤 바꾸지 않으며, 값이 바뀌면 기체를 새로 만든다.
- **내놓는 것**: 기능 비트마스크 초기값과 단계별 보강 규칙; 모드 표(id ↔ 표시 이름); 이륙·일시정지·고도 변경·속도 변경의 **`CommandStep` 목록**(어떤 메시지를 어떤 순서로, 어떤 ACK를 기다리는지 — 실행은 기능 모듈); 시동 불가 사유 해석 함수(PX4 events / AP PreArm 텍스트 → 사유 목록); 파라미터 메타데이터 출처와 이름 바꿈표; 미션 번호 규칙(PX4 홈 없음); 보정 방식 기술자(STATUSTEXT 파싱 / 역방향 명령); 구성도 대응표(R-D33); AI 거부 목록(R-D5 ⑥); 지원 버전 범위(D17); 고도 틀 값(`AltitudeFrame` — `app/geo/Altitude`는 펌웨어를 모르고 frame을 인자로 받는다, R-N11).
- 플러그인은 **실행하지 않는다**(기체에 보내지 않고 상태를 쓰지 않는다). 기능 모듈이 `plugin.takeoffSteps()`를 받아 §9.2의 VehicleSender로 보낸다.
- 검사: `vehicle/firmware/` 밖에서 `MAV_AUTOPILOT_PX4`·`MAV_AUTOPILOT_ARDUPILOTMEGA` 비교, `"PX4"`·`"ArduPilot"`·`"APM"` 문자열 비교를 잡되 **허용 목록**: `features/fcFlash`(펌웨어 선택 화면), `features/logs`(.ulg/.bin), `features/params`(메타데이터 출처 표시), 번역 파일. 기능 모듈 안에 펌웨어 이름의 하위 폴더는 금지.

### 8.1 전송 방식별 절차의 자리
펌웨어가 아니라 **전송 방식**에 따라 절차가 다른 것은 기능 모듈 `internal/paths/<방식>/`: `esc/internal/paths/{blhPassthrough, serialControl, direct}`, `elrs/internal/paths/{uart, wifi, bfPassthrough, etxPassthrough, fcPassthrough}`. 어느 방식을 쓸지는 `Capabilities`와 사용자 선택으로 고른다. **ESP 시리얼 부트로더 클라이언트**(SLIP·스텁·DTR/RTS 리셋·MD5 검증)는 `elrs`와 `tracker`가 함께 쓰므로 바탕 `platform/comm/espflash`에 둔다(`<kgc/comm/espflash/EspFlasher.h>`); `elrs/paths/uart`와 `tracker`의 굽기는 이것을 부른다.

## 9. 조립·수명·송신·공통 틀

### 9.1 조립과 수명
- `src/main/Composition`이 모든 `api/`를 include해 조립한다(로직 없음). `main.cpp`: BootOptions → Composition → Shell 창.
- `comm/links/LinkManager`(링크), `vehicle/registry/VehicleRegistry`(기체, sysid 키; compid 1이 자동조종기, 다른 compid는 그 기체의 `Component`). **활성 기체** `VehicleRegistry::active` 하나 — 상단바 기체 표시, 트래커 기본 대상, 구성도 편집 대상이 본다.
- 수명은 §3.1 규칙. 기체 삭제: `vehicleRemoved` → `deleteLater`. 참조는 `QPointer`/활성 기체 프로퍼티.
- **포트 소유 등록부** `comm/rawport/PortRegistry`: 모든 시리얼 포트의 현재 소유자 — MAVLink 링크 / `RawPort`(트래커·GCS GPS처럼 오래 쥐는 비-MAVLink 포트) / `PortLease`(굽기·패스스루, R-Q1a (가)형). **굽기·패스스루는 임대 없이 포트를 열 수 없다.** USB 자동 연결은 등록부에 있는 포트를 건드리지 않는다.
- **임대의 동작**(R-Q1a (가)): `PortLease::acquire(port)`는 그 포트를 쓰던 MAVLink 링크에 `LinkManager::suspend(link)`를 걸어 **GCS 하트비트·자동 재접속·끊김 경고를 멈추고** 포트를 넘긴다(`Link.suspended`=true, 기체는 "작업 중"으로 표시). `release()`가 `resume(link)`으로 되돌린다. AP BLHeli 패스스루의 4초 침묵은 임대 뒤 `PortLease`가 기다려 준다.
- **의도한 재부팅 구간** `vehicle/safety/RebootWindow`: 기능이 `begin(vehicle, timeout)`으로 열면 그동안 끊김 판정이 "끊김"이 아니라 "재부팅 중"(`Link.rebooting`)이 된다(R-Q1a (나)).

### 9.2 기체로 나가는 문 하나와 시동 중 규칙
- `vehicle/outbound/VehicleSender`가 기체로 가는 **모든 MAVLink 송신**의 유일한 문(주 링크 선택, sysid·compid 채움). 송신마다 `OutboundKind`(Arm, Motion, Command, Param, Mission, Other).
- **시동 중 규칙(R-N10)은 두 곳에서 판정한다.**
  - ① **작업 시작 때**: 호출자가 `OperationKind`를 밝히고 `vehicle/safety/ArmedPolicy::check(kind, scope)`를 부른다. `scope`는 `One(vehicle)`(그 기체만 본다) 또는 `Any`(연결된 기체 중 하나라도 시동 중이면 시동 중으로 본다). **기체 없는 작업**(ESC 직접 연결·ELRS UART/WiFi·트래커 굽기)은 모두 `Any`로 판정한다 — 어느 기체든 시동 중이면 조종 중인 장치를 구울 수 있으므로. 결과는 `Allow / AllowWithWarning(길게 눌러 확인) / Deny`. 표는 R-N10을 그대로 옮긴다:

    | OperationKind | 시동 중 |
    |---|---|
    | `SetupPage`(보정·기체 종류·조종기 등 설정 페이지 열기), `FcFlash`, `EscFlash`, `ElrsFlash`, `TrackerFlash`(scope Any), `EscConfig`, `ElrsConfig`, `AiWrite`, `ParamBulk`(파일 일괄), `LayoutWrite`, `MotorTest` | Deny |
    | `ParamSingle`, `Tuning`, `Autotune` | AllowWithWarning |
    | `TrackerConfig`, `TrackerCalib`, `TrackerManual` | Allow(추적 중단 경고) |
    | `FlyAction`(시동·이륙·착륙·RTL·일시정지·유도), `MissionStartStop`, `Read`(읽기·내려받기) | Allow(각자의 길게 누르기 규칙) |
  - ② **Sender에서**: `OperationKind`를 모르므로 **`WorkLock`과 `RebootWindow`만** 본다 — 잠금 중에는 `Arm`·`Motion`·`Command`·`Param`·`Mission` 가운데 토큰을 쥔 작업의 송신만 통과하고(그 작업 자신의 단계는 예외), **`Other`(GCS 하트비트·스트림 요청 등 링크 유지 송신)는 막지 않는다**(R-Q1a (나) 하트비트 유지). 재부팅 구간의 기체에는 `Arm`·`Motion`을 보내지 않는다. 시동 중의 `Param`·`Mission` 송신은 ②에서 막지 않는다(판정은 ①에서 끝남).
  - **작업 도중 시동이 걸리면**: `vehicle/safety/JobArmWatch`(Job에 붙은 도우미)가 `Arming.armed` 변화를 보고 Deny 종류는 Job을 **중단**(안전한 단계까지 끝낸 뒤), AllowWithWarning 종류는 경고만.
- `vehicle/command/CommandQueue`는 Sender 위의 **재시도 계층**(COMMAND_LONG/INT·ACK·타임아웃·재시도). 보정 시작처럼 ACK가 늦는 명령은 `{retries=0, timeout=설정값}` 모드로 큐를 탄다 — 예외 목록 없음. AP 가속도 보정의 "다음"(COMMAND_ACK 송신)도 Sender를 지난다.
- 보내기 스레드: Sender가 `QMetaObject::invokeMethod(link, …, Qt::QueuedConnection)`으로 링크 워커에 넘긴다.
- 검사: 링크 쓰기 API는 `comm/`·`vehicle/outbound/` 밖 금지; `command_(long|int)_(pack|encode)`는 `vehicle/command/` 밖 금지.

### 9.3 바깥과 닿는 자리

| 무엇 | 유일한 자리 | 검사 |
|---|---|---|
| HTTP(S) | `platform/app/net/Http`: HTTPS만, 타임아웃·재시도, 다운로드 캐시+해시 기록, `onlineChanged`(R-N13) | `QNetworkAccessManager`가 밖에 있으면 |
| 환경 변수 | `platform/app/boot/BootOptions` | `qEnvironmentVariable`·`getenv` 위치 |
| 시리얼 포트 | `platform/comm/links/serial`, `platform/comm/rawport` | `QSerialPort` 생성 위치 |
| 조이스틱(SDL3) | `platform/comm/joystick` | SDL include 위치 |
| SQL(SQLite: 타일·메타데이터·펌웨어 캐시) | `platform/app/store` | QtSql include·`QSqlQuery` 위치 |
| TTS | `platform/app/notify/Speech` | — |

### 9.4 오래 걸리는 작업과 오류
- `platform/app/job/Job`(QObject): `OperationKind`(열거는 0층 `<kgc/app/job/OperationKind.h>`에 정의 — 기체를 모르는 이름표일 뿐), 상태(대기·진행·성공·실패·취소), 단계 이름, 진행률, 남은 시간, `cancel()`, 끝날 때 `finished(Result<void>)` 한 번, `errorCode`·`errorText` 프로퍼티. 임대·잠금·ArmGuard 토큰은 Job이 **RAII**로 쥔다. Job은 기체를 모른다: 시동 감시는 2층 `vehicle/safety/JobArmWatch`가 `attach(job, scope)`로 붙여서 `Arming.armed` 변화에 Job을 중단·경고한다(§9.2). 화면은 `platform/ui/components/JobView.qml`로 보여 준다. FC·ESC·ELRS·트래커 굽기, 미션 전송, 파라미터 내려받기, 보정, 로그 내려받기가 전부 Job이다.
- `Result<T> = std::expected<T, ErrorCode>`. 함수는 예외 대신 Result. 예외·`Q_ASSERT`·`qFatal`은 프로그램 버그에만. 워커 스레드의 오류는 `ErrorCode`를 큐드 시그널로 소유 객체에 올리고, 알림은 소유 객체만 낸다.

### 9.5 주기 작업·타임아웃·스레드
- `platform/app/scheduler/Scheduler`에 **이름 있는 반복 작업만** `register(name, period, owner, context)`(context 스레드에서 돈다): GCS 하트비트 1Hz(링크 스레드), 끊김 검사 1Hz, 시리얼 포트 목록 1Hz, tlog 플러시. 트래커 각도 송신은 반복이 아니라 **위치 수신에 따라**(최대 5Hz 제한은 `Timeout`).
- 일회성 타임아웃은 이름 붙은 `app/scheduler/Timeout`. QML `Timer`는 `platform/ui/components/TimerBox`만. 검사 대상 `QTimer`·`singleShot`·`startTimer(`·`QBasicTimer`·QML `Timer {`.
- 스레드: 링크 I/O·MAVLink 파싱은 링크마다 워커, **영상 디코드·타일 로드·지형 조회도 워커**(R-N15). 상태 갱신·기능 로직·화면은 GUI 스레드.

### 9.6 공통 함수
`platform/app/geo/Altitude`(AMSL·상대·지형·타원체고↔MSL, frame 인자) **한 묶음**, `Geodesy`(방위·고각·거리 — R-D23의 한 함수), `app/units`(표시 단위), `app/crc`(CRC16-XMODEM, CRC-0xA001, CRC8-0xD5 — 시험 벡터는 `tests/`), `app/util`. 네임스페이스 범위의 자유 함수가 두 모듈에 같은 이름이면 검사가 잡는다.

### 9.7 화면 바탕 `platform/ui` (QML 모듈 `Kgc.Ui`)
- `Theme.qml`(크기·색·글꼴 토큰), `components/`(HoldToConfirm, JobView, Pill, Chip, TimerBox), `PanelRegistry`(좌 메뉴 패널: id·순서·QML 컴포넌트), **슬롯**(`TopbarSlot` 왼쪽/가운데/오른쪽, `StatusChipSlot` 하단 왼쪽/오른쪽, `RightTabSlot`, `MapLayerSlot`), `Navigator`(`open(panelId, arg)`).
- 기능 모듈의 `ui/register.cpp`가 `registerUi(registry)`에서 패널·슬롯 기여·레이어를 등록한다. `shell`은 **잎 모듈**: 틀을 그리고 등록부를 읽어 배치만 한다.
- 컨트롤러(`<Id>Controller`)는 상태를 표시값으로 옮기고 서비스를 호출만. 판정은 서비스·ArmedPolicy.
- QML `import` 허용: `QtQuick*`·`QtQml*`, §2의 Qt 모듈(Location·Positioning·Multimedia·Graphs·Quick3D), `Kgc.Ui`, 자기 `Kgc.<Id>`, **순위표에서 아래인** `Kgc.<Id>`. 그 밖은 검사가 잡는다.

### 9.8 기여 등록(아래가 위에 보이는 법)

| 기여 | 누가 | 어디에 |
|---|---|---|
| 상단바 조작 띠(모드·ARM·이륙·착륙·RTL·일시정지·비상 정지) | `fly` | `TopbarSlot(가운데)` |
| 상단바 미션 선택·시작·WP | `mission` | `TopbarSlot(가운데)` |
| 상단바 벨 | `notify` | `TopbarSlot(오른쪽)` |
| 상단바 AI 아이콘·패널 | `ai` | `TopbarSlot(오른쪽)` |
| 하단 기체 칩·연결 팝업 | `link` | `StatusChipSlot(왼쪽)` |
| 하단 센서 ✓/✕ 칩 | `vehicleInfo` | `StatusChipSlot(왼쪽)` |
| 하단 트래커·GCS GPS 칩 | `tracker`, `gcsposition` | `StatusChipSlot(오른쪽)` |
| 우 사이드바 탭 | `vehicleInfo`(계기·개요·상세·메시지), `logs`(로그), `preflight`(점검) | `RightTabSlot` |
| 지도 레이어 | `mission`(편집·경로·**펜스·랠리**), `fly`(유도 표식), `tracker`(부채꼴), `terrain`(단면), `video`(PiP), `extras`의 ADS-B 모듈(교통) | `MapLayerSlot` — `map`이 그릇 |
| 좌 메뉴 패널 | §3.1 좌 메뉴 모듈 | `PanelRegistry` |
| 정보 창 | `appSettings` | `Navigator.open("appSettings","about")` |

## 10. 로그·오류·알림·번역 (§17-6 확정)

- **로그**: 호출은 `KGC_LOG(kCategory, EscEvent::FlashFailed, "메시지", 값…)` 매크로 **하나**로만(`qDebug`·`qInfo`·`qWarning` 직접 호출과 QML `console.log` 금지 — 검사). 카테고리는 모듈 경로(`kgc.comm.serial`, `kgc.esc`). 메시지는 **한국어 명사형 전보체**(값은 콜론 뒤, 절은 쉼표, 문장 어미 없음). 사건은 모듈 `Events.h`의 열거(§6). `platform/app/log/LogSink`가 Qt 메시지 핸들러로 파일 회전(부팅 설정)과 비밀값 마스킹을 한다. tlog·포워딩에는 `comm/mavlink/signing`의 필터가 SETUP_SIGNING을 뺀다(R-N12·R-N23).
- **오류**: §6의 `ErrorCode`. 사용자 문구는 `tr()`.
- **알림**: `platform/app/notify/Notify`(저장·수준 `Info/Warning/Critical`·출처(기체·트래커·GCS·앱)·시각·읽음) — 상단 벨·토스트·TTS(`Speech`)가 읽는 한 입구. 기체 메시지 중 무엇을 알림으로 올릴지는 **기능 스펙(B4)이 정한다**.
- **번역**: 원문 **한국어**, `i18n/kgc_en.ts`가 영어. `platform/app/i18n/Translator.retranslate()`로 재시작 없는 전환(R-N16). 검사: C++의 한글 리터럴이 `tr(`·`QT_TR_NOOP(`·`KGC_LOG(` 밖에 있으면, QML의 `text:`·`title:`·`placeholderText:`·`ToolTip.text:` 리터럴이 `qsTr(` 없이 있으면 잡는다.

## 11. 트래커 스케치 `tracker/` (R-D27)

- `tracker/`에 둔다(별도 프로그램, 층 규칙 없음, 크기 검사는 받음). 빌드 도구(PlatformIO/ESP-IDF)는 **트래커 기능 스펙에서** 정한다. `.github/workflows/tracker.yml`이 변종별 이미지를 만들고 패키징이 `firmware/tracker/<변종>/`에 넣는다(R-D28).
- 공용 헤더 `tracker/protocol/kgc_tracker_protocol.h`: **C99 헤더 전용**, `static inline` 함수가 바이트를 리틀 엔디언으로 직접 읽고 쓴다(패킹 구조체 금지 — MSVC/GCC 차이). 프레임 CRC·매직값·프로토콜 버전·명령 id·절대 한계는 **이 헤더에만**. GCS 쪽은 `features/tracker/internal/protocol/`이 이 파일 하나를 include한다(복사 금지; `src/` 밖 include 허용은 이곳뿐).
- 장치 설정(서보 범위·반전·중립·슬루·히스테리시스·편각·생존 시간)의 **주인은 ESP32 NVS**. GCS는 `TrackerStates`로 읽고 "설정 쓰기" 명령으로만 바꾼다. GCS 운영 설정에는 추적 대상·위치 출처·고정 좌표만.
- 평상시 연결은 `comm/rawport/RawPort`로 쥔다(PortRegistry 등록).

## 12. 모듈 설명서 `MODULE.md` 틀

```
# <모듈 id> (<바탕 n층 | 기능 순위 n>)
역할: 한 문장.
수명: 전역 | 기체별 | 혼합(전역 X + 기체별 Y).
받는 것: 읽는 상태 묶음·시그널·설정(이름으로).
주는 것: api/의 클래스·함수·시그널·상태 묶음.
기여: 등록하는 슬롯·패널·레이어.
의존: 쓰는 모듈(api 이름).
안 하는 것: 일부러 하지 않는 일과 그것을 하는 모듈.
흐름(사용자가 따라가는 진입점이 있으면): 1. … 2. …
```
- 100줄 이하. `api/`나 모듈 간 의존이 바뀐 커밋에서 함께 고친다(검사).

## 13. 용어집 (한국어 ↔ 식별자)

| 한국어 | 식별자 | 뜻 |
|---|---|---|
| 바탕 모듈 / 기능 모듈 / 조립 | `platform` / `feature` / `main` | §1 |
| 공개 인터페이스 / 내부 / 화면 | `api` / `internal` / `ui` | 모듈 폴더 |
| 화면 바탕 | `platform/ui` | 테마·부품·등록부·슬롯 |
| 기여 지점 / 이름으로 열기 | `Slot` / `Navigator` | §9.7·9.8 |
| 기체 / 활성 기체 / 컴포넌트 | `Vehicle` / `ActiveVehicle` / `Component` | |
| 링크 / 주 링크 / 원시 포트 / 포트 임대 / 포트 등록부 | `Link` / `PrimaryLink` / `RawPort` / `PortLease` / `PortRegistry` | §9.1 |
| 상태 묶음 / 주인 / 목록 상태 | `Bundle` / `Owner` / `ListModel` | §5 |
| 기능 비트마스크 | `Capability` | |
| 펌웨어 플러그인 / 명령 단계 | `FirmwarePlugin` / `CommandStep` | §8 |
| 송신 문 / 송신 종류 / 명령 큐 | `VehicleSender` / `OutboundKind` / `CommandQueue` | §9.2 |
| 작업 종류 / 기체 범위 | `OperationKind` / `VehicleScope` | 시동 중 규칙 판정 |
| 작업 잠금 / 시동 지킴이 / 시동 중 규칙 / 재부팅 구간 | `WorkLock` / `ArmGuard` / `ArmedPolicy` / `RebootWindow` | |
| 작업 | `Job` | §9.4 |
| 서비스 / 컨트롤러 / 패널 / 메뉴 | `Service` / `Controller` / `Panel` / `Menu` | |
| 파라미터 / 묶음(계열) / 작업 단위 묶음 | `Param` / `ParamGroup` / `TaskSet` | |
| 미션 / 미션 편집기 / 유도 명령 | `Mission` / `MissionEditor` / `GuidedAction` | |
| 시동 / 해제 / 비상 정지 / 길게 눌러 확인 | `Arm` / `Disarm` / `EmergencyStop` / `HoldToConfirm` | |
| 기체 구성도 / 부품 표식 | `Layout` / `LayoutMarker` | 5.5절 |
| 안테나 트래커 / 받침 정면 방위 | `Tracker` / `BaseHeading` | 5.4절 |
| 알림 / 기체 메시지 | `Notification` / `VehicleMessage` | |
| 앱 로그 / 텔레메트리 로그 / 기체 로그 | `AppLog` / `TelemetryLog` / `OnboardLog` | |
| 로그 사건 / 기체 이벤트 | `LogEvent` / `VehicleEvent` | 우리 로그 / PX4 events |
| 운영 설정 / 부팅 설정 / 고정값 / 비밀값 / 모듈 id | `Setting` / `BootOption` / `Constant` / `Secret` / `ModuleId` | |
| 펌웨어 세팅 모듈 / 안전·페일세이프 모듈 / 연결 모듈 | `fcFlash` / `failsafe` / `link` | 기능 id |

## 14. 자동 검사 `tools/check/` (R-N21; §17-7 확정)

커밋 훅(`core.hooksPath=tools/check/hooks`)과 CI(`before..after` 범위의 커밋마다)가 같은 Python 3 스크립트를 돈다. 함수 길이는 lizard(버전 고정). 제3자·생성 코드 제외.

| 검사 | 규칙 | 도입 |
|---|---|---|
| 경계 | (컴파일러) 남의 `internal/`·`ui/` include 불가. (스크립트) 바탕 층 순서 app<comm<vehicle<ui, 바탕→기능 include 금지, 기능 안 ui→internal→api 방향, `..` include 금지, api→internal include 금지, **모듈 그래프 순환**(graphviz와 §3.2 대조) | 0.0.1 |
| 크기 | 파일 400 / 함수 60 / QML 300 | 0.0.1 |
| MODULE.md | 모듈마다 있고 100줄 이하; api·의존 변경 커밋에 함께 | 0.0.1 |
| QML import | §9.7 허용 목록 밖 import | 0.0.1 |
| 문자열(QML) | `text:`·`title:`·`placeholderText:`·`ToolTip.text:` 리터럴에 `qsTr(` 없음; `console.log` | 0.0.1 |
| MAVLink include | `<kgc/comm/mavlink/Mavlink.h>` 밖의 `mavlink/*.h`, `ui/`의 MAVLink | 0.1.x |
| 펌웨어 분기 | §8 규칙 + 허용 목록 | 0.1.x |
| 송신 | §9.2 규칙 | 0.1.x |
| 바깥 접점 | §9.3 표 | 각 자리가 처음 쓰일 때 |
| 타이머 | §9.5 | 0.1.x |
| 고정값 | §6 범위 | 0.1.x |
| 설정 키 | 설정 키 문자열 리터럴(생성 접근자 밖) | 0.1.x |
| 로그·오류 | `qDebug` 계열 직접 호출; 등록부(모든 모듈 `Events.h`·`Errors.h`·`ModuleId.h`) 밖·중복; `KGC_LOG` 메시지에 문장 어미(…다/…요) | 0.1.x |
| 문자열(C++) | 한글 리터럴이 `tr(`·`QT_TR_NOOP(`·`KGC_LOG(` 밖 | 0.1.x |
| 비밀값 | `Secret::reveal()`이 허용 파일 밖 | 비밀값이 처음 쓰일 때 |
| 이름 중복 | 자유 함수 이름이 두 모듈에 | 0.1.x |

## 15. 빌드·CI·배포 (R-N18~R-N22)

- `.github/workflows/`: `check.yml`(구조 검사·`tests/`), `linux.yml`(Ubuntu 24.04 → AppImage), `windows.yml`(MSVC 2022, Qt 6.12.x aqtinstall → zip), `tracker.yml`(트래커 이미지). 태그에서 Release.
- 산출물에 `LICENSE`, `THIRD_PARTY_NOTICES.md`(CPM 목록 + `cmake/notices/extra.json`의 CPM 밖 의존 — Qt·GStreamer·MSVC 런타임·트래커 라이브러리 — 를 CI가 합쳐 생성), 소스 위치. 정보 창(`appSettings`)이 이것을 읽어 보여 준다. SBOM은 0.1.x부터.
- 0.0.1 산출물: Windows zip(windeployqt·MSVC 런타임) + AppImage. GStreamer·NSIS는 영상 기능이 들어오는 버전부터.

## 16. 이 문서가 정하지 않는 것
각 기능의 동작(기능 스펙), 화면의 모양(`specs/ui/`), 바이트 명세(규격 문서), 테스트 내용, 알림 승격 기준(B4), 트래커 빌드 도구(트래커 스펙), 성능 수치, `extras`의 모듈 분할(각 기능 스펙).

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
| 8 | 네임스페이스·크기 상한 400/60/300 | **확정** |
| 9 | **요구사항 개정**: R-N6을 "모듈러 모놀리스(§1.2 대응표)"로, R-N2를 "api·의존 변경 커밋에서 함께"로 좁힘. 기능 간 의존은 순위표(§3.2)로 허용, 역방향은 기여 등록·Navigator·상태로 | **확정** 2026-10-24 |
| 10 | 기능 id: `firmware`→`fcFlash`, `safety`→`failsafe`; 와이어프레임 id 맞춤 | **확정** 2026-10-24 |
| 11 | **초안 3·4의 폴더 변경**: `platform/ui` 신설, `api/kgc/<경로>/` include 형식, 기능 모듈 `link`·`map`·`vehicleInfo`·`preflight`·`analyze`·`extras` 추가, `mission` 전역 편집기+기체별 서비스 분리 | **확정** 2026-10-24 |

## 18. 검토·승인 기록

| 날짜 | 누가 | 무엇 |
|---|---|---|
| 2026-10-24 | reviewer-1 | 설계 검토 1: fail, 20개(`.pm/base/reviewer-1.md`) → 초안 2 |
| 2026-10-24 | 사용자 | §17 1~8 확정 |
| 2026-10-24 | reviewer-2 | 설계 검토 2: fail, 이전 15 해결·5 부분, 새 12개(`.pm/base/reviewer-2.md`) → 초안 3 |
| 2026-10-24 | reviewer-3 | 설계 검토 3: fail, 이전 6 해결·6 부분, 새 8개(`.pm/base/reviewer-3.md`) → 초안 4(참조 없이 완결, params 순위 8, 슬롯은 platform/ui 한 곳, ArmedPolicy 표·VehicleScope·② 범위, `link` 모듈, mission 분리, QML import 허용, 하위 영역 접두어·ModuleId, 기여 표 보완) |
| 2026-10-24 | 사용자 | §17 9·10·11 승인(R-N6·R-N2 개정, 기능 id, 폴더 변경) |
| 2026-10-24 | reviewer-4 | 설계 검토 4: fail, 이전 7 해결·1 부분, 새 6개(`.pm/base/reviewer-4.md`) → 초안 5(comm/espflash, map 레이어 정리, OperationKind 0층·JobArmWatch, scope Any 통일·② Other 통과, 문장·순위·부등호, ModuleId, PortLease 동작, 센서 칩·링크 편집 주인) |
| | reviewer-5 | (대기) |
| | 사용자 | (대기) |
