# fcFlash (기능 순위 5)
역할: PX4·ArduPilot 부트로더가 있는 FC에 펌웨어를 굽고 재연결로 확인해요.
수명: 전역.
받는 것: LinkManager의 포트 목록과 링크 시그널, PortInfo(usbName 포함), VehicleRegistry의 기체와 Capabilities 묶음, ArmedPolicy 판정, resources/data/fcflash/px4-boards.json.
주는 것: FcFlashService(ports·portsChanged, vehicleFirmwareName, flashStage, usbName, matchKind, matchedCandidates, otherBoards), FlashStates(Step·Phase·FirmwareSource·BoardMatch), FcFlashError, errorText, registerUi.
기여: 좌 메뉴 1번 펌웨어 세팅 패널(묶음 1).
의존: app/errors, app/log, app/crc, app/job, app/net, app/paths, app/scheduler, app/constants, comm/rawport, comm/links, vehicle/registry, vehicle/safety, vehicle/command, ui.
안 하는 것: DFU, Betaflight, SiK, 부트로더 업데이트, 파라미터 백업과 복원, 다운로드 캐시 재사용, 자동 연결은 하지 않아요.
화면: FcFlashController가 서비스의 읽기 값과 요청을 형이 있는 싱글턴으로 전달해요. 고르기 카드는 Kgc.Ui의 ChoiceCard를 써요. "다른 보드 ID도 보기"(showOtherBoards)는 화면 상태라 컨트롤러에만 두고, 검색어는 QML 안에만 둬요.
스레드: FlashWorker가 자기 스레드에서 RawPort와 Bootloader를 쥐어요. 임대는 GUI 스레드의 서비스가 쥐어요.
흐름(보드 확인):
1. FcFlashController → FcFlashService::beginProbe — 시동·연결 조건을 보고 고른 포트를 확인해요.
2. FlashWorker — 임대한 포트에 sync·identify를 해요.
3. CommandQueue — 필요하면 부트로더 재부팅 명령을 보내요.
4. fcFlashScan — 새 포트와 돌아온 포트를 찾아 Ready로 가요.
흐름(굽기):
1. FcFlashController → FcFlashService::startFlash — 새 Job을 만들어요.
2. Http::download — 공식 출처만 내려받아요.
3. FlashWorker → Bootloader::flashImage — 외부, 내부 순서로 지우기·쓰기·검증을 해요.
4. Bootloader::boot — 검증이 성공했을 때만 부팅해요.
5. LinkManager::connectLink — 다시 연결해 종류와 버전을 비교해요.
선택 변경: 진행 중인 요청을 취소하고 선택 번호로 늦은 해석 결과를 버려요. 두 공식 목록 모두 작업 스레드에서 해석해요.
재시도: sync·identify로 확인한 뒤, 이미지가 없으면 내려받기부터 다시 해요. 호환·파일 오류는 재시도하지 않아요.
재연결: 기체가 나타나면 60초 제한을 멈추고 버전 확인을 기다려요.
보드 이름 맞추기(2단계):
1. ready — 보드 확인 때 LinkManager::portInfo로 usbName을 같이 읽어요.
2. rebuildCandidates — 후보마다 맞춰 볼 이름(ArduPilot은 platform과 bootloader_str, PX4는 px4-boards.json의 names)을 모아요.
3. recognize → preselect — NoName·Matched·NoMatch를 내고, 하나만 맞으면 그 후보를 골라 둬요. 보드 255의 PX4는 미리 고르지 않아요.
4. manifestOtherBoards·px4OtherBoards — 보드 ID가 다른 빌드를 회색 행용 목록으로 내요. 고를 수 없어요.
5. BoardMatched 로그 — (종류, 맞은 수, usbName)이 마지막으로 남긴 값과 다를 때만 한 줄 남겨요.
