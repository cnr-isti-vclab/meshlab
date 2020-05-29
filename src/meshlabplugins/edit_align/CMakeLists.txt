# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/edit_align.cmake (custom for this directory),
### then re-run ./make-cmake.py

# Only build if we have newuoa
if(TARGET external-newuoa)

    set(SOURCES
        align/align_parameter.cpp
        align/AlignGlobal.cpp
        align/OccupancyGrid.cpp
        alignDialog.cpp
        AlignPairDialog.cpp
        AlignPairWidget.cpp
        edit_align_factory.cpp
        edit_align.cpp
        meshtree.cpp
        ../../meshlab/stdpardialog.cpp
        ${VCGDIR}/wrap/gui/trackball.cpp
        ${VCGDIR}/wrap/gui/trackmode.cpp
        ${VCGDIR}/wrap/ply/plylib.cpp)

    set(HEADERS
        align/align_parameter.h
        align/AlignGlobal.h
        align/OccupancyGrid.h
        alignDialog.h
        AlignPairDialog.h
        AlignPairWidget.h
        edit_align_factory.h
        edit_align.h
        meshtree.h
        ../../meshlab/stdpardialog.h
        ${VCGDIR}/wrap/gui/trackball.h
        ${VCGDIR}/wrap/gui/trackmode.h
        ${VCGDIR}/wrap/gl/trimesh.h)

    set(RESOURCES edit_align.qrc)

    set(UI alignDialog.ui)

    add_library(edit_align MODULE ${SOURCES} ${HEADERS} ${RESOURCES} ${UI})

    target_include_directories(edit_align PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(edit_align PUBLIC common)

    target_link_libraries(edit_align PRIVATE external-newuoa OpenGL::GLU)
    if(OpenMP_CXX_FOUND)
        target_link_libraries(edit_align PRIVATE OpenMP::OpenMP_CXX)
    endif()

    set_property(TARGET edit_align PROPERTY FOLDER Plugins)

    set_property(TARGET edit_align PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                            ${MESHLAB_PLUGIN_OUTPUT_DIR})

    set_property(TARGET edit_align PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                            ${MESHLAB_PLUGIN_OUTPUT_DIR})

    install(
        TARGETS edit_align
        DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
        COMPONENT Plugins)

else()
    message(
        STATUS "Skipping edit_align - missing newuoa in external directory.")
endif()
