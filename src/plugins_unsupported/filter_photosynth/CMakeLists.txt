# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES SoapTest/main.cpp downloader.cpp filter_photosynth.cpp)

set(HEADERS SoapTest/handler.h filter_photosynth.h synthData.h)

add_library(filter_photosynth MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_photosynth
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_photosynth PUBLIC common)

set_property(TARGET filter_photosynth PROPERTY FOLDER Plugins)

set_property(TARGET filter_photosynth PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                               ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_photosynth PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                               ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS filter_photosynth DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
                COMPONENT Plugins)
