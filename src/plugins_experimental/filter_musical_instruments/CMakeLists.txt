# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES filter_musical_instruments.cpp)

set(HEADERS filter_musical_instruments.h)

add_library(filter_musical_instruments MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_musical_instruments
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_musical_instruments PUBLIC common)

set_property(TARGET filter_musical_instruments PROPERTY FOLDER Plugins)

set_property(TARGET filter_musical_instruments
             PROPERTY RUNTIME_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_musical_instruments
             PROPERTY LIBRARY_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS filter_musical_instruments DESTINATION
                ${MESHLAB_PLUGIN_INSTALL_DIR} COMPONENT Plugins)
