# [0.4.0 5.4] specs/features/0.4.0-sitl.md
file(GLOB headers CONFIGURE_DEPENDS api/kgc/sitl/*.h)
file(GLOB internal_sources CONFIGURE_DEPENDS internal/*.h internal/*.cpp)
set(qml_files ui/SitlPanel.qml ui/SitlChoices.qml ui/SitlHomeFields.qml ui/SitlStatus.qml ui/SitlOutput.qml)
foreach(file IN LISTS qml_files)
    get_filename_component(name "${file}" NAME)
    set_source_files_properties("${file}" PROPERTIES QT_RESOURCE_ALIAS "${name}")
endforeach()
qt_add_qml_module(kgc_sitl STATIC URI Kgc.Sitl
    OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml/Kgc/Sitl"
    QML_FILES ${qml_files}
    SOURCES ${headers} ${internal_sources} ui/SitlController.h ui/SitlController.cpp ui/register.cpp)
target_include_directories(kgc_sitl PUBLIC api PRIVATE internal ui api/kgc/sitl)
target_link_libraries(kgc_sitl PUBLIC kgc_app_errors kgc_app_job kgc_ui
    PRIVATE kgc_app_log kgc_app_net kgc_app_paths kgc_app_scheduler kgc_comm_links)
kgc_use_settings(kgc_sitl sitl)
