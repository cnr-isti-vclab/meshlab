# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES
    DominancyClassifier.cpp
    GPU/FrameBuffer.cpp
    GPU/Shader.cpp
    GPU/Texture.cpp
    filter_output_optical_flow.cpp
    floatbuffer.cpp
    utils.cpp)

set(HEADERS
    DominancyClassifier.h
    GPU/FrameBuffer.h
    GPU/GPU.h
    GPU/InstantiatedObject.h
    GPU/Shader.h
    GPU/Texture.h
    GPU/VBO.h
    GPU/commonDefs.h
    OOCRaster.h
    filter_output_optical_flow.h
    floatbuffer.h
    utils.h)

set(RESOURCES filter_output_optical_flow.qrc)

add_library(filter_ouput_optical_flow MODULE ${SOURCES} ${HEADERS} ${RESOURCES})

target_include_directories(filter_ouput_optical_flow
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_ouput_optical_flow PUBLIC common)

set_property(TARGET filter_ouput_optical_flow PROPERTY FOLDER Plugins)

set_property(TARGET filter_ouput_optical_flow
             PROPERTY RUNTIME_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_ouput_optical_flow
             PROPERTY LIBRARY_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS filter_ouput_optical_flow DESTINATION
                ${MESHLAB_PLUGIN_INSTALL_DIR} COMPONENT Plugins)
