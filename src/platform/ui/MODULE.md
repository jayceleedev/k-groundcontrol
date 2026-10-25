# ui (바탕 2.5층)
역할: 테마의 선택·수명·화면 토큰과 공용 부품, 메뉴 등록과 탐색을 제공해요.
수명: 전역.
받는 것: ui.theme, ui.holdToConfirmMs, 운영체제 색 모드, PanelEntry와 이름으로 여는 요청.
주는 것: ThemeChoice와 resolveTheme, ThemeSwitcher(themeId·currentIndex·themes·selected·choose), UiSettingValues, PanelRegistry, PanelOrder.h에 함께 정의한 PanelEntry와 PanelOrder 순수 함수, SlotModel을 공유하는 슬롯 넷, Navigator, UiRegistry, UiError와 Ui 문맥의 errorText, Theme(계열 색 목록 seriesColors 포함)과 공용 부품 42개(버튼·고르기·입력·표시·묶음·알림·차트·아이콘·시간).
기여: 비어 있는 기여 목록. 차트 셋은 내부 ChartTheme으로 색 규칙을 공유해요.
의존: app/errors, app/settings(생성 접근자), app/log, Qt Quick·Qml·QuickControls2·QuickEffects·Graphs·Svg.
안 하는 것: 기능 내용과 창 배치는 기능 모듈이 맡고 테마를 고르는 화면은 appSettings가 맡아요. 테마 판정은 resolveTheme 한 곳에만 있어요.
흐름:
1. ThemeSwitcher.choose → settings::setTheme — 고른 테마를 저장해요.
2. settings::onThemeChanged → apply → resolveTheme — 저장값과 운영체제 색 모드로 테마를 정해요.
3. apply — 모르는 값은 ThemeUnknown을 값마다 한 번 기록하고, 적용된 테마가 같으면 끝나요.
4. QQmlComponent::create → Theme.current — 새 객체를 넣고 이전 객체는 deleteLater로 정리해요.
5. themeIdChanged → ThemeApplied — 식별자와 목록 위치 변경을 알리고 적용 이유를 기록해요.
6. UiSettingValues.setHoldToConfirmMs → settings::setHoldToConfirmMs — 허용 범위의 시간만 저장해요.
