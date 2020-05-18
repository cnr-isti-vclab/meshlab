# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/filter_mutualinfo.cmake (custom for this directory),
### then re-run ./make-cmake.py

# Only build if we have newuoa and levmar
if(TARGET external-newuoa AND TARGET external-levmar)

    set(SOURCES
        alignGlobal.cpp
        alignset.cpp
        filter_mutualinfo.cpp
        levmarmethods.cpp
        mutual.cpp
        parameters.cpp
        pointCorrespondence.cpp
        solver.cpp)

    set(HEADERS
        alignGlobal.h
        alignset.h
        filter_mutualinfo.h
        levmarmethods.h
        mutual.h
        parameters.h
        pointCorrespondence.h
        pointOnLayer.h
        shutils.h
        solver.h)

    add_library(filter_mutualinfo MODULE ${SOURCES} ${HEADERS})

    target_include_directories(filter_mutualinfo
                               PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(filter_mutualinfo PUBLIC common)

    target_link_libraries(filter_mutualinfo PRIVATE external-newuoa
                                                      external-levmar)

    set_property(TARGET filter_mutualinfo PROPERTY FOLDER Plugins)

    set_property(TARGET filter_mutualinfo
                 PROPERTY RUNTIME_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

    set_property(TARGET filter_mutualinfo
                 PROPERTY LIBRARY_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

    install(
        TARGETS filter_mutualinfo
        DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
        COMPONENT Plugins)

else()
    message(
        STATUS
            "Skipping filter_mutualinfo - missing either newuoa or levmar in external directory."
    )
endif()
