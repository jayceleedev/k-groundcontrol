# app/net (바탕 0층)
역할: HTTPS 요청과 내려받기, gzip 풀기를 한 곳에서 맡아요.
수명: 전역 Http 하나(Composition이 만들어요). 요청마다 HttpRequest 하나.
받는 것: 주소, 저장 경로, HttpOptions.
주는 것: Http, HttpRequest, HttpOptions, redirectAllowed, gunzip, failureOf(마지막 실패의 오류: 상태 400 이상은 HttpStatus, 그 밖은 TimedOut·NetworkFailed), trustCertificateFile(--tile-ca 시험용 인증서; 시험용 인증서는 그 실행의 모든 HTTPS 요청에 적용, 시스템 루트 인증서도 계속 믿음), HttpError(BadCertificate·HttpStatus 포함), errorText. onlineChanged는 성공과 HttpStatus를 온라인으로 보고 Aborted는 무시해요. 마지막 실패마다 "Request failed: <host>, HTTP <status>|TimedOut|NetworkFailed"를 경고로 남겨요.
기여: 없음.
의존: app/errors, app/log, Qt Network, zlib(gunzip 안에서만).
안 하는 것: 다운로드 캐시와 해시 기록은 뒤 버전이에요(0.2.0 §10-5). 응답 해석은 부르는 모듈이 맡아요.
