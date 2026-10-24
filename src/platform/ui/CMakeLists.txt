# [0.0.1 T5] specs/features/0.0.1-shell.md
file(GLOB headers CONFIGURE_DEPENDS api/kgc/ui/*.h)
set_source_files_properties(Theme.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)
qt_add_qml_module(kgc_ui STATIC URI Kgc.Ui
    OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml/Kgc/Ui"
    QML_FILES Theme.qml
    SOURCES ${headers} internal/PanelRegistry.cpp internal/PanelOrder.cpp internal/Slots.cpp internal/Navigator.cpp)
target_include_directories(kgc_ui PUBLIC api)
target_link_libraries(kgc_ui PUBLIC kgc_app_errors Qt6::Quick Qt6::Qml Qt6::QuickControls2)
