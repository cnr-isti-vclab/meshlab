# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES dirt_utils.cpp filter_dirt.cpp)

set(HEADERS dirt_utils.h dustparticle.h dustsampler.h filter_dirt.h particle.h)

add_library(filter_dirt MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_dirt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_dirt PUBLIC common)

set_property(TARGET filter_dirt PROPERTY FOLDER Plugins)

set_property(TARGET filter_dirt PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                         ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_dirt PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                         ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS filter_dirt DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR} COMPONENT
                Plugins)
