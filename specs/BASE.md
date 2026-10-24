# K-GroundControl 기반 문서 (BASE)

- 상태: **초안 1 (승인 대기)** — 2026-10-24, PM 작성
- 이 문서가 정하는 것: **무엇을 어떻게 구분해서 어디에 두는가.** 폴더·층·모듈의 모양, 설정·상태·고정값·공통 함수·저장소·주기 작업·서비스·입출력의 자리, 로그·오류·알림의 규칙, 그리고 그것을 지키는 자동 검사. 기능이 무엇을 하는지는 `specs/REQUIREMENTS.md`와 `specs/features/`가 정한다.
- 이 문서가 승인되기 전에는 기능 스펙을 쓰지 않는다. 기능 스펙의 "기반" 절은 이 문서의 이름을 그대로 쓴다. 구현 중에 여기 없는 자리가 필요해지면 만들지 않고 이 문서로 돌아온다.
- 구현 주체는 GPT(D12)이므로, 이 문서는 "읽는 사람이 대화 기록 없이도 같은 구조를 세울 수 있게" 쓴다. 코드 규칙의 영어판은 저장소 루트 `AGENTS.md`(다음 문서).

## 1. 첫째 원칙 — 부분만 보고 고칠 수 있다 (R-N1~N7)

| # | 원칙 | 코드에서의 뜻 |
|---|---|---|
| 1 | 모듈 = 폴더 하나 | 밖에서 쓰는 것은 그 폴더의 **공개 헤더**(`<Module>.h`)에만. 내부는 `internal/`. 다른 모듈의 `internal/`을 include하면 검사가 막는다 |
| 2 | 모듈마다 자기 설명서 | 폴더 안 `MODULE.md`: 역할 한 줄, 받는 것, 주는 것, 의존하는 모듈, 안 하는 것, 흐름(있으면). 코드와 **같은 커밋**에서 고친다 |
| 3 | 공유 상태는 주인 하나 | `states`는 주인 모듈만 쓰고 나머지는 읽는다(§5) |
| 4 | 펌웨어 차이는 한 계층 | `vehicle/firmware/`의 플러그인 + 기능 비트마스크(§8). 그 밖의 어디에도 `if (px4)`가 없다 |
| 5 | 크기 상한 | 파일 **400줄**, 함수 **60줄**, QML 파일 **300줄**. 넘으면 쪼갠다. 검사가 센다 |
| 6 | 의존은 아래로만 | 0층 < 1층 < 2층 < 3층 < 4층. 위층은 아래층의 공개 헤더만 include. 아래층이 위로 알릴 때는 **시그널**(Qt signal) 또는 콜백 |
| 7 | 이름은 용어집에서 | §13 용어집의 낱말만. 새 낱말은 용어집에 먼저 넣는다 |

## 2. 기술 기반 (요구사항 D3·D4·R-N18 그대로)

- 언어 C++20, Qt 6.12 LTS(Quick/QML 화면, Core·Network·SerialPort·Multimedia·Location·Positioning·TextToSpeech·Graphs·Quick3D 사용 가능 — GPL-3.0).
- 빌드 CMake ≥ 3.28 + Ninja. Qt는 aqtinstall로 패치 버전 고정, 그 밖의 C/C++ 의존은 CPM(태그·체크섬 고정). vcpkg 안 씀.
- 네임스페이스 `kgc::<층>` (`kgc::app`, `kgc::comm`, `kgc::vehicle`, `kgc::service`, `kgc::ui`).
- 버전은 유의적 버전(`0.0.1`부터), `CMakeLists.txt`의 `project(VERSION)` 한 곳에서 정하고 정보 창·로그·CI 산출물 이름이 그것을 읽는다.

## 3. 폴더와 층

