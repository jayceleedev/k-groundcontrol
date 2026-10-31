# [0.1.0 T9] specs/features/0.1.0-link.md
# [0.0.4 T1] specs/features/0.0.4-righttabs.md
# [0.0.4 T24a T26] specs/features/0.0.4-righttabs.md
# 화면은 기체 상태를 읽기만 하므로 별도 서비스 없이 컨트롤러를 둬요.
file(GLOB headers CONFIGURE_DEPENDS api/kgc/vehicleInfo/*.h)
set(screens VehicleInfoValues InstrumentView VehicleAttitude3D OverviewTab DetailTab MessagesTab)
set(qml_files)
foreach(screen IN LISTS screens)
    set_source_files_properties(ui/${screen}.qml PROPERTIES QT_RESOURCE_ALIAS ${screen}.qml)
    list(APPEND qml_files ui/${screen}.qml)
endforeach()
qt_add_qml_module(kgc_vehicleInfo STATIC URI Kgc.VehicleInfo
    OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml/Kgc/VehicleInfo"
    QML_FILES ${qml_files}
    SOURCES ${headers} ui/register.cpp ui/VehicleInfoController.h ui/VehicleInfoController.cpp)
target_include_directories(kgc_vehicleInfo PUBLIC api PRIVATE ui)
target_link_libraries(kgc_vehicleInfo PUBLIC kgc_app_errors kgc_ui kgc_vehicle_registry)
# 기체 3D(VehicleAttitude3D)는 Quick3D를 써요. BASE §2상 Quick3D는 기능 모듈만 쓸 수 있어 platform/ui가 아니라 여기에 둬요.
find_package(Qt6 6.12 REQUIRED COMPONENTS Quick3D)
target_link_libraries(kgc_vehicleInfo PRIVATE Qt6::Quick3D)
