# map (기능 순위 6)
역할: 지도 부품 MapPane(타일·입력·축척·출처·제어)과, 모든 지도 부품이 같이 쓰는 타일 받기·저장·지도 종류를 맡아요.
수명: 전역. Composition의 mapLifetime(Http보다 먼저 지워지는 수명 객체)이 MapService의 부모예요. MapController는 서비스가 소유하는 형이 있는 싱글턴이에요.
받는 것: Http::get, Paths::cacheTiles, Translator의 language·languageChanged(타일 지명 언어), 부팅 값 tileHost(Composition이 넘김), 설정 map.mapType·map.lastLatitude·map.lastLongitude·map.lastZoom·map.tileStoreLimitMb, MapLayerSlot(레이어 목록).
주는 것: MapService(mapType·setMapType·language·vehicles·requestStoreSize·clearStore·lastView·saveView와 시그널 mapTypeChanged·languageChanged·downloadChanged·storeSizeReady·storeCleared·storeFailed·trailPointAdded·trailsCleared), SavedView, MapError, errorText, registerUi, ModuleId 113, 생성 설정 settings::MapType(Satellite·Hybrid·Road), QML Kgc.Map의 MapPane·MapInput·MapControls·MapScaleBar와 싱글턴 MapController.
기여: 없음(셸이 가운데에 MapPane을 놓아요).
의존: app/errors, app/i18n, app/job, app/log, app/net, app/paths, app/scheduler, app/settings(생성 접근자), app/store, vehicle/registry, vehicle/states, ui(Theme·IconButton·PillGroup·MapLayerSlot·UiRegistry), Qt Location·Positioning.
안 하는 것: 미션·펜스·랠리·유도 표식·트래커·영상 같은 다른 기능의 그림은 그리지 않아요(각 기능이 MapLayerSlot에 레이어로 얹어요). 기체로 아무것도 보내지 않아요.

Qt 비공개 API: internal/geoservice/만 Qt Location 비공개 헤더(*_p.h)와 Qt6::LocationPrivate를 써요(검사 qt_private). Qt를 올리면 지도를 다시 확인해요.
지도 플러그인: internal/geoservice의 KgcGeoServiceFactory가 provider "kgc"인 정적 플러그인이에요(QT_STATICPLUGIN, Composition의 Q_IMPORT_PLUGIN 한 번). register.cpp가 QML 엔진 프로퍼티 kgcTileSource에 TileSource를 두고, TileSource가 지워지면 비워요. KgcMappingEngine은 확대 2~20·256 px·방위와 기울기 없음·미리 받기 없음(createMap의 NoPrefetching)이고, 언어가 바뀌면 타일 판을 올리고 서버에 다시 닿으면(onlineAgain) 보이는 장면을 비워 빈 칸만 다시 청해요.
Qt 캐시: KgcTileCache는 메모리·텍스처만 쓰고 init()·clearAll()을 재정의해 디스크를 건드리지 않아요. clearMapId()와 Map.clearData()·prefetchData()는 부르지 않아요(작업 폴더의 파일을 지워요).
타일 공급: TileSource(GUI 스레드)는 같은 타일 요청을 열쇠(tileKey)마다 하나로 묶고, 저장소 → 없으면 Http 순서로 받아 그림이면 저장해요. 기다리는 사람이 없으면 대기를 먼저 지우고 요청을 멈춰요. 서버에 닿지 않는 동안 Timeout mapOnlineProbe가 10초마다 마지막 실패 주소를 다시 물어요.
타일 저장소: TileStore는 저장소 스레드에서 BlobStore(연결 이름 mapTiles, cache/tiles/tiles.sqlite)를 열어요. 열지 못하면 파일을 한 번 지우고 다시 열고, 그래도 안 되면 저장 없이 답해요. 본 타일은 본 타일 저장 한도까지 오래 안 본 것부터 지워요(한도를 바꾸면 바로).
순수 함수(tests/): TileUrl(tileUrl·tileKey), TileMath(tileOf·tileRange·tileCount·metersPerPixel), ScaleBar(scaleBar·scaleLabel), SizeText(sizeText), TileImage(tileImageFormat), Trail(isMapPosition·appendTrailPoint), DownloadPlan(planDownload).
흐름(지도 종류): 1. MapControls의 mapTypeChoice → MapController.mapType 2. MapService::setMapType — 설정에 쓰고 mapTypeChanged 3. MapPane의 Binding이 activeMapType을 바꾸고 KgcTileFetcher가 새 종류 타일을 TileSource::fetch로 청해요.
흐름(타일 한 장): 1. Qt Location → KgcTileFetcher::getTileImage → KgcTileReply 2. TileSource::fetch — TileStore::lookup 3. 없으면 Http::get(tileUrl) 4. 그림이면 TileStore::save 뒤 답에 바이트를 넣어요.
