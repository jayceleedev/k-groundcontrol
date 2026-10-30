# uiGallery (기능 순위 2)
역할: 공용 부품의 변형과 상태를 테마별로 확인하는 목록이에요.
수명: 전역.
받는 것: Theme, ThemeSwitcher(테마 목록과 currentIndex), UiSettingValues, shell이 패널의 glassSource에 넣는 지도 바닥.
주는 것: UiGalleryService, registerUi.
기여: 좌 메뉴 17번 UI 부품 목록 패널.
의존: app/errors, ui(등록부·테마·공용 부품).
안 하는 것: 실제 기체 조작과 알림 발행은 각 기능이 맡아요.
흐름: 1. PanelRegistry에 패널을 등록해요. 2. 패널의 선택 줄이 ThemeSwitcher.choose로 테마를 바꿔요.
