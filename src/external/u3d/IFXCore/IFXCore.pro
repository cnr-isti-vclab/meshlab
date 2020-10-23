include(../../ext_common.pri)
TEMPLATE = lib
CONFIG += dll
TARGET = IFXCore
#DESTDIR = ../

BASE_SRC_IDTF = $${PWD}/../src/IDTF
BASE_SRC_RTL = $${PWD}/../src/RTL

linux{
	U3D_PLATFORM=Lin32
}
macx {
	DEFINES+=MAC32
	U3D_PLATFORM=Mac32
}

win32 {
	U3D_PLATFORM=Win32
}

include(zlib.pri)
include(png.pri)
include(jpeg.pri)

INCLUDEPATH += \
	$${BASE_SRC_RTL}/Component/Include \
	$${BASE_SRC_RTL}/Kernel/Include \
	$${BASE_SRC_RTL}/Platform/Include \
	$${BASE_SRC_RTL}/Component/Base \
	$${BASE_SRC_RTL}/Component/BitStream \
	$${BASE_SRC_RTL}/Component/Bones \
	$${BASE_SRC_RTL}/Component/BoundHierarchy \
	$${BASE_SRC_RTL}/Component/CLODAuthor \
	$${BASE_SRC_RTL}/Component/Common \
	$${BASE_SRC_RTL}/Component/Generators/CLOD \
	$${BASE_SRC_RTL}/Component/Generators/Glyph2D \
	$${BASE_SRC_RTL}/Component/Generators/LineSet \
	$${BASE_SRC_RTL}/Component/Generators/PointSet \
	$${BASE_SRC_RTL}/Component/Mesh \
	$${BASE_SRC_RTL}/Component/ModifierChain \
	$${BASE_SRC_RTL}/Component/Palette \
	$${BASE_SRC_RTL}/Component/Rendering \
	$${BASE_SRC_RTL}/Component/SceneGraph \
	$${BASE_SRC_RTL}/Component/Shaders \
	$${BASE_SRC_RTL}/Component/Subdiv \
	$${BASE_SRC_RTL}/Component/Texture \
	$${BASE_SRC_RTL}/Component/UVGenerator \
	$${BASE_SRC_RTL}/Kernel/IFXCom \
	$${BASE_SRC_RTL}/Kernel/Common \
	$${BASE_SRC_RTL}/Dependencies/FNVHash \
	$${BASE_SRC_RTL}/Dependencies/Predicates \
	$${BASE_SRC_RTL}/Dependencies/WildCards

