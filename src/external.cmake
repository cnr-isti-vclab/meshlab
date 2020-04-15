# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

# jhead - required
message(STATUS "- jhead - using bundled source")
set(JHEAD_DIR ${EXTERNAL_DIR}/jhead-3.04)
add_library(
    external-jhead STATIC
    "${JHEAD_DIR}/jpgfile.c"
    "${JHEAD_DIR}/jpgqguess.c"
    "${JHEAD_DIR}/jhead.c"
    "${JHEAD_DIR}/paths.c"
    "${JHEAD_DIR}/exif.c"
    "${JHEAD_DIR}/iptc.c"
    "${JHEAD_DIR}/gpsinfo.c"
    "${JHEAD_DIR}/makernote.c")
if(WIN32)
    target_sources(external-jhead PRIVATE "${JHEAD_DIR}/myglob.c")
endif()
target_compile_definitions(external-jhead PRIVATE main=jhead_main)
target_include_directories(external-jhead PUBLIC ${JHEAD_DIR})
set_property(TARGET external-jhead PROPERTY FOLDER External)

# GLEW - either this or GLAD is required
set(GLEW_DIR ${EXTERNAL_DIR}/glew-2.1.0)
if(ALLOW_SYSTEM_GLEW AND TARGET GLEW::GLEW)
    message(STATUS "- glew - using system-provided library")
    add_library(external-glew INTERFACE)
    target_link_libraries(external-glew INTERFACE GLEW::GLEW)
    if(TARGET OpenGL::OpenGL)
        target_link_libraries(external-glew INTERFACE OpenGL::OpenGL)
    elseif(TARGET OpenGL::GL)
        target_link_libraries(external-glew INTERFACE OpenGL::GL)
    else()
        message(FATAL_ERROR "OpenGL not found or your CMake version is too old!")
    endif()
elseif(ALLOW_BUNDLED_GLEW AND EXISTS "${GLEW_DIR}/src/glew.c")
    message(STATUS "- glew - using bundled source")
    add_library(external-glew STATIC "${GLEW_DIR}/src/glew.c")
    target_compile_definitions(external-glew PUBLIC GLEW_STATIC)
    target_include_directories(external-glew SYSTEM PUBLIC ${GLEW_DIR}/include)
    if(TARGET OpenGL::OpenGL)
        target_link_libraries(external-glew PUBLIC OpenGL::OpenGL)
    elseif(TARGET OpenGL::GL)
        target_link_libraries(external-glew PUBLIC OpenGL::GL)
    else()
        message(FATAL_ERROR "OpenGL not found or your CMake version is too old!")
    endif()
    if(TARGET OpenGL::GLX)
        target_link_libraries(external-glew PUBLIC OpenGL::GLX)
    endif()
    set_property(TARGET external-glew PROPERTY FOLDER External)
endif()

# glad - either this or GLEW is required
set(GLAD_DIR ${EXTERNAL_DIR}/glad)
if(ALLOW_BUNDLED_GLAD AND EXISTS "${GLAD_DIR}/src/glad.c")
    message(STATUS "- glad - using bundled source")
    add_library(external-glad STATIC "${GLAD_DIR}/src/glad.c")
    target_compile_definitions(external-glad PUBLIC MESHLAB_USE_GLAD)
    target_include_directories(external-glad SYSTEM PUBLIC ${GLAD_DIR}/include)
    if(TARGET OpenGL::OpenGL)
        target_link_libraries(external-glad PUBLIC OpenGL::OpenGL)
    elseif(TARGET OpenGL::GL)
        target_link_libraries(external-glad PUBLIC OpenGL::GL)
    else()
        message(FATAL_ERROR "OpenGL not found or your CMake version is too old!")
    endif()
    if(TARGET OpenGL::GLX)
        target_link_libraries(external-glad PUBLIC OpenGL::GLX)
    endif()
    target_link_libraries(external-glad PRIVATE ${CMAKE_DL_LIBS})
    set_property(TARGET external-glad PROPERTY FOLDER External)
endif()

# Determine which OpenGL wrapper to use
unset(USE_GLAD)
unset(USE_GLEW)
if(TARGET external-glad AND TARGET external-glew)
    if(BUILD_PREFER_GLAD)
        set(USE_GLAD ON)
    else()
        set(USE_GLEW ON)
    endif()
