# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES glstateholder.cpp parser/RmPass.cpp parser/RmXmlParser.cpp
            parser/UniformVar.cpp rmmeshrender.cpp rmshaderdialog.cpp)

set(HEADERS
    glstateholder.h
    parser/GlState.h
    parser/RmEffect.h
    parser/RmPass.h
    parser/RmXmlParser.h
    parser/UniformVar.h
    rmmeshrender.h
    rmshaderdialog.h)

set(UI rmShadowDialog.ui)

add_library(render_rm MODULE ${SOURCES} ${HEADERS} ${UI})

target_include_directories(render_rm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(render_rm PUBLIC common)

set_property(TARGET render_rm PROPERTY FOLDER Plugins)

set_property(TARGET render_rm PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                       ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET render_rm PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                       ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS render_rm
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
