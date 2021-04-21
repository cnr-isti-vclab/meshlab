set (U3D_DIR ${CMAKE_CURRENT_SOURCE_DIR})

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
ENDIF(WIN32)

IF(UNIX AND NOT APPLE)
	ADD_DEFINITIONS( -DLIN32 -fno-strict-aliasing )
	SET( U3D_PLATFORM Lin32 )
ENDIF(UNIX AND NOT APPLE)

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

SET(ZLIB_SOURCE_DIR ${U3D_DIR}/src/RTL/Dependencies/zlib)
#configure_file(${ZLIB_SOURCE_DIR}/zconf.h.cmakein
#			   ${CMAKE_CURRENT_BINARY_DIR}/zconf.h @ONLY)
#include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${ZLIB_SOURCE_DIR})

set(ZLIB_PUBLIC_HDRS
	${ZLIB_SOURCE_DIR}/zconf.h
	${ZLIB_SOURCE_DIR}/zlib.h
)
set(ZLIB_PRIVATE_HDRS
	${ZLIB_SOURCE_DIR}/zconf.h
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
		PATHS /usr/lib /usr/local/lib)
	if(NOT M_LIBRARY)
	message(STATUS
		"math library 'libm' not found - floating point support disabled")
	endif()
else()
	# not needed on windows
	set(M_LIBRARY "")
endif()

set(PNG_SOURCE_DIR ${U3D_DIR}/src/RTL/Dependencies/png)
#include_directories(${CMAKE_CURRENT_BINARY_DIR})

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

set(JPEG_SOURCE_DIR ${U3D_DIR}/src/RTL/Dependencies/jpeg)
#configure_file(${JPEG_SOURCE_DIR}/jconfig.h.cmake
#			   ${CMAKE_CURRENT_BINARY_DIR}/jconfig.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${JPEG_SOURCE_DIR})

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

