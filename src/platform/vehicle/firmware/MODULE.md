# vehicle/firmware (바탕 2층)
역할: 펌웨어 차이를 데이터와 순수 함수로 제공해요.
수명: 기체별 플러그인, Vehicle이 소유해요.
받는 것: HEARTBEAT의 autopilot·custom_mode·type, EXTENDED_SYS_STATE의 vtol_state(부르는 쪽이 넘김), 펌웨어 버전 글, 파라미터 값(부르는 쪽이 넘김).
주는 것: FirmwarePluginFactory, FirmwarePlugin의 displayName·modeName·streamRequests·paramEncoding·paramMetadataSource·paramGroupTitle(첫 단계 가지의 접두어 → 묶음 제목)·paramFileSkip, StreamRequest, ParamEncoding, ParamMetadataFormat, ParamMetadataSource, 그리고 0.6.0 비행 데이터로 형 VehicleClass(Unknown·Multicopter·FixedWing·Vtol·Other)·FlightShape(Hover·Forward)·TakeoffKind(None·WithAltitude·FixedAltitude)·CommandForm(Long·Int)·CommandStep(form·command·param1~7·frame·x·y·z·confirmModes·confirmArmed)·FlightMode(customMode·name·pilotControlled)·MessageInterval(messageId·intervalUs)·LoiterCircle(show·radiusM·clockwise), 자유 함수 vehicleClassOf(vtol_state 1~4는 고정익 type에서만 Vtol, 멀티콥터 종류는 vtol_state와 상관없이 Multicopter — QGC 규칙)·flightShapeOf, 플러그인 함수 messageIntervals·vtolStateDecidesClass·launchOnArmModes·flightModes(modeName과 양방향 같은 글자)·guidedMode·setModeSteps(기체 종류를 받음)·armSteps·disarmSteps·emergencyStopSteps·takeoffKind·takeoffAltitudeParam·takeoffAltitudeFrame·takeoffSteps·landSteps·rtlSteps·pauseSteps(펌웨어 버전을 받음 — PX4 1.12 전은 빈 목록)·goToAltitudeFrame·goToSteps·loiterRadiusParam·vtolHoverParam·loitersAtTarget·loiterCircle. 빈 단계 목록은 지원 안 함(단추 숨김)이에요.
ArduPilot modeName: type이 GENERIC이거나 멀티콥터 종류면 Copter 표, 고정익·VTOL 종류면 Plane 표(0~26), 그 밖은 빈 이름이에요.
내부: Steps(armStep·modeStep·repositionStep·px4Mode), VehicleClass.cpp, ardupilot/ArduPilotModes.cpp(모드 표)·ArduPilotFly.cpp, px4/Px4Fly.cpp, Constants.h(kCoordinateScale·kExtendedSysStateIntervalUs·kLoiterRadiusDefaultM·kLoiterRadiusSmallM·kPx4KeepLatLonMajor·kPx4KeepLatLonMinor).
기여: 없음.
의존: app/errors, app/geo(AltitudeFrame), app/constants(kEmergencyStopMagic), comm/mavlink, Qt Core.
안 하는 것: 송신과 상태 쓰기는 하지 않아요(단계는 기능 모듈이 실행해요). 모르는 모드는 빈 이름, 모르는 자동조종기는 범용 플러그인으로 제공해요.
