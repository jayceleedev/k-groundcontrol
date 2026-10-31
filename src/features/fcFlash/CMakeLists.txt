# [0.2.0 T7–T10 T12 T14–T23] specs/features/0.2.0-fcflash.md
file(GLOB headers CONFIGURE_DEPENDS api/kgc/fcFlash/*.h)
file(GLOB_RECURSE internal_sources CONFIGURE_DEPENDS internal/*.h internal/*.cpp)
set(qml_files ui/FcFlashPanel.qml ui/StepBoard.qml ui/StepFirmware.qml ui/StepConfirm.qml ui/StepFlash.qml)
foreach(file IN LISTS qml_files)
    get_filename_component(name "${file}" NAME)
    set_source_files_properties("${file}" PROPERTIES QT_RESOURCE_ALIAS "${name}")
endforeach()
qt_add_qml_module(kgc_fcFlash STATIC URI Kgc.FcFlash
    OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml/Kgc/FcFlash"
    QML_FILES ${qml_files}
    SOURCES ${headers} ${internal_sources} ui/FcFlashController.h ui/FcFlashController.cpp ui/register.cpp)
target_include_directories(kgc_fcFlash PUBLIC api PRIVATE internal ui api/kgc/fcFlash)
target_link_libraries(kgc_fcFlash PUBLIC kgc_app_errors kgc_app_job kgc_ui
    PRIVATE kgc_app_log kgc_app_crc kgc_app_net kgc_app_paths kgc_app_scheduler kgc_app_constants
    kgc_comm_rawport kgc_comm_links kgc_vehicle_registry kgc_vehicle_safety kgc_vehicle_command)
