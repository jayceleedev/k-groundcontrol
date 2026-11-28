# vehicle/command (바탕 2층)
역할: COMMAND_LONG·COMMAND_INT를 한 큐로 하나씩 보내고 ACK·시간 초과·재시도·끼어들기를 맡아요.
수명: 기체별(Vehicle이 하나 만들어 commands()로 내요).
받는 것: 송신 함수 SendFn, 대상 sysid, Vehicle이 넘기는 COMMAND_ACK와 stop.
주는 것: CommandQueue(send 둘 — CommandLong·CommandInt), CommandLong, CommandInt(frame·x·y·z), CommandOptions(timeout·retries·kind·preempt), CommandResult, CommandFailure(Timeout·Gone·NoLink·Preempted).
기여: 없음.
의존: app/errors, app/log, app/constants(ACK 시간·횟수), app/scheduler(Timeout), comm/mavlink(명령 패킹), vehicle/outbound(VehicleSender::PackFn, OutboundKind).
안 하는 것: 무슨 명령을 보낼지는 기능 모듈이 정해요. 큐의 LinkSuspended 처리는 쓰는 버전에서 더해요.
송신 종류: 명령마다 CommandOptions.kind로 보내요(기본 Command; fly는 시동 Arm, 그 밖 Motion).
끼어들기: preempt가 참이면 보내고 기다리던 명령(어느 context든)과 같은 context로 줄 선 명령을 Preempted로 끝내고 send 안에서 곧바로 보내요. 다른 context로 줄 선 명령은 그대로 뒤에 나가요. 끼어든 명령의 ACK와 번호가 같은 앞 명령의 늦은 ACK는 구별하지 못해요(fly의 시동 끄기·비상 정지만 쓰고, fly가 시동 상태로 끝을 정해요). 밀린 다른 context의 명령을 다시 넣는 것은 낸 쪽의 일이에요. 멈춘 큐에는 preempt와 상관없이 Gone이에요.
