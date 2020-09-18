# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/common.cmake (custom for this directory),
### then re-run ./make-cmake.py

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../ML_VERSION")
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/../../ML_VERSION" MESHLAB_VERSION)
    add_definitions(-DMESHLAB_VERSION=${MESHLAB_VERSION})
endif()

set(SOURCES
    filter_parameter/rich_parameter.cpp
    filter_parameter/rich_parameter_list.cpp
    filter_parameter/value.cpp
    interfaces/plugin_interface.cpp
    interfaces/filter_plugin_interface.cpp
    GLExtensionsManager.cpp
    GLLogStream.cpp
    filterscript.cpp
    interfaces.cpp
    meshlabdocumentbundler.cpp
    meshlabdocumentxml.cpp
    meshmodel.cpp
    ml_selection_buffers.cpp
    ml_shared_data_context.cpp
    ml_thread_safe_memory_info.cpp
    mlapplication.cpp
    pluginmanager.cpp
    searcher.cpp)

set(HEADERS
    filter_parameter/rich_parameter.h
    filter_parameter/rich_parameter_list.h
    filter_parameter/value.h
    interfaces/mainwindow_interface.h
    interfaces/plugin_interface.h
    interfaces/filter_plugin_interface.h
    interfaces/io_plugin_interface.h
    GLExtensionsManager.h
    GLLogStream.h
    filterscript.h
    interfaces.h
    meshlabdocumentbundler.h
    meshlabdocumentxml.h
    meshmodel.h
    ml_mesh_type.h
    ml_selection_buffers.h
    ml_shared_data_context.h
    ml_thread_safe_memory_info.h
    mlapplication.h
    mlexception.h
    pluginmanager.h
    searcher.h)

set(RESOURCES common.qrc)

set(TARGET_TYPE SHARED)
if(WIN32)
    set(TARGET_TYPE STATIC)
endif()
add_library(common ${TARGET_TYPE} ${SOURCES} ${HEADERS} ${RESOURCES})

set_target_properties(common PROPERTIES OUTPUT_NAME meshlab-common)
target_link_libraries(
    common
    PUBLIC Qt5::Core
           Qt5::OpenGL
           Qt5::Widgets
           Qt5::Xml
           Qt5::XmlPatterns
           external-glew
    PRIVATE external-jhead)

set_property(TARGET common PROPERTY FOLDER Core)

if(NOT WIN32)
    install(TARGETS common DESTINATION ${MESHLAB_LIB_INSTALL_DIR})
endif()
