# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0W

# Only build if we have opengr
if(TARGET external-opengr)

    set(SOURCES
        globalregistration.cpp)

    set(HEADERS
        globalregistration.h)

    add_library(globalregistration MODULE ${SOURCES} ${HEADERS})

    target_include_directories(globalregistration PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(globalregistration PUBLIC common)

    target_link_libraries(globalregistration PRIVATE external-opengr)
    if(OpenMP_CXX_FOUND)
        target_link_libraries(globalregistration PRIVATE OpenMP::OpenMP_CXX)
    endif()

    set_property(TARGET globalregistration PROPERTY FOLDER Plugins)

    set_property(TARGET globalregistration PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                            ${MESHLAB_PLUGIN_OUTPUT_DIR})

    set_property(TARGET globalregistration PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                            ${MESHLAB_PLUGIN_OUTPUT_DIR})

    install(
        TARGETS globalregistration
        DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
        COMPONENT Plugins)

else()
    message(
        STATUS "Skipping globalregistration - missing opengr in external directory.")
endif()
