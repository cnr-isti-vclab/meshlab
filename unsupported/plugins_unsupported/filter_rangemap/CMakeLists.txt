# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_rangemap.cpp)

set(HEADERS filter_rangemap.h)

add_library(filter_rangemap MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_rangemap PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_rangemap PUBLIC common)

set_property(TARGET filter_rangemap PROPERTY FOLDER Plugins)

set_property(TARGET filter_rangemap PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                             ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_rangemap PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                             ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_rangemap
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
