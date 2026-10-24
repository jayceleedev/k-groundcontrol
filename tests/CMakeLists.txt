# [0.0.1 T15] specs/features/0.0.1-shell.md
qt_add_executable(test_panel_order test_panel_order.cpp)
target_link_libraries(test_panel_order PRIVATE kgc_ui Qt6::Test)
add_test(NAME panel_order COMMAND test_panel_order)
