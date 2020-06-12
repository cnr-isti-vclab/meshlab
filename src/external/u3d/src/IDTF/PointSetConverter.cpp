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
	@file PointSetConverter.cpp

	This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "PointSetConverter.h"
#include "MetaDataConverter.h"
#include "ModelNode.h"
#include "PointSetResource.h"
#include "Color.h"

#include "IFXAutoRelease.h"
#include "IFXAuthorPointSetResource.h"

using namespace U3D_IDTF;

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

PointSetConverter::PointSetConverter(
							const PointSetResource* pModelResource,
							SceneUtilities* pSceneUtils ) : 
ModelConverter( pSceneUtils ),
m_pIDTFResource( pModelResource )
{
}

PointSetConverter::~PointSetConverter()
{
	m_pIDTFResource = NULL;
}


/**
Build a model node in U3D from the IDTF representation.
*/
IFXRESULT PointSetConverter::Convert()
{
	IFXRESULT result = IFX_OK;

	IFXDECLARELOCAL( IFXAuthorPointSet,   pAuthorModel );
	IFXDECLARELOCAL( IFXAuthorPointSetResource,   pModelResource );

	// Convert the point set data for this model
	result = ConvertPointSet( &pAuthorModel );

	// Using all the above data, create an IFXAuthorPointSetResource for this model
	if( m_pSceneUtils && IFXSUCCESS( result ) )
	{
		result = m_pSceneUtils->CreatePointSetResource(
			m_pIDTFResource->GetName(),
			pAuthorModel,
			256,
			&pModelResource );
	}

	// convert point set resource meta-data
	IFXDECLARELOCAL( IFXMetaDataX, pMetaData );

	if( pModelResource && IFXSUCCESS( result ) )
		result = pModelResource->QueryInterface( IID_IFXMetaDataX, (void**)&pMetaData );

	if( IFXSUCCESS( result ) )
	{
		MetaDataConverter metaDataConverter( m_pIDTFResource, pMetaData );
		metaDataConverter.Convert();
	}

	IFXASSERT( IFXSUCCESS( result ) );
	return result;
}

//***************************************************************************
//  Protected methods
//***************************************************************************

/**
	Read high-level information about the PointSet from the IDTF file,
	such as the number of vertices, materials, and texture layers
	if will contain.

	@param	pAllocationDescriptor	High level information on the
									properties of the PointSet we 
									are about to create

	@return IFXRESULT             Return status of this method.
*/
IFXRESULT PointSetConverter::ConvertPointSetFormat(
								IFXAuthorPointSetDesc* pAllocationDescriptor )
{
	IFXRESULT result = IFX_OK;

	const ModelDescription& rDescription = m_pIDTFResource->m_modelDescription;

	if( NULL == pAllocationDescriptor )
	{
		result = IFX_E_INVALID_POINTER;
		IFXASSERT( NULL == pAllocationDescriptor );
	}

	if( IFXSUCCESS( result ) )
	{
		pAllocationDescriptor->m_numPoints = m_pIDTFResource->pointCount;
		pAllocationDescriptor->m_numPositions = rDescription.positionCount;
		pAllocationDescriptor->m_numNormals = rDescription.normalCount;
		pAllocationDescriptor->m_numTexCoords = rDescription.textureCoordCount;
		pAllocationDescriptor->m_numDiffuseColors = rDescription.diffuseColorCount;
		pAllocationDescriptor->m_numSpecularColors = rDescription.specularColorCount;
		pAllocationDescriptor->m_numMaterials = rDescription.shadingCount;
	}

	return result;
}


