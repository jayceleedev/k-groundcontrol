# [0.0.4 T1] specs/features/0.0.4-righttabs.md
file(GLOB headers CONFIGURE_DEPENDS api/kgc/logs/*.h)
set_source_files_properties(ui/LogsTab.qml PROPERTIES QT_RESOURCE_ALIAS LogsTab.qml)
qt_add_qml_module(kgc_logs STATIC URI Kgc.Logs
    OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml/Kgc/Logs"
    QML_FILES ui/LogsTab.qml
    SOURCES ${headers} internal/LogsViewer.cpp ui/register.cpp)
target_include_directories(kgc_logs PUBLIC api PRIVATE internal ui)
target_link_libraries(kgc_logs PUBLIC kgc_app_errors kgc_ui)
