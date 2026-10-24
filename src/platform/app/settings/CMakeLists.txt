# [0.0.1 T1] specs/features/0.0.1-shell.md
file(GLOB headers CONFIGURE_DEPENDS api/kgc/app/settings/*.h)
add_library(kgc_app_settings STATIC internal/Settings.cpp "${CMAKE_BINARY_DIR}/gen/kgc/app/settings/Registry.cpp" ${headers})
target_include_directories(kgc_app_settings PUBLIC api)
target_link_libraries(kgc_app_settings PUBLIC kgc_app_errors Qt6::Core)
