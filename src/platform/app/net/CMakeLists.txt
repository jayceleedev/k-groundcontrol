# [0.2.0 T3] specs/features/0.2.0-fcflash.md
CPMAddPackage(NAME zlib EXCLUDE_FROM_ALL YES
    URL "https://github.com/madler/zlib/releases/download/v1.3.1/zlib-1.3.1.tar.gz"
    URL_HASH SHA256=9a93b2b7dfdac77ceba5a558a580e74667dd6fede4585b91eefb60f03b72df23
    OPTIONS "ZLIB_BUILD_EXAMPLES OFF")
file(GLOB_RECURSE sources CONFIGURE_DEPENDS api/*.h internal/*.h internal/*.cpp)
add_library(kgc_app_net STATIC ${sources})
target_include_directories(kgc_app_net PUBLIC api PRIVATE internal)
target_include_directories(kgc_app_net SYSTEM PRIVATE "${zlib_SOURCE_DIR}" "${zlib_BINARY_DIR}")
target_compile_definitions(kgc_app_net PRIVATE KGC_VERSION="${PROJECT_VERSION}")
target_link_libraries(kgc_app_net PUBLIC kgc_app_errors Qt6::Network PRIVATE kgc_app_log zlibstatic)
