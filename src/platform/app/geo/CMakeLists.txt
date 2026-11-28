# [0.6.0 5.1] specs/features/0.6.0-fly.md
file(GLOB_RECURSE sources CONFIGURE_DEPENDS api/*.h internal/*.h internal/*.cpp)
add_library(kgc_app_geo STATIC ${sources})
target_include_directories(kgc_app_geo PUBLIC api PRIVATE internal)
target_link_libraries(kgc_app_geo PUBLIC kgc_app_errors Qt6::Core)
