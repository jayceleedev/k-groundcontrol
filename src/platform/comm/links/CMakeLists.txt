# [0.1.0 T1–T6] specs/features/0.1.0-link.md
file(GLOB_RECURSE sources CONFIGURE_DEPENDS api/*.h internal/*.h internal/*.cpp serial/*.h serial/*.cpp udp/*.h udp/*.cpp)
add_library(kgc_comm_links STATIC ${sources})
target_include_directories(kgc_comm_links PUBLIC api PRIVATE internal)
target_link_libraries(kgc_comm_links PUBLIC kgc_app_errors kgc_comm_mavlink kgc_app_scheduler kgc_app_log Qt6::Network Qt6::SerialPort)
