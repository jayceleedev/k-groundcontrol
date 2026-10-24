# [0.0.1 T1] specs/features/0.0.1-shell.md
file(GLOB headers CONFIGURE_DEPENDS api/kgc/app/i18n/*.h)
add_library(kgc_app_i18n STATIC internal/Translator.cpp ${headers})
target_include_directories(kgc_app_i18n PUBLIC api)
target_link_libraries(kgc_app_i18n PUBLIC kgc_app_errors Qt6::Core)
