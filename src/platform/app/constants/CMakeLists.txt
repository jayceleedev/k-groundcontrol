# [0.1.0 T3] specs/features/0.1.0-link.md
add_library(kgc_app_constants INTERFACE)
target_include_directories(kgc_app_constants INTERFACE api)
target_link_libraries(kgc_app_constants INTERFACE kgc_app_errors)
