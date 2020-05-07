# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/filter_qhull.cmake (custom for this directory),
### then re-run ./make-cmake.py

# Only build if we have qhull
if(TARGET external-qhull)

    set(SOURCES filter_qhull.cpp qhull_tools.cpp)

    set(HEADERS filter_qhull.h qhull_tools.h)

    add_library(filter_qhull MODULE ${SOURCES} ${HEADERS})

    target_include_directories(filter_qhull PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(filter_qhull PUBLIC common)

    target_link_libraries(filter_qhull PRIVATE external-qhull)

    set_property(TARGET filter_qhull PROPERTY FOLDER Plugins)

    set_property(TARGET filter_qhull PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                              ${MESHLAB_PLUGIN_OUTPUT_DIR})

    set_property(TARGET filter_qhull PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                              ${MESHLAB_PLUGIN_OUTPUT_DIR})

    install(
        TARGETS filter_qhull
        DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
        COMPONENT Plugins)

else()
    message(STATUS "Skipping filter_qhull - missing qhull")
endif()
