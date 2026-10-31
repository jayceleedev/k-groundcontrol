# [0.2.0 T5] specs/features/0.2.0-fcflash.md
file(GLOB_RECURSE sources CONFIGURE_DEPENDS api/*.h internal/*.h internal/*.cpp)
add_library(kgc_vehicle_safety STATIC ${sources})
target_include_directories(kgc_vehicle_safety PUBLIC api PRIVATE internal)
target_link_libraries(kgc_vehicle_safety PUBLIC kgc_app_errors kgc_app_job kgc_vehicle_registry Qt6::Core PRIVATE kgc_app_log)
