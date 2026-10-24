# [0.0.1 T3] specs/features/0.0.1-shell.md
set(stage "${PROJECT_BINARY_DIR}/package/K-GroundControl")
file(MAKE_DIRECTORY "${stage}")
file(COPY "${EXECUTABLE}" "${PROJECT_SOURCE_DIR}/LICENSE"
    "${PROJECT_BINARY_DIR}/THIRD_PARTY_NOTICES.md" DESTINATION "${stage}")
find_program(WINDEPLOYQT windeployqt HINTS "${QT_ROOT}/bin" REQUIRED)
execute_process(COMMAND "${WINDEPLOYQT}" --release --compiler-runtime
    --qmldir "${PROJECT_SOURCE_DIR}/src" "${stage}/KGroundControl.exe"
    COMMAND_ERROR_IS_FATAL ANY)
file(ARCHIVE_CREATE OUTPUT "${PROJECT_BINARY_DIR}/K-GroundControl-${PROJECT_VERSION}-windows-x86_64.zip"
    PATHS "${stage}" FORMAT zip)
