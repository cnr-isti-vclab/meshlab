# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/io_epoch.cmake,
### then re-run ./make-cmake.py

set(SOURCES
    epoch_camera.cpp
    epoch_io.cpp
    fillImage.cpp
    maskImageWidget.cpp
    maskRenderWidget.cpp
    radial_distortion.cpp
    reconstruction.cpp
    scalar_image.cpp
    v3dImportDialog.cpp)

set(HEADERS
    epoch_camera.h
    epoch_io.h
    epoch_reconstruction.h
    fillImage.h
    maskImageWidget.h
    maskRenderWidget.h
    radial_distortion.h
    scalar_image.h
    v3dImportDialog.h)

set(UI ui/v3dImportDialog.ui)

add_library(io_epoch MODULE ${SOURCES} ${HEADERS} ${UI})

target_include_directories(io_epoch PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(io_epoch PUBLIC common)

set_property(TARGET io_epoch PROPERTY FOLDER Plugins)

set_property(TARGET io_epoch PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                      ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET io_epoch PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                      ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS io_epoch
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
