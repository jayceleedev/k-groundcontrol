# vehicle/firmware (바탕 2층)
역할: 펌웨어 차이를 데이터와 순수 함수로 제공해요.
수명: 기체별 플러그인, Vehicle이 소유해요.
받는 것: HEARTBEAT의 autopilot·custom_mode·type.
주는 것: FirmwarePluginFactory, FirmwarePlugin의 displayName·modeName·streamRequests, StreamRequest.
기여: 없음.
의존: app/errors, comm/mavlink, Qt Core.
안 하는 것: 송신과 상태 쓰기는 하지 않아요. 모르는 모드는 빈 이름, 모르는 자동조종기는 범용 플러그인으로 제공해요.