/**
	Convert PointSet data

	@param	ppModel						The IFXAuthorPointSet we populated
	@param	pMinimumPointSetResolution	The minimum PointSet resolution we
										will allow to be represented by
										multiresolution data.  A static
										model represents the PointSet for
										lower resolutions.

	@return IFXRESULT             Return status of this method.
*/
IFXRESULT PointSetConverter::ConvertPointSet( IFXAuthorPointSet** ppModel )
{
	IFXRESULT   result = IFX_OK;

	U32 i, j;

	IFXAuthorPointSetDesc allocationDescriptor;
	IFXAuthorMaterial*  pAuthorShaders = NULL;
	IFXAuthorPointSet*  pModel = NULL;

	if( !ppModel || !m_pIDTFResource )
	{
		result = IFX_E_INVALID_POINTER;
		IFXASSERT( IFXSUCCESS( result ) );
	}

	// Read the PointSet format (the number of points, texture coordinates,
	// shaders, etc.)
	if( IFXSUCCESS( result ) )
		result = ConvertPointSetFormat( &allocationDescriptor );

	// Create the IFXAuthorPointSet
	if( IFXSUCCESS( result ) )
		result = IFXCreateComponent( 
						CID_IFXAuthorPointSet, IID_IFXAuthorPointSet, (void**) &pModel );

	// Set up the IFXAuthorPointSet now that we know how much of what sort of
	// data it needs to store
	if( IFXSUCCESS( result ) )
		result = pModel->Allocate( &allocationDescriptor );

	if( IFXSUCCESS( result ) )
		result = pModel->GetMaterials( &pAuthorShaders );

	if( IFXSUCCESS( result ) )
		result = ConvertShadingDescriptions(
						m_pIDTFResource->m_shadingDescriptions,
						allocationDescriptor.m_numMaterials,
						pAuthorShaders );

	// Now get and store the Point Positions (index into the position array)
	if( IFXSUCCESS( result ) && allocationDescriptor.m_numPoints > 0 )
	{
		U32* pPoint = NULL;

		result = pModel->GetPositionPoints( &pPoint );

		if( IFXSUCCESS( result ) )
		{
			for( i = 0; i < allocationDescriptor.m_numPoints; ++i )
			{
				const I32& pointPosition = m_pIDTFResource->m_pointPositions.GetElementConst( i );
				pPoint[i] = pointPosition;
			}
		}
	}

	// Now get and store the Point Normals (index into the normal array)
	if( IFXSUCCESS( result ) && allocationDescriptor.m_numPoints > 0 && 
		allocationDescriptor.m_numNormals > 0 )
	{
		U32* pPoint = NULL;

		result = pModel->GetNormalPoints( &pPoint );

		if( IFXSUCCESS( result ) )
		{
			for(i = 0; i < allocationDescriptor.m_numPoints; ++i )
			{
				const I32& pointNormal = m_pIDTFResource->m_pointNormals.GetElementConst( i );
				pPoint[i] = pointNormal;
			}
		}
	}

	// Now get and store the Point TexCoords (index into the texture
	// coordinate array)
	if( IFXSUCCESS( result ) && ( allocationDescriptor.m_numTexCoords > 0 ) &&
		allocationDescriptor.m_numPoints > 0 )
	{
		U32 layers = 0;

		// loop through all points
		for( i = 0; i < allocationDescriptor.m_numPoints && IFXSUCCESS( result ); ++i )
		{
			const I32 rShadingIndex = m_pIDTFResource->m_pointShaders.GetElementConst( i );
			const ShadingDescription& rShading =
				m_pIDTFResource->m_shadingDescriptions.GetShadingDescription( rShadingIndex );
			layers = rShading.GetTextureLayerCount();

			const PointTexCoords& pointTexCoords = m_pIDTFResource->m_pointTextureCoords.GetElementConst( i );

			// texture layer loop
			// do not define texture coordinates for point if it has 0 texture layers
			for( j = 0; j < layers && IFXSUCCESS( result ); ++j )
			{
				U32* pPoint = NULL;

				// Get the texture coordinates for specified texture layer for this point
				result = pModel->GetTexPoints( j, &pPoint );

				if( IFXSUCCESS( result ) )
				{
					const I32& texCoord = pointTexCoords.m_texCoords.GetElementConst( j );
					pPoint[i] = texCoord;
				}
			}
		}
	}

	// Set up the IFXAuthorMaterial structure with the
	// initial shader to Shading mapping information.
	/** 
		@todo: define shading description attributes and move this following to the
		ModelConverter::ConvertShadingDescription 
	*/
	if( IFXSUCCESS( result ) )
	{
		for( i = 0; i < allocationDescriptor.m_numMaterials; ++i )
		{
			// NOTE TO IMPLEMENTER:  Note that this allows you to mark
			// only certain parts of the model as containing vertex colors
			// (diffuse or specular).
			if( allocationDescriptor.m_numDiffuseColors > 0)
				pAuthorShaders[i].m_uDiffuseColors = TRUE;
			else
				pAuthorShaders[i].m_uDiffuseColors = FALSE;

			if( allocationDescriptor.m_numSpecularColors > 0)
				pAuthorShaders[i].m_uSpecularColors = TRUE;
			else
				pAuthorShaders[i].m_uSpecularColors = FALSE;

			if( allocationDescriptor.m_numNormals > 0)
				pAuthorShaders[i].m_uNormals = TRUE;
			else
				pAuthorShaders[i].m_uNormals = FALSE;
		}
	}

	// Get and store the shading index for the shader used by each point
	if( IFXSUCCESS( result ) && allocationDescriptor.m_numPoints > 0 && 
		allocationDescriptor.m_numMaterials > 0 )
	{
		for( i = 0; i < allocationDescriptor.m_numPoints && IFXSUCCESS( result ); ++i )
		{
			const I32& shadingIndex = m_pIDTFResource->m_pointShaders.GetElementConst( i );
			pModel->SetPointMaterial( i, shadingIndex );
		}
	}

	// Now read and store the Point Diffuse Colors (index into the diffuse color array)
	if( IFXSUCCESS( result ) && allocationDescriptor.m_numPoints > 0 &&
		allocationDescriptor.m_numDiffuseColors > 0 )
	{
		U32* pPoint = NULL;

		// Point VertexDiffuseColors
		result = pModel->GetDiffusePoints( &pPoint );

		if( IFXSUCCESS( result ) )
		{
			for( i = 0; i < allocationDescriptor.m_numPoints; ++i )
			{
				pPoint[i] = m_pIDTFResource->m_pointDiffuseColors.GetElementConst( i );
			}
		}
	}

	// Now read and store the Point Specular Colors (index into the specular color array)
	if( IFXSUCCESS( result ) && allocationDescriptor.m_numPoints > 0 &&
		allocationDescriptor.m_numSpecularColors > 0 )
	{
		U32* pPoint = NULL;

		// Point VertexSpecularColors
		result = pModel->GetSpecularPoints( &pPoint );

		if( IFXSUCCESS( result ) )
		{
			for(i = 0; i < allocationDescriptor.m_numPoints; ++i )
			{
				pPoint[i] = m_pIDTFResource->m_pointSpecularColors.GetElementConst( i );
			}
		}
	}

	// Get and store the actual position data for the PointSet.
	if( IFXSUCCESS( result ) && allocationDescriptor.m_numPositions > 0 )
	{
		IFXVector3* pVector3 = NULL;

		result = pModel->GetPositions( &pVector3 );

		if( IFXSUCCESS( result ) )
		{
			for( i = 0; i < allocationDescriptor.m_numPositions; ++i )
			{
				const Point& point = m_pIDTFResource->m_positions.GetElementConst( i );
				pVector3[i] = point.GetPoint();
			}
		}
	}

	// Get and store the actual normal data for the PointSet.
	if( IFXSUCCESS( result ) && allocationDescriptor.m_numNormals > 0 )
	{
		IFXVector3* pVector3 = NULL;

		result = pModel->GetNormals( &pVector3 );

		if( IFXSUCCESS( result ) )
		{
			for( i = 0; i < allocationDescriptor.m_numNormals; ++i )
			{
				const Point& point = m_pIDTFResource->m_normals.GetElementConst( i );
				pVector3[i] = point.GetPoint();
			}
		}
	}

	// Get and store the actual texture coordinate data for the PointSet.
	if( IFXSUCCESS( result ) && ( allocationDescriptor.m_numTexCoords > 0 ) )
	{
		IFXVector4* pVector4 = NULL;

		// TexCoords
		result = pModel->GetTexCoords( &pVector4 );

		if( IFXSUCCESS( result ) )
		{
			for( i = 0; i < allocationDescriptor.m_numTexCoords; ++i )
				pVector4[i] = m_pIDTFResource->m_textureCoords.GetElementConst( i );
		}
	}

	// Read and store the actual vertex diffuse color data for the PointSet.
	if( IFXSUCCESS( result ) )
	{
		for( i = 0; i < allocationDescriptor.m_numDiffuseColors && IFXSUCCESS( result ); ++i )
		{
			const Color& color = m_pIDTFResource->m_diffuseColors.GetElementConst( i );
			IFXVector4 vColor4 = color.GetColor();

			// Vertex colors (diffuse and specular) are stored
			// in BGRA order.  The following call correctly converts
			// the color vector into this format.
			vColor4.BGR();
			result = pModel->SetDiffuseColor( i, &vColor4 );
		}
	}

	// Read and store the actual vertex specular color data for the PointSet.
	if( IFXSUCCESS( result ) )
	{
		for( i = 0; i < allocationDescriptor.m_numSpecularColors && IFXSUCCESS( result ); ++i )
		{
			const Color& color = m_pIDTFResource->m_specularColors.GetElementConst( i );
			IFXVector4 vColor4 = color.GetColor();

			// Vertex colors (diffuse and specular) are stored
			// in BGRA order.  The following call correctly converts
			// the color vector into this format.
			vColor4.BGR();
			result = pModel->SetSpecularColor( i, &vColor4 );
		}
	}

	if(  ppModel && IFXSUCCESS( result ) )
	{
		// Releasing pModel is left to the caller
		*ppModel = pModel;
	}

	IFXASSERT( IFXSUCCESS( result ) );
	return result;
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
