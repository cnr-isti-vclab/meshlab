# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES
    PhotoTexturingWidget.cpp
    edit_phototex_factory.cpp
    photoTexturing.cpp
    src/Camera.cpp
    src/CameraCalibration.cpp
    src/PhotoTexturer.cpp
    src/QuadTree/QuadTreeNode.cpp
    src/SmartBlendTextureMerger.cpp
    src/TextureMerger.cpp
    src/Tsai/TsaiCameraCalibration.cpp
    src/UVFaceTexture.cpp
    src/WinnerTakesAllTextureMerger.cpp)

set(HEADERS
    PhotoTexturingWidget.h
    edit_phototex_factory.h
    photoTexturing.h
    src/Camera.h
    src/CameraCalibration.h
    src/CameraCalibrationData.h
    src/CameraSensor.h
    src/PhotoTexturer.h
    src/QuadTree/QuadTreeLeaf.h
    src/QuadTree/QuadTreeNode.h
    src/SmartBlendTextureMerger.h
    src/TextureFilter.h
    src/TextureMerger.h
    src/Tsai/TsaiCameraCalibration.h
    src/UVFaceTexture.h
    src/WinnerTakesAllTextureMerger.h
    src/photo_texture_tools.h)

set(RESOURCES edit_phototexturing.qrc)

set(UI ui/photoTexturingWidget.ui)

add_library(edit_phototexturing MODULE ${SOURCES} ${HEADERS} ${RESOURCES} ${UI})

target_include_directories(edit_phototexturing
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(edit_phototexturing PUBLIC common)

set_property(TARGET edit_phototexturing PROPERTY FOLDER Plugins)

set_property(TARGET edit_phototexturing PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                                 ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET edit_phototexturing PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                                 ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS edit_phototexturing
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
