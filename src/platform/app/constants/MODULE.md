# app/constants (바탕 0층)
역할: 링크 유지와 안전 명령에 함께 쓰는 고정값을 한 곳에 둬요.
수명: 전역 고정값.
받는 것: 없음.
주는 것: mavlink.h의 하트비트 주기, 끊김 한계, GCS sysid·compid, ACK 시간과 재시도 횟수. safety.h의 비상 정지 값 kEmergencyStopMagic(COMPONENT_ARM_DISARM param2 21196, 0.6.0; 꺾쇠 `<kgc/app/constants/safety.h>`로만 include해요).
기여: 없음.
의존: app/errors.
안 하는 것: 화면 표시와 사용자 명령은 기능 모듈이 맡아요.
