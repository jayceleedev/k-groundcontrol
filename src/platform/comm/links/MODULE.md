# comm/links (바탕 1층)
역할: 하나의 링크를 워커에서 열고 GUI 스레드로 수신을 전달해요.
수명: 전역 LinkManager와 연결마다 만드는 워커.
받는 것: LinkConfig, Scheduler, PortRegistry, BootOptions.extraSerial 경로 목록.
주는 것: 포트가 목록과 /dev/ 장치 파일 양쪽에서 사라지면 PortLost로 알리는 LinkManager의 연결·수신·종료 시그널과 포트 목록, Link·UdpLink·SerialLink, LinkError.
기여: 없음.
의존: app/errors, app/log, app/scheduler, comm/mavlink, comm/rawport(PortRegistry), Qt Core·Network·SerialPort.
안 하는 것: 화면 표시와 사용자 명령은 기능 모듈이 맡아요.
흐름: 1. connectLink가 시리얼 포트를 PortRegistry.claim으로 점유한 뒤 워커에서 열어요. 2. disconnectLink가 열기 중에도 취소해요. 3. 워커 종료 뒤 PortRegistry.release로 점유를 놓고 linkClosed 또는 linkFailed를 정확히 한 번 알려요.
포트 조회: refreshPorts는 파일이 존재하는 추가 경로만 목록에 넣어요. portInfo는 운영체제의 제조사·제품 설명을 돌려주며, 없으면 빈 값이에요.
오류 전달: ErrorCode와 선택적 오류를 등록해 워커 시그널로 받아요. 시리얼이 열린 뒤 NoError·TimeoutError는 무시하고, ResourceError 중 EAGAIN(쓰기가 잠깐 바쁨)은 오류를 지우고 링크를 유지해요. 남은 바이트는 Qt의 쓰기 알림이 스스로 다시 보내고, 로그는 링크마다 한 번이에요. 나머지 오류와 음수 쓰기 결과는 PortLost로 닫고 SerialPortLost 경고를 남겨요.
통로 유지: 시리얼의 DTR·RTS를 켜되 실패하면 오류를 지우고 연결을 계속해요. 미지원은 조용히 넘기고 다른 제어선 실패는 매번 경고해요. UDP는 최근 10초 안에 수신한 모든 주소에 회신해요.
