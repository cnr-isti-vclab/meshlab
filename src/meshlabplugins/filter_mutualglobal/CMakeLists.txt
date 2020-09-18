# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/filter_mutualglobal.cmake (custom for this directory),
### then re-run ./make-cmake.py

# Only build if we have newuoa and levmar
if(TARGET external-newuoa AND TARGET external-levmar)

    set(SOURCES
        alignset.cpp
        filter_mutualglobal.cpp
        levmarmethods.cpp
        mutual.cpp
        parameters.cpp
        pointCorrespondence.cpp
        solver.cpp)

    set(HEADERS
        alignset.h
        filter_mutualglobal.h
        levmarmethods.h
        mutual.h
        parameters.h
        pointCorrespondence.h
        pointOnLayer.h
        shutils.h
        solver.h)

    add_library(filter_mutualglobal MODULE ${SOURCES} ${HEADERS})

    target_include_directories(filter_mutualglobal
                               PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(filter_mutualglobal PUBLIC common)

    target_link_libraries(filter_mutualglobal PRIVATE external-newuoa
                                                      external-levmar)

    set_property(TARGET filter_mutualglobal PROPERTY FOLDER Plugins)

    set_property(TARGET filter_mutualglobal
                 PROPERTY RUNTIME_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

    set_property(TARGET filter_mutualglobal
                 PROPERTY LIBRARY_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

    install(
        TARGETS filter_mutualglobal
        DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
        COMPONENT Plugins)

else()
    message(
        STATUS
            "Skipping filter_mutualglobal - missing either newuoa or levmar in external directory."
    )
endif()
