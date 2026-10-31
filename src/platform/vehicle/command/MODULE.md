# vehicle/command (바탕 2층)
역할: COMMAND_LONG을 하나씩 보내고 ACK·시간 초과·재시도를 맡아요.
수명: 기체별(Vehicle이 하나 만들어 commands()로 내요).
받는 것: 송신 함수 SendFn, 대상 sysid, Vehicle이 넘기는 COMMAND_ACK와 stop.
주는 것: CommandQueue, CommandLong, CommandOptions, CommandResult, CommandFailure.
기여: 없음.
의존: app/errors, app/log, app/constants(ACK 시간·횟수), app/scheduler(Timeout), comm/mavlink(명령 패킹), vehicle/outbound(VehicleSender::PackFn, OutboundKind).
안 하는 것: 무슨 명령을 보낼지는 기능 모듈이 정해요. COMMAND_INT와 큐의 LinkSuspended 처리는 쓰는 버전에서 더해요.
