# comm/rawport (바탕 1층)
역할: MAVLink가 아닌 시리얼 바이트 통로와 포트의 주인 기록을 맡아요.
수명: 전역 PortRegistry 하나(Composition이 만들어 LinkManager에 넘겨요). RawPort·PortLease는 쓰는 동안만.
받는 것: 포트 이름, 소유자 종류.
주는 것: ByteStream, RawPort, PortRegistry(Owner, Token), PortLease, PortError, errorText.
기여: 없음.
의존: app/errors, app/log, Qt SerialPort.
안 하는 것: 링크 일시 정지(suspend)와 RawPort의 장시간 소유는 뒤 버전이에요. 임대는 빈 포트만 받아요. 부트로더 대화는 features/fcFlash가 맡아요.
스레드: PortRegistry·PortLease는 GUI 스레드, RawPort는 만든 스레드에서만 써요.
