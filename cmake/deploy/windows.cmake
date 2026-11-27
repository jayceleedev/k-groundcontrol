# [0.0.1 T3] specs/features/0.0.1-shell.md
# [0.5.0 T13] specs/features/0.5.0-map.md
set(stage "${PROJECT_BINARY_DIR}/package/K-GroundControl")
# [0.5.0 T13] specs/features/0.5.0-map.md
# 지난 묶기의 플러그인이 남지 않게 배포 폴더를 비우고 시작해요.
file(REMOVE_RECURSE "${stage}")
file(MAKE_DIRECTORY "${stage}")
file(COPY "${EXECUTABLE}" "${PROJECT_SOURCE_DIR}/LICENSE"
    "${PROJECT_BINARY_DIR}/THIRD_PARTY_NOTICES.md" DESTINATION "${stage}")
find_program(WINDEPLOYQT windeployqt HINTS "${QT_ROOT}/bin" REQUIRED)
# [0.5.0 T13] specs/features/0.5.0-map.md
# Qt 지도 플러그인(geoservices)은 Http를 거치지 않아 빼고, SQL 드라이버는 SQLite만 둬요.
execute_process(COMMAND "${WINDEPLOYQT}" --release --compiler-runtime
    --skip-plugin-types geoservices --exclude-plugins qsqlibase,qsqlmimer,qsqlmysql,qsqloci,qsqlodbc,qsqlpsql
    --qmldir "${PROJECT_SOURCE_DIR}/src" "${stage}/KGroundControl.exe"
    COMMAND_ERROR_IS_FATAL ANY)
# [0.5.0 T13] specs/features/0.5.0-map.md
if(EXISTS "${stage}/geoservices")
    message(FATAL_ERROR "Qt 지도 플러그인이 zip에 들어감")
endif()
# 빌드 경로가 압축 파일 안에 들어가지 않도록 배포 폴더에서 묶어요.
execute_process(COMMAND ${CMAKE_COMMAND} -E tar cf
    "${PROJECT_BINARY_DIR}/K-GroundControl-${PROJECT_VERSION}-windows-x86_64.zip"
    --format=zip K-GroundControl
    WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/package" COMMAND_ERROR_IS_FATAL ANY)
