# analyze (기능 순위 2)
역할: 최근 앱 로그를 별도 창에서 실시간으로 보여줘요.
수명: 전역 AppLogService(기체별 기능은 아직 없어요).
받는 것: LogFeed의 recent·recordAdded·capacity, analyze.logWindowPlacement, Theme 토큰.
주는 것: AppLogService와 읽기 전용 model·loopGuardPrefixes(조립에 넘기는 경고 출처), registerUi, ModuleId 103.
기여: BottomActionSlot의 appLog 단추, appLogController, AppLogWindow.
의존: app/errors, app/log, app/settings(생성 접근자), ui(SeparateWindow·UiRegistry·공용 부품).
안 하는 것: 기체 로그·인스펙터·거르기·저장은 구현하지 않아요. 파일 기록은 app/log가 맡아요.
흐름:
1. AppLogService::AppLogService → connect·LogFeed::recent·AppLogModel::append — 통로를 구독한 뒤 최근 목록을 채우고 마지막 순번 이하의 신호는 버려요.
2. BottomActionSlot.trigger → AppLogController.open → SeparateWindow::open — 같은 엔진의 루트 문맥에서 처음에만 창을 만들어요.
3. SeparateWindow::open → restorePlacement → setScreen·setGeometry·show — 자리를 복원하고 앞으로 가져와요.
4. LogView::userScrolled·Toggle::toggled → following·LogView::positionAtEnd — 따라가기 상태를 정하고 토글을 켜면 끝으로 옮겨요.
5. TimerBox·closing → AppLogController::savePlacement·SeparateWindow::savePlacement — 보통 상태의 창 자리를 저장해요. 닫기는 숨김이에요.
6. QCoreApplication::aboutToQuit → SeparateWindow::savePlacement·QQuickWindow::close — 종료 전에 자리를 저장하고 창을 닫아요.
