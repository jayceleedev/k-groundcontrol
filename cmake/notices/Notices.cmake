# [0.0.1 T9] specs/features/0.0.1-shell.md
file(READ "${PROJECT_SOURCE_DIR}/cmake/notices/extra.json" notices)
set(output "# Third-party notices\n\nK-GroundControl ${PROJECT_VERSION}\nSource: ${PROJECT_HOMEPAGE_URL}\n\n")
string(JSON count LENGTH "${notices}")
math(EXPR last "${count} - 1")
foreach(index RANGE 0 ${last})
    foreach(key name version license url platform)
        string(JSON ${key} GET "${notices}" ${index} ${key})
    endforeach()
    if(platform STREQUAL "all" OR platform STREQUAL CMAKE_SYSTEM_NAME)
        string(APPEND output "## ${name} ${version}\nLicense: ${license}\nSource and license terms: ${url}\n\n")
    endif()
endforeach()
# 첫 버전에는 등록된 외부 패키지가 없으므로 추가 고지 목록만 합쳐요.
file(WRITE "${CMAKE_BINARY_DIR}/THIRD_PARTY_NOTICES.md" "${output}")
add_custom_command(TARGET KGroundControl POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/LICENSE" "${CMAKE_BINARY_DIR}/THIRD_PARTY_NOTICES.md"
        "$<TARGET_FILE_DIR:KGroundControl>")
add_custom_target(package
    COMMAND ${CMAKE_COMMAND}
        "-DEXECUTABLE=$<TARGET_FILE:KGroundControl>"
        "-DPROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}"
        "-DPROJECT_BINARY_DIR=${CMAKE_BINARY_DIR}"
        "-DPROJECT_VERSION=${PROJECT_VERSION}"
        "-DQT_ROOT=${QT6_INSTALL_PREFIX}"
        -P "${PROJECT_SOURCE_DIR}/cmake/deploy/$<IF:$<PLATFORM_ID:Windows>,windows,linux>.cmake"
    DEPENDS KGroundControl)
