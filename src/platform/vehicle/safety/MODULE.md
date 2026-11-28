# vehicle/safety (바탕 2층)
역할: 작업을 시작해도 되는지 시동 상태로 판정하고, 유도 비행 한계 설정을 갖고 판정해요.
수명: 전역 ArmedPolicy 하나(Composition이 VehicleRegistry 뒤에 만들어요).
받는 것: VehicleRegistry의 기체 목록, 각 기체의 Arming 묶음, 자기 운영 설정 셋.
주는 것: ArmedPolicy, Decision, decide, VehicleScope, GuidedLimits(maxDistanceM·minAltitudeM·maxAltitudeM)·guidedLimits(최저가 최고보다 크면 바꿈)·GuidedCheck(Ok·TooFar·TooLow·TooHigh)·checkGuided(거리 먼저, NaN은 한계 밖, 경계값 Ok), 운영 설정 safety.guidedMaxDistanceM(기본 1000 m)·safety.guidedMinAltitudeM(2 m)·safety.guidedMaxAltitudeM(121.92 m) — 모듈 id 한 낱말 safety, 생성 접근자 kgc::safety::settings(`<kgc/safety/Settings.h>`).
기여: 없음.
의존: app/errors, app/log, app/job(OperationKind), app/settings(생성 접근자), vehicle/registry.
안 하는 것: WorkLock·RebootWindow·JobArmWatch·ArmGuard는 쓸 곳이 생기는 버전에 더해요. 거부를 화면에 보이는 일은 기능 모듈이 맡아요.
