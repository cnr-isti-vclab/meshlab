# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/filter_plymc.cmake (custom for this directory),
### then re-run ./make-cmake.py

set(SOURCES filter_plymc.cpp ${VCGDIR}/wrap/ply/plylib.cpp)

set(HEADERS filter_plymc.h)

add_library(filter_plymc MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_plymc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_plymc PUBLIC meshlab-common)

target_link_libraries(filter_plymc PRIVATE OpenGL::GLU)

set_property(TARGET filter_plymc PROPERTY FOLDER Plugins)

set_property(TARGET filter_plymc PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                          ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_plymc PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                          ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_plymc
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
