# vehicleInfo (기능 순위 3)
역할: 우 사이드바의 계기 칸과 개요·상세·메시지 탭으로 기체 상태를 보여줘요. 이번 버전엔 "기체 없음" 모습만 그려요.
수명: 기체별(전역 부분이 없어요). 화면 기여는 앱 시작 때 Composition이 registerUi(uiRegistry)로 한 번 등록해요.
받는 것: UiSettingValues의 instrumentStyle·instrumentStyles(계기 모양, 주인 ui), Theme 토큰(언어 전환은 엔진 retranslate와 셸의 qsTranslate("RightTab")가 맡아요).
주는 것: registerUi(UiRegistry&), ModuleId 105.
기여: RightTabSlot — instrument(Top, order 0, InstrumentView: 맨 위 줄 모양 단추 넷 instrumentStyle_<값>과 그 아래 그림 칸에 둥근 계기·PFD·HUD·기체 3D VehicleAttitude3D 중 하나), overview(Tab 10, OverviewTab), detail(Tab 20, DetailTab), messages(Tab 30, MessagesTab).
의존: app/errors, ui(RightTabSlot·UiRegistry·UiSettingValues·공용 부품 AttitudeIndicator·PrimaryFlightDisplay·HeadUpDisplay·IconButton·EmptyState·Section·KeyValueRow·ProgressLine·SearchBox·PillGroup), Qt Quick3D(VehicleAttitude3D만; BASE §2상 기능 모듈만 써요).
안 하는 것: 기체 종류로 3D 모형(quad·plane)을 고르는 것과 기체 값 표시·컨트롤러·서비스는 0.1.x에서 기체 모델과 함께 더해요. 하단 센서·배터리·RSSI 칩은 아직 Composition의 자리 표시예요. 상세의 별표 고정·메시지 목록·필터·찾기도 0.1.x예요. internal/은 로직이 생길 때 만들어요.
