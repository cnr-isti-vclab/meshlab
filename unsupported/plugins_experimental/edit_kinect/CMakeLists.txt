# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/edit_kinect.cmake,
### then re-run ./make-cmake.py

set(SOURCES calibration_load.cpp edit_kinect.cpp edit_kinect_factory.cpp
            freenect.cpp glarea.cpp)

set(HEADERS calibration_data.h edit_kinect.h edit_kinect_factory.h freenect.h
            glarea.h shader_basic.h)

set(RESOURCES edit_kinect.qrc)

set(UI kinect.ui)

add_library(edit_kinect MODULE ${SOURCES} ${HEADERS} ${RESOURCES} ${UI})

target_include_directories(edit_kinect PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(edit_kinect PUBLIC common)

set_property(TARGET edit_kinect PROPERTY FOLDER Plugins)

set_property(TARGET edit_kinect PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                         ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET edit_kinect PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                         ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS edit_kinect
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
