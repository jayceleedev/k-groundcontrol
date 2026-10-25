# appSettings (기능 순위 2)
역할: 언어·테마·길게 누르기 설정과 배포 정보를 제공해요.
수명: 전역.
받는 것: Translator, AppInfo, appSettings.language, ThemeSwitcher, UiSettingValues.
주는 것: AppSettingsService, AppInfo, registerUi.
기여: 좌 메뉴 15번 앱 설정 패널과 컨트롤러.
의존: app/errors, app/i18n, app/log, app/settings(생성 접근자의 공개 의존), ui.
안 하는 것: 창 배치는 shell이 맡고 테마 판정은 ui가 맡아요.
흐름: 1. AppSettingsController가 설정을 저장해요. 2. AppSettingsService가 Translator에 적용해요. 3. 언어 변경 사건을 기록해요.
