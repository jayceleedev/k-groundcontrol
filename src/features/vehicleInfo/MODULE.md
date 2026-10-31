# vehicleInfo (기능 순위 3)
역할: 활성 기체 상태를 계기와 개요에 보여줘요.
수명: 기체별 상태를 읽는 화면, VehicleRegistry가 소유하는 전역 VehicleInfoController 싱글턴.
받는 것: VehicleRegistry.activeChanged, 활성 Vehicle의 linkConfig와 link·arming·attitude·position·gps·battery 묶음, UiSettingValues.instrumentStyle·instrumentStyles, Theme.
주는 것: registerUi(UiRegistry&, VehicleRegistry&), ModuleId 105.
기여: RightTabSlot의 instrument(Top 0), overview(Tab 10), detail(Tab 20), messages(Tab 30).
의존: app/errors, vehicle/registry, ui, Qt Qml·Quick3D.
안 하는 것: 수신값을 저장하거나 다시 요청하지 않아요. 상세·메시지 탭과 하단 센서·배터리·RSSI 칩은 자리 표시를 유지해요. 홈 거리·링크 품질·비행 시간은 아직 제공하지 않아요.
표시: VehicleInfoValues.qml이 계기와 개요의 모드·GPS 이름과 숫자 단위를 번역해 조립해요. 기울기 값이 유한해진 뒤 계기를 켜요. 응답이 끊기면 계기만 비활성이고 개요는 원본 상태의 마지막 값을 유지해요.
