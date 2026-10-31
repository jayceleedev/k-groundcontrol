# fcFlash (기능 순위 5)
역할: PX4·ArduPilot 부트로더가 있는 FC에 펌웨어를 굽고 재연결로 확인해요.
수명: 전역.
받는 것: LinkManager의 포트 목록과 링크 시그널, VehicleRegistry의 기체와 Capabilities 묶음, ArmedPolicy 판정, resources/data/fcflash/px4-boards.json.
주는 것: FcFlashService, FlashStates(Step·Phase·FirmwareSource), FcFlashError, errorText, registerUi.
기여: 좌 메뉴 1번 펌웨어 세팅 패널(묶음 1).
의존: app/errors, app/log, app/crc, app/job, app/net, app/paths, app/scheduler, app/constants, comm/rawport, comm/links, vehicle/registry, vehicle/safety, vehicle/command, ui.
안 하는 것: DFU, Betaflight, SiK, 부트로더 업데이트, 파라미터 백업과 복원, 다운로드 캐시 재사용, 자동 연결은 하지 않아요.
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