```
k-groundcontrol/
├─ CMakeLists.txt            버전·옵션·하위 폴더 한 곳
├─ CMakePresets.json         linux-debug / linux-release / windows-release
├─ cmake/                    CPM, 배포 스크립트(windeployqt·AppImage), 고지 생성
├─ src/
│  ├─ app/        0층 앱 기반    설정·로그·오류·알림·단위·지형/고도·저장소·주기 작업·i18n·고정값
│  ├─ comm/       1층 통신       링크(시리얼·UDP·TCP·모의)·MAVLink 파서·서명·채널·하트비트·tlog·포트 임대
│  ├─ vehicle/    2층 기체 모델  기체 발견·Vehicle·states(주인)·펌웨어 플러그인(px4/ ardupilot/)·기능 비트마스크
│  ├─ service/    3층 기체 서비스 명령 큐·파라미터·미션·보정·FC 굽기·ESC·ELRS·트래커·AI·구성도·로그 내려받기·영상
│  └─ ui/         4층 화면       QML(화면·패널·부품)과 컨트롤러. 비즈니스 로직 없음
├─ tracker/                  ESP32 스케치(§11), 자기 MODULE.md
├─ i18n/                     번역 파일 .ts/.qm (ko, en)
├─ resources/                아이콘·실루엣 그림·qrc
├─ tools/check/              자동 구조 검사(개발용 Python 3, 배포물 제외)
├─ tests/                    프로토콜 시험 벡터와 단위 테스트(§14 — 승인 사항)
├─ docs/                     THIRD_PARTY_NOTICES 원본, 사용자 안내
├─ specs/                    요구사항·기반·기능 스펙·UI 그림 (사용자가 읽는 문서)
├─ .github/workflows/        linux.yml, windows.yml, check.yml
├─ AGENTS.md                 구현자(GPT)용 코드 규칙, 영어
├─ LICENSE  THIRD_PARTY_NOTICES.md  README.md
└─ .pm/                      PM 작업 파일(저장소 밖, .gitignore)
```

층의 뜻과 include 규칙:

| 층 | 폴더 | 안다 | 모른다 |
|---|---|---|---|
| 0 | `app/` | Qt Core만 | 링크·기체·화면 |
| 1 | `comm/` | app | 기체가 무엇인지(바이트와 메시지만) |
| 2 | `vehicle/` | app, comm | 화면, 서비스의 절차 |
| 3 | `service/` | app, comm, vehicle | 화면 |
| 4 | `ui/` | 전부(공개 헤더만) | — |

같은 층 안에서 모듈끼리는 공개 헤더로만 서로 쓴다. 순환 의존은 금지(검사).

## 4. 설정 — 세 가지를 섞지 않는다

| 종류 | 뜻 | 자리 | 바꾸는 사람 | 저장 |
|---|---|---|---|---|
| **부팅 설정** | 프로그램이 뜨기 전에 정해지는 것: 데이터 폴더, 로그 수준, 디스플레이 옵션, `--sitl` 같은 개발 플래그 | `app/boot/BootOptions` (명령줄 + `kgc.ini`) | 개발자·고급 사용자 | ini 파일 |
| **운영 설정** | 사람이 쓰면서 바꾸는 값: 언어, 단위, 링크 목록, 지도 제공자, 영상 소스, AI 제공자·모델, 길게 누르기 시간, 유도 거리·고도 한계, 트래커 서보 범위… | `app/settings/` — 정의는 `settings/*.settings.json`(이름·형·기본값·범위·열거·카테고리·라벨 키), 값은 `Settings` 클래스가 `QSettings`로 | 사용자(앱 설정 화면, 또는 해당 패널) | QSettings(플랫폼 기본 위치) |
| **고정값** | 바꾸면 안 되는 수: 매직값, 프로토콜 상수, 타임아웃 기본 | `app/constants/<영역>.h` (§6) | 아무도(코드 수정) | 코드 |

- 운영 설정은 **메타데이터 먼저**(R-N17): JSON에 없는 설정은 코드에서 못 만든다. 앱 설정 화면의 검색은 이 JSON을 읽는다. 카테고리는 §13 용어집의 메뉴 이름을 쓴다.
- 비밀값(API 키·서명 키·바인드 문구·WiFi 비밀번호)은 설정 JSON에 **값을 두지 않고** `app/secrets/SecretStore`(Windows Credential Manager / Linux Secret Service)에 둔다. 로그·tlog·내보내기에서 가린다(R-N23).

## 5. 상태(states) — 주인 하나, 나머지는 읽기

- 자리: `vehicle/states/`. 기체마다 `VehicleStates` 하나(값 묶음 구조체들 + Qt 시그널).
- **주인**: `vehicle/VehicleStateOwner`만 쓴다. MAVLink 메시지(1층에서 올라온 것)를 받아 상태를 갱신하고 `changed` 시그널을 낸다. 3층·4층은 **읽기만**(const 참조 또는 시그널 구독). 쓰려면 주인에게 요청하지 않는다 — 쓸 일이 있으면 그것은 상태가 아니라 명령(§9 명령 큐).
- 같은 값을 다시 받아오지 않는다: 어떤 모듈도 자기 캐시로 기체 값을 들고 있지 않는다.
- 초기 상태 묶음(기능 스펙이 더한다):

