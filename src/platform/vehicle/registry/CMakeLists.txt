# [0.1.0 T1–T6] specs/features/0.1.0-link.md
file(GLOB_RECURSE sources CONFIGURE_DEPENDS api/*.h internal/*.h internal/*.cpp serial/*.h serial/*.cpp udp/*.h udp/*.cpp)
add_library(kgc_vehicle_registry STATIC ${sources})
target_include_directories(kgc_vehicle_registry PUBLIC api PRIVATE internal)
target_link_libraries(kgc_vehicle_registry PUBLIC kgc_app_errors kgc_vehicle_states kgc_vehicle_outbound kgc_comm_links Qt6::Core)
