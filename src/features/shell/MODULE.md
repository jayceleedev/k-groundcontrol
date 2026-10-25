# shell (기능 순위 1)
역할: 창의 다섯 영역과 메뉴 탐색을 제공해요.
수명: 전역.
받는 것: Paths::dataDir(AppStarted 로그), Translator, PanelRegistry, 슬롯 넷, Navigator, shell.leftMenuCollapsed.
주는 것: ShellService, registerUi.
기여: 메인 창과 컨트롤러. Theme.uiScale의 주인.
의존: app/errors, app/i18n, app/log, app/settings, app/paths, ui.
안 하는 것: 슬롯 내용과 지도는 각 기능이 맡아요.
흐름: 1. ShellController가 메뉴 동작을 받아요. 2. ShellService가 사건을 기록해요. 3. 등록된 패널을 표시해요.
