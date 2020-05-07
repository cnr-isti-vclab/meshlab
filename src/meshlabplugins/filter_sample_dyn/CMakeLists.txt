# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/filter_sample_dyn.cmake,
### then re-run ./make-cmake.py

set(SOURCES filter_sample_dyn.cpp)

set(HEADERS filter_sample_dyn.h)

add_library(filter_sample_dyn MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_sample_dyn
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_sample_dyn PUBLIC common)

set_property(TARGET filter_sample_dyn PROPERTY FOLDER Plugins)

set_property(TARGET filter_sample_dyn PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                               ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_sample_dyn PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                               ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_sample_dyn
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
