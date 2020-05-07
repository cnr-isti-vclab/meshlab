# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/meshlabserver.cmake (custom for this directory),
### then re-run ./make-cmake.py

set(SOURCES mainserver.cpp)

set(RESOURCES meshlabserver.qrc)

list(APPEND RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../meshlab/meshlab.qrc)
if(WIN32)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../meshlab/meshlab.rc)
endif()

add_executable(meshlabserver ${SOURCES} ${HEADERS} ${RESOURCES} ${UI})

target_compile_definitions(
    meshlabserver PUBLIC QT_DISABLE_DEPRECATED_BEFORE=0x000000
                         NO_XSERVER_DEPENDENCY)
target_include_directories(meshlabserver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(meshlabserver PUBLIC common Qt5::Network)

set_property(TARGET meshlabserver PROPERTY FOLDER Core)

install(
    TARGETS meshlabserver
    DESTINATION ${MESHLAB_BIN_INSTALL_DIR}
    COMPONENT MeshLab-Server)
