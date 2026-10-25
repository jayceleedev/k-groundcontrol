# app/paths (바탕 0층)
역할: 데이터 폴더 아래의 경로를 한 곳에서 만들어요.
수명: 전역.
받는 것: BootOptions가 정하고 조립이 전달한 데이터 폴더.
주는 것: Paths의 데이터 경로 함수.
기여: 없음.
의존: app/errors, Qt Core.
안 하는 것: 로그를 찍지 않아요. 부팅 기본값은 app/boot가 정하고 로그는 app/log가 기록해요.
