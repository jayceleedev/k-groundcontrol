# [0.0.1 T1] specs/features/0.0.1-shell.md
file(GLOB headers CONFIGURE_DEPENDS api/kgc/app/paths/*.h)
add_library(kgc_app_paths STATIC internal/Paths.cpp ${headers})
target_include_directories(kgc_app_paths PUBLIC api)
target_link_libraries(kgc_app_paths PUBLIC kgc_app_errors Qt6::Core)
