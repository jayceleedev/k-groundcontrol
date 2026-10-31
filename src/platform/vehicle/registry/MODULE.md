# vehicle/registry (바탕 2층)
역할: 수신한 하트비트로 기체 수명과 활성 기체를 정해요.
수명: 전역 VehicleRegistry와 기체별 Vehicle.
받는 것: LinkManager.messageReceived·linkClosed, Scheduler.
주는 것: VehicleRegistry.active·vehicleAdded·vehicleRemoved·activeChanged, Vehicle.states·linkConfig, VehicleSelection. 기체 포인터의 소유권은 등록부에 남아요.
기여: 없음.
의존: app/errors, comm/links, vehicle/firmware, vehicle/states, vehicle/outbound, Qt Core.
안 하는 것: 화면 표시와 사용자 명령은 기능 모듈이 맡아요.
