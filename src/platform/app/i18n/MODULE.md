# app/i18n (바탕 0층)
역할: 적용된 언어와 번역기를 관리해요.
수명: 전역.
받는 것: 번역 파일 폴더, 요청 언어 코드, system 선택 시 운영체제 언어.
주는 것: Translator::setLanguage, language, languageChanged.
기여: 없음.
의존: app/errors, Qt Core.
안 하는 것: QML 엔진을 모르며 화면 재번역 연결은 조립이 맡아요. 언어 설정 저장은 appSettings가 맡아요.
