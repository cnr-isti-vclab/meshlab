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
	@file	SceneUtilities.h

			This header defines the U3D scene helpers functionality.
*/


#ifndef SceneUtilities_H
#define SceneUtilities_H

//***************************************************************************
//  Includes
//***************************************************************************

#include "U3DHeaders.h"
#include "IFXAutoRelease.h"
#include "IFXPalette.h"
#include "ParentList.h"

namespace U3D_IDTF
{
//***************************************************************************
//  Defines
//***************************************************************************


//***************************************************************************
//  Constants
//***************************************************************************


//***************************************************************************
//  Enumerations
//***************************************************************************


//***************************************************************************
//  Classes, structures and types
//***************************************************************************

/**
This is the implementation of a class that is used to create U3D scene objects.
*/
class SceneUtilities
{
public:
	// constructors/destructors
	SceneUtilities();
	virtual ~SceneUtilities();  // Scene destroyed here.

	// Scenegraph initialization
	IFXRESULT InitializeScene( U32 profile, F64 scaleFactor = 1.0 );

	// Debugging
	IFXRESULT WriteDebugInfo( const char* pFile );

	// Disk I/O
	IFXRESULT WriteSceneToFile( const IFXCHAR* pFileName,
		IFXExportOptions ExportOptions );

	IFXRESULT LoadU3DFile(  const IFXCHAR* pFileName );

	IFXRESULT CreateFileReference( 
		const IFXString& rName,
		IFXFileReference** ppFileReference );

	// Node utilities:  creation and finding
	IFXRESULT CreateNodePlaceholder( 
		const IFXString& rNodeName,
		U32* pNodeId = NULL );

	IFXRESULT CreateNode(
		const IFXString& rNodeName,
		const IFXCID& rComponentId,
		IFXNode** ppNode, 
		U32* pNodeId = NULL );

	IFXRESULT CreateViewNode(
		const IFXString& rNodeName,
		const IFXString& rResourceName,
		IFXView** ppView );

	IFXRESULT CreateGroupNode(
		const IFXString& rNodeName,
		IFXNode** ppNode );

	IFXRESULT CreateLightNode(
		const IFXString& rNodeName,
		const IFXString& rResourceName,
		IFXLight** ppLight );

	IFXRESULT CreateModelNode(
		const IFXString& rName,
		const IFXString& rResourceName,
		IFXModel** ppModel );

	IFXRESULT FindNode( 
		const IFXString& rName, 
		IFXNode** ppNode, 
		U32* pNodeId = NULL );

	// Resource utilities: creation and finding
	IFXRESULT CreateResourcePlaceholder( 
		const IFXString& rName,
		IFXSceneGraph::EIFXPalette palette,
		U32* pId = NULL );

	IFXRESULT CreateViewResource(
		const IFXString& rName,
		IFXViewResource** ppViewResource );

	IFXRESULT CreateLightResource(
		const IFXString& rName,
		IFXLightResource** ppLightResource );

	IFXRESULT CreateMeshResource(
		const IFXString& rName,
		IFXAuthorMesh*  pMesh,
		IFXAuthorGeomCompilerParams& rParams,
		IFXAuthorCLODResource** ppModel );

	IFXRESULT CreatePointSetResource(
		const IFXString& rName,
		IFXAuthorPointSet* pLineSet,
		U32 modelPriority,
		IFXAuthorPointSetResource** ppModel );

	IFXRESULT CreateLineSetResource(
		const IFXString& rName,
		IFXAuthorLineSet*  pLineSet,
		U32 uModelPriority,
		IFXAuthorLineSetResource** ppModel );

	IFXRESULT CreateModelResource(
		const IFXString& rName,
		IFXREFCID ComponentId,
		U32 uPrimitivePriority,
		IFXGenerator** ppPrimitive);

	IFXRESULT CreateMotionResource(
		const IFXString& rMotionName,
		const U32 motionQuality,
		const U32 motionPriority,
		U32*  motionID,
		IFXMotionResource** pMotionResource);

	IFXRESULT FindPaletteEntry(
		const IFXString& rName,
		IFXSceneGraph::EIFXPalette palette,
		IFXUnknown** ppResource,
		U32* pPaletteId );

	IFXRESULT FindTexture(
		const IFXString& rName,
		IFXTextureObject** ppResource,
		U32* pTextureId );

	IFXRESULT FindMaterial(
		const IFXString& rName,
		IFXMaterialResource** ppResource,
		U32* pMaterialId );

	IFXRESULT FindShader(
		const IFXString& rName,
		IFXShaderLitTexture** ppResource,
		U32* pShaderId );

	IFXRESULT FindMotionResource(
		const IFXString& rName,
		IFXMotionResource** ppResource );

	IFXRESULT FindResourceByModelName(
		const IFXString& rModelName,
		IFXAuthorCLODResource** ppAuthorCLODResource);

