# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES edit_measure.cpp edit_measure_factory.cpp
            ${VCGDIR}/wrap/gui/rubberband.cpp)

set(HEADERS edit_measure.h edit_measure_factory.h)

set(RESOURCES edit_measure.qrc)

add_library(edit_measure MODULE ${SOURCES} ${HEADERS} ${RESOURCES})

target_include_directories(edit_measure PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(edit_measure PUBLIC common)

target_link_libraries(edit_measure PRIVATE OpenGL::GLU)

set_property(TARGET edit_measure PROPERTY FOLDER Plugins)

set_property(TARGET edit_measure PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                          ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET edit_measure PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                          ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS edit_measure DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
                COMPONENT Plugins)
