# [0.0.1 T1] specs/features/0.0.1-shell.md
file(GLOB headers CONFIGURE_DEPENDS api/kgc/appSettings/*.h)
set_source_files_properties(ui/AppSettingsPanel.qml PROPERTIES QT_RESOURCE_ALIAS AppSettingsPanel.qml)
qt_add_qml_module(kgc_appSettings STATIC URI Kgc.AppSettings
    OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml/Kgc/AppSettings"
    QML_FILES ui/AppSettingsPanel.qml
    SOURCES ${headers} internal/AppSettingsService.cpp internal/Events.h
        ui/AppSettingsController.h ui/AppSettingsController.cpp ui/register.cpp)
target_include_directories(kgc_appSettings PUBLIC api "${CMAKE_BINARY_DIR}/gen" PRIVATE internal ui)
target_link_libraries(kgc_appSettings PUBLIC kgc_app_errors kgc_app_i18n kgc_ui
    PRIVATE kgc_app_log kgc_app_settings )
