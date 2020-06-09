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

INCLUDE_DIRECTORIES( RTL/Component/Include RTL/Kernel/Include RTL/Platform/Include
	IDTF
	IDTF/Include
	IDTF/Common )
SET( IDTFConverter_SRCS
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
	IDTF/Converter.cpp
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
ADD_LIBRARY( IDTFConverter STATIC ${IFXCoreStatic_SRCS} ${IFXCoreStatic_HDRS} ${IDTFConverter_SRCS} ${IDTFConverter_HDRS} )
install(
	TARGETS IDTFConverter
	ARCHIVE DESTINATION ${LIB_DESTINATION}
	LIBRARY DESTINATION ${LIB_DESTINATION}
)

SET( TEST_SRCS
	test/main.cpp
)
add_executable( test ${TEST_SRCS})
target_link_libraries(
	test
	PUBLIC IDTFConverter  ${CMAKE_DL_LIBS} )
add_dependencies(
	test
	IDTFConverter )
install(
	TARGETS test
	DESTINATION ${BIN_DESTINATION}
)
