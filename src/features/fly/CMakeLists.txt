# [0.6.0 5.11.1] specs/features/0.6.0-fly.md
file(GLOB headers CONFIGURE_DEPENDS api/kgc/fly/*.h)
file(GLOB internal_sources CONFIGURE_DEPENDS internal/*.h internal/*.cpp)
set(qml_files ui/FlyVehicle.qml ui/FlyMode.qml ui/FlyArm.qml ui/FlyTakeoff.qml ui/FlyLand.qml ui/FlyRtl.qml
    ui/FlyPause.qml ui/FlyEmergencyStop.qml ui/FlyHoldButton.qml ui/FlyTargetLayer.qml ui/FlyGoToLayer.qml
    ui/FlyGoToWindow.qml)
foreach(file IN LISTS qml_files)
    get_filename_component(name "${file}" NAME)
    set_source_files_properties("${file}" PROPERTIES QT_RESOURCE_ALIAS "${name}")
endforeach()
# DEPENDENCIES QtPositioning이 있어야 qmllint가 FlyController의 QGeoCoordinate 형을 찾아요.
qt_add_qml_module(kgc_fly STATIC URI Kgc.Fly
    OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml/Kgc/Fly"
    QML_FILES ${qml_files}
    SOURCES ${headers} ${internal_sources} ui/FlyController.h ui/FlyController.cpp ui/register.cpp
    DEPENDENCIES QtPositioning)
target_include_directories(kgc_fly PUBLIC api PRIVATE internal ui api/kgc/fly)
target_link_libraries(kgc_fly PUBLIC kgc_app_errors kgc_ui kgc_vehicle_registry kgc_vehicle_firmware kgc_vehicle_safety
    PRIVATE kgc_app_log kgc_app_job kgc_app_geo kgc_app_scheduler kgc_comm_mavlink kgc_vehicle_command
        kgc_vehicle_outbound kgc_vehicle_states kgc_params Qt6::Positioning)
