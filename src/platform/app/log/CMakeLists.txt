# [0.0.1 T1] specs/features/0.0.1-shell.md
file(GLOB headers CONFIGURE_DEPENDS api/kgc/app/log/*.h)
add_library(kgc_app_log STATIC internal/LogSink.cpp internal/Format.cpp internal/LogFeed.cpp ${headers})
target_include_directories(kgc_app_log PUBLIC api)
target_link_libraries(kgc_app_log PUBLIC kgc_app_errors Qt6::Core)
# 로그 매크로가 __VA_OPT__를 써서, MSVC는 표준 전처리기를 켜야 읽어요. 매크로를 쓰는 모듈에도 따라가도록 PUBLIC으로 둬요.
target_compile_options(kgc_app_log PUBLIC $<$<CXX_COMPILER_ID:MSVC>:/Zc:preprocessor>)
