# [0.0.2 T1 T10] specs/features/0.0.2-theme.md
file(GLOB headers CONFIGURE_DEPENDS api/kgc/uiGallery/*.h)
set(panels UiGalleryPanel GalleryButtons GalleryChoices GalleryInputs
    GalleryIndicators GalleryContainers GalleryNotices GalleryCharts)
set(qml_files)
foreach(panel IN LISTS panels)
    set_source_files_properties(ui/${panel}.qml PROPERTIES QT_RESOURCE_ALIAS ${panel}.qml)
    list(APPEND qml_files ui/${panel}.qml)
endforeach()
qt_add_qml_module(kgc_uiGallery STATIC URI Kgc.UiGallery
    OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml/Kgc/UiGallery"
    QML_FILES ${qml_files}
    SOURCES ${headers} internal/UiGalleryService.cpp ui/register.cpp)
target_include_directories(kgc_uiGallery PUBLIC api PRIVATE internal ui)
target_link_libraries(kgc_uiGallery PUBLIC kgc_app_errors kgc_ui)
