# app/boot (바탕 0층)
역할: 명령줄과 부팅 설정을 읽어요.
수명: 전역.
받는 것: 프로그램 이름을 포함한 명령줄 인자, AppConfigLocation의 kgc.ini 안 log·paths 항목.
주는 것: BootOptions::parse 값 묶음.
기여: 없음.
의존: app/errors, Qt Core.
안 하는 것: 다른 명령줄 옵션을 받거나 로그를 찍지 않아요. 운영 설정은 app/settings가 맡아요.
