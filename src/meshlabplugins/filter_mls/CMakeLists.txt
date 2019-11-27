# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES apss.cpp balltree.cpp mlsplugin.cpp rimls.cpp)

set(HEADERS
    apss.h
    balltree.h
    implicits.h
    mlsmarchingcube.h
    mlsplugin.h
    mlssurface.h
    priorityqueue.h
    rimls.h
    smallcomponentselection.h)

add_library(filter_mls MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_mls PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_mls PUBLIC common)

set_property(TARGET filter_mls PROPERTY FOLDER Plugins)

set_property(TARGET filter_mls PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_mls PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS filter_mls DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR} COMPONENT
                Plugins)
