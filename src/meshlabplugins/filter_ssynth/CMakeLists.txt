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

	add_meshlab_plugin(filter_ssynth ${SOURCES} ${HEADERS} ${RESOURCES})

    target_link_libraries(filter_ssynth PRIVATE external-ssynth)

else()
    message(STATUS "Skipping filter_ssynth - missing structure-synth sources.")
endif()
