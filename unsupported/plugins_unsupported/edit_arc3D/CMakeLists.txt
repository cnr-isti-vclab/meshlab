# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/edit_arc3D.cmake,
### then re-run ./make-cmake.py

set(SOURCES
    arc3D_camera.cpp
    edit_arc3D.cpp
    edit_arc3D_factory.cpp
    fillImage.cpp
    maskImageWidget.cpp
    maskRenderWidget.cpp
    radial_distortion.cpp
    reconstruction.cpp
    scalar_image.cpp
    v3dImportDialog.cpp)

set(HEADERS
    arc3D_camera.h
    arc3D_reconstruction.h
    edit_arc3D.h
    edit_arc3D_factory.h
    fillImage.h
    maskImageWidget.h
    maskRenderWidget.h
    pushpull.h
    radial_distortion.h
    resource.h
    resource1.h
    scalar_image.h
    v3dImportDialog.h)

set(RESOURCES edit_arc3D.qrc)

set(UI ui/v3dImportDialog.ui)

add_library(edit_arc3D MODULE ${SOURCES} ${HEADERS} ${RESOURCES} ${UI})

target_include_directories(edit_arc3D PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(edit_arc3D PUBLIC common)

set_property(TARGET edit_arc3D PROPERTY FOLDER Plugins)

set_property(TARGET edit_arc3D PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET edit_arc3D PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS edit_arc3D
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
