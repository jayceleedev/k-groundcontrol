# vehicle/outbound (바탕 2층)
역할: 기체로 보내는 모든 프레임을 하나의 문으로 통과시켜요.
수명: 기체별.
받는 것: LinkManager, Scheduler, FirmwarePlugin.streamRequests, 대상 sysid·linkId.
주는 것: VehicleSender::send·sendStreamRequests, OutboundKind, NoLink. send의 성공은 전송 완료가 아니라 워커 송신 예약을 뜻해요.
기여: 없음.
의존: app/errors, app/constants, app/scheduler, comm/links, vehicle/firmware, Qt Core.
안 하는 것: 화면 표시와 사용자 명령은 기능 모듈이 맡아요.
