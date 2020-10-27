# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/filter_watermark.cmake,
### then re-run ./make-cmake.py

set(SOURCES filter_watermark.cpp utilsWatermark.cpp)

set(HEADERS ERF.h filter_watermark.h utilsWatermark.h)

add_library(filter_watermark MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_watermark PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_watermark PUBLIC common)

set_property(TARGET filter_watermark PROPERTY FOLDER Plugins)

set_property(TARGET filter_watermark PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                              ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_watermark PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                              ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_watermark
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
