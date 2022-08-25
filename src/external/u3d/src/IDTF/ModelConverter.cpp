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
  @file ModelConverter.cpp

      This module defines common functionality for models conversion.
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "ModelConverter.h"
#include "MetaDataConverter.h"
#include "ModelNode.h"
#include "ModelSkeleton.h"

using namespace U3D_IDTF;

//***************************************************************************
//  Defines
//***************************************************************************


//***************************************************************************
//  Constants
//***************************************************************************

const U32 DEFAULT_QUALITY_FACTOR = 1000;

//***************************************************************************
//  Enumerations
//***************************************************************************


//***************************************************************************
//  Classes, structures and types
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************


//***************************************************************************
//  Local data
//***************************************************************************


//***************************************************************************
//  Local function prototypes
//***************************************************************************


//***************************************************************************
//  Public methods
//***************************************************************************

ModelConverter::ModelConverter( SceneUtilities* pSceneUtils )
: m_pSceneUtils( pSceneUtils )
{
	m_defaultGeoQuality = DEFAULT_QUALITY_FACTOR;
	m_positionQuality = DEFAULT_QUALITY_FACTOR;
	m_texCoordQuality = DEFAULT_QUALITY_FACTOR;
	m_normalQuality = DEFAULT_QUALITY_FACTOR;
	m_diffuseQuality = DEFAULT_QUALITY_FACTOR;
	m_specularQuality = DEFAULT_QUALITY_FACTOR;

	m_excludeNormals = FALSE;
}

ModelConverter::~ModelConverter()
{
	m_pSceneUtils = NULL;
}


/**
	Build a model node in U3D from the IDTF representation.
*/
IFXRESULT ModelConverter::Convert()
{
	return IFX_OK;
}

void ModelConverter::SetDefaultQuality( U32 defaultGeoQuality )
{
	m_defaultGeoQuality = defaultGeoQuality;
}

void ModelConverter::SetPositionQuality( U32 positionQuality )
{
	m_positionQuality = positionQuality;
}

void ModelConverter::SetTexCoordQuality( U32 texCoordQuality )
{
	m_texCoordQuality = texCoordQuality;
}

void ModelConverter::SetNormalQuality( U32 normalQuality )
{
	m_normalQuality = normalQuality;
}

void ModelConverter::SetDiffuseColorQuality( U32 diffuseQuality )
{
	m_diffuseQuality = diffuseQuality;
}

void ModelConverter::SetSpecularColorQuality( U32 specularQuality )
{
	m_specularQuality = specularQuality;
}

void ModelConverter::SetZeroAreaFacesRemoval( BOOL isRemove )
{
	m_removeZeroAreaFaces = isRemove;
}

void ModelConverter::SetZeroAreaFaceTolerance( F32 tolerance )
{
	m_zeroAreaFaceTolerance = tolerance;
}

void ModelConverter::SetNormalsExclusion( BOOL excludeNormals )
{
	m_excludeNormals = excludeNormals;
}

//***************************************************************************
//  Protected methods
//***************************************************************************

