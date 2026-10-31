# [0.1.0 T1] specs/features/0.1.0-link.md
CPMAddPackage(NAME c_library_v2 DOWNLOAD_ONLY YES
    URL "https://github.com/mavlink/c_library_v2/archive/a2d0ecf011a4aaa7840480e43f4e38d8c9e79d29.tar.gz"
    URL_HASH SHA256=3fc41a3cd46717b1aa226b958aed0829fbe7b7c9793bc8ffdcca76345a9dfde3)
# [0.1.0 T1–T6] specs/features/0.1.0-link.md
file(GLOB_RECURSE sources CONFIGURE_DEPENDS api/*.h internal/*.h internal/*.cpp serial/*.h serial/*.cpp udp/*.h udp/*.cpp)
add_library(kgc_comm_mavlink STATIC ${sources})
target_include_directories(kgc_comm_mavlink PUBLIC api PRIVATE internal)
target_link_libraries(kgc_comm_mavlink PUBLIC kgc_app_errors Qt6::Core)
target_include_directories(kgc_comm_mavlink SYSTEM PUBLIC "${c_library_v2_SOURCE_DIR}")
