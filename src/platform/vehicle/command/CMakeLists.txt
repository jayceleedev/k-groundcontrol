# [0.2.0 T6] specs/features/0.2.0-fcflash.md
file(GLOB_RECURSE sources CONFIGURE_DEPENDS api/*.h internal/*.h internal/*.cpp)
add_library(kgc_vehicle_command STATIC ${sources})
target_include_directories(kgc_vehicle_command PUBLIC api PRIVATE internal)
target_link_libraries(kgc_vehicle_command PUBLIC kgc_app_errors kgc_vehicle_outbound kgc_comm_mavlink kgc_app_constants kgc_app_scheduler Qt6::Core PRIVATE kgc_app_log)
