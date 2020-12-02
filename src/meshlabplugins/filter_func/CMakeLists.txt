# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


# Only build if we have muparser
if(TARGET external-muparser)

    set(SOURCES filter_func.cpp)

    set(HEADERS filter_func.h filter_refine.h string_conversion.h)

    add_library(filter_func MODULE ${SOURCES} ${HEADERS})

    target_include_directories(filter_func PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(filter_func PUBLIC meshlab-common)

    target_link_libraries(filter_func PRIVATE external-muparser)

    set_property(TARGET filter_func PROPERTY FOLDER Plugins)

    set_property(TARGET filter_func PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                             ${MESHLAB_PLUGIN_OUTPUT_DIR})

    set_property(TARGET filter_func PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                             ${MESHLAB_PLUGIN_OUTPUT_DIR})

    install(
        TARGETS filter_func
        DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
        COMPONENT Plugins)

else()
    message(STATUS "Skipping filter_func - don't have muparser.")
endif()
