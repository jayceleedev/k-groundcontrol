# ui (바탕 2.5층)
역할: 테마의 선택·수명·화면 토큰과 공용 부품, 메뉴·슬롯 등록과 탐색, WindowPlacement의 자리 변환과 SeparateWindow의 창 수명 관리를 제공해요.
수명: 전역.
받는 것: ui.theme, ui.holdToConfirmMs, ui.backgroundKind, ui.backgroundImage, ui.instrumentStyle, 운영체제 색 모드, PanelEntry·SlotEntry·BottomActionSlot::registerAction의 호출 대상, 이름으로 여는 요청, WindowPlacement의 화면 정보·저장값, SeparateWindow의 엔진·창 주소·기본 크기·설정 읽기와 쓰기 함수.
주는 것: SeparateWindow(open·window·savePlacement·windowChanged; 기능 컨트롤러의 값 멤버로 두고 QObject 부모를 받지 않음, 창의 부모는 SeparateWindow), WindowPlacement의 ScreenInfo·Placement·restorePlacement·savePlacement, applyDefaultSurfaceFormat(창 생성 전 공용 멀티샘플 형식), ThemeChoice와 resolveTheme, ThemeSwitcher(themeId·currentIndex·themes·selected·choose), UiSettingValues(길게 누르기 시간·배경 종류·사진 경로·backgroundImageSource 로컬 주소·chooseBackgroundImage 사진 선택·실패 판정과 경로별 한 번 기록, 계기 모양 instrumentStyle(목록 밖 저장값은 pfd)·instrumentStyles 목록), PanelRegistry, PanelOrder.h에 함께 정의한 PanelEntry와 PanelOrder 순수 함수, SlotModel(add·id·order 정렬·중복 거부, 파생 슬롯용 entryAt)을 공유하는 슬롯 다섯(TopbarSlot·StatusChipSlot의 registerPlaceholder와 영역·쪽·이름·아이콘·앞 구분선 역할, TopbarSlot의 모양 인자·역할 shape("button" 기본·"dropdown"·"display", 모르는 모양은 프로그래밍 오류로 종료; 0.0.4 §14)(자리 순서는 0.0.3 스펙 §12.5), BottomActionSlot의 registerAction·trigger·icon·hintKey 포함, 잘못된 호출 대상은 프로그래밍 오류로 종료; RightTabSlot의 registerTab과 RightTabPlacement(Top 하나·Tab), titleKey·component·placement("top"|"tab") 역할 — 같은 id는 DuplicateId를 먼저, 두 번째 Top은 TopTaken), Navigator, UiRegistry, UiError(DuplicateId·TopTaken)와 Ui 문맥의 errorText, Theme(계열 색 목록 seriesColors, 계기 색 attitudeSky·attitudeGround, 계기 모양 넷의 색 attitudeMark·attitudeTape·osdInk·osdShadow·attitudeAircraft·hudBackground·hudInk·modelBody·modelAccent·compassRing·compassNorthColor, 스크롤바 폭 scrollBarWidth·홈 scrollGutter, 설명 글자 captionSize, 계기 단추 줄 instrumentStyleBarHeight 포함)과 공용 부품 47개(버튼·고르기·입력·표시·묶음·알림·차트·아이콘·시간; 0.0.4의 계기 AttitudeIndicator·PrimaryFlightDisplay·HeadUpDisplay와 빈 상태 EmptyState 포함). PFD·HUD는 내부 형 InstrumentOsd로 OSD 여섯 글자(osdLink·osdModeArm·osdClimb·osdHome·osdBattery·osdGps)와 빈 자리 "—" 규칙을 함께 써요. ActionButton.trailingIcon은 빈 값이면 자리를 차지하지 않고, 이름이 있으면 글자 뒤에 같은 크기·글자색의 아이콘을 그려요. IconButton은 checked면 accent 바탕·accentInk 아이콘이에요. 부품 크기는 Theme의 쓰임별 토큰으로 정해요. SettingsPage는 카테고리 목록과 선택 위치를 받아 왼쪽 1 : 오른쪽 3 비율과 키보드 이동을 제공하며 Section.category에 맞춰 보이기·머리글·즉시 배치를 정해요. 차트 셋은 내부 ChartTheme으로 색 규칙을 공유해요. TabStrip은 PillGroup의 선택 동작을, Chip은 StatusDot의 color·softColor를 공유해요. TabStrip은 fill이면 줄 폭을 탭 수로 똑같이 나눠 채우고 긴 이름은 …로 줄여 힌트로 보여요. TextBox·ComboField는 비활성이면 ActionButton처럼 흐려요.
기여: 없음.
의존: app/errors, app/settings(생성 접근자), app/log, Qt Quick·Qml·QuickControls2·QuickEffects·Graphs·Svg.
안 하는 것: 기능 내용과 별도 창의 설정·소유는 기능 컨트롤러가 맡고 테마를 고르는 화면은 appSettings가 맡아요. 테마 판정은 resolveTheme 한 곳에만 있어요.
흐름(테마 선택):
1. ThemeSwitcher.choose → settings::setTheme — 고른 테마를 저장해요.
2. settings::onThemeChanged → apply → resolveTheme — 저장값과 운영체제 색 모드로 테마를 정해요.
3. apply — 모르는 값은 ThemeUnknown을 값마다 한 번 기록하고, 적용된 테마가 같으면 끝나요.
4. QQmlComponent::create → Theme.current — 새 객체를 넣고 이전 객체는 deleteLater로 정리해요.
5. themeIdChanged → ThemeApplied — 식별자와 목록 위치 변경을 알리고 적용 이유를 기록해요.
흐름(계기 모양 선택):
1. UiSettingValues.setInstrumentStyle → settings::setInstrumentStyle — 목록에 있는 계기 모양만 저장하고 instrumentStyleChanged를 내요.
흐름(길게 누르기 시간 설정):
1. UiSettingValues.setHoldToConfirmMs → settings::setHoldToConfirmMs — 허용 범위의 시간만 저장해요.
흐름(배경 선택):
1. UiSettingValues.chooseBackgroundImage → setBackgroundImage·settings::setBackgroundKind — 사진 경로를 저장하고 사진 배경을 선택해요. 종류만 고를 때는 setBackgroundKind가 생성 접근자로 저장해요.
2. backgroundImageSource → QUrl::fromLocalFile — 특수 문자와 운영체제별 경로를 화면 주소로 바꿔요.
3. 생성자·설정 변경·화면의 읽기 실패 → reportBackgroundImageMissing — 빈 경로와 손상 파일을 포함한 읽기 실패를 판정하고 현재 사진 배경의 경로마다 한 번 기록해요.
