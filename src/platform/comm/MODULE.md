# comm (바탕 1층)
역할: 링크와 프로토콜의 의존 경계를 정해요.
수명: 전역.
받는 것: 연결 값과 워커의 수신 바이트.
주는 것: comm/mavlink < comm/links 순서의 하위 영역.
기여: 없음.
의존: app/* < comm/mavlink < comm/links < vehicle/firmware 순서를 지켜요.
안 하는 것: 기체 발견과 상태 갱신은 vehicle이 맡아요.
