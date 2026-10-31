# [0.1.0 T9] specs/features/0.1.0-link.md
file(GLOB headers CONFIGURE_DEPENDS api/kgc/link/*.h)
set(qml_files)
foreach(screen LinkPort LinkBaud LinkConnect LinkConnection)
    set_source_files_properties(ui/${screen}.qml PROPERTIES QT_RESOURCE_ALIAS ${screen}.qml)
    list(APPEND qml_files ui/${screen}.qml)
endforeach()
qt_add_qml_module(kgc_link STATIC URI Kgc.Link
    OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml/Kgc/Link"
    QML_FILES ${qml_files}
    SOURCES ${headers} internal/LinkService.cpp ui/LinkController.h ui/LinkController.cpp ui/register.cpp)
target_include_directories(kgc_link PUBLIC api PRIVATE internal ui api/kgc/link)
target_link_libraries(kgc_link PUBLIC kgc_app_errors kgc_ui kgc_vehicle_registry PRIVATE kgc_comm_links)
