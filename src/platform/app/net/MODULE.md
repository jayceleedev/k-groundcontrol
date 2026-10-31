# app/net (바탕 0층)
역할: HTTPS 요청과 내려받기, gzip 풀기를 한 곳에서 맡아요.
수명: 전역 Http 하나(Composition이 만들어요). 요청마다 HttpRequest 하나.
받는 것: 주소, 저장 경로, HttpOptions.
주는 것: Http, HttpRequest, HttpOptions, redirectAllowed, gunzip, HttpError, errorText.
기여: 없음.
의존: app/errors, app/log, Qt Network, zlib(gunzip 안에서만).
안 하는 것: 다운로드 캐시와 해시 기록은 뒤 버전이에요(0.2.0 §10-5). 응답 해석은 부르는 모듈이 맡아요.
