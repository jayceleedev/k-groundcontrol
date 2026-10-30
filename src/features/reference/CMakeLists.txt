# [0.0.4 T41 T42 T43] specs/features/0.0.4-righttabs.md
file(GLOB headers CONFIGURE_DEPENDS api/kgc/reference/*.h)
set(panels ReferencePanel ReferenceCard)
set(qml_files)
foreach(panel IN LISTS panels)
    set_source_files_properties(ui/${panel}.qml PROPERTIES QT_RESOURCE_ALIAS ${panel}.qml)
    list(APPEND qml_files ui/${panel}.qml)
endforeach()
qt_add_qml_module(kgc_reference STATIC URI Kgc.Reference
    OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml/Kgc/Reference"
    QML_FILES ${qml_files}
    SOURCES ${headers} internal/ReferenceService.cpp ui/register.cpp)
target_include_directories(kgc_reference PUBLIC api PRIVATE internal ui)
target_link_libraries(kgc_reference PUBLIC kgc_app_errors kgc_ui)
