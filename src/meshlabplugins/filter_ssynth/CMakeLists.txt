# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


# Only build if we have structure-synth
if(TARGET external-ssynth)

    set(SOURCES
        filter_ssynth.cpp mytrenderer.cpp
        ../../meshlabplugins/io_x3d/vrml/Parser.cpp
        ../../meshlabplugins/io_x3d/vrml/Scanner.cpp)

    set(HEADERS
        filter_ssynth.h
        mytrenderer.h
        ../../meshlabplugins/io_x3d/import_x3d.h
        ../../meshlabplugins/io_x3d/util_x3d.h
        ../../meshlabplugins/io_x3d/vrml/Parser.h
        ../../meshlabplugins/io_x3d/vrml/Scanner.h)

    set(RESOURCES ssynthres.qrc)

    add_library(filter_ssynth MODULE ${SOURCES} ${HEADERS} ${RESOURCES})

    target_include_directories(filter_ssynth
                               PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(filter_ssynth PUBLIC meshlab-common)

    target_link_libraries(filter_ssynth PRIVATE external-ssynth)

    set_property(TARGET filter_ssynth PROPERTY FOLDER Plugins)

    set_property(TARGET filter_ssynth PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                               ${MESHLAB_PLUGIN_OUTPUT_DIR})

    set_property(TARGET filter_ssynth PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                               ${MESHLAB_PLUGIN_OUTPUT_DIR})

    install(
        TARGETS filter_ssynth
        DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
        COMPONENT Plugins)

else()
    message(STATUS "Skipping filter_ssynth - missing structure-synth sources.")
endif()
