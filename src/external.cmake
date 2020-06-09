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

# GLEW - required
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
else()
    message(
        FATAL_ERROR
            "GLEW is required - at least one of ALLOW_SYSTEM_GLEW or ALLOW_BUNDLED_GLEW must be enabled and found.")
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
set(NEWUOA_DIR ${VCGDIR}/wrap/newuoa)
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
    target_link_libraries(external-ssynth PRIVATE Qt5::Core Qt5::Xml Qt5::OpenGL OpenGL::GLU)
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

# u3d - optional, for io_u3d
set(U3D_DIR ${EXTERNAL_DIR}/u3d)
if(ALLOW_BUNDLED_U3D)
    message(STATUS "- u3d - using bundled source")
    
    IF(APPLE)
       ADD_DEFINITIONS( -DMAC32 -fno-strict-aliasing )
       SET( U3D_PLATFORM Mac32 )
    ENDIF(APPLE)

    IF(WIN32)
        ADD_DEFINITIONS( -DUNICODE -D_UNICODE -D_CRT_SECURE_NO_DEPRECATE )
        SET( U3D_PLATFORM Win32 )
        LINK_LIBRARIES( winmm )
        UNSET( CMAKE_SHARED_LIBRARY_PREFIX )
        UNSET( CMAKE_SHARED_MODULE_PREFIX )
        UNSET( CMAKE_STATIC_LIBRARY_PREFIX )
        UNSET( CMAKE_IMPORT_LIBRARY_PREFIX )
        if(MSVC)
            add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
            add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
        endif(MSVC)
    ENDIF(WIN32)

    IF(UNIX AND NOT APPLE)
        ADD_DEFINITIONS( -DLIN32 -fno-strict-aliasing )
        SET( U3D_PLATFORM Lin32 )
    ENDIF(UNIX AND NOT APPLE)

    SET( Component_HDRS
	${U3D_DIR}/RTL/Component/Include/CArrayList.h
	${U3D_DIR}/RTL/Component/Include/DX7asDX8.h
	${U3D_DIR}/RTL/Component/Include/IFXACContext.h
	${U3D_DIR}/RTL/Component/Include/IFXAdaptiveMetric.h
	${U3D_DIR}/RTL/Component/Include/IFXAnimationModifier.h
	${U3D_DIR}/RTL/Component/Include/IFXArray.h
	${U3D_DIR}/RTL/Component/Include/IFXAttributes.h
	${U3D_DIR}/RTL/Component/Include/IFXAuthor.h
	${U3D_DIR}/RTL/Component/Include/IFXAuthorCLODAccess.h
	${U3D_DIR}/RTL/Component/Include/IFXAuthorCLODGen.h
	${U3D_DIR}/RTL/Component/Include/IFXAuthorCLODMesh.h
	${U3D_DIR}/RTL/Component/Include/IFXAuthorCLODResource.h
	${U3D_DIR}/RTL/Component/Include/IFXAuthorGeomCompiler.h
	${U3D_DIR}/RTL/Component/Include/IFXAuthorLineSet.h
	${U3D_DIR}/RTL/Component/Include/IFXAuthorLineSetAccess.h
	${U3D_DIR}/RTL/Component/Include/IFXAuthorLineSetAnalyzer.h
	${U3D_DIR}/RTL/Component/Include/IFXAuthorLineSetResource.h
	${U3D_DIR}/RTL/Component/Include/IFXAuthorMesh.h
	${U3D_DIR}/RTL/Component/Include/IFXAuthorMeshMap.h
	${U3D_DIR}/RTL/Component/Include/IFXAuthorMeshScrub.h
	${U3D_DIR}/RTL/Component/Include/IFXAuthorPointSet.h
	${U3D_DIR}/RTL/Component/Include/IFXAuthorPointSetAccess.h
	${U3D_DIR}/RTL/Component/Include/IFXAuthorPointSetResource.h
	${U3D_DIR}/RTL/Component/Include/IFXAuthorUpdate.h
	${U3D_DIR}/RTL/Component/Include/IFXAutoPtr.h
	${U3D_DIR}/RTL/Component/Include/IFXBaseOpenGL.h
	${U3D_DIR}/RTL/Component/Include/IFXBitStreamCompressedX.h
	${U3D_DIR}/RTL/Component/Include/IFXBitStreamX.h
	${U3D_DIR}/RTL/Component/Include/IFXBlendParam.h
	${U3D_DIR}/RTL/Component/Include/IFXBlockReaderX.h
	${U3D_DIR}/RTL/Component/Include/IFXBlockTypes.h
	${U3D_DIR}/RTL/Component/Include/IFXBlockWriterX.h
	${U3D_DIR}/RTL/Component/Include/IFXBones.h
	${U3D_DIR}/RTL/Component/Include/IFXBonesManager.h
	${U3D_DIR}/RTL/Component/Include/IFXBoneWeightsModifier.h
	${U3D_DIR}/RTL/Component/Include/IFXBoundHierarchy.h
	${U3D_DIR}/RTL/Component/Include/IFXBoundHierarchyMgr.h
	${U3D_DIR}/RTL/Component/Include/IFXBoundingBox.h
	${U3D_DIR}/RTL/Component/Include/IFXBoundSphereDataElement.h
	${U3D_DIR}/RTL/Component/Include/IFXBoundVolume.h
	${U3D_DIR}/RTL/Component/Include/IFXBTTHash.h
	${U3D_DIR}/RTL/Component/Include/IFXClock.h
	${U3D_DIR}/RTL/Component/Include/IFXCLODManagerInterface.h
	${U3D_DIR}/RTL/Component/Include/IFXCLODModifier.h
	${U3D_DIR}/RTL/Component/Include/IFXCoincidentVertexMap.h
	${U3D_DIR}/RTL/Component/Include/IFXCollection.h
	${U3D_DIR}/RTL/Component/Include/IFXCoreCIDs.h
	${U3D_DIR}/RTL/Component/Include/IFXCoreServices.h
	${U3D_DIR}/RTL/Component/Include/IFXCoreServicesRef.h
	${U3D_DIR}/RTL/Component/Include/IFXCornerIter.h
	${U3D_DIR}/RTL/Component/Include/IFXDataBlock.h
	${U3D_DIR}/RTL/Component/Include/IFXDataBlockQueueX.h
	${U3D_DIR}/RTL/Component/Include/IFXDataBlockX.h
	${U3D_DIR}/RTL/Component/Include/IFXDataPacket.h
	${U3D_DIR}/RTL/Component/Include/IFXDecoderChainX.h
	${U3D_DIR}/RTL/Component/Include/IFXDecoderX.h
	${U3D_DIR}/RTL/Component/Include/IFXDeque.h
	${U3D_DIR}/RTL/Component/Include/IFXDevice.h
	${U3D_DIR}/RTL/Component/Include/IFXDidRegistry.h
	${U3D_DIR}/RTL/Component/Include/IFXDids.h
	${U3D_DIR}/RTL/Component/Include/IFXDirectX7.h
	${U3D_DIR}/RTL/Component/Include/IFXDirectX8.h
	${U3D_DIR}/RTL/Component/Include/IFXDummyModifier.h
	${U3D_DIR}/RTL/Component/Include/IFXEncoderX.h
	${U3D_DIR}/RTL/Component/Include/IFXEnums.h
	${U3D_DIR}/RTL/Component/Include/IFXErrorInfo.h
	${U3D_DIR}/RTL/Component/Include/IFXEuler.h
	${U3D_DIR}/RTL/Component/Include/IFXExportingCIDs.h
	${U3D_DIR}/RTL/Component/Include/IFXExportingInterfaces.h
	${U3D_DIR}/RTL/Component/Include/IFXExportOptions.h
	${U3D_DIR}/RTL/Component/Include/IFXExtensionDecoderX.h
	${U3D_DIR}/RTL/Component/Include/IFXExtensionEncoderX.h
	${U3D_DIR}/RTL/Component/Include/IFXFace.h
	${U3D_DIR}/RTL/Component/Include/IFXFastAllocator.h
	${U3D_DIR}/RTL/Component/Include/IFXFastHeap.h
	${U3D_DIR}/RTL/Component/Include/IFXFatCornerIter.h
	${U3D_DIR}/RTL/Component/Include/IFXFileReference.h
	${U3D_DIR}/RTL/Component/Include/IFXFrustum.h
	${U3D_DIR}/RTL/Component/Include/IFXGenerator.h
	${U3D_DIR}/RTL/Component/Include/IFXGlyph2DCommands.h
	${U3D_DIR}/RTL/Component/Include/IFXGlyph2DModifier.h
	${U3D_DIR}/RTL/Component/Include/IFXGlyphCommandList.h
	${U3D_DIR}/RTL/Component/Include/IFXHash.h
	${U3D_DIR}/RTL/Component/Include/IFXHashMap.h
	${U3D_DIR}/RTL/Component/Include/IFXHistogramDynamic.h
	${U3D_DIR}/RTL/Component/Include/IFXIDManager.h
	${U3D_DIR}/RTL/Component/Include/IFXImageCodec.h
	${U3D_DIR}/RTL/Component/Include/IFXImportingCIDs.h
	${U3D_DIR}/RTL/Component/Include/IFXImportingInterfaces.h
	${U3D_DIR}/RTL/Component/Include/IFXInet.h
	${U3D_DIR}/RTL/Component/Include/IFXInstant.h
	${U3D_DIR}/RTL/Component/Include/IFXInterleavedData.h
	${U3D_DIR}/RTL/Component/Include/IFXIterators.h
	${U3D_DIR}/RTL/Component/Include/IFXKeyFrame.h
	${U3D_DIR}/RTL/Component/Include/IFXKeyTrack.h
	${U3D_DIR}/RTL/Component/Include/IFXKeyTrackArray.h
	${U3D_DIR}/RTL/Component/Include/IFXLight.h
	${U3D_DIR}/RTL/Component/Include/IFXLightResource.h
	${U3D_DIR}/RTL/Component/Include/IFXLightSet.h
	${U3D_DIR}/RTL/Component/Include/IFXLine.h
	${U3D_DIR}/RTL/Component/Include/IFXList.h
	${U3D_DIR}/RTL/Component/Include/IFXListContext.h
	${U3D_DIR}/RTL/Component/Include/IFXListNode.h
	${U3D_DIR}/RTL/Component/Include/IFXLoadManager.h
	${U3D_DIR}/RTL/Component/Include/IFXMarker.h
	${U3D_DIR}/RTL/Component/Include/IFXMarkerX.h
	${U3D_DIR}/RTL/Component/Include/IFXMaterialResource.h
	${U3D_DIR}/RTL/Component/Include/IFXMesh.h
	${U3D_DIR}/RTL/Component/Include/IFXMeshCompiler.h
	${U3D_DIR}/RTL/Component/Include/IFXMeshGroup.h
	${U3D_DIR}/RTL/Component/Include/IFXMeshMap.h
	${U3D_DIR}/RTL/Component/Include/IFXMetaDataX.h
	${U3D_DIR}/RTL/Component/Include/IFXMixerConstruct.h
	${U3D_DIR}/RTL/Component/Include/IFXMixerQueue.h
	${U3D_DIR}/RTL/Component/Include/IFXModel.h
	${U3D_DIR}/RTL/Component/Include/IFXModifier.h
	${U3D_DIR}/RTL/Component/Include/IFXModifierBaseDecoder.h
	${U3D_DIR}/RTL/Component/Include/IFXModifierChain.h
	${U3D_DIR}/RTL/Component/Include/IFXModifierDataElementIter.h
	${U3D_DIR}/RTL/Component/Include/IFXModifierDataPacket.h
	${U3D_DIR}/RTL/Component/Include/IFXMotion.h
	${U3D_DIR}/RTL/Component/Include/IFXMotionManager.h
	${U3D_DIR}/RTL/Component/Include/IFXMotionMixer.h
	${U3D_DIR}/RTL/Component/Include/IFXMotionResource.h
	${U3D_DIR}/RTL/Component/Include/IFXNameMap.h
	${U3D_DIR}/RTL/Component/Include/IFXNeighborFace.h
	${U3D_DIR}/RTL/Component/Include/IFXNeighborMesh.h
	${U3D_DIR}/RTL/Component/Include/IFXNeighborResControllerIntfc.h
	${U3D_DIR}/RTL/Component/Include/IFXNode.h
	${U3D_DIR}/RTL/Component/Include/IFXNotificationInfo.h
	${U3D_DIR}/RTL/Component/Include/IFXNotificationManager.h
	${U3D_DIR}/RTL/Component/Include/IFXObserver.h
	${U3D_DIR}/RTL/Component/Include/IFXOpenGL.h
	${U3D_DIR}/RTL/Component/Include/IFXPalette.h
	${U3D_DIR}/RTL/Component/Include/IFXPickObject.h
	${U3D_DIR}/RTL/Component/Include/IFXProgressCallback.h
	${U3D_DIR}/RTL/Component/Include/IFXRay.h
	${U3D_DIR}/RTL/Component/Include/IFXReadBuffer.h
	${U3D_DIR}/RTL/Component/Include/IFXReadBufferX.h
	${U3D_DIR}/RTL/Component/Include/IFXRender.h
	${U3D_DIR}/RTL/Component/Include/IFXRenderable.h
	${U3D_DIR}/RTL/Component/Include/IFXRenderBlend.h
	${U3D_DIR}/RTL/Component/Include/IFXRenderCaps.h
	${U3D_DIR}/RTL/Component/Include/IFXRenderClear.h
	${U3D_DIR}/RTL/Component/Include/IFXRenderContext.h
	${U3D_DIR}/RTL/Component/Include/IFXRenderDevice.h
	${U3D_DIR}/RTL/Component/Include/IFXRenderFog.h
	${U3D_DIR}/RTL/Component/Include/IFXRenderHelpers.h
	${U3D_DIR}/RTL/Component/Include/IFXRenderingCIDs.h
	${U3D_DIR}/RTL/Component/Include/IFXRenderingInterfaces.h
	${U3D_DIR}/RTL/Component/Include/IFXRenderLight.h
	${U3D_DIR}/RTL/Component/Include/IFXRenderMaterial.h
	${U3D_DIR}/RTL/Component/Include/IFXRenderPass.h
	${U3D_DIR}/RTL/Component/Include/IFXRenderServices.h
	${U3D_DIR}/RTL/Component/Include/IFXRenderStencil.h
	${U3D_DIR}/RTL/Component/Include/IFXRenderTexUnit.h
	${U3D_DIR}/RTL/Component/Include/IFXRenderView.h
	${U3D_DIR}/RTL/Component/Include/IFXResourceClient.h
	${U3D_DIR}/RTL/Component/Include/IFXSceneGraph.h
	${U3D_DIR}/RTL/Component/Include/IFXScheduler.h
	${U3D_DIR}/RTL/Component/Include/IFXSchedulerInfo.h
	${U3D_DIR}/RTL/Component/Include/IFXSchedulerTypes.h
	${U3D_DIR}/RTL/Component/Include/IFXSchedulingCIDs.h
	${U3D_DIR}/RTL/Component/Include/IFXSchedulingInterfaces.h
	${U3D_DIR}/RTL/Component/Include/IFXScreenSpaceMetricInterface.h
	${U3D_DIR}/RTL/Component/Include/IFXSetAdjacencyX.h
	${U3D_DIR}/RTL/Component/Include/IFXSetX.h
	${U3D_DIR}/RTL/Component/Include/IFXShader.h
	${U3D_DIR}/RTL/Component/Include/IFXShaderList.h
	${U3D_DIR}/RTL/Component/Include/IFXShaderLitTexture.h
	${U3D_DIR}/RTL/Component/Include/IFXShadingModifier.h
	${U3D_DIR}/RTL/Component/Include/IFXSimpleHash.h
	${U3D_DIR}/RTL/Component/Include/IFXSimpleList.h
	${U3D_DIR}/RTL/Component/Include/IFXSimulationInfo.h
	${U3D_DIR}/RTL/Component/Include/IFXSimulationManager.h
	${U3D_DIR}/RTL/Component/Include/IFXSimulationTask.h
	${U3D_DIR}/RTL/Component/Include/IFXSkeleton.h
	${U3D_DIR}/RTL/Component/Include/IFXSkeletonDataElement.h
	${U3D_DIR}/RTL/Component/Include/IFXSkeletonMixer.h
	${U3D_DIR}/RTL/Component/Include/IFXSmartPtr.h
	${U3D_DIR}/RTL/Component/Include/IFXSpatial.h
	${U3D_DIR}/RTL/Component/Include/IFXSpatialAssociation.h
	${U3D_DIR}/RTL/Component/Include/IFXSpatialSetQuery.h
	${U3D_DIR}/RTL/Component/Include/IFXStdio.h
	${U3D_DIR}/RTL/Component/Include/IFXSubdivManagerInterface.h
	${U3D_DIR}/RTL/Component/Include/IFXSubdivModifier.h
	${U3D_DIR}/RTL/Component/Include/IFXSubject.h
	${U3D_DIR}/RTL/Component/Include/IFXSystemManager.h
	${U3D_DIR}/RTL/Component/Include/IFXTask.h
	${U3D_DIR}/RTL/Component/Include/IFXTaskCallback.h
	${U3D_DIR}/RTL/Component/Include/IFXTaskData.h
	${U3D_DIR}/RTL/Component/Include/IFXTaskManager.h
	${U3D_DIR}/RTL/Component/Include/IFXTaskManagerNode.h
	${U3D_DIR}/RTL/Component/Include/IFXTaskManagerView.h
	${U3D_DIR}/RTL/Component/Include/IFXTextureImageTools.h
	${U3D_DIR}/RTL/Component/Include/IFXTextureObject.h
	${U3D_DIR}/RTL/Component/Include/IFXTimeManager.h
	${U3D_DIR}/RTL/Component/Include/IFXTransform.h
	${U3D_DIR}/RTL/Component/Include/IFXTransformStack.h
	${U3D_DIR}/RTL/Component/Include/IFXUnitAllocator.h
	${U3D_DIR}/RTL/Component/Include/IFXUpdates.h
	${U3D_DIR}/RTL/Component/Include/IFXUpdatesGroup.h
	${U3D_DIR}/RTL/Component/Include/IFXUVGenerator.h
	${U3D_DIR}/RTL/Component/Include/IFXUVMapper.h
	${U3D_DIR}/RTL/Component/Include/IFXVertexAttributes.h
	${U3D_DIR}/RTL/Component/Include/IFXVertexIndexer.h
	${U3D_DIR}/RTL/Component/Include/IFXVertexMap.h
	${U3D_DIR}/RTL/Component/Include/IFXVertexMapGroup.h
	${U3D_DIR}/RTL/Component/Include/IFXView.h
	${U3D_DIR}/RTL/Component/Include/IFXViewResource.h
	${U3D_DIR}/RTL/Component/Include/IFXVoidStar.h
	${U3D_DIR}/RTL/Component/Include/IFXVoidWrapper.h
	${U3D_DIR}/RTL/Component/Include/IFXWriteBuffer.h
	${U3D_DIR}/RTL/Component/Include/IFXWriteBufferX.h
	${U3D_DIR}/RTL/Component/Include/IFXWriteManager.h
	${U3D_DIR}/RTL/Component/Include/InsertionSort.h
    )
    SET( Kernel_HDRS
	${U3D_DIR}/RTL/Kernel/Include/IFXAutoRelease.h
	${U3D_DIR}/RTL/Kernel/Include/IFXCheckX.h
	${U3D_DIR}/RTL/Kernel/Include/IFXCOM.h
	${U3D_DIR}/RTL/Kernel/Include/IFXConnection.h
	${U3D_DIR}/RTL/Kernel/Include/IFXConnectionServer.h
	${U3D_DIR}/RTL/Kernel/Include/IFXDataTypes.h
	${U3D_DIR}/RTL/Kernel/Include/IFXDebug.h
	${U3D_DIR}/RTL/Kernel/Include/IFXException.h
	${U3D_DIR}/RTL/Kernel/Include/IFXGUID.h
	${U3D_DIR}/RTL/Kernel/Include/IFXIPP.h
	${U3D_DIR}/RTL/Kernel/Include/IFXMatrix4x4.h
	${U3D_DIR}/RTL/Kernel/Include/IFXMemory.h
	${U3D_DIR}/RTL/Kernel/Include/IFXPerformanceTimer.h
	${U3D_DIR}/RTL/Kernel/Include/IFXPlugin.h
	${U3D_DIR}/RTL/Kernel/Include/IFXQuaternion.h
	${U3D_DIR}/RTL/Kernel/Include/IFXResult.h
	${U3D_DIR}/RTL/Kernel/Include/IFXResultComponentEngine.h
	${U3D_DIR}/RTL/Kernel/Include/IFXString.h
	${U3D_DIR}/RTL/Kernel/Include/IFXUnknown.h
	${U3D_DIR}/RTL/Kernel/Include/IFXVector2.h
	${U3D_DIR}/RTL/Kernel/Include/IFXVector3.h
	${U3D_DIR}/RTL/Kernel/Include/IFXVector4.h
	${U3D_DIR}/RTL/Kernel/Include/IFXVersion.h
    )
    SET( Platform_HDRS
	${U3D_DIR}/RTL/Platform/Include/IFXAPI.h
	${U3D_DIR}/RTL/Platform/Include/IFXOSFileIterator.h
	${U3D_DIR}/RTL/Platform/Include/IFXOSLoader.h
	${U3D_DIR}/RTL/Platform/Include/IFXOSRender.h
	${U3D_DIR}/RTL/Platform/Include/IFXOSSocket.h
	${U3D_DIR}/RTL/Platform/Include/IFXOSUtilities.h
	${U3D_DIR}/RTL/Platform/Include/IFXRenderWindow.h
    )
    # IFXCoreStatic
    INCLUDE_DIRECTORIES( 
        ${U3D_DIR}/RTL/Component/Include 
        ${U3D_DIR}/RTL/Kernel/Include 
        ${U3D_DIR}/RTL/Platform/Include
        ${U3D_DIR}/RTL/Component/Base
        ${U3D_DIR}/RTL/Component/Rendering
        ${U3D_DIR}/RTL/Dependencies/WildCards )

    SET( IFXCoreStatic_HDRS
	${Component_HDRS}
	${Kernel_HDRS}
	${Platform_HDRS}
	${U3D_DIR}/RTL/Component/Base/IFXVectorHasher.h
	${U3D_DIR}/RTL/Component/Rendering/CIFXDeviceBase.h
	${U3D_DIR}/RTL/Component/Rendering/CIFXDeviceLight.h
	${U3D_DIR}/RTL/Component/Rendering/CIFXDeviceTexture.h
	${U3D_DIR}/RTL/Component/Rendering/CIFXDeviceTexUnit.h
	${U3D_DIR}/RTL/Component/Rendering/CIFXRender.h
	${U3D_DIR}/RTL/Component/Rendering/CIFXRenderContext.h
	${U3D_DIR}/RTL/Component/Rendering/CIFXRenderDevice.h
	${U3D_DIR}/RTL/Component/Rendering/CIFXRenderServices.h
	${U3D_DIR}/RTL/Component/Rendering/DX7/CIFXDeviceLightDX7.h
	${U3D_DIR}/RTL/Component/Rendering/DX7/CIFXDeviceTextureDX7.h
	${U3D_DIR}/RTL/Component/Rendering/DX7/CIFXDeviceTexUnitDX7.h
	${U3D_DIR}/RTL/Component/Rendering/DX7/CIFXDirectX7.h
	${U3D_DIR}/RTL/Component/Rendering/DX7/CIFXRenderDeviceDX7.h
	${U3D_DIR}/RTL/Component/Rendering/DX7/CIFXRenderDX7.h
	${U3D_DIR}/RTL/Component/Rendering/DX7/IFXRenderPCHDX7.h
	${U3D_DIR}/RTL/Component/Rendering/DX8/CIFXDeviceLightDX8.h
	${U3D_DIR}/RTL/Component/Rendering/DX8/CIFXDeviceTextureDX8.h
	${U3D_DIR}/RTL/Component/Rendering/DX8/CIFXDeviceTexUnitDX8.h
	${U3D_DIR}/RTL/Component/Rendering/DX8/CIFXDirectX8.h
	${U3D_DIR}/RTL/Component/Rendering/DX8/CIFXRenderDeviceDX8.h
	${U3D_DIR}/RTL/Component/Rendering/DX8/CIFXRenderDX8.h
	${U3D_DIR}/RTL/Component/Rendering/DX8/IFXRenderPCHDX8.h
	${U3D_DIR}/RTL/Component/Rendering/IFXAAFilter.h
	${U3D_DIR}/RTL/Component/Rendering/IFXRenderPCH.h
	${U3D_DIR}/RTL/Component/Rendering/Null/CIFXDeviceLightNULL.h
	${U3D_DIR}/RTL/Component/Rendering/Null/CIFXDeviceTextureNULL.h
	${U3D_DIR}/RTL/Component/Rendering/Null/CIFXDeviceTexUnitNULL.h
	${U3D_DIR}/RTL/Component/Rendering/Null/CIFXRenderDeviceNULL.h
	${U3D_DIR}/RTL/Component/Rendering/Null/CIFXRenderNULL.h
	${U3D_DIR}/RTL/Component/Rendering/Null/IFXRenderPCHNULL.h
	${U3D_DIR}/RTL/Component/Rendering/OpenGL/CIFXDeviceLightOGL.h
	${U3D_DIR}/RTL/Component/Rendering/OpenGL/CIFXDeviceTextureOGL.h
	${U3D_DIR}/RTL/Component/Rendering/OpenGL/CIFXDeviceTexUnitOGL.h
	${U3D_DIR}/RTL/Component/Rendering/OpenGL/CIFXOpenGL.h
	${U3D_DIR}/RTL/Component/Rendering/OpenGL/CIFXRenderDeviceOGL.h
	${U3D_DIR}/RTL/Component/Rendering/OpenGL/CIFXRenderOGL.h
	${U3D_DIR}/RTL/Component/Rendering/OpenGL/IFXRenderPCHOGL.h
	${U3D_DIR}/RTL/Dependencies/WildCards/wcmatch.h
    )
    SET( IFXCoreStatic_SRCS
	${U3D_DIR}/RTL/IFXCoreStatic/IFXCoreStatic.cpp
	${U3D_DIR}/RTL/Platform/${U3D_PLATFORM}/Common/IFXOSUtilities.cpp
	${U3D_DIR}/RTL/Platform/${U3D_PLATFORM}/Common/IFXOSLoader.cpp
	${U3D_DIR}/RTL/Platform/${U3D_PLATFORM}/Common/IFXOSRenderWindow.cpp
	${U3D_DIR}/RTL/Component/Common/IFXDids.cpp
	${U3D_DIR}/RTL/Component/Base/IFXCoincidentVertexMap.cpp
	${U3D_DIR}/RTL/Component/Base/IFXCornerIter.cpp
	${U3D_DIR}/RTL/Component/Base/IFXEuler.cpp
	${U3D_DIR}/RTL/Component/Base/IFXFatCornerIter.cpp
	${U3D_DIR}/RTL/Component/Base/IFXTransform.cpp
	${U3D_DIR}/RTL/Component/Base/IFXVectorHasher.cpp
	${U3D_DIR}/RTL/Component/Base/IFXVertexMap.cpp
	${U3D_DIR}/RTL/Component/Base/IFXVertexMapGroup.cpp
	${U3D_DIR}/RTL/Kernel/DataTypes/IFXCoreArray.cpp
	${U3D_DIR}/RTL/Kernel/DataTypes/IFXCoreList.cpp
	${U3D_DIR}/RTL/Kernel/DataTypes/IFXFastAllocator.cpp
	${U3D_DIR}/RTL/Kernel/DataTypes/IFXListNode.cpp
	${U3D_DIR}/RTL/Kernel/DataTypes/IFXMatrix4x4.cpp
	${U3D_DIR}/RTL/Kernel/DataTypes/IFXQuaternion.cpp
	${U3D_DIR}/RTL/Kernel/DataTypes/IFXString.cpp
	${U3D_DIR}/RTL/Kernel/DataTypes/IFXUnitAllocator.cpp
	${U3D_DIR}/RTL/Kernel/DataTypes/IFXVector3.cpp
	${U3D_DIR}/RTL/Kernel/DataTypes/IFXVector4.cpp
	${U3D_DIR}/RTL/Dependencies/WildCards/wcmatch.cpp
	${U3D_DIR}/RTL/Kernel/Common/IFXDebug.cpp
    )

    INCLUDE_DIRECTORIES( 
        ${U3D_DIR}/RTL/Component/Include 
        ${U3D_DIR}/RTL/Kernel/Include 
        ${U3D_DIR}/RTL/Platform/Include
        ${U3D_DIR}/IDTF
        ${U3D_DIR}/IDTF/Include
        ${U3D_DIR}/IDTF/Common )
    SET( IDTFConverter_SRCS
	${U3D_DIR}/IDTF/FileParser.cpp
	${U3D_DIR}/IDTF/SceneConverter.cpp
	${U3D_DIR}/IDTF/PointSetResourceParser.cpp
	${U3D_DIR}/IDTF/UrlListParser.cpp
	${U3D_DIR}/IDTF/NodeParser.cpp
	${U3D_DIR}/IDTF/ModifierParser.cpp
	${U3D_DIR}/IDTF/PointSetConverter.cpp
	${U3D_DIR}/IDTF/MaterialParser.cpp
	${U3D_DIR}/IDTF/MetaDataConverter.cpp
	${U3D_DIR}/IDTF/MeshResourceParser.cpp
	${U3D_DIR}/IDTF/ResourceConverter.cpp
	${U3D_DIR}/IDTF/TextureConverter.cpp
	${U3D_DIR}/IDTF/ResourceListParser.cpp
	${U3D_DIR}/IDTF/File.cpp
	${U3D_DIR}/IDTF/LineSetConverter.cpp
	${U3D_DIR}/IDTF/Converter.cpp
	${U3D_DIR}/IDTF/ModelConverter.cpp
	${U3D_DIR}/IDTF/TextureParser.cpp
	${U3D_DIR}/IDTF/NodeConverter.cpp
	${U3D_DIR}/IDTF/MeshConverter.cpp
	${U3D_DIR}/IDTF/BlockParser.cpp
	${U3D_DIR}/IDTF/ModelResourceParser.cpp
	${U3D_DIR}/IDTF/FileReferenceConverter.cpp
	${U3D_DIR}/IDTF/ShaderParser.cpp
	${U3D_DIR}/IDTF/FileScanner.cpp
	${U3D_DIR}/IDTF/FileReferenceParser.cpp
	${U3D_DIR}/IDTF/ModifierConverter.cpp
	${U3D_DIR}/IDTF/MetaDataParser.cpp
	${U3D_DIR}/IDTF/LineSetResourceParser.cpp
	${U3D_DIR}/IDTF/Helpers/MiscUtilities.cpp
	${U3D_DIR}/IDTF/Helpers/TGAImage.cpp
	${U3D_DIR}/IDTF/Helpers/ModifierUtilities.cpp
	${U3D_DIR}/IDTF/Helpers/ConverterHelpers.cpp
	${U3D_DIR}/IDTF/Helpers/SceneUtilities.cpp
	${U3D_DIR}/IDTF/Helpers/DebugInfo.cpp
	${U3D_DIR}/IDTF/Helpers/Guids.cpp
	${U3D_DIR}/IDTF/Common/GlyphModifier.cpp
	${U3D_DIR}/IDTF/Common/ModelResource.cpp
	${U3D_DIR}/IDTF/Common/ModifierList.cpp
	${U3D_DIR}/IDTF/Common/NodeList.cpp
	${U3D_DIR}/IDTF/Common/FileReference.cpp
	${U3D_DIR}/IDTF/Common/ResourceList.cpp
	${U3D_DIR}/IDTF/Common/SceneResources.cpp
	${U3D_DIR}/IDTF/Common/ModelResourceList.cpp
	${U3D_DIR}/IDTF/Common/MetaDataList.cpp
	${U3D_DIR}/IDTF/Common/ParentList.cpp
	${U3D_DIR}/IDTF/Common/GlyphCommandList.cpp
    )
    SET( IDTFConverter_HDRS
	${Component_HDRS}
	${Kernel_HDRS}
	${Platform_HDRS}
	${U3D_DIR}/IDTF/BlockParser.h
	${U3D_DIR}/IDTF/DefaultSettings.h
	${U3D_DIR}/IDTF/File.h
	${U3D_DIR}/IDTF/FileParser.h
	${U3D_DIR}/IDTF/FileReferenceConverter.h
	${U3D_DIR}/IDTF/FileReferenceParser.h
	${U3D_DIR}/IDTF/FileScanner.h
	${U3D_DIR}/IDTF/IConverter.h
	${U3D_DIR}/IDTF/LineSetConverter.h
	${U3D_DIR}/IDTF/LineSetResourceParser.h
	${U3D_DIR}/IDTF/MaterialParser.h
	${U3D_DIR}/IDTF/MeshConverter.h
	${U3D_DIR}/IDTF/MeshResourceParser.h
	${U3D_DIR}/IDTF/MetaDataConverter.h
	${U3D_DIR}/IDTF/MetaDataParser.h
	${U3D_DIR}/IDTF/ModelConverter.h
	${U3D_DIR}/IDTF/ModelResourceParser.h
	${U3D_DIR}/IDTF/ModifierConverter.h
	${U3D_DIR}/IDTF/ModifierParser.h
	${U3D_DIR}/IDTF/NodeConverter.h
	${U3D_DIR}/IDTF/NodeParser.h
	${U3D_DIR}/IDTF/PointSetConverter.h
	${U3D_DIR}/IDTF/PointSetResourceParser.h
	${U3D_DIR}/IDTF/ResourceConverter.h
	${U3D_DIR}/IDTF/ResourceListParser.h
	${U3D_DIR}/IDTF/SceneConverter.h
	${U3D_DIR}/IDTF/ShaderParser.h
	${U3D_DIR}/IDTF/TextureConverter.h
	${U3D_DIR}/IDTF/TextureParser.h
	${U3D_DIR}/IDTF/UrlListParser.h
	${U3D_DIR}/IDTF/Include/ConverterHelpers.h
	${U3D_DIR}/IDTF/Include/ConverterOptions.h
	${U3D_DIR}/IDTF/Include/ConverterResult.h
	${U3D_DIR}/IDTF/Include/DebugInfo.h
	${U3D_DIR}/IDTF/Include/SceneUtilities.h
	${U3D_DIR}/IDTF/Include/TGAImage.h
	${U3D_DIR}/IDTF/Include/U3DHeaders.h
	${U3D_DIR}/IDTF/Common/AnimationModifier.h
	${U3D_DIR}/IDTF/Common/BoneWeightModifier.h
	${U3D_DIR}/IDTF/Common/CLODModifier.h
	${U3D_DIR}/IDTF/Common/Color.h
	${U3D_DIR}/IDTF/Common/FileReference.h
	${U3D_DIR}/IDTF/Common/GlyphCommandList.h
	${U3D_DIR}/IDTF/Common/GlyphCommands.h
	${U3D_DIR}/IDTF/Common/GlyphModifier.h
	${U3D_DIR}/IDTF/Common/INode.h
	${U3D_DIR}/IDTF/Common/Int2.h
	${U3D_DIR}/IDTF/Common/Int3.h
	${U3D_DIR}/IDTF/Common/IResource.h
	${U3D_DIR}/IDTF/Common/LightNode.h
	${U3D_DIR}/IDTF/Common/LightResource.h
	${U3D_DIR}/IDTF/Common/LightResourceList.h
	${U3D_DIR}/IDTF/Common/LineSetResource.h
	${U3D_DIR}/IDTF/Common/MaterialResource.h
	${U3D_DIR}/IDTF/Common/MaterialResourceList.h
	${U3D_DIR}/IDTF/Common/MeshResource.h
	${U3D_DIR}/IDTF/Common/MetaDataList.h
	${U3D_DIR}/IDTF/Common/ModelNode.h
	${U3D_DIR}/IDTF/Common/ModelResource.h
	${U3D_DIR}/IDTF/Common/ModelResourceList.h
	${U3D_DIR}/IDTF/Common/ModelSkeleton.h
	${U3D_DIR}/IDTF/Common/Modifier.h
	${U3D_DIR}/IDTF/Common/ModifierList.h
	${U3D_DIR}/IDTF/Common/MotionResource.h
	${U3D_DIR}/IDTF/Common/MotionResourceList.h
	${U3D_DIR}/IDTF/Common/Node.h
	${U3D_DIR}/IDTF/Common/NodeList.h
	${U3D_DIR}/IDTF/Common/ParentData.h
	${U3D_DIR}/IDTF/Common/ParentList.h
	${U3D_DIR}/IDTF/Common/Point.h
	${U3D_DIR}/IDTF/Common/PointSetResource.h
	${U3D_DIR}/IDTF/Common/Quat.h
	${U3D_DIR}/IDTF/Common/Resource.h
	${U3D_DIR}/IDTF/Common/ResourceList.h
	${U3D_DIR}/IDTF/Common/SceneData.h
	${U3D_DIR}/IDTF/Common/SceneResources.h
	${U3D_DIR}/IDTF/Common/ShaderResource.h
	${U3D_DIR}/IDTF/Common/ShaderResourceList.h
	${U3D_DIR}/IDTF/Common/ShadingDescription.h
	${U3D_DIR}/IDTF/Common/ShadingDescriptionList.h
	${U3D_DIR}/IDTF/Common/ShadingModifier.h
	${U3D_DIR}/IDTF/Common/SubdivisionModifier.h
	${U3D_DIR}/IDTF/Common/TextureResource.h
	${U3D_DIR}/IDTF/Common/TextureResourceList.h
	${U3D_DIR}/IDTF/Common/Tokens.h
	${U3D_DIR}/IDTF/Common/UrlList.h
	${U3D_DIR}/IDTF/Common/ViewNodeData.h
	${U3D_DIR}/IDTF/Common/ViewNode.h
	${U3D_DIR}/IDTF/Common/ViewResource.h
	${U3D_DIR}/IDTF/Common/ViewResourceList.h
    )

    add_library(
        external-IDTFConverter STATIC
        ${IFXCoreStatic_SRCS} ${IFXCoreStatic_HDRS} ${IDTFConverter_SRCS} ${IDTFConverter_HDRS} )

    # These sources were disabled in the .pro file: "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/EngineWidget.cpp"
    # "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/Raytracer/RayTracer.cpp"
    # "${SSYNTH_DIR}/ssynth/SyntopiaCore/GLEngine/Sphere.cpp"
    # "${SSYNTH_DIR}/ssynth/StructureSynth/Model/Rendering/OpenGLRenderer.cpp"
    target_include_directories(external-IDTFConverter SYSTEM PUBLIC "${U3D_DIR}/IDTF")
    set_property(TARGET external-IDTFConverter PROPERTY FOLDER External)
endif()

