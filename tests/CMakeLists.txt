# [0.0.1 T15] specs/features/0.0.1-shell.md
qt_add_executable(test_panel_order test_panel_order.cpp)
target_link_libraries(test_panel_order PRIVATE kgc_ui Qt6::Test)
add_test(NAME panel_order COMMAND test_panel_order)

# [0.0.2 T20 T21] specs/features/0.0.2-theme.md
qt_add_executable(test_settings_range test_settings_range.cpp)
target_link_libraries(test_settings_range PRIVATE kgc_app_settings Qt6::Test)
add_test(NAME settings_range COMMAND test_settings_range)
qt_add_executable(test_slot_model test_slot_model.cpp)
target_link_libraries(test_slot_model PRIVATE kgc_ui Qt6::Test)
add_test(NAME slot_model COMMAND test_slot_model)
qt_add_executable(test_theme_choice test_theme_choice.cpp)
target_link_libraries(test_theme_choice PRIVATE kgc_ui Qt6::Test)
add_test(NAME theme_choice COMMAND test_theme_choice)

# [0.0.3 T8] specs/features/0.0.3-windows.md
qt_add_executable(test_log_feed test_log_feed.cpp)
target_link_libraries(test_log_feed PRIVATE kgc_app_log Qt6::Test)
add_test(NAME log_feed COMMAND test_log_feed)
qt_add_executable(test_window_placement test_window_placement.cpp)
target_link_libraries(test_window_placement PRIVATE kgc_ui Qt6::Test)
add_test(NAME window_placement COMMAND test_window_placement)
qt_add_executable(test_cell_arrangement test_cell_arrangement.cpp)
target_link_libraries(test_cell_arrangement PRIVATE kgc_secondPanel Qt6::Test)
add_test(NAME cell_arrangement COMMAND test_cell_arrangement)
