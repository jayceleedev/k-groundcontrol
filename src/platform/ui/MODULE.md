# ui (바탕 2.5층)
역할: 메뉴 등록과 탐색과 화면 토큰을 제공해요.
수명: 전역.
받는 것: PanelEntry와 이름으로 여는 요청.
주는 것: PanelRegistry, PanelOrder.h에 함께 정의한 PanelEntry와 PanelOrder 순수 함수, 슬롯 넷, Navigator, UiRegistry, UiError, Theme.
기여: 비어 있는 기여 목록.
의존: app/errors, Qt Quick·Qml·QuickControls2.
안 하는 것: 기능 내용과 창 배치는 기능 모듈이 맡아요.
