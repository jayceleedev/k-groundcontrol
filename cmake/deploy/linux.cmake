# [0.0.1 T13] specs/features/0.0.1-shell.md
# [0.0.2 T27] specs/features/0.0.2-theme.md
# [0.5.0 T13] specs/features/0.5.0-map.md
set(tools "${PROJECT_BINARY_DIR}/deploy-tools")
set(stage "${PROJECT_BINARY_DIR}/AppDir")
# [0.5.0 T13] specs/features/0.5.0-map.md
# 지난 묶기의 플러그인이 남지 않게 배포 폴더를 비우고 시작해요.
file(REMOVE_RECURSE "${stage}")
file(MAKE_DIRECTORY "${tools}" "${stage}/usr/bin")
set(linuxdeploy_tag "1-alpha-20250213-2")
set(linuxdeploy_sha256 "4648f278ab3ef31f819e67c30d50f462640e5365a77637d7e6f2ad9fd0b4522a")
set(linuxdeploy-plugin-qt_tag "1-alpha-20250213-1")
set(linuxdeploy-plugin-qt_sha256 "15106be885c1c48a021198e7e1e9a48ce9d02a86dd0a1848f00bdbf3c1c92724")
foreach(tool linuxdeploy linuxdeploy-plugin-qt)
    set(path "${tools}/${tool}-x86_64.AppImage")
    file(DOWNLOAD "https://github.com/linuxdeploy/${tool}/releases/download/${${tool}_tag}/${tool}-x86_64.AppImage"
        "${path}" EXPECTED_HASH "SHA256=${${tool}_sha256}" TLS_VERIFY ON)
    file(CHMOD "${path}" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endforeach()
file(COPY "${PROJECT_SOURCE_DIR}/LICENSE" "${PROJECT_BINARY_DIR}/THIRD_PARTY_NOTICES.md"
    DESTINATION "${stage}/usr/bin")
file(WRITE "${tools}/KGroundControl.desktop"
    "[Desktop Entry]\nType=Application\nName=K-GroundControl\nExec=KGroundControl\nIcon=KGroundControl\nCategories=Science;\n")
file(COPY_FILE "${PROJECT_SOURCE_DIR}/resources/icons/app-tile.svg" "${tools}/KGroundControl.svg")
# 스크립트 실행에는 컴파일러 초기화가 없으므로 구성 때 얻은 다중 아키텍처 경로를 써요.
find_library(XCB_CURSOR NAMES xcb-cursor libxcb-cursor.so.0
    PATHS "/usr/lib/${CMAKE_LIBRARY_ARCHITECTURE}" "/lib/${CMAKE_LIBRARY_ARCHITECTURE}" REQUIRED)
# [0.5.0 T13] specs/features/0.5.0-map.md
# Qt 지도 플러그인(geoservices)은 Http를 거치지 않아 빼고, SQL 드라이버는 SQLite만 둬요.
# linuxdeploy-plugin-qt에는 빼기 설정이 없어서 걸러 낸 플러그인 사본을 qmake -query가 가리키게 해요.
set(plugins "${tools}/qt-plugins")
file(REMOVE_RECURSE "${plugins}")
file(COPY "${QT_ROOT}/plugins/" DESTINATION "${plugins}" PATTERN "geoservices" EXCLUDE PATTERN "sqldrivers" EXCLUDE)
file(COPY "${QT_ROOT}/plugins/sqldrivers/libqsqlite.so" DESTINATION "${plugins}/sqldrivers")
# Qt Location을 쓰면 linuxdeploy-plugin-qt가 geoservices 폴더를 꼭 열어 보므로 빈 폴더를 둬요.
file(MAKE_DIRECTORY "${plugins}/geoservices")
file(WRITE "${tools}/qt.conf" "[Paths]\nPrefix=${QT_ROOT}\nPlugins=${plugins}\n")
file(WRITE "${tools}/qmake" "#!/bin/sh\nexec \"${QT_ROOT}/bin/qmake\" -qtconf \"${tools}/qt.conf\" \"$@\"\n")
file(CHMOD "${tools}/qmake" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
execute_process(COMMAND ${CMAKE_COMMAND} -E env
    "PATH=${tools}:$ENV{PATH}" "QMAKE=${tools}/qmake" "QML_SOURCES_PATHS=${PROJECT_SOURCE_DIR}/src"
    "EXTRA_QT_PLUGINS=tls;svg;iconengines" "VERSION=${PROJECT_VERSION}" "ARCH=x86_64"
    "OUTPUT=${PROJECT_BINARY_DIR}/K-GroundControl-${PROJECT_VERSION}-linux-x86_64.AppImage"
    "${tools}/linuxdeploy-x86_64.AppImage" --appdir "${stage}"
    --executable "${EXECUTABLE}" --library "${XCB_CURSOR}" --plugin qt
    --desktop-file "${tools}/KGroundControl.desktop" --icon-file "${tools}/KGroundControl.svg"
    --output appimage WORKING_DIRECTORY "${tools}" COMMAND_ERROR_IS_FATAL ANY)
# [0.5.0 T13] specs/features/0.5.0-map.md
if(EXISTS "${stage}/usr/plugins/geoservices")
    message(FATAL_ERROR "Qt 지도 플러그인이 AppImage에 들어감")
endif()
