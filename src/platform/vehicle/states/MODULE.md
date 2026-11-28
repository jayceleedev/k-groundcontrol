# vehicle/states (바탕 2층)
역할: 기체 값을 하나의 소유자만 갱신하도록 지켜요.
수명: 기체별.
받는 것: 등록부가 거른 자동조종기(compid 1)의 모든 MAVLink 메시지(HOME_POSITION 포함 — 등록부가 기체를 만들 때 한 번 요청하고 기체가 스스로 보내는 것도 받아요; EXTENDED_SYS_STATE·POSITION_TARGET_GLOBAL_INT·ORBIT_EXECUTION_STATUS·STATUSTEXT도), FirmwarePlugin, Scheduler.
주는 것: VehicleStates의 아홉 읽기 전용 묶음과 변경 시그널, 기체 메시지 신호 vehicleMessage(VehicleMessage — STATUSTEXT 하나마다 한 번, 목록 없음), VehicleStateOwner, isLinkLost. FlightBundle(landedState·vtolState, MAVLink 값 그대로; 받기 전과 5초 안 오거나 링크가 끊기면 −1)·NavTargetBundle(POSITION_TARGET_GLOBAL_INT의 목표 셋과 ORBIT_EXECUTION_STATUS의 선회 셋, 3초 안 오면 NaN; targetCount는 목표 메시지를 받은 수). LinkBundle.lastMessageMs는 모든 수신으로 갱신하고 linkChanged는 하트비트 수신과 끊김 변화 때만 알려요. 공개 상태 헤더는 방언 헤더 없이 전방 선언만 써요. CapabilitiesBundle.firmwareVersion은 AUTOPILOT_VERSION에서 와요. PositionBundle의 홈 필드 homeLatDeg·homeLonDeg·homeAltAmslM(처음 NaN)은 HOME_POSITION에서만 바뀌고, GLOBAL_POSITION_INT는 나머지 일곱 필드를 덮어쓰며 모르는 헤딩은 NaN이에요.
기여: 없음.
의존: app/errors, app/constants, app/geo(isKnownPosition, 위치 거르기), app/scheduler, vehicle/firmware, comm/mavlink, Qt Core.
만료: 1Hz 작업 linkLoss.<sysid>가 끊김을 보고(끊기면 비행 상태를 비움), 비행 상태(kFlightStale 5초)와 항법 목표·선회(kNavTargetStale 3초)를 받은 때 기준으로 비워요. 비울 것이 있을 때만 알려요.
위치 거르기(0.6.0 §12): GLOBAL_POSITION_INT는 updatePosition 뒤 gatePosition을 거쳐요 — GPS fix_type이 kPositionFixMin(3, 3D) 이상이거나 홈을 알 때만 위도·경도를 채우고, 아니면 위도·경도만 NaN이에요(GPS 고정 전 (0, 0) 근처 값을 버림; 고도·속도·방향·홈은 그대로).
안 하는 것: 화면 표시와 사용자 명령은 기능 모듈이 맡아요.
