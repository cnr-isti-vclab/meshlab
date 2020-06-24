# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/filter_mutualinfo_xml.cmake,
### then re-run ./make-cmake.py

set(SOURCES
    alignset.cpp
    filter_mutualinfo_xml.cpp
    levmarmethods.cpp
    mutual.cpp
    parameters.cpp
    pointCorrespondence.cpp
    solver.cpp)

set(HEADERS
    alignset.h
    filter_mutualinfo_xml.h
    levmarmethods.h
    mutual.h
    parameters.h
    pointCorrespondence.h
    pointOnLayer.h
    shutils.h
    solver.h)

add_library(filter_mutualinfo_xml MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_mutualinfo_xml
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_mutualinfo_xml PUBLIC common)

set_property(TARGET filter_mutualinfo_xml PROPERTY FOLDER Plugins)

set_property(TARGET filter_mutualinfo_xml PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                                   ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_mutualinfo_xml PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                                   ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_mutualinfo_xml
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
