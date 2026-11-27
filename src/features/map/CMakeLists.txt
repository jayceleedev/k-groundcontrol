# [0.5.0 5.6.1 5.6.9] specs/features/0.5.0-map.md
# Qt 비공개 API는 지도 geoservice만 써요(BASE §2, 검사 qt_private). 그래서 비공개 구성 요소는 이 파일에서만 찾아요.
# GLOBAL이 없으면 이 폴더에서만 보이는 XKB::XKB를 src/main·tests의 실행 파일이 찾지 못해 구성 경고가 남아요.
find_package(Qt6 6.12 REQUIRED COMPONENTS LocationPrivate GLOBAL)
file(GLOB headers CONFIGURE_DEPENDS api/kgc/map/*.h)
file(GLOB internal_sources CONFIGURE_DEPENDS internal/*.h internal/*.cpp)
file(GLOB geoservice_sources CONFIGURE_DEPENDS internal/geoservice/*.h internal/geoservice/*.cpp)
set(qml_files ui/MapPane.qml ui/MapInput.qml ui/MapControls.qml ui/MapScaleBar.qml ui/MapDownloadCard.qml
    ui/TrailLayer.qml ui/HomeLayer.qml ui/VehicleLayer.qml)
foreach(file IN LISTS qml_files)
    get_filename_component(name "${file}" NAME)
    set_source_files_properties("${file}" PROPERTIES QT_RESOURCE_ALIAS "${name}")
endforeach()
# DEPENDENCIES QtPositioning이 있어야 qmllint가 MapController의 QGeoCoordinate·QGeoRectangle 형을 찾아요.
qt_add_qml_module(kgc_map STATIC URI Kgc.Map
    OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml/Kgc/Map"
    QML_FILES ${qml_files}
    SOURCES ${headers} ${internal_sources} ${geoservice_sources}
        ui/MapController.h ui/MapController.cpp
        ui/VehicleMarkerModel.h ui/VehicleMarkerModel.cpp ui/register.cpp
    DEPENDENCIES QtPositioning)
target_include_directories(kgc_map PUBLIC api PRIVATE internal ui api/kgc/map)
# 지도 플러그인 팩토리의 moc 출력은 이 대상의 mocs_compilation.cpp에서 컴파일되므로 대상 전체에 둬요.
# qt_add_plugin은 쓰지 않아요(둘째 대상은 순환 검사에 걸리고 숨은 Q_IMPORT_PLUGIN을 하나 더 퍼뜨려요).
target_compile_definitions(kgc_map PRIVATE QT_STATICPLUGIN)
target_link_libraries(kgc_map PUBLIC kgc_app_errors kgc_app_job kgc_ui Qt6::Positioning
    PRIVATE kgc_app_log kgc_app_net kgc_app_paths kgc_app_scheduler kgc_app_store kgc_app_i18n
        kgc_vehicle_registry kgc_vehicle_states Qt6::Location Qt6::LocationPrivate)
kgc_use_settings(kgc_map map)
