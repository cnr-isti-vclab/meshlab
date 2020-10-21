# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/filter_feature_alignment.cmake,
### then re-run ./make-cmake.py

set(SOURCES filter_feature_alignment.cpp)

set(HEADERS feature_alignment.h feature_msc.h feature_rgb.h
            filter_feature_alignment.h)

add_library(filter_feature_alignment MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_feature_alignment
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_feature_alignment PUBLIC common)

set_property(TARGET filter_feature_alignment PROPERTY FOLDER Plugins)

set_property(TARGET filter_feature_alignment
             PROPERTY RUNTIME_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_feature_alignment
             PROPERTY LIBRARY_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_feature_alignment
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
