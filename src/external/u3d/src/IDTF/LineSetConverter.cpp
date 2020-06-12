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
@file LineSetConverter.cpp

This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "LineSetConverter.h"
#include "MetaDataConverter.h"
#include "ModelNode.h"
#include "LineSetResource.h"
#include "Color.h"
#include "Int2.h"

#include "IFXAutoRelease.h"
#include "IFXAuthorLineSetResource.h"
#include "IFXLine.h"

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

LineSetConverter::LineSetConverter(
							const LineSetResource* pModelResource,
							SceneUtilities* pSceneUtils ) : 
ModelConverter( pSceneUtils ),
m_pIDTFResource( pModelResource )
{
}

LineSetConverter::~LineSetConverter()
{
	m_pIDTFResource = NULL;
}


/**
	Build a model node in U3D from the IDTF representation.
*/
IFXRESULT LineSetConverter::Convert()
{
	IFXRESULT result = IFX_OK;

	IFXDECLARELOCAL( IFXAuthorLineSet,    pAuthorModel );
	IFXDECLARELOCAL( IFXAuthorLineSetResource,    pModelResource );

	// Convert the line set data for this model
	result = ConvertLineSet( &pAuthorModel );

	// Using all the above data, create an IFXAuthorLineSetResource for this model
	if( m_pSceneUtils && IFXSUCCESS( result ) )
	{
		result = m_pSceneUtils->CreateLineSetResource(
			m_pIDTFResource->GetName(),
			pAuthorModel,
			256,
			&pModelResource );
	}

	// convert line set resource meta-data
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
	Read high-level information about the lineSet from the IDTF file,
	such as the number of vertices, materials, and texture layers
	if will contain.

	@param	pAllocationDescriptor   High level information on the
									properties of the lineSet we are about 
									to create

	@return IFXRESULT             Return status of this method.
*/
IFXRESULT LineSetConverter::ConvertLineSetFormat( IFXAuthorLineSetDesc* pAllocationDescriptor )
{
	IFXRESULT result = IFX_OK;

	if( NULL == pAllocationDescriptor )
	{
		result = IFX_E_INVALID_POINTER;
		IFXASSERT( NULL == pAllocationDescriptor );
	}

	if( IFXSUCCESS( result ) )
	{
		const ModelDescription& rDescription = m_pIDTFResource->m_modelDescription;

		pAllocationDescriptor->m_numLines = m_pIDTFResource->lineCount;
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
	Convert lineSet data

	@param	ppModel						The IFXAuthorLineSet we populated
	@param	pMinimumLineSetResolution	The minimum lineSet resolution we
										will allow to be represented by
										multiresolution data.  A static
										model represents the lineSet for
										lower resolutions.

	@return IFXRESULT					Return status of this method.
*/
IFXRESULT LineSetConverter::ConvertLineSet( IFXAuthorLineSet**  ppModel )
{
	IFXRESULT   result = IFX_OK;

	U32 i, j;

	IFXAuthorLineSetDesc  allocationDescriptor;
	IFXAuthorMaterial*  pAuthorShaders = NULL;
	IFXAuthorLineSet* pModel = NULL;

	if( !ppModel || !m_pIDTFResource )
	{
		result = IFX_E_INVALID_POINTER;
		IFXASSERT( IFXSUCCESS( result ) );
	}

	// Read the lineSet format (the number of lines, texture coordinates,
	// shaders, etc.)
	if( IFXSUCCESS( result ) )
		result = ConvertLineSetFormat( &allocationDescriptor );

	// Create the IFXAuthorLineSet
	if( IFXSUCCESS( result ) )
		result = IFXCreateComponent( 
						CID_IFXAuthorLineSet, IID_IFXAuthorLineSet,	(void**) &pModel );

	// Set up the IFXAuthorLineSet now that we know how much of what sort of
	// data it needs to store
	if( IFXSUCCESS( result ) )
		result = pModel->Allocate( &allocationDescriptor );

	if( IFXSUCCESS( result ) )
		result = pModel->GetMaterials( &pAuthorShaders );

	if( IFXSUCCESS( result ) )
		result = ConvertShadingDescriptions(
						m_pIDTFResource->m_shadingDescriptions,
						allocationDescriptor.m_numMaterials,
						pAuthorShaders 	);

	// Now get and store the Line Positions (indices into the position array)
	if( IFXSUCCESS( result ) && allocationDescriptor.m_numLines > 0 )
	{
		IFXU32Line* pLine = NULL;

		result = pModel->GetPositionLines( &pLine );

		if( IFXSUCCESS( result ) )
		{
			for( i = 0; i < allocationDescriptor.m_numLines; ++i )
			{
				const Int2& linePosition = m_pIDTFResource->m_linePositions.GetElementConst( i );
				pLine[i].Set( linePosition.GetA(), linePosition.GetB() );
			}
		}
	}

	// Now get and store the Line Normals (indices into the normal array)
	if( IFXSUCCESS( result ) && allocationDescriptor.m_numNormals > 0 && 
		allocationDescriptor.m_numLines > 0 )
	{
		IFXU32Line* pLine = NULL;

		result = pModel->GetNormalLines( &pLine );

		if( IFXSUCCESS( result ) )
		{
			for(i = 0; i < allocationDescriptor.m_numLines; ++i )
			{
				const Int2& lineNormal = m_pIDTFResource->m_lineNormals.GetElementConst( i );
				pLine[i].Set( lineNormal.GetA(), lineNormal.GetB() );
			}
		}
	}

	// Now get and store the Line TexCoords (indices into the texture
	// coordinate array)
	if( IFXSUCCESS( result ) && ( allocationDescriptor.m_numTexCoords > 0 ) &&
		allocationDescriptor.m_numLines > 0 )
	{
		U32 layers = 0;

		// loop through all lines
		for( i = 0; i < allocationDescriptor.m_numLines && IFXSUCCESS( result ); ++i )
		{
			const I32 rShadingIndex = m_pIDTFResource->m_lineShaders.GetElementConst( i );
			const ShadingDescription& rShading =
				m_pIDTFResource->m_shadingDescriptions.GetShadingDescription( rShadingIndex );
			layers = rShading.GetTextureLayerCount();

			const LineTexCoords& lineTexCoords = m_pIDTFResource->m_lineTextureCoords.GetElementConst( i );

			// texture layer loop
			// do not define texture coordinates for line if it has 0 texture layers
			for( j = 0; j < layers && IFXSUCCESS( result ); ++j )
			{
				IFXU32Line* pLine = NULL;

				// Get the texture coordinates for specified texture layer for this line
				result = pModel->GetTexLines( j, &pLine );

				if( IFXSUCCESS( result ) )
				{
					const Int2& texCoord = lineTexCoords.m_texCoords.GetElementConst( j );
					pLine[i].Set( texCoord.GetA(), texCoord.GetB() );
				}
			}
		}
	}

	// Set up the IFXAuthorMaterial structure with the
	// initial shader to Shading mapping information.
	/** 
		@todo:	define shading description attributes and move this following to the
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

	// Get and store the shading index for the shader used by each line
	if( IFXSUCCESS( result ) && allocationDescriptor.m_numLines > 0 )
	{
		for( i = 0; i < allocationDescriptor.m_numLines && IFXSUCCESS( result ); ++i )
		{
			const I32& shadingIndex = m_pIDTFResource->m_lineShaders.GetElementConst( i );
			pModel->SetLineMaterial( i, shadingIndex );
		}
	}

	// Now read and store the Line Diffuse Colors (indices into the diffuse color array)
	if( IFXSUCCESS( result ) && allocationDescriptor.m_numLines > 0 &&
		allocationDescriptor.m_numDiffuseColors > 0 )
	{
		IFXU32Line* pLine = NULL;

		// Line VertexDiffuseColors
		result = pModel->GetDiffuseLines( &pLine );

		if( IFXSUCCESS( result ) )
		{
			for( i = 0; i < allocationDescriptor.m_numLines; ++i )
			{
				const Int2& color = m_pIDTFResource->m_lineDiffuseColors.GetElementConst( i );
				pLine[i].Set( color.GetA(), color.GetB() );
			}
		}
	}

	// Now read and store the Line Specular Colors (indices into the specular color array)
	if( IFXSUCCESS( result ) && allocationDescriptor.m_numLines > 0 &&
		allocationDescriptor.m_numSpecularColors > 0 )
	{
		IFXU32Line* pLine = NULL;

		// Line VertexSpecularColors
		result = pModel->GetSpecularLines( &pLine );

		if( IFXSUCCESS( result ) )
		{
			for(i = 0; i < allocationDescriptor.m_numLines; ++i )
			{
				const Int2& color = m_pIDTFResource->m_lineSpecularColors.GetElementConst( i );
				pLine[i].Set( color.GetA(), color.GetB() );
			}
		}
	}

	// Get and store the actual position data for the lineSet.
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

	// Get and store the actual normal data for the lineSet.
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

	// Get and store the actual texture coordinate data for the lineSet.
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

	// Read and store the actual vertex diffuse color data for the lineSet.
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

	// Read and store the actual vertex specular color data for the lineSet.
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
