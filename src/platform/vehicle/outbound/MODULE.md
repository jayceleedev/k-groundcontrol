# vehicle/outbound (바탕 2층)
역할: 기체로 보내는 모든 프레임을 하나의 문으로 통과시켜요.
수명: 혼합(전역 LinkHeartbeat + 기체별 VehicleSender).
받는 것: LinkManager, Scheduler, FirmwarePlugin.streamRequests, 대상 sysid·linkId.
주는 것: 링크별 1Hz GCS 하트비트를 맡는 LinkHeartbeat, VehicleSender::send·sendStreamRequests, OutboundKind, NoLink. send의 성공은 전송 완료가 아니라 워커 송신 예약을 뜻해요.
송신 구분: CommandQueue의 명령은 Command, 하트비트와 스트림 요청은 Other로 받아요.
기여: 없음.
의존: app/errors, app/constants, app/scheduler, comm/links, vehicle/firmware, Qt Core.
안 하는 것: 화면 표시와 사용자 명령은 기능 모듈이 맡아요.
반복: 링크가 열리면 하트비트를 바로 보내고, 스트림 요청은 요청 뒤 500ms마다 목록 전체를 다섯 차례 보내요.