elseif(TARGET external-glad)
    set(USE_GLAD ON)
elseif(TARGET external-glew)
    set(USE_GLEW ON)
else()
    message(
        FATAL_ERROR
            "glew or glad is required - at least one of ALLOW_SYSTEM_GLEW, ALLOW_BUNDLED_GLEW, or ALLOW_BUNDLED_GLAD must be enabled and found.")
endif()
set(MESHLAB_USE_GLAD "${USE_GLAD}" CACHE INTERNAL "" FORCE)
set(MESHLAB_USE_GLEW "${USE_GLEW}" CACHE INTERNAL "" FORCE)

if(USE_GLAD)
    message(STATUS "  - Using glad as OpenGL loader and extension handler")
    add_library(external-opengl ALIAS external-glad)
elseif(USE_GLEW)
    message(STATUS "  - Using glew as OpenGL loader and extension handler")
    add_library(external-opengl ALIAS external-glew)
endif()

# Eigen3 - required
set(EIGEN_DIR ${VCGDIR}/eigenlib)
if(ALLOW_SYSTEM_EIGEN AND EIGEN3_INCLUDE_DIR)
    message(STATUS "- Eigen - using system-provided library")
    set(EIGEN_INCLUDE_DIRS ${EIGEN3_INCLUDE_DIR})
elseif(ALLOW_BUNDLED_EIGEN AND EXISTS "${EIGEN_DIR}/Eigen/Eigen")
    message(STATUS "- Eigen - using bundled source")
    set(EIGEN_INCLUDE_DIRS ${EIGEN_DIR})
else()
    message(
        FATAL_ERROR
            "Eigen is required - at least one of ALLOW_SYSTEM_EIGEN or ALLOW_BUNDLED_EIGEN must be enabled and found.")
endif()

# newuoa - optional and header-only, for several plugins including all that use levmar
set(NEWUOA_DIR ${EXTERNAL_DIR}/newuoa)
if(ALLOW_BUNDLED_NEWUOA AND EXISTS "${NEWUOA_DIR}/include/newuoa.h")
    message(STATUS "- newuoa - using bundled source")
    add_library(external-newuoa INTERFACE)
    target_include_directories(external-newuoa INTERFACE ${NEWUOA_DIR}/include)
endif()

# levmar - optional, for several plugins
set(LEVMAR_DIR ${EXTERNAL_DIR}/levmar-2.3)
if(ALLOW_BUNDLED_LEVMAR AND EXISTS "${LEVMAR_DIR}/lm.h")
    message(STATUS "- levmar - using bundled source")
    add_library(
        external-levmar STATIC
        "${LEVMAR_DIR}/compiler.h"
        "${LEVMAR_DIR}/lm.h"
        "${LEVMAR_DIR}/misc.h"
        "${LEVMAR_DIR}/Axb.c"
        "${LEVMAR_DIR}/lm.c"
        "${LEVMAR_DIR}/lmbc.c"
        "${LEVMAR_DIR}/lmblec.c"
        "${LEVMAR_DIR}/lmlec.c"
        "${LEVMAR_DIR}/misc.c")
    target_include_directories(external-levmar PUBLIC ${LEVMAR_DIR})
    set_property(TARGET external-levmar PROPERTY FOLDER External)
endif()

# lib3ds - optional, for io_3ds
set(LIB3DS_DIR ${EXTERNAL_DIR}/lib3ds-1.3.0)
if(ALLOW_SYSTEM_LIB3DS AND TARGET Lib3ds::Lib3ds)
    message(STATUS "- lib3ds - using system-provided library")
    add_library(external-lib3ds INTERFACE)
    target_link_libraries(external-lib3ds INTERFACE Lib3ds::Lib3ds)
