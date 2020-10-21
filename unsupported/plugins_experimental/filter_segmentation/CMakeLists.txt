# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/filter_segmentation.cmake,
### then re-run ./make-cmake.py

set(SOURCES HC_Segmenter.cpp RG_Segmenter.cpp Segmenter.cpp Utils.cpp
            meshsegmentation.cpp)

set(HEADERS Cluster.h HC_Segmenter.h RG_Segmenter.h Segmenter.h Utils.h
            meshsegmentation.h)

add_library(filter_segmentation MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_segmentation
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_segmentation PUBLIC common)

set_property(TARGET filter_segmentation PROPERTY FOLDER Plugins)

set_property(TARGET filter_segmentation PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                                 ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_segmentation PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                                 ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_segmentation
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