| 묶음 | 담는 것 | 출처 메시지 |
|---|---|---|
| `Link` | 연결 여부·주 링크·손실률·하트비트 시각·끊김 | HEARTBEAT, RADIO_STATUS, 시퀀스 |
| `Arming` | armed, 비행 모드(펌웨어 모드 id + 표시 이름), 준비 상태, **시동 불가 사유 목록** | HEARTBEAT, SYS_STATUS, EVENT/STATUSTEXT(플러그인이 해석) |
| `Attitude` | roll·pitch·yaw·각속도 | ATTITUDE |
| `Position` | 위도·경도·AMSL·상대 고도·속도·헤딩·홈·GCS 거리 | GLOBAL_POSITION_INT, HOME_POSITION, GPS_RAW_INT |
| `Gps` | fix·위성·HDOP·인스턴스 | GPS_RAW_INT, GPS2_RAW |
| `Battery[]` | 전압·전류·잔량·소모·셀 | BATTERY_STATUS, SYS_STATUS |
| `Sensors` | 센서별 켜짐·건강(나침반·IMU·기압·RC·…) | SYS_STATUS 비트 |
| `Rc` | 채널 값·RSSI | RC_CHANNELS |
| `Flight` | 비행 시간·거리·착륙 상태·VTOL 상태 | EXTENDED_SYS_STATE, 계산 |
| `Mission` | 현재 항목·전체 수·기체의 미션 해시 | MISSION_CURRENT, 미션 서비스 |
| `Messages` | STATUSTEXT 목록(심각도·시각·읽음) | STATUSTEXT, EVENT |
| `Esc[]` | rpm·전압·전류·온도 | ESC_STATUS/INFO, AP ESC_TELEMETRY |

- 트래커·GCS GPS처럼 기체가 아닌 것의 상태는 그 서비스가 자기 `*States`를 같은 규칙(주인 하나, 읽기 전용)으로 갖는다: `service/tracker/TrackerStates`, `app/gcsposition/GcsPositionStates`.

## 6. 고정값과 정해진 값의 이름

- 고정값: `app/constants/` 아래 영역별 헤더 하나씩 — `mavlink.h`(ACK 1200ms·3회, 하트비트 1Hz, 끊김 3500ms), `safety.h`(비상 정지 21196, 강제 시동 2989, 모터 시험 3초, 길게 누르기 기본 500ms), `mission.h`(1500ms·250ms·5회), `esc.h`(19200, CRC 다항식, 청크 64), `elrs.h`(옵션 블록 128+16+512+2048, 보레이트), `tracker.h`. **코드 다른 곳에 숫자 리터럴로 쓰면 검사가 잡는다**(0·1·-1과 단위 변환 상수는 예외 목록 없이 허용 패턴만).
- 정해진 값의 이름(열거): 모듈 공개 헤더의 `enum class`. 펌웨어 모드·회전·서보 기능처럼 **펌웨어가 정하는 열거는 플러그인이 메타데이터에서 읽어** 제공하고 코드에 박지 않는다.
- 로그 사건 이름과 오류 코드는 §10의 등록부 하나에.

## 7. 자료 규격(파일 형식)

| 형식 | 자리(코드) | 문서 | 비고 |
|---|---|---|---|
| 설정 메타데이터 `*.settings.json` | `app/settings/` | `specs/formats/settings.md` | §4 |
| 운영값 저장 | QSettings | — | |
| 미션 `.plan` | `service/mission/PlanFile` | `specs/formats/plan.md` | QGC .plan v1 호환(P-67) |
| 기체 구성도 `.kgclayout.json` + 파라미터 `.params` | `service/layout/LayoutFile` | `specs/formats/layout.md` | R-D36 |
| 텔레메트리 로그 `.tlog` | `comm/tlog/` | `specs/formats/tlog.md` | 8바이트 BE µs + 패킷(C-23) |
| 트래커 프로토콜 | `service/tracker/protocol/` + `tracker/` 공용 헤더 | `specs/formats/tracker-protocol.md` | R-D24, 양쪽이 같은 정의를 include |
| 파라미터 파일 | `service/params/ParamFile` | `specs/formats/params.md` | QGC 형식(S-13) + MP 2열 읽기 |
| 번역 `.ts/.qm` | `i18n/` | — | Qt Linguist |

