# map (기능 순위 6)
역할: 지도 부품 MapPane(타일·입력·축척·출처·제어·기체 아이콘·궤적·홈·지도 저장 카드)과, 모든 지도 부품이 같이 쓰는 타일 받기·저장·지도 종류·궤적·영역 받기를 맡아요.
수명: 전역. Composition의 mapLifetime(Http보다 먼저 지워지는 수명 객체)이 MapService의 부모예요. MapController는 서비스가 소유하는 형이 있는 싱글턴이에요.
받는 것: Http::get, Paths::cacheTiles, Translator의 language·languageChanged(타일 지명 언어), 부팅 값 tileHost(Composition이 넘김), 화면 배율(QGuiApplication::screens의 QScreen::devicePixelRatio, screenAdded·screenRemoved, 화면마다 logicalDotsPerInchChanged·physicalDotsPerInchChanged), VehicleRegistry(vehicles·active·activeChanged·vehicleAdded·vehicleRemoved), VehicleStates의 position(홈 세 필드 포함)·attitude와 positionChanged·attitudeChanged, 설정 map.mapType·map.lastLatitude·map.lastLongitude·map.lastZoom·map.tileStoreLimitMb·map.trailMinMeters·map.trailMaxPoints·map.autoFollow, MapLayerSlot(레이어 목록).
주는 것: MapService(mapType·setMapType·language·tilePixelRatio·vehicles·tileCount·startDownload·downloadJob·dismissDownload·requestStoreSize·clearStore·trailOf·clearTrails·lastView·saveView와 시그널 mapTypeChanged·languageChanged·tilePixelRatioChanged·downloadChanged·storeSizeReady·storeCleared·storeFailed·trailPointAdded·trailsCleared), SavedView, MapError, errorText, registerUi, ModuleId 113, 생성 설정 settings::MapType(Satellite·Hybrid·Road)과 지도 설정 접근자, QML Kgc.Map의 MapPane(지도 누르기 신호 MapPane.tapped(coordinate, position) — 레이어가 mapPane으로 들어요)·MapInput·MapControls·MapScaleBar·MapDownloadCard·TrailLayer·HomeLayer·VehicleLayer와 싱글턴 MapController(앱 설정 "지도" 칸이 지도 설정 네 개와 범위를 읽고 써요).
기여: MapLayerSlot에 mapTrails(10, TrailLayer)·mapHomes(20, HomeLayer)·mapVehicles(30, VehicleLayer). 사이를 비워 미션·유도·트래커 레이어가 끼게 해요. 셸이 가운데에 MapPane을 놓아요.
의존: app/errors, app/geo(isKnownPosition), app/i18n, app/job, app/log, app/net, app/paths, app/scheduler, app/settings(생성 접근자), app/store, vehicle/registry, vehicle/states, ui(Theme·IconButton·ActionButton·PillGroup·NumberBox·HoldToConfirm·Card·JobView·TimerBox·MapLayerSlot·UiRegistry), Qt Location·Positioning.
안 하는 것: 미션·펜스·랠리·유도 표식·트래커·영상 같은 다른 기능의 그림은 그리지 않아요(각 기능이 MapLayerSlot에 레이어로 얹어요). 기체로 아무것도 보내지 않고 기체 상태 값을 바꾸지 않아요. 영역 받기는 기체와 무관해 ArmedPolicy::check를 부르지 않아요. 누른 곳에서 할 일은 레이어를 낸 기능이 정해요(map은 fly를 몰라요).

