# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES filter_sketchfab.cpp ${VCGDIR}/wrap/ply/plylib.cpp)

set(HEADERS filter_sketchfab.h)

set(XML filter_sketchfab.xml)
meshlab_install_plugin_xml(${CMAKE_CURRENT_SOURCE_DIR}/${XML} XML_OUT)
list(APPEND SOURCES ${XML_OUT})

add_library(filter_sketchfab MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_sketchfab PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_sketchfab PUBLIC common)

set_property(TARGET filter_sketchfab PROPERTY FOLDER Plugins)

set_property(TARGET filter_sketchfab PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                              ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_sketchfab PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                              ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS filter_sketchfab DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
                COMPONENT Plugins)