규격 문서는 기능 스펙과 함께 쓰고, 코드의 읽기·쓰기 함수는 그 모듈 하나에만 있다.

## 8. 펌웨어 플러그인 계층 (R-Q12·R-N4)

- 자리: `vehicle/firmware/FirmwarePlugin.h`(인터페이스) + `vehicle/firmware/px4/`, `vehicle/firmware/ardupilot/`, `vehicle/firmware/generic/`.
- 플러그인이 갖는 것: 기능 비트마스크(`Capability::Orbit`, `Roi`, `GuidedTakeoff`, `MissionFence`, `EscPassthrough`…), 모드 표(id↔표시 이름), 이륙·일시정지·고도 변경·속도 변경의 **명령 순서**, 시동 불가 사유 해석(PX4 events / AP PreArm 텍스트), 파라미터 메타데이터 출처와 이름 바꿈표, 미션 번호 매기기 규칙(PX4 홈 없음), 보정 방식(STATUSTEXT 파싱 / 역방향 명령), 구성도 대응표(R-D33), AI 거부 목록(R-D5 ⑥), 지원 버전 범위(D17) 판정.
- 규칙: `vehicle/firmware/` **밖**에서 `MAV_AUTOPILOT_*`·펌웨어 이름·버전 문자열을 비교하는 코드는 검사가 잡는다. 화면은 비트마스크만 본다.

## 9. 공통 함수·저장소·주기 작업·서비스·입출력

| 종류 | 자리 | 규칙 |
|---|---|---|
| 공통 함수 | `app/util/`(문자열·시간·바이트), `app/geo/Altitude`(AMSL·상대·지형·타원체고↔MSL — **한 함수 묶음**, R-N11), `app/units/`(표시 단위 변환), `app/crc/`(CRC16-XMODEM, CRC-0xA001, CRC8-0xD5 — 시험 벡터 §14) | 두 곳에 같은 함수가 생기면 검사가 잡는다(이름 중복) |
| 저장소 | `app/store/`: SQLite(`QtSql`) — 지도 타일 캐시, 파라미터 메타데이터 캐시, 펌웨어 다운로드 캐시(해시 포함). 파일 저장은 `app/paths/Paths`가 정한 폴더 아래만 | SQL 문장은 `app/store/` 밖에 없다(검사) |
| 주기 작업 | `app/scheduler/Scheduler`에 등록(이름·주기·소유 모듈). 초기 목록: GCS 하트비트 1Hz, 끊김 검사 1Hz, 시리얼 포트 목록 1Hz, 트래커 각도 5Hz, tlog 플러시 | `QTimer`를 모듈이 직접 만들지 않는다(검사) |
| 명령 큐 | `service/command/CommandQueue`: COMMAND_LONG/INT·ACK·재시도·타임아웃(§6). 기체에 명령을 보내는 유일한 문. 예외 하나: 보정 시작(ACK가 분 단위라 큐 밖, S-39) | 다른 모듈이 `mavlink_msg_command_*`를 직접 만들면 검사가 잡는다 |
| 서비스 | `service/<이름>/<Name>Service` 하나 + 공개 헤더 + MODULE.md. 초기 목록: command, params, mission, calibration, firmware(FC 굽기), esc, elrs, tracker, ai, layout(구성도), logs(내려받기·뷰어), video, gcsposition, notify(→ app) | 서비스는 다른 서비스를 공개 헤더로 쓴다. 화면은 모른다 |
| 포트 임대 | `comm/lease/PortLease`: R-Q1a (가)형의 임대, (나)형의 기체 작업 잠금(`VehicleWorkLock`) | FC 굽기·ESC·ELRS·트래커 굽기는 임대 없이 포트를 열 수 없다 |
| 입출력(화면) | `ui/`: 화면마다 폴더(`fly/`, `menu/<메뉴>/`, `sidebar/`, `topbar/`, `bottombar/`, `ai/`, `notify/`), QML + `*Controller`(Q_PROPERTY·Q_INVOKABLE). 컨트롤러는 3층 서비스와 2층 states를 **읽고 호출만**, 계산·프로토콜·파싱 없음 | QML에서 하드코딩 문자열(qsTr 없이)·숫자 상수는 검사가 잡는다 |
| 스레드 | 링크 I/O·MAVLink 파싱은 링크마다 워커 스레드, 영상 디코드·타일 로드도 워커. 2층 states 갱신은 **GUI 스레드에서**(큐드 시그널). 3·4층은 GUI 스레드 | R-N15 |

