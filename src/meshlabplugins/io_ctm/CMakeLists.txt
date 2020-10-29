# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/io_ctm.cmake (custom for this directory),
### then re-run ./make-cmake.py

# Only build if we have OpenCTM
if(TARGET external-openctm)

    set(SOURCES io_ctm.cpp)

    set(HEADERS io_ctm.h)

    add_library(io_ctm MODULE ${SOURCES} ${HEADERS})

    target_include_directories(io_ctm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(io_ctm PUBLIC meshlab-common)

    target_link_libraries(io_ctm PRIVATE external-openctm)

    set_property(TARGET io_ctm PROPERTY FOLDER Plugins)

    set_property(TARGET io_ctm PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

    set_property(TARGET io_ctm PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

    install(
        TARGETS io_ctm
        DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
        COMPONENT Plugins)

else()
    message(STATUS "Skipping io_ctm - missing OpenCTM.")
endif()