IFXRESULT ModelConverter::ConvertShadingDescriptions( 
										const ShadingDescriptionList& rShadingDescriptions,
										const U32 numberOfShaders,
										IFXAuthorMaterial* pShaders )
{
	IFXRESULT result = IFX_OK;
	//const U32 count = rShadingDescriptions.GetShadingDescriptionCount();

	if( NULL == pShaders )
	{
		result = IFX_E_INVALID_POINTER;
		IFXASSERT( IFXRESULT( result ) );
	}

	if( IFXSUCCESS( result ) )
	{
		// Loop thru all shaders used by this model.
		U32 i;
		for( i = 0; i < numberOfShaders; ++i )
		{
			const ShadingDescription& rShading = 
				rShadingDescriptions.GetShadingDescription( i );

			// Get the number of texture layers used by the shader
			const U32 layerCount = rShading.GetTextureLayerCount();
			IFXASSERT( IFX_MAX_TEXUNITS >= layerCount );

			pShaders[i].m_uNumTextureLayers = layerCount;

			// Collect the dimensions of the texture coordinates used by each
			// texture layer in this shader.
			U32 k;
			for( k = 0; k < layerCount; ++k )
			{
				// load the layer dimension data
				const U32& texDim = rShading.GetTextureCoordDimention( k );
				pShaders[i].m_uTexCoordDimensions[k] = texDim;
			}

			// Set the original mapping
			pShaders[i].m_uOriginalMaterialID = rShading.m_shaderId;
		} 
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}



IFXRESULT ModelConverter::ConvertSkeleton( const ModelSkeleton& rIDTFSkeleton,
										   IFXSkeleton** ppSkeleton )
{
	IFXRESULT result = IFX_OK;
	IFXSkeleton* pSkeleton = NULL;

	if( !ppSkeleton )
	{
		result = IFX_E_INVALID_POINTER;
		IFXASSERT( IFXSUCCESS( result ) );
	}

	// create skeleton component
	if( IFXSUCCESS( result ) )
		result = IFXCreateComponent( CID_IFXSkeleton, IID_IFXSkeleton, (void**)&pSkeleton );

	// Load the reference data and animation for each bone in the skeleton.
	if( IFXSUCCESS( result ) )
	{
		U32 i;
		for( i = 0; i < rIDTFSkeleton.GetBoneInfoCount() && IFXSUCCESS( result ); ++i )
			result = ConvertBone( pSkeleton, i, rIDTFSkeleton.GetBoneInfo( i ) );
	}

	if( pSkeleton && IFXSUCCESS( result ) )
		*ppSkeleton = pSkeleton; // releasing the ppSkeleton is left to the caller

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}


IFXRESULT ModelConverter::ConvertBone(
								IFXSkeleton* pSkeleton, 
								U32 boneIndex,
								const BoneInfo& rIDTFBoneInfo )
{
	IFXRESULT	result = IFX_OK;
	IFXBoneInfo	boneInfo;

	if( !pSkeleton )
	{
		result = IFX_E_INVALID_POINTER;
		IFXASSERT( IFXSUCCESS( result ) );
	}

	// Get the bone name
	if( IFXSUCCESS( result ) )
		boneInfo.stringBoneName = rIDTFBoneInfo.name;

	// Get the name of the parent bone
	// NOTE!  For this to work the parent bone must have been loaded
	//		  before any of the children that use it.
	if( IFXSUCCESS( result ) )
	{
		// Record the ID of the parent bone
		boneInfo.iParentBoneID = GetBoneIdx( pSkeleton, rIDTFBoneInfo.parentName );
	}

	// Set the initial length of the bone
	if( IFXSUCCESS( result ) )
		boneInfo.fBoneLength = rIDTFBoneInfo.length;

	// Is displacement from the tip of its parent
	if( IFXSUCCESS( result ) )
		boneInfo.v3BoneDisplacement = rIDTFBoneInfo.displacement.GetPoint();

	// The bone's rotation relative to the X-axis of its parent.
	if( IFXSUCCESS( result ) )
		boneInfo.v4BoneRotation = rIDTFBoneInfo.orientation.GetQuat();

	// We now have collected all necessary information for this bone - 
	// send these data to the BonesGenerator.
	if( IFXSUCCESS( result ) )
		result = pSkeleton->SetBoneInfo( boneIndex, &boneInfo );

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}


I32 ModelConverter::GetBoneIdx( 
							IFXSkeleton* pSkeleton, 
							const IFXString& rName )
{
	IFXRESULT result = IFX_OK;
	U32       boneCount = 0;
	IFXBoneInfo   boneInfo;

	if( !pSkeleton )
	{
		IFXASSERT( pSkeleton != 0 );
		return -1;
	}

	result = pSkeleton->GetNumBones( boneCount );

	if( IFXSUCCESS( result ) )
	{
		U32 i;
		for( i = 0; i < boneCount; ++i )
		{
			result = pSkeleton->GetBoneInfo( i, &boneInfo );

			if( !( boneInfo.stringBoneName.Compare( &rName ) ) 
				&& IFXSUCCESS( result ) )
			{
				return (I32)i;
			}
		}
	}

	return -1;
}

//***************************************************************************
//  Private methods
//***************************************************************************


//***************************************************************************
//  Global functions
//***************************************************************************


//***************************************************************************
//  Local functions
//***************************************************************************
