# vehicle/states (바탕 2층)
역할: 기체 값을 하나의 소유자만 갱신하도록 지켜요.
수명: 기체별.
받는 것: 등록부가 거른 MAVLink 메시지, FirmwarePlugin, Scheduler.
주는 것: VehicleStates의 일곱 읽기 전용 묶음과 변경 시그널, VehicleStateOwner, isLinkLost. 공개 상태 헤더는 방언 헤더 없이 전방 선언만 써요.
기여: 없음.
의존: app/errors, app/constants, app/scheduler, vehicle/firmware, comm/mavlink, Qt Core.
안 하는 것: 화면 표시와 사용자 명령은 기능 모듈이 맡아요.
