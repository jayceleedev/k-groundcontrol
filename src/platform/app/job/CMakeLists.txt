# [0.2.0 T2] specs/features/0.2.0-fcflash.md
file(GLOB_RECURSE sources CONFIGURE_DEPENDS api/*.h internal/*.h internal/*.cpp)
add_library(kgc_app_job STATIC ${sources})
target_include_directories(kgc_app_job PUBLIC api PRIVATE internal)
target_link_libraries(kgc_app_job PUBLIC kgc_app_errors Qt6::Core PRIVATE kgc_app_log)
