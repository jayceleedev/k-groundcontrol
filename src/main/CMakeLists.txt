# [0.0.4 T1] specs/features/0.0.4-righttabs.md
# [0.0.3 T1 T14] specs/features/0.0.3-windows.md
# [0.0.1 T1] specs/features/0.0.1-shell.md
# [0.0.2 T27] specs/features/0.0.2-theme.md
qt_add_resources(kgc_composition_rc "${PROJECT_SOURCE_DIR}/resources/kgc.qrc")
add_library(kgc_composition STATIC Composition.h Composition.cpp ${kgc_composition_rc})
qt_add_resources(kgc_composition tabler_icons PREFIX /kgc/icons BASE "${PROJECT_BINARY_DIR}/icons" FILES ${kgc_icon_files})
target_include_directories(kgc_composition PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(kgc_composition PUBLIC
    KGC_VERSION="${PROJECT_VERSION}" KGC_HOMEPAGE_URL="${PROJECT_HOMEPAGE_URL}")
target_link_libraries(kgc_composition PUBLIC kgc_app_errors kgc_app_boot kgc_app_paths
    kgc_app_log kgc_app_settings kgc_app_i18n kgc_ui kgc_shell kgc_appSettings kgc_uiGallery kgc_analyze kgc_secondPanel
    kgc_vehicleInfo kgc_logs kgc_preflight
    kgc_uiplugin kgc_shellplugin kgc_appSettingsplugin kgc_uiGalleryplugin kgc_analyzeplugin kgc_secondPanelplugin
    kgc_vehicleInfoplugin kgc_logsplugin kgc_preflightplugin)
qt_add_translations(kgc_composition
    TS_FILES "${PROJECT_SOURCE_DIR}/i18n/kgc_en.ts"
    RESOURCE_PREFIX /kgc/i18n
    SOURCE_TARGETS kgc_app_boot kgc_ui kgc_shell kgc_appSettings kgc_uiGallery kgc_analyze kgc_secondPanel
        kgc_vehicleInfo kgc_logs kgc_preflight kgc_composition)
qt_add_executable(KGroundControl main.cpp)
if(WIN32)
    enable_language(RC)
    target_sources(KGroundControl PRIVATE app.rc)
    target_include_directories(KGroundControl PRIVATE "${PROJECT_SOURCE_DIR}")
endif()
set_target_properties(KGroundControl PROPERTIES WIN32_EXECUTABLE TRUE)
target_link_libraries(KGroundControl PRIVATE kgc_composition Qt6::QuickControls2)
include("${PROJECT_SOURCE_DIR}/cmake/notices/Notices.cmake")
