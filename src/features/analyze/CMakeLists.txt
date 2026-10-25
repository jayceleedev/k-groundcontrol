# [0.0.3 T1] specs/features/0.0.3-windows.md
file(GLOB headers CONFIGURE_DEPENDS api/kgc/analyze/*.h)
set_source_files_properties(ui/AppLogWindow.qml PROPERTIES QT_RESOURCE_ALIAS AppLogWindow.qml)
qt_add_qml_module(kgc_analyze STATIC URI Kgc.Analyze
    OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml/Kgc/Analyze"
    QML_FILES ui/AppLogWindow.qml
    SOURCES ${headers} internal/AppLogService.cpp internal/AppLogModel.h internal/AppLogModel.cpp internal/Events.h
        ui/AppLogController.h ui/AppLogController.cpp ui/register.cpp)
target_include_directories(kgc_analyze PUBLIC api PRIVATE internal ui)
target_link_libraries(kgc_analyze PUBLIC kgc_app_errors kgc_ui kgc_app_log)
kgc_use_settings(kgc_analyze analyze)
