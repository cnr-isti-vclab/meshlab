# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES edit_straightener_factory.cpp editstraightener.cpp
            editstraightenerdialog.cpp)

set(HEADERS edit_straightener_factory.h editstraightener.h
            editstraightenerdialog.h)

set(RESOURCES editstraightener.qrc)

set(UI editstraightener.ui)

add_library(edit_straightener MODULE ${SOURCES} ${HEADERS} ${RESOURCES} ${UI})

target_include_directories(edit_straightener
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(edit_straightener PUBLIC common)

set_property(TARGET edit_straightener PROPERTY FOLDER Plugins)

set_property(TARGET edit_straightener PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                               ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET edit_straightener PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                               ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS edit_straightener
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
