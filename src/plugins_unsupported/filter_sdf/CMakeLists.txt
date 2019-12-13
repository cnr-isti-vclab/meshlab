# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES filter_sdf+.cpp filter_sdf.cpp)

set(HEADERS filter_sdf.h filterinterface.h mysampling.h)

add_library(filter_sdf MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_sdf PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_sdf PUBLIC common)

set_property(TARGET filter_sdf PROPERTY FOLDER Plugins)

set_property(TARGET filter_sdf PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_sdf PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS filter_sdf DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR} COMPONENT
                Plugins)
