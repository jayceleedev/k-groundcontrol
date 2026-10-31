# app/scheduler (바탕 0층)
역할: 이름 있는 반복 작업의 스레드와 수명을 지켜요.
수명: 전역.
받는 것: registerTask의 고유 이름·양수 주기·같은 스레드의 owner와 context·fn.
주는 것: Scheduler::registerTask, 소유자 종료에 따른 작업 취소.
기여: 없음.
의존: app/errors, Qt Core.
안 하는 것: 화면 표시와 사용자 명령은 기능 모듈이 맡아요.
