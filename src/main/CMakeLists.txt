# [0.0.1 T1] specs/features/0.0.1-shell.md
qt_add_resources(kgc_composition_rc "${PROJECT_SOURCE_DIR}/resources/kgc.qrc")
add_library(kgc_composition STATIC Composition.h Composition.cpp ${kgc_composition_rc})
target_include_directories(kgc_composition PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(kgc_composition PUBLIC
    KGC_VERSION="${PROJECT_VERSION}" KGC_HOMEPAGE_URL="${PROJECT_HOMEPAGE_URL}")
target_link_libraries(kgc_composition PUBLIC kgc_app_errors kgc_app_boot kgc_app_paths
    kgc_app_log kgc_app_settings kgc_app_i18n kgc_ui kgc_shell kgc_appSettings
    kgc_uiplugin kgc_shellplugin kgc_appSettingsplugin)
qt_add_translations(kgc_composition
    TS_FILES "${PROJECT_SOURCE_DIR}/i18n/kgc_en.ts"
    RESOURCE_PREFIX /kgc/i18n
    SOURCE_TARGETS kgc_ui kgc_shell kgc_appSettings kgc_composition)
qt_add_executable(KGroundControl main.cpp)
target_link_libraries(KGroundControl PRIVATE kgc_composition)
include("${PROJECT_SOURCE_DIR}/cmake/notices/Notices.cmake")
