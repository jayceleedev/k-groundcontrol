# fly (기능 순위 3)
역할: 상단바 비행 단추(모드·시동·이륙·착륙·RTL·일시정지·비상 정지)로 활성 기체에 비행 명령을 보내고, 여기로 가기 목표를 들고 확인해요.
수명: 기체별(FlyService, 부모는 Vehicle; Composition이 vehicleAdded에서 ParamService 다음에 만들어요) + 화면 기여는 앱 시작 때 한 번. 컨트롤러는 기체 등록부가 소유하는 형이 있는 싱글턴이고 activeChanged로 다시 묶어요.
받는 것: VehicleStates의 link·arming·position·capabilities(vehicleType·firmwareVersion)·flight(landedState·vtolState)·navTarget(targetCount·선회)와 vehicleMessage, FirmwarePlugin의 비행 데이터(종류 판정·모드 목록·명령 단계·고도 틀·선회 원·파라미터 이름 셋 TKOFF_ALT/WP_LOITER_RAD/Q_GUIDED_MODE·NAV_LOITER_RAD), 같은 기체 ParamService의 Params 값, 설정 safety.guidedMaxDistanceM·guidedMinAltitudeM·guidedMaxAltitudeM(GuidedLimits), ui.holdToConfirmMs(HoldToConfirm), ArmedPolicy 판정(FlyAction), 지도 부품의 MapPane.tapped(좌표, 화면 점)·follow·map(레이어의 mapPane으로).
주는 것: FlyService(of·vehicle·capabilities·readiness·busy·arm·disarm·emergencyStop·takeoff·land·rtl·pause·setMode·checkGoTo·goTo·distanceToM·target·loiter와 신호 capabilitiesChanged·readinessChanged·targetChanged·loiterChanged·actionFinished), FlyTypes(FlyActionKind·GoToState·GoToTarget·FlyCapabilities·FlyReadiness), FlyError·make·errorText(문맥 "Fly"), registerUi, ModuleId 114. QML 모듈 Kgc.Fly의 FlyController(앱 설정 "조작" 칸이 유도 한계 셋과 범위를 읽고 써요).
기여: 상단바 가운데 여덟(fly.vehicle 1·묶음 1, fly.mode 2·2, fly.arm 3·2, fly.takeoff 4·3, fly.land 5·3, fly.rtl 6·3, fly.pause 7·3, fly.emergencyStop 8·4). 지원하지 않는 takeoff·land·rtl·pause는 TopbarSlot.setHidden으로 숨겨요. MapLayerSlot에 flyTarget(25, FlyTargetLayer — 목표 표식·"목표 50 m"·선회 원·"반지름 80 m", 홈 20과 기체 30 사이)·flyGoTo(90, FlyGoToLayer — 여기로 가기 창 FlyGoToWindow와 점선 미리 보기, 지도 부품마다 따로).
의존: app/errors, app/geo(distanceMeters·isKnownPosition·altitudeIn·heightIn), app/job(OperationKind), app/log, app/scheduler(Timeout flyConfirm·flyTargetConfirm), comm/mavlink, vehicle/registry, vehicle/firmware, vehicle/states, vehicle/command(CommandLong·CommandInt·preempt), vehicle/outbound(OutboundKind), vehicle/safety(ArmedPolicy·GuidedLimits), ui(TopbarSlot·MapLayerSlot·HoldToConfirm·ActionButton·NumberBox·Toast·TimerBox·BusySpinner·HintTip), Qt Positioning(좌표)·레이어 QML의 QtLocation(지도 부품은 map이 레이어의 mapPane으로 넘겨요, 링크 없음), params(파라미터 값 읽기만).
안 하는 것: 명령 단계와 모드 번호를 만들지 않아요(펌웨어 플러그인의 데이터를 실행만 해요). 기체 메시지 목록·알림·벨·TTS는 Notify 버전에서, 작업 잠금·재부팅 구간은 vehicle/safety 그 버전에서 해요. 파라미터를 요청하거나 쓰지 않아요. 실패 토스트는 Notify가 생길 때까지 상단바 기체 글(FlyVehicle)이 직접 보여요. 성공은 알리지 않아요.
순수 함수(tests/): FlyChecks의 readinessOf·modeChoosable·isReason, StepPacking의 toCommandLong·toCommandInt·outboundKindOf·errorOf, FlyText의 distanceText·lengthNumber·positionText·actionName·flyErrorName.
흐름(비상 정지·시동 끄기):
1. FlyEmergencyStop·FlyArm 길게 누르기 → FlyController.emergencyStop·disarm — 누르기 시작한 기체가 아직 활성인지 봐요.
2. FlyService::emergencyStop·disarm — readiness와 ArmedPolicy::check를 보고 플러그인 단계를 받아요.
3. FlyRunner::start(preempt) — 진행 중 실행을 Preempted로 끝내고 CommandQueue에 끼어들어 바로 보내요.
4. ACK가 오면(받아들임이든 거부든) 3초(kConfirmTimeout) 안 HEARTBEAT 시동 상태가 꺼지면 성공이고, 아니면 ArmNotChanged(받아들임)나 그 ACK의 오류(거부)예요.
5. ACK가 끝내 오지 않으면 큐가 재시도 뒤 포기한 때(약 4.8초) HEARTBEAT가 이미 꺼짐이면 성공, 아니면 NoAck예요. NoLink·Gone은 곧바로 실패예요.
흐름(이륙·착륙·RTL·일시정지·모드):
1. 상단바 단추·창의 길게 누르기 → FlyController — 누르기 시작한 기체가 아직 활성인지 봐요.
2. FlyService — 준비·바쁨·정책·단계를 보고(실패는 actionFinished로 큐를 거쳐), 이륙은 높이를 heightIn으로 바꿔요.
3. FlyRunner — 단계마다 ACK를 받고 HEARTBEAT 모드·시동 확인(3초)을 기다린 뒤 다음 단계를 보내요.
4. FlyService.actionFinished → FlyController.toast — 실패면 "<동작>. <오류>"(+ "기체: …") 토스트예요.
흐름(여기로 가기):
1. 지도 한 번 누르기 → MapPane.tapped → FlyGoToLayer.openAt — canGoTo면 그 자리에 창과 점선 미리 보기를 열고 그 지도 부품의 따라가기를 꺼요.
   FlyGoToWindow.prepare가 뒤집기를 정하고, 새로 열 때만 고도 칸(기체 지금 고도)과 기체 sysid를 정해요. 열린 채 다시 누르면 창만 옮겨요.
2. flyGoToConfirm(길게 누르기) → FlyController.goTo → FlyService::goTo — checkGoTo(준비·좌표·거리·고도 한계·홈 고도)를 봐요.
3. altitudeIn(goToAltitudeFrame) → FirmwarePlugin::goToSteps — DO_REPOSITION COMMAND_INT 단계를 받아요.
4. GoToTracker::begin — 목표를 Sending(FlyTargetLayer의 점선 표식, 모든 지도 부품)으로 두고 FlyRunner로 보내요. 창은 닫혀요.
5. ACK·유도 모드 확인 → GoToTracker::accept — 실선 표식과 "목표 50 m"(고정익 모양이면 선회 원)이고, 5초 안 POSITION_TARGET_GLOBAL_INT가 10 m 안이면 Confirmed예요.
   모드가 안 바뀌면 GoToTracker::watch로 5초 더 보고, 기체 목표가 맞으면 TargetWithoutMode 안내만 보여요.
6. 거부·실패면 앞 목표를 되살리거나 지우고, 모드 바뀜·시동 꺼짐·착륙·바뀐 목표·다른 동작 성공이면 지워요.
