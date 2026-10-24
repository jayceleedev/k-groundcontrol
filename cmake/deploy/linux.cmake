# [0.0.1 T13] specs/features/0.0.1-shell.md
set(tools "${PROJECT_BINARY_DIR}/deploy-tools")
set(stage "${PROJECT_BINARY_DIR}/AppDir")
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
file(COPY_FILE "${PROJECT_SOURCE_DIR}/resources/icons/app.svg" "${tools}/KGroundControl.svg")
find_library(XCB_CURSOR NAMES xcb-cursor REQUIRED)
execute_process(COMMAND ${CMAKE_COMMAND} -E env
    "PATH=${tools}:$ENV{PATH}" "QMAKE=${QT_ROOT}/bin/qmake" "QML_SOURCES_PATHS=${PROJECT_SOURCE_DIR}/src"
    "EXTRA_QT_PLUGINS=svg;iconengines" "VERSION=${PROJECT_VERSION}" "ARCH=x86_64"
    "OUTPUT=${PROJECT_BINARY_DIR}/K-GroundControl-${PROJECT_VERSION}-linux-x86_64.AppImage"
    "${tools}/linuxdeploy-x86_64.AppImage" --appdir "${stage}"
    --executable "${EXECUTABLE}" --library "${XCB_CURSOR}" --plugin qt
    --desktop-file "${tools}/KGroundControl.desktop" --icon-file "${tools}/KGroundControl.svg"
    --output appimage WORKING_DIRECTORY "${tools}" COMMAND_ERROR_IS_FATAL ANY)