elseif(ALLOW_BUNDLED_LIB3DS AND EXISTS "${LIB3DS_DIR}/lib3ds/types.h")
    message(STATUS "- lib3ds - using bundled source")
    add_library(
        external-lib3ds STATIC
        "${LIB3DS_DIR}/lib3ds/atmosphere.c"
        "${LIB3DS_DIR}/lib3ds/atmosphere.h"
        "${LIB3DS_DIR}/lib3ds/background.c"
        "${LIB3DS_DIR}/lib3ds/background.h"
        "${LIB3DS_DIR}/lib3ds/camera.c"
        "${LIB3DS_DIR}/lib3ds/camera.h"
        "${LIB3DS_DIR}/lib3ds/chunk.c"
        "${LIB3DS_DIR}/lib3ds/chunk.h"
        "${LIB3DS_DIR}/lib3ds/chunktable.h"
        "${LIB3DS_DIR}/lib3ds/ease.c"
        "${LIB3DS_DIR}/lib3ds/ease.h"
        "${LIB3DS_DIR}/lib3ds/file.c"
        "${LIB3DS_DIR}/lib3ds/file.h"
        "${LIB3DS_DIR}/lib3ds/io.c"
        "${LIB3DS_DIR}/lib3ds/io.h"
        "${LIB3DS_DIR}/lib3ds/light.c"
        "${LIB3DS_DIR}/lib3ds/light.h"
        "${LIB3DS_DIR}/lib3ds/material.c"
        "${LIB3DS_DIR}/lib3ds/material.h"
        "${LIB3DS_DIR}/lib3ds/matrix.c"
        "${LIB3DS_DIR}/lib3ds/matrix.h"
        "${LIB3DS_DIR}/lib3ds/mesh.c"
        "${LIB3DS_DIR}/lib3ds/mesh.h"
        "${LIB3DS_DIR}/lib3ds/node.c"
        "${LIB3DS_DIR}/lib3ds/node.h"
        "${LIB3DS_DIR}/lib3ds/quat.c"
        "${LIB3DS_DIR}/lib3ds/quat.h"
        "${LIB3DS_DIR}/lib3ds/shadow.c"
        "${LIB3DS_DIR}/lib3ds/shadow.h"
        "${LIB3DS_DIR}/lib3ds/tcb.c"
        "${LIB3DS_DIR}/lib3ds/tcb.h"
        "${LIB3DS_DIR}/lib3ds/tracks.c"
        "${LIB3DS_DIR}/lib3ds/tracks.h"
        "${LIB3DS_DIR}/lib3ds/types.h"
        "${LIB3DS_DIR}/lib3ds/vector.c"
        "${LIB3DS_DIR}/lib3ds/vector.h"
        "${LIB3DS_DIR}/lib3ds/viewport.c"
        "${LIB3DS_DIR}/lib3ds/viewport.h")
    target_include_directories(external-lib3ds SYSTEM PUBLIC "${LIB3DS_DIR}")
    target_compile_definitions(external-lib3ds PUBLIC LIB3DS_STATIC)
    set_property(TARGET external-lib3ds PROPERTY FOLDER External)
endif()

# gmp or mpir - optional, for filter_csg
if(ALLOW_SYSTEM_GMP AND GMP_FOUND)
    message(STATUS "- GMP/MPIR - using system-provided GMP library")
    add_library(external-gmp INTERFACE)
    target_include_directories(external-gmp SYSTEM INTERFACE ${GMP_INCLUDE_DIRS})
    target_link_libraries(external-gmp INTERFACE ${GMP_LIBRARIES})
else()
    # TODO set up external-mpir per-os for bundled mpir
endif()

# muparser - optional, for filter_func
set(MUPARSER_DIR ${EXTERNAL_DIR}/muparser_v225)
if(ALLOW_SYSTEM_MUPARSER AND TARGET muparser::muparser)
    message(STATUS "- muparser - using system-provided library")
    add_library(external-muparser INTERFACE)
    target_link_libraries(external-muparser INTERFACE muparser::muparser)
elseif(ALLOW_BUNDLED_MUPARSER AND EXISTS "${MUPARSER_DIR}/src/muParser.cpp")
    message(STATUS "- muparser - using bundled source")
    add_library(
        external-muparser STATIC
        "${MUPARSER_DIR}/src/muParser.cpp"
        "${MUPARSER_DIR}/src/muParserBase.cpp"
        "${MUPARSER_DIR}/src/muParserBytecode.cpp"
        "${MUPARSER_DIR}/src/muParserCallback.cpp"
        "${MUPARSER_DIR}/src/muParserDLL.cpp"
        "${MUPARSER_DIR}/src/muParserError.cpp"
        "${MUPARSER_DIR}/src/muParserInt.cpp"
        "${MUPARSER_DIR}/src/muParserTest.cpp"
        "${MUPARSER_DIR}/src/muParserTokenReader.cpp")
    target_include_directories(external-muparser SYSTEM PUBLIC ${MUPARSER_DIR}/include)
    target_compile_definitions(external-muparser PUBLIC _UNICODE)
    set_property(TARGET external-muparser PROPERTY FOLDER External)