set(JPEG_PUBLIC_HDRS
	${JPEG_SOURCE_DIR}/jconfig.h
	${JPEG_SOURCE_DIR}/jerror.h
	${JPEG_SOURCE_DIR}/jmorecfg.h
	${JPEG_SOURCE_DIR}/jpeglib.h
	${JPEG_SOURCE_DIR}/jconfig.h
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

SET(DEPENDENCIES_SRCS
	${ZLIB_SRCS}
	${ZLIB_PUBLIC_HDRS}
	${ZLIB_PRIVATE_HDRS}
	${libpng_sources}
	${JPEG_systemdependent_SRCS}
	${JPEG_SRCS}
	${JPEG_PUBLIC_HDRS}
	${JPEG_PRIVATE_HDRS})
#====


INCLUDE_DIRECTORIES(
	${U3D_DIR}/src/RTL/Component/Include
	${U3D_DIR}/src/RTL/Kernel/Include
	${U3D_DIR}/src/RTL/Platform/Include)
SET( Component_HDRS
	${U3D_DIR}/src/RTL/Component/Include/CArrayList.h
	${U3D_DIR}/src/RTL/Component/Include/DX7asDX8.h
	${U3D_DIR}/src/RTL/Component/Include/IFXACContext.h
	${U3D_DIR}/src/RTL/Component/Include/IFXAdaptiveMetric.h
	${U3D_DIR}/src/RTL/Component/Include/IFXAnimationModifier.h
	${U3D_DIR}/src/RTL/Component/Include/IFXArray.h
	${U3D_DIR}/src/RTL/Component/Include/IFXAttributes.h
	${U3D_DIR}/src/RTL/Component/Include/IFXAuthor.h
	${U3D_DIR}/src/RTL/Component/Include/IFXAuthorCLODAccess.h
	${U3D_DIR}/src/RTL/Component/Include/IFXAuthorCLODGen.h
	${U3D_DIR}/src/RTL/Component/Include/IFXAuthorCLODMesh.h
	${U3D_DIR}/src/RTL/Component/Include/IFXAuthorCLODResource.h
	${U3D_DIR}/src/RTL/Component/Include/IFXAuthorGeomCompiler.h
	${U3D_DIR}/src/RTL/Component/Include/IFXAuthorLineSet.h
	${U3D_DIR}/src/RTL/Component/Include/IFXAuthorLineSetAccess.h
	${U3D_DIR}/src/RTL/Component/Include/IFXAuthorLineSetAnalyzer.h
	${U3D_DIR}/src/RTL/Component/Include/IFXAuthorLineSetResource.h
	${U3D_DIR}/src/RTL/Component/Include/IFXAuthorMesh.h
	${U3D_DIR}/src/RTL/Component/Include/IFXAuthorMeshMap.h
	${U3D_DIR}/src/RTL/Component/Include/IFXAuthorMeshScrub.h
	${U3D_DIR}/src/RTL/Component/Include/IFXAuthorPointSet.h
	${U3D_DIR}/src/RTL/Component/Include/IFXAuthorPointSetAccess.h
	${U3D_DIR}/src/RTL/Component/Include/IFXAuthorPointSetResource.h
	${U3D_DIR}/src/RTL/Component/Include/IFXAuthorUpdate.h
	${U3D_DIR}/src/RTL/Component/Include/IFXAutoPtr.h
	${U3D_DIR}/src/RTL/Component/Include/IFXBaseOpenGL.h
	${U3D_DIR}/src/RTL/Component/Include/IFXBitStreamCompressedX.h
	${U3D_DIR}/src/RTL/Component/Include/IFXBitStreamX.h
	${U3D_DIR}/src/RTL/Component/Include/IFXBlendParam.h
	${U3D_DIR}/src/RTL/Component/Include/IFXBlockReaderX.h
	${U3D_DIR}/src/RTL/Component/Include/IFXBlockTypes.h
	${U3D_DIR}/src/RTL/Component/Include/IFXBlockWriterX.h
	${U3D_DIR}/src/RTL/Component/Include/IFXBones.h
	${U3D_DIR}/src/RTL/Component/Include/IFXBonesManager.h
	${U3D_DIR}/src/RTL/Component/Include/IFXBoneWeightsModifier.h
	${U3D_DIR}/src/RTL/Component/Include/IFXBoundHierarchy.h
	${U3D_DIR}/src/RTL/Component/Include/IFXBoundHierarchyMgr.h
	${U3D_DIR}/src/RTL/Component/Include/IFXBoundingBox.h
	${U3D_DIR}/src/RTL/Component/Include/IFXBoundSphereDataElement.h
	${U3D_DIR}/src/RTL/Component/Include/IFXBoundVolume.h
	${U3D_DIR}/src/RTL/Component/Include/IFXBTTHash.h
	${U3D_DIR}/src/RTL/Component/Include/IFXClock.h
	${U3D_DIR}/src/RTL/Component/Include/IFXCLODManagerInterface.h
	${U3D_DIR}/src/RTL/Component/Include/IFXCLODModifier.h
	${U3D_DIR}/src/RTL/Component/Include/IFXCoincidentVertexMap.h
	${U3D_DIR}/src/RTL/Component/Include/IFXCollection.h
	${U3D_DIR}/src/RTL/Component/Include/IFXCoreCIDs.h
	${U3D_DIR}/src/RTL/Component/Include/IFXCoreServices.h
	${U3D_DIR}/src/RTL/Component/Include/IFXCoreServicesRef.h
	${U3D_DIR}/src/RTL/Component/Include/IFXCornerIter.h
	${U3D_DIR}/src/RTL/Component/Include/IFXDataBlock.h
	${U3D_DIR}/src/RTL/Component/Include/IFXDataBlockQueueX.h
	${U3D_DIR}/src/RTL/Component/Include/IFXDataBlockX.h
	${U3D_DIR}/src/RTL/Component/Include/IFXDataPacket.h
	${U3D_DIR}/src/RTL/Component/Include/IFXDecoderChainX.h
	${U3D_DIR}/src/RTL/Component/Include/IFXDecoderX.h
	${U3D_DIR}/src/RTL/Component/Include/IFXDeque.h
	${U3D_DIR}/src/RTL/Component/Include/IFXDevice.h
	${U3D_DIR}/src/RTL/Component/Include/IFXDidRegistry.h
	${U3D_DIR}/src/RTL/Component/Include/IFXDids.h
	${U3D_DIR}/src/RTL/Component/Include/IFXDirectX7.h
	${U3D_DIR}/src/RTL/Component/Include/IFXDirectX8.h
	${U3D_DIR}/src/RTL/Component/Include/IFXDummyModifier.h
	${U3D_DIR}/src/RTL/Component/Include/IFXEncoderX.h
	${U3D_DIR}/src/RTL/Component/Include/IFXEnums.h
	${U3D_DIR}/src/RTL/Component/Include/IFXErrorInfo.h
	${U3D_DIR}/src/RTL/Component/Include/IFXEuler.h
	${U3D_DIR}/src/RTL/Component/Include/IFXExportingCIDs.h
	${U3D_DIR}/src/RTL/Component/Include/IFXExportingInterfaces.h
	${U3D_DIR}/src/RTL/Component/Include/IFXExportOptions.h
	${U3D_DIR}/src/RTL/Component/Include/IFXExtensionDecoderX.h
	${U3D_DIR}/src/RTL/Component/Include/IFXExtensionEncoderX.h
	${U3D_DIR}/src/RTL/Component/Include/IFXFace.h
	${U3D_DIR}/src/RTL/Component/Include/IFXFastAllocator.h
	${U3D_DIR}/src/RTL/Component/Include/IFXFastHeap.h
	${U3D_DIR}/src/RTL/Component/Include/IFXFatCornerIter.h
	${U3D_DIR}/src/RTL/Component/Include/IFXFileReference.h
	${U3D_DIR}/src/RTL/Component/Include/IFXFrustum.h
	${U3D_DIR}/src/RTL/Component/Include/IFXGenerator.h
	${U3D_DIR}/src/RTL/Component/Include/IFXGlyph2DCommands.h
	${U3D_DIR}/src/RTL/Component/Include/IFXGlyph2DModifier.h
	${U3D_DIR}/src/RTL/Component/Include/IFXGlyphCommandList.h
	${U3D_DIR}/src/RTL/Component/Include/IFXHash.h
	${U3D_DIR}/src/RTL/Component/Include/IFXHashMap.h
	${U3D_DIR}/src/RTL/Component/Include/IFXHistogramDynamic.h
	${U3D_DIR}/src/RTL/Component/Include/IFXIDManager.h
	${U3D_DIR}/src/RTL/Component/Include/IFXImageCodec.h
	${U3D_DIR}/src/RTL/Component/Include/IFXImportingCIDs.h
	${U3D_DIR}/src/RTL/Component/Include/IFXImportingInterfaces.h
	${U3D_DIR}/src/RTL/Component/Include/IFXInet.h
	${U3D_DIR}/src/RTL/Component/Include/IFXInstant.h
	${U3D_DIR}/src/RTL/Component/Include/IFXInterleavedData.h
	${U3D_DIR}/src/RTL/Component/Include/IFXIterators.h
	${U3D_DIR}/src/RTL/Component/Include/IFXKeyFrame.h
	${U3D_DIR}/src/RTL/Component/Include/IFXKeyTrack.h
	${U3D_DIR}/src/RTL/Component/Include/IFXKeyTrackArray.h
	${U3D_DIR}/src/RTL/Component/Include/IFXLight.h
	${U3D_DIR}/src/RTL/Component/Include/IFXLightResource.h
	${U3D_DIR}/src/RTL/Component/Include/IFXLightSet.h
	${U3D_DIR}/src/RTL/Component/Include/IFXLine.h
	${U3D_DIR}/src/RTL/Component/Include/IFXList.h
	${U3D_DIR}/src/RTL/Component/Include/IFXListContext.h
	${U3D_DIR}/src/RTL/Component/Include/IFXListNode.h
	${U3D_DIR}/src/RTL/Component/Include/IFXLoadManager.h
	${U3D_DIR}/src/RTL/Component/Include/IFXMarker.h
	${U3D_DIR}/src/RTL/Component/Include/IFXMarkerX.h
	${U3D_DIR}/src/RTL/Component/Include/IFXMaterialResource.h
	${U3D_DIR}/src/RTL/Component/Include/IFXMesh.h
	${U3D_DIR}/src/RTL/Component/Include/IFXMeshCompiler.h
	${U3D_DIR}/src/RTL/Component/Include/IFXMeshGroup.h
	${U3D_DIR}/src/RTL/Component/Include/IFXMeshMap.h
	${U3D_DIR}/src/RTL/Component/Include/IFXMetaDataX.h
	${U3D_DIR}/src/RTL/Component/Include/IFXMixerConstruct.h
	${U3D_DIR}/src/RTL/Component/Include/IFXMixerQueue.h
	${U3D_DIR}/src/RTL/Component/Include/IFXModel.h
	${U3D_DIR}/src/RTL/Component/Include/IFXModifier.h
	${U3D_DIR}/src/RTL/Component/Include/IFXModifierBaseDecoder.h
	${U3D_DIR}/src/RTL/Component/Include/IFXModifierChain.h
	${U3D_DIR}/src/RTL/Component/Include/IFXModifierDataElementIter.h
	${U3D_DIR}/src/RTL/Component/Include/IFXModifierDataPacket.h
	${U3D_DIR}/src/RTL/Component/Include/IFXMotion.h
	${U3D_DIR}/src/RTL/Component/Include/IFXMotionManager.h
	${U3D_DIR}/src/RTL/Component/Include/IFXMotionMixer.h
	${U3D_DIR}/src/RTL/Component/Include/IFXMotionResource.h
	${U3D_DIR}/src/RTL/Component/Include/IFXNameMap.h
	${U3D_DIR}/src/RTL/Component/Include/IFXNeighborFace.h
	${U3D_DIR}/src/RTL/Component/Include/IFXNeighborMesh.h
	${U3D_DIR}/src/RTL/Component/Include/IFXNeighborResControllerIntfc.h
	${U3D_DIR}/src/RTL/Component/Include/IFXNode.h
	${U3D_DIR}/src/RTL/Component/Include/IFXNotificationInfo.h
	${U3D_DIR}/src/RTL/Component/Include/IFXNotificationManager.h
	${U3D_DIR}/src/RTL/Component/Include/IFXObserver.h
	${U3D_DIR}/src/RTL/Component/Include/IFXOpenGL.h
	${U3D_DIR}/src/RTL/Component/Include/IFXPalette.h
	${U3D_DIR}/src/RTL/Component/Include/IFXPickObject.h
	${U3D_DIR}/src/RTL/Component/Include/IFXProgressCallback.h
	${U3D_DIR}/src/RTL/Component/Include/IFXRay.h
	${U3D_DIR}/src/RTL/Component/Include/IFXReadBuffer.h
	${U3D_DIR}/src/RTL/Component/Include/IFXReadBufferX.h
	${U3D_DIR}/src/RTL/Component/Include/IFXRender.h
	${U3D_DIR}/src/RTL/Component/Include/IFXRenderable.h
	${U3D_DIR}/src/RTL/Component/Include/IFXRenderBlend.h
	${U3D_DIR}/src/RTL/Component/Include/IFXRenderCaps.h
	${U3D_DIR}/src/RTL/Component/Include/IFXRenderClear.h
	${U3D_DIR}/src/RTL/Component/Include/IFXRenderContext.h
	${U3D_DIR}/src/RTL/Component/Include/IFXRenderDevice.h
	${U3D_DIR}/src/RTL/Component/Include/IFXRenderFog.h
	${U3D_DIR}/src/RTL/Component/Include/IFXRenderHelpers.h
	${U3D_DIR}/src/RTL/Component/Include/IFXRenderingCIDs.h
	${U3D_DIR}/src/RTL/Component/Include/IFXRenderingInterfaces.h
	${U3D_DIR}/src/RTL/Component/Include/IFXRenderLight.h
	${U3D_DIR}/src/RTL/Component/Include/IFXRenderMaterial.h
	${U3D_DIR}/src/RTL/Component/Include/IFXRenderPass.h
	${U3D_DIR}/src/RTL/Component/Include/IFXRenderServices.h
	${U3D_DIR}/src/RTL/Component/Include/IFXRenderStencil.h
	${U3D_DIR}/src/RTL/Component/Include/IFXRenderTexUnit.h
	${U3D_DIR}/src/RTL/Component/Include/IFXRenderView.h
	${U3D_DIR}/src/RTL/Component/Include/IFXResourceClient.h
	${U3D_DIR}/src/RTL/Component/Include/IFXSceneGraph.h
	${U3D_DIR}/src/RTL/Component/Include/IFXScheduler.h
	${U3D_DIR}/src/RTL/Component/Include/IFXSchedulerInfo.h
	${U3D_DIR}/src/RTL/Component/Include/IFXSchedulerTypes.h
	${U3D_DIR}/src/RTL/Component/Include/IFXSchedulingCIDs.h
	${U3D_DIR}/src/RTL/Component/Include/IFXSchedulingInterfaces.h
	${U3D_DIR}/src/RTL/Component/Include/IFXScreenSpaceMetricInterface.h
	${U3D_DIR}/src/RTL/Component/Include/IFXSetAdjacencyX.h
	${U3D_DIR}/src/RTL/Component/Include/IFXSetX.h
	${U3D_DIR}/src/RTL/Component/Include/IFXShader.h
	${U3D_DIR}/src/RTL/Component/Include/IFXShaderList.h
	${U3D_DIR}/src/RTL/Component/Include/IFXShaderLitTexture.h
	${U3D_DIR}/src/RTL/Component/Include/IFXShadingModifier.h
	${U3D_DIR}/src/RTL/Component/Include/IFXSimpleHash.h
	${U3D_DIR}/src/RTL/Component/Include/IFXSimpleList.h
	${U3D_DIR}/src/RTL/Component/Include/IFXSimulationInfo.h
	${U3D_DIR}/src/RTL/Component/Include/IFXSimulationManager.h
	${U3D_DIR}/src/RTL/Component/Include/IFXSimulationTask.h
	${U3D_DIR}/src/RTL/Component/Include/IFXSkeleton.h
	${U3D_DIR}/src/RTL/Component/Include/IFXSkeletonDataElement.h
	${U3D_DIR}/src/RTL/Component/Include/IFXSkeletonMixer.h
	${U3D_DIR}/src/RTL/Component/Include/IFXSmartPtr.h
	${U3D_DIR}/src/RTL/Component/Include/IFXSpatial.h
	${U3D_DIR}/src/RTL/Component/Include/IFXSpatialAssociation.h
	${U3D_DIR}/src/RTL/Component/Include/IFXSpatialSetQuery.h
	${U3D_DIR}/src/RTL/Component/Include/IFXStdio.h
	${U3D_DIR}/src/RTL/Component/Include/IFXSubdivManagerInterface.h
	${U3D_DIR}/src/RTL/Component/Include/IFXSubdivModifier.h
	${U3D_DIR}/src/RTL/Component/Include/IFXSubject.h
	${U3D_DIR}/src/RTL/Component/Include/IFXSystemManager.h
	${U3D_DIR}/src/RTL/Component/Include/IFXTask.h
	${U3D_DIR}/src/RTL/Component/Include/IFXTaskCallback.h
	${U3D_DIR}/src/RTL/Component/Include/IFXTaskData.h
	${U3D_DIR}/src/RTL/Component/Include/IFXTaskManager.h
	${U3D_DIR}/src/RTL/Component/Include/IFXTaskManagerNode.h
	${U3D_DIR}/src/RTL/Component/Include/IFXTaskManagerView.h
	${U3D_DIR}/src/RTL/Component/Include/IFXTextureImageTools.h
	${U3D_DIR}/src/RTL/Component/Include/IFXTextureObject.h
	${U3D_DIR}/src/RTL/Component/Include/IFXTimeManager.h
	${U3D_DIR}/src/RTL/Component/Include/IFXTransform.h
	${U3D_DIR}/src/RTL/Component/Include/IFXTransformStack.h
	${U3D_DIR}/src/RTL/Component/Include/IFXUnitAllocator.h
	${U3D_DIR}/src/RTL/Component/Include/IFXUpdates.h
	${U3D_DIR}/src/RTL/Component/Include/IFXUpdatesGroup.h
	${U3D_DIR}/src/RTL/Component/Include/IFXUVGenerator.h
	${U3D_DIR}/src/RTL/Component/Include/IFXUVMapper.h
	${U3D_DIR}/src/RTL/Component/Include/IFXVertexAttributes.h
	${U3D_DIR}/src/RTL/Component/Include/IFXVertexIndexer.h
	${U3D_DIR}/src/RTL/Component/Include/IFXVertexMap.h
	${U3D_DIR}/src/RTL/Component/Include/IFXVertexMapGroup.h
	${U3D_DIR}/src/RTL/Component/Include/IFXView.h
	${U3D_DIR}/src/RTL/Component/Include/IFXViewResource.h
	${U3D_DIR}/src/RTL/Component/Include/IFXVoidStar.h
	${U3D_DIR}/src/RTL/Component/Include/IFXVoidWrapper.h
	${U3D_DIR}/src/RTL/Component/Include/IFXWriteBuffer.h
	${U3D_DIR}/src/RTL/Component/Include/IFXWriteBufferX.h
	${U3D_DIR}/src/RTL/Component/Include/IFXWriteManager.h
	${U3D_DIR}/src/RTL/Component/Include/InsertionSort.h
)
SET( Kernel_HDRS
	${U3D_DIR}/src/RTL/Kernel/Include/IFXAutoRelease.h
	${U3D_DIR}/src/RTL/Kernel/Include/IFXCheckX.h
	${U3D_DIR}/src/RTL/Kernel/Include/IFXCOM.h
	${U3D_DIR}/src/RTL/Kernel/Include/IFXConnection.h
	${U3D_DIR}/src/RTL/Kernel/Include/IFXConnectionServer.h
	${U3D_DIR}/src/RTL/Kernel/Include/IFXDataTypes.h
	${U3D_DIR}/src/RTL/Kernel/Include/IFXDebug.h
	${U3D_DIR}/src/RTL/Kernel/Include/IFXException.h
	${U3D_DIR}/src/RTL/Kernel/Include/IFXGUID.h
	${U3D_DIR}/src/RTL/Kernel/Include/IFXIPP.h
	${U3D_DIR}/src/RTL/Kernel/Include/IFXMatrix4x4.h
	${U3D_DIR}/src/RTL/Kernel/Include/IFXMemory.h
	${U3D_DIR}/src/RTL/Kernel/Include/IFXPerformanceTimer.h
	${U3D_DIR}/src/RTL/Kernel/Include/IFXPlugin.h
	${U3D_DIR}/src/RTL/Kernel/Include/IFXQuaternion.h
	${U3D_DIR}/src/RTL/Kernel/Include/IFXResult.h
	${U3D_DIR}/src/RTL/Kernel/Include/IFXResultComponentEngine.h
	${U3D_DIR}/src/RTL/Kernel/Include/IFXString.h
	${U3D_DIR}/src/RTL/Kernel/Include/IFXUnknown.h
	${U3D_DIR}/src/RTL/Kernel/Include/IFXVector2.h
	${U3D_DIR}/src/RTL/Kernel/Include/IFXVector3.h
	${U3D_DIR}/src/RTL/Kernel/Include/IFXVector4.h
	${U3D_DIR}/src/RTL/Kernel/Include/IFXVersion.h
)
SET( Platform_HDRS
	${U3D_DIR}/src/RTL/Platform/Include/IFXAPI.h
	${U3D_DIR}/src/RTL/Platform/Include/IFXOSFileIterator.h
	${U3D_DIR}/src/RTL/Platform/Include/IFXOSLoader.h
	${U3D_DIR}/src/RTL/Platform/Include/IFXOSRender.h
	${U3D_DIR}/src/RTL/Platform/Include/IFXOSSocket.h
	${U3D_DIR}/src/RTL/Platform/Include/IFXOSUtilities.h
	${U3D_DIR}/src/RTL/Platform/Include/IFXRenderWindow.h
)

# IFXCoreStatic
INCLUDE_DIRECTORIES(
	${U3D_DIR}/src/RTL/Component/Include
	${U3D_DIR}/src/RTL/Kernel/Include
	${U3D_DIR}/src/RTL/Platform/Include
	${U3D_DIR}/src/RTL/Component/Base
	${U3D_DIR}/src/RTL/Component/Rendering
	${U3D_DIR}/src/RTL/Dependencies/WildCards )

SET( IFXCoreStatic_HDRS
	${Component_HDRS}
	${Kernel_HDRS}
	${Platform_HDRS}
	${U3D_DIR}/src/RTL/Component/Base/IFXVectorHasher.h
	${U3D_DIR}/src/RTL/Component/Rendering/CIFXDeviceBase.h
	${U3D_DIR}/src/RTL/Component/Rendering/CIFXDeviceLight.h
	${U3D_DIR}/src/RTL/Component/Rendering/CIFXDeviceTexture.h
	${U3D_DIR}/src/RTL/Component/Rendering/CIFXDeviceTexUnit.h
	${U3D_DIR}/src/RTL/Component/Rendering/CIFXRender.h
	${U3D_DIR}/src/RTL/Component/Rendering/CIFXRenderContext.h
	${U3D_DIR}/src/RTL/Component/Rendering/CIFXRenderDevice.h
	${U3D_DIR}/src/RTL/Component/Rendering/CIFXRenderServices.h
	${U3D_DIR}/src/RTL/Component/Rendering/DX7/CIFXDeviceLightDX7.h
	${U3D_DIR}/src/RTL/Component/Rendering/DX7/CIFXDeviceTextureDX7.h
	${U3D_DIR}/src/RTL/Component/Rendering/DX7/CIFXDeviceTexUnitDX7.h
	${U3D_DIR}/src/RTL/Component/Rendering/DX7/CIFXDirectX7.h
	${U3D_DIR}/src/RTL/Component/Rendering/DX7/CIFXRenderDeviceDX7.h
	${U3D_DIR}/src/RTL/Component/Rendering/DX7/CIFXRenderDX7.h
	${U3D_DIR}/src/RTL/Component/Rendering/DX7/IFXRenderPCHDX7.h
	${U3D_DIR}/src/RTL/Component/Rendering/DX8/CIFXDeviceLightDX8.h
	${U3D_DIR}/src/RTL/Component/Rendering/DX8/CIFXDeviceTextureDX8.h
	${U3D_DIR}/src/RTL/Component/Rendering/DX8/CIFXDeviceTexUnitDX8.h
	${U3D_DIR}/src/RTL/Component/Rendering/DX8/CIFXDirectX8.h
	${U3D_DIR}/src/RTL/Component/Rendering/DX8/CIFXRenderDeviceDX8.h
	${U3D_DIR}/src/RTL/Component/Rendering/DX8/CIFXRenderDX8.h
	${U3D_DIR}/src/RTL/Component/Rendering/DX8/IFXRenderPCHDX8.h
	${U3D_DIR}/src/RTL/Component/Rendering/IFXAAFilter.h
	${U3D_DIR}/src/RTL/Component/Rendering/IFXRenderPCH.h
	${U3D_DIR}/src/RTL/Component/Rendering/Null/CIFXDeviceLightNULL.h
	${U3D_DIR}/src/RTL/Component/Rendering/Null/CIFXDeviceTextureNULL.h
	${U3D_DIR}/src/RTL/Component/Rendering/Null/CIFXDeviceTexUnitNULL.h
	${U3D_DIR}/src/RTL/Component/Rendering/Null/CIFXRenderDeviceNULL.h
	${U3D_DIR}/src/RTL/Component/Rendering/Null/CIFXRenderNULL.h
	${U3D_DIR}/src/RTL/Component/Rendering/Null/IFXRenderPCHNULL.h
	${U3D_DIR}/src/RTL/Component/Rendering/OpenGL/CIFXDeviceLightOGL.h
	${U3D_DIR}/src/RTL/Component/Rendering/OpenGL/CIFXDeviceTextureOGL.h
	${U3D_DIR}/src/RTL/Component/Rendering/OpenGL/CIFXDeviceTexUnitOGL.h
	${U3D_DIR}/src/RTL/Component/Rendering/OpenGL/CIFXOpenGL.h
	${U3D_DIR}/src/RTL/Component/Rendering/OpenGL/CIFXRenderDeviceOGL.h
	${U3D_DIR}/src/RTL/Component/Rendering/OpenGL/CIFXRenderOGL.h
	${U3D_DIR}/src/RTL/Component/Rendering/OpenGL/IFXRenderPCHOGL.h
	${U3D_DIR}/src/RTL/Dependencies/WildCards/wcmatch.h
)
SET( IFXCoreStatic_SRCS
	${U3D_DIR}/src/RTL/IFXCoreStatic/IFXCoreStatic.cpp
	${U3D_DIR}/src/RTL/Platform/${U3D_PLATFORM}/Common/IFXOSUtilities.cpp
	${U3D_DIR}/src/RTL/Platform/${U3D_PLATFORM}/Common/IFXOSLoader.cpp
	${U3D_DIR}/src/RTL/Platform/${U3D_PLATFORM}/Common/IFXOSRenderWindow.cpp
	${U3D_DIR}/src/RTL/Component/Common/IFXDids.cpp
	${U3D_DIR}/src/RTL/Component/Base/IFXCoincidentVertexMap.cpp
	${U3D_DIR}/src/RTL/Component/Base/IFXCornerIter.cpp
	${U3D_DIR}/src/RTL/Component/Base/IFXEuler.cpp
	${U3D_DIR}/src/RTL/Component/Base/IFXFatCornerIter.cpp
	${U3D_DIR}/src/RTL/Component/Base/IFXTransform.cpp
	${U3D_DIR}/src/RTL/Component/Base/IFXVectorHasher.cpp
	${U3D_DIR}/src/RTL/Component/Base/IFXVertexMap.cpp
	${U3D_DIR}/src/RTL/Component/Base/IFXVertexMapGroup.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXCoreArray.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXCoreList.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXFastAllocator.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXListNode.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXMatrix4x4.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXQuaternion.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXString.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXUnitAllocator.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXVector3.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXVector4.cpp
	${U3D_DIR}/src/RTL/Dependencies/WildCards/wcmatch.cpp
	${U3D_DIR}/src/RTL/Kernel/Common/IFXDebug.cpp
)
ADD_LIBRARY( IFXCoreStatic STATIC ${IFXCoreStatic_SRCS} ${IFXCoreStatic_HDRS} )
IF(UNIX)
	target_link_libraries( IFXCoreStatic ${CMAKE_DL_LIBS} )
ENDIF(UNIX)
target_compile_options(IFXCoreStatic PRIVATE -w)

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
INCLUDE_DIRECTORIES(
	${U3D_DIR}/src/RTL/Component/Include
	${U3D_DIR}/src/RTL/Kernel/Include
	${U3D_DIR}/src/RTL/Platform/Include
	${U3D_DIR}/src/RTL/Component/Base
	${U3D_DIR}/src/RTL/Component/BitStream
	${U3D_DIR}/src/RTL/Component/Bones
	${U3D_DIR}/src/RTL/Component/BoundHierarchy
	${U3D_DIR}/src/RTL/Component/CLODAuthor
	${U3D_DIR}/src/RTL/Component/Common
	${U3D_DIR}/src/RTL/Component/Generators/CLOD
	${U3D_DIR}/src/RTL/Component/Generators/Glyph2D
	${U3D_DIR}/src/RTL/Component/Generators/LineSet
	${U3D_DIR}/src/RTL/Component/Generators/PointSet
	${U3D_DIR}/src/RTL/Component/Mesh
	${U3D_DIR}/src/RTL/Component/ModifierChain
	${U3D_DIR}/src/RTL/Component/Palette
	${U3D_DIR}/src/RTL/Component/Rendering
	${U3D_DIR}/src/RTL/Component/SceneGraph
	${U3D_DIR}/src/RTL/Component/Shaders
	${U3D_DIR}/src/RTL/Component/Subdiv
	${U3D_DIR}/src/RTL/Component/Texture
	${U3D_DIR}/src/RTL/Component/UVGenerator
	${U3D_DIR}/src/RTL/Kernel/IFXCom
	${U3D_DIR}/src/RTL/Kernel/Common
	${U3D_DIR}/src/RTL/Dependencies/FNVHash
	${U3D_DIR}/src/RTL/Dependencies/Predicates
	${U3D_DIR}/src/RTL/Dependencies/WildCards
)

SET( IFXCore_SRCS
	${U3D_DIR}/src/RTL/Platform/${U3D_PLATFORM}/IFXCore/IFXCoreDllMain.cpp
	${U3D_DIR}/src/RTL/IFXCorePluginStatic/IFXCorePluginStatic.cpp
	${U3D_DIR}/src/RTL/Platform/${U3D_PLATFORM}/Common/IFXOSLoader.cpp
	${U3D_DIR}/src/RTL/Platform/${U3D_PLATFORM}/Common/IFXOSRenderWindow.cpp
	${U3D_DIR}/src/RTL/Platform/${U3D_PLATFORM}/Common/IFXOSUtilities.cpp
	${U3D_DIR}/src/RTL/Component/Base/IFXCoincidentVertexMap.cpp
	${U3D_DIR}/src/RTL/Component/Base/IFXCornerIter.cpp
	${U3D_DIR}/src/RTL/Component/Base/IFXEuler.cpp
	${U3D_DIR}/src/RTL/Component/Base/IFXFatCornerIter.cpp
	${U3D_DIR}/src/RTL/Component/Base/IFXTransform.cpp
	${U3D_DIR}/src/RTL/Component/Base/IFXVectorHasher.cpp
	${U3D_DIR}/src/RTL/Component/Base/IFXVertexMap.cpp
	${U3D_DIR}/src/RTL/Component/Base/IFXVertexMapGroup.cpp
	${U3D_DIR}/src/RTL/Component/BitStream/CIFXBitStreamX.cpp
	${U3D_DIR}/src/RTL/Component/BitStream/CIFXDataBlockQueueX.cpp
	${U3D_DIR}/src/RTL/Component/BitStream/CIFXDataBlockX.cpp
	${U3D_DIR}/src/RTL/Component/BitStream/IFXHistogramDynamic.cpp
	${U3D_DIR}/src/RTL/Component/Bones/CIFXAnimationModifier.cpp
	${U3D_DIR}/src/RTL/Component/Bones/CIFXBoneWeightsModifier.cpp
	${U3D_DIR}/src/RTL/Component/Bones/CIFXSkeleton.cpp
	${U3D_DIR}/src/RTL/Component/Bones/IFXBoneCacheArray.cpp
	${U3D_DIR}/src/RTL/Component/Bones/IFXBoneNode.cpp
	${U3D_DIR}/src/RTL/Component/Bones/IFXBonesManagerImpl.cpp
	${U3D_DIR}/src/RTL/Component/Bones/IFXCharacter.cpp
	${U3D_DIR}/src/RTL/Component/Bones/IFXCoreNode.cpp
	${U3D_DIR}/src/RTL/Component/Bones/IFXKeyTrack.cpp
	${U3D_DIR}/src/RTL/Component/Bones/IFXMeshGroup_Character.cpp
	${U3D_DIR}/src/RTL/Component/Bones/IFXMixerQueue.cpp
	${U3D_DIR}/src/RTL/Component/Bones/IFXMixerQueueImpl.cpp
	${U3D_DIR}/src/RTL/Component/Bones/IFXMotion.cpp
	${U3D_DIR}/src/RTL/Component/Bones/IFXMotionManagerImpl.cpp
	${U3D_DIR}/src/RTL/Component/Bones/IFXMotionMixerImpl.cpp
	${U3D_DIR}/src/RTL/Component/Bones/IFXSkin.cpp
	${U3D_DIR}/src/RTL/Component/Bones/IFXSkin_p3.cpp
	${U3D_DIR}/src/RTL/Component/Bones/IFXVertexWeight.cpp
	${U3D_DIR}/src/RTL/Component/Bones/IFXVertexWeights.cpp
	${U3D_DIR}/src/RTL/Component/BoundHierarchy/CIFXAABBHierarchyBuilder.cpp
	${U3D_DIR}/src/RTL/Component/BoundHierarchy/CIFXAxisAlignedBBox.cpp
	${U3D_DIR}/src/RTL/Component/BoundHierarchy/CIFXBoundFace.cpp
	${U3D_DIR}/src/RTL/Component/BoundHierarchy/CIFXBoundHierarchy.cpp
	${U3D_DIR}/src/RTL/Component/BoundHierarchy/CIFXBoundUtil.cpp
	${U3D_DIR}/src/RTL/Component/BoundHierarchy/CIFXBTree.cpp
	${U3D_DIR}/src/RTL/Component/BoundHierarchy/CIFXBTreeNode.cpp
	${U3D_DIR}/src/RTL/Component/BoundHierarchy/CIFXCollisionResult.cpp
	${U3D_DIR}/src/RTL/Component/BoundHierarchy/CIFXPickObject.cpp
	${U3D_DIR}/src/RTL/Component/BoundHierarchy/CIFXPrimitiveOverlap.cpp
	${U3D_DIR}/src/RTL/Component/BoundHierarchy/CIFXResultAllocator.cpp
	${U3D_DIR}/src/RTL/Component/CLODAuthor/CIFXAuthorCLODGen.cpp
	${U3D_DIR}/src/RTL/Component/CLODAuthor/CIFXAuthorMeshMap.cpp
	${U3D_DIR}/src/RTL/Component/CLODAuthor/CIFXSetAdjacencyX.cpp
	${U3D_DIR}/src/RTL/Component/CLODAuthor/CIFXSetX.cpp
	${U3D_DIR}/src/RTL/Component/CLODAuthor/CLODGenerator.cpp
	${U3D_DIR}/src/RTL/Component/CLODAuthor/ContractionRecorder.cpp
	${U3D_DIR}/src/RTL/Component/CLODAuthor/Face.cpp
	${U3D_DIR}/src/RTL/Component/CLODAuthor/Matrix4x4.cpp
	${U3D_DIR}/src/RTL/Component/CLODAuthor/NormalMap.cpp
	${U3D_DIR}/src/RTL/Component/CLODAuthor/Pair.cpp
	${U3D_DIR}/src/RTL/Component/CLODAuthor/PairFinder.cpp
	${U3D_DIR}/src/RTL/Component/CLODAuthor/PairHash.cpp
	${U3D_DIR}/src/RTL/Component/CLODAuthor/PairHeap.cpp
	${U3D_DIR}/src/RTL/Component/CLODAuthor/Primitives.cpp
	${U3D_DIR}/src/RTL/Component/CLODAuthor/Vertex.cpp
	${U3D_DIR}/src/RTL/Component/CLODAuthor/VertexPairContractor.cpp
	${U3D_DIR}/src/RTL/Component/Common/CIFXCoreServices.cpp
	${U3D_DIR}/src/RTL/Component/Common/CIFXCoreServicesRef.cpp
	${U3D_DIR}/src/RTL/Component/Common/CIFXHashMap.cpp
	${U3D_DIR}/src/RTL/Component/Common/CIFXIDManager.cpp
	${U3D_DIR}/src/RTL/Component/Common/CIFXMetaData.cpp
	${U3D_DIR}/src/RTL/Component/Common/CIFXNameMap.cpp
	${U3D_DIR}/src/RTL/Component/Common/CIFXSimpleHash.cpp
	${U3D_DIR}/src/RTL/Component/Common/CIFXVoidWrapper.cpp
	${U3D_DIR}/src/RTL/Component/Common/IFXComponentDescriptorList.cpp
	${U3D_DIR}/src/RTL/Component/Common/IFXComponentIds.cpp
	${U3D_DIR}/src/RTL/Component/Common/IFXDids.cpp
	${U3D_DIR}/src/RTL/Component/Generators/CLOD/CIFXAuthorCLODResource.cpp
	${U3D_DIR}/src/RTL/Component/Generators/CLOD/CIFXAuthorMesh.cpp
	${U3D_DIR}/src/RTL/Component/Generators/CLOD/CIFXAuthorMeshScrub.cpp
	${U3D_DIR}/src/RTL/Component/Generators/CLOD/CIFXCLODModifier.cpp
	${U3D_DIR}/src/RTL/Component/Generators/CLOD/CIFXMeshCompiler.cpp
	${U3D_DIR}/src/RTL/Component/Generators/CLOD/CIFXMeshMap.cpp
	${U3D_DIR}/src/RTL/Component/Generators/CLOD/IFXCLODManager.cpp
	${U3D_DIR}/src/RTL/Component/Generators/CLOD/IFXNeighborResController.cpp
	${U3D_DIR}/src/RTL/Component/Generators/Glyph2D/CIFXContour.cpp
	${U3D_DIR}/src/RTL/Component/Generators/Glyph2D/CIFXContourExtruder.cpp
	${U3D_DIR}/src/RTL/Component/Generators/Glyph2D/CIFXContourGenerator.cpp
	${U3D_DIR}/src/RTL/Component/Generators/Glyph2D/CIFXContourTessellator.cpp
	${U3D_DIR}/src/RTL/Component/Generators/Glyph2D/CIFXGlyph2DCommands.cpp
	${U3D_DIR}/src/RTL/Component/Generators/Glyph2D/CIFXGlyph2DModifier.cpp
	${U3D_DIR}/src/RTL/Component/Generators/Glyph2D/CIFXGlyph3DGenerator.cpp
	${U3D_DIR}/src/RTL/Component/Generators/Glyph2D/CIFXGlyphCommandList.cpp
	${U3D_DIR}/src/RTL/Component/Generators/Glyph2D/CIFXQuadEdge.cpp
	${U3D_DIR}/src/RTL/Component/Generators/LineSet/CIFXAuthorLineSetAnalyzer.cpp
	${U3D_DIR}/src/RTL/Component/Generators/LineSet/CIFXAuthorLineSet.cpp
	${U3D_DIR}/src/RTL/Component/Generators/LineSet/CIFXAuthorLineSetResource.cpp
	${U3D_DIR}/src/RTL/Component/Generators/PointSet/CIFXAuthorPointSet.cpp
	${U3D_DIR}/src/RTL/Component/Generators/PointSet/CIFXAuthorPointSetResource.cpp
	${U3D_DIR}/src/RTL/Component/Mesh/CIFXInterleavedData.cpp
	${U3D_DIR}/src/RTL/Component/Mesh/CIFXMesh.cpp
	${U3D_DIR}/src/RTL/Component/Mesh/CIFXMeshGroup.cpp
	${U3D_DIR}/src/RTL/Component/Mesh/CIFXNeighborMesh.cpp
	${U3D_DIR}/src/RTL/Component/Mesh/CIFXRenderable.cpp
	${U3D_DIR}/src/RTL/Component/Mesh/IFXFaceLists.cpp
	${U3D_DIR}/src/RTL/Component/ModifierChain/CIFXDidRegistry.cpp
	${U3D_DIR}/src/RTL/Component/ModifierChain/CIFXModifierChain.cpp
	${U3D_DIR}/src/RTL/Component/ModifierChain/CIFXModifier.cpp
	${U3D_DIR}/src/RTL/Component/ModifierChain/CIFXModifierDataElementIter.cpp
	${U3D_DIR}/src/RTL/Component/ModifierChain/CIFXModifierDataPacket.cpp
	${U3D_DIR}/src/RTL/Component/ModifierChain/CIFXSubject.cpp
	${U3D_DIR}/src/RTL/Component/ModifierChain/IFXModifierChainState.cpp
	${U3D_DIR}/src/RTL/Component/Palette/CIFXPalette.cpp
	${U3D_DIR}/src/RTL/Component/Palette/CIFXSimpleObject.cpp
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXBoundSphereDataElement.cpp
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXDevice.cpp
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXDummyModifier.cpp
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXFileReference.cpp
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXGroup.cpp
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXLight.cpp
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXLightResource.cpp
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXLightSet.cpp
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXMarker.cpp
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXMaterialResource.cpp
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXMixerConstruct.cpp
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXModel.cpp
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXMotionResource.cpp
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXNode.cpp
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXResourceClient.cpp
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXSceneGraph.cpp
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXShaderList.cpp
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXSimpleCollection.cpp
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXSimpleList.cpp
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXView.cpp
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXViewResource.cpp
	${U3D_DIR}/src/RTL/Component/SceneGraph/IFXRenderPass.cpp
	${U3D_DIR}/src/RTL/Component/Shaders/CIFXShader.cpp
	${U3D_DIR}/src/RTL/Component/Shaders/CIFXShaderLitTexture.cpp
	${U3D_DIR}/src/RTL/Component/Shaders/CIFXShadingModifier.cpp
	${U3D_DIR}/src/RTL/Component/Subdiv/CIFXSubdivModifier.cpp
	${U3D_DIR}/src/RTL/Component/Subdiv/IFXAttributeNeighborhood.cpp
	${U3D_DIR}/src/RTL/Component/Subdiv/IFXButterflyScheme.cpp
	${U3D_DIR}/src/RTL/Component/Subdiv/IFXScreenSpaceMetric.cpp
	${U3D_DIR}/src/RTL/Component/Subdiv/IFXSharedUnitAllocator.cpp
	${U3D_DIR}/src/RTL/Component/Subdiv/IFXSubdivisionManager.cpp
	${U3D_DIR}/src/RTL/Component/Subdiv/IFXTQTAddress.cpp
	${U3D_DIR}/src/RTL/Component/Subdiv/IFXTQTAttribute.cpp
	${U3D_DIR}/src/RTL/Component/Subdiv/IFXTQTBaseTriangle.cpp
	${U3D_DIR}/src/RTL/Component/Subdiv/IFXTQTTriangleAllocator.cpp
	${U3D_DIR}/src/RTL/Component/Subdiv/IFXTQTTriangle.cpp
	${U3D_DIR}/src/RTL/Component/Subdiv/IFXVertexAllocator.cpp
	${U3D_DIR}/src/RTL/Component/Texture/CIFXImageTools.cpp
	${U3D_DIR}/src/RTL/Component/Texture/CIFXTextureImageTools.cpp
	${U3D_DIR}/src/RTL/Component/Texture/CIFXTextureObject.cpp
	${U3D_DIR}/src/RTL/Component/UVGenerator/CIFXUVGenerator.cpp
	${U3D_DIR}/src/RTL/Component/UVGenerator/CIFXUVMapperCylindrical.cpp
	${U3D_DIR}/src/RTL/Component/UVGenerator/CIFXUVMapperNone.cpp
	${U3D_DIR}/src/RTL/Component/UVGenerator/CIFXUVMapperPlanar.cpp
	${U3D_DIR}/src/RTL/Component/UVGenerator/CIFXUVMapperReflection.cpp
	${U3D_DIR}/src/RTL/Component/UVGenerator/CIFXUVMapperSpherical.cpp
	${U3D_DIR}/src/RTL/Kernel/Common/CIFXConnector.cpp
	${U3D_DIR}/src/RTL/Kernel/Common/CIFXPerformanceTimer.cpp
	${U3D_DIR}/src/RTL/Kernel/Common/IFXCheckX.cpp
	${U3D_DIR}/src/RTL/Kernel/IFXCom/CIFXComponentManager.cpp
	${U3D_DIR}/src/RTL/Kernel/IFXCom/CIFXGUIDHashMap.cpp
	${U3D_DIR}/src/RTL/Kernel/IFXCom/CIFXPluginProxy.cpp
	${U3D_DIR}/src/RTL/Kernel/IFXCom/IFXCom.cpp
	${U3D_DIR}/src/RTL/Kernel/Memory/IFXMemory.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXCoreArray.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXCoreList.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXFastAllocator.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXListNode.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXMatrix4x4.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXQuaternion.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXString.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXUnitAllocator.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXVector3.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXVector4.cpp
	${U3D_DIR}/src/RTL/Dependencies/WildCards/wcmatch.cpp
	${U3D_DIR}/src/RTL/Dependencies/Predicates/predicates.cpp
	${U3D_DIR}/src/RTL/Kernel/Common/IFXDebug.cpp
)
SET( IFXCore_HDRS
	${Component_HDRS}
	${Kernel_HDRS}
	${Platform_HDRS}
	${U3D_DIR}/src/RTL/Component/Base/IFXVectorHasher.h
	${U3D_DIR}/src/RTL/Component/BitStream/CIFXBitStreamX.h
	${U3D_DIR}/src/RTL/Component/BitStream/CIFXDataBlockQueueX.h
	${U3D_DIR}/src/RTL/Component/BitStream/CIFXDataBlockX.h
	${U3D_DIR}/src/RTL/Component/Bones/CIFXAnimationModifier.h
	${U3D_DIR}/src/RTL/Component/Bones/CIFXBoneWeightsModifier.h
	${U3D_DIR}/src/RTL/Component/Bones/CIFXSkeleton.h
	${U3D_DIR}/src/RTL/Component/Bones/IFXBoneCache.h
	${U3D_DIR}/src/RTL/Component/Bones/IFXBoneCacheArray.h
	${U3D_DIR}/src/RTL/Component/Bones/IFXBoneContext.h
	${U3D_DIR}/src/RTL/Component/Bones/IFXBoneLinks.h
	${U3D_DIR}/src/RTL/Component/Bones/IFXBoneNode.h
	${U3D_DIR}/src/RTL/Component/Bones/IFXBoneNodeList.h
	${U3D_DIR}/src/RTL/Component/Bones/IFXBonesManagerImpl.h
	${U3D_DIR}/src/RTL/Component/Bones/IFXCharacter.h
	${U3D_DIR}/src/RTL/Component/Bones/IFXConstraints.h
	${U3D_DIR}/src/RTL/Component/Bones/IFXCoreNode.h
	${U3D_DIR}/src/RTL/Component/Bones/IFXCylinder.h
	${U3D_DIR}/src/RTL/Component/Bones/IFXIKModes.h
	${U3D_DIR}/src/RTL/Component/Bones/IFXKeyFrameContext.h
	${U3D_DIR}/src/RTL/Component/Bones/IFXLong3.h
	${U3D_DIR}/src/RTL/Component/Bones/IFXMatrix3x4.h
	${U3D_DIR}/src/RTL/Component/Bones/IFXMeshGroup_Character.h
	${U3D_DIR}/src/RTL/Component/Bones/IFXMeshGroup_Impl.h
	${U3D_DIR}/src/RTL/Component/Bones/IFXMeshInterface.h
	${U3D_DIR}/src/RTL/Component/Bones/IFXMeshVertexMap.h
	${U3D_DIR}/src/RTL/Component/Bones/IFXMixerQueueImpl.h
	${U3D_DIR}/src/RTL/Component/Bones/IFXMotionManagerImpl.h
	${U3D_DIR}/src/RTL/Component/Bones/IFXMotionMixerImpl.h
	${U3D_DIR}/src/RTL/Component/Bones/IFXMotionReader.h
	${U3D_DIR}/src/RTL/Component/Bones/IFXPackWeights.h
	${U3D_DIR}/src/RTL/Component/Bones/IFXSkin.h
	${U3D_DIR}/src/RTL/Component/Bones/IFXVertexWeight.h
	${U3D_DIR}/src/RTL/Component/Bones/IFXVertexWeights.h
	${U3D_DIR}/src/RTL/Component/BoundHierarchy/CIFXAABBHierarchyBuilder.h
	${U3D_DIR}/src/RTL/Component/BoundHierarchy/CIFXAxisAlignedBBox.h
	${U3D_DIR}/src/RTL/Component/BoundHierarchy/CIFXBoundFace.h
	${U3D_DIR}/src/RTL/Component/BoundHierarchy/CIFXBoundHierarchy.h
	${U3D_DIR}/src/RTL/Component/BoundHierarchy/CIFXBoundUtil.h
	${U3D_DIR}/src/RTL/Component/BoundHierarchy/CIFXBTree.h
	${U3D_DIR}/src/RTL/Component/BoundHierarchy/CIFXBTreeNode.h
	${U3D_DIR}/src/RTL/Component/BoundHierarchy/CIFXCollisionResult.h
	${U3D_DIR}/src/RTL/Component/BoundHierarchy/CIFXPickObject.h
	${U3D_DIR}/src/RTL/Component/BoundHierarchy/CIFXPrimitiveOverlap.h
	${U3D_DIR}/src/RTL/Component/BoundHierarchy/CIFXResultAllocator.h
	${U3D_DIR}/src/RTL/Component/CLODAuthor/CIFXAuthorCLODGen.h
	${U3D_DIR}/src/RTL/Component/CLODAuthor/CIFXAuthorMeshMap.h
	${U3D_DIR}/src/RTL/Component/CLODAuthor/CIFXSetAdjacencyX.h
	${U3D_DIR}/src/RTL/Component/CLODAuthor/CIFXSetX.h
	${U3D_DIR}/src/RTL/Component/CLODAuthor/CLODGenerator.h
	${U3D_DIR}/src/RTL/Component/CLODAuthor/ContractionRecorder.h
	${U3D_DIR}/src/RTL/Component/CLODAuthor/CostMap.h
	${U3D_DIR}/src/RTL/Component/CLODAuthor/Face.h
	${U3D_DIR}/src/RTL/Component/CLODAuthor/FaceExam.h
	${U3D_DIR}/src/RTL/Component/CLODAuthor/FacePtrSet.h
	${U3D_DIR}/src/RTL/Component/CLODAuthor/FaceUpdate.h
	${U3D_DIR}/src/RTL/Component/CLODAuthor/GeometryObject.h
	${U3D_DIR}/src/RTL/Component/CLODAuthor/IFXSList.h
	${U3D_DIR}/src/RTL/Component/CLODAuthor/Matrix4x4.h
	${U3D_DIR}/src/RTL/Component/CLODAuthor/NormalMap.h
	${U3D_DIR}/src/RTL/Component/CLODAuthor/Pair.h
	${U3D_DIR}/src/RTL/Component/CLODAuthor/PairFinder.h
	${U3D_DIR}/src/RTL/Component/CLODAuthor/PairHash.h
	${U3D_DIR}/src/RTL/Component/CLODAuthor/PairHeap.h
	${U3D_DIR}/src/RTL/Component/CLODAuthor/Primitives.h
	${U3D_DIR}/src/RTL/Component/CLODAuthor/QEConstants.h
	${U3D_DIR}/src/RTL/Component/CLODAuthor/SmallPtrSet.h
	${U3D_DIR}/src/RTL/Component/CLODAuthor/Vertex.h
	${U3D_DIR}/src/RTL/Component/CLODAuthor/VertexPairContractor.h
	${U3D_DIR}/src/RTL/Component/Common/CIFXCoreServices.h
	${U3D_DIR}/src/RTL/Component/Common/CIFXCoreServicesRef.h
	${U3D_DIR}/src/RTL/Component/Common/CIFXHashMap.h
	${U3D_DIR}/src/RTL/Component/Common/CIFXIDManager.h
	${U3D_DIR}/src/RTL/Component/Common/CIFXMetaData.h
	${U3D_DIR}/src/RTL/Component/Common/CIFXNameMap.h
	${U3D_DIR}/src/RTL/Component/Common/CIFXSimpleHash.h
	${U3D_DIR}/src/RTL/Component/Common/CIFXVoidWrapper.h
	${U3D_DIR}/src/RTL/Component/Common/IFXComponentFactories.h
	${U3D_DIR}/src/RTL/Component/Generators/CLOD/CIFXAuthorCLODResource.h
	${U3D_DIR}/src/RTL/Component/Generators/CLOD/CIFXAuthorMesh.h
	${U3D_DIR}/src/RTL/Component/Generators/CLOD/CIFXAuthorMeshScrub.h
	${U3D_DIR}/src/RTL/Component/Generators/CLOD/CIFXCLODModifier.h
	${U3D_DIR}/src/RTL/Component/Generators/CLOD/CIFXMeshCompiler.h
	${U3D_DIR}/src/RTL/Component/Generators/CLOD/CIFXMeshMap.h
	${U3D_DIR}/src/RTL/Component/Generators/CLOD/IFXCLODManager.h
	${U3D_DIR}/src/RTL/Component/Generators/CLOD/IFXNeighborResController.h
	${U3D_DIR}/src/RTL/Component/Generators/Glyph2D/CIFXContour.h
	${U3D_DIR}/src/RTL/Component/Generators/Glyph2D/CIFXContourExtruder.h
	${U3D_DIR}/src/RTL/Component/Generators/Glyph2D/CIFXContourGenerator.h
	${U3D_DIR}/src/RTL/Component/Generators/Glyph2D/CIFXContourTessellator.h
	${U3D_DIR}/src/RTL/Component/Generators/Glyph2D/CIFXGeom2D.h
	${U3D_DIR}/src/RTL/Component/Generators/Glyph2D/CIFXGlyph2DCommands.h
	${U3D_DIR}/src/RTL/Component/Generators/Glyph2D/CIFXGlyph2DModifier.h
	${U3D_DIR}/src/RTL/Component/Generators/Glyph2D/CIFXGlyph3DGenerator.h
	${U3D_DIR}/src/RTL/Component/Generators/Glyph2D/CIFXGlyphCommandList.h
	${U3D_DIR}/src/RTL/Component/Generators/Glyph2D/CIFXQuadEdge.h
	${U3D_DIR}/src/RTL/Component/Generators/Glyph2D/IFXContour.h
	${U3D_DIR}/src/RTL/Component/Generators/Glyph2D/IFXContourExtruder.h
	${U3D_DIR}/src/RTL/Component/Generators/Glyph2D/IFXContourGenerator.h
	${U3D_DIR}/src/RTL/Component/Generators/Glyph2D/IFXContourTessellator.h
	${U3D_DIR}/src/RTL/Component/Generators/Glyph2D/IFXGlyph3DGenerator.h
	${U3D_DIR}/src/RTL/Component/Generators/LineSet/CIFXAuthorLineSet.h
	${U3D_DIR}/src/RTL/Component/Generators/LineSet/CIFXAuthorLineSetAnalyzer.h
	${U3D_DIR}/src/RTL/Component/Generators/LineSet/CIFXAuthorLineSetResource.h
	${U3D_DIR}/src/RTL/Component/Generators/PointSet/CIFXAuthorPointSet.h
	${U3D_DIR}/src/RTL/Component/Generators/PointSet/CIFXAuthorPointSetResource.h
	${U3D_DIR}/src/RTL/Component/Mesh/CIFXInterleavedData.h
	${U3D_DIR}/src/RTL/Component/Mesh/CIFXMesh.h
	${U3D_DIR}/src/RTL/Component/Mesh/CIFXMeshGroup.h
	${U3D_DIR}/src/RTL/Component/Mesh/CIFXNeighborMesh.h
	${U3D_DIR}/src/RTL/Component/Mesh/CIFXRenderable.h
	${U3D_DIR}/src/RTL/Component/Mesh/IFXFaceLists.h
	${U3D_DIR}/src/RTL/Component/ModifierChain/CIFXDidRegistry.h
	${U3D_DIR}/src/RTL/Component/ModifierChain/CIFXModifier.h
	${U3D_DIR}/src/RTL/Component/ModifierChain/CIFXModifierChain.h
	${U3D_DIR}/src/RTL/Component/ModifierChain/CIFXModifierDataElementIter.h
	${U3D_DIR}/src/RTL/Component/ModifierChain/CIFXModifierDataPacket.h
	${U3D_DIR}/src/RTL/Component/ModifierChain/CIFXObserverStateTree.h
	${U3D_DIR}/src/RTL/Component/ModifierChain/CIFXSubject.h
	${U3D_DIR}/src/RTL/Component/ModifierChain/CRedBlackTree.h
	${U3D_DIR}/src/RTL/Component/ModifierChain/IFXModifierChainInternal.h
	${U3D_DIR}/src/RTL/Component/ModifierChain/IFXModifierChainState.h
	${U3D_DIR}/src/RTL/Component/ModifierChain/IFXModifierDataPacketInternal.h
	${U3D_DIR}/src/RTL/Component/ModifierChain/IFXSet.h
	${U3D_DIR}/src/RTL/Component/Palette/CIFXPalette.h
	${U3D_DIR}/src/RTL/Component/Palette/CIFXSimpleObject.h
	${U3D_DIR}/src/RTL/Component/Rendering/CIFXDeviceBase.h
	${U3D_DIR}/src/RTL/Component/Rendering/CIFXDeviceLight.h
	${U3D_DIR}/src/RTL/Component/Rendering/CIFXDeviceTexture.h
	${U3D_DIR}/src/RTL/Component/Rendering/CIFXDeviceTexUnit.h
	${U3D_DIR}/src/RTL/Component/Rendering/CIFXRender.h
	${U3D_DIR}/src/RTL/Component/Rendering/CIFXRenderContext.h
	${U3D_DIR}/src/RTL/Component/Rendering/CIFXRenderDevice.h
	${U3D_DIR}/src/RTL/Component/Rendering/CIFXRenderServices.h
	${U3D_DIR}/src/RTL/Component/Rendering/DX7/CIFXDeviceLightDX7.h
	${U3D_DIR}/src/RTL/Component/Rendering/DX7/CIFXDeviceTextureDX7.h
	${U3D_DIR}/src/RTL/Component/Rendering/DX7/CIFXDeviceTexUnitDX7.h
	${U3D_DIR}/src/RTL/Component/Rendering/DX7/CIFXDirectX7.h
	${U3D_DIR}/src/RTL/Component/Rendering/DX7/CIFXRenderDeviceDX7.h
	${U3D_DIR}/src/RTL/Component/Rendering/DX7/CIFXRenderDX7.h
	${U3D_DIR}/src/RTL/Component/Rendering/DX7/IFXRenderPCHDX7.h
	${U3D_DIR}/src/RTL/Component/Rendering/DX8/CIFXDeviceLightDX8.h
	${U3D_DIR}/src/RTL/Component/Rendering/DX8/CIFXDeviceTextureDX8.h
	${U3D_DIR}/src/RTL/Component/Rendering/DX8/CIFXDeviceTexUnitDX8.h
	${U3D_DIR}/src/RTL/Component/Rendering/DX8/CIFXDirectX8.h
	${U3D_DIR}/src/RTL/Component/Rendering/DX8/CIFXRenderDeviceDX8.h
	${U3D_DIR}/src/RTL/Component/Rendering/DX8/CIFXRenderDX8.h
	${U3D_DIR}/src/RTL/Component/Rendering/DX8/IFXRenderPCHDX8.h
	${U3D_DIR}/src/RTL/Component/Rendering/IFXAAFilter.h
	${U3D_DIR}/src/RTL/Component/Rendering/IFXRenderPCH.h
	${U3D_DIR}/src/RTL/Component/Rendering/Null/CIFXDeviceLightNULL.h
	${U3D_DIR}/src/RTL/Component/Rendering/Null/CIFXDeviceTextureNULL.h
	${U3D_DIR}/src/RTL/Component/Rendering/Null/CIFXDeviceTexUnitNULL.h
	${U3D_DIR}/src/RTL/Component/Rendering/Null/CIFXRenderDeviceNULL.h
	${U3D_DIR}/src/RTL/Component/Rendering/Null/CIFXRenderNULL.h
	${U3D_DIR}/src/RTL/Component/Rendering/Null/IFXRenderPCHNULL.h
	${U3D_DIR}/src/RTL/Component/Rendering/OpenGL/CIFXDeviceLightOGL.h
	${U3D_DIR}/src/RTL/Component/Rendering/OpenGL/CIFXDeviceTextureOGL.h
	${U3D_DIR}/src/RTL/Component/Rendering/OpenGL/CIFXDeviceTexUnitOGL.h
	${U3D_DIR}/src/RTL/Component/Rendering/OpenGL/CIFXOpenGL.h
	${U3D_DIR}/src/RTL/Component/Rendering/OpenGL/CIFXRenderDeviceOGL.h
	${U3D_DIR}/src/RTL/Component/Rendering/OpenGL/CIFXRenderOGL.h
	${U3D_DIR}/src/RTL/Component/Rendering/OpenGL/IFXRenderPCHOGL.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXBoundSphereDataElement.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXDevice.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXDummyModifier.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXFileReference.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXGroup.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXLight.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXLightResource.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXLightSet.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXMarker.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXMaterialResource.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXMixerConstruct.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXModel.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXMotionResource.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXNode.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXResourceClient.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXSceneGraph.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXShaderList.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXSimpleCollection.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXSimpleList.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXView.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXViewResource.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/IFXSceneGraphPCH.h
	${U3D_DIR}/src/RTL/Component/Shaders/CIFXShader.h
	${U3D_DIR}/src/RTL/Component/Shaders/CIFXShaderLitTexture.h
	${U3D_DIR}/src/RTL/Component/Shaders/CIFXShadingModifier.h
	${U3D_DIR}/src/RTL/Component/Subdiv/CIFXSubdivModifier.h
	${U3D_DIR}/src/RTL/Component/Subdiv/IFXAttributeNeighborhood.h
	${U3D_DIR}/src/RTL/Component/Subdiv/IFXBFMaskLayout.h
	${U3D_DIR}/src/RTL/Component/Subdiv/IFXButterflyMask.h
	${U3D_DIR}/src/RTL/Component/Subdiv/IFXButterflyScheme.h
	${U3D_DIR}/src/RTL/Component/Subdiv/IFXScreenSpaceMetric.h
	${U3D_DIR}/src/RTL/Component/Subdiv/IFXSharedUnitAllocator.h
	${U3D_DIR}/src/RTL/Component/Subdiv/IFXSpecularMetric.h
	${U3D_DIR}/src/RTL/Component/Subdiv/IFXSubdivisionManager.h
	${U3D_DIR}/src/RTL/Component/Subdiv/IFXTQTAddress.h
	${U3D_DIR}/src/RTL/Component/Subdiv/IFXTQTAttribute.h
	${U3D_DIR}/src/RTL/Component/Subdiv/IFXTQTBaseTriangle.h
	${U3D_DIR}/src/RTL/Component/Subdiv/IFXTQTTriangle.h
	${U3D_DIR}/src/RTL/Component/Subdiv/IFXTQTTriangleAllocator.h
	${U3D_DIR}/src/RTL/Component/Subdiv/IFXTQTVertex.h
	${U3D_DIR}/src/RTL/Component/Subdiv/IFXVertexAllocator.h
	${U3D_DIR}/src/RTL/Component/Texture/CIFXImageTools.h
	${U3D_DIR}/src/RTL/Component/Texture/CIFXTextureImageTools.h
	${U3D_DIR}/src/RTL/Component/Texture/CIFXTextureObject.h
	${U3D_DIR}/src/RTL/Component/Texture/CIFXUtilities.h
	${U3D_DIR}/src/RTL/Component/Texture/IFXTextureErrors.h
	${U3D_DIR}/src/RTL/Component/UVGenerator/CIFXUVGenerator.h
	${U3D_DIR}/src/RTL/Component/UVGenerator/CIFXUVMapperCylindrical.h
	${U3D_DIR}/src/RTL/Component/UVGenerator/CIFXUVMapperNone.h
	${U3D_DIR}/src/RTL/Component/UVGenerator/CIFXUVMapperPlanar.h
	${U3D_DIR}/src/RTL/Component/UVGenerator/CIFXUVMapperReflection.h
	${U3D_DIR}/src/RTL/Component/UVGenerator/CIFXUVMapperSpherical.h
	${U3D_DIR}/src/RTL/Kernel/IFXCom/CIFXComponentManager.h
	${U3D_DIR}/src/RTL/Kernel/IFXCom/CIFXGuidHashMap.h
	${U3D_DIR}/src/RTL/Kernel/IFXCom/CIFXPluginProxy.h
	${U3D_DIR}/src/RTL/Kernel/Common/CIFXConnector.h
	${U3D_DIR}/src/RTL/Kernel/Common/CIFXPerformanceTimer.h
	${U3D_DIR}/src/RTL/Dependencies/FNVHash/FNVPlusPlus.h
	${U3D_DIR}/src/RTL/Dependencies/Predicates/predicates.h
	${U3D_DIR}/src/RTL/Dependencies/WildCards/wcmatch.h
)
IF(WIN32)
	SET( CORE_DIR ${U3D_DIR}/src/RTL/Platform/Win32/IFXCore )
	ADD_LIBRARY( IFXCore SHARED ${IFXCore_SRCS} ${IFXCore_HDRS} ${CORE_DIR}/IFXCore.rc ${CORE_DIR}/IFXResource.h ${CORE_DIR}/IFXCore.def ${DEPENDENCIES_SRCS} )
ENDIF(WIN32)
IF(APPLE)
	ADD_LIBRARY( IFXCore SHARED ${IFXCore_SRCS} ${IFXCore_HDRS} ${DEPENDENCIES_SRCS} )
	set_target_properties( IFXCore PROPERTIES
	LINK_FLAGS "${MY_LINK_FLAGS} -exported_symbols_list ${U3D_DIR}/src/RTL/Platform/Mac32/IFXCore/IFXCore.def"
	SUFFIX .so)
ENDIF(APPLE)
IF(UNIX AND NOT APPLE)
	ADD_LIBRARY( IFXCore SHARED ${IFXCore_SRCS} ${IFXCore_HDRS} ${DEPENDENCIES_SRCS} )
	set_target_properties( IFXCore PROPERTIES
	LINK_FLAGS "-Wl,--version-script=${U3D_DIR}/src/RTL/Platform/Lin32/IFXCore/IFXCore.list -Wl,--no-undefined" )
	target_link_libraries( IFXCore ${CMAKE_DL_LIBS} )
ENDIF(UNIX AND NOT APPLE)
TARGET_LINK_LIBRARIES( IFXCore ${ADDITIONAL_LIBRARIES} )
target_compile_options(IFXCore PRIVATE -w)


#IFXExporting
INCLUDE_DIRECTORIES(
	${U3D_DIR}/src/RTL/Component/Include
	${U3D_DIR}/src/RTL/Kernel/Include
	${U3D_DIR}/src/RTL/Platform/Include
	${U3D_DIR}/src/RTL/Component/Exporting
	${U3D_DIR}/src/RTL/Dependencies/WildCards )
SET( IFXExporting_HDRS
	${Component_HDRS}
	${Kernel_HDRS}
	${Platform_HDRS}
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXAnimationModifierEncoder.h
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXAuthorCLODEncoderX.h
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXAuthorGeomCompiler.h
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXBlockPriorityQueueX.h
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXBlockWriterX.h
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXBoneWeightsModifierEncoder.h
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXCLODModifierEncoder.h
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXDummyModifierEncoder.h
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXFileReferenceEncoder.h
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXGlyphModifierEncoder.h
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXGroupNodeEncoder.h
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXLightNodeEncoder.h
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXLightResourceEncoder.h
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXLineSetEncoder.h
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXMaterialResourceEncoder.h
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXModelNodeEncoder.h
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXMotionResourceEncoder.h
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXNodeBaseEncoder.h
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXPointSetEncoder.h
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXShaderLitTextureEncoder.h
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXShadingModifierEncoder.h
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXStdioWriteBufferX.h
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXSubdivisionModifierEncoder.h
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXViewNodeEncoder.h
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXViewResourceEncoder.h
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXWriteManager.h
	${U3D_DIR}/src/RTL/Dependencies/WildCards/wcmatch.h
)
SET( IFXExporting_SRCS
	${U3D_DIR}/src/RTL/Platform/${U3D_PLATFORM}/IFXExporting/IFXExportingDllMain.cpp
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXAnimationModifierEncoder.cpp
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXAuthorCLODEncoderX.cpp
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXAuthorCLODEncoderX_P.cpp
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXAuthorCLODEncoderX_S.cpp
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXAuthorGeomCompiler.cpp
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXBlockPriorityQueueX.cpp
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXBlockWriterX.cpp
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXBoneWeightsModifierEncoder.cpp
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXCLODModifierEncoder.cpp
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXDummyModifierEncoder.cpp
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXFileReferenceEncoder.cpp
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXGlyphModifierEncoder.cpp
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXGroupNodeEncoder.cpp
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXLightNodeEncoder.cpp
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXLightResourceEncoder.cpp
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXLineSetEncoder.cpp
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXMaterialResourceEncoder.cpp
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXModelNodeEncoder.cpp
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXMotionResourceEncoder.cpp
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXNodeBaseEncoder.cpp
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXPointSetEncoder.cpp
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXShaderLitTextureEncoder.cpp
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXShadingModifierEncoder.cpp
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXStdioWriteBufferX.cpp
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXSubdivisionModifierEncoder.cpp
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXViewNodeEncoder.cpp
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXViewResourceEncoder.cpp
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXWriteManager.cpp
	${U3D_DIR}/src/RTL/Component/Exporting/IFXExporting.cpp
	${U3D_DIR}/src/RTL/Component/Exporting/IFXExportingGuids.cpp
	${U3D_DIR}/src/RTL/IFXCorePluginStatic/IFXCorePluginStatic.cpp
	${U3D_DIR}/src/RTL/Platform/${U3D_PLATFORM}/Common/IFXOSUtilities.cpp
	${U3D_DIR}/src/RTL/Platform/${U3D_PLATFORM}/Common/IFXOSLoader.cpp
	${U3D_DIR}/src/RTL/Component/Base/IFXVertexMap.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXCoreArray.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXCoreList.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXFastAllocator.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXListNode.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXMatrix4x4.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXQuaternion.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXString.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXUnitAllocator.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXVector3.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXVector4.cpp
	${U3D_DIR}/src/RTL/Dependencies/WildCards/wcmatch.cpp
	${U3D_DIR}/src/RTL/Kernel/Common/IFXDebug.cpp
)
IF(WIN32)
	SET( EXPORT_DIR ${U3D_DIR}/src/RTL/Platform/Win32/IFXExporting )
	ADD_LIBRARY( IFXExporting  SHARED ${IFXExporting_SRCS} ${IFXExporting_HDRS} ${EXPORT_DIR}/IFXExporting.rc ${EXPORT_DIR}/IFXResource.h ${EXPORT_DIR}/IFXExporting.def )
	TARGET_LINK_LIBRARIES( IFXExporting IFXCore )
ENDIF(WIN32)
IF(APPLE)
	ADD_LIBRARY( IFXExporting SHARED ${IFXExporting_SRCS} ${IFXExporting_HDRS} )
	set_target_properties( IFXExporting IFXCore PROPERTIES
	LINK_FLAGS "${MY_LINK_FLAGS} -exported_symbols_list ${U3D_DIR}/src/RTL/Platform/Mac32/IFXExporting/IFXExporting.def   -undefined dynamic_lookup"
	SUFFIX .so)
ENDIF(APPLE)
IF(UNIX AND NOT APPLE)
	ADD_LIBRARY( IFXExporting SHARED ${IFXExporting_SRCS} ${IFXExporting_HDRS} )
	set_target_properties( IFXExporting  PROPERTIES
	LINK_FLAGS "-Wl,--version-script=${U3D_DIR}/src/RTL/Platform/Lin32/IFXExporting/IFXExporting.list" )
	TARGET_LINK_LIBRARIES( IFXExporting IFXCore IFXCoreStatic ${CMAKE_DL_LIBS} )
ENDIF(UNIX AND NOT APPLE)
target_compile_options(IFXExporting PRIVATE -w)

#IFXScheduling
INCLUDE_DIRECTORIES(
	${U3D_DIR}/src/RTL/Component/Include
	${U3D_DIR}/src/RTL/Kernel/Include
	${U3D_DIR}/src/RTL/Platform/Include
	${U3D_DIR}/src/RTL/Component/ModifierChain
	${U3D_DIR}/src/RTL/Component/SceneGraph
	${U3D_DIR}/src/RTL/Component/Scheduling
	${U3D_DIR}/src/RTL/Dependencies/WildCards )
SET( IFXScheduling_HDRS
	${Component_HDRS}
	${Kernel_HDRS}
	${Platform_HDRS}
	${U3D_DIR}/src/RTL/Component/ModifierChain/CIFXDidRegistry.h
	${U3D_DIR}/src/RTL/Component/ModifierChain/CIFXModifier.h
	${U3D_DIR}/src/RTL/Component/ModifierChain/CIFXModifierChain.h
	${U3D_DIR}/src/RTL/Component/ModifierChain/CIFXModifierDataElementIter.h
	${U3D_DIR}/src/RTL/Component/ModifierChain/CIFXModifierDataPacket.h
	${U3D_DIR}/src/RTL/Component/ModifierChain/CIFXObserverStateTree.h
	${U3D_DIR}/src/RTL/Component/ModifierChain/CIFXSubject.h
	${U3D_DIR}/src/RTL/Component/ModifierChain/CRedBlackTree.h
	${U3D_DIR}/src/RTL/Component/ModifierChain/IFXModifierChainInternal.h
	${U3D_DIR}/src/RTL/Component/ModifierChain/IFXModifierChainState.h
	${U3D_DIR}/src/RTL/Component/ModifierChain/IFXModifierDataPacketInternal.h
	${U3D_DIR}/src/RTL/Component/ModifierChain/IFXSet.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXBoundSphereDataElement.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXDevice.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXDummyModifier.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXFileReference.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXGroup.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXLight.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXLightResource.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXLightSet.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXMarker.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXMaterialResource.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXMixerConstruct.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXModel.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXMotionResource.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXNode.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXResourceClient.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXSceneGraph.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXShaderList.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXSimpleCollection.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXSimpleList.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXView.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXViewResource.h
	${U3D_DIR}/src/RTL/Component/SceneGraph/IFXSceneGraphPCH.h
	${U3D_DIR}/src/RTL/Component/Scheduling/CIFXClock.h
	${U3D_DIR}/src/RTL/Component/Scheduling/CIFXErrorInfo.h
	${U3D_DIR}/src/RTL/Component/Scheduling/CIFXNotificationInfo.h
	${U3D_DIR}/src/RTL/Component/Scheduling/CIFXNotificationManager.h
	${U3D_DIR}/src/RTL/Component/Scheduling/CIFXScheduler.h
	${U3D_DIR}/src/RTL/Component/Scheduling/CIFXSchedulerInfo.h
	${U3D_DIR}/src/RTL/Component/Scheduling/CIFXSimulationInfo.h
	${U3D_DIR}/src/RTL/Component/Scheduling/CIFXSimulationManager.h
	${U3D_DIR}/src/RTL/Component/Scheduling/CIFXSystemManager.h
	${U3D_DIR}/src/RTL/Component/Scheduling/CIFXTaskCallback.h
	${U3D_DIR}/src/RTL/Component/Scheduling/CIFXTaskData.h
	${U3D_DIR}/src/RTL/Component/Scheduling/CIFXTaskManager.h
	${U3D_DIR}/src/RTL/Component/Scheduling/CIFXTaskManagerNode.h
	${U3D_DIR}/src/RTL/Component/Scheduling/CIFXTaskManagerView.h
	${U3D_DIR}/src/RTL/Component/Scheduling/CIFXTimeManager.h
	${U3D_DIR}/src/RTL/Dependencies/WildCards/wcmatch.h
)
SET( IFXScheduling_SRCS
	${U3D_DIR}/src/RTL/Platform/${U3D_PLATFORM}/IFXScheduling/IFXSchedulingDllMain.cpp
	${U3D_DIR}/src/RTL/Component/Scheduling/CIFXClock.cpp
	${U3D_DIR}/src/RTL/Component/Scheduling/CIFXErrorInfo.cpp
	${U3D_DIR}/src/RTL/Component/Scheduling/CIFXNotificationInfo.cpp
	${U3D_DIR}/src/RTL/Component/Scheduling/CIFXNotificationManager.cpp
	${U3D_DIR}/src/RTL/Component/Scheduling/CIFXScheduler.cpp
	${U3D_DIR}/src/RTL/Component/Scheduling/CIFXSchedulerInfo.cpp
	${U3D_DIR}/src/RTL/Component/Scheduling/CIFXSimulationInfo.cpp
	${U3D_DIR}/src/RTL/Component/Scheduling/CIFXSimulationManager.cpp
	${U3D_DIR}/src/RTL/Component/Scheduling/CIFXSystemManager.cpp
	${U3D_DIR}/src/RTL/Component/Scheduling/CIFXTaskCallback.cpp
	${U3D_DIR}/src/RTL/Component/Scheduling/CIFXTaskData.cpp
	${U3D_DIR}/src/RTL/Component/Scheduling/CIFXTaskManager.cpp
	${U3D_DIR}/src/RTL/Component/Scheduling/CIFXTaskManagerNode.cpp
	${U3D_DIR}/src/RTL/Component/Scheduling/CIFXTaskManagerView.cpp
	${U3D_DIR}/src/RTL/Component/Scheduling/CIFXTimeManager.cpp
	${U3D_DIR}/src/RTL/Component/Scheduling/IFXScheduling.cpp
	${U3D_DIR}/src/RTL/Component/Scheduling/IFXSchedulingGuids.cpp
	${U3D_DIR}/src/RTL/IFXCorePluginStatic/IFXCorePluginStatic.cpp
	${U3D_DIR}/src/RTL/Platform/${U3D_PLATFORM}/Common/IFXOSUtilities.cpp
	${U3D_DIR}/src/RTL/Component/ModifierChain/CIFXSubject.cpp
	${U3D_DIR}/src/RTL/Component/ModifierChain/CIFXModifier.cpp
	${U3D_DIR}/src/RTL/Component/SceneGraph/CIFXMarker.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXCoreArray.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXCoreList.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXFastAllocator.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXListNode.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXMatrix4x4.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXQuaternion.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXString.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXUnitAllocator.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXVector3.cpp
	${U3D_DIR}/src/RTL/Kernel/DataTypes/IFXVector4.cpp
	${U3D_DIR}/src/RTL/Dependencies/WildCards/wcmatch.cpp
	${U3D_DIR}/src/RTL/Kernel/Common/IFXDebug.cpp
	)
IF(WIN32)
	SET( SCHED_DIR ${U3D_DIR}/src/RTL/Platform/Win32/IFXScheduling )
	ADD_LIBRARY( IFXScheduling SHARED ${IFXScheduling_SRCS} ${IFXScheduling_HDRS} ${SCHED_DIR}/IFXScheduling.rc ${SCHED_DIR}/IFXResource.h ${SCHED_DIR}/IFXScheduling.def )
	TARGET_LINK_LIBRARIES( IFXScheduling IFXCore )
ENDIF(WIN32)
IF(APPLE)
	ADD_LIBRARY( IFXScheduling SHARED ${IFXScheduling_SRCS} ${IFXScheduling_HDRS} )
	set_target_properties( IFXScheduling IFXCore PROPERTIES
	LINK_FLAGS "${MY_LINK_FLAGS} -exported_symbols_list ${U3D_DIR}/src/RTL/Platform/Mac32/IFXScheduling/IFXScheduling.def   -undefined dynamic_lookup"
	SUFFIX .so)
ENDIF(APPLE)
IF(UNIX AND NOT APPLE)
	ADD_LIBRARY( IFXScheduling SHARED ${IFXScheduling_SRCS} ${IFXScheduling_HDRS} )
	set_target_properties( IFXScheduling  PROPERTIES
	LINK_FLAGS "-Wl,--version-script=${U3D_DIR}/src/RTL/Platform/Lin32/IFXScheduling/IFXScheduling.list" )
	TARGET_LINK_LIBRARIES( IFXScheduling IFXCore IFXCoreStatic ${CMAKE_DL_LIBS} )
ENDIF(UNIX AND NOT APPLE)
target_compile_options(IFXScheduling PRIVATE -w)

#external-IDTFConverter
INCLUDE_DIRECTORIES(
	${U3D_DIR}/src/RTL/Component/Include
	${U3D_DIR}/src/RTL/Kernel/Include
	${U3D_DIR}/src/RTL/Platform/Include
	${U3D_DIR}/src/IDTF
	${U3D_DIR}/src/IDTF/Include
	${U3D_DIR}/src/IDTF/Common )
SET( IDTFConverter_SRCS
	${U3D_DIR}/src/IDTF/FileParser.cpp
	${U3D_DIR}/src/IDTF/SceneConverter.cpp
	${U3D_DIR}/src/IDTF/PointSetResourceParser.cpp
	${U3D_DIR}/src/IDTF/UrlListParser.cpp
	${U3D_DIR}/src/IDTF/NodeParser.cpp
	${U3D_DIR}/src/IDTF/ModifierParser.cpp
	${U3D_DIR}/src/IDTF/PointSetConverter.cpp
	${U3D_DIR}/src/IDTF/MaterialParser.cpp
	${U3D_DIR}/src/IDTF/MetaDataConverter.cpp
	${U3D_DIR}/src/IDTF/MeshResourceParser.cpp
	${U3D_DIR}/src/IDTF/ResourceConverter.cpp
	${U3D_DIR}/src/IDTF/TextureConverter.cpp
	${U3D_DIR}/src/IDTF/ResourceListParser.cpp
	${U3D_DIR}/src/IDTF/File.cpp
	${U3D_DIR}/src/IDTF/LineSetConverter.cpp
	${U3D_DIR}/src/IDTF/Converter.cpp
	#${U3D_DIR}/src/IDTF/ConverterDriver.cpp
	${U3D_DIR}/src/IDTF/ModelConverter.cpp
	${U3D_DIR}/src/IDTF/TextureParser.cpp
	${U3D_DIR}/src/IDTF/NodeConverter.cpp
	${U3D_DIR}/src/IDTF/MeshConverter.cpp
	${U3D_DIR}/src/IDTF/BlockParser.cpp
	${U3D_DIR}/src/IDTF/ModelResourceParser.cpp
	${U3D_DIR}/src/IDTF/FileReferenceConverter.cpp
	${U3D_DIR}/src/IDTF/ShaderParser.cpp
	${U3D_DIR}/src/IDTF/FileScanner.cpp
	${U3D_DIR}/src/IDTF/FileReferenceParser.cpp
	${U3D_DIR}/src/IDTF/ModifierConverter.cpp
	${U3D_DIR}/src/IDTF/MetaDataParser.cpp
	${U3D_DIR}/src/IDTF/LineSetResourceParser.cpp
	${U3D_DIR}/src/IDTF/Helpers/MiscUtilities.cpp
	${U3D_DIR}/src/IDTF/Helpers/TGAImage.cpp
	${U3D_DIR}/src/IDTF/Helpers/ModifierUtilities.cpp
	${U3D_DIR}/src/IDTF/Helpers/ConverterHelpers.cpp
	${U3D_DIR}/src/IDTF/Helpers/SceneUtilities.cpp
	${U3D_DIR}/src/IDTF/Helpers/DebugInfo.cpp
	${U3D_DIR}/src/IDTF/Helpers/Guids.cpp
	${U3D_DIR}/src/IDTF/Common/GlyphModifier.cpp
	${U3D_DIR}/src/IDTF/Common/ModelResource.cpp
	${U3D_DIR}/src/IDTF/Common/ModifierList.cpp
	${U3D_DIR}/src/IDTF/Common/NodeList.cpp
	${U3D_DIR}/src/IDTF/Common/FileReference.cpp
	${U3D_DIR}/src/IDTF/Common/ResourceList.cpp
	${U3D_DIR}/src/IDTF/Common/SceneResources.cpp
	${U3D_DIR}/src/IDTF/Common/ModelResourceList.cpp
	${U3D_DIR}/src/IDTF/Common/MetaDataList.cpp
	${U3D_DIR}/src/IDTF/Common/ParentList.cpp
	${U3D_DIR}/src/IDTF/Common/GlyphCommandList.cpp
)
SET( IDTFConverter_HDRS
	${Component_HDRS}
	${Kernel_HDRS}
	${Platform_HDRS}
	${U3D_DIR}/src/IDTF/Converter.h
	${U3D_DIR}/src/IDTF/BlockParser.h
	${U3D_DIR}/src/IDTF/DefaultSettings.h
	${U3D_DIR}/src/IDTF/File.h
	${U3D_DIR}/src/IDTF/FileParser.h
	${U3D_DIR}/src/IDTF/FileReferenceConverter.h
	${U3D_DIR}/src/IDTF/FileReferenceParser.h
	${U3D_DIR}/src/IDTF/FileScanner.h
	${U3D_DIR}/src/IDTF/IConverter.h
	${U3D_DIR}/src/IDTF/LineSetConverter.h
	${U3D_DIR}/src/IDTF/LineSetResourceParser.h
	${U3D_DIR}/src/IDTF/MaterialParser.h
	${U3D_DIR}/src/IDTF/MeshConverter.h
	${U3D_DIR}/src/IDTF/MeshResourceParser.h
	${U3D_DIR}/src/IDTF/MetaDataConverter.h
	${U3D_DIR}/src/IDTF/MetaDataParser.h
	${U3D_DIR}/src/IDTF/ModelConverter.h
	${U3D_DIR}/src/IDTF/ModelResourceParser.h
	${U3D_DIR}/src/IDTF/ModifierConverter.h
	${U3D_DIR}/src/IDTF/ModifierParser.h
	${U3D_DIR}/src/IDTF/NodeConverter.h
	${U3D_DIR}/src/IDTF/NodeParser.h
	${U3D_DIR}/src/IDTF/PointSetConverter.h
	${U3D_DIR}/src/IDTF/PointSetResourceParser.h
	${U3D_DIR}/src/IDTF/ResourceConverter.h
	${U3D_DIR}/src/IDTF/ResourceListParser.h
	${U3D_DIR}/src/IDTF/SceneConverter.h
	${U3D_DIR}/src/IDTF/ShaderParser.h
	${U3D_DIR}/src/IDTF/TextureConverter.h
	${U3D_DIR}/src/IDTF/TextureParser.h
	${U3D_DIR}/src/IDTF/UrlListParser.h
	${U3D_DIR}/src/IDTF/Include/ConverterHelpers.h
	${U3D_DIR}/src/IDTF/Include/ConverterOptions.h
	${U3D_DIR}/src/IDTF/Include/ConverterResult.h
	${U3D_DIR}/src/IDTF/Include/DebugInfo.h
	${U3D_DIR}/src/IDTF/Include/SceneUtilities.h
	${U3D_DIR}/src/IDTF/Include/TGAImage.h
	${U3D_DIR}/src/IDTF/Include/U3DHeaders.h
	${U3D_DIR}/src/IDTF/Common/AnimationModifier.h
	${U3D_DIR}/src/IDTF/Common/BoneWeightModifier.h
	${U3D_DIR}/src/IDTF/Common/CLODModifier.h
	${U3D_DIR}/src/IDTF/Common/Color.h
	${U3D_DIR}/src/IDTF/Common/FileReference.h
	${U3D_DIR}/src/IDTF/Common/GlyphCommandList.h
	${U3D_DIR}/src/IDTF/Common/GlyphCommands.h
	${U3D_DIR}/src/IDTF/Common/GlyphModifier.h
	${U3D_DIR}/src/IDTF/Common/INode.h
	${U3D_DIR}/src/IDTF/Common/Int2.h
	${U3D_DIR}/src/IDTF/Common/Int3.h
	${U3D_DIR}/src/IDTF/Common/IResource.h
	${U3D_DIR}/src/IDTF/Common/LightNode.h
	${U3D_DIR}/src/IDTF/Common/LightResource.h
	${U3D_DIR}/src/IDTF/Common/LightResourceList.h
	${U3D_DIR}/src/IDTF/Common/LineSetResource.h
	${U3D_DIR}/src/IDTF/Common/MaterialResource.h
	${U3D_DIR}/src/IDTF/Common/MaterialResourceList.h
	${U3D_DIR}/src/IDTF/Common/MeshResource.h
	${U3D_DIR}/src/IDTF/Common/MetaDataList.h
	${U3D_DIR}/src/IDTF/Common/ModelNode.h
	${U3D_DIR}/src/IDTF/Common/ModelResource.h
	${U3D_DIR}/src/IDTF/Common/ModelResourceList.h
	${U3D_DIR}/src/IDTF/Common/ModelSkeleton.h
	${U3D_DIR}/src/IDTF/Common/Modifier.h
	${U3D_DIR}/src/IDTF/Common/ModifierList.h
	${U3D_DIR}/src/IDTF/Common/MotionResource.h
	${U3D_DIR}/src/IDTF/Common/MotionResourceList.h
	${U3D_DIR}/src/IDTF/Common/Node.h
	${U3D_DIR}/src/IDTF/Common/NodeList.h
	${U3D_DIR}/src/IDTF/Common/ParentData.h
	${U3D_DIR}/src/IDTF/Common/ParentList.h
	${U3D_DIR}/src/IDTF/Common/Point.h
	${U3D_DIR}/src/IDTF/Common/PointSetResource.h
	${U3D_DIR}/src/IDTF/Common/Quat.h
	${U3D_DIR}/src/IDTF/Common/Resource.h
	${U3D_DIR}/src/IDTF/Common/ResourceList.h
	${U3D_DIR}/src/IDTF/Common/SceneData.h
	${U3D_DIR}/src/IDTF/Common/SceneResources.h
	${U3D_DIR}/src/IDTF/Common/ShaderResource.h
	${U3D_DIR}/src/IDTF/Common/ShaderResourceList.h
	${U3D_DIR}/src/IDTF/Common/ShadingDescription.h
	${U3D_DIR}/src/IDTF/Common/ShadingDescriptionList.h
	${U3D_DIR}/src/IDTF/Common/ShadingModifier.h
	${U3D_DIR}/src/IDTF/Common/SubdivisionModifier.h
	${U3D_DIR}/src/IDTF/Common/TextureResource.h
	${U3D_DIR}/src/IDTF/Common/TextureResourceList.h
	${U3D_DIR}/src/IDTF/Common/Tokens.h
	${U3D_DIR}/src/IDTF/Common/UrlList.h
	${U3D_DIR}/src/IDTF/Common/ViewNodeData.h
	${U3D_DIR}/src/IDTF/Common/ViewNode.h
	${U3D_DIR}/src/IDTF/Common/ViewResource.h
	${U3D_DIR}/src/IDTF/Common/ViewResourceList.h
)
IF(WIN32)
	SET( CORE_DIR ${U3D_DIR}/src/RTL/Platform/Win32/IDTFConverter )
	ADD_LIBRARY( external-IDTFConverter STATIC ${IDTFConverter_SRCS} ${IDTFConverter_HDRS} ${U3D_DIR}/src/IDTF/IDTFConverter.rc ${U3D_DIR}/src/IDTF/IFXResource.h)
ENDIF(WIN32)
IF(APPLE)
	ADD_LIBRARY( external-IDTFConverter ${IDTFConverter_SRCS} ${IDTFConverter_HDRS})
#	set_target_properties( external-IDTFConverter  PROPERTIES
#	LINK_FLAGS "${MY_LINK_FLAGS} -exported_symbols_list /dev/null" )
ENDIF(APPLE)
IF(UNIX AND NOT APPLE)
	ADD_LIBRARY( external-IDTFConverter STATIC ${IDTFConverter_SRCS} ${IDTFConverter_HDRS} )
ENDIF(UNIX AND NOT APPLE)

IF(UNIX AND NOT APPLE)
	set(U3D_LIB_RELATIVE_PATH "../external/u3d/")
	set(U3D_INSTALL_RELATIVE_PATH "../lib/meshlab/")
ELSEIF(APPLE)
	set(U3D_LIB_RELATIVE_PATH "../../../../external/u3d/")
	set(U3D_INSTALL_RELATIVE_PATH "../Frameworks/")
elseif(WIN32)
	set(U3D_LIB_RELATIVE_PATH "../external/u3d/")
	set(U3D_INSTALL_RELATIVE_PATH ".")
endif()

TARGET_LINK_LIBRARIES( external-IDTFConverter IFXCoreStatic ${ADDITIONAL_LIBRARIES} ${CMAKE_DL_LIBS} )

set_property(TARGET external-IDTFConverter PROPERTY FOLDER External)

SET_PROPERTY( SOURCE
	${U3D_DIR}/src/RTL/Platform/${U3D_PLATFORM}/Common/IFXOSLoader.cpp
	PROPERTY COMPILE_DEFINITIONS U3DPluginsPath="Plugins" U3DCorePath="${U3D_LIB_RELATIVE_PATH}" U3DInstallPath="${U3D_INSTALL_RELATIVE_PATH}")
IF(STDIO_HACK)
SET_PROPERTY( SOURCE
	${U3D_DIR}/src/RTL/Component/Exporting/CIFXStdioWriteBufferX.cpp
	${U3D_DIR}/src/IDTF/ConverterDriver.cpp
	${U3D_DIR}/src/IDTF/File.cpp
	PROPERTY COMPILE_DEFINITIONS STDIO_HACK )
SET_PROPERTY( SOURCE
	${U3D_DIR}/src/RTL/Platform/${U3D_PLATFORM}/Common/IFXOSLoader.cpp
	PROPERTY COMPILE_DEFINITIONS U3DPluginsPath="Plugins" U3DCorePath="${U3D_LIB_RELATIVE_PATH}" U3DInstallPath="${U3D_INSTALL_RELATIVE_PATH}" STDIO_HACK )
ENDIF(STDIO_HACK)

install(TARGETS IFXCore IFXExporting IFXScheduling DESTINATION ${MESHLAB_LIB_INSTALL_DIR})
