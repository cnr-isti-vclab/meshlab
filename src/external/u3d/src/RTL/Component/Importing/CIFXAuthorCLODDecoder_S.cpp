//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
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
//
//  CIFXAuthorCLODDecoder_Static.cpp
//
//  DESCRIPTION
//
//  NOTES
//
//***************************************************************************

#include "CIFXAuthorCLODDecoder.h"
#include "IFXCoreCIDs.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"
#include "IFXException.h"
#include "IFXACContext.h"
#include "IFXExportingCIDs.h"

//extern void DumpAuthorCLODMeshX(const char* pFileName, IFXAuthorCLODMesh* pAuthorCLODMesh);

// Process the model resource static geometry block
void CIFXAuthorCLODDecoder::ProcessAuthorCLODStaticGeometryBlockX( U32 uBlockType )
{
	IFXASSERT(m_pBitStreamX);

	
	//
	// Read the block info for AuthorCLODStatic
	//
	//  1. Name
	//  2. Chain Index (always 0 for this block)
	//  3. Base Mesh Description
	//    3.1 U32 NumFaces;
	//    3.2 U32 NumPositions;
	//    3.3 U32 NumNormals;
	//    3.4 U32 NumDiffuseColors;
	//    3.5 U32 NumSpecularColors;
	//    3.6 U32 NumTexCoords;
	//  4. Base Mesh Data
	//      4.1 Positions [NumPositions]
	//          4.1.1 F32 PositionX
	//          4.1.2 F32 PositionY
	//          4.1.3 F32 PositionZ
	//      4.2 Normals [NumNormals]
	//          4.2.1 F32 NormalX
	//          4.2.2 F32 NormalY
	//          4.2.3 F32 NormalZ
	//      4.3 Diffuse Colors [NumDiffuseColors]
	//          4.3.1 F32 R
	//          4.3.2 F32 G
	//          4.3.3 F32 B
	//          4.3.4 F32 A
	//      4.4 Specular Colors [NumSpecularColors]
	//          4.4.1 F32 R
	//          4.4.2 F32 G
	//          4.4.3 F32 B
	//          4.4.4 F32 A
	//      4.5 Tex Coords [NumTexCoords]
	//          4.5.1 F32 U
	//          4.5.2 F32 V
	//          4.5.3 F32 S
	//          4.5.4 F32 T
	//      4.6 Faces [NumFaces]
	//        4.6.1 Face Shader ID
	//        4.6.2 Position Faces
	//        4.6.3 Normal Faces
	//        4.6.4 DiffuseColor Faces (if present for that material)
	//        4.6.5 SpecularColor Faces (if present for that material)
	//        4.6.6 TexCoord Faces [NumLayers] (if present for that material)
	//
	

	//  1. Name
	IFXString stringName;
	m_pBitStreamX->ReadIFXStringX(stringName);

	//  2. Chain Index
	if( uBlockType == BlockType_GeneratorCLODBaseMeshContinuationU3D )
	{
		U32 uChainIndex;
		m_pBitStreamX->ReadU32X(uChainIndex);
	}

	const IFXAuthorMeshDesc * pMaxMeshDesc = m_pReconstructedAuthorCLODMesh->GetMaxMeshDesc();

	/// @todo: Find a way to set up the minimum resolution mesh description
	IFXAuthorMeshDesc samdMinMeshDescription;

	//    3.1 U32 NumFaces;
	m_pBitStreamX->ReadU32X(samdMinMeshDescription.NumFaces);
	//    3.2 U32 NumPositions;
	m_pBitStreamX->ReadU32X(samdMinMeshDescription.NumPositions);
	//    3.3 U32 NumNormals;
	m_pBitStreamX->ReadU32X(samdMinMeshDescription.NumNormals);
	//    3.4 U32 NumDiffuseColors;
	m_pBitStreamX->ReadU32X(samdMinMeshDescription.NumDiffuseColors);
	//    3.5 U32 NumSpecularColors;
	m_pBitStreamX->ReadU32X(samdMinMeshDescription.NumSpecularColors);
	//    3.6 U32 NumTexCoords;
	m_pBitStreamX->ReadU32X(samdMinMeshDescription.NumTexCoords);

	samdMinMeshDescription.NumMaterials = pMaxMeshDesc->NumMaterials;

	m_pReconstructedAuthorCLODMesh->SetMeshDesc( &samdMinMeshDescription );

	// Get exclude normals flag (set during declaration block processing)
	BOOL bExcludeNormals = FALSE;
	IFXASSERT(m_pAuthorCLODResource);
	m_pAuthorCLODResource->GetExcludeNormals(bExcludeNormals);

	if( uBlockType == BlockType_GeneratorCLODBaseMeshContinuationU3D )
	{

		IFXCHECKX(m_pReconstructedAuthorCLODMesh->Lock());
		{
			U32 i = 0;
			//  4.1 Positions [NumPositions]
			if( samdMinMeshDescription.NumPositions )
			{
				IFXVector3* pReconstructedPositions = NULL;
				IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetPositions(&pReconstructedPositions));
				for(i = 0; i < samdMinMeshDescription.NumPositions; i++)
				{
					F32 fX = 0;
					F32 fY = 0;
					F32 fZ = 0;
					m_pBitStreamX->ReadF32X(fX);
					m_pBitStreamX->ReadF32X(fY);
					m_pBitStreamX->ReadF32X(fZ);
					pReconstructedPositions[i].X() = fX;
					pReconstructedPositions[i].Y() = fY;
					pReconstructedPositions[i].Z() = fZ;
				}
			}

			//  4.2 Normals [NumNormals]
			if( samdMinMeshDescription.NumNormals )
			{
				IFXVector3* pReconstructedNormals = NULL;
				IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetNormals(&pReconstructedNormals));
				for(i = 0; i < samdMinMeshDescription.NumNormals; i++)
				{
					F32 fX = 0;
					F32 fY = 0;
					F32 fZ = 0;
					m_pBitStreamX->ReadF32X(fX);
					m_pBitStreamX->ReadF32X(fY);
					m_pBitStreamX->ReadF32X(fZ);
					pReconstructedNormals[i].X() = fX;
					pReconstructedNormals[i].Y() = fY;
					pReconstructedNormals[i].Z() = fZ;
				}
			}

			IFXVector4 color;
			IFXVector4* pReconstructedColors = NULL;

			//  4.3 Diffuse Colors [NumDiffuseColors]
			if( samdMinMeshDescription.NumDiffuseColors )
			{
				IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetDiffuseColors(&pReconstructedColors));
				for(i = 0; i < samdMinMeshDescription.NumDiffuseColors; i++)
				{
					F32 R, G, B, A;
					m_pBitStreamX->ReadF32X( R );
					m_pBitStreamX->ReadF32X( G );
					m_pBitStreamX->ReadF32X( B );
					m_pBitStreamX->ReadF32X( A );
					color.Set( B, G, R, A );

					pReconstructedColors[i] = color;
				}
			}

			//  4.4 Specular Colors [NumSpecularColors]
			if( samdMinMeshDescription.NumSpecularColors )
			{
				IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetSpecularColors(&pReconstructedColors));
				for(i = 0; i < samdMinMeshDescription.NumSpecularColors; i++)
				{
					F32 R, G, B, A;
					m_pBitStreamX->ReadF32X( R );
					m_pBitStreamX->ReadF32X( G );
					m_pBitStreamX->ReadF32X( B );
					m_pBitStreamX->ReadF32X( A );
					color.Set( B, G, R, A );

					pReconstructedColors[i] = color;
				}
			}

			//  4.5 Tex Coords [NumTexCoords]
			if( samdMinMeshDescription.NumTexCoords )
			{
				IFXVector4* pReconstructedTexCoords = NULL;
				/// @todo: When using IFXVector4 as a Texture Coordinate,
				// the short cuts R(), G(), B(), A() do not make sense.
				// Change to something more meaningful.
				IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetTexCoords(&pReconstructedTexCoords));
				for(i = 0; i < samdMinMeshDescription.NumTexCoords; i++)
				{
					F32 fU = 0;
					F32 fV = 0;
					F32 fW = 0;
					F32 fT = 0;
					m_pBitStreamX->ReadF32X(fU);
					m_pBitStreamX->ReadF32X(fV);
					m_pBitStreamX->ReadF32X(fW);
					m_pBitStreamX->ReadF32X(fT);
					pReconstructedTexCoords[i].U() = fU;
					pReconstructedTexCoords[i].V() = fV;
					pReconstructedTexCoords[i].W() = fW;
					pReconstructedTexCoords[i].T() = fT;
				}
			}

			//  4.6 Faces [NumFaces]
			U32 uFaceMaterial = 0;

			IFXAuthorMaterial* pAuthorMeshMaterial = NULL;
			IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetMaterials(&pAuthorMeshMaterial));

			U32 uNumFaceTypes;
			U32 count, j;
			IFXAuthorFace *faces;

			for(i = 0; i < samdMinMeshDescription.NumFaces; i++)
			{
				//  4.6.1 Face Shaders
				m_pBitStreamX->ReadCompressedU32X(uACContextBaseShadingID, uFaceMaterial);
				m_pReconstructedAuthorCLODMesh->SetFaceMaterial(i,uFaceMaterial);

				// allocate a memory for whole bunch of data for this face
				uNumFaceTypes = 2 + pAuthorMeshMaterial[uFaceMaterial].m_uNumTextureLayers;
				if( pAuthorMeshMaterial[uFaceMaterial].m_uDiffuseColors )
					uNumFaceTypes++;
				if( pAuthorMeshMaterial[uFaceMaterial].m_uSpecularColors )
					uNumFaceTypes++;
				faces = new IFXAuthorFace[ uNumFaceTypes ];

				// read 1st indices
				//  4.6.2 Position Faces
				m_pBitStreamX->ReadCompressedU32X(uACStaticFull + samdMinMeshDescription.NumPositions, faces[0].VertexA());
				//  4.6.3 Normal Faces
				if(!bExcludeNormals) {
					m_pBitStreamX->ReadCompressedU32X(uACStaticFull + samdMinMeshDescription.NumNormals, faces[1].VertexA());
				}
				count = 2;
				// 4.6.4 DiffuseColor Faces (if present for that material)
				if(pAuthorMeshMaterial[uFaceMaterial].m_uDiffuseColors)
				{
					m_pBitStreamX->ReadCompressedU32X(uACStaticFull + samdMinMeshDescription.NumDiffuseColors, faces[count].VertexA());
					count++;
				}
				//  4.6.5 SpecularColor Faces (if present for that material)
				if(pAuthorMeshMaterial[uFaceMaterial].m_uSpecularColors)
				{
					m_pBitStreamX->ReadCompressedU32X(uACStaticFull + samdMinMeshDescription.NumSpecularColors, faces[count].VertexA());
					count++;
				}
				//  4.6.6 TexCoord Faces [NumLayers] (if present for that material)
				for( j=0; j < pAuthorMeshMaterial[uFaceMaterial].m_uNumTextureLayers; j++)
				{
					m_pBitStreamX->ReadCompressedU32X(uACStaticFull + samdMinMeshDescription.NumTexCoords, faces[count].VertexA());
					count++;
				}

				// read 2nd indices
				m_pBitStreamX->ReadCompressedU32X(uACStaticFull + samdMinMeshDescription.NumPositions, faces[0].VertexB());
				if(!bExcludeNormals) {
					m_pBitStreamX->ReadCompressedU32X(uACStaticFull + samdMinMeshDescription.NumNormals, faces[1].VertexB());
				}
				count = 2;
				if(pAuthorMeshMaterial[uFaceMaterial].m_uDiffuseColors)
				{
					m_pBitStreamX->ReadCompressedU32X(uACStaticFull + samdMinMeshDescription.NumDiffuseColors, faces[count].VertexB());
					count++;
				}
				if(pAuthorMeshMaterial[uFaceMaterial].m_uSpecularColors)
				{
					m_pBitStreamX->ReadCompressedU32X(uACStaticFull + samdMinMeshDescription.NumSpecularColors, faces[count].VertexB());
					count++;
				}
				for( j=0; j < pAuthorMeshMaterial[uFaceMaterial].m_uNumTextureLayers; j++)
				{
					m_pBitStreamX->ReadCompressedU32X(uACStaticFull + samdMinMeshDescription.NumTexCoords, faces[count].VertexB());
					count++;
				}

				// read 3rd indices
				m_pBitStreamX->ReadCompressedU32X(uACStaticFull + samdMinMeshDescription.NumPositions, faces[0].VertexC());
				if(!bExcludeNormals) {
					m_pBitStreamX->ReadCompressedU32X(uACStaticFull + samdMinMeshDescription.NumNormals, faces[1].VertexC());
				}
				count = 2;
				if(pAuthorMeshMaterial[uFaceMaterial].m_uDiffuseColors)
				{
					m_pBitStreamX->ReadCompressedU32X(uACStaticFull + samdMinMeshDescription.NumDiffuseColors, faces[count].VertexC());
					count++;
				}
				if(pAuthorMeshMaterial[uFaceMaterial].m_uSpecularColors)
				{
					m_pBitStreamX->ReadCompressedU32X(uACStaticFull + samdMinMeshDescription.NumSpecularColors, faces[count].VertexC());
					count++;
				}
				for( j=0; j < pAuthorMeshMaterial[uFaceMaterial].m_uNumTextureLayers; j++)
				{
					m_pBitStreamX->ReadCompressedU32X(uACStaticFull + samdMinMeshDescription.NumTexCoords, faces[count].VertexC());
					count++;
				}

				// set up faces
				m_pReconstructedAuthorCLODMesh->SetPositionFace( i, &faces[0] );
				if(!bExcludeNormals) {
					m_pReconstructedAuthorCLODMesh->SetNormalFace( i, &faces[1] );
				}
				count = 2;
				if(pAuthorMeshMaterial[uFaceMaterial].m_uDiffuseColors)
				{
					m_pReconstructedAuthorCLODMesh->SetDiffuseFace( i, &faces[count] );
					count++;
				}
				if(pAuthorMeshMaterial[uFaceMaterial].m_uSpecularColors)
				{
					m_pReconstructedAuthorCLODMesh->SetSpecularFace( i,&faces[count] );
					count++;
				}
				for(j=0; j < pAuthorMeshMaterial[uFaceMaterial].m_uNumTextureLayers; j++)
				{
					m_pReconstructedAuthorCLODMesh->SetTexFace( j,i,&faces[count] );
					count++;
				}

				delete [] faces;
			}

		}
		// DEBUG
		//  DumpAuthorCLODMeshX("/DecodedStatic.txt", m_pReconstructedAuthorCLODMesh);

		IFXAuthorFace face;
		U32 tmp;
		for( tmp = 0; tmp < samdMinMeshDescription.NumFaces; tmp++ )
		{
			m_pReconstructedAuthorCLODMesh->GetPositionFace(tmp, &face);
			m_pSetAdjacencyX->AddX( face.VertexA(), tmp );
			m_pSetAdjacencyX->AddX( face.VertexB(), tmp );
			m_pSetAdjacencyX->AddX( face.VertexC(), tmp );
		}

		IFXCHECKX(m_pReconstructedAuthorCLODMesh->Unlock());

	}
	

	IFXRELEASE(m_pBitStreamX);
	m_eWIPType = WIP_NONE;

	return;
}
