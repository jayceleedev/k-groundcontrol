# appSettings (기능 순위 2)
역할: 언어·테마·배경·길게 누르기 설정과 배포 정보를 제공해요.
수명: 전역.
받는 것: Translator, AppInfo, appSettings.language, ThemeSwitcher(테마 목록과 currentIndex), UiSettingValues(배경 종류·사진 경로 포함).
주는 것: AppSettingsService, AppInfo, registerUi.
기여: 좌 메뉴 15번 앱 설정 패널과 컨트롤러. SettingsPage에서 일반·화면·조작·정보 카테고리를 제공하고 about 탐색 인자는 정보 카테고리를 열어요.
의존: app/errors, app/i18n, app/log, app/settings(생성 접근자의 공개 의존), ui.
안 하는 것: 창 배치는 shell이 맡고 테마 판정은 ui가 맡아요.
흐름(언어 선택):
1. AppSettingsController.setLanguage → settings::setLanguage — 선택한 언어를 저장해요.
2. AppSettingsService의 settings::onLanguageChanged → Translator.setLanguage — 언어를 적용하고 변경 사건을 기록해요.
흐름(배경 선택):
1. FileField.editingFinished·FileDialog.accepted → UiSettingValues.chooseBackgroundImage — 입력하거나 고른 경로를 저장하고 사진 배경을 선택해요.
2. backgroundKindPicker → UiSettingValues.setBackgroundKind — 배경 종류만 고르면 해당 종류를 저장해요.
흐름(카테고리 탐색):
1. AppSettingsPanel.onArg → SettingsPage.currentIndex — 탐색 인자로 받은 카테고리를 선택하며 모르는 인자는 일반으로 돌아가요.
2. SettingsPage — Section.category로 보일 항목과 머리글을 정해요.
