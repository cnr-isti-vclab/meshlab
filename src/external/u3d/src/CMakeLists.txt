project(U3D)
cmake_minimum_required(VERSION 2.8)

# I do not want to support asm code, so disable it
# there were real problems with cpuid on Ubuntu 13.04 i386
ADD_DEFINITIONS( -DU3D_NO_ASM )

# add debug definitions
IF( CMAKE_GENERATOR STREQUAL "Unix Makefiles" )
SET(CMAKE_VERBOSE_MAKEFILE ON)
IF( CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" )
  ADD_DEFINITIONS( -D_DEBUG )
  set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG} -Wall)
ELSE( CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" )
  ADD_DEFINITIONS( -DNDEBUG )
ENDIF( CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" )
ENDIF( CMAKE_GENERATOR STREQUAL "Unix Makefiles" )

if( NOT MSVC )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif( NOT MSVC )

IF(APPLE)
# Tiger build
#  SET( CMAKE_OSX_ARCHITECTURES "ppc;i386" )
#  SET( CMAKE_OSX_SYSROOT /Developer/SDKs/MacOSX10.4u.sdk )
#  SET( CMAKE_OSX_DEPLOYMENT_TARGET 10.4 )
#----
# Leopard build
#  SET( CMAKE_OSX_ARCHITECTURES "ppc;i386" )
#  SET( CMAKE_OSX_SYSROOT /Developer/SDKs/MacOSX10.5.sdk )
#  SET( CMAKE_OSX_DEPLOYMENT_TARGET 10.5 )
#----
# Snow Leopard build
#  SET( CMAKE_OSX_ARCHITECTURES "ppc;i386" )
#  SET( CMAKE_OSX_SYSROOT /Developer/SDKs/MacOSX10.6.sdk )
#  SET( CMAKE_OSX_DEPLOYMENT_TARGET 10.6 )
#----
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
ENDIF(WIN32)

IF(UNIX AND NOT APPLE)
  ADD_DEFINITIONS( -DLIN32 -fno-strict-aliasing )
  SET( U3D_PLATFORM Lin32 )
ENDIF(UNIX AND NOT APPLE)


SET( LIB_DESTINATION u3d )
SET( BIN_DESTINATION u3d )
SET( INCLUDE_DESTINATION u3d/include )
SET( PLUGIN_DESTINATION u3d )
SET( SAMPLE_DESTINATION u3d/samples )
SET( DOC_DESTINATION u3d/docs )
#============================================================================
# zlib
#============================================================================

include(CheckTypeSize)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckCSourceCompiles)

check_include_file(sys/types.h HAVE_SYS_TYPES_H)
check_include_file(stdint.h    HAVE_STDINT_H)
check_include_file(stddef.h    HAVE_STDDEF_H)

#
# Check to see if we have large file support
#
set(CMAKE_REQUIRED_DEFINITIONS -D_LARGEFILE64_SOURCE=1)
check_type_size(off64_t OFF64_T)
if(HAVE_OFF64_T)
   add_definitions(-D_LARGEFILE64_SOURCE=1)
endif()
set(CMAKE_REQUIRED_DEFINITIONS) # clear variable

#
# Check for fseeko
#
check_function_exists(fseeko HAVE_FSEEKO)
if(NOT HAVE_FSEEKO)
	add_definitions(-DNO_FSEEKO)
endif()

#
# Check for unistd.h
#
check_include_file(unistd.h Z_HAVE_UNISTD_H)

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
	add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
endif()

SET(ZLIB_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/RTL/Dependencies/zlib)
configure_file(${ZLIB_SOURCE_DIR}/zconf.h.cmakein
			   ${CMAKE_CURRENT_BINARY_DIR}/zconf.h @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${ZLIB_SOURCE_DIR})


set(ZLIB_PUBLIC_HDRS
	${CMAKE_CURRENT_BINARY_DIR}/zconf.h
	${ZLIB_SOURCE_DIR}/zlib.h
)
set(ZLIB_PRIVATE_HDRS
	${ZLIB_SOURCE_DIR}/crc32.h
	${ZLIB_SOURCE_DIR}/deflate.h
	${ZLIB_SOURCE_DIR}/gzguts.h
	${ZLIB_SOURCE_DIR}/inffast.h
	${ZLIB_SOURCE_DIR}/inffixed.h
	${ZLIB_SOURCE_DIR}/inflate.h
	${ZLIB_SOURCE_DIR}/inftrees.h
	${ZLIB_SOURCE_DIR}/trees.h
	${ZLIB_SOURCE_DIR}/zutil.h
)
set(ZLIB_SRCS
	${ZLIB_SOURCE_DIR}/adler32.c
	${ZLIB_SOURCE_DIR}/compress.c
	${ZLIB_SOURCE_DIR}/crc32.c
	${ZLIB_SOURCE_DIR}/deflate.c
	${ZLIB_SOURCE_DIR}/gzclose.c
	${ZLIB_SOURCE_DIR}/gzlib.c
	${ZLIB_SOURCE_DIR}/gzread.c
	${ZLIB_SOURCE_DIR}/gzwrite.c
	${ZLIB_SOURCE_DIR}/inflate.c
	${ZLIB_SOURCE_DIR}/infback.c
	${ZLIB_SOURCE_DIR}/inftrees.c
	${ZLIB_SOURCE_DIR}/inffast.c
	${ZLIB_SOURCE_DIR}/trees.c
	${ZLIB_SOURCE_DIR}/uncompr.c
	${ZLIB_SOURCE_DIR}/zutil.c
)


#============================================================================
# png
#============================================================================

if(NOT WIN32)
  find_library(M_LIBRARY
	NAMES m
	PATHS /usr/lib /usr/local/lib
  )
  if(NOT M_LIBRARY)
	message(STATUS
	  "math library 'libm' not found - floating point support disabled")
  endif()
else()
  # not needed on windows
  set(M_LIBRARY "")
endif()

set(PNG_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/RTL/Dependencies/png)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# OUR SOURCES
set(libpng_public_hdrs
  ${PNG_SOURCE_DIR}/png.h
  ${PNG_SOURCE_DIR}/pngconf.h
  ${PNG_SOURCE_DIR}/pnglibconf.h
)
set(libpng_sources
  ${libpng_public_hdrs}
  ${PNG_SOURCE_DIR}/pngdebug.h
  ${PNG_SOURCE_DIR}/pnginfo.h
  ${PNG_SOURCE_DIR}/pngpriv.h
  ${PNG_SOURCE_DIR}/pngstruct.h
  ${PNG_SOURCE_DIR}/png.c
  ${PNG_SOURCE_DIR}/pngerror.c
  ${PNG_SOURCE_DIR}/pngget.c
  ${PNG_SOURCE_DIR}/pngmem.c
  ${PNG_SOURCE_DIR}/pngpread.c
  ${PNG_SOURCE_DIR}/pngread.c
  ${PNG_SOURCE_DIR}/pngrio.c
  ${PNG_SOURCE_DIR}/pngrtran.c
  ${PNG_SOURCE_DIR}/pngrutil.c
  ${PNG_SOURCE_DIR}/pngset.c
  ${PNG_SOURCE_DIR}/pngtrans.c
  ${PNG_SOURCE_DIR}/pngwio.c
  ${PNG_SOURCE_DIR}/pngwrite.c
  ${PNG_SOURCE_DIR}/pngwtran.c
  ${PNG_SOURCE_DIR}/pngwutil.c
)
# SOME NEEDED DEFINITIONS

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
endif(MSVC)

set(ADDITIONAL_LIBRARIES ${ADDITIONAL_LIBRARIES} ${M_LIBRARY})
include_directories(${PNG_SOURCE_DIR})

CHECK_INCLUDE_FILE(stdlib.h HAVE_STDLIB_H)

set(JPEG_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/RTL/Dependencies/jpeg)
configure_file(${JPEG_SOURCE_DIR}/jconfig.h.cmake
			   ${CMAKE_CURRENT_BINARY_DIR}/jconfig.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${JPEG_SOURCE_DIR})

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

set(JPEG_PUBLIC_HDRS
	${JPEG_SOURCE_DIR}/jerror.h
	${JPEG_SOURCE_DIR}/jmorecfg.h
	${JPEG_SOURCE_DIR}/jpeglib.h
	${CMAKE_CURRENT_BINARY_DIR}/jconfig.h
)
set(JPEG_PRIVATE_HDRS
	${JPEG_SOURCE_DIR}/cderror.h
	${JPEG_SOURCE_DIR}/jdct.h
	${JPEG_SOURCE_DIR}/jinclude.h
	${JPEG_SOURCE_DIR}/jmemsys.h
	${JPEG_SOURCE_DIR}/jpegint.h
	${JPEG_SOURCE_DIR}/jversion.h
	${JPEG_SOURCE_DIR}/transupp.h
)

# memmgr back ends: compile only one of these into a working library
# (For now, let's use the mode that requires the image fit into memory.
# This is the recommended mode for Win32 anyway.)
SET(JPEG_systemdependent_SRCS ${JPEG_SOURCE_DIR}/jmemnobs.c)

set(JPEG_SRCS
	${JPEG_SOURCE_DIR}/jaricom.c
	${JPEG_SOURCE_DIR}/jcapimin.c
	${JPEG_SOURCE_DIR}/jcapistd.c
	${JPEG_SOURCE_DIR}/jcarith.c
	${JPEG_SOURCE_DIR}/jccoefct.c
	${JPEG_SOURCE_DIR}/jccolor.c
	${JPEG_SOURCE_DIR}/jcdctmgr.c
	${JPEG_SOURCE_DIR}/jchuff.c
	${JPEG_SOURCE_DIR}/jcinit.c
	${JPEG_SOURCE_DIR}/jcmainct.c
	${JPEG_SOURCE_DIR}/jcmarker.c
	${JPEG_SOURCE_DIR}/jcmaster.c
	${JPEG_SOURCE_DIR}/jcomapi.c
	${JPEG_SOURCE_DIR}/jcparam.c
	${JPEG_SOURCE_DIR}/jcprepct.c
	${JPEG_SOURCE_DIR}/jcsample.c
	${JPEG_SOURCE_DIR}/jctrans.c
	${JPEG_SOURCE_DIR}/jdapimin.c
	${JPEG_SOURCE_DIR}/jdapistd.c
	${JPEG_SOURCE_DIR}/jdarith.c
	${JPEG_SOURCE_DIR}/jdatadst.c
	${JPEG_SOURCE_DIR}/jdatasrc.c
	${JPEG_SOURCE_DIR}/jdcoefct.c
	${JPEG_SOURCE_DIR}/jdcolor.c
	${JPEG_SOURCE_DIR}/jddctmgr.c
	${JPEG_SOURCE_DIR}/jdhuff.c
	${JPEG_SOURCE_DIR}/jdinput.c
	${JPEG_SOURCE_DIR}/jdmainct.c
	${JPEG_SOURCE_DIR}/jdmarker.c
	${JPEG_SOURCE_DIR}/jdmaster.c
	${JPEG_SOURCE_DIR}/jdmerge.c
	${JPEG_SOURCE_DIR}/jdpostct.c
	${JPEG_SOURCE_DIR}/jdsample.c
	${JPEG_SOURCE_DIR}/jdtrans.c
	${JPEG_SOURCE_DIR}/jerror.c
	${JPEG_SOURCE_DIR}/jfdctflt.c
	${JPEG_SOURCE_DIR}/jfdctfst.c
	${JPEG_SOURCE_DIR}/jfdctint.c
	${JPEG_SOURCE_DIR}/jidctflt.c
	${JPEG_SOURCE_DIR}/jidctfst.c
	${JPEG_SOURCE_DIR}/jidctint.c
	${JPEG_SOURCE_DIR}/jquant1.c
	${JPEG_SOURCE_DIR}/jquant2.c
	${JPEG_SOURCE_DIR}/jutils.c
	${JPEG_SOURCE_DIR}/jmemmgr.c)

SET(DEPENDENCIES_SRCS ${ZLIB_SRCS} ${ZLIB_PUBLIC_HDRS} ${ZLIB_PRIVATE_HDRS} ${libpng_sources} ${JPEG_systemdependent_SRCS} ${JPEG_SRCS} ${JPEG_PUBLIC_HDRS} ${JPEG_PRIVATE_HDRS})
SET_PROPERTY( SOURCE
	RTL/Platform/${U3D_PLATFORM}/Common/IFXOSLoader.cpp
	PROPERTY COMPILE_DEFINITIONS U3DPluginsPath="Plugins" U3DCorePath="." )
IF(STDIO_HACK)
SET_PROPERTY( SOURCE
	RTL/Component/Exporting/CIFXStdioWriteBufferX.cpp
	IDTF/ConverterDriver.cpp
	IDTF/File.cpp
	PROPERTY COMPILE_DEFINITIONS STDIO_HACK )
SET_PROPERTY( SOURCE
	RTL/Platform/${U3D_PLATFORM}/Common/IFXOSLoader.cpp
	PROPERTY COMPILE_DEFINITIONS U3DPluginsPath="Plugins" U3DCorePath="." STDIO_HACK )
ENDIF(STDIO_HACK)

#====

MESSAGE( STATUS "CMAKE_INSTALL_PREFIX:         " ${CMAKE_INSTALL_PREFIX} )
MESSAGE( STATUS "LIB_DESTINATION:         " ${LIB_DESTINATION} )


# INCLUDE_DIRECTORIES( RTL/Component/Include RTL/Kernel/Include RTL/Platform/Include
SET( Component_HDRS
	RTL/Component/Include/CArrayList.h
	RTL/Component/Include/DX7asDX8.h
	RTL/Component/Include/IFXACContext.h
	RTL/Component/Include/IFXAdaptiveMetric.h
	RTL/Component/Include/IFXAnimationModifier.h
	RTL/Component/Include/IFXArray.h
	RTL/Component/Include/IFXAttributes.h
	RTL/Component/Include/IFXAuthor.h
	RTL/Component/Include/IFXAuthorCLODAccess.h
	RTL/Component/Include/IFXAuthorCLODGen.h
	RTL/Component/Include/IFXAuthorCLODMesh.h
	RTL/Component/Include/IFXAuthorCLODResource.h
	RTL/Component/Include/IFXAuthorGeomCompiler.h
	RTL/Component/Include/IFXAuthorLineSet.h
	RTL/Component/Include/IFXAuthorLineSetAccess.h
	RTL/Component/Include/IFXAuthorLineSetAnalyzer.h
	RTL/Component/Include/IFXAuthorLineSetResource.h
	RTL/Component/Include/IFXAuthorMesh.h
	RTL/Component/Include/IFXAuthorMeshMap.h
	RTL/Component/Include/IFXAuthorMeshScrub.h
	RTL/Component/Include/IFXAuthorPointSet.h
	RTL/Component/Include/IFXAuthorPointSetAccess.h
	RTL/Component/Include/IFXAuthorPointSetResource.h
	RTL/Component/Include/IFXAuthorUpdate.h
	RTL/Component/Include/IFXAutoPtr.h
	RTL/Component/Include/IFXBaseOpenGL.h
	RTL/Component/Include/IFXBitStreamCompressedX.h
	RTL/Component/Include/IFXBitStreamX.h
	RTL/Component/Include/IFXBlendParam.h
	RTL/Component/Include/IFXBlockReaderX.h
	RTL/Component/Include/IFXBlockTypes.h
	RTL/Component/Include/IFXBlockWriterX.h
	RTL/Component/Include/IFXBones.h
	RTL/Component/Include/IFXBonesManager.h
	RTL/Component/Include/IFXBoneWeightsModifier.h
	RTL/Component/Include/IFXBoundHierarchy.h
	RTL/Component/Include/IFXBoundHierarchyMgr.h
	RTL/Component/Include/IFXBoundingBox.h
	RTL/Component/Include/IFXBoundSphereDataElement.h
	RTL/Component/Include/IFXBoundVolume.h
	RTL/Component/Include/IFXBTTHash.h
	RTL/Component/Include/IFXClock.h
	RTL/Component/Include/IFXCLODManagerInterface.h
	RTL/Component/Include/IFXCLODModifier.h
	RTL/Component/Include/IFXCoincidentVertexMap.h
	RTL/Component/Include/IFXCollection.h
	RTL/Component/Include/IFXCoreCIDs.h
	RTL/Component/Include/IFXCoreServices.h
	RTL/Component/Include/IFXCoreServicesRef.h
	RTL/Component/Include/IFXCornerIter.h
	RTL/Component/Include/IFXDataBlock.h
	RTL/Component/Include/IFXDataBlockQueueX.h
	RTL/Component/Include/IFXDataBlockX.h
	RTL/Component/Include/IFXDataPacket.h
	RTL/Component/Include/IFXDecoderChainX.h
	RTL/Component/Include/IFXDecoderX.h
	RTL/Component/Include/IFXDeque.h
	RTL/Component/Include/IFXDevice.h
	RTL/Component/Include/IFXDidRegistry.h
	RTL/Component/Include/IFXDids.h
	RTL/Component/Include/IFXDirectX7.h
	RTL/Component/Include/IFXDirectX8.h
	RTL/Component/Include/IFXDummyModifier.h
	RTL/Component/Include/IFXEncoderX.h
	RTL/Component/Include/IFXEnums.h
	RTL/Component/Include/IFXErrorInfo.h
	RTL/Component/Include/IFXEuler.h
	RTL/Component/Include/IFXExportingCIDs.h
	RTL/Component/Include/IFXExportingInterfaces.h
	RTL/Component/Include/IFXExportOptions.h
	RTL/Component/Include/IFXExtensionDecoderX.h
	RTL/Component/Include/IFXExtensionEncoderX.h
	RTL/Component/Include/IFXFace.h
	RTL/Component/Include/IFXFastAllocator.h
	RTL/Component/Include/IFXFastHeap.h
	RTL/Component/Include/IFXFatCornerIter.h
	RTL/Component/Include/IFXFileReference.h
	RTL/Component/Include/IFXFrustum.h
	RTL/Component/Include/IFXGenerator.h
	RTL/Component/Include/IFXGlyph2DCommands.h
	RTL/Component/Include/IFXGlyph2DModifier.h
	RTL/Component/Include/IFXGlyphCommandList.h
	RTL/Component/Include/IFXHash.h
	RTL/Component/Include/IFXHashMap.h
	RTL/Component/Include/IFXHistogramDynamic.h
	RTL/Component/Include/IFXIDManager.h
	RTL/Component/Include/IFXImageCodec.h
	RTL/Component/Include/IFXImportingCIDs.h
	RTL/Component/Include/IFXImportingInterfaces.h
	RTL/Component/Include/IFXInet.h
	RTL/Component/Include/IFXInstant.h
	RTL/Component/Include/IFXInterleavedData.h
	RTL/Component/Include/IFXIterators.h
	RTL/Component/Include/IFXKeyFrame.h
	RTL/Component/Include/IFXKeyTrack.h
	RTL/Component/Include/IFXKeyTrackArray.h
	RTL/Component/Include/IFXLight.h
	RTL/Component/Include/IFXLightResource.h
	RTL/Component/Include/IFXLightSet.h
	RTL/Component/Include/IFXLine.h
	RTL/Component/Include/IFXList.h
	RTL/Component/Include/IFXListContext.h
	RTL/Component/Include/IFXListNode.h
	RTL/Component/Include/IFXLoadManager.h
	RTL/Component/Include/IFXMarker.h
	RTL/Component/Include/IFXMarkerX.h
	RTL/Component/Include/IFXMaterialResource.h
	RTL/Component/Include/IFXMesh.h
	RTL/Component/Include/IFXMeshCompiler.h
	RTL/Component/Include/IFXMeshGroup.h
	RTL/Component/Include/IFXMeshMap.h
	RTL/Component/Include/IFXMetaDataX.h
	RTL/Component/Include/IFXMixerConstruct.h
	RTL/Component/Include/IFXMixerQueue.h
	RTL/Component/Include/IFXModel.h
	RTL/Component/Include/IFXModifier.h
	RTL/Component/Include/IFXModifierBaseDecoder.h
	RTL/Component/Include/IFXModifierChain.h
	RTL/Component/Include/IFXModifierDataElementIter.h
	RTL/Component/Include/IFXModifierDataPacket.h
	RTL/Component/Include/IFXMotion.h
	RTL/Component/Include/IFXMotionManager.h
	RTL/Component/Include/IFXMotionMixer.h
	RTL/Component/Include/IFXMotionResource.h
	RTL/Component/Include/IFXNameMap.h
	RTL/Component/Include/IFXNeighborFace.h
	RTL/Component/Include/IFXNeighborMesh.h
	RTL/Component/Include/IFXNeighborResControllerIntfc.h
	RTL/Component/Include/IFXNode.h
	RTL/Component/Include/IFXNotificationInfo.h
	RTL/Component/Include/IFXNotificationManager.h
	RTL/Component/Include/IFXObserver.h
	RTL/Component/Include/IFXOpenGL.h
	RTL/Component/Include/IFXPalette.h
	RTL/Component/Include/IFXPickObject.h
	RTL/Component/Include/IFXProgressCallback.h
	RTL/Component/Include/IFXRay.h
	RTL/Component/Include/IFXReadBuffer.h
	RTL/Component/Include/IFXReadBufferX.h
	RTL/Component/Include/IFXRender.h
	RTL/Component/Include/IFXRenderable.h
	RTL/Component/Include/IFXRenderBlend.h
	RTL/Component/Include/IFXRenderCaps.h
	RTL/Component/Include/IFXRenderClear.h
	RTL/Component/Include/IFXRenderContext.h
	RTL/Component/Include/IFXRenderDevice.h
	RTL/Component/Include/IFXRenderFog.h
	RTL/Component/Include/IFXRenderHelpers.h
	RTL/Component/Include/IFXRenderingCIDs.h
	RTL/Component/Include/IFXRenderingInterfaces.h
	RTL/Component/Include/IFXRenderLight.h
	RTL/Component/Include/IFXRenderMaterial.h
	RTL/Component/Include/IFXRenderPass.h
	RTL/Component/Include/IFXRenderServices.h
	RTL/Component/Include/IFXRenderStencil.h
	RTL/Component/Include/IFXRenderTexUnit.h
	RTL/Component/Include/IFXRenderView.h
	RTL/Component/Include/IFXResourceClient.h
	RTL/Component/Include/IFXSceneGraph.h
	RTL/Component/Include/IFXScheduler.h
	RTL/Component/Include/IFXSchedulerInfo.h
	RTL/Component/Include/IFXSchedulerTypes.h
	RTL/Component/Include/IFXSchedulingCIDs.h
	RTL/Component/Include/IFXSchedulingInterfaces.h
	RTL/Component/Include/IFXScreenSpaceMetricInterface.h
	RTL/Component/Include/IFXSetAdjacencyX.h
	RTL/Component/Include/IFXSetX.h
	RTL/Component/Include/IFXShader.h
	RTL/Component/Include/IFXShaderList.h
	RTL/Component/Include/IFXShaderLitTexture.h
	RTL/Component/Include/IFXShadingModifier.h
	RTL/Component/Include/IFXSimpleHash.h
	RTL/Component/Include/IFXSimpleList.h
	RTL/Component/Include/IFXSimulationInfo.h
	RTL/Component/Include/IFXSimulationManager.h
	RTL/Component/Include/IFXSimulationTask.h
	RTL/Component/Include/IFXSkeleton.h
	RTL/Component/Include/IFXSkeletonDataElement.h
	RTL/Component/Include/IFXSkeletonMixer.h
	RTL/Component/Include/IFXSmartPtr.h
	RTL/Component/Include/IFXSpatial.h
	RTL/Component/Include/IFXSpatialAssociation.h
	RTL/Component/Include/IFXSpatialSetQuery.h
	RTL/Component/Include/IFXStdio.h
	RTL/Component/Include/IFXSubdivManagerInterface.h
	RTL/Component/Include/IFXSubdivModifier.h
	RTL/Component/Include/IFXSubject.h
	RTL/Component/Include/IFXSystemManager.h
	RTL/Component/Include/IFXTask.h
	RTL/Component/Include/IFXTaskCallback.h
	RTL/Component/Include/IFXTaskData.h
	RTL/Component/Include/IFXTaskManager.h
	RTL/Component/Include/IFXTaskManagerNode.h
	RTL/Component/Include/IFXTaskManagerView.h
	RTL/Component/Include/IFXTextureImageTools.h
	RTL/Component/Include/IFXTextureObject.h
	RTL/Component/Include/IFXTimeManager.h
	RTL/Component/Include/IFXTransform.h
	RTL/Component/Include/IFXTransformStack.h
	RTL/Component/Include/IFXUnitAllocator.h
	RTL/Component/Include/IFXUpdates.h
	RTL/Component/Include/IFXUpdatesGroup.h
	RTL/Component/Include/IFXUVGenerator.h
	RTL/Component/Include/IFXUVMapper.h
	RTL/Component/Include/IFXVertexAttributes.h
	RTL/Component/Include/IFXVertexIndexer.h
	RTL/Component/Include/IFXVertexMap.h
	RTL/Component/Include/IFXVertexMapGroup.h
	RTL/Component/Include/IFXView.h
	RTL/Component/Include/IFXViewResource.h
	RTL/Component/Include/IFXVoidStar.h
	RTL/Component/Include/IFXVoidWrapper.h
	RTL/Component/Include/IFXWriteBuffer.h
	RTL/Component/Include/IFXWriteBufferX.h
	RTL/Component/Include/IFXWriteManager.h
	RTL/Component/Include/InsertionSort.h
)
SET( Kernel_HDRS
	RTL/Kernel/Include/IFXAutoRelease.h
	RTL/Kernel/Include/IFXCheckX.h
	RTL/Kernel/Include/IFXCOM.h
	RTL/Kernel/Include/IFXConnection.h
	RTL/Kernel/Include/IFXConnectionServer.h
	RTL/Kernel/Include/IFXDataTypes.h
	RTL/Kernel/Include/IFXDebug.h
	RTL/Kernel/Include/IFXException.h
	RTL/Kernel/Include/IFXGUID.h
	RTL/Kernel/Include/IFXIPP.h
	RTL/Kernel/Include/IFXMatrix4x4.h
	RTL/Kernel/Include/IFXMemory.h
	RTL/Kernel/Include/IFXPerformanceTimer.h
	RTL/Kernel/Include/IFXPlugin.h
	RTL/Kernel/Include/IFXQuaternion.h
	RTL/Kernel/Include/IFXResult.h
	RTL/Kernel/Include/IFXResultComponentEngine.h
	RTL/Kernel/Include/IFXString.h
	RTL/Kernel/Include/IFXUnknown.h
	RTL/Kernel/Include/IFXVector2.h
	RTL/Kernel/Include/IFXVector3.h
	RTL/Kernel/Include/IFXVector4.h
	RTL/Kernel/Include/IFXVersion.h
)
SET( Platform_HDRS
	RTL/Platform/Include/IFXAPI.h
	RTL/Platform/Include/IFXOSFileIterator.h
	RTL/Platform/Include/IFXOSLoader.h
	RTL/Platform/Include/IFXOSRender.h
	RTL/Platform/Include/IFXOSSocket.h
	RTL/Platform/Include/IFXOSUtilities.h
	RTL/Platform/Include/IFXRenderWindow.h
)

# IFXCoreStatic
INCLUDE_DIRECTORIES( RTL/Component/Include RTL/Kernel/Include RTL/Platform/Include
	RTL/Component/Base
        RTL/Component/Rendering
	RTL/Dependencies/WildCards )

SET( IFXCoreStatic_HDRS
	${Component_HDRS}
	${Kernel_HDRS}
	${Platform_HDRS}
	RTL/Component/Base/IFXVectorHasher.h
	RTL/Component/Rendering/CIFXDeviceBase.h
	RTL/Component/Rendering/CIFXDeviceLight.h
	RTL/Component/Rendering/CIFXDeviceTexture.h
	RTL/Component/Rendering/CIFXDeviceTexUnit.h
	RTL/Component/Rendering/CIFXRender.h
	RTL/Component/Rendering/CIFXRenderContext.h
	RTL/Component/Rendering/CIFXRenderDevice.h
	RTL/Component/Rendering/CIFXRenderServices.h
	RTL/Component/Rendering/DX7/CIFXDeviceLightDX7.h
	RTL/Component/Rendering/DX7/CIFXDeviceTextureDX7.h
	RTL/Component/Rendering/DX7/CIFXDeviceTexUnitDX7.h
	RTL/Component/Rendering/DX7/CIFXDirectX7.h
	RTL/Component/Rendering/DX7/CIFXRenderDeviceDX7.h
	RTL/Component/Rendering/DX7/CIFXRenderDX7.h
	RTL/Component/Rendering/DX7/IFXRenderPCHDX7.h
	RTL/Component/Rendering/DX8/CIFXDeviceLightDX8.h
	RTL/Component/Rendering/DX8/CIFXDeviceTextureDX8.h
	RTL/Component/Rendering/DX8/CIFXDeviceTexUnitDX8.h
	RTL/Component/Rendering/DX8/CIFXDirectX8.h
	RTL/Component/Rendering/DX8/CIFXRenderDeviceDX8.h
	RTL/Component/Rendering/DX8/CIFXRenderDX8.h
	RTL/Component/Rendering/DX8/IFXRenderPCHDX8.h
	RTL/Component/Rendering/IFXAAFilter.h
	RTL/Component/Rendering/IFXRenderPCH.h
	RTL/Component/Rendering/Null/CIFXDeviceLightNULL.h
	RTL/Component/Rendering/Null/CIFXDeviceTextureNULL.h
	RTL/Component/Rendering/Null/CIFXDeviceTexUnitNULL.h
	RTL/Component/Rendering/Null/CIFXRenderDeviceNULL.h
	RTL/Component/Rendering/Null/CIFXRenderNULL.h
	RTL/Component/Rendering/Null/IFXRenderPCHNULL.h
	RTL/Component/Rendering/OpenGL/CIFXDeviceLightOGL.h
	RTL/Component/Rendering/OpenGL/CIFXDeviceTextureOGL.h
	RTL/Component/Rendering/OpenGL/CIFXDeviceTexUnitOGL.h
	RTL/Component/Rendering/OpenGL/CIFXOpenGL.h
	RTL/Component/Rendering/OpenGL/CIFXRenderDeviceOGL.h
	RTL/Component/Rendering/OpenGL/CIFXRenderOGL.h
	RTL/Component/Rendering/OpenGL/IFXRenderPCHOGL.h
	RTL/Dependencies/WildCards/wcmatch.h
)
SET( IFXCoreStatic_SRCS
	RTL/IFXCoreStatic/IFXCoreStatic.cpp
	RTL/Platform/${U3D_PLATFORM}/Common/IFXOSUtilities.cpp
	RTL/Platform/${U3D_PLATFORM}/Common/IFXOSLoader.cpp
	RTL/Platform/${U3D_PLATFORM}/Common/IFXOSRenderWindow.cpp
	RTL/Component/Common/IFXDids.cpp
	RTL/Component/Base/IFXCoincidentVertexMap.cpp
	RTL/Component/Base/IFXCornerIter.cpp
	RTL/Component/Base/IFXEuler.cpp
	RTL/Component/Base/IFXFatCornerIter.cpp
	RTL/Component/Base/IFXTransform.cpp
	RTL/Component/Base/IFXVectorHasher.cpp
	RTL/Component/Base/IFXVertexMap.cpp
	RTL/Component/Base/IFXVertexMapGroup.cpp
	RTL/Kernel/DataTypes/IFXCoreArray.cpp
	RTL/Kernel/DataTypes/IFXCoreList.cpp
	RTL/Kernel/DataTypes/IFXFastAllocator.cpp
	RTL/Kernel/DataTypes/IFXListNode.cpp
	RTL/Kernel/DataTypes/IFXMatrix4x4.cpp
	RTL/Kernel/DataTypes/IFXQuaternion.cpp
	RTL/Kernel/DataTypes/IFXString.cpp
	RTL/Kernel/DataTypes/IFXUnitAllocator.cpp
	RTL/Kernel/DataTypes/IFXVector3.cpp
	RTL/Kernel/DataTypes/IFXVector4.cpp
	RTL/Dependencies/WildCards/wcmatch.cpp
	RTL/Kernel/Common/IFXDebug.cpp
)
ADD_LIBRARY( IFXCoreStatic STATIC ${IFXCoreStatic_SRCS} ${IFXCoreStatic_HDRS} )
install(
	TARGETS IFXCoreStatic
	ARCHIVE DESTINATION ${LIB_DESTINATION}
	LIBRARY DESTINATION ${LIB_DESTINATION}
)

# Something Windows-only 
# INCLUDE_DIRECTORIES( RTL/Component/Include RTL/Kernel/Include RTL/Platform/Include )
# 
# SET( IFXImportingStatic_SRCS
# 	RTL/Component/Base/IFXCoincidentVertexMap.cpp
# 	RTL/Component/Base/IFXCornerIter.cpp
# 	RTL/Component/Base/IFXVectorHasher.cpp
# 	RTL/Component/Base/IFXBoundingBox.cpp
# 	RTL/Component/Base/IFXVertexMap.cpp )

# ADD_LIBRARY( IFXImportingStatic ${IFXImportingStatic_SRCS} )

# IFXCore
INCLUDE_DIRECTORIES( RTL/Component/Include RTL/Kernel/Include RTL/Platform/Include
	RTL/Component/Base
	RTL/Component/BitStream
	RTL/Component/Bones
	RTL/Component/BoundHierarchy
	RTL/Component/CLODAuthor
	RTL/Component/Common
	RTL/Component/Generators/CLOD
	RTL/Component/Generators/Glyph2D
	RTL/Component/Generators/LineSet
	RTL/Component/Generators/PointSet
	RTL/Component/Mesh
	RTL/Component/ModifierChain
	RTL/Component/Palette
	RTL/Component/Rendering
	RTL/Component/SceneGraph
	RTL/Component/Shaders
	RTL/Component/Subdiv
	RTL/Component/Texture
	RTL/Component/UVGenerator
	RTL/Kernel/IFXCom
	RTL/Kernel/Common
	RTL/Dependencies/FNVHash
	RTL/Dependencies/Predicates
	RTL/Dependencies/WildCards
)

SET( IFXCore_SRCS
	RTL/Platform/${U3D_PLATFORM}/IFXCore/IFXCoreDllMain.cpp
	RTL/IFXCorePluginStatic/IFXCorePluginStatic.cpp
	RTL/Platform/${U3D_PLATFORM}/Common/IFXOSLoader.cpp
	RTL/Platform/${U3D_PLATFORM}/Common/IFXOSRenderWindow.cpp
	RTL/Platform/${U3D_PLATFORM}/Common/IFXOSUtilities.cpp
	RTL/Component/Base/IFXCoincidentVertexMap.cpp
	RTL/Component/Base/IFXCornerIter.cpp
	RTL/Component/Base/IFXEuler.cpp
	RTL/Component/Base/IFXFatCornerIter.cpp
	RTL/Component/Base/IFXTransform.cpp
	RTL/Component/Base/IFXVectorHasher.cpp
	RTL/Component/Base/IFXVertexMap.cpp
	RTL/Component/Base/IFXVertexMapGroup.cpp
	RTL/Component/BitStream/CIFXBitStreamX.cpp
	RTL/Component/BitStream/CIFXDataBlockQueueX.cpp
	RTL/Component/BitStream/CIFXDataBlockX.cpp
	RTL/Component/BitStream/IFXHistogramDynamic.cpp
	RTL/Component/Bones/CIFXAnimationModifier.cpp
	RTL/Component/Bones/CIFXBoneWeightsModifier.cpp
	RTL/Component/Bones/CIFXSkeleton.cpp
	RTL/Component/Bones/IFXBoneCacheArray.cpp
	RTL/Component/Bones/IFXBoneNode.cpp
	RTL/Component/Bones/IFXBonesManagerImpl.cpp
	RTL/Component/Bones/IFXCharacter.cpp
	RTL/Component/Bones/IFXCoreNode.cpp
	RTL/Component/Bones/IFXKeyTrack.cpp
	RTL/Component/Bones/IFXMeshGroup_Character.cpp
	RTL/Component/Bones/IFXMixerQueue.cpp
	RTL/Component/Bones/IFXMixerQueueImpl.cpp
	RTL/Component/Bones/IFXMotion.cpp
	RTL/Component/Bones/IFXMotionManagerImpl.cpp
	RTL/Component/Bones/IFXMotionMixerImpl.cpp
	RTL/Component/Bones/IFXSkin.cpp
	RTL/Component/Bones/IFXSkin_p3.cpp
	RTL/Component/Bones/IFXVertexWeight.cpp
	RTL/Component/Bones/IFXVertexWeights.cpp
	RTL/Component/BoundHierarchy/CIFXAABBHierarchyBuilder.cpp
	RTL/Component/BoundHierarchy/CIFXAxisAlignedBBox.cpp
	RTL/Component/BoundHierarchy/CIFXBoundFace.cpp
	RTL/Component/BoundHierarchy/CIFXBoundHierarchy.cpp
	RTL/Component/BoundHierarchy/CIFXBoundUtil.cpp
	RTL/Component/BoundHierarchy/CIFXBTree.cpp
	RTL/Component/BoundHierarchy/CIFXBTreeNode.cpp
	RTL/Component/BoundHierarchy/CIFXCollisionResult.cpp
	RTL/Component/BoundHierarchy/CIFXPickObject.cpp
	RTL/Component/BoundHierarchy/CIFXPrimitiveOverlap.cpp
	RTL/Component/BoundHierarchy/CIFXResultAllocator.cpp
	RTL/Component/CLODAuthor/CIFXAuthorCLODGen.cpp
	RTL/Component/CLODAuthor/CIFXAuthorMeshMap.cpp
	RTL/Component/CLODAuthor/CIFXSetAdjacencyX.cpp
	RTL/Component/CLODAuthor/CIFXSetX.cpp
	RTL/Component/CLODAuthor/CLODGenerator.cpp
	RTL/Component/CLODAuthor/ContractionRecorder.cpp
	RTL/Component/CLODAuthor/Face.cpp
	RTL/Component/CLODAuthor/Matrix4x4.cpp
	RTL/Component/CLODAuthor/NormalMap.cpp
	RTL/Component/CLODAuthor/Pair.cpp
	RTL/Component/CLODAuthor/PairFinder.cpp
	RTL/Component/CLODAuthor/PairHash.cpp
	RTL/Component/CLODAuthor/PairHeap.cpp
	RTL/Component/CLODAuthor/Primitives.cpp
	RTL/Component/CLODAuthor/Vertex.cpp
	RTL/Component/CLODAuthor/VertexPairContractor.cpp
	RTL/Component/Common/CIFXCoreServices.cpp
	RTL/Component/Common/CIFXCoreServicesRef.cpp
	RTL/Component/Common/CIFXHashMap.cpp
	RTL/Component/Common/CIFXIDManager.cpp
	RTL/Component/Common/CIFXMetaData.cpp
	RTL/Component/Common/CIFXNameMap.cpp
	RTL/Component/Common/CIFXSimpleHash.cpp
	RTL/Component/Common/CIFXVoidWrapper.cpp
	RTL/Component/Common/IFXComponentDescriptorList.cpp
	RTL/Component/Common/IFXComponentIds.cpp
	RTL/Component/Common/IFXDids.cpp
	RTL/Component/Generators/CLOD/CIFXAuthorCLODResource.cpp
	RTL/Component/Generators/CLOD/CIFXAuthorMesh.cpp
	RTL/Component/Generators/CLOD/CIFXAuthorMeshScrub.cpp
	RTL/Component/Generators/CLOD/CIFXCLODModifier.cpp
	RTL/Component/Generators/CLOD/CIFXMeshCompiler.cpp
	RTL/Component/Generators/CLOD/CIFXMeshMap.cpp
	RTL/Component/Generators/CLOD/IFXCLODManager.cpp
	RTL/Component/Generators/CLOD/IFXNeighborResController.cpp
	RTL/Component/Generators/Glyph2D/CIFXContour.cpp
	RTL/Component/Generators/Glyph2D/CIFXContourExtruder.cpp
	RTL/Component/Generators/Glyph2D/CIFXContourGenerator.cpp
	RTL/Component/Generators/Glyph2D/CIFXContourTessellator.cpp
	RTL/Component/Generators/Glyph2D/CIFXGlyph2DCommands.cpp
	RTL/Component/Generators/Glyph2D/CIFXGlyph2DModifier.cpp
	RTL/Component/Generators/Glyph2D/CIFXGlyph3DGenerator.cpp
	RTL/Component/Generators/Glyph2D/CIFXGlyphCommandList.cpp
	RTL/Component/Generators/Glyph2D/CIFXQuadEdge.cpp
	RTL/Component/Generators/LineSet/CIFXAuthorLineSetAnalyzer.cpp
	RTL/Component/Generators/LineSet/CIFXAuthorLineSet.cpp
	RTL/Component/Generators/LineSet/CIFXAuthorLineSetResource.cpp
	RTL/Component/Generators/PointSet/CIFXAuthorPointSet.cpp
	RTL/Component/Generators/PointSet/CIFXAuthorPointSetResource.cpp
	RTL/Component/Mesh/CIFXInterleavedData.cpp
	RTL/Component/Mesh/CIFXMesh.cpp
	RTL/Component/Mesh/CIFXMeshGroup.cpp
	RTL/Component/Mesh/CIFXNeighborMesh.cpp
	RTL/Component/Mesh/CIFXRenderable.cpp
	RTL/Component/Mesh/IFXFaceLists.cpp
	RTL/Component/ModifierChain/CIFXDidRegistry.cpp
	RTL/Component/ModifierChain/CIFXModifierChain.cpp
	RTL/Component/ModifierChain/CIFXModifier.cpp
	RTL/Component/ModifierChain/CIFXModifierDataElementIter.cpp
	RTL/Component/ModifierChain/CIFXModifierDataPacket.cpp
	RTL/Component/ModifierChain/CIFXSubject.cpp
	RTL/Component/ModifierChain/IFXModifierChainState.cpp
	RTL/Component/Palette/CIFXPalette.cpp
	RTL/Component/Palette/CIFXSimpleObject.cpp
	RTL/Component/SceneGraph/CIFXBoundSphereDataElement.cpp
	RTL/Component/SceneGraph/CIFXDevice.cpp
	RTL/Component/SceneGraph/CIFXDummyModifier.cpp
	RTL/Component/SceneGraph/CIFXFileReference.cpp
	RTL/Component/SceneGraph/CIFXGroup.cpp
	RTL/Component/SceneGraph/CIFXLight.cpp
	RTL/Component/SceneGraph/CIFXLightResource.cpp
	RTL/Component/SceneGraph/CIFXLightSet.cpp
	RTL/Component/SceneGraph/CIFXMarker.cpp
	RTL/Component/SceneGraph/CIFXMaterialResource.cpp
	RTL/Component/SceneGraph/CIFXMixerConstruct.cpp
	RTL/Component/SceneGraph/CIFXModel.cpp
	RTL/Component/SceneGraph/CIFXMotionResource.cpp
	RTL/Component/SceneGraph/CIFXNode.cpp
	RTL/Component/SceneGraph/CIFXResourceClient.cpp
	RTL/Component/SceneGraph/CIFXSceneGraph.cpp
	RTL/Component/SceneGraph/CIFXShaderList.cpp
	RTL/Component/SceneGraph/CIFXSimpleCollection.cpp
	RTL/Component/SceneGraph/CIFXSimpleList.cpp
	RTL/Component/SceneGraph/CIFXView.cpp
	RTL/Component/SceneGraph/CIFXViewResource.cpp
	RTL/Component/SceneGraph/IFXRenderPass.cpp
	RTL/Component/Shaders/CIFXShader.cpp
	RTL/Component/Shaders/CIFXShaderLitTexture.cpp
	RTL/Component/Shaders/CIFXShadingModifier.cpp
	RTL/Component/Subdiv/CIFXSubdivModifier.cpp
	RTL/Component/Subdiv/IFXAttributeNeighborhood.cpp
	RTL/Component/Subdiv/IFXButterflyScheme.cpp
	RTL/Component/Subdiv/IFXScreenSpaceMetric.cpp
	RTL/Component/Subdiv/IFXSharedUnitAllocator.cpp
	RTL/Component/Subdiv/IFXSubdivisionManager.cpp
	RTL/Component/Subdiv/IFXTQTAddress.cpp
	RTL/Component/Subdiv/IFXTQTAttribute.cpp
	RTL/Component/Subdiv/IFXTQTBaseTriangle.cpp
	RTL/Component/Subdiv/IFXTQTTriangleAllocator.cpp
	RTL/Component/Subdiv/IFXTQTTriangle.cpp
	RTL/Component/Subdiv/IFXVertexAllocator.cpp
	RTL/Component/Texture/CIFXImageTools.cpp
	RTL/Component/Texture/CIFXTextureImageTools.cpp
	RTL/Component/Texture/CIFXTextureObject.cpp
	RTL/Component/UVGenerator/CIFXUVGenerator.cpp
	RTL/Component/UVGenerator/CIFXUVMapperCylindrical.cpp
	RTL/Component/UVGenerator/CIFXUVMapperNone.cpp
	RTL/Component/UVGenerator/CIFXUVMapperPlanar.cpp
	RTL/Component/UVGenerator/CIFXUVMapperReflection.cpp
	RTL/Component/UVGenerator/CIFXUVMapperSpherical.cpp
	RTL/Kernel/Common/CIFXConnector.cpp
	RTL/Kernel/Common/CIFXPerformanceTimer.cpp
	RTL/Kernel/Common/IFXCheckX.cpp
	RTL/Kernel/IFXCom/CIFXComponentManager.cpp
	RTL/Kernel/IFXCom/CIFXGUIDHashMap.cpp
	RTL/Kernel/IFXCom/CIFXPluginProxy.cpp
	RTL/Kernel/IFXCom/IFXCom.cpp
	RTL/Kernel/Memory/IFXMemory.cpp
	RTL/Kernel/DataTypes/IFXCoreArray.cpp
	RTL/Kernel/DataTypes/IFXCoreList.cpp
	RTL/Kernel/DataTypes/IFXFastAllocator.cpp
	RTL/Kernel/DataTypes/IFXListNode.cpp
	RTL/Kernel/DataTypes/IFXMatrix4x4.cpp
	RTL/Kernel/DataTypes/IFXQuaternion.cpp
	RTL/Kernel/DataTypes/IFXString.cpp
	RTL/Kernel/DataTypes/IFXUnitAllocator.cpp
	RTL/Kernel/DataTypes/IFXVector3.cpp
	RTL/Kernel/DataTypes/IFXVector4.cpp
	RTL/Dependencies/WildCards/wcmatch.cpp
	RTL/Dependencies/Predicates/predicates.cpp
	RTL/Kernel/Common/IFXDebug.cpp
)
SET( IFXCore_HDRS
	${Component_HDRS}
	${Kernel_HDRS}
	${Platform_HDRS}
	RTL/Component/Base/IFXVectorHasher.h
	RTL/Component/BitStream/CIFXBitStreamX.h
	RTL/Component/BitStream/CIFXDataBlockQueueX.h
	RTL/Component/BitStream/CIFXDataBlockX.h
	RTL/Component/Bones/CIFXAnimationModifier.h
	RTL/Component/Bones/CIFXBoneWeightsModifier.h
	RTL/Component/Bones/CIFXSkeleton.h
	RTL/Component/Bones/IFXBoneCache.h
	RTL/Component/Bones/IFXBoneCacheArray.h
	RTL/Component/Bones/IFXBoneContext.h
	RTL/Component/Bones/IFXBoneLinks.h
	RTL/Component/Bones/IFXBoneNode.h
	RTL/Component/Bones/IFXBoneNodeList.h
	RTL/Component/Bones/IFXBonesManagerImpl.h
	RTL/Component/Bones/IFXCharacter.h
	RTL/Component/Bones/IFXConstraints.h
	RTL/Component/Bones/IFXCoreNode.h
	RTL/Component/Bones/IFXCylinder.h
	RTL/Component/Bones/IFXIKModes.h
	RTL/Component/Bones/IFXKeyFrameContext.h
	RTL/Component/Bones/IFXLong3.h
	RTL/Component/Bones/IFXMatrix3x4.h
	RTL/Component/Bones/IFXMeshGroup_Character.h
	RTL/Component/Bones/IFXMeshGroup_Impl.h
	RTL/Component/Bones/IFXMeshInterface.h
	RTL/Component/Bones/IFXMeshVertexMap.h
	RTL/Component/Bones/IFXMixerQueueImpl.h
	RTL/Component/Bones/IFXMotionManagerImpl.h
	RTL/Component/Bones/IFXMotionMixerImpl.h
	RTL/Component/Bones/IFXMotionReader.h
	RTL/Component/Bones/IFXPackWeights.h
	RTL/Component/Bones/IFXSkin.h
	RTL/Component/Bones/IFXVertexWeight.h
	RTL/Component/Bones/IFXVertexWeights.h
	RTL/Component/BoundHierarchy/CIFXAABBHierarchyBuilder.h
	RTL/Component/BoundHierarchy/CIFXAxisAlignedBBox.h
	RTL/Component/BoundHierarchy/CIFXBoundFace.h
	RTL/Component/BoundHierarchy/CIFXBoundHierarchy.h
	RTL/Component/BoundHierarchy/CIFXBoundUtil.h
	RTL/Component/BoundHierarchy/CIFXBTree.h
	RTL/Component/BoundHierarchy/CIFXBTreeNode.h
	RTL/Component/BoundHierarchy/CIFXCollisionResult.h
	RTL/Component/BoundHierarchy/CIFXPickObject.h
	RTL/Component/BoundHierarchy/CIFXPrimitiveOverlap.h
	RTL/Component/BoundHierarchy/CIFXResultAllocator.h
	RTL/Component/CLODAuthor/CIFXAuthorCLODGen.h
	RTL/Component/CLODAuthor/CIFXAuthorMeshMap.h
	RTL/Component/CLODAuthor/CIFXSetAdjacencyX.h
	RTL/Component/CLODAuthor/CIFXSetX.h
	RTL/Component/CLODAuthor/CLODGenerator.h
	RTL/Component/CLODAuthor/ContractionRecorder.h
	RTL/Component/CLODAuthor/CostMap.h
	RTL/Component/CLODAuthor/Face.h
	RTL/Component/CLODAuthor/FaceExam.h
	RTL/Component/CLODAuthor/FacePtrSet.h
	RTL/Component/CLODAuthor/FaceUpdate.h
	RTL/Component/CLODAuthor/GeometryObject.h
	RTL/Component/CLODAuthor/IFXSList.h
	RTL/Component/CLODAuthor/Matrix4x4.h
	RTL/Component/CLODAuthor/NormalMap.h
	RTL/Component/CLODAuthor/Pair.h
	RTL/Component/CLODAuthor/PairFinder.h
	RTL/Component/CLODAuthor/PairHash.h
	RTL/Component/CLODAuthor/PairHeap.h
	RTL/Component/CLODAuthor/Primitives.h
	RTL/Component/CLODAuthor/QEConstants.h
	RTL/Component/CLODAuthor/SmallPtrSet.h
	RTL/Component/CLODAuthor/Vertex.h
	RTL/Component/CLODAuthor/VertexPairContractor.h
	RTL/Component/Common/CIFXCoreServices.h
	RTL/Component/Common/CIFXCoreServicesRef.h
	RTL/Component/Common/CIFXHashMap.h
	RTL/Component/Common/CIFXIDManager.h
	RTL/Component/Common/CIFXMetaData.h
	RTL/Component/Common/CIFXNameMap.h
	RTL/Component/Common/CIFXSimpleHash.h
	RTL/Component/Common/CIFXVoidWrapper.h
	RTL/Component/Common/IFXComponentFactories.h
	RTL/Component/Generators/CLOD/CIFXAuthorCLODResource.h
	RTL/Component/Generators/CLOD/CIFXAuthorMesh.h
	RTL/Component/Generators/CLOD/CIFXAuthorMeshScrub.h
	RTL/Component/Generators/CLOD/CIFXCLODModifier.h
	RTL/Component/Generators/CLOD/CIFXMeshCompiler.h
	RTL/Component/Generators/CLOD/CIFXMeshMap.h
	RTL/Component/Generators/CLOD/IFXCLODManager.h
	RTL/Component/Generators/CLOD/IFXNeighborResController.h
	RTL/Component/Generators/Glyph2D/CIFXContour.h
	RTL/Component/Generators/Glyph2D/CIFXContourExtruder.h
	RTL/Component/Generators/Glyph2D/CIFXContourGenerator.h
	RTL/Component/Generators/Glyph2D/CIFXContourTessellator.h
	RTL/Component/Generators/Glyph2D/CIFXGeom2D.h
	RTL/Component/Generators/Glyph2D/CIFXGlyph2DCommands.h
	RTL/Component/Generators/Glyph2D/CIFXGlyph2DModifier.h
	RTL/Component/Generators/Glyph2D/CIFXGlyph3DGenerator.h
	RTL/Component/Generators/Glyph2D/CIFXGlyphCommandList.h
	RTL/Component/Generators/Glyph2D/CIFXQuadEdge.h
	RTL/Component/Generators/Glyph2D/IFXContour.h
	RTL/Component/Generators/Glyph2D/IFXContourExtruder.h
	RTL/Component/Generators/Glyph2D/IFXContourGenerator.h
	RTL/Component/Generators/Glyph2D/IFXContourTessellator.h
	RTL/Component/Generators/Glyph2D/IFXGlyph3DGenerator.h
	RTL/Component/Generators/LineSet/CIFXAuthorLineSet.h
	RTL/Component/Generators/LineSet/CIFXAuthorLineSetAnalyzer.h
	RTL/Component/Generators/LineSet/CIFXAuthorLineSetResource.h
	RTL/Component/Generators/PointSet/CIFXAuthorPointSet.h
	RTL/Component/Generators/PointSet/CIFXAuthorPointSetResource.h
	RTL/Component/Mesh/CIFXInterleavedData.h
	RTL/Component/Mesh/CIFXMesh.h
	RTL/Component/Mesh/CIFXMeshGroup.h
	RTL/Component/Mesh/CIFXNeighborMesh.h
	RTL/Component/Mesh/CIFXRenderable.h
	RTL/Component/Mesh/IFXFaceLists.h
	RTL/Component/ModifierChain/CIFXDidRegistry.h
	RTL/Component/ModifierChain/CIFXModifier.h
	RTL/Component/ModifierChain/CIFXModifierChain.h
	RTL/Component/ModifierChain/CIFXModifierDataElementIter.h
	RTL/Component/ModifierChain/CIFXModifierDataPacket.h
	RTL/Component/ModifierChain/CIFXObserverStateTree.h
	RTL/Component/ModifierChain/CIFXSubject.h
	RTL/Component/ModifierChain/CRedBlackTree.h
	RTL/Component/ModifierChain/IFXModifierChainInternal.h
	RTL/Component/ModifierChain/IFXModifierChainState.h
	RTL/Component/ModifierChain/IFXModifierDataPacketInternal.h
	RTL/Component/ModifierChain/IFXSet.h
	RTL/Component/Palette/CIFXPalette.h
	RTL/Component/Palette/CIFXSimpleObject.h
	RTL/Component/Rendering/CIFXDeviceBase.h
	RTL/Component/Rendering/CIFXDeviceLight.h
	RTL/Component/Rendering/CIFXDeviceTexture.h
	RTL/Component/Rendering/CIFXDeviceTexUnit.h
	RTL/Component/Rendering/CIFXRender.h
	RTL/Component/Rendering/CIFXRenderContext.h
	RTL/Component/Rendering/CIFXRenderDevice.h
	RTL/Component/Rendering/CIFXRenderServices.h
	RTL/Component/Rendering/DX7/CIFXDeviceLightDX7.h
	RTL/Component/Rendering/DX7/CIFXDeviceTextureDX7.h
	RTL/Component/Rendering/DX7/CIFXDeviceTexUnitDX7.h
	RTL/Component/Rendering/DX7/CIFXDirectX7.h
	RTL/Component/Rendering/DX7/CIFXRenderDeviceDX7.h
	RTL/Component/Rendering/DX7/CIFXRenderDX7.h
	RTL/Component/Rendering/DX7/IFXRenderPCHDX7.h
	RTL/Component/Rendering/DX8/CIFXDeviceLightDX8.h
	RTL/Component/Rendering/DX8/CIFXDeviceTextureDX8.h
	RTL/Component/Rendering/DX8/CIFXDeviceTexUnitDX8.h
	RTL/Component/Rendering/DX8/CIFXDirectX8.h
	RTL/Component/Rendering/DX8/CIFXRenderDeviceDX8.h
	RTL/Component/Rendering/DX8/CIFXRenderDX8.h
	RTL/Component/Rendering/DX8/IFXRenderPCHDX8.h
	RTL/Component/Rendering/IFXAAFilter.h
	RTL/Component/Rendering/IFXRenderPCH.h
	RTL/Component/Rendering/Null/CIFXDeviceLightNULL.h
	RTL/Component/Rendering/Null/CIFXDeviceTextureNULL.h
	RTL/Component/Rendering/Null/CIFXDeviceTexUnitNULL.h
	RTL/Component/Rendering/Null/CIFXRenderDeviceNULL.h
	RTL/Component/Rendering/Null/CIFXRenderNULL.h
	RTL/Component/Rendering/Null/IFXRenderPCHNULL.h
	RTL/Component/Rendering/OpenGL/CIFXDeviceLightOGL.h
	RTL/Component/Rendering/OpenGL/CIFXDeviceTextureOGL.h
	RTL/Component/Rendering/OpenGL/CIFXDeviceTexUnitOGL.h
	RTL/Component/Rendering/OpenGL/CIFXOpenGL.h
	RTL/Component/Rendering/OpenGL/CIFXRenderDeviceOGL.h
	RTL/Component/Rendering/OpenGL/CIFXRenderOGL.h
	RTL/Component/Rendering/OpenGL/IFXRenderPCHOGL.h
	RTL/Component/SceneGraph/CIFXBoundSphereDataElement.h
	RTL/Component/SceneGraph/CIFXDevice.h
	RTL/Component/SceneGraph/CIFXDummyModifier.h
	RTL/Component/SceneGraph/CIFXFileReference.h
	RTL/Component/SceneGraph/CIFXGroup.h
	RTL/Component/SceneGraph/CIFXLight.h
	RTL/Component/SceneGraph/CIFXLightResource.h
	RTL/Component/SceneGraph/CIFXLightSet.h
	RTL/Component/SceneGraph/CIFXMarker.h
	RTL/Component/SceneGraph/CIFXMaterialResource.h
	RTL/Component/SceneGraph/CIFXMixerConstruct.h
	RTL/Component/SceneGraph/CIFXModel.h
	RTL/Component/SceneGraph/CIFXMotionResource.h
	RTL/Component/SceneGraph/CIFXNode.h
	RTL/Component/SceneGraph/CIFXResourceClient.h
	RTL/Component/SceneGraph/CIFXSceneGraph.h
	RTL/Component/SceneGraph/CIFXShaderList.h
	RTL/Component/SceneGraph/CIFXSimpleCollection.h
	RTL/Component/SceneGraph/CIFXSimpleList.h
	RTL/Component/SceneGraph/CIFXView.h
	RTL/Component/SceneGraph/CIFXViewResource.h
	RTL/Component/SceneGraph/IFXSceneGraphPCH.h
	RTL/Component/Shaders/CIFXShader.h
	RTL/Component/Shaders/CIFXShaderLitTexture.h
	RTL/Component/Shaders/CIFXShadingModifier.h
	RTL/Component/Subdiv/CIFXSubdivModifier.h
	RTL/Component/Subdiv/IFXAttributeNeighborhood.h
	RTL/Component/Subdiv/IFXBFMaskLayout.h
	RTL/Component/Subdiv/IFXButterflyMask.h
	RTL/Component/Subdiv/IFXButterflyScheme.h
	RTL/Component/Subdiv/IFXScreenSpaceMetric.h
	RTL/Component/Subdiv/IFXSharedUnitAllocator.h
	RTL/Component/Subdiv/IFXSpecularMetric.h
	RTL/Component/Subdiv/IFXSubdivisionManager.h
	RTL/Component/Subdiv/IFXTQTAddress.h
	RTL/Component/Subdiv/IFXTQTAttribute.h
	RTL/Component/Subdiv/IFXTQTBaseTriangle.h
	RTL/Component/Subdiv/IFXTQTTriangle.h
	RTL/Component/Subdiv/IFXTQTTriangleAllocator.h
	RTL/Component/Subdiv/IFXTQTVertex.h
	RTL/Component/Subdiv/IFXVertexAllocator.h
	RTL/Component/Texture/CIFXImageTools.h
	RTL/Component/Texture/CIFXTextureImageTools.h
	RTL/Component/Texture/CIFXTextureObject.h
	RTL/Component/Texture/CIFXUtilities.h
	RTL/Component/Texture/IFXTextureErrors.h
	RTL/Component/UVGenerator/CIFXUVGenerator.h
	RTL/Component/UVGenerator/CIFXUVMapperCylindrical.h
	RTL/Component/UVGenerator/CIFXUVMapperNone.h
	RTL/Component/UVGenerator/CIFXUVMapperPlanar.h
	RTL/Component/UVGenerator/CIFXUVMapperReflection.h
	RTL/Component/UVGenerator/CIFXUVMapperSpherical.h
	RTL/Kernel/IFXCom/CIFXComponentManager.h
	RTL/Kernel/IFXCom/CIFXGuidHashMap.h
	RTL/Kernel/IFXCom/CIFXPluginProxy.h
	RTL/Kernel/Common/CIFXConnector.h
	RTL/Kernel/Common/CIFXPerformanceTimer.h
	RTL/Dependencies/FNVHash/FNVPlusPlus.h
	RTL/Dependencies/Predicates/predicates.h
	RTL/Dependencies/WildCards/wcmatch.h
)
IF(WIN32)
  SET( CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/RTL/Platform/Win32/IFXCore )
  ADD_LIBRARY( IFXCore SHARED ${IFXCore_SRCS} ${IFXCore_HDRS} ${CORE_DIR}/IFXCore.rc ${CORE_DIR}/IFXResource.h ${CORE_DIR}/IFXCore.def ${DEPENDENCIES_SRCS} )
ENDIF(WIN32)
IF(APPLE)
  ADD_LIBRARY( IFXCore MODULE ${IFXCore_SRCS} ${IFXCore_HDRS} ${DEPENDENCIES_SRCS} )
  set_target_properties( IFXCore PROPERTIES
	LINK_FLAGS "${MY_LINK_FLAGS} -exported_symbols_list ${CMAKE_CURRENT_SOURCE_DIR}/RTL/Platform/Mac32/IFXCore/IFXCore.def" )
ENDIF(APPLE)
IF(UNIX AND NOT APPLE)
  ADD_LIBRARY( IFXCore MODULE ${IFXCore_SRCS} ${IFXCore_HDRS} ${DEPENDENCIES_SRCS} )
  set_target_properties( IFXCore PROPERTIES
	LINK_FLAGS "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/RTL/Platform/Lin32/IFXCore/IFXCore.list -Wl,--no-undefined" )
  target_link_libraries( IFXCore ${CMAKE_DL_LIBS} )
ENDIF(UNIX AND NOT APPLE)
TARGET_LINK_LIBRARIES( IFXCore ${ADDITIONAL_LIBRARIES} )
install(
	TARGETS IFXCore
	RUNTIME DESTINATION ${BIN_DESTINATION}
	ARCHIVE DESTINATION ${LIB_DESTINATION}
	LIBRARY DESTINATION ${LIB_DESTINATION}
)


INCLUDE_DIRECTORIES( RTL/Component/Include RTL/Kernel/Include RTL/Platform/Include
	RTL/Component/Exporting
	RTL/Dependencies/WildCards )
SET( IFXExporting_HDRS
	${Component_HDRS}
	${Kernel_HDRS}
	${Platform_HDRS}
	RTL/Component/Exporting/CIFXAnimationModifierEncoder.h
	RTL/Component/Exporting/CIFXAuthorCLODEncoderX.h
	RTL/Component/Exporting/CIFXAuthorGeomCompiler.h
	RTL/Component/Exporting/CIFXBlockPriorityQueueX.h
	RTL/Component/Exporting/CIFXBlockWriterX.h
	RTL/Component/Exporting/CIFXBoneWeightsModifierEncoder.h
	RTL/Component/Exporting/CIFXCLODModifierEncoder.h
	RTL/Component/Exporting/CIFXDummyModifierEncoder.h
	RTL/Component/Exporting/CIFXFileReferenceEncoder.h
	RTL/Component/Exporting/CIFXGlyphModifierEncoder.h
	RTL/Component/Exporting/CIFXGroupNodeEncoder.h
	RTL/Component/Exporting/CIFXLightNodeEncoder.h
	RTL/Component/Exporting/CIFXLightResourceEncoder.h
	RTL/Component/Exporting/CIFXLineSetEncoder.h
	RTL/Component/Exporting/CIFXMaterialResourceEncoder.h
	RTL/Component/Exporting/CIFXModelNodeEncoder.h
	RTL/Component/Exporting/CIFXMotionResourceEncoder.h
	RTL/Component/Exporting/CIFXNodeBaseEncoder.h
	RTL/Component/Exporting/CIFXPointSetEncoder.h
	RTL/Component/Exporting/CIFXShaderLitTextureEncoder.h
	RTL/Component/Exporting/CIFXShadingModifierEncoder.h
	RTL/Component/Exporting/CIFXStdioWriteBufferX.h
	RTL/Component/Exporting/CIFXSubdivisionModifierEncoder.h
	RTL/Component/Exporting/CIFXViewNodeEncoder.h
	RTL/Component/Exporting/CIFXViewResourceEncoder.h
	RTL/Component/Exporting/CIFXWriteManager.h
	RTL/Dependencies/WildCards/wcmatch.h
)
SET( IFXExporting_SRCS
	RTL/Platform/${U3D_PLATFORM}/IFXExporting/IFXExportingDllMain.cpp
	RTL/Component/Exporting/CIFXAnimationModifierEncoder.cpp
	RTL/Component/Exporting/CIFXAuthorCLODEncoderX.cpp
	RTL/Component/Exporting/CIFXAuthorCLODEncoderX_P.cpp
	RTL/Component/Exporting/CIFXAuthorCLODEncoderX_S.cpp
	RTL/Component/Exporting/CIFXAuthorGeomCompiler.cpp
	RTL/Component/Exporting/CIFXBlockPriorityQueueX.cpp
	RTL/Component/Exporting/CIFXBlockWriterX.cpp
	RTL/Component/Exporting/CIFXBoneWeightsModifierEncoder.cpp
	RTL/Component/Exporting/CIFXCLODModifierEncoder.cpp
	RTL/Component/Exporting/CIFXDummyModifierEncoder.cpp
	RTL/Component/Exporting/CIFXFileReferenceEncoder.cpp
	RTL/Component/Exporting/CIFXGlyphModifierEncoder.cpp
	RTL/Component/Exporting/CIFXGroupNodeEncoder.cpp
	RTL/Component/Exporting/CIFXLightNodeEncoder.cpp
	RTL/Component/Exporting/CIFXLightResourceEncoder.cpp
	RTL/Component/Exporting/CIFXLineSetEncoder.cpp
	RTL/Component/Exporting/CIFXMaterialResourceEncoder.cpp
	RTL/Component/Exporting/CIFXModelNodeEncoder.cpp
	RTL/Component/Exporting/CIFXMotionResourceEncoder.cpp
	RTL/Component/Exporting/CIFXNodeBaseEncoder.cpp
	RTL/Component/Exporting/CIFXPointSetEncoder.cpp
	RTL/Component/Exporting/CIFXShaderLitTextureEncoder.cpp
	RTL/Component/Exporting/CIFXShadingModifierEncoder.cpp
	RTL/Component/Exporting/CIFXStdioWriteBufferX.cpp
	RTL/Component/Exporting/CIFXSubdivisionModifierEncoder.cpp
	RTL/Component/Exporting/CIFXViewNodeEncoder.cpp
	RTL/Component/Exporting/CIFXViewResourceEncoder.cpp
	RTL/Component/Exporting/CIFXWriteManager.cpp
	RTL/Component/Exporting/IFXExporting.cpp
	RTL/Component/Exporting/IFXExportingGuids.cpp
	RTL/IFXCorePluginStatic/IFXCorePluginStatic.cpp
	RTL/Platform/${U3D_PLATFORM}/Common/IFXOSUtilities.cpp
	RTL/Platform/${U3D_PLATFORM}/Common/IFXOSLoader.cpp
	RTL/Component/Base/IFXVertexMap.cpp
	RTL/Kernel/DataTypes/IFXCoreArray.cpp
	RTL/Kernel/DataTypes/IFXCoreList.cpp
	RTL/Kernel/DataTypes/IFXFastAllocator.cpp
	RTL/Kernel/DataTypes/IFXListNode.cpp
	RTL/Kernel/DataTypes/IFXMatrix4x4.cpp
	RTL/Kernel/DataTypes/IFXQuaternion.cpp
	RTL/Kernel/DataTypes/IFXString.cpp
	RTL/Kernel/DataTypes/IFXUnitAllocator.cpp
	RTL/Kernel/DataTypes/IFXVector3.cpp
	RTL/Kernel/DataTypes/IFXVector4.cpp
	RTL/Dependencies/WildCards/wcmatch.cpp
	RTL/Kernel/Common/IFXDebug.cpp
)
IF(WIN32)
  SET( EXPORT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/RTL/Platform/Win32/IFXExporting )
  ADD_LIBRARY( IFXExporting  SHARED ${IFXExporting_SRCS} ${IFXExporting_HDRS} ${EXPORT_DIR}/IFXExporting.rc ${EXPORT_DIR}/IFXResource.h ${EXPORT_DIR}/IFXExporting.def )
  TARGET_LINK_LIBRARIES( IFXExporting IFXCore )
ENDIF(WIN32)
IF(APPLE)
  ADD_LIBRARY( IFXExporting  MODULE ${IFXExporting_SRCS} ${IFXExporting_HDRS} )
  set_target_properties( IFXExporting  PROPERTIES
	LINK_FLAGS "${MY_LINK_FLAGS} -exported_symbols_list ${CMAKE_CURRENT_SOURCE_DIR}/RTL/Platform/Mac32/IFXExporting/IFXExporting.def   -undefined dynamic_lookup" )
ENDIF(APPLE)
IF(UNIX AND NOT APPLE)
  ADD_LIBRARY( IFXExporting  MODULE ${IFXExporting_SRCS} ${IFXExporting_HDRS} )
  set_target_properties( IFXExporting  PROPERTIES
	LINK_FLAGS "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/RTL/Platform/Lin32/IFXExporting/IFXExporting.list" )
ENDIF(UNIX AND NOT APPLE)
install(
	TARGETS IFXExporting
	RUNTIME DESTINATION ${BIN_DESTINATION}
	ARCHIVE DESTINATION ${LIB_DESTINATION}
	LIBRARY DESTINATION ${PLUGIN_DESTINATION}
)

#INCLUDE_DIRECTORIES( RTL/Component/Include RTL/Kernel/Include RTL/Platform/Include
#	RTL/Component/Importing
#	RTL/Dependencies/WildCards )
#SET( IFXImporting_HDRS
#	${Component_HDRS}
#	${Kernel_HDRS}
#	${Platform_HDRS}
#	RTL/Component/Importing/CIFXAnimationModifierDecoder.h
#	RTL/Component/Importing/CIFXAuthorCLODDecoder.h
#	RTL/Component/Importing/CIFXBlockReaderX.h
#	RTL/Component/Importing/CIFXBoneWeightsModifierDecoder.h
#	RTL/Component/Importing/CIFXBTTHash.h
#	RTL/Component/Importing/CIFXCLODModifierDecoder.h
#	RTL/Component/Importing/CIFXDecoderChainX.h
#	RTL/Component/Importing/CIFXDummyModifierDecoder.h
#	RTL/Component/Importing/CIFXGlyphModifierDecoder.h
#	RTL/Component/Importing/CIFXGroupDecoder.h
#	RTL/Component/Importing/CIFXInternetReadBufferX.h
#	RTL/Component/Importing/CIFXLightDecoder.h
#	RTL/Component/Importing/CIFXLightResourceDecoder.h
#	RTL/Component/Importing/CIFXLineSetDecoder.h
#	RTL/Component/Importing/CIFXLoadManager.h
#	RTL/Component/Importing/CIFXMaterialDecoder.h
#	RTL/Component/Importing/CIFXModelDecoder.h
#	RTL/Component/Importing/CIFXMotionDecoder.h
#	RTL/Component/Importing/CIFXNodeDecoder.h
#	RTL/Component/Importing/CIFXPointSetDecoder.h
#	RTL/Component/Importing/CIFXShaderLitTextureDecoder.h
#	RTL/Component/Importing/CIFXShadingModifierDecoder.h
#	RTL/Component/Importing/CIFXStdioReadBufferX.h
#	RTL/Component/Importing/CIFXSubdivisionModifierDecoder.h
#	RTL/Component/Importing/CIFXTextureDecoder.h
#	RTL/Component/Importing/CIFXViewDecoder.h
#	RTL/Component/Importing/CIFXViewResourceDecoder.h
#	RTL/Component/Importing/IFXInternetConnectionX.h
#	RTL/Component/Importing/IFXInternetSessionX.h
#	RTL/Component/Importing/IFXSocketStream.h
#	RTL/Dependencies/WildCards/wcmatch.h
#)
#SET( IFXImporting_SRCS
#	RTL/Platform/${U3D_PLATFORM}/IFXImporting/IFXImportingDllMain.cpp
#	RTL/Component/Importing/CIFXAnimationModifierDecoder.cpp
#	RTL/Component/Importing/CIFXAuthorCLODDecoder.cpp
#	RTL/Component/Importing/CIFXAuthorCLODDecoder_P.cpp
#	RTL/Component/Importing/CIFXAuthorCLODDecoder_S.cpp
#	RTL/Component/Importing/CIFXBlockReaderX.cpp
#	RTL/Component/Importing/CIFXBoneWeightsModifierDecoder.cpp
#	RTL/Component/Importing/CIFXBTTHash.cpp
#	RTL/Component/Importing/CIFXCLODModifierDecoder.cpp
#	RTL/Component/Importing/CIFXDecoderChainX.cpp
#	RTL/Component/Importing/CIFXDummyModifierDecoder.cpp
#	RTL/Component/Importing/CIFXGlyphModifierDecoder.cpp
#	RTL/Component/Importing/CIFXGroupDecoder.cpp
#	RTL/Component/Importing/CIFXInternetReadBufferX.cpp
#	RTL/Component/Importing/CIFXLightDecoder.cpp
#	RTL/Component/Importing/CIFXLightResourceDecoder.cpp
#	RTL/Component/Importing/CIFXLineSetDecoder.cpp
#	RTL/Component/Importing/CIFXLoadManager.cpp
#	RTL/Component/Importing/CIFXMaterialDecoder.cpp
#	RTL/Component/Importing/CIFXModelDecoder.cpp
#	RTL/Component/Importing/CIFXMotionDecoder.cpp
#	RTL/Component/Importing/CIFXNodeDecoder.cpp
#	RTL/Component/Importing/CIFXPointSetDecoder.cpp
#	RTL/Component/Importing/CIFXShaderLitTextureDecoder.cpp
#	RTL/Component/Importing/CIFXShadingModifierDecoder.cpp
#	RTL/Component/Importing/CIFXStdioReadBufferX.cpp
#	RTL/Component/Importing/CIFXSubdivisionModifierDecoder.cpp
#	RTL/Component/Importing/CIFXTextureDecoder.cpp
#	RTL/Component/Importing/CIFXViewDecoder.cpp
#	RTL/Component/Importing/CIFXViewResourceDecoder.cpp
#	RTL/Component/Importing/IFXImporting.cpp
#	RTL/Component/Importing/IFXImportingGuids.cpp
#	RTL/Component/Importing/IFXInternetConnectionX.cpp
#	RTL/Component/Importing/IFXInternetSessionX.cpp
#	RTL/Component/Importing/IFXModifierBaseDecoder.cpp
#	RTL/Component/Importing/IFXSocketStream.cpp
#	RTL/IFXCorePluginStatic/IFXCorePluginStatic.cpp
#	RTL/Platform/${U3D_PLATFORM}/Common/IFXOSUtilities.cpp
#	RTL/Platform/${U3D_PLATFORM}/Common/IFXOSLoader.cpp
#	RTL/Platform/${U3D_PLATFORM}/Common/IFXOSSocket.cpp
#	RTL/Kernel/DataTypes/IFXCoreArray.cpp
#	RTL/Kernel/DataTypes/IFXCoreList.cpp
#	RTL/Kernel/DataTypes/IFXFastAllocator.cpp
#	RTL/Kernel/DataTypes/IFXListNode.cpp
#	RTL/Kernel/DataTypes/IFXMatrix4x4.cpp
#	RTL/Kernel/DataTypes/IFXQuaternion.cpp
#	RTL/Kernel/DataTypes/IFXString.cpp
#	RTL/Kernel/DataTypes/IFXUnitAllocator.cpp
#	RTL/Kernel/DataTypes/IFXVector3.cpp
#	RTL/Kernel/DataTypes/IFXVector4.cpp
#	RTL/Dependencies/WildCards/wcmatch.cpp
#	RTL/Kernel/Common/IFXDebug.cpp
#	)
#IF(WIN32)
#  SET( IMPORT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/RTL/Platform/Win32/IFXImporting )
#  ADD_LIBRARY( IFXImporting  SHARED ${IFXImporting_SRCS} ${IFXImporting_HDRS} ${IMPORT_DIR}/IFXImporting.rc ${IMPORT_DIR}/IFXResource.h ${IMPORT_DIR}/IFXImporting.def )
#  TARGET_LINK_LIBRARIES( IFXImporting IFXCore ws2_32 )
#ENDIF(WIN32)
#IF(APPLE)
#  ADD_LIBRARY( IFXImporting  MODULE ${IFXImporting_SRCS} ${IFXImporting_HDRS} )
#  set_target_properties( IFXImporting  PROPERTIES
#	LINK_FLAGS "${MY_LINK_FLAGS} -exported_symbols_list ${CMAKE_CURRENT_SOURCE_DIR}/RTL/Platform/Mac32/IFXImporting/IFXImporting.def   -undefined dynamic_lookup" )
#ENDIF(APPLE)
#IF(UNIX AND NOT APPLE)
#  ADD_LIBRARY( IFXImporting  MODULE ${IFXImporting_SRCS} ${IFXImporting_HDRS} )
#  set_target_properties( IFXImporting  PROPERTIES
#	LINK_FLAGS "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/RTL/Platform/Lin32/IFXImporting/IFXImporting.list" )
#ENDIF(UNIX AND NOT APPLE)
#install(
#	TARGETS IFXImporting
#	RUNTIME DESTINATION ${BIN_DESTINATION}
#	ARCHIVE DESTINATION ${LIB_DESTINATION}
#	LIBRARY DESTINATION ${PLUGIN_DESTINATION}
#)

INCLUDE_DIRECTORIES( RTL/Component/Include RTL/Kernel/Include RTL/Platform/Include
	RTL/Component/ModifierChain
	RTL/Component/SceneGraph
	RTL/Component/Scheduling
	RTL/Dependencies/WildCards )
SET( IFXScheduling_HDRS
	${Component_HDRS}
	${Kernel_HDRS}
	${Platform_HDRS}
	RTL/Component/ModifierChain/CIFXDidRegistry.h
	RTL/Component/ModifierChain/CIFXModifier.h
	RTL/Component/ModifierChain/CIFXModifierChain.h
	RTL/Component/ModifierChain/CIFXModifierDataElementIter.h
	RTL/Component/ModifierChain/CIFXModifierDataPacket.h
	RTL/Component/ModifierChain/CIFXObserverStateTree.h
	RTL/Component/ModifierChain/CIFXSubject.h
	RTL/Component/ModifierChain/CRedBlackTree.h
	RTL/Component/ModifierChain/IFXModifierChainInternal.h
	RTL/Component/ModifierChain/IFXModifierChainState.h
	RTL/Component/ModifierChain/IFXModifierDataPacketInternal.h
	RTL/Component/ModifierChain/IFXSet.h
	RTL/Component/SceneGraph/CIFXBoundSphereDataElement.h
	RTL/Component/SceneGraph/CIFXDevice.h
	RTL/Component/SceneGraph/CIFXDummyModifier.h
	RTL/Component/SceneGraph/CIFXFileReference.h
	RTL/Component/SceneGraph/CIFXGroup.h
	RTL/Component/SceneGraph/CIFXLight.h
	RTL/Component/SceneGraph/CIFXLightResource.h
	RTL/Component/SceneGraph/CIFXLightSet.h
	RTL/Component/SceneGraph/CIFXMarker.h
	RTL/Component/SceneGraph/CIFXMaterialResource.h
	RTL/Component/SceneGraph/CIFXMixerConstruct.h
	RTL/Component/SceneGraph/CIFXModel.h
	RTL/Component/SceneGraph/CIFXMotionResource.h
	RTL/Component/SceneGraph/CIFXNode.h
	RTL/Component/SceneGraph/CIFXResourceClient.h
	RTL/Component/SceneGraph/CIFXSceneGraph.h
	RTL/Component/SceneGraph/CIFXShaderList.h
	RTL/Component/SceneGraph/CIFXSimpleCollection.h
	RTL/Component/SceneGraph/CIFXSimpleList.h
	RTL/Component/SceneGraph/CIFXView.h
	RTL/Component/SceneGraph/CIFXViewResource.h
	RTL/Component/SceneGraph/IFXSceneGraphPCH.h
	RTL/Component/Scheduling/CIFXClock.h
	RTL/Component/Scheduling/CIFXErrorInfo.h
	RTL/Component/Scheduling/CIFXNotificationInfo.h
	RTL/Component/Scheduling/CIFXNotificationManager.h
	RTL/Component/Scheduling/CIFXScheduler.h
	RTL/Component/Scheduling/CIFXSchedulerInfo.h
	RTL/Component/Scheduling/CIFXSimulationInfo.h
	RTL/Component/Scheduling/CIFXSimulationManager.h
	RTL/Component/Scheduling/CIFXSystemManager.h
	RTL/Component/Scheduling/CIFXTaskCallback.h
	RTL/Component/Scheduling/CIFXTaskData.h
	RTL/Component/Scheduling/CIFXTaskManager.h
	RTL/Component/Scheduling/CIFXTaskManagerNode.h
	RTL/Component/Scheduling/CIFXTaskManagerView.h
	RTL/Component/Scheduling/CIFXTimeManager.h
	RTL/Dependencies/WildCards/wcmatch.h
)
SET( IFXScheduling_SRCS
	RTL/Platform/${U3D_PLATFORM}/IFXScheduling/IFXSchedulingDllMain.cpp
	RTL/Component/Scheduling/CIFXClock.cpp
	RTL/Component/Scheduling/CIFXErrorInfo.cpp
	RTL/Component/Scheduling/CIFXNotificationInfo.cpp
	RTL/Component/Scheduling/CIFXNotificationManager.cpp
	RTL/Component/Scheduling/CIFXScheduler.cpp
	RTL/Component/Scheduling/CIFXSchedulerInfo.cpp
	RTL/Component/Scheduling/CIFXSimulationInfo.cpp
	RTL/Component/Scheduling/CIFXSimulationManager.cpp
	RTL/Component/Scheduling/CIFXSystemManager.cpp
	RTL/Component/Scheduling/CIFXTaskCallback.cpp
	RTL/Component/Scheduling/CIFXTaskData.cpp
	RTL/Component/Scheduling/CIFXTaskManager.cpp
	RTL/Component/Scheduling/CIFXTaskManagerNode.cpp
	RTL/Component/Scheduling/CIFXTaskManagerView.cpp
	RTL/Component/Scheduling/CIFXTimeManager.cpp
	RTL/Component/Scheduling/IFXScheduling.cpp
	RTL/Component/Scheduling/IFXSchedulingGuids.cpp
	RTL/IFXCorePluginStatic/IFXCorePluginStatic.cpp
	RTL/Platform/${U3D_PLATFORM}/Common/IFXOSUtilities.cpp
	RTL/Component/ModifierChain/CIFXSubject.cpp
	RTL/Component/ModifierChain/CIFXModifier.cpp
	RTL/Component/SceneGraph/CIFXMarker.cpp
	RTL/Kernel/DataTypes/IFXCoreArray.cpp
	RTL/Kernel/DataTypes/IFXCoreList.cpp
	RTL/Kernel/DataTypes/IFXFastAllocator.cpp
	RTL/Kernel/DataTypes/IFXListNode.cpp
	RTL/Kernel/DataTypes/IFXMatrix4x4.cpp
	RTL/Kernel/DataTypes/IFXQuaternion.cpp
	RTL/Kernel/DataTypes/IFXString.cpp
	RTL/Kernel/DataTypes/IFXUnitAllocator.cpp
	RTL/Kernel/DataTypes/IFXVector3.cpp
	RTL/Kernel/DataTypes/IFXVector4.cpp
	RTL/Dependencies/WildCards/wcmatch.cpp
	RTL/Kernel/Common/IFXDebug.cpp
	)
IF(WIN32)
  SET( SCHED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/RTL/Platform/Win32/IFXScheduling )
  ADD_LIBRARY( IFXScheduling  SHARED ${IFXScheduling_SRCS} ${IFXScheduling_HDRS} ${SCHED_DIR}/IFXScheduling.rc ${SCHED_DIR}/IFXResource.h ${SCHED_DIR}/IFXScheduling.def )
  TARGET_LINK_LIBRARIES( IFXScheduling IFXCore )
ENDIF(WIN32)
IF(APPLE)
  ADD_LIBRARY( IFXScheduling  MODULE ${IFXScheduling_SRCS} ${IFXScheduling_HDRS} )
  set_target_properties( IFXScheduling  PROPERTIES
	LINK_FLAGS "${MY_LINK_FLAGS} -exported_symbols_list ${CMAKE_CURRENT_SOURCE_DIR}/RTL/Platform/Mac32/IFXScheduling/IFXScheduling.def   -undefined dynamic_lookup" )
ENDIF(APPLE)
IF(UNIX AND NOT APPLE)
  ADD_LIBRARY( IFXScheduling  MODULE ${IFXScheduling_SRCS} ${IFXScheduling_HDRS} )
  set_target_properties( IFXScheduling  PROPERTIES
	LINK_FLAGS "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/RTL/Platform/Lin32/IFXScheduling/IFXScheduling.list" )
ENDIF(UNIX AND NOT APPLE)
install(
	TARGETS IFXScheduling
	RUNTIME DESTINATION ${BIN_DESTINATION}
	ARCHIVE DESTINATION ${LIB_DESTINATION}
	LIBRARY DESTINATION ${PLUGIN_DESTINATION}
)

INCLUDE_DIRECTORIES( RTL/Component/Include RTL/Kernel/Include RTL/Platform/Include
	IDTF
	IDTF/Include
	IDTF/Common )
SET( IDTFConverter_SRCS
	IDTF/Converter.cpp
	IDTF/FileParser.cpp
	IDTF/SceneConverter.cpp
	IDTF/PointSetResourceParser.cpp
	IDTF/UrlListParser.cpp
	IDTF/NodeParser.cpp
	IDTF/ModifierParser.cpp
	IDTF/PointSetConverter.cpp
	IDTF/MaterialParser.cpp
	IDTF/MetaDataConverter.cpp
	IDTF/MeshResourceParser.cpp
	IDTF/ResourceConverter.cpp
	IDTF/TextureConverter.cpp
	IDTF/ResourceListParser.cpp
	IDTF/File.cpp
	IDTF/LineSetConverter.cpp
	#IDTF/ConverterDriver.cpp
	IDTF/ModelConverter.cpp
	IDTF/TextureParser.cpp
	IDTF/NodeConverter.cpp
	IDTF/MeshConverter.cpp
	IDTF/BlockParser.cpp
	IDTF/ModelResourceParser.cpp
	IDTF/FileReferenceConverter.cpp
	IDTF/ShaderParser.cpp
	IDTF/FileScanner.cpp
	IDTF/FileReferenceParser.cpp
	IDTF/ModifierConverter.cpp
	IDTF/MetaDataParser.cpp
	IDTF/LineSetResourceParser.cpp
	IDTF/Helpers/MiscUtilities.cpp
	IDTF/Helpers/TGAImage.cpp
	IDTF/Helpers/ModifierUtilities.cpp
	IDTF/Helpers/ConverterHelpers.cpp
	IDTF/Helpers/SceneUtilities.cpp
	IDTF/Helpers/DebugInfo.cpp
	IDTF/Helpers/Guids.cpp
	IDTF/Common/GlyphModifier.cpp
	IDTF/Common/ModelResource.cpp
	IDTF/Common/ModifierList.cpp
	IDTF/Common/NodeList.cpp
	IDTF/Common/FileReference.cpp
	IDTF/Common/ResourceList.cpp
	IDTF/Common/SceneResources.cpp
	IDTF/Common/ModelResourceList.cpp
	IDTF/Common/MetaDataList.cpp
	IDTF/Common/ParentList.cpp
	IDTF/Common/GlyphCommandList.cpp
)
SET( IDTFConverter_HDRS
	${Component_HDRS}
	${Kernel_HDRS}
	${Platform_HDRS}
	IDTF/Converter.h
	IDTF/BlockParser.h
	IDTF/DefaultSettings.h
	IDTF/File.h
	IDTF/FileParser.h
	IDTF/FileReferenceConverter.h
	IDTF/FileReferenceParser.h
	IDTF/FileScanner.h
	IDTF/IConverter.h
	IDTF/LineSetConverter.h
	IDTF/LineSetResourceParser.h
	IDTF/MaterialParser.h
	IDTF/MeshConverter.h
	IDTF/MeshResourceParser.h
	IDTF/MetaDataConverter.h
	IDTF/MetaDataParser.h
	IDTF/ModelConverter.h
	IDTF/ModelResourceParser.h
	IDTF/ModifierConverter.h
	IDTF/ModifierParser.h
	IDTF/NodeConverter.h
	IDTF/NodeParser.h
	IDTF/PointSetConverter.h
	IDTF/PointSetResourceParser.h
	IDTF/ResourceConverter.h
	IDTF/ResourceListParser.h
	IDTF/SceneConverter.h
	IDTF/ShaderParser.h
	IDTF/TextureConverter.h
	IDTF/TextureParser.h
	IDTF/UrlListParser.h
	IDTF/Include/ConverterHelpers.h
	IDTF/Include/ConverterOptions.h
	IDTF/Include/ConverterResult.h
	IDTF/Include/DebugInfo.h
	IDTF/Include/SceneUtilities.h
	IDTF/Include/TGAImage.h
	IDTF/Include/U3DHeaders.h
	IDTF/Common/AnimationModifier.h
	IDTF/Common/BoneWeightModifier.h
	IDTF/Common/CLODModifier.h
	IDTF/Common/Color.h
	IDTF/Common/FileReference.h
	IDTF/Common/GlyphCommandList.h
	IDTF/Common/GlyphCommands.h
	IDTF/Common/GlyphModifier.h
	IDTF/Common/INode.h
	IDTF/Common/Int2.h
	IDTF/Common/Int3.h
	IDTF/Common/IResource.h
	IDTF/Common/LightNode.h
	IDTF/Common/LightResource.h
	IDTF/Common/LightResourceList.h
	IDTF/Common/LineSetResource.h
	IDTF/Common/MaterialResource.h
	IDTF/Common/MaterialResourceList.h
	IDTF/Common/MeshResource.h
	IDTF/Common/MetaDataList.h
	IDTF/Common/ModelNode.h
	IDTF/Common/ModelResource.h
	IDTF/Common/ModelResourceList.h
	IDTF/Common/ModelSkeleton.h
	IDTF/Common/Modifier.h
	IDTF/Common/ModifierList.h
	IDTF/Common/MotionResource.h
	IDTF/Common/MotionResourceList.h
	IDTF/Common/Node.h
	IDTF/Common/NodeList.h
	IDTF/Common/ParentData.h
	IDTF/Common/ParentList.h
	IDTF/Common/Point.h
	IDTF/Common/PointSetResource.h
	IDTF/Common/Quat.h
	IDTF/Common/Resource.h
	IDTF/Common/ResourceList.h
	IDTF/Common/SceneData.h
	IDTF/Common/SceneResources.h
	IDTF/Common/ShaderResource.h
	IDTF/Common/ShaderResourceList.h
	IDTF/Common/ShadingDescription.h
	IDTF/Common/ShadingDescriptionList.h
	IDTF/Common/ShadingModifier.h
	IDTF/Common/SubdivisionModifier.h
	IDTF/Common/TextureResource.h
	IDTF/Common/TextureResourceList.h
	IDTF/Common/Tokens.h
	IDTF/Common/UrlList.h
	IDTF/Common/ViewNodeData.h
	IDTF/Common/ViewNode.h
	IDTF/Common/ViewResource.h
	IDTF/Common/ViewResourceList.h
)
IF(WIN32)
  SET( CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/RTL/Platform/Win32/IDTFConverter )
  ADD_LIBRARY( IDTFConverter STATIC ${IDTFConverter_SRCS} ${IDTFConverter_HDRS} IDTF/IDTFConverter.rc IDTF/IFXResource.h)
ENDIF(WIN32)
IF(APPLE)
  ADD_LIBRARY( IDTFConverter ${IDTFConverter_SRCS} ${IDTFConverter_HDRS})
#  set_target_properties( IDTFConverter  PROPERTIES
#	LINK_FLAGS "${MY_LINK_FLAGS} -exported_symbols_list /dev/null" )
ENDIF(APPLE)
IF(UNIX AND NOT APPLE)
  ADD_LIBRARY( IDTFConverter ${IDTFConverter_SRCS} ${IDTFConverter_HDRS} )
  #set_target_properties( IFXCore PROPERTIES
	#LINK_FLAGS "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/RTL/Platform/Lin32/IFXCore/IFXCore.list -Wl,--no-undefined" )
  target_link_libraries( IDTFConverter ${CMAKE_DL_LIBS} )
ENDIF(UNIX AND NOT APPLE)
TARGET_LINK_LIBRARIES( IDTFConverter ${ADDITIONAL_LIBRARIES} )
install(
	TARGETS IDTFConverter
	RUNTIME DESTINATION ${BIN_DESTINATION}
	ARCHIVE DESTINATION ${LIB_DESTINATION}
	LIBRARY DESTINATION ${LIB_DESTINATION}
)

#INCLUDE_DIRECTORIES( RTL/Component/Include RTL/Kernel/Include RTL/Platform/Include
#	RTL/Dependencies/WildCards
#	RTL/Component/Base
#	IDTF
#	IDTF/Include
#	IDTF/Common
#        libIDTF )
#SET( libIDTF_SRCS
#	libIDTF/SceneConverterLib.cpp
#	libIDTF/Writer.cpp
#	libIDTF/IFXOSLoader.cpp
#	libIDTF/IFXOSUtilities.cpp
#	IDTF/PointSetConverter.cpp
#	IDTF/MetaDataConverter.cpp
#	IDTF/ResourceConverter.cpp
#	IDTF/TextureConverter.cpp
#	IDTF/File.cpp
#	IDTF/LineSetConverter.cpp
#	IDTF/ModelConverter.cpp
#	IDTF/NodeConverter.cpp
#	IDTF/MeshConverter.cpp
#	IDTF/FileReferenceConverter.cpp
#	IDTF/ModifierConverter.cpp
#	IDTF/Helpers/MiscUtilities.cpp
#	IDTF/Helpers/TGAImage.cpp
#	IDTF/Helpers/ModifierUtilities.cpp
#	IDTF/Helpers/SceneUtilities.cpp
#	IDTF/Helpers/DebugInfo.cpp
#	IDTF/Helpers/Guids.cpp
#	IDTF/Common/GlyphModifier.cpp
#	IDTF/Common/ModelResource.cpp
#	IDTF/Common/ModifierList.cpp
#	IDTF/Common/NodeList.cpp
#	IDTF/Common/FileReference.cpp
#	IDTF/Common/ResourceList.cpp
#	IDTF/Common/SceneResources.cpp
#	IDTF/Common/ModelResourceList.cpp
#	IDTF/Common/MetaDataList.cpp
#	IDTF/Common/ParentList.cpp
#	IDTF/Common/GlyphCommandList.cpp
#	RTL/Dependencies/WildCards/wcmatch.cpp
#	RTL/Component/Base/IFXCoincidentVertexMap.cpp
#	RTL/Component/Base/IFXFatCornerIter.cpp
#	RTL/Component/Base/IFXCornerIter.cpp
#	RTL/Component/Base/IFXTransform.cpp
#	RTL/Component/Base/IFXVertexMap.cpp
#	RTL/Component/Base/IFXEuler.cpp
#	RTL/Component/Base/IFXVectorHasher.cpp
#	RTL/Component/Base/IFXVertexMapGroup.cpp
#	RTL/Kernel/DataTypes/IFXCoreArray.cpp
#	RTL/Kernel/DataTypes/IFXFastAllocator.cpp
#	RTL/Kernel/DataTypes/IFXMatrix4x4.cpp
#	RTL/Kernel/DataTypes/IFXString.cpp
#	RTL/Kernel/DataTypes/IFXVector3.cpp
#	RTL/Kernel/DataTypes/IFXCoreList.cpp
#	RTL/Kernel/DataTypes/IFXListNode.cpp
#	RTL/Kernel/DataTypes/IFXQuaternion.cpp
#	RTL/Kernel/DataTypes/IFXUnitAllocator.cpp
#	RTL/Kernel/DataTypes/IFXVector4.cpp
#	RTL/Component/Common/IFXDids.cpp
#	RTL/Kernel/Common/IFXDebug.cpp
#	RTL/IFXCoreStatic/IFXCoreStatic.cpp
#)
#SET( libIDTF_HDRS
#	${Component_HDRS}
#	${Kernel_HDRS}
#	${Platform_HDRS}
#	libIDTF/SceneConverterLib.h
#	libIDTF/Writer.h
#	IDTF/DefaultSettings.h
#	IDTF/File.h
#	IDTF/FileReferenceConverter.h
#	IDTF/IConverter.h
#	IDTF/LineSetConverter.h
#	IDTF/MeshConverter.h
#	IDTF/MetaDataConverter.h
#	IDTF/ModelConverter.h
#	IDTF/ModifierConverter.h
#	IDTF/NodeConverter.h
#	IDTF/PointSetConverter.h
#	IDTF/ResourceConverter.h
#	IDTF/TextureConverter.h
#	IDTF/Include/ConverterOptions.h
#	IDTF/Include/ConverterResult.h
#	IDTF/Include/DebugInfo.h
#	IDTF/Include/SceneUtilities.h
#	IDTF/Include/TGAImage.h
#	IDTF/Include/U3DHeaders.h
#	IDTF/Common/AnimationModifier.h
#	IDTF/Common/BoneWeightModifier.h
#	IDTF/Common/CLODModifier.h
#	IDTF/Common/Color.h
#	IDTF/Common/FileReference.h
#	IDTF/Common/GlyphCommandList.h
#	IDTF/Common/GlyphCommands.h
#	IDTF/Common/GlyphModifier.h
#	IDTF/Common/INode.h
#	IDTF/Common/Int2.h
#	IDTF/Common/Int3.h
#	IDTF/Common/IResource.h
#	IDTF/Common/LightNode.h
#	IDTF/Common/LightResource.h
#	IDTF/Common/LightResourceList.h
#	IDTF/Common/LineSetResource.h
#	IDTF/Common/MaterialResource.h
#	IDTF/Common/MaterialResourceList.h
#	IDTF/Common/MeshResource.h
#	IDTF/Common/MetaDataList.h
#	IDTF/Common/ModelNode.h
#	IDTF/Common/ModelResource.h
#	IDTF/Common/ModelResourceList.h
#	IDTF/Common/ModelSkeleton.h
#	IDTF/Common/Modifier.h
#	IDTF/Common/ModifierList.h
#	IDTF/Common/MotionResource.h
#	IDTF/Common/MotionResourceList.h
#	IDTF/Common/Node.h
#	IDTF/Common/NodeList.h
#	IDTF/Common/ParentData.h
#	IDTF/Common/ParentList.h
#	IDTF/Common/Point.h
#	IDTF/Common/PointSetResource.h
#	IDTF/Common/Quat.h
#	IDTF/Common/Resource.h
#	IDTF/Common/ResourceList.h
#	IDTF/Common/SceneData.h
#	IDTF/Common/SceneResources.h
#	IDTF/Common/ShaderResource.h
#	IDTF/Common/ShaderResourceList.h
#	IDTF/Common/ShadingDescription.h
#	IDTF/Common/ShadingDescriptionList.h
#	IDTF/Common/ShadingModifier.h
#	IDTF/Common/SubdivisionModifier.h
#	IDTF/Common/TextureResource.h
#	IDTF/Common/TextureResourceList.h
#	IDTF/Common/Tokens.h
#	IDTF/Common/UrlList.h
#	IDTF/Common/ViewNodeData.h
#	IDTF/Common/ViewNode.h
#	IDTF/Common/ViewResource.h
#	IDTF/Common/ViewResourceList.h
#	RTL/Dependencies/WildCards/wcmatch.h
#	RTL/Component/Base/IFXVectorHasher.h
#)

#SET_PROPERTY( SOURCE
#	libIDTF/IFXOSLoader.cpp
#	PROPERTY COMPILE_DEFINITIONS U3DPluginsPath="Plugins" U3DCorePath="." )
#if(MSVC)
#ADD_LIBRARY( IDTF STATIC ${libIDTF_SRCS} ${libIDTF_HDRS} )
#else(MSVC)
#ADD_LIBRARY( IDTF SHARED ${libIDTF_SRCS} ${libIDTF_HDRS} )
#endif(MSVC)
#SET_PROPERTY(
#	TARGET IDTF
#	PROPERTY COMPILE_DEFINITIONS LIBIDTF )
#install(
#	TARGETS IDTF
#	ARCHIVE DESTINATION ${LIB_DESTINATION}
#	LIBRARY DESTINATION ${LIB_DESTINATION}
#)

ADD_EXECUTABLE( IDTFTest IDTFTest/main.cpp ${IDTFConverter_HDRS} )
target_link_libraries( IDTFTest IDTFConverter IFXCoreStatic ${CMAKE_DL_LIBS} )
add_dependencies( IDTFTest  IDTFConverter IFXCoreStatic )

install(
	TARGETS IDTFTest
	DESTINATION ${BIN_DESTINATION}
)


INSTALL( DIRECTORY RTL/Component/Include/ RTL/Kernel/Include/ RTL/Platform/Include/
	DESTINATION ${INCLUDE_DESTINATION}
	FILES_MATCHING PATTERN "*.h"
)
INSTALL( FILES RTL/Platform/${U3D_PLATFORM}/IFXRendering/OpenGL/IFXOpenGLOS.h
	DESTINATION ${INCLUDE_DESTINATION}
)
INSTALL( FILES ${libIDTF_HDRS}
	DESTINATION ${INCLUDE_DESTINATION}
)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
