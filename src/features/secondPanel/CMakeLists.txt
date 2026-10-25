# [0.0.3 T1] specs/features/0.0.3-windows.md
file(GLOB headers CONFIGURE_DEPENDS api/kgc/secondPanel/*.h)
set_source_files_properties(ui/SecondPanelWindow.qml PROPERTIES QT_RESOURCE_ALIAS SecondPanelWindow.qml)
qt_add_qml_module(kgc_secondPanel STATIC URI Kgc.SecondPanel
    OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml/Kgc/SecondPanel"
    QML_FILES ui/SecondPanelWindow.qml
    SOURCES ${headers} internal/SecondPanelService.cpp internal/CellArrangement.cpp
        ui/SecondPanelController.h ui/SecondPanelController.cpp ui/register.cpp)
target_include_directories(kgc_secondPanel PUBLIC api PRIVATE internal ui)
target_link_libraries(kgc_secondPanel PUBLIC kgc_app_errors kgc_ui)
kgc_use_settings(kgc_secondPanel secondPanel)
