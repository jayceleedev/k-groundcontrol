# [0.5.0 S1 T10] specs/features/0.5.0-map.md
file(GLOB_RECURSE sources CONFIGURE_DEPENDS api/*.h internal/*.h internal/*.cpp)
add_library(kgc_app_store STATIC ${sources})
target_include_directories(kgc_app_store PUBLIC api PRIVATE internal)
target_link_libraries(kgc_app_store PUBLIC kgc_app_errors Qt6::Core PRIVATE kgc_app_log Qt6::Sql)
