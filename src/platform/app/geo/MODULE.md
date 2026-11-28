# app/geo (바탕 0층)
역할: 위치 사이 거리와 고도 틀 바꾸기를 한 곳에서 계산해요. 고도 변환 함수는 이 모듈에만 둬요(R-N11).
수명: 없음(순수 함수).
받는 것: 위도·경도(도), 고도(m), 고도 틀 AltitudeFrame.
주는 것: Geodesy.h의 distanceMeters(하버사인, 지구 평균 반지름 kEarthMeanRadiusM 6371007.2 m — Qt QGeoCoordinate::distanceTo와 같은 값)·isKnownPosition(둘 다 유한하고 (0, 0)이 아님), Altitude.h의 AltitudeFrame(Amsl·RelativeHome·AboveCurrent)·altitudeIn(홈 기준 → 틀, 여기로 가기)·heightIn(지금 자리 위 → 틀, 이륙), ModuleId 22.
기여: 없음.
의존: app/errors, Qt Core.
안 하는 것: 펌웨어를 모르고 어느 틀을 쓸지는 정하지 않아요(펌웨어 플러그인이 틀을 내고 부르는 모듈이 넘겨요). 지형·타원체고↔MSL 변환과 방위·고각은 그 버전에서 더해요.
