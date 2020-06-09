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
//  CIFXAuthorCLODEncoderX_Static.cpp
//

#include "CIFXAuthorCLODEncoderX.h"
#include "IFXExportingCIDs.h"
#include "IFXCoreCIDs.h"
#include "IFXBitStreamCompressedX.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"
#include "IFXACContext.h"

//
// Write the block info for AuthorCLODStatic
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
// In addition to writing the block, the reconstructed mesh is filled in by this method

void CIFXAuthorCLODEncoderX::MakeAuthorCLODStaticGeometryBlockX(IFXString &rName, IFXDataBlockQueueX &rDataBlockQueue, IFXMeshMap &rMeshMap)
{
	// Set resolution to minimum and get the mesh description
	IFXASSERT(m_pOriginalAuthorCLODMesh);
	U32 uMinResolution = m_pOriginalAuthorCLODMesh->GetMinResolution();
#ifdef _DEBUG
	U32 uTempResolution = 
#endif
		m_pOriginalAuthorCLODMesh->SetResolution(uMinResolution);
	IFXASSERT(uTempResolution == uMinResolution);
	const IFXAuthorMeshDesc* pMinMeshDescription = m_pOriginalAuthorCLODMesh->GetMeshDesc();

	BOOL bExcludeNormals = FALSE;
	IFXASSERT(m_pAuthorCLODResource);
	m_pAuthorCLODResource->GetExcludeNormals(bExcludeNormals);

	m_pReconstructedAuthorCLODMesh->SetMeshDesc( pMinMeshDescription );

	if(uMinResolution > 0) 
	{
		// Create a bitstream
		IFXDECLARELOCAL(IFXBitStreamCompressedX,pBitStreamX);
		IFXCHECKX(IFXCreateComponent(CID_IFXBitStreamX,IID_IFXBitStreamCompressedX,(void**)&pBitStreamX));
		U32 uProfile;
		m_pCoreServices->GetProfile(uProfile);
		pBitStreamX->SetNoCompressionMode((uProfile & IFXPROFILE_NOCOMPRESSION) ? TRUE : FALSE);

		//  1. Name
		pBitStreamX->WriteIFXStringX(rName);

		//  2. Chain Index
		pBitStreamX->WriteU32X( 0 );

		//  3. Base Mesh Description
		//    3.1 U32 NumFaces;
		pBitStreamX->WriteU32X(pMinMeshDescription->NumFaces);
		//    3.2 U32 NumPositions;
		pBitStreamX->WriteU32X(pMinMeshDescription->NumPositions);
		//    3.3 U32 NumNormals;
		if(bExcludeNormals) 
		{
			pBitStreamX->WriteU32X(0);
		} 
		else 
		{
			pBitStreamX->WriteU32X(pMinMeshDescription->NumNormals);
		}
		//    3.4 U32 NumDiffuseColors;
		pBitStreamX->WriteU32X(pMinMeshDescription->NumDiffuseColors);
		//    3.5 U32 NumSpecularColors;
		pBitStreamX->WriteU32X(pMinMeshDescription->NumSpecularColors);
		//    3.6 U32 NumTexCoords;
		pBitStreamX->WriteU32X(pMinMeshDescription->NumTexCoords);

		/// @todo: use author mesh locker object to correctly unlock mesh if there is an exception
		U32 uCount;
		IFXCHECKX(m_pOriginalAuthorCLODMesh->Lock());
		IFXCHECKX(m_pReconstructedAuthorCLODMesh->Lock());

		// 4.1
		IFXVector3* pPos = NULL;
		m_pOriginalAuthorCLODMesh->GetPositions( &pPos );
		for( uCount = 0; uCount < pMinMeshDescription->NumPositions; uCount++ )
		{
			pBitStreamX->WriteF32X( pPos[ uCount ].X() );
			pBitStreamX->WriteF32X( pPos[ uCount ].Y() );
			pBitStreamX->WriteF32X( pPos[ uCount ].Z() );
			m_pReconstructedAuthorCLODMesh->SetPosition( uCount, &pPos[ uCount ] );
		}

		if(!bExcludeNormals) 
		{
			// If normals are not excluded
			// 4.2
			IFXVector3* pNorm;
			m_pOriginalAuthorCLODMesh->GetNormals( &pNorm );
			for( uCount = 0; uCount < pMinMeshDescription->NumNormals; uCount++ )
			{
				pBitStreamX->WriteF32X( pNorm[ uCount ].X() );
				pBitStreamX->WriteF32X( pNorm[ uCount ].Y() );
				pBitStreamX->WriteF32X( pNorm[ uCount ].Z() );
				m_pReconstructedAuthorCLODMesh->SetNormal( uCount, &pNorm[ uCount ] );
			}
		}

		// 4.3
		IFXVector4* pDiffColors;
		// We will use IFXVector4 to convert color from U32 BGRA format in which
		// they are stored in AuthorCLODMesh; and then write
		// it in R, G, B, A order
		IFXVector4 color;
		m_pOriginalAuthorCLODMesh->GetDiffuseColors( &pDiffColors );
		for( uCount = 0; uCount < pMinMeshDescription->NumDiffuseColors; uCount++ )
		{
			color = pDiffColors[ uCount ];
			pBitStreamX->WriteF32X( color.B() );
			pBitStreamX->WriteF32X( color.G() );
			pBitStreamX->WriteF32X( color.R() );
			pBitStreamX->WriteF32X( color.A() );
			m_pReconstructedAuthorCLODMesh->SetDiffuseColor( uCount, &pDiffColors[ uCount ] );
		}

		// 4.4
		IFXVector4* pSpecColors;
		m_pOriginalAuthorCLODMesh->GetSpecularColors( &pSpecColors );
		for( uCount = 0; uCount < pMinMeshDescription->NumSpecularColors; uCount++ )
		{
			color = pSpecColors[ uCount ];
			pBitStreamX->WriteF32X( color.B() );
			pBitStreamX->WriteF32X( color.G() );
			pBitStreamX->WriteF32X( color.R() );
			pBitStreamX->WriteF32X( color.A() );
			m_pReconstructedAuthorCLODMesh->SetSpecularColor( uCount, &pSpecColors[ uCount ] );
		}

		// 4.5
		IFXVector4* pTexCoords;
		m_pOriginalAuthorCLODMesh->GetTexCoords( &pTexCoords );
		for( uCount = 0; uCount < pMinMeshDescription->NumTexCoords; uCount++ )
		{
			pBitStreamX->WriteF32X( pTexCoords[ uCount ].U() );
			pBitStreamX->WriteF32X( pTexCoords[ uCount ].V() );
			pBitStreamX->WriteF32X( pTexCoords[ uCount ].W() );
			pBitStreamX->WriteF32X( pTexCoords[ uCount ].T() );
			m_pReconstructedAuthorCLODMesh->SetTexCoord( uCount, &pTexCoords[ uCount ] );
		}

		// 4.6
		U32 uMat;
		IFXAuthorFace facePos;
		IFXAuthorFace faceNorm;
		IFXAuthorFace faceDiff;
		IFXAuthorFace faceSpec;
		IFXAuthorFace faceTexCoord;

		IFXAuthorMaterial *pMeshMat;
		m_pOriginalAuthorCLODMesh->GetMaterials( &pMeshMat );

		for( uCount = 0; uCount < pMinMeshDescription->NumFaces; uCount++ )
		{
			U32 j = 0;

			// 4.6.1
			m_pOriginalAuthorCLODMesh->GetFaceMaterial( uCount, &uMat );
			m_pReconstructedAuthorCLODMesh->SetFaceMaterial( uCount, uMat );
			pBitStreamX->WriteCompressedU32X( uACContextBaseShadingID, uMat );

			// Get data and write 1st indices for this Face:
			// 4.6.2
			m_pOriginalAuthorCLODMesh->GetPositionFace( uCount, &facePos );
			m_pReconstructedAuthorCLODMesh->SetPositionFace( uCount, &facePos );
			pBitStreamX->WriteCompressedU32X( uACStaticFull + pMinMeshDescription->NumPositions,  facePos.VertexA() );

			// 4.6.3
			if(!bExcludeNormals) 
			{
				m_pOriginalAuthorCLODMesh->GetNormalFace( uCount, &faceNorm );
				m_pReconstructedAuthorCLODMesh->SetNormalFace( uCount, &faceNorm );
				pBitStreamX->WriteCompressedU32X( uACStaticFull + pMinMeshDescription->NumNormals, faceNorm.VertexA() );
			}

			// 4.6.4
			if( pMeshMat[ uMat ].m_uDiffuseColors )
			{
				m_pOriginalAuthorCLODMesh->GetDiffuseFace( uCount, &faceDiff );
				m_pReconstructedAuthorCLODMesh->SetDiffuseFace( uCount, &faceDiff );
				pBitStreamX->WriteCompressedU32X( uACStaticFull + pMinMeshDescription->NumDiffuseColors, faceDiff.VertexA() );
			}

			// 4.6.5
			if( pMeshMat[ uMat ].m_uSpecularColors )
			{
				m_pOriginalAuthorCLODMesh->GetSpecularFace( uCount, &faceSpec );
				m_pReconstructedAuthorCLODMesh->SetSpecularFace( uCount, &faceSpec );
				pBitStreamX->WriteCompressedU32X( uACStaticFull + pMinMeshDescription->NumSpecularColors, faceSpec.VertexA() );
			}

			// 4.6.6
			for( j = 0; j < pMeshMat[ uMat ].m_uNumTextureLayers; j++ )
			{
				m_pOriginalAuthorCLODMesh->GetTexFace( j, uCount, &faceTexCoord );
				m_pReconstructedAuthorCLODMesh->SetTexFace( j, uCount, &faceTexCoord );
				pBitStreamX->WriteCompressedU32X( uACStaticFull + pMinMeshDescription->NumTexCoords, faceTexCoord.VertexA() );
			}

			// Repeat writing of 4.6.2-4.6.6 for 2nd and 3rd indices:
			pBitStreamX->WriteCompressedU32X( uACStaticFull + pMinMeshDescription->NumPositions, facePos.VertexB() );
			if(!bExcludeNormals) {
				pBitStreamX->WriteCompressedU32X( uACStaticFull + pMinMeshDescription->NumNormals, faceNorm.VertexB() );
			}
			if( pMeshMat[ uMat ].m_uDiffuseColors )
				pBitStreamX->WriteCompressedU32X( uACStaticFull + pMinMeshDescription->NumDiffuseColors, faceDiff.VertexB() );
			if( pMeshMat[ uMat ].m_uSpecularColors )
				pBitStreamX->WriteCompressedU32X( uACStaticFull + pMinMeshDescription->NumSpecularColors, faceSpec.VertexB() );
			for( j = 0; j < pMeshMat[ uMat ].m_uNumTextureLayers; j++ )
			{
				m_pOriginalAuthorCLODMesh->GetTexFace( j, uCount, &faceTexCoord );
				pBitStreamX->WriteCompressedU32X( uACStaticFull + pMinMeshDescription->NumTexCoords, faceTexCoord.VertexB() );
			}

			pBitStreamX->WriteCompressedU32X( uACStaticFull + pMinMeshDescription->NumPositions, facePos.VertexC() );
			if(!bExcludeNormals) {
				pBitStreamX->WriteCompressedU32X( uACStaticFull + pMinMeshDescription->NumNormals, faceNorm.VertexC() );
			}
			if( pMeshMat[ uMat ].m_uDiffuseColors )
				pBitStreamX->WriteCompressedU32X( uACStaticFull + pMinMeshDescription->NumDiffuseColors, faceDiff.VertexC() );
			if( pMeshMat[ uMat ].m_uSpecularColors )
				pBitStreamX->WriteCompressedU32X( uACStaticFull + pMinMeshDescription->NumSpecularColors, faceSpec.VertexC() );
			for( j = 0; j < pMeshMat[ uMat ].m_uNumTextureLayers; j++ )
			{
				m_pOriginalAuthorCLODMesh->GetTexFace( j, uCount, &faceTexCoord );
				pBitStreamX->WriteCompressedU32X( uACStaticFull + pMinMeshDescription->NumTexCoords, faceTexCoord.VertexC() );
			}
		}

		IFXCHECKX(m_pOriginalAuthorCLODMesh->Unlock());
		IFXCHECKX(m_pReconstructedAuthorCLODMesh->Unlock());

		pBitStreamX->WriteU32X(0);

		// Get the data block
		IFXDECLARELOCAL(IFXDataBlockX,pDataBlockX);
		pBitStreamX->GetDataBlockX(pDataBlockX);
		IFXASSERT(pDataBlockX);

		// Set block type
		pDataBlockX->SetBlockTypeX( BlockType_GeneratorCLODBaseMeshContinuationU3D );

		// Set block priority
		pDataBlockX->SetPriorityX(m_uPriorityCurrent);
		m_uPriorityCurrent += m_uPriorityIncrement;

		// Add block to queue
		rDataBlockQueue.AppendBlockX(*pDataBlockX);
	}
}
