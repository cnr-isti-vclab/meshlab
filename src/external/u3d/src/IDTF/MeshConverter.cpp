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
	@file	MeshConverter.cpp

	This module defines MeshConverter fuctionality.
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "MeshConverter.h"
#include "MetaDataConverter.h"
#include "ModelNode.h"
#include "MeshResource.h"
#include "Color.h"

#ifndef LIBIDTF
extern FILE *stdmsg;
#endif

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

MeshConverter::MeshConverter(
							 const MeshResource* pModelResource,
							 SceneUtilities* pSceneUtils )
: ModelConverter( pSceneUtils ),
  m_pIDTFResource( pModelResource )
{
}

MeshConverter::~MeshConverter()
{
}


/**
	Build a model node in U3D from the IDTF representation.
*/
IFXRESULT MeshConverter::Convert()
{
	IFXRESULT  result = IFX_OK;

	IFXDECLARELOCAL( IFXAuthorCLODResource, pModelResource );
	IFXDECLARELOCAL( IFXAuthorMesh, pAuthorMesh );
	IFXDECLARELOCAL( IFXSkeleton, pSkeleton );
	const IFXAuthorMeshDesc* pMeshDesc = NULL;

	U32 minimumMeshResolution = 0;

	// Convert the mesh data for this model
	result = ConvertMesh( &pAuthorMesh, &minimumMeshResolution );

	if( IFXSUCCESS( result ) )
		pMeshDesc = pAuthorMesh->GetMaxMeshDesc();

	if( NULL == pMeshDesc )
		result = IFX_E_INVALID_POINTER;

	// Using all the above data, create an IFXAuthorCLODResource for this model
	if( IFXSUCCESS( result ) )
	{
		// Compiler parameters
		IFXAuthorGeomCompilerParams aparams;

		// CLOD Compression parameters
		IFXAuthorCLODCompressionParams cparams;

		// Scrubber parameters
		IFXAuthorMeshScrubParams sparams;

		// Set exclude normals if no normals are provided
		if( 0 == pMeshDesc->NumNormals || TRUE == m_excludeNormals )
		{
			cparams.bExcludeNormals = TRUE;
			cparams.bSetExcludeNormals = TRUE;
			aparams.CLODParams.NormalsMode = NoNormals;
		}

		// check if CLOD generation is needed
		if( pMeshDesc->NumBaseVertices != pMeshDesc->NumPositions )
		{
			// set CLOD compression parameters

			// Set the minimum mesh resolution if necessary
			if( minimumMeshResolution > 0)
			{
				cparams.bSetMinimumResolution = TRUE;
				cparams.uMinimumResolution = minimumMeshResolution;
			}

			// Set the geometry quality
			cparams.bSetDefaultQuality = TRUE;
			cparams.uDefaultQuality = m_defaultGeoQuality;
			cparams.bSetPositionQuality = TRUE;
			cparams.uPositionQuality = m_positionQuality;
			cparams.bSetTexCoordQuality = TRUE;
			cparams.uTexCoordQuality = m_texCoordQuality;
			cparams.bSetNormalQuality = TRUE;
			cparams.uNormalQuality = m_normalQuality;
			cparams.bSetDiffuseQuality = TRUE;
			cparams.uDiffuseQuality = m_diffuseQuality;
			cparams.bSetSpecularQuality = TRUE;
			cparams.uSpecularQuality = m_specularQuality;

			// Set the streaming priority
			cparams.bSetStreamingPriority = TRUE;
			cparams.uStreamingPriority = 256;

			aparams.CompressParams = cparams;
			aparams.bCompressSettings = TRUE;
			aparams.bCLOD = TRUE;
		}
		else
		{
			// Exclude CLOD generation if only base mesh is needed
			aparams.CompressParams = cparams;
			aparams.bCompressSettings = TRUE;
			aparams.bCLOD = FALSE;
		}

		// Set Scrubber parameters
		// Set zero area removal and tolerance
		sparams.bZeroAreaFaces = m_removeZeroAreaFaces;
		sparams.ZeroAreaFaceTolerance = m_zeroAreaFaceTolerance;

		aparams.ScrubParams = sparams;
		aparams.bScrub = TRUE;

		if( NULL != m_pSceneUtils )
		{
			result = m_pSceneUtils->CreateMeshResource(
									m_pIDTFResource->GetName(),
									pAuthorMesh,
									aparams,
									&pModelResource );
		}
		else
			result = IFX_E_NOT_INITIALIZED;
	}

	// Convert any bone data associated with the model, and create a motion
	// resource which holds the skeletal bones animation for this model.
	// NOTE:  This call arbitrarily assigns the motion the name of the model.
	//        There is no reason you cannot give the motion a unique name.
	//
	if( IFXSUCCESS( result ) )
		result = ConvertSkeleton( m_pIDTFResource->m_modelSkeleton, &pSkeleton );

	if( pModelResource && IFXSUCCESS( result ) )
		result = pModelResource->SetBones( pSkeleton );

	// convert mesh resource meta-data
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
	Convert high-level information about the mesh from the IDTF file,
	such as the number of vertices, materials, and texture layers
	if will contain.

	@param	pAllocationDescriptor	High level information on the
									properties of the mesh we are 
									about to create

	@return IFXRESULT             Return status of this method.
*/
IFXRESULT MeshConverter::ConvertMeshFormat(
							IFXAuthorMeshDesc* pAllocationDescriptor,
							U32* pMinimumMeshResolution )
{
	IFXRESULT result = IFX_OK;

	if( NULL == pAllocationDescriptor || NULL == pMinimumMeshResolution )
	{
		result = IFX_E_INVALID_POINTER;
		IFXASSERT( IFXSUCCESS( result ) );
	}

	if(  IFXSUCCESS( result ) )
	{
		const ModelDescription& rDescription = m_pIDTFResource->m_modelDescription;

		pAllocationDescriptor->NumFaces = m_pIDTFResource->faceCount;
		pAllocationDescriptor->NumPositions = rDescription.positionCount;
		pAllocationDescriptor->NumNormals = rDescription.normalCount;
		pAllocationDescriptor->NumTexCoords = rDescription.textureCoordCount;
		pAllocationDescriptor->NumDiffuseColors = rDescription.diffuseColorCount;
		pAllocationDescriptor->NumSpecularColors = rDescription.specularColorCount;
		pAllocationDescriptor->NumBaseVertices = rDescription.basePositionCount;
		pAllocationDescriptor->NumMaterials = rDescription.shadingCount;

		// Optionally set CLOD parameters
		if( 0 != rDescription.basePositionCount )
		{
			// Only part of mesh has CLOD updates
			*pMinimumMeshResolution = rDescription.basePositionCount;
		}
		else
		{
			// Entire mesh has CLOD updates
			*pMinimumMeshResolution = 0;
		}
	}

	IFXASSERT( IFXSUCCESS( result ) );
	return result;
}


/**
	Convert mesh data

	@param	ppMesh					The IFXAuthorMesh we populated
	@param	pMinimumMeshResolution	The minimum mesh resolution we
									will allow to be represented by
									multiresolution data.  A static
									model represents the mesh for
									lower resolutions.

	@return IFXRESULT           Return status of this method.
*/
IFXRESULT MeshConverter::ConvertMesh( IFXAuthorMesh** ppMesh,
									  U32*        pMinimumMeshResolution )
{
	IFXRESULT   result = IFX_OK;

	U32       i, j;
	IFXVector3*   pVector3 = NULL;
	IFXVector4*   pVector4 = NULL;
	U32*      pShadingIndices = 0;

	IFXAuthorMeshDesc allocationDescriptor;
	IFXAuthorMaterial*  pAuthorShaders = NULL;
	IFXAuthorMesh*    pMesh = NULL;

	if( !ppMesh || !m_pIDTFResource )
	{
		result = IFX_E_INVALID_POINTER;
		IFXASSERT( IFXSUCCESS( result ) );
	}

	// Read the mesh format (the number of faces, texture coordinates,
	// shaders, etc.)
	if( IFXSUCCESS( result ) )
		result = ConvertMeshFormat( &allocationDescriptor, pMinimumMeshResolution );

	// Create the IFXAuthorMesh
	if( IFXSUCCESS( result ) )
		result = IFXCreateComponent( 
						CID_IFXAuthorMesh, IID_IFXAuthorMesh, (void**) &pMesh );

	// Set up the IFXAuthorMesh now that we know how much of what sort of
	// data it needs to store
	if( IFXSUCCESS( result ) )
		result = pMesh->Allocate( &allocationDescriptor );

	// Lock the IFXAuthorMesh so that we can change its properties
	if( IFXSUCCESS( result ) )
		result = pMesh->Lock();

	if( IFXSUCCESS( result ) )
		result = pMesh->GetMaterials( &pAuthorShaders );

	if( IFXSUCCESS( result ) )
		result = ConvertShadingDescriptions(
						m_pIDTFResource->m_shadingDescriptions,
						allocationDescriptor.NumMaterials,
						pAuthorShaders );

	// Now get and store the Face Positions (indices into the position array)
	if( IFXSUCCESS( result ) && allocationDescriptor.NumFaces > 0 )
	{
		IFXAuthorFace* pFace = NULL;
		result = pMesh->GetPositionFaces( &pFace );

		if( NULL != pFace )
		{
			for(i = 0; i < allocationDescriptor.NumFaces; ++i )
			{
				const Int3& facePosition = 
					m_pIDTFResource->m_facePositions.GetElementConst( i );
				pFace[i].VertexA() = facePosition.GetA();
				pFace[i].VertexB() = facePosition.GetB();
				pFace[i].VertexC() = facePosition.GetC();
			}
		}
	}

	// Now get and store the Face Normals (indices into the normal array)
	if( IFXSUCCESS( result ) && allocationDescriptor.NumNormals > 0 && 
		allocationDescriptor.NumFaces > 0 )
	{
		IFXAuthorFace* pFace = NULL;
		result = pMesh->GetNormalFaces( &pFace );

		if( NULL != pFace )
		{
			for(i = 0; i < allocationDescriptor.NumFaces; ++i )
			{
				const Int3& faceNormal = 
					m_pIDTFResource->m_faceNormals.GetElementConst( i );
				pFace[i].VertexA() = faceNormal.GetA();
				pFace[i].VertexB() = faceNormal.GetB();
				pFace[i].VertexC() = faceNormal.GetC();
			}
		}
	}

	// Now get and store the Face TexCoords (indices into the texture
	// coordinate array)
	if( IFXSUCCESS( result ) && ( allocationDescriptor.NumTexCoords > 0 ) &&
		allocationDescriptor.NumFaces > 0 )
	{
		U32 layers = 0;

		// loop through all faces
		for( i = 0; i < allocationDescriptor.NumFaces && IFXSUCCESS( result ); ++i )
		{
			const I32 rShadingIndex = m_pIDTFResource->m_faceShaders.GetElementConst( i );
			const ShadingDescription& rShading =
				m_pIDTFResource->m_shadingDescriptions.GetShadingDescription( rShadingIndex );
			layers = rShading.GetTextureLayerCount();

			const FaceTexCoords& faceTexCoords = 
				m_pIDTFResource->m_faceTextureCoords.GetElementConst( i );

			IFXAuthorFace* pFace = NULL;

			// texture layer loop
			// do not define texture coordinates for face if it has 0 texture layers
			for( j = 0; j < layers && IFXSUCCESS( result ); ++j )
			{
				// Get the texture coordinates for specified texture layer for this face
				result = pMesh->GetTexFaces( j, &pFace );

				if( NULL != pFace )
				{
					const Int3& texCoord = faceTexCoords.m_texCoords.GetElementConst( j );
					pFace[i].VertexA() = texCoord.GetA();
					pFace[i].VertexB() = texCoord.GetB();
					pFace[i].VertexC() = texCoord.GetC();
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
	if( pAuthorShaders && IFXSUCCESS( result ) )
	{
		for( i = 0; i < allocationDescriptor.NumMaterials; ++i )
		{
			// NOTE TO IMPLEMENTER:  Note that this allows you to mark
			// only certain parts of the model as containing vertex colors
			// (diffuse or specular).
			if( allocationDescriptor.NumDiffuseColors > 0)
				pAuthorShaders[i].m_uDiffuseColors = TRUE;
			else
				pAuthorShaders[i].m_uDiffuseColors = FALSE;

			if( allocationDescriptor.NumSpecularColors > 0)
				pAuthorShaders[i].m_uSpecularColors = TRUE;
			else
				pAuthorShaders[i].m_uSpecularColors = FALSE;

			if( allocationDescriptor.NumNormals > 0)
				pAuthorShaders[i].m_uNormals = TRUE;
			else
				pAuthorShaders[i].m_uNormals = FALSE;
		}
	}

	// Get and store the Shading index for the shader used by each face
	if( IFXSUCCESS( result ) && allocationDescriptor.NumFaces > 0 ) // Face FaceShaders
	{
		result = pMesh->GetFaceMaterials( &pShadingIndices );

		if( NULL != pShadingIndices )
		{
			for( i = 0; i < allocationDescriptor.NumFaces; ++i )
			{
				pShadingIndices[i] = m_pIDTFResource->m_faceShaders.GetElementConst( i );
			}
		}
	}

	// Now read and store the Face Diffuse Colors (indices into the diffuse color array)
	if( IFXSUCCESS( result ) && allocationDescriptor.NumFaces > 0  &&
		allocationDescriptor.NumDiffuseColors > 0 )
	{
		IFXAuthorFace* pFace = NULL;

		// Face VertexDiffuseColors
		result = pMesh->GetDiffuseFaces( &pFace );

		if( NULL != pFace )
		{
			for( i = 0; i < allocationDescriptor.NumFaces; ++i )
			{
				const Int3& color = m_pIDTFResource->m_faceDiffuseColors.GetElementConst( i );
				pFace[i].VertexA() = color.GetA();
				pFace[i].VertexB() = color.GetB();
				pFace[i].VertexC() = color.GetC();
			}
		}
	}

	// Now read and store the Face Specular Colors (indices into the specular color array)
	if( IFXSUCCESS( result ) && allocationDescriptor.NumFaces > 0 &&
		allocationDescriptor.NumSpecularColors > 0 )
	{
		IFXAuthorFace* pFace = NULL;

		// Face VertexSpecularColors
		result = pMesh->GetSpecularFaces( &pFace );

		if( NULL != pFace )
		{
			for(i = 0; i < allocationDescriptor.NumFaces; ++i )
			{
				const Int3& color = m_pIDTFResource->m_faceSpecularColors.GetElementConst( i );
				pFace[i].VertexA() = color.GetA();
				pFace[i].VertexB() = color.GetB();
				pFace[i].VertexC() = color.GetC();
			}
		}
	}

	// Get and store the actual position data for the mesh.
	if( IFXSUCCESS( result ) && allocationDescriptor.NumPositions > 0 )
	{
		result = pMesh->GetPositions( &pVector3 );

		if( NULL != pVector3 )
		{
			for( i = 0; i < allocationDescriptor.NumPositions; ++i )
			{
				const Point& point = m_pIDTFResource->m_positions.GetElementConst( i );
				pVector3[i] = point.GetPoint();
			}
		}
	}

	// Get and store the actual normal data for the mesh.
	if( IFXSUCCESS( result ) && allocationDescriptor.NumNormals > 0 )
	{
		result = pMesh->GetNormals( &pVector3 );

		if( NULL != pVector3 )
		{
			for( i = 0; i < allocationDescriptor.NumNormals; ++i )
			{
				const Point& point = m_pIDTFResource->m_normals.GetElementConst( i );
				pVector3[i] = point.GetPoint();
			}
		}
	}

	// Get and store the actual texture coordinate data for the mesh.
	if( IFXSUCCESS( result ) && ( allocationDescriptor.NumTexCoords > 0 ) )
	{
		// TexCoords
		result = pMesh->GetTexCoords( &pVector4 );

		if( NULL != pVector4 )
		{
			for( i = 0; i < allocationDescriptor.NumTexCoords; ++i )
				pVector4[i] = m_pIDTFResource->m_textureCoords.GetElementConst( i );
		}
	}

	// Read and store the actual vertex diffuse color data for the mesh.
	if( IFXSUCCESS( result ) && (allocationDescriptor.NumDiffuseColors > 0) )
	{
		for( i = 0; i < allocationDescriptor.NumDiffuseColors && IFXSUCCESS( result ); ++i )
		{
			const Color& color = m_pIDTFResource->m_diffuseColors.GetElementConst( i );
			IFXVector4 vColor4 = color.GetColor();

			// Vertex colors (diffuse and specular) are stored
			// in BGRA order.  The following call correctly converts
			// the color vector into this format.
			vColor4.BGR();
			result = pMesh->SetDiffuseColor( i, &vColor4 );
		}
	}

	// Read and store the actual vertex specular color data for the mesh.
	if( IFXSUCCESS( result ) && (allocationDescriptor.NumSpecularColors > 0) )
	{
		for( i = 0; i < allocationDescriptor.NumSpecularColors && IFXSUCCESS( result ); ++i )
		{
			const Color& color = m_pIDTFResource->m_specularColors.GetElementConst( i );
			IFXVector4 vColor4 = color.GetColor();

			// Vertex colors (diffuse and specular) are stored
			// in BGRA order.  The following call correctly converts
			// the color vector into this format.
			vColor4.BGR();
			result = pMesh->SetSpecularColor( i, &vColor4 );
		}
	}

	// Read and store the indices of the mesh base vertices (those vertices
	// you want to go away last as the LOD of the model falls towards zero)
	if( IFXSUCCESS( result ) && (allocationDescriptor.NumBaseVertices > 0) )
	{
		for( i = 0; i < allocationDescriptor.NumBaseVertices && IFXSUCCESS( result ); ++i )
		{
			const I32& rBaseVertexIndex = m_pIDTFResource->m_basePositions.GetElementConst( i );
			result = ( pMesh->SetBaseVertex( i, rBaseVertexIndex ) );
		}
	}

	// Unlock the IFXAuthorMesh now that we have finished changing its properties.
	if( pMesh )
	{
		IFXRESULT unlockStatus = IFX_OK;

		unlockStatus = pMesh->Unlock();

		if( IFXFAILURE( unlockStatus ) )
#ifndef LIBIDTF
			fprintf(stdmsg, " - Failure unlocking mesh\n");
#else
			IFXTRACE_GENERIC( L"[Converter] - Failure unlocking mesh\n");
#endif
	}

	if(  ppMesh && IFXSUCCESS( result ) )
	{
		// Releasing pMesh is left to the caller
		*ppMesh = pMesh;
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
