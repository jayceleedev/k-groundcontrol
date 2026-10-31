# vehicle (바탕 2층)
역할: 펌웨어와 상태, 송신, 기체 등록의 의존 경계를 정해요.
수명: 전역 등록부와 기체별 객체.
받는 것: comm/links의 메시지와 링크 수명 시그널.
주는 것: 기체와 읽기 전용 상태, 유일한 송신 문.
기여: 없음.
의존: app/* < comm/mavlink < comm/links < vehicle/firmware < vehicle/states < vehicle/outbound < vehicle/registry 순서를 지켜요.
안 하는 것: 사용자 화면과 명령 작업은 기능 모듈이 맡아요.
