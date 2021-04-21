# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


# Only build if we have newuoa and levmar
if(TARGET external-newuoa AND TARGET external-levmar)

    set(SOURCES
        alignset.cpp
        edit_mutualcorrs.cpp
        edit_mutualcorrsDialog.cpp
        edit_mutualcorrs_factory.cpp
        levmarmethods.cpp
        mutual.cpp
        parameters.cpp
        solver.cpp)

    set(HEADERS
        alignset.h
        edit_mutualcorrs.h
        edit_mutualcorrsDialog.h
        edit_mutualcorrs_factory.h
        levmarmethods.h
        mutual.h
        parameters.h
        shutils.h
        solver.h)

    set(RESOURCES edit_mutualcorrs.qrc)

    set(UI edit_mutualcorrsDialog.ui)

	add_meshlab_plugin(edit_mutualcorrs ${SOURCES} ${HEADERS} ${RESOURCES}
                                        ${UI})

    target_link_libraries(edit_mutualcorrs PRIVATE external-newuoa
                                                   external-levmar)

    target_link_libraries(edit_mutualcorrs PRIVATE OpenGL::GLU)

else()
    message(
        STATUS
            "Skipping edit_mutualcorrs - missing either newuoa or levmar in external directory."
    )
endif()
