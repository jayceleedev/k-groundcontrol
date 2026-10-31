# [0.0.1 T1] specs/features/0.0.1-shell.md
add_library(kgc_app_errors INTERFACE)
target_include_directories(kgc_app_errors INTERFACE api)

target_link_libraries(kgc_app_errors INTERFACE Qt6::Core)