endif()

# OpenCTM - optional, for io_ctm
set(OPENCTM_DIR ${EXTERNAL_DIR}/OpenCTM-1.0.3)
if(ALLOW_SYSTEM_OPENCTM AND TARGET OpenCTM::OpenCTM)
    message(STATUS "- OpenCTM - using system-provided library")
    add_library(external-openctm INTERFACE)
    target_link_libraries(external-openctm INTERFACE OpenCTM::OpenCTM)
elseif(ALLOW_BUNDLED_OPENCTM AND EXISTS "${OPENCTM_DIR}/lib/openctm.c")
    message(STATUS "- OpenCTM - using bundled source")
    # Modified liblzma included - can't build against system version
    add_library(
        external-openctm STATIC
        "${OPENCTM_DIR}/lib/openctm.c"
        "${OPENCTM_DIR}/lib/stream.c"
        "${OPENCTM_DIR}/lib/compressRAW.c"
        "${OPENCTM_DIR}/lib/compressMG1.c"
        "${OPENCTM_DIR}/lib/compressMG2.c"
        "${OPENCTM_DIR}/lib/liblzma/Alloc.c"
        "${OPENCTM_DIR}/lib/liblzma/LzFind.c"
        "${OPENCTM_DIR}/lib/liblzma/LzmaDec.c"
        "${OPENCTM_DIR}/lib/liblzma/LzmaEnc.c"
        "${OPENCTM_DIR}/lib/liblzma/LzmaLib.c")
    target_compile_definitions(external-openctm PUBLIC OPENCTM_STATIC)
    target_include_directories(
        external-openctm SYSTEM
        PUBLIC "${OPENCTM_DIR}/lib" SYSTEM
        PRIVATE "${OPENCTM_DIR}/lib/liblzma")
    if(MSVC)
        target_compile_definitions(external-openctm PRIVATE _CRT_SECURE_NO_WARNINGS)
    endif()
    set_property(TARGET external-openctm PROPERTY FOLDER External)
endif()

