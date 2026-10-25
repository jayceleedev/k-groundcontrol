# app/settings (바탕 0층)
역할: 생성된 메타데이터로 운영 설정을 저장해요.
수명: 전역.
받는 것: 생성 등록 테이블의 설정 메타데이터, QSettings의 저장값, 호출자가 전달하는 설정 식별자와 값.
주는 것: Settings::instance, value, setValue, changed, definitions.
기여: 없음.
의존: app/errors, Qt Core.
안 하는 것: 검색 화면은 설정을 표시하는 기능 모듈이 맡아요. 부팅 옵션은 app/boot가 읽어요.
