# comm/links (바탕 1층)
역할: 하나의 링크를 워커에서 열고 GUI 스레드로 수신을 전달해요.
수명: 전역 LinkManager와 연결마다 만드는 워커.
받는 것: LinkConfig, Scheduler, BootOptions.extraSerial.
주는 것: LinkManager의 연결·수신·종료 시그널과 포트 목록, Link·UdpLink·SerialLink, LinkError.
기여: 없음.
의존: app/errors, app/log, app/scheduler, comm/mavlink, Qt Core·Network·SerialPort.
안 하는 것: 화면 표시와 사용자 명령은 기능 모듈이 맡아요.
흐름: 1. connectLink가 워커에서 열어요. 2. disconnectLink가 열기 중에도 취소해요. 3. 워커 종료 뒤 linkClosed 또는 linkFailed를 정확히 한 번 알려요.
오류 전달: ErrorCode와 선택적 오류를 등록해 워커 시그널로 받아요.