## 10. 로그·오류·알림

- **로그**: Qt 로깅 카테고리를 층·모듈 이름으로(`kgc.comm.serial`, `kgc.service.esc`). 메시지는 **한국어 명사형 전보체**(AGENTS.md 규칙), 값은 콜론 뒤, 절은 쉼표. 사건 이름은 영어 식별자로 `app/log/events.h` 등록부에 하나씩(`LinkLost`, `ParamWriteFailed`…) — 등록부에 없는 이름은 검사가 잡는다. 파일 회전(크기·개수는 부팅 설정). 비밀값 마스킹은 `app/log/`에서 한 번.
- **오류**: `app/errors/ErrorCode`(영어 식별자, 등록부 `errors.h`) + 사용자 문구(한국어, 번역 대상)는 `i18n`. 함수는 예외 대신 `Result<T>`(값 또는 오류 코드)를 돌려준다. 예외는 프로그램 버그에만.
- **알림**: `app/notify/Notify`: 수준 `Info / Warning / Critical`, 출처(기체·트래커·GCS·앱), 시각, 읽음. 상단 벨 목록과 토스트가 이것을 읽는다. 기체 STATUSTEXT는 2층 `Messages` 상태에 있고, 알림으로 올릴지는 수준 규칙(Critical 이상)으로 `vehicle/`이 정한다. 음성 안내(TTS)는 `app/notify/Speech`가 같은 입구에서 받는다.

## 11. 트래커 스케치 `tracker/` (R-D27)

- 저장소 안 `tracker/`에 둔다. 자기 `MODULE.md`. 빌드 도구는 PlatformIO 또는 ESP-IDF — **미확인(요구사항 10절), 승인 때 정한다**. CI가 같은 태그에서 변종별 이미지를 만들어 설치본에 넣는다.
- GCS와 **같은 프로토콜 정의**를 쓴다: `tracker/protocol/` 안의 C 헤더 하나를 GCS `service/tracker/protocol/`이 include한다(복사본 금지).
- 구조 검사의 대상이다(파일·함수 길이). 층 규칙은 해당 없음(별도 프로그램).

## 12. 모듈 설명서 `MODULE.md` 틀

```
# <모듈 이름> (<층>)
역할: 한 문장.
받는 것: 어떤 입력·시그널·상태를 읽는가(모듈 이름으로).
주는 것: 공개 헤더가 내놓는 클래스·함수·시그널.
의존: 가져다 쓰는 모듈(공개 헤더 이름).
안 하는 것: 이 모듈이 일부러 하지 않는 일(어디서 하는지).
흐름(사용자가 따라가는 진입점이 있으면): 1. … 2. …
```
- 100줄을 넘지 않는다. 바뀐 코드와 같은 커밋.

## 13. 용어집 (한국어 ↔ 식별자)

| 한국어 | 식별자 | 뜻 |
|---|---|---|
| 기체 | `Vehicle` | MAVLink 시스템 하나(자동조종기 컴포넌트) |
| 링크 | `Link` | 시리얼·UDP·TCP 연결 하나 |
| 주 링크 | `PrimaryLink` | 명령이 나가는 링크 |
| 상태 | `States` | §5 읽기 전용 값 묶음 |
| 명령 큐 | `CommandQueue` | §9 |
| 펌웨어 플러그인 | `FirmwarePlugin` | §8 |
| 기능 비트마스크 | `Capability` | 플러그인이 알려 주는 지원 여부 |
| 파라미터 | `Param` | 기체 파라미터 |
| 묶음(계열) | `ParamGroup` | R-D1 계열별 묶음 |
| 작업 단위 묶음 | `TaskSet` | R-D3 |
| 미션 | `Mission` | 웨이포인트 목록(.plan) |
| 유도 명령 | `GuidedAction` | 지점 이동·고도·선회·ROI |
| 시동 / 시동 해제 | `Arm` / `Disarm` | |
| 비상 정지 | `EmergencyStop` | 21196 |
| 길게 눌러 확인 | `HoldToConfirm` | R-N8 |
| 포트 임대 | `PortLease` | R-Q1a (가) |
| 기체 작업 잠금 | `VehicleWorkLock` | R-Q1a (나) |
| 기체 구성도 | `Layout` | 5.5절 |
| 부품 표식 | `LayoutMarker` | |
| 안테나 트래커 | `Tracker` | 5.4절 |
| 받침 정면 방위 | `BaseHeading` | R-D25 |
| 패널 | `Panel` | 좌 메뉴가 가운데에 띄우는 화면 |
| 메뉴 | `Menu` | 좌 사이드바 항목 |
| 알림 | `Notification` | §10 |
| 기체 메시지 | `VehicleMessage` | STATUSTEXT |
| 설정(운영) | `Setting` | §4 |
| 부팅 설정 | `BootOption` | §4 |
| 고정값 | `Constant` | §6 |

