# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES filter_zippering.cpp)

set(HEADERS filter_zippering.h)

add_library(filter_zippering MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_zippering PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_zippering PUBLIC common)

set_property(TARGET filter_zippering PROPERTY FOLDER Plugins)

set_property(TARGET filter_zippering PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                              ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_zippering PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                              ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS filter_zippering DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
                COMPONENT Plugins)
