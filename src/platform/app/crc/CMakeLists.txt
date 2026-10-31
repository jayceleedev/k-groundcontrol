# [0.2.0 T1] specs/features/0.2.0-fcflash.md
add_library(kgc_app_crc INTERFACE)
target_include_directories(kgc_app_crc INTERFACE api)
target_link_libraries(kgc_app_crc INTERFACE kgc_app_errors)