	IFXRESULT FindModelResource(
		const IFXString& rResourceName,
		IFXGenerator** ppModelResource );

	IFXRESULT FindModel(
		const IFXString& rModelName,
		IFXModel** ppModel);

	// Surface properties
	IFXRESULT CreateShader(
		const IFXString& rShaderName,
		const IFXString& rMaterialName,
		IFXShaderLitTexture** ppShader );

	IFXRESULT GetMaterialFromShader(
		IFXShaderLitTexture* pShaderLitTexture,
		IFXMaterialResource** ppMaterial );

	IFXRESULT GetTextureFromShader(
		IFXShaderLitTexture* pShaderLitTexture,
		IFXTextureObject** ppTexture,
		U32 layer );

	IFXRESULT CreateMaterial(
		const IFXString& rMaterial,
		IFXMaterialResource** ppMaterial );

	IFXRESULT AddTexturePaletteEntry(
		const IFXString& rName,
		U32* pTextureId );

	IFXRESULT CreateTexture(
		const IFXString& rName,
		const U32 priority,
		IFXTextureObject** ppTextureObject );

	IFXRESULT SetRenderWire( const IFXString& rShaderName, BOOL onOff );

	// Modifiers

	IFXRESULT AddModifier(
		const IFXString& rModName,
		const IFXString& rModChainType,
		IFXModifier* pModifier );

	IFXRESULT AddBoneWeightModifier(
		const IFXString& rModName,
		const IFXString& rModChainType,
		IFXBoneWeightsModifier** ppBoneWeightModifier );

	IFXRESULT AddShadingModifier(
		const IFXString& rModName,
		const IFXString& rModChainType,
		IFXShadingModifier** ppShadingModifier );

	IFXRESULT AddGlyphModifier(
		const IFXString& rModName,
		const IFXString& rModChainType,
		F64 defaultWidth,
		F64 defaultSpacing,
		F64 defaultHeight,
		IFXGlyphCommandList* pCommandList,
		IFXGlyph2DModifier** ppGlyph2DModifier );

	IFXRESULT AddCLODModifier(
		const IFXString& rModName,
		const IFXString& rModChainType,
		IFXCLODModifier** ppCLODModifier );

	IFXRESULT AddSubdivisionModifier(
		const IFXString& rModName,
		const IFXString& rModChainType,
		IFXSubdivModifier** ppSubDiv );

	// -- Animation and its utilities.
	IFXRESULT AddAnimationModifier(
		const IFXString& rModName,
		const IFXString& rModChainType,
		BOOL isKeyFrameModifier,
		IFXAnimationModifier** ppAnimationModifier );

	/**
		Queues up a named motion into an animation modifier

		@param	pAnimationModifier	The modifier to assign the
									named motion.
		@param	rMotionName			The name of the motion to assign to
									the modifier.
		@param	F32
		@param	F32
		@param	BOOL
		@param  BOOL

		@return  IFXRESULT     Return status of this method.
	*/
	IFXRESULT AssignAnimationToModifier(
		IFXAnimationModifier* pAnimationModifer,
		const IFXString& rMotionName,
		F32 rTimeOffset = 0.0, F32 rTimeScale = 1.0,
		BOOL loop = TRUE, BOOL sync = FALSE );

	IFXRESULT CompressMotionTracks( IFXMotionResource* motionResource );

	// Misc utilities
	IFXRESULT TestModifierResourceChain(
		IFXAuthorCLODResource* pCLODResource,
		IFXREFIID ModifierID,
		U32* pFound );

	IFXRESULT TestModifierNodeChain(
		IFXNode*  pNode,
		IFXREFIID ModifierID,
		U32* pFound );

	IFXRESULT GetWorldTransform(
		IFXNode* pNode,
		IFXMatrix4x4* pWorldTransform );

	/**
	Returns file meta data interface
	*/
	IFXRESULT GetSceneMetaData( IFXMetaDataX** ppMetaData );

private:
	SceneUtilities( SceneUtilities& );
	SceneUtilities& operator= ( SceneUtilities& );

	IFXDECLAREMEMBER(IFXCoreServices,m_pCoreServicesMain);///< IFX core services object
	IFXDECLAREMEMBER(IFXCoreServices,m_pCoreServices);	///< IFX core services object
	IFXDECLAREMEMBER(IFXSceneGraph,m_pSceneGraph);		///< IFX scenegraph

	IFXDECLAREMEMBER( IFXPalette, m_pShaderPalette );	///< IFX shader palette
	IFXDECLAREMEMBER(IFXPalette,m_pGeneratorPalette);	///< IFX generator palette
	IFXDECLAREMEMBER(IFXPalette,m_pNodePalette);		///< IFX node palette

	BOOL m_bInit;	///< Whether or not this object is initialized
};

//***************************************************************************
//  Inline functions
//***************************************************************************


//***************************************************************************
//  Global function prototypes
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************
}

#endif
