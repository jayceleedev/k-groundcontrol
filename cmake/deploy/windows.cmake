# [0.0.1 T3] specs/features/0.0.1-shell.md
set(stage "${PROJECT_BINARY_DIR}/package/K-GroundControl")
file(MAKE_DIRECTORY "${stage}")
file(COPY "${EXECUTABLE}" "${PROJECT_SOURCE_DIR}/LICENSE"
    "${PROJECT_BINARY_DIR}/THIRD_PARTY_NOTICES.md" DESTINATION "${stage}")
find_program(WINDEPLOYQT windeployqt HINTS "${QT_ROOT}/bin" REQUIRED)
execute_process(COMMAND "${WINDEPLOYQT}" --release --compiler-runtime
    --qmldir "${PROJECT_SOURCE_DIR}/src" "${stage}/KGroundControl.exe"
    COMMAND_ERROR_IS_FATAL ANY)
# 빌드 경로가 압축 파일 안에 들어가지 않도록 배포 폴더에서 묶어요.
execute_process(COMMAND ${CMAKE_COMMAND} -E tar cf
    "${PROJECT_BINARY_DIR}/K-GroundControl-${PROJECT_VERSION}-windows-x86_64.zip"
    --format=zip K-GroundControl
    WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/package" COMMAND_ERROR_IS_FATAL ANY)
