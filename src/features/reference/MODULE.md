# reference (기능 순위 2)
역할: 설정에 참고할 공식 사이트의 소개와 바로가기를 제공해요.
수명: 전역.
받는 것: UiRegistry, Theme, shell이 패널의 glassSource에 넣는 지도 바닥.
주는 것: ReferenceService, registerUi.
기여: 좌 메뉴 15번 참고 패널을 묶음 4의 params와 appSettings 사이에 등록해요.
의존: app/errors, ui(등록부·테마·공용 부품).
안 하는 것: 앱 안에서 문서를 표시하거나 내려받지 않고, 외부 문서 표시는 기본 브라우저가 맡아요.
흐름(패널 등록):
1. registerUi → PanelRegistry.registerPanel — 참고 패널의 메뉴와 화면 주소를 등록해요.
흐름(바로가기):
1. ReferenceCard의 ActionButton.onClicked → Qt.openUrlExternally — 카드의 주소를 기본 브라우저에 넘겨요.
