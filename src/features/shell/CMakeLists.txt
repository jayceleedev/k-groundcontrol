# [0.0.1 T1] specs/features/0.0.1-shell.md
file(GLOB headers CONFIGURE_DEPENDS api/kgc/shell/*.h)
set_source_files_properties(ui/Shell.qml PROPERTIES QT_RESOURCE_ALIAS Shell.qml)
set_source_files_properties(ui/TopBar.qml PROPERTIES QT_RESOURCE_ALIAS TopBar.qml)
set_source_files_properties(ui/LeftMenu.qml PROPERTIES QT_RESOURCE_ALIAS LeftMenu.qml)
set_source_files_properties(ui/CenterArea.qml PROPERTIES QT_RESOURCE_ALIAS CenterArea.qml)
set_source_files_properties(ui/RightBar.qml PROPERTIES QT_RESOURCE_ALIAS RightBar.qml)
set_source_files_properties(ui/BottomBar.qml PROPERTIES QT_RESOURCE_ALIAS BottomBar.qml)
qt_add_qml_module(kgc_shell STATIC URI Kgc.Shell
    OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml/Kgc/Shell"
    QML_FILES ui/Shell.qml ui/TopBar.qml ui/LeftMenu.qml ui/CenterArea.qml ui/RightBar.qml ui/BottomBar.qml
    SOURCES ${headers} internal/ShellService.cpp internal/Events.h
        ui/ShellController.h ui/ShellController.cpp ui/register.cpp)
target_include_directories(kgc_shell PUBLIC api "${CMAKE_BINARY_DIR}/gen" PRIVATE internal ui)
target_link_libraries(kgc_shell PUBLIC kgc_app_errors kgc_app_i18n kgc_ui
    PRIVATE kgc_app_log kgc_app_settings kgc_app_paths)
