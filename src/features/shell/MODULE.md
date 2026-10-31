# shell (기능 순위 1)
역할: 창의 다섯 영역과 메뉴 탐색을 제공해요.
수명: 전역.
받는 것: Paths::dataDir(AppStarted 로그), Translator, PanelRegistry, 슬롯 다섯(BottomActionSlot의 단추는 하단바 맨 오른쪽, objectName은 bottomAction_<id>), Navigator, shell.leftMenuCollapsed, shell.leftMenuWidth(160~320, 배율 1 기준), Theme의 유리 여부와 토큰, UiSettingValues의 배경 종류와 사진 주소. 우 사이드바는 RightTabSlot의 placement가 "top"인 항목을 계기 칸 rightTop에, "tab"인 항목을 탭 줄 rightTabs(TabStrip fill, 단추 rightTab_<id>, 이름은 qsTranslate("RightTab", titleKey))에 order 순으로 두고, 고른 탭을 rightTabContent(세로로만 스크롤, 스크롤바는 오른쪽 Theme.scrollGutter 홈의 오른쪽 끝에 배치, 내용은 홈 밖에 두고 스크롤바가 숨겨져도 홈 유지)에 실어요. 탭 id와 RightTabPlacement는 몰라요.
주는 것: ShellService, registerUi. 등록된 패널 뿌리에 property Item glassSource가 있으면 불러온 뒤 mapStage를 넣어요. 패널의 Card·GlassSurface가 이 값을 source로 받아요.
기여: 좌 메뉴 오른쪽 가장자리 손잡이 leftMenuResizeHandle(끄는 동안 dragWidth로 그리고 놓을 때 ShellController.leftMenuWidth에 한 번 저장, 접힌 동안 없음; 메뉴 이름은 폭에 안 들어가면 …로 줄이고 힌트), backgroundLayer에 민무늬·체크무늬·사진을 그리는 지도 바닥과 가운데 안쪽을 나눈 메인 창과 컨트롤러. Theme.uiScale의 주인.
의존: app/errors, app/i18n, app/log, app/settings(생성 접근자의 공개 의존), app/paths, ui.
안 하는 것: 슬롯 내용과 지도는 각 기능이 맡아요.
흐름(좌 메뉴 폭): 1. LeftMenu 손잡이를 놓으면 ShellController.setLeftMenuWidth → settings::setLeftMenuWidth로 저장해요. 2. leftMenuWidthChanged로 폭이 바뀌고 CenterArea 폭(유리 테마는 viewportLeft)이 따라 바뀌어요.
흐름: 1. ShellController가 메뉴 동작을 받아요. 2. ShellService가 사건을 기록해요. 3. 등록된 패널을 표시해요.

연결 기여: TopbarEntry·StatusChipEntry가 component 역할이 있으면 Loader로 기여 화면을 띄워요. 기여가 자기 objectName을 정하고 자리 표시의 모습은 유지해요.
