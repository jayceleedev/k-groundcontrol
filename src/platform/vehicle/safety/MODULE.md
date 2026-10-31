# vehicle/safety (바탕 2층)
역할: 작업을 시작해도 되는지 시동 상태로 판정해요.
수명: 전역 ArmedPolicy 하나(Composition이 VehicleRegistry 뒤에 만들어요).
받는 것: VehicleRegistry의 기체 목록, 각 기체의 Arming 묶음.
주는 것: ArmedPolicy, Decision, decide, VehicleScope.
기여: 없음.
의존: app/errors, app/log, app/job(OperationKind), vehicle/registry.
안 하는 것: WorkLock·RebootWindow·JobArmWatch·ArmGuard는 쓸 곳이 생기는 버전에 더해요. 거부를 화면에 보이는 일은 기능 모듈이 맡아요.
