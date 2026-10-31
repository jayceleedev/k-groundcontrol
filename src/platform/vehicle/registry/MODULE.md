# vehicle/registry (바탕 2층)
역할: 수신한 하트비트로 기체 수명과 활성 기체를 정해요.
수명: 전역 VehicleRegistry와 기체별 Vehicle.
받는 것: LinkManager.messageReceived·linkClosed, Scheduler.
주는 것: VehicleRegistry.active·vehicleAdded·vehicleRemoved·activeChanged, Vehicle.states·linkConfig·commands, VehicleSelection::accepts(sysid, compid, autopilot, mavType). 기체 포인터의 소유권은 등록부에 남아요.
기여: 없음.
의존: app/errors, comm/links, vehicle/firmware, vehicle/states, vehicle/outbound, vehicle/command, Qt Core.
안 하는 것: 화면 표시와 사용자 명령은 기능 모듈이 맡아요.
발견 조건: sysid 0·GCS·짐벌·컴패니언·ADS-B를 거르고 유효한 compid 1 자동조종기만 받아요.
명령 수명: 생성 때 CommandQueue로 AUTOPILOT_VERSION을 재시도 없이 한 번 요청해요. consume이 COMMAND_ACK를 큐에 넘기고, stop은 큐를 Gone으로 끝낸 뒤 송신 문을 지워요.
