# [0.2.0 T4] specs/features/0.2.0-fcflash.md
file(GLOB_RECURSE sources CONFIGURE_DEPENDS api/*.h internal/*.h internal/*.cpp)
add_library(kgc_comm_rawport STATIC ${sources})
target_include_directories(kgc_comm_rawport PUBLIC api PRIVATE internal)
target_link_libraries(kgc_comm_rawport PUBLIC kgc_app_errors Qt6::Core PRIVATE kgc_app_log Qt6::SerialPort)
