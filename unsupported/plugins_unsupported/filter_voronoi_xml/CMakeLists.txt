# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_voronoi_xml.cpp)

set(HEADERS filter_voronoi_xml.h)

set(XML filter_voronoi_xml.xml)
meshlab_install_plugin_xml(${CMAKE_CURRENT_SOURCE_DIR}/${XML} XML_OUT)
list(APPEND SOURCES ${XML_OUT})

add_library(filter_voronoi_xml MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_voronoi_xml PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_voronoi_xml PUBLIC common)

set_property(TARGET filter_voronoi_xml PROPERTY FOLDER Plugins)

set_property(TARGET filter_voronoi_xml PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                            ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_voronoi_xml PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                            ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_voronoi_xml
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
