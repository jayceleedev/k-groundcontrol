# app/log (바탕 0층)
역할: 앱 로그를 기록하고 회전하며 최근 기록을 화면에 전달해요.
수명: 전역.
받는 것: 조립이 넘기는 로그 폴더·최소 수준·회전 크기·보관 개수, 수준 매크로의 LogEvent와 사건 이름·메시지, Qt 메시지.
주는 것: LogEvent, LogLevel, parseLogLevel, LogSink::install·attachFeed, LogRecord, LogFeed(publish·recent·capacity·recordAdded), KGC_LOG 수준 매크로 넷. 기록 구현은 detail과 비공개 멤버에 둬요.
기여: 없음.
의존: app/errors, Qt Core.
안 하는 것: 비밀값 마스킹은 비밀값을 도입하는 버전에서 맡아요. 부팅 파일은 app/boot가 읽어요.
통로: publish의 잠금 안에서 순번·보관·큐 등록을 함께 처리해요. 파일 잠금 밖에서 전달하고, 수신 중이나 로그 화면에서 생긴 Qt 경고는 파일에만 남겨요.
