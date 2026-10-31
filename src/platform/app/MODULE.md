# app (바탕 0층)
역할: 앱 공통 기반의 경계를 정해요.
수명: 전역.
받는 것: 조립의 초기 값.
주는 것: errors < log < paths < boot < settings < i18n, 독립된 constants·scheduler 순서의 하위 영역.
기여: 없음.
의존: 모든 하위 영역은 errors에 기대요. boot는 log의 LogLevel과 parseLogLevel로 부팅 수준을 검증해요.
안 하는 것: 하위 영역 간 값 전달은 main이 맡아요.
부팅 연결: BootOptions.extraSerial은 main이 LinkManager에 넘기며 운영 설정으로 저장하지 않아요.
