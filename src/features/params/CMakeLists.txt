# [0.3.0 5.5] specs/features/0.3.0-params.md
file(GLOB headers CONFIGURE_DEPENDS api/kgc/params/*.h)
file(GLOB internal_sources CONFIGURE_DEPENDS internal/*.h internal/*.cpp)
set(qml_files ui/ParamsPanel.qml ui/ParamTable.qml ui/ParamRow.qml ui/ParamDetail.qml ui/ParamGroups.qml
    ui/ParamActions.qml ui/WriteConfirmDialog.qml ui/FileCompareDialog.qml ui/BitmaskDialog.qml)
foreach(file IN LISTS qml_files)
    get_filename_component(name "${file}" NAME)
    set_source_files_properties("${file}" PROPERTIES QT_RESOURCE_ALIAS "${name}")
endforeach()
qt_add_qml_module(kgc_params STATIC URI Kgc.Params
    OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml/Kgc/Params"
    QML_FILES ${qml_files}
    SOURCES ${headers} ${internal_sources} ui/ParamsController.h ui/ParamsController.cpp
        ui/ParamsFilterModel.h ui/ParamsFilterModel.cpp ui/register.cpp)
target_include_directories(kgc_params PUBLIC api PRIVATE internal ui api/kgc/params)
target_link_libraries(kgc_params PUBLIC kgc_app_errors kgc_app_job kgc_ui kgc_vehicle_registry kgc_vehicle_firmware
    kgc_vehicle_safety
    PRIVATE kgc_app_log kgc_app_net kgc_app_paths kgc_app_scheduler kgc_comm_links kgc_comm_mavlink
    kgc_vehicle_command kgc_vehicle_outbound)
