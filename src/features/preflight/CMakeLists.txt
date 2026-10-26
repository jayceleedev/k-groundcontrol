# [0.0.4 T1] specs/features/0.0.4-righttabs.md
# 이번 버전엔 로직이 없어 internal/ 없이 api/와 ui/만 둬요(BASE §3).
file(GLOB headers CONFIGURE_DEPENDS api/kgc/preflight/*.h)
set_source_files_properties(ui/PreflightTab.qml PROPERTIES QT_RESOURCE_ALIAS PreflightTab.qml)
qt_add_qml_module(kgc_preflight STATIC URI Kgc.Preflight
    OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml/Kgc/Preflight"
    QML_FILES ui/PreflightTab.qml
    SOURCES ${headers} ui/register.cpp)
target_include_directories(kgc_preflight PUBLIC api PRIVATE ui)
target_link_libraries(kgc_preflight PUBLIC kgc_app_errors kgc_ui)