## 14. 자동 검사 `tools/check/` (R-N21) — 커밋 훅과 CI에서 같은 스크립트

| 검사 | 규칙 |
|---|---|
| include 방향 | 위층→아래층만, 같은 층은 공개 헤더만, `internal/` 밖에서 include 금지, 순환 금지 |
| 크기 | 파일 400 / 함수 60 / QML 300줄 |
| MODULE.md | 모듈 폴더마다 있고 100줄 이하, 바뀐 모듈의 MODULE.md가 같은 커밋에 있는지 |
| 고정값 | `app/constants/` 밖의 숫자 리터럴(허용 패턴 밖) |
| 펌웨어 분기 | `vehicle/firmware/` 밖의 `MAV_AUTOPILOT_`·펌웨어 이름 비교 |
| SQL | `app/store/` 밖의 SQL 문자열 |
| 타이머 | `app/scheduler/` 밖의 `QTimer` 생성 |
| 명령 | `service/command/` 밖의 `mavlink_msg_command_*` |
| 문자열 | QML 하드코딩 문자열(qsTr 없이), C++ 사용자 문구의 `tr()` 누락 |
| 로그·오류 | 등록부에 없는 사건 이름·오류 코드 |
| 이름 중복 | 공개 헤더에 같은 이름의 함수가 두 모듈에 |
| 비밀값 | 로그 호출 인자에 `key`·`token`·`phrase`·`password` 변수(마스킹 함수 안 거친 것) |

- **승인 사항(§14-1)**: 테스트 코드의 자리. 코딩팀 규칙은 "테스트는 저장소에 넣지 않음(tester가 .pm에서)"이지만, 요구사항 R-D13은 CRC·프레임 시험 벡터를 **자동 테스트로 지키라**고 하고 CI(R-N19)가 그것을 돌려야 한다. 제안: **프로토콜 시험 벡터와 순수 함수 단위 테스트만 `tests/`에**(제품 품질의 일부), 기능의 확인 항목 테스트는 지금처럼 `.pm/tests/`에.

## 15. 빌드·CI·배포 (R-N18~R-N22)

- `.github/workflows/check.yml`(구조 검사·`tests/`), `linux.yml`(Ubuntu 24.04, AppImage), `windows.yml`(MSVC 2022, Qt 6.12.x aqtinstall, zip). 태그에서 Release.
- 산출물에 `LICENSE`, `THIRD_PARTY_NOTICES.md`(CPM 의존 목록에서 CI가 생성), 소스 위치. SBOM은 0.1.x부터.
- 0.0.1 산출물: Windows zip(windeployqt·MSVC 런타임) + AppImage. GStreamer·NSIS는 영상 버전부터.

## 16. 이 문서가 정하지 않는 것

- 각 기능의 동작(기능 스펙), 화면의 모양(`specs/ui/`), 바이트 명세(규격 문서), 테스트 내용.
- 성능 수치 목표(기능 스펙에서 필요할 때).

## 17. 승인 사항 요약 (사용자가 정할 것)

1. 층·폴더 이름(§3)과 네임스페이스.
2. 크기 상한 수치(파일 400 / 함수 60 / QML 300).
3. 테스트 코드의 자리(§14-1).
4. 트래커 빌드 도구(§11, PlatformIO vs ESP-IDF — 미확인 항목).
5. 설정 저장소로 QSettings(플랫폼 기본) 쓰기.

## 18. 검토·승인 기록

| 날짜 | 누가 | 무엇 |
|---|---|---|
| | reviewer | 설계 검토 (대기) |
| | 사용자 | 승인 (대기) |