SOURCES += \
	$${BASE_SRC_RTL}/Platform/$${U3D_PLATFORM}/IFXCore/IFXCoreDllMain.cpp \
	$${BASE_SRC_RTL}/IFXCorePluginStatic/IFXCorePluginStatic.cpp \
	$${BASE_SRC_RTL}/Platform/$${U3D_PLATFORM}/Common/IFXOSLoader.cpp \
	$${BASE_SRC_RTL}/Platform/$${U3D_PLATFORM}/Common/IFXOSRenderWindow.cpp \
	$${BASE_SRC_RTL}/Platform/$${U3D_PLATFORM}/Common/IFXOSUtilities.cpp \
	$${BASE_SRC_RTL}/Component/Base/IFXCoincidentVertexMap.cpp \
	$${BASE_SRC_RTL}/Component/Base/IFXCornerIter.cpp \
	$${BASE_SRC_RTL}/Component/Base/IFXEuler.cpp \
	$${BASE_SRC_RTL}/Component/Base/IFXFatCornerIter.cpp \
	$${BASE_SRC_RTL}/Component/Base/IFXTransform.cpp \
	$${BASE_SRC_RTL}/Component/Base/IFXVectorHasher.cpp \
	$${BASE_SRC_RTL}/Component/Base/IFXVertexMap.cpp \
	$${BASE_SRC_RTL}/Component/Base/IFXVertexMapGroup.cpp \
	$${BASE_SRC_RTL}/Component/BitStream/CIFXBitStreamX.cpp \
	$${BASE_SRC_RTL}/Component/BitStream/CIFXDataBlockQueueX.cpp \
	$${BASE_SRC_RTL}/Component/BitStream/CIFXDataBlockX.cpp \
	$${BASE_SRC_RTL}/Component/BitStream/IFXHistogramDynamic.cpp \
	$${BASE_SRC_RTL}/Component/Bones/CIFXAnimationModifier.cpp \
	$${BASE_SRC_RTL}/Component/Bones/CIFXBoneWeightsModifier.cpp \
	$${BASE_SRC_RTL}/Component/Bones/CIFXSkeleton.cpp \
	$${BASE_SRC_RTL}/Component/Bones/IFXBoneCacheArray.cpp \
	$${BASE_SRC_RTL}/Component/Bones/IFXBoneNode.cpp \
	$${BASE_SRC_RTL}/Component/Bones/IFXBonesManagerImpl.cpp \
	$${BASE_SRC_RTL}/Component/Bones/IFXCharacter.cpp \
	$${BASE_SRC_RTL}/Component/Bones/IFXCoreNode.cpp \
	$${BASE_SRC_RTL}/Component/Bones/IFXKeyTrack.cpp \
	$${BASE_SRC_RTL}/Component/Bones/IFXMeshGroup_Character.cpp \
	$${BASE_SRC_RTL}/Component/Bones/IFXMixerQueue.cpp \
	$${BASE_SRC_RTL}/Component/Bones/IFXMixerQueueImpl.cpp \
	$${BASE_SRC_RTL}/Component/Bones/IFXMotion.cpp \
	$${BASE_SRC_RTL}/Component/Bones/IFXMotionManagerImpl.cpp \
	$${BASE_SRC_RTL}/Component/Bones/IFXMotionMixerImpl.cpp \
	$${BASE_SRC_RTL}/Component/Bones/IFXSkin.cpp \
	$${BASE_SRC_RTL}/Component/Bones/IFXSkin_p3.cpp \
	$${BASE_SRC_RTL}/Component/Bones/IFXVertexWeight.cpp \
	$${BASE_SRC_RTL}/Component/Bones/IFXVertexWeights.cpp \
	$${BASE_SRC_RTL}/Component/BoundHierarchy/CIFXAABBHierarchyBuilder.cpp \
	$${BASE_SRC_RTL}/Component/BoundHierarchy/CIFXAxisAlignedBBox.cpp \
	$${BASE_SRC_RTL}/Component/BoundHierarchy/CIFXBoundFace.cpp \
	$${BASE_SRC_RTL}/Component/BoundHierarchy/CIFXBoundHierarchy.cpp \
	$${BASE_SRC_RTL}/Component/BoundHierarchy/CIFXBoundUtil.cpp \
	$${BASE_SRC_RTL}/Component/BoundHierarchy/CIFXBTree.cpp \
	$${BASE_SRC_RTL}/Component/BoundHierarchy/CIFXBTreeNode.cpp \
	$${BASE_SRC_RTL}/Component/BoundHierarchy/CIFXCollisionResult.cpp \
	$${BASE_SRC_RTL}/Component/BoundHierarchy/CIFXPickObject.cpp \
	$${BASE_SRC_RTL}/Component/BoundHierarchy/CIFXPrimitiveOverlap.cpp \
	$${BASE_SRC_RTL}/Component/BoundHierarchy/CIFXResultAllocator.cpp \
	$${BASE_SRC_RTL}/Component/CLODAuthor/CIFXAuthorCLODGen.cpp \
	$${BASE_SRC_RTL}/Component/CLODAuthor/CIFXAuthorMeshMap.cpp \
	$${BASE_SRC_RTL}/Component/CLODAuthor/CIFXSetAdjacencyX.cpp \
	$${BASE_SRC_RTL}/Component/CLODAuthor/CIFXSetX.cpp \
	$${BASE_SRC_RTL}/Component/CLODAuthor/CLODGenerator.cpp \
	$${BASE_SRC_RTL}/Component/CLODAuthor/ContractionRecorder.cpp \
	$${BASE_SRC_RTL}/Component/CLODAuthor/Face.cpp \
	$${BASE_SRC_RTL}/Component/CLODAuthor/Matrix4x4.cpp \
	$${BASE_SRC_RTL}/Component/CLODAuthor/NormalMap.cpp \
	$${BASE_SRC_RTL}/Component/CLODAuthor/Pair.cpp \
	$${BASE_SRC_RTL}/Component/CLODAuthor/PairFinder.cpp \
	$${BASE_SRC_RTL}/Component/CLODAuthor/PairHash.cpp \
	$${BASE_SRC_RTL}/Component/CLODAuthor/PairHeap.cpp \
	$${BASE_SRC_RTL}/Component/CLODAuthor/Primitives.cpp \
	$${BASE_SRC_RTL}/Component/CLODAuthor/Vertex.cpp \
	$${BASE_SRC_RTL}/Component/CLODAuthor/VertexPairContractor.cpp \
	$${BASE_SRC_RTL}/Component/Common/CIFXCoreServices.cpp \
	$${BASE_SRC_RTL}/Component/Common/CIFXCoreServicesRef.cpp \
	$${BASE_SRC_RTL}/Component/Common/CIFXHashMap.cpp \
	$${BASE_SRC_RTL}/Component/Common/CIFXIDManager.cpp \
	$${BASE_SRC_RTL}/Component/Common/CIFXMetaData.cpp \
	$${BASE_SRC_RTL}/Component/Common/CIFXNameMap.cpp \
	$${BASE_SRC_RTL}/Component/Common/CIFXSimpleHash.cpp \
	$${BASE_SRC_RTL}/Component/Common/CIFXVoidWrapper.cpp \
	$${BASE_SRC_RTL}/Component/Common/IFXComponentDescriptorList.cpp \
	$${BASE_SRC_RTL}/Component/Common/IFXComponentIds.cpp \
	$${BASE_SRC_RTL}/Component/Common/IFXDids.cpp \
	$${BASE_SRC_RTL}/Component/Generators/CLOD/CIFXAuthorCLODResource.cpp \
	$${BASE_SRC_RTL}/Component/Generators/CLOD/CIFXAuthorMesh.cpp \
	$${BASE_SRC_RTL}/Component/Generators/CLOD/CIFXAuthorMeshScrub.cpp \
	$${BASE_SRC_RTL}/Component/Generators/CLOD/CIFXCLODModifier.cpp \
	$${BASE_SRC_RTL}/Component/Generators/CLOD/CIFXMeshCompiler.cpp \
	$${BASE_SRC_RTL}/Component/Generators/CLOD/CIFXMeshMap.cpp \
	$${BASE_SRC_RTL}/Component/Generators/CLOD/IFXCLODManager.cpp \
	$${BASE_SRC_RTL}/Component/Generators/CLOD/IFXNeighborResController.cpp \
	$${BASE_SRC_RTL}/Component/Generators/Glyph2D/CIFXContour.cpp \
	$${BASE_SRC_RTL}/Component/Generators/Glyph2D/CIFXContourExtruder.cpp \
	$${BASE_SRC_RTL}/Component/Generators/Glyph2D/CIFXContourGenerator.cpp \
	$${BASE_SRC_RTL}/Component/Generators/Glyph2D/CIFXContourTessellator.cpp \
	$${BASE_SRC_RTL}/Component/Generators/Glyph2D/CIFXGlyph2DCommands.cpp \
	$${BASE_SRC_RTL}/Component/Generators/Glyph2D/CIFXGlyph2DModifier.cpp \
	$${BASE_SRC_RTL}/Component/Generators/Glyph2D/CIFXGlyph3DGenerator.cpp \
	$${BASE_SRC_RTL}/Component/Generators/Glyph2D/CIFXGlyphCommandList.cpp \
	$${BASE_SRC_RTL}/Component/Generators/Glyph2D/CIFXQuadEdge.cpp \
	$${BASE_SRC_RTL}/Component/Generators/LineSet/CIFXAuthorLineSetAnalyzer.cpp \
	$${BASE_SRC_RTL}/Component/Generators/LineSet/CIFXAuthorLineSet.cpp \
	$${BASE_SRC_RTL}/Component/Generators/LineSet/CIFXAuthorLineSetResource.cpp \
	$${BASE_SRC_RTL}/Component/Generators/PointSet/CIFXAuthorPointSet.cpp \
	$${BASE_SRC_RTL}/Component/Generators/PointSet/CIFXAuthorPointSetResource.cpp \
	$${BASE_SRC_RTL}/Component/Mesh/CIFXInterleavedData.cpp \
	$${BASE_SRC_RTL}/Component/Mesh/CIFXMesh.cpp \
	$${BASE_SRC_RTL}/Component/Mesh/CIFXMeshGroup.cpp \
	$${BASE_SRC_RTL}/Component/Mesh/CIFXNeighborMesh.cpp \
	$${BASE_SRC_RTL}/Component/Mesh/CIFXRenderable.cpp \
	$${BASE_SRC_RTL}/Component/Mesh/IFXFaceLists.cpp \
	$${BASE_SRC_RTL}/Component/ModifierChain/CIFXDidRegistry.cpp \
	$${BASE_SRC_RTL}/Component/ModifierChain/CIFXModifierChain.cpp \
	$${BASE_SRC_RTL}/Component/ModifierChain/CIFXModifier.cpp \
	$${BASE_SRC_RTL}/Component/ModifierChain/CIFXModifierDataElementIter.cpp \
	$${BASE_SRC_RTL}/Component/ModifierChain/CIFXModifierDataPacket.cpp \
	$${BASE_SRC_RTL}/Component/ModifierChain/CIFXSubject.cpp \
	$${BASE_SRC_RTL}/Component/ModifierChain/IFXModifierChainState.cpp \
	$${BASE_SRC_RTL}/Component/Palette/CIFXPalette.cpp \
	$${BASE_SRC_RTL}/Component/Palette/CIFXSimpleObject.cpp \
	$${BASE_SRC_RTL}/Component/SceneGraph/CIFXBoundSphereDataElement.cpp \
	$${BASE_SRC_RTL}/Component/SceneGraph/CIFXDevice.cpp \
	$${BASE_SRC_RTL}/Component/SceneGraph/CIFXDummyModifier.cpp \
	$${BASE_SRC_RTL}/Component/SceneGraph/CIFXFileReference.cpp \
	$${BASE_SRC_RTL}/Component/SceneGraph/CIFXGroup.cpp \
	$${BASE_SRC_RTL}/Component/SceneGraph/CIFXLight.cpp \
	$${BASE_SRC_RTL}/Component/SceneGraph/CIFXLightResource.cpp \
	$${BASE_SRC_RTL}/Component/SceneGraph/CIFXLightSet.cpp \
	$${BASE_SRC_RTL}/Component/SceneGraph/CIFXMarker.cpp \
	$${BASE_SRC_RTL}/Component/SceneGraph/CIFXMaterialResource.cpp \
	$${BASE_SRC_RTL}/Component/SceneGraph/CIFXMixerConstruct.cpp \
	$${BASE_SRC_RTL}/Component/SceneGraph/CIFXModel.cpp \
	$${BASE_SRC_RTL}/Component/SceneGraph/CIFXMotionResource.cpp \
	$${BASE_SRC_RTL}/Component/SceneGraph/CIFXNode.cpp \
	$${BASE_SRC_RTL}/Component/SceneGraph/CIFXResourceClient.cpp \
	$${BASE_SRC_RTL}/Component/SceneGraph/CIFXSceneGraph.cpp \
	$${BASE_SRC_RTL}/Component/SceneGraph/CIFXShaderList.cpp \
	$${BASE_SRC_RTL}/Component/SceneGraph/CIFXSimpleCollection.cpp \
	$${BASE_SRC_RTL}/Component/SceneGraph/CIFXSimpleList.cpp \
	$${BASE_SRC_RTL}/Component/SceneGraph/CIFXView.cpp \
	$${BASE_SRC_RTL}/Component/SceneGraph/CIFXViewResource.cpp \
	$${BASE_SRC_RTL}/Component/SceneGraph/IFXRenderPass.cpp \
	$${BASE_SRC_RTL}/Component/Shaders/CIFXShader.cpp \
	$${BASE_SRC_RTL}/Component/Shaders/CIFXShaderLitTexture.cpp \
	$${BASE_SRC_RTL}/Component/Shaders/CIFXShadingModifier.cpp \
	$${BASE_SRC_RTL}/Component/Subdiv/CIFXSubdivModifier.cpp \
	$${BASE_SRC_RTL}/Component/Subdiv/IFXAttributeNeighborhood.cpp \
	$${BASE_SRC_RTL}/Component/Subdiv/IFXButterflyScheme.cpp \
	$${BASE_SRC_RTL}/Component/Subdiv/IFXScreenSpaceMetric.cpp \
	$${BASE_SRC_RTL}/Component/Subdiv/IFXSharedUnitAllocator.cpp \
	$${BASE_SRC_RTL}/Component/Subdiv/IFXSubdivisionManager.cpp \
	$${BASE_SRC_RTL}/Component/Subdiv/IFXTQTAddress.cpp \
	$${BASE_SRC_RTL}/Component/Subdiv/IFXTQTAttribute.cpp \
	$${BASE_SRC_RTL}/Component/Subdiv/IFXTQTBaseTriangle.cpp \
	$${BASE_SRC_RTL}/Component/Subdiv/IFXTQTTriangleAllocator.cpp \
	$${BASE_SRC_RTL}/Component/Subdiv/IFXTQTTriangle.cpp \
	$${BASE_SRC_RTL}/Component/Subdiv/IFXVertexAllocator.cpp \
	$${BASE_SRC_RTL}/Component/Texture/CIFXImageTools.cpp \
	$${BASE_SRC_RTL}/Component/Texture/CIFXTextureImageTools.cpp \
	$${BASE_SRC_RTL}/Component/Texture/CIFXTextureObject.cpp \
	$${BASE_SRC_RTL}/Component/UVGenerator/CIFXUVGenerator.cpp \
	$${BASE_SRC_RTL}/Component/UVGenerator/CIFXUVMapperCylindrical.cpp \
	$${BASE_SRC_RTL}/Component/UVGenerator/CIFXUVMapperNone.cpp \
	$${BASE_SRC_RTL}/Component/UVGenerator/CIFXUVMapperPlanar.cpp \
	$${BASE_SRC_RTL}/Component/UVGenerator/CIFXUVMapperReflection.cpp \
	$${BASE_SRC_RTL}/Component/UVGenerator/CIFXUVMapperSpherical.cpp \
	$${BASE_SRC_RTL}/Kernel/Common/CIFXConnector.cpp \
	$${BASE_SRC_RTL}/Kernel/Common/CIFXPerformanceTimer.cpp \
	$${BASE_SRC_RTL}/Kernel/Common/IFXCheckX.cpp \
	$${BASE_SRC_RTL}/Kernel/IFXCom/CIFXComponentManager.cpp \
	$${BASE_SRC_RTL}/Kernel/IFXCom/CIFXGUIDHashMap.cpp \
	$${BASE_SRC_RTL}/Kernel/IFXCom/CIFXPluginProxy.cpp \
	$${BASE_SRC_RTL}/Kernel/IFXCom/IFXCom.cpp \
	$${BASE_SRC_RTL}/Kernel/Memory/IFXMemory.cpp \
	$${BASE_SRC_RTL}/Kernel/DataTypes/IFXCoreArray.cpp \
	$${BASE_SRC_RTL}/Kernel/DataTypes/IFXCoreList.cpp \
	$${BASE_SRC_RTL}/Kernel/DataTypes/IFXFastAllocator.cpp \
	$${BASE_SRC_RTL}/Kernel/DataTypes/IFXListNode.cpp \
	$${BASE_SRC_RTL}/Kernel/DataTypes/IFXMatrix4x4.cpp \
	$${BASE_SRC_RTL}/Kernel/DataTypes/IFXQuaternion.cpp \
	$${BASE_SRC_RTL}/Kernel/DataTypes/IFXString.cpp \
	$${BASE_SRC_RTL}/Kernel/DataTypes/IFXUnitAllocator.cpp \
	$${BASE_SRC_RTL}/Kernel/DataTypes/IFXVector3.cpp \
	$${BASE_SRC_RTL}/Kernel/DataTypes/IFXVector4.cpp \
	$${BASE_SRC_RTL}/Dependencies/WildCards/wcmatch.cpp \
	$${BASE_SRC_RTL}/Dependencies/Predicates/predicates.cpp \
	$${BASE_SRC_RTL}/Kernel/Common/IFXDebug.cpp

linux {
	LIBS += -ldl
}

macx {
	QMAKE_LFLAGS_PLUGIN -= -dynamiclib
	QMAKE_LFLAGS_PLUGIN += -bundle
	QMAKE_EXTENSION_SHLIB = so
}

win32 {
	RC_FILE = $${BASE_SRC_RTL}/Platform/Win32/IFXCore/IFXCore.rc
	HEADERS += \
		$${BASE_SRC_RTL}//Platform/Win32/IFXCore/IFXResource.h
	DEF_FILE = $${BASE_SRC_RTL}/Platform/Win32/IFXCore/IFXCore.def
	LIBS += winmm.lib user32.lib
}
