# app/settings (바탕 0층)
역할: 생성된 메타데이터로 운영 설정을 저장해요.
수명: 전역.
받는 것: 조립에서 전달하는 초기 값.
주는 것: Settings::instance, value, setValue, changed, definitions.
기여: 없음.
의존: app/errors, Qt Core.
안 하는 것: 다른 앱 모듈의 값 조회와 기능 실행은 조립이 맡아요.
