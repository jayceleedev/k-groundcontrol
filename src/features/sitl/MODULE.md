# sitl (기능 순위 5)
역할: ArduPilot SITL을 내려받아 이 PC에서 실행하고 앱의 UDP 14550 링크로 바로 붙여요.
수명: 전역. Composition의 sitlLifetime(LinkManager·Http보다 먼저 지워지는 수명 객체)이 SitlService의 부모예요. 컨트롤러는 서비스가 소유하는 형이 있는 싱글턴이에요.
받는 것: LinkManager의 linkId·linkConfig·linkOpened·linkFailed·linkClosed, Http::download, Paths::cacheSitl·sitl, 설정 sitl.vehicle·sitl.release·sitl.homeLatitude·sitl.homeLongitude·sitl.homeAltitude·sitl.homeHeading·sitl.speedup, QCoreApplication::aboutToQuit.
주는 것: SitlService(state·job·cached·cachedAt·linkOpenedBySitl·linkBusy·lastExitCode·output·lastError·start·stop·redownload·reconnect와 시그널 stateChanged·jobChanged·outputChanged·linkOpenedBySitlChanged·linkBusyChanged·cacheChanged), SitlTypes(QML 이름 Sitl: SitlVehicle·SitlRelease·SitlState·SitlHomeField, QML에 내지 않는 SitlPlatform, SitlHome·SitlFile), SitlError, errorText, registerUi, ModuleId 112.
기여: 좌 메뉴 2번 시뮬레이션 패널(묶음 1, fcFlash 뒤).
의존: app/errors, app/job, app/log, app/net, app/paths, app/scheduler, app/settings(생성 접근자), comm/links, ui.
안 하는 것: PX4 SITL, 여러 대 동시 실행, 지도에서 홈 고르기, 추가 명령줄, TCP 링크, 내려받은 파일의 해시 확인, 프레임 고르기, 조종기 입력, Windows 방화벽 설정은 하지 않아요. 기체로 아무것도 보내지 않아 ArmedPolicy를 부르지 않아요(Read는 늘 Allow). 링크 화면 표시는 link가 맡아요.
다른 프로그램 실행: internal/Simulator만 QProcess를 쥐어요(tools/check). 자식 환경은 건드리지 않고, 표준 출력과 오류를 합쳐 줄 단위로 받아요.
순수 함수(tests/): SitlSources의 sitlFiles·sitlFolderName·sitlModel·sitlRunFolderName, SitlCommand의 sitlArguments·checkHome과 설정 열거 변환.
흐름(시작):
1. SitlController.startOrStop → SitlService::start — Busy·BadHome·LinkBusy를 보고 설정 일곱 값을 저장해요.
2. SitlDownload — 캐시가 없으면 Job(Read, 단계 "내려받기")으로 Http::download를 차례로 불러 cache/sitl/.part-<무작위>/에 받은 뒤 <폴더 이름>/과 바꿔요.
3. launch — sitl/<기체>/를 만들고 defaults.parm을 덮어 복사한 뒤 Simulator::start로 sitlArguments를 넘겨요.
4. Simulator.started → LinkManager::connectLink(UDP) — linkOpened면 Running, linkFailed면 프로세스를 멈추고 Failed(LinkFailed)예요.
흐름(정지): 1. SitlService::stop — 내려받는 중이면 Job을 취소하고, 아니면 이 서비스의 링크를 끊고 Simulator::stop(kill 뒤 3초 대기)으로 Idle이에요.
실행 중: 이 서비스가 연 링크가 닫혀도 프로세스는 두고, reconnect로 다시 열어요. 실행 중에 상단 단추가 연 UDP 링크도 이 서비스의 것으로 봐요. 요청하지 않은 종료는 링크를 끊고 Failed(Crashed)예요.
출력: 마지막 200줄만 들고, 줄 끝 \r을 지우고, outputChanged는 200ms Timeout으로 묶어 내요. 출력 줄은 앱 로그에 넣지 않아요.
앱 종료: aboutToQuit에서 stop을 부르고, 소멸자는 LinkManager를 부르지 않고 프로세스만 멈춰요.
