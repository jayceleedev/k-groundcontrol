# logs (기능 순위 3)
역할: 텔레메트리·온보드 로그를 다뤄요. 이번 버전엔 우 사이드바 로그 탭을 "기체 없음" 모습으로만 보여줘요.
수명: 혼합(전역 LogsViewer; 기체별 내려받기 서비스는 아직 없어요).
받는 것: Theme 토큰, Translator.language(탭 이름은 셸이 번역 문맥 "RightTab"으로 그려요).
주는 것: LogsViewer(상태·함수 없음, Composition이 하나 만듦), registerUi(UiRegistry&, LogsViewer&), ModuleId 106.
기여: RightTabSlot — logs(Tab, order 40, LogsTab).
의존: app/errors, ui(RightTabSlot·UiRegistry·공용 부품 Section·KeyValueRow·ActionButton·EmptyState).
안 하는 것: 좌 메뉴 "로그 분석" 패널(Composition의 자리 표시 그대로), tlog 기록, 폴더 열기, 온보드 로그 목록·내려받기·지우기는 0.1.x 이후예요. 앱 로그 창은 analyze가 맡아요.
