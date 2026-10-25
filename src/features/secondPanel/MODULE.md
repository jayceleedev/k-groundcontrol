# secondPanel (기능 순위 2)
역할: 별도 창에 최대 여섯 개의 빈 칸을 배치해요.
수명: 전역 SecondPanelService.
받는 것: secondPanel.arrangement·windowPlacement, Theme 토큰과 메인 창 배율.
주는 것: SecondPanelService(arrangement·cells·setArrangement·arrangementChanged), cellRects 순수 함수, registerUi, ModuleId 104.
기여: BottomActionSlot의 secondPanel 단추, secondPanelController, SecondPanelWindow.
의존: app/errors, app/settings(생성 접근자), ui(SeparateWindow·UiRegistry·공용 부품).
안 하는 것: 칸 안 정보 부품과 크기 끌기는 아직 구현하지 않아요.
흐름:
1. BottomActionSlot.trigger → SecondPanelController.open → SeparateWindow::open — 처음에만 루트 문맥으로 창을 만들어요.
2. SeparateWindow::open → restorePlacement → setScreen·setGeometry·show — 저장된 자리에 창을 표시해요.
3. arrangementPicker → chooseArrangement → SecondPanelService.setArrangement — 선택을 저장해요.
4. cellRects → cells — 비율 사각형을 내고 화면은 맞닿은 변의 여백을 빼요.
5. TimerBox·closing·aboutToQuit → SecondPanelController::savePlacement·SeparateWindow::savePlacement — 보통 상태의 자리를 저장해요. 닫기는 숨김이에요.
