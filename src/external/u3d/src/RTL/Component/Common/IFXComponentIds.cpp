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
	@file	IFXComponentIds.cpp

	This file actually declares all of the IFXCOM interface and
	component identifiers that are exposed by the core IFX engine DLL. It
	should be compiled once per project. This should be accomplished by
	adding it to the make file.

	@note
	Interface identifiers are declared here even though they are defined
	in the header that actually defines the interface.  Ultimately there
	really needs to be a separate public H and CPP file for each interface
	and another set for the components.  However since there aren't initially
	going to be IFXCOM plug-in DLL besides the engine, a plethora of
	files is probably unnecessary.
*/


//***************************************************************************
//  Includes
//***************************************************************************

#define IFX_INIT_GUID

#include "IFXGUID.h"

#include "IFXExportingCIDs.h"
#include "IFXImportingCIDs.h"
#include "IFXCoreCIDs.h"

#include "IFXAnimationModifier.h"
#include "IFXAuthorLineSet.h"
#include "IFXAuthorLineSetResource.h"
#include "IFXAuthorLineSetAccess.h"
#include "IFXAuthorLineSetAnalyzer.h"
#include "IFXAuthorPointSet.h"
#include "IFXAuthorPointSetResource.h"
#include "IFXAuthorPointSetAccess.h"
#include "IFXAuthorMesh.h"
#include "IFXAuthorCLODGen.h"
#include "IFXAuthorMeshMap.h"
#include "IFXAuthorCLODMesh.h"
#include "IFXAuthorCLODAccess.h"
#include "IFXAuthorMeshScrub.h"
#include "IFXAuthorCLODResource.h"
#include "IFXBoneWeightsModifier.h"
#include "IFXBitStreamX.h"
#include "IFXBitStreamCompressedX.h"
#include "IFXBoundHierarchy.h"
#include "IFXBoundHierarchyMgr.h"
#include "IFXBoundSphereDataElement.h"
#include "IFXCollection.h"
#include "IFXConnection.h"
#include "IFXConnectionServer.h"
#include "IFXContour.h"
#include "IFXContourGenerator.h"
#include "IFXContourExtruder.h"
#include "IFXContourTessellator.h"
#include "IFXCoreServices.h"
#include "IFXCoreServicesRef.h"
#include "IFXDataBlock.h"
#include "IFXDataBlockX.h"
#include "IFXDataBlockQueueX.h"
#include "IFXDevice.h"
#include "IFXDidRegistry.h"
#include "IFXDummyModifier.h"
#include "IFXNameMap.h"
#include "IFXFileReference.h"
#include "IFXGenerator.h"
#include "IFXGlyph3DGenerator.h"
#include "IFXGlyph2DCommands.h"
#include "IFXGlyphCommandList.h"
#include "IFXHashMap.h"
#include "IFXIDManager.h"
#include "IFXImageCodec.h"
#include "IFXLight.h"
#include "IFXLightSet.h"
#include "IFXCLODModifier.h"
#include "IFXCLODManagerInterface.h"
#include "IFXMarker.h"
#include "IFXMarkerX.h"
#include "IFXMaterialResource.h"
#include "IFXMesh.h"
#include "IFXMeshGroup.h"
#include "IFXNeighborMesh.h"
#include "IFXMeshCompiler.h"
#include "IFXMeshMap.h"
#include "IFXMixerConstruct.h"
#include "IFXModifier.h"
#include "IFXGlyph2DModifier.h"
#include "IFXModifierChain.h"
#include "IFXModifierChainInternal.h"
#include "IFXModifierDataPacket.h"
#include "IFXModifierDataPacketInternal.h"
#include "IFXModel.h"
#include "IFXMotionResource.h"
#include "IFXNode.h"
#include "IFXNotificationInfo.h"
#include "IFXNotificationManager.h"
#include "IFXObserver.h"
#include "IFXPalette.h"
#include "IFXPerformanceTimer.h"
#include "IFXPickObject.h"
#include "IFXRenderable.h"
#include "IFXResourceClient.h"
#include "IFXSceneGraph.h"
#include "IFXSetX.h"
#include "IFXSetAdjacencyX.h"
#include "IFXShaderLitTexture.h"
#include "IFXShader.h"
#include "IFXShaderList.h"
#include "IFXShadingModifier.h"
#include "IFXSimpleHash.h"
#include "IFXSimpleList.h"
#include "IFXSkeletonMixer.h"
#include "IFXSpatial.h"
#include "IFXSpatialAssociation.h"
#include "IFXSpatialSetQuery.h"
#include "IFXSubject.h"
#include "IFXStdio.h"
#include "IFXSubdivModifier.h"
#include "IFXTextureObject.h"
#include "IFXUVMapper.h"
#include "IFXView.h"
#include "IFXVoidWrapper.h"
#include "IFXWriteBuffer.h"
#include "IFXWriteBufferX.h"

// interfaces used by IFXCore
#include "IFXScheduler.h"
#include "IFXDecoderX.h"
#include "IFXInet.h"
#include "IFXReadBuffer.h"
#include "IFXReadBufferX.h"
