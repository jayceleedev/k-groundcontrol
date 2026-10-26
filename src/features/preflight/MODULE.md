# preflight (기능 순위 3)
역할: 비행 전 점검표를 보여줘요. 이번 버전엔 우 사이드바 점검 탭을 "기체 없음" 모습으로만 보여줘요.
수명: 기체별(전역 부분이 없어요). 화면 기여는 앱 시작 때 Composition이 registerUi(uiRegistry)로 한 번 등록해요.
받는 것: Theme 토큰(언어 전환은 엔진 retranslate와 셸의 qsTranslate("RightTab")가 맡아요).
주는 것: registerUi(UiRegistry&), ModuleId 107.
기여: RightTabSlot — preflight(Tab, order 50, PreflightTab).
의존: app/errors, ui(RightTabSlot·UiRegistry·공용 부품 ComboField·EmptyState).
안 하는 것: 점검표 정의(resources/data/preflight/)·판정·시동 잠금은 0.1.x 이후예요. internal/은 로직이 생길 때 만들어요.
