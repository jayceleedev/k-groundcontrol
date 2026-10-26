# [0.0.4 T1] specs/features/0.0.4-righttabs.md
# 이번 버전엔 로직이 없어 internal/ 없이 api/와 ui/만 둬요(BASE §3).
file(GLOB headers CONFIGURE_DEPENDS api/kgc/vehicleInfo/*.h)
set(screens InstrumentView OverviewTab DetailTab MessagesTab)
set(qml_files)
foreach(screen IN LISTS screens)
    set_source_files_properties(ui/${screen}.qml PROPERTIES QT_RESOURCE_ALIAS ${screen}.qml)
    list(APPEND qml_files ui/${screen}.qml)
endforeach()
qt_add_qml_module(kgc_vehicleInfo STATIC URI Kgc.VehicleInfo
    OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml/Kgc/VehicleInfo"
    QML_FILES ${qml_files}
    SOURCES ${headers} ui/register.cpp)
target_include_directories(kgc_vehicleInfo PUBLIC api PRIVATE ui)
target_link_libraries(kgc_vehicleInfo PUBLIC kgc_app_errors kgc_ui)
