# [0.0.1 T1] specs/features/0.0.1-shell.md
file(GLOB headers CONFIGURE_DEPENDS api/kgc/app/boot/*.h)
add_library(kgc_app_boot STATIC internal/BootOptions.cpp ${headers})
target_include_directories(kgc_app_boot PUBLIC api)
target_link_libraries(kgc_app_boot PUBLIC kgc_app_errors Qt6::Core)