Qt 비공개 API: internal/geoservice/만 Qt Location 비공개 헤더(*_p.h)와 Qt6::LocationPrivate를 써요(검사 qt_private). Qt를 올리면 지도를 다시 확인해요.
지도 플러그인: internal/geoservice의 KgcGeoServiceFactory가 provider "kgc"인 정적 플러그인이에요(QT_STATICPLUGIN, Composition의 Q_IMPORT_PLUGIN 한 번). register.cpp가 QML 엔진 프로퍼티 kgcTileSource에 TileSource를 두고, TileSource가 지워지면 비워요. KgcMappingEngine은 확대 2~20·256 px 칸(카메라 능력값, 2배 타일의 512 px 그림도 같은 칸에 줄여 그림)·방위와 기울기 없음·미리 받기 없음(createMap의 NoPrefetching)이에요. 언어나 타일 화소 배수가 바뀌거나 다른 배수를 대신 보여 준 뒤 다시 닿으면(fallbackStale) 타일 판을 올려 보이는 칸을 지금 배수로 다시 청하고, 그 밖에 서버에 다시 닿으면(onlineAgain) 보이는 장면을 비워 빈 칸만 다시 청해요.
Qt 캐시: KgcTileCache는 메모리·텍스처만 쓰고 init()·clearAll()을 재정의해 디스크를 건드리지 않아요. 메모리·텍스처 예산은 엔진이 만들어질 때의 배수²배예요(실행 중에 배수가 바뀌어도 그대로). clearMapId()와 Map.clearData()·prefetchData()는 부르지 않아요(작업 폴더의 파일을 지워요).
타일 공급: TileSource(GUI 스레드)는 같은 타일 요청을 열쇠(tileKey, 2배 타일은 끝에 @2)마다 하나로 묶고, 청한 배수의 열쇠로 저장소 → 없으면 Http 순서로 받아 그림이면 저장해요. 지도 요청은 저장소에 없을 때 오프라인이면 Http 전에, 서버가 실패하면 그 뒤에 다른 배수의 열쇠를 저장소에서 찾아 대신 줘요(아래 다른 배수 대신 쓰기). 기다리는 사람이 없으면 대기를 먼저 지우고 요청을 멈춰요. 서버에 닿지 않는 동안 Timeout mapOnlineProbe가 10초마다 마지막 실패 주소를 다시 물어요.
타일 화소 배수: MapService가 연결된 화면 가운데 가장 큰 배율이 1.15 이상이면 2, 아니면 1로 정해요(시작할 때와 화면을 더하거나 빼거나 배율이 바뀔 때, 로그 "Map tile ratio"). 2배 타일은 주소 끝 &scale=2, 열쇠 끝 @2예요(1배 주소·열쇠는 0.5.0과 같아 저장된 타일과 받은 영역을 그대로 써요).
다른 배수 대신 쓰기: 지도 요청은 오프라인이면 서버에 묻기 전에, 온라인이면 서버가 실패한 뒤에 다른 배수의 저장된 타일을 대신 줘요(저장·고정 안 함). 영역 받기는 지금 배수로만 받고 대신 쓰지 않아요. 대신 쓴 뒤 다시 닿으면 fallbackStale로 보이는 칸을 지금 배수로 다시 청해요.
타일 저장소: TileStore는 저장소 스레드에서 BlobStore(연결 이름 mapTiles, cache/tiles/tiles.sqlite)를 열어요. 열지 못하면 파일을 한 번 지우고 다시 열고, 그래도 안 되면 저장 없이 답해요(그동안 영역 받기는 StoreFailed). 본 타일은 본 타일 저장 한도까지 오래 안 본 것부터 지워요(한도를 바꾸면 바로). 영역 받기로 받은 타일은 고정(pinned)이라 비우기 전까지 남아요.
궤적: TrailBook이 기체마다 궤적 점 간격보다 많이 움직였을 때만 점을 더하고 궤적 최대 점 수까지 둬요(최대 점 수를 줄이면 바로 오래된 점부터 지움). 위치 없음((0, 0)·NaN)은 점으로 더하지 않아요. 기체가 없어지면 그 궤적을 버려요. TrailLayer도 같은 규칙으로 선을 다듬어요.
기체 표식: VehicleMarkerModel이 기체마다 한 줄(sysid 순서)로 위치·기수 방향(yaw, 없으면 hdg, 없으면 0)·활성·홈·홈 기준 고도 글(altitudeText "30 m", 정수 반올림, 모르면 빈 값)을 내놓아요. VehicleLayer는 안쪽 Shape를 돌린 화살표와 그 오른쪽의 돌지 않는 고도 글(모든 기체), HomeLayer는 "H" 동그라미예요.
따라가기(부품마다): 따라가기 자동 켜기가 켜져 있으면 활성 기체가 바뀔 때(사용자가 꺼 두었어도) 다시 켜고, 위치가 아직 없으면 첫 위치에서 켜요. 같은 기체 동안은 다시 켜지 않아요. 지도를 끌거나 영역 고르기에 들어가면 꺼지고, 따라가기 단추로 다시 켜요.
보던 곳: rememberView인 부품(가운데 지도)만 지도가 saveViewDelayMs(2초) 멈춘 뒤 MapService::saveView로 저장해요. 모든 부품은 lastView에서 시작해요.
지도 누르기: MapInput의 mapTap(TapHandler, 끌기 문턱)이 끌지 않은 한 번 누르기를 tapped(좌표, 화면 점)로 내고 MapPane.tapped로 넘겨요. 지도를 바꾸지 않아요. 문턱을 넘긴 끌기는 mapDrag가 가져가고, 영역 고르는 중에는 내지 않아요.
정수 확대: 휠은 모은 값 120마다 한 층(잔 값 흐름은 한 층 넘긴 뒤 130 ms마다 한 층, 몸짓은 휠이 0.25초 쉬면 끝), 확대·축소 단추는 그 방향의 다음 정수 층, 핀치는 놓을 때 가까운 정수 층으로 바로 맞춰요. 지도 폭에 맞춘 Qt의 최소 확대만 소수 그대로예요. 시작 확대는 저장된 값을 반올림해요.
순수 함수(tests/): TileUrl(tileUrl·tileKey), TileMath(tileOf·tileRange·tileCount·metersPerPixel), ScaleBar(scaleBar·scaleLabel), SizeText(sizeText), TileImage(tileImageFormat), Trail(isMapPosition·appendTrailPoint), DownloadPlan(planDownload), ZoomStep(wheelStep·zoomStepTarget·pinchSnapTarget·tilePixelRatioFor).
흐름(지도 종류): 1. MapControls의 mapTypeChoice → MapController.mapType 2. MapService::setMapType — 설정에 쓰고 mapTypeChanged 3. MapPane의 Binding이 activeMapType을 바꾸고 KgcTileFetcher가 새 종류 타일을 TileSource::fetch로 청해요.
흐름(타일 한 장): 1. Qt Location → KgcTileFetcher::getTileImage → KgcTileReply 2. TileSource::fetch(지금 배수) — TileStore::lookup 3. 없으면 Http::get(tileUrl), 오프라인이면 먼저 대신 쓰기 4. 그림이면 TileStore::save 뒤 답에 바이트를 넣어요 5. 서버가 실패했거나 오프라인이면 startFallback — 다른 배수 열쇠를 TileStore::lookup해 있으면 그 바이트를, 없으면 오류를 답에 넣어요.
흐름(영역 받기):
1. MapControls의 mapStoreButton → MapDownloadCard가 보이고 MapController.refreshStoreSize로 저장 크기를 물어요.
2. mapDownloadArea → MapPane.selecting = true — 지도 끌기가 영역 고르기(MapInput의 mapSelectDrag)로 바뀌어요.
3. MapInput.selectionDone → MapDownloadCard.choose — 영역과 기본 확대 범위(지금 확대의 내림 ~ +4)로 계획 단계예요.
4. mapDownloadStart → MapController.startDownload → MapService::startDownload — planDownload로 확인하고 지금 타일 화소 배수로 MapDownloadJob을 만들어요(받는 동안 배수가 바뀌어도 그 배수로 끝까지 받음).
5. MapDownloadJob::start → TileSource::fetch(pin) — 작은 확대부터 kDownloadParallel개씩 TileStore::pin(있으면 고정) 또는 Http::get → TileStore::save(pinned)예요.
6. Job이 끝나면 downloadChanged → 카드가 "다 받았어요" 또는 실패·취소 글을 보이고, mapDownloadClose → dismissDownload예요.
흐름(저장소 비우기): 1. mapStoreClear(길게 누르기) → MapController.clearStore 2. MapService::clearStore → 저장소 스레드의 TileStore::clear 3. storeCleared → MapController가 refreshStoreSize로 크기를 다시 물어요.
흐름(따라가기 단추): 1. mapFollow → MapPane.follow를 뒤집어요 2. 켜졌으면 map.center = MapController.activeCoordinate 3. 그 뒤 activePositionChanged마다 MapPane이 가운데를 기체로 옮겨요.
