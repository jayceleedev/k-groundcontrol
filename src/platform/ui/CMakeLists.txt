# [0.0.1 T5] specs/features/0.0.1-shell.md
file(GLOB headers CONFIGURE_DEPENDS api/kgc/ui/*.h)
file(GLOB themes CONFIGURE_DEPENDS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" themes/*.qml)
set_source_files_properties(${themes} PROPERTIES QT_QML_INTERNAL_TYPE TRUE)
file(GLOB components CONFIGURE_DEPENDS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" components/*.qml)
set_source_files_properties(components/ChartTheme.qml PROPERTIES QT_QML_INTERNAL_TYPE TRUE)
set_source_files_properties(Theme.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)
qt_add_qml_module(kgc_ui STATIC URI Kgc.Ui
    OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml/Kgc/Ui"
    QML_FILES Theme.qml ${themes} ${components}
    SOURCES ${headers} internal/PanelRegistry.cpp internal/PanelOrder.cpp internal/Slots.cpp internal/Navigator.cpp
        internal/ThemeChoice.cpp internal/ThemeSwitcher.cpp internal/UiSettingValues.cpp)
target_include_directories(kgc_ui PUBLIC api)
target_link_libraries(kgc_ui PUBLIC kgc_app_errors Qt6::Quick Qt6::Qml Qt6::QuickControls2)
kgc_use_settings(kgc_ui ui)
target_link_libraries(kgc_ui PRIVATE kgc_app_log Qt6::QuickEffects Qt6::Graphs Qt6::Svg)
