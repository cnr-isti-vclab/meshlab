//***************************************************************************
//
//  Copyright (c) 1999 - 2006 Intel Corporation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//***************************************************************************

/**
	@file	IFXComponentDescriptorList.cpp

			This module contains the component descriptors for the Core DL.
*/


//***************************************************************************
//  Includes
//***************************************************************************


#include "IFXComponentFactories.h"
#include "IFXCoreCIDs.h"


//***************************************************************************
//  Global data
//***************************************************************************


/**
	List of IFXComponentDescriptor structures for each IFXCOM component
	exposed by the DL.
*/
IFXComponentDescriptor g_coreComponentDescriptorList[] =
{
	{
		&CID_IFXNameMap,
		{CIFXNameMap_Factory},
		1
	},
	{
		&CID_IFXAuthorMeshMap,
		{CIFXAuthorMeshMap_Factory},
		1
	},
	{
		&CID_IFXCLODManager,
		{CIFXCLODManager_Factory},
		1
	},
	{
		&CID_IFXAuthorLineSetAnalyzer,
		{CIFXAuthorLineSetAnalyzer_Factory},
		1
	},
	{
		&CID_IFXMeshCompiler,
		{CIFXMeshCompiler_Factory},
		1
	},
	{
		&CID_IFXMeshMap,
		{CIFXMeshMap_Factory},
		1
	},
	{
		&CID_IFXAuthorCLODGen,
		{CIFXAuthorCLODGen_Factory},
		1
	},
	{
		&CID_IFXSetAdjacencyX,
		{CIFXSetAdjacencyX_Factory},
		1
	},
	{
		&CID_IFXSetX,
		{CIFXSetX_Factory},
		1
	},
	{
		&CID_IFXGlyph3DGenerator,
		{CIFXGlyph3DGenerator_Factory},
		1
	},
	{
		&CID_IFXGlyphCommandList,
		{CIFXGlyphCommandList_Factory},
		1
	},
	{
		&CID_IFXAuthorMesh,
		{CIFXAuthorMesh_Factory},
		1
	},
	{
		&CID_IFXAuthorCLODResource,
		{CIFXAuthorCLODResource_Factory},
		1
	},
	{
		&CID_IFXAuthorMeshScrub,
		{CIFXAuthorMeshScrub_Factory}, 1
	},
	{
		&CID_IFXBitStreamX,
		{CIFXBitStreamX_Factory}, 1
	},
	{
		&CID_IFXFileReference,
		{CIFXFileReference_Factory}, 1
	},
	{
		&CID_IFXBoundHierarchy,
		{CIFXBoundHierarchy_Factory}, 1
	},
	{
		&CID_IFXSkeleton,
		{CIFXSkeleton_Factory}, 1
	},
	{
		&CID_IFXBoneWeightsModifier,
		{CIFXBoneWeightsModifier_Factory}, 1
	},
	{
		&CID_IFXBoundSphereDataElement,
		{CIFXBoundSphereDataElement_Factory}, 1
	},
	{
		&CID_IFXAuthorLineSet,
		{CIFXAuthorLineSet_Factory},1
	},
	{
		&CID_IFXAuthorLineSetResource,
		{CIFXAuthorLineSetResource_Factory},1
	},
	{
		&CID_IFXAuthorPointSet,
		{CIFXAuthorPointSet_Factory},1
	},
	{
		&CID_IFXAuthorPointSetResource,
		{CIFXAuthorPointSetResource_Factory},1
	},
	{
		&CID_IFXConnector,
		{CIFXConnector_Factory}, 1
	},
	{
		&CID_IFXContour,
		{CIFXContour_Factory}, 1
	},
	{
		&CID_IFXContourExtruder,
		{CIFXContourExtruder_Factory}, 1
	},
	{
		&CID_IFXContourGenerator,
		{CIFXContourGenerator_Factory}, 1
	},
	{
		&CID_IFXContourTessellator,
		{CIFXContourTessellator_Factory}, 1
	},
	{
		&CID_IFXCoreServices,
		{CIFXCoreServices_Factory}, 1
	},
	{
		&CID_IFXCoreServicesRef,
		{CIFXCoreServicesRef_Factory}, 1
	},
	{
		&CID_IFXDataBlock,
		{CIFXDataBlockX_Factory}, 1
	},
	{
		&CID_IFXDataBlockX,
		{CIFXDataBlockX_Factory}, 1
	},
	{
		&CID_IFXDataBlockQueueX,
		{CIFXDataBlockQueueX_Factory}, 1
	},
	{
		&CID_IFXModifierDataPacket,
		{CIFXModifierDataPacket_Factory}, 1
	},
	{
		&CID_IFXDevice,
		{CIFXDevice_Factory}, 1
	},
	{
		&CID_IFXGlyphCurveToBlock,
		{CIFXCurveToBlock_Factory}, 1
	},
	{
		&CID_IFXGlyphLineToBlock,
		{CIFXGlyphLineToBlock_Factory}, 1
	},
	{
		&CID_IFXGlyphMoveToBlock,
		{CIFXGlyphMoveToBlock_Factory}, 1
	},
	{
		&CID_IFXGlyphTagBlock,
		{CIFXGlyphTagBlock_Factory}, 1
	},
	{
		&CID_IFXGroup,
		{CIFXGroup_Factory}, 1
	},
	{
		&CID_IFXHashMap,
		{CIFXHashMap_Factory}, 1
	},
	{
		&CID_IFXIDManager,
		{CIFXIDManagerFactory}, 1
	},
	{
		&CID_IFXImageCodec,
		{CIFXImageTools_Factory}, 1
	},
	{
		&CID_IFXLight,
		{CIFXLight_Factory}, 1
	},
	{
		&CID_IFXLightResource,
		{CIFXLightResource_Factory}, 1
	},
	{
		&CID_IFXViewResource,
		{CIFXViewResource_Factory}, 1
	},
	{
		&CID_IFXLightSet,
		{CIFXLightSet_Factory}, 1
	},
	{
		&CID_IFXMesh,
		{CIFXMeshFactory}, 1
	},
	{
		&CID_IFXMeshGroup,
		{CIFXMeshGroupFactory},1 
	},
	{
		&CID_IFXMetaDataX,
		{CIFXMetaData_Factory}, 1
	},
	{
		&CID_IFXInterleavedData,
		{CIFXInterleavedDataFactory}, 1
	},
	{
		&CID_IFXNeighborMesh,
		{CIFXNeighborMeshFactory}, 1
	},
	{
		&CID_IFXCLODModifier,
		{CIFXCLODModifier_Factory}, 1
	},
	{
		&CID_IFXMixerConstruct,
		{CIFXMixerConstruct_Factory}, 1
	},
	{
		&CID_IFXModifierChain,
		{CIFXModifierChain_Factory}, 1
	},
	{
		&CID_IFXShaderList,
		{CIFXShaderList_Factory}, 1
	},
	{
		&CID_IFXMaterialResource,
		{CIFXMaterialResource_Factory}, 1
	},
	{
		&CID_IFXModel,
		{CIFXModel_Factory}, 1
	},
	{
		&CID_IFXGlyph2DModifier,
		{CIFXGlyph2DModifier_Factory}, 1
	},
	{
		&CID_IFXMotionResource,
		{CIFXMotionResource_Factory}, 1
	},
	{
		&CID_IFXPalette,
		{CIFXPalette_Factory}, 1
	},
	{
		&CID_IFXPerformanceTimer,
		{CIFXPerformanceTimer_Factory}, 1
	},
	{
		&CID_IFXPickObject,
		{CIFXPickObject_Factory}, 1
	},
	{
		&CID_IFXSimpleHash,
		{CIFXSimpleHashFactory},1
	},
	{
		&CID_IFXSimpleList,
		{CIFXSimpleList_Factory}, 1
	},
	{
		&CID_IFXSimpleObject,
		{CIFXSimpleObject_Factory}, 1
	},
	{
		&CID_IFXSceneGraph,
		{CIFXSceneGraph_Factory}, 1
	},
	{
		&CID_IFXAnimationModifier,
		{CIFXAnimationModifier_Factory}, 1
	},
	{
		&CID_IFXSimpleCollection,
		{CIFXSimpleCollection_Factory}, 1
	},
	{
		&CID_IFXSubdivModifier,
		{CIFXSubdivModifier_Factory}, 1
	},
	{
		&CID_IFXTextureImageTools,
		{CIFXTextureImageTools_Factory}, 1
	},
	{
		&CID_IFXTextureObject,
		{CIFXTextureObject_Factory}, 1
	},
	{
		&CID_IFXUVGenerator,
		{CIFXUVGenerator_Factory}, 1
	},
	{
		&CID_IFXUVMapperCylindrical,
		{CIFXUVMapperCylindrical_Factory}, 1
	},
	{
		&CID_IFXUVMapperNone,
		{CIFXUVMapperNone_Factory}, 1
	},
	{
		&CID_IFXUVMapperPlanar,
		{CIFXUVMapperPlanar_Factory}, 1
	},
	{
		&CID_IFXUVMapperSpherical,
		{CIFXUVMapperSpherical_Factory}, 1
	},
	{
		&CID_IFXUVMapperReflection,
		{CIFXUVMapperReflection_Factory}, 1
	},
	{
		&CID_IFXView,
		{CIFXView_Factory}, 1
	},
	{
		&CID_IFXVoidWrapper,
		{CIFXVoidWrapper_Factory}, 1
	},
	{
		&CID_IFXDidRegistry,
		{CIFXDidRegistry_Factory}, 1
	},
	{
		&CID_IFXShadingModifier,
		{CIFXShadingModifier_Factory}, 1
	},
	{
		&CID_IFXDummyModifier,
		{CIFXDummyModifier_Factory}, 1
	},
	{
		&CID_IFXShaderLitTexture,
		{CIFXShaderLitTexture_Factory}, 1
	}
};

/**
	Count of the number of IFXComponentDescriptor structures contained in
	gsComponentDescriptorList.
*/
U32 g_coreComponentNumber =
  sizeof( g_coreComponentDescriptorList ) / sizeof( IFXComponentDescriptor );
