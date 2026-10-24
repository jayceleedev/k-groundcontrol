# [0.0.1 T1] specs/features/0.0.1-shell.md
file(GLOB headers CONFIGURE_DEPENDS api/kgc/app/log/*.h)
add_library(kgc_app_log STATIC internal/LogSink.cpp internal/Format.cpp ${headers})
target_include_directories(kgc_app_log PUBLIC api)
target_link_libraries(kgc_app_log PUBLIC kgc_app_errors Qt6::Core)
