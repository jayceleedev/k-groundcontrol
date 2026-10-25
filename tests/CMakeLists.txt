# [0.0.1 T15] specs/features/0.0.1-shell.md
qt_add_executable(test_panel_order test_panel_order.cpp)
target_link_libraries(test_panel_order PRIVATE kgc_ui Qt6::Test)
add_test(NAME panel_order COMMAND test_panel_order)

# [0.0.2 T20 T21] specs/features/0.0.2-theme.md
qt_add_executable(test_settings_range test_settings_range.cpp)
target_link_libraries(test_settings_range PRIVATE kgc_app_settings Qt6::Test)
add_test(NAME settings_range COMMAND test_settings_range)
qt_add_executable(test_slot_model test_slot_model.cpp)
target_link_libraries(test_slot_model PRIVATE kgc_ui)
add_test(NAME slot_model COMMAND test_slot_model)
qt_add_executable(test_theme_choice test_theme_choice.cpp)
target_link_libraries(test_theme_choice PRIVATE kgc_ui Qt6::Test)
add_test(NAME theme_choice COMMAND test_theme_choice)
