# link (기능 순위 2)
역할: 수동 연결 선택과 연결 상태를 상단바·하단 칩에 보여줘요.
수명: 전역 LinkService, 서비스가 소유하는 LinkController 싱글턴.
받는 것: LinkManager의 포트 목록·열기·종료·실패 신호, VehicleRegistry.activeChanged와 활성 기체의 link·capabilities 묶음.
주는 것: LinkService(ConnectionState Idle·Opening·Open·Failed, canConnect·connectLink·disconnectLink·ports·failureText·vehicles), registerUi(UiRegistry&, LinkService&), ModuleId 109.
기여: TopbarSlot의 link.port(12)·link.baud(13)·link.connect(14), StatusChipSlot의 link.connection(1).
의존: app/errors, comm/links, vehicle/registry, ui, Qt Qml.
안 하는 것: 포트 열기·파싱은 comm/links, 기체 발견·수신 상태는 vehicle이 맡아요. 명령 송신·자동 연결·설정 저장은 하지 않아요.
흐름:
1. LinkController.toggleConnection → LinkService.connectLink·disconnectLink — 고른 통로를 열거나 끊어요.
2. LinkManager.connectLink·disconnectLink — 워커에 요청해요.
3. linkOpened·linkFailed·linkClosed — 서비스 상태를 바꾸고 QML이 현재 언어로 칩을 그려요.
포트 선택: 위치 대신 이름을 보존하고 목록에서 사라지면 UDP로 돌아가요.
취소: 열기 성공을 기다리지 않고 linkClosed만으로 Idle 또는 Failed에 도달해요.
