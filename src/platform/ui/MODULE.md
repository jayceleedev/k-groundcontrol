# ui (바탕 2.5층)
역할: 메뉴 등록과 탐색과 화면 토큰을 제공해요.
수명: 전역.
받는 것: ui.theme, ui.holdToConfirmMs, 운영체제 색 모드, PanelEntry와 이름으로 여는 요청.
주는 것: ThemeChoice와 resolveTheme, ThemeSwitcher, UiSettingValues, PanelRegistry, PanelOrder.h에 함께 정의한 PanelEntry와 PanelOrder 순수 함수, SlotModel을 공유하는 슬롯 넷, Navigator, UiRegistry, UiError와 Ui 문맥의 errorText, Theme(계열 색 목록 seriesColors 포함)과 공용 부품 42개(버튼·고르기·입력·표시·묶음·알림·차트·아이콘·시간).
기여: 비어 있는 기여 목록. 차트 셋은 내부 ChartTheme으로 색 규칙을 공유해요.
의존: app/errors, app/settings(생성 접근자), app/log, Qt Quick·Qml·QuickControls2·QuickEffects·Graphs·Svg.
안 하는 것: 기능 내용과 창 배치는 기능 모듈이 맡아요.