# structure-synth - optional, for filter_ssynth
set(SSYNTH_DIR ${EXTERNAL_DIR}/structuresynth-1.5)
if(ALLOW_BUNDLED_SSYNTH AND EXISTS "${LEVMAR_DIR}/lm.h")
    message(STATUS "- structure-synth - using bundled source")
    # Can't use a system version because using StructureSynth as a library is not common
    add_library(
        external-ssynth STATIC
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/Action.h"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/AmbiguousRule.h"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/Builder.h"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/ColorPool.h"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/CustomRule.h"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/ExecutionStack.h"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/PrimitiveClass.h"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/PrimitiveRule.h"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/RandomStreams.h"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/Rule.h"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/RuleRef.h"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/RuleSet.h"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/State.h"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/Transformation.h"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/TransformationLoop.h"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Parser/EisenParser.h"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Parser/Preprocessor.h"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Parser/Tokenizer.h"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/Exceptions/Exception.h"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/Box.h"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/Dot.h"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/EngineWidget.h"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/Grid.h"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/Line.h"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/Mesh.h"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/Object3D.h"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/Raytracer/AtomicCounter.h"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/Raytracer/RayTracer.h"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/Raytracer/RenderThread.h"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/Raytracer/Sampler.h"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/Raytracer/VoxelStepper.h"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/RaytraceTriangle.h"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/Sphere.h"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/Triangle.h"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/Logging/ListWidgetLogger.h"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/Logging/Logging.h"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/Math/Matrix4.h"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/Math/Random.h"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/Math/Vector3.h"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/Misc/ColorUtils.h"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/Misc/MiniParser.h"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/Misc/Persistence.h"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/Misc/Version.h"
        "${SSYNTH_DIR}/ssynth/ThirdPartyCode/MersenneTwister/MersenneTwister.h"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/Rendering/OpenGLRenderer.h"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/Rendering/Renderer.h"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/Rendering/TemplateRenderer.h"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/Action.cpp"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/AmbiguousRule.cpp"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/Builder.cpp"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/ColorPool.cpp"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/CustomRule.cpp"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/ExecutionStack.cpp"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/PrimitiveClass.cpp"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/PrimitiveRule.cpp"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/RandomStreams.cpp"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/Rule.cpp"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/RuleRef.cpp"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/RuleSet.cpp"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/State.cpp"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/Transformation.cpp"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/TransformationLoop.cpp"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Parser/EisenParser.cpp"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Parser/Preprocessor.cpp"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Parser/Tokenizer.cpp"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/Box.cpp"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/Dot.cpp"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/Grid.cpp"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/Line.cpp"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/Mesh.cpp"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/Object3D.cpp"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/Raytracer/AtomicCounter.cpp"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/Raytracer/RenderThread.cpp"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/Raytracer/Sampler.cpp"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/Raytracer/VoxelStepper.cpp"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/RaytraceTriangle.cpp"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/Triangle.cpp"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/Logging/ListWidgetLogger.cpp"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/Logging/Logging.cpp"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/Math/Matrix4.cpp"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/Math/Random.cpp"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/Math/Vector3.cpp"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/Misc/ColorUtils.cpp"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/Misc/MiniParser.cpp"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/Misc/Persistence.cpp"
        "${SSYNTH_DIR}/ssynth/SyntopiaCore/Misc/Version.cpp"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/Rendering/Renderer.cpp"
        "${SSYNTH_DIR}/ssynth/StructureSynth/Model/Rendering/TemplateRenderer.cpp")

    # These sources were disabled in the .pro file: "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/EngineWidget.cpp"
    # "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/Raytracer/RayTracer.cpp"
    # "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/Sphere.cpp"
    # "${SSYNTH_DIR}/ssynth/StructureSynth/Model/Rendering/OpenGLRenderer.cpp"
    target_include_directories(external-ssynth SYSTEM PUBLIC "${SSYNTH_DIR}/ssynth")
    target_link_libraries(external-ssynth PRIVATE Qt5::Core Qt5::Xml Qt5::OpenGL Qt5::Script OpenGL::GLU)
    set_property(TARGET external-ssynth PROPERTY FOLDER External)
endif()

# qhull - optional, for filter_qhull
set(QHULL_DIR ${EXTERNAL_DIR}/qhull-2003.1)
if(ALLOW_SYSTEM_QHULL AND TARGET Qhull::qhull)
    message(STATUS "- qhull - using system-provided library")
    add_library(external-qhull INTERFACE)
    target_link_libraries(external-qhull INTERFACE Qhull::qhull)
    target_include_directories(external-qhull INTERFACE ${QHULL_qhull_INCLUDE_DIR}/qhull)
elseif(ALLOW_BUNDLED_QHULL AND EXISTS "${QHULL_DIR}/src/qhull.h")
    message(STATUS "- qhull - using bundled source")
    add_library(
        external-qhull STATIC
        "${QHULL_DIR}/src/geom2.c"
        "${QHULL_DIR}/src/global.c"
        "${QHULL_DIR}/src/io.c"
        "${QHULL_DIR}/src/io.h"
        "${QHULL_DIR}/src/mem.c"
        "${QHULL_DIR}/src/mem.h"
        "${QHULL_DIR}/src/merge.c"
        "${QHULL_DIR}/src/merge.h"
        "${QHULL_DIR}/src/poly.c"
        "${QHULL_DIR}/src/poly.h"
        "${QHULL_DIR}/src/poly2.c"
        "${QHULL_DIR}/src/qconvex.c"
        "${QHULL_DIR}/src/qhull.c"
        "${QHULL_DIR}/src/qhull.h"
        "${QHULL_DIR}/src/qset.c"
        "${QHULL_DIR}/src/qset.h"
        "${QHULL_DIR}/src/stat.c"
        "${QHULL_DIR}/src/stat.h"
        "${QHULL_DIR}/src/geom.c"
        "${QHULL_DIR}/src/geom.h"
        "${QHULL_DIR}/src/user.c"
        "${QHULL_DIR}/src/user.h")
    target_include_directories(external-qhull INTERFACE "${QHULL_DIR}/src")
    set_property(TARGET external-qhull PROPERTY FOLDER External)
endif()
