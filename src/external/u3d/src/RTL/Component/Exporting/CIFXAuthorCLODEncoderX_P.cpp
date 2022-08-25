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

#include "CIFXAuthorCLODEncoderX.h"
#include "IFXExportingCIDs.h"
#include "IFXCoreCIDs.h"
#include "IFXBitStreamCompressedX.h"
#include "IFXBlockTypes.h"
#include "IFXACContext.h"
#include "IFXCheckX.h"
#include "IFXVertexMap.h"

void CIFXAuthorCLODEncoderX::CalculateFaceNormalX(U32 uFaceIndex, IFXVector3& rvFaceNormal)
{
	// Get the face
	IFXAuthorFace facePosition;
	IFXASSERT(m_pReconstructedAuthorCLODMesh);
	IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetPositionFace(uFaceIndex, &facePosition));

	// Get the positions
	IFXVector3 vA, vB, vC;
	IFXVector3 vZero(0.0f,0.0f,0.0f);

	IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetPosition(facePosition.VertexA(),&vA));
	IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetPosition(facePosition.VertexB(),&vB));
	IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetPosition(facePosition.VertexC(),&vC));

	// Calculate the face normal
	IFXVector3 vD1, vD2;

	vD1.Subtract(vB,vA);
	vD2.Subtract(vC,vA);
	if( vD1.IsApproximately(vZero, IFX_LEN_EPSILON) || vD2.IsApproximately(vZero, IFX_LEN_EPSILON) ) 
	{
		rvFaceNormal.Set(-20.0f, 0.0f, 0.0f);
	} 
	else
	{
		// if we are working with very close vertices we can face 'too small numbers'
		// problem. So, if vD1 and vD2 aren't already too small then normalize them
		vD1.Normalize();
		vD2.Normalize();

		rvFaceNormal.CrossProduct(vD1,vD2);
		if(rvFaceNormal.IsApproximately(vZero, IFX_LEN_EPSILON)) 
		{
			rvFaceNormal.Set(-20.0f, 0.0f, 0.0f);
		} 
		else 
		{
			rvFaceNormal.Normalize();
		}
	}
}



//
//  Block info for AuthorCLODProgressive
//
//  1. Name
//  2. Chain Index (always 0 for this block)
//  3. Resolution Update Range
//      2.1 U32 Start Resolution
//      2.2 U32 End Resolution
//  4. Resolution Update [End Resolution - Start Resolution]
//      ...


// Make the progressive geometry blocks
void CIFXAuthorCLODEncoderX::MakeAuthorCLODProgressiveGeometryBlocksX(IFXString &rName, IFXDataBlockQueueX &rDataBlockQueue, IFXMeshMap*& rpMeshMap)
{
	IFXDECLARELOCAL( IFXSetAdjacencyX, pSetAdjacencyX );

	// Get the exclude normals flag
	BOOL bExcludeNormals = FALSE;
	IFXASSERT(m_pAuthorCLODResource);
	m_pAuthorCLODResource->GetExcludeNormals(bExcludeNormals);

	// Set up the SetAdjacency object
	IFXCHECKX(IFXCreateComponent(CID_IFXSetAdjacencyX,IID_IFXSetAdjacencyX,(void**)&pSetAdjacencyX));
	pSetAdjacencyX->InitializeX(m_pReconstructedAuthorCLODMesh);

	// Set up mesh map object
	IFXDECLARELOCAL(IFXMeshMap,pNewMeshMap);
	IFXCHECKX(IFXCreateComponent(CID_IFXMeshMap,IID_IFXMeshMap,(void**)&pNewMeshMap));
	IFXCHECKX(pNewMeshMap->Allocate(m_pOriginalAuthorCLODMesh));

	/// @todo: What do we do if the mesh is to be encoded partially static???
	if (0 == m_pOriginalAuthorCLODMesh->GetMinResolution())
	{
		// Initialize the maps we aren't going to change to an
		// identity mapping (normals are the only thing this case will
		// end up changing).
		IFXCHECKX(pNewMeshMap->AddIdentityMappingToMap(0)); // faces
		IFXCHECKX(pNewMeshMap->AddIdentityMappingToMap(1)); // positions
		IFXCHECKX(pNewMeshMap->AddIdentityMappingToMap(3)); // textures
		IFXCHECKX(pNewMeshMap->AddIdentityMappingToMap(4)); // diffuse
		IFXCHECKX(pNewMeshMap->AddIdentityMappingToMap(5)); // specular
	}

	// Set resolution to minimum and get the mesh description
	U32 uCurrentResolution = m_pOriginalAuthorCLODMesh->GetMinResolution();
	{
#ifdef _DEBUG
		U32 uTempResolution = 
#endif
			m_pOriginalAuthorCLODMesh->SetResolution(uCurrentResolution);
		IFXASSERT(uTempResolution == uCurrentResolution);
	}

	IFXAuthorMeshDesc samdMinMeshDescription = * (m_pOriginalAuthorCLODMesh->GetMeshDesc());
	IFXAuthorMeshDesc samdMaxMeshDescription = * (m_pOriginalAuthorCLODMesh->GetMaxMeshDesc());
	const IFXAuthorMeshDesc* pcCurrentMeshDescription = m_pOriginalAuthorCLODMesh->GetMeshDesc();
	const IFXAuthorMeshDesc* pcReconstructedMeshDescription = m_pReconstructedAuthorCLODMesh->GetMeshDesc();

	U32 tmp;

	IFXAuthorFace face;
	for( tmp = 0; tmp < samdMinMeshDescription.NumFaces; tmp++ )
	{
		m_pReconstructedAuthorCLODMesh->GetPositionFace(tmp, &face);
		pSetAdjacencyX->AddX( face.VertexA(), tmp );
		pSetAdjacencyX->AddX( face.VertexB(), tmp );
		pSetAdjacencyX->AddX( face.VertexC(), tmp );
	}

	U32 uMinResolution = samdMinMeshDescription.NumPositions ;
	U32 uMaxResolution = samdMaxMeshDescription.NumPositions ;

	U32 uNumResCoded = uMinResolution;

	while (uNumResCoded < uMaxResolution)
	{
		U32 uStartResolution = uNumResCoded;
		U32 uEndResolution = uStartResolution + 4096;
		if(uEndResolution > uMaxResolution)
		{
			uEndResolution = uMaxResolution;
		}

		// Create a bitstream
		IFXDECLARELOCAL(IFXBitStreamCompressedX,pBitStreamX);
		IFXCHECKX(IFXCreateComponent(CID_IFXBitStreamX,IID_IFXBitStreamCompressedX,(void**)&pBitStreamX));
		U32 uProfile;
		m_pCoreServices->GetProfile(uProfile);
		pBitStreamX->SetNoCompressionMode((uProfile & IFXPROFILE_NOCOMPRESSION) ? TRUE : FALSE);

		//  1. Name
		pBitStreamX->WriteIFXStringX(rName);

		//  2. Chain Index (always zero for this block)
		pBitStreamX->WriteU32X(0);

		//  3. Resolution Update Range
		pBitStreamX->WriteU32X(uStartResolution);
		pBitStreamX->WriteU32X(uEndResolution);

		//  4. Resolution Updates [NumVertexUpdates]
		U32 uPrevSplitDiffColor  = 0;
		U32 uPrevUpdateDiffColor = 0;
		U32 uPrevThirdDiffColor  = 0;
		U32 uPrevSplitSpecColor  = 0;
		U32 uPrevUpdateSpecColor = 0;
		U32 uPrevThirdSpecColor  = 0;
		U32 uPrevSplitTexCoord  = 0;
		U32 uPrevUpdateTexCoord = 0;
		U32 uPrevThirdTexCoord = 0;

		U32 i = 0;
		for(i = uStartResolution; i < uEndResolution; i++)
		{
			// Get the vertex updates
			IFXAuthorVertexUpdate avuOriginalVertexUpdate;
			IFXAuthorVertexUpdate avuReconstructedVertexUpdate;
			const IFXAuthorVertexUpdate *pConstVertexUpdate;

			// Below are the steps (A through N) for encoding
			// positions, position faces, and position face updates.

			//  A. Determine split position
			//  B. Get set of faces using split position (set B)
			//  C. Get set of positions used by set B (set C)
			//  D. Get number of new faces
			//  E. For each new face ...
			//  F. For each face in set B, predict stay or move, then classify as stay or move
			//  G. Update set adjacency for move faces
			//  H. Update set adjacency for new faces
			//  I.  Get set of faces using new position (set I)
			//  J.  Get set of positions used by set I (set J)
			//  K. Predict position vector from set J.
			//  L. Calculate position difference (new position vector - predicted position vector)
			//  M. Quantize position difference
			//  N. Calculate reconstructed position vector and store

			//  A. Determine split position
			U32  uSplitPositionIndex = 0;
			{
				// First, look for the split position index in the face updates
				BOOL bSplitPositionFound = FALSE;
				U32 j=0;
				IFXCHECKX(m_pOriginalAuthorCLODMesh->GetVertexUpdate(i,pConstVertexUpdate));
				avuOriginalVertexUpdate = *pConstVertexUpdate;
				for(j=0; j < avuOriginalVertexUpdate.NumFaceUpdates && !bSplitPositionFound; j++) {
					IFXASSERT(avuOriginalVertexUpdate.pFaceUpdates);
					if(IFXAuthorFaceUpdate::Position == avuOriginalVertexUpdate.pFaceUpdates[j].Attribute ) {
						bSplitPositionFound = TRUE;
						uSplitPositionIndex = avuOriginalVertexUpdate.pFaceUpdates[j].DecrValue;
						IFXASSERT((uCurrentResolution) == avuOriginalVertexUpdate.pFaceUpdates[j].IncrValue);
					}
				}

				// If the split position hasn't been found yet, try looking in the new faces
				if(!bSplitPositionFound && avuOriginalVertexUpdate.NumNewFaces > 0)
				{
					// Allocate tally array
					U32* puSplitPositionCandidates = new U32[uCurrentResolution+1];
					if(NULL == puSplitPositionCandidates) {
						IFXCHECKX(IFX_E_OUT_OF_MEMORY);
					}
					memset(puSplitPositionCandidates,0,sizeof(U32)*(uCurrentResolution+1));

					try {
						// Count the votes
						for(j=0; j < avuOriginalVertexUpdate.NumNewFaces; j++)
						{
							IFXAuthorFace tempFace;
							IFXCHECKX(m_pOriginalAuthorCLODMesh->GetPositionFace(pcCurrentMeshDescription->NumFaces+j, &tempFace));
							puSplitPositionCandidates[tempFace.VertexA()] ++;
							puSplitPositionCandidates[tempFace.VertexB()] ++;
							puSplitPositionCandidates[tempFace.VertexC()] ++;
						}

						// Pick a winner
						U32 uBestCount = 0;
						for(j=0; j < uCurrentResolution; j++) {
							// Note: uCurrentResolution cannot be the winner
							if(puSplitPositionCandidates[j] > uBestCount) {
								uSplitPositionIndex = j;
								bSplitPositionFound = TRUE;
								uBestCount = puSplitPositionCandidates[j] ;
							}
						}

						// Free tally array
						IFXDELETE_ARRAY(puSplitPositionCandidates);
					}
					catch (IFXException e) {
						// Free tally array
						IFXDELETE_ARRAY(puSplitPositionCandidates);
						throw(e);
					}
				}
				// If the split position still hasn't been found, then the split position doesn't matter.
				// Just pick any position (pick index zero).
			}

			//  4.1 split position index
			pBitStreamX->WriteCompressedU32X(uACStaticFull+i,uSplitPositionIndex);

			//  B. Get set of faces using split position (set B)
			IFXDECLARELOCAL(IFXSetX,pSplitFaceSetX);
			pSetAdjacencyX->GetFaceSetX(uSplitPositionIndex,pSplitFaceSetX);

			//  C. Get set of positions used by set B (set C)
			IFXDECLARELOCAL(IFXSetX,pPositionSetX);
			pSetAdjacencyX->GetPositionSetX(*pSplitFaceSetX,pPositionSetX);
			pPositionSetX->RemoveX(uSplitPositionIndex);

			// New Diffuse Color Count
			U16 uNumNewDiffuseColors   = avuOriginalVertexUpdate.NumNewDiffuseColors;
			pBitStreamX->WriteCompressedU16X(uACContextNewDiffuseColorsCount,uNumNewDiffuseColors);
			avuReconstructedVertexUpdate.NumNewDiffuseColors = uNumNewDiffuseColors;

			// New Diffuse Colors
			if(uNumNewDiffuseColors > 0)
			{
				// Calculate predicted Diffuse vertex color
				IFXVector4 v4PredictedVertColor;
				{
					IFXVector4 v4Acc, v4VertColor;
					v4Acc.Set(0.0f,0.0f,0.0f,0.0f);
					// Get the set of diffuse colors used at the split position
					IFXDECLARELOCAL(IFXSetX,pSplitDiffuseColorSet);
					pSetAdjacencyX->GetDiffuseColorSetX(uSplitPositionIndex,pSplitDiffuseColorSet);

					U32 uSize = 0;
					pSplitDiffuseColorSet->GetSizeX(uSize);

					U32 j;
					for(j=0; j < uSize; j++)
					{
						U32 uIndex = 0;
						pSplitDiffuseColorSet->GetMemberX(j,uIndex);
						m_pReconstructedAuthorCLODMesh->GetDiffuseColor(uIndex,&v4VertColor);
						v4Acc.Add(v4VertColor);
					}

					if(uSize > 0)
					{
						v4PredictedVertColor.R() = v4Acc.R()/(uSize);
						v4PredictedVertColor.G() = v4Acc.G()/(uSize);
						v4PredictedVertColor.B() = v4Acc.B()/(uSize);
						v4PredictedVertColor.A() = v4Acc.A()/(uSize);
					} else {
						v4PredictedVertColor.Set(0.0f,0.0f,0.0f,0.0f);
					}
				}

				// Write quantized prediction difference and calculate reconstructed color
				{
					U32 j=0;
					for(j=0;j<uNumNewDiffuseColors;j++)
					{
						U32 uVertColorIndex = pcReconstructedMeshDescription->NumDiffuseColors + j;

						IFXVector4 v4VertColorDifference, v4VertColor;
						m_pOriginalAuthorCLODMesh->GetDiffuseColor(uVertColorIndex,&v4VertColor);
						v4VertColorDifference.Subtract(v4VertColor,v4PredictedVertColor);

						// Quantize color difference
						U8 u8Signs = (v4VertColorDifference.R() < 0.0) | ((v4VertColorDifference.G() < 0.0) << 1)
							| ((v4VertColorDifference.B() < 0.0) << 2) | ((v4VertColorDifference.A() < 0.0) << 3);
						U32 udR = (U32) (0.5f + m_fQuantDiffuseColor * fabs(v4VertColorDifference.R()));
						U32 udG = (U32) (0.5f + m_fQuantDiffuseColor * fabs(v4VertColorDifference.G()));
						U32 udB = (U32) (0.5f + m_fQuantDiffuseColor * fabs(v4VertColorDifference.B()));
						U32 udA = (U32) (0.5f + m_fQuantDiffuseColor * fabs(v4VertColorDifference.A()));

						pBitStreamX->WriteCompressedU8X(uACContextDiffuseColorSign,u8Signs);
						pBitStreamX->WriteCompressedU32X(uACContextVertexColorDiffMagR,udR);
						pBitStreamX->WriteCompressedU32X(uACContextVertexColorDiffMagG,udG);
						pBitStreamX->WriteCompressedU32X(uACContextVertexColorDiffMagB,udB);
						pBitStreamX->WriteCompressedU32X(uACContextVertexColorDiffMagA,udA);

						//  Calculate reconstructed position vector and store
						{
							IFXVector4 v4ReconstructedVertColor;
							if(u8Signs & 1) {
								v4ReconstructedVertColor.R() = - m_fInverseQuantDiffuseColor * udR;
							} else {
								v4ReconstructedVertColor.R() = m_fInverseQuantDiffuseColor * udR;
							}
							if(u8Signs & 2) {
								v4ReconstructedVertColor.G() = - m_fInverseQuantDiffuseColor * udG;
							} else {
								v4ReconstructedVertColor.G() = m_fInverseQuantDiffuseColor * udG;
							}
							if(u8Signs & 4) {
								v4ReconstructedVertColor.B() = - m_fInverseQuantDiffuseColor * udB;
							} else {
								v4ReconstructedVertColor.B() = m_fInverseQuantDiffuseColor * udB;
							}
							if(u8Signs & 8) {
								v4ReconstructedVertColor.A() = - m_fInverseQuantDiffuseColor * udA;
							} else {
								v4ReconstructedVertColor.A() = m_fInverseQuantDiffuseColor * udA;
							}

							v4ReconstructedVertColor.Add(v4PredictedVertColor);
							IFXCHECKX(m_pReconstructedAuthorCLODMesh->SetDiffuseColor(uVertColorIndex,&v4ReconstructedVertColor));
						}
					}
				}
			}

			// New Specular Color Count
			U16 uNumNewSpecularColors  = avuOriginalVertexUpdate.NumNewSpecularColors;
			pBitStreamX->WriteCompressedU16X(uACContextNewSpecularColorsCount,uNumNewSpecularColors);
			avuReconstructedVertexUpdate.NumNewSpecularColors = uNumNewSpecularColors;

			// New Specular Colors
			if (uNumNewSpecularColors > 0)
			{
				// Calculate predicted Specular vertex color
				IFXVector4 v4PredictedVertColor;
				{
					IFXVector4 v4Acc, v4VertColor;
					v4Acc.Set(0.0f,0.0f,0.0f,0.0f);
					// Get the set of specular colors used at the split position
					IFXDECLARELOCAL(IFXSetX,pSplitSpecularColorSet);
					pSetAdjacencyX->GetSpecularColorSetX(uSplitPositionIndex,pSplitSpecularColorSet);

					U32 uSize = 0;
					pSplitSpecularColorSet->GetSizeX(uSize);

					U32 j;
					for(j=0; j < uSize; j++)
					{
						U32 uIndex = 0;
						pSplitSpecularColorSet->GetMemberX(j,uIndex);
						m_pReconstructedAuthorCLODMesh->GetSpecularColor(uIndex,&v4VertColor);
						v4Acc.Add(v4VertColor);
					}

					if(uSize > 0) {
						v4PredictedVertColor.R() = v4Acc.R()/(uSize);
						v4PredictedVertColor.G() = v4Acc.G()/(uSize);
						v4PredictedVertColor.B() = v4Acc.B()/(uSize);
						v4PredictedVertColor.A() = v4Acc.A()/(uSize);
					} else {
						v4PredictedVertColor.Set(0.0f,0.0f,0.0f,0.0f);
					}
				}

				// Write quantized prediction difference and calculate reconstructed color
				{
					U32 j=0;
					for(j=0;j<uNumNewSpecularColors;j++)
					{
						U32 uVertColorIndex = pcReconstructedMeshDescription->NumSpecularColors + j;

						IFXVector4 v4VertColorDifference, v4VertColor;
						m_pOriginalAuthorCLODMesh->GetSpecularColor(uVertColorIndex,&v4VertColor);
						v4VertColorDifference.Subtract(v4VertColor,v4PredictedVertColor);

						// Quantize color difference
						U8 u8Signs = (v4VertColorDifference.R() < 0.0) | ((v4VertColorDifference.G() < 0.0) << 1)
							| ((v4VertColorDifference.B() < 0.0) << 2) | ((v4VertColorDifference.A() < 0.0) << 3);
						U32 udR = (U32) (0.5f + m_fQuantSpecularColor * fabs(v4VertColorDifference.R()));
						U32 udG = (U32) (0.5f + m_fQuantSpecularColor * fabs(v4VertColorDifference.G()));
						U32 udB = (U32) (0.5f + m_fQuantSpecularColor * fabs(v4VertColorDifference.B()));
						U32 udA = (U32) (0.5f + m_fQuantSpecularColor * fabs(v4VertColorDifference.A()));

						pBitStreamX->WriteCompressedU8X(uACContextSpecularColorSign,u8Signs);
						pBitStreamX->WriteCompressedU32X(uACContextVertexColorDiffMagR,udR);
						pBitStreamX->WriteCompressedU32X(uACContextVertexColorDiffMagG,udG);
						pBitStreamX->WriteCompressedU32X(uACContextVertexColorDiffMagB,udB);
						pBitStreamX->WriteCompressedU32X(uACContextVertexColorDiffMagA,udA);

						//  Calculate reconstructed position vector and store
						{
							IFXVector4 v4ReconstructedVertColor;
							if(u8Signs & 1) {
								v4ReconstructedVertColor.R() = - m_fInverseQuantSpecularColor * udR;
							} else {
								v4ReconstructedVertColor.R() = m_fInverseQuantSpecularColor * udR;
							}
							if(u8Signs & 2) {
								v4ReconstructedVertColor.G() = - m_fInverseQuantSpecularColor * udG;
							} else {
								v4ReconstructedVertColor.G() = m_fInverseQuantSpecularColor * udG;
							}
							if(u8Signs & 4) {
								v4ReconstructedVertColor.B() = - m_fInverseQuantSpecularColor * udB;
							} else {
								v4ReconstructedVertColor.B() = m_fInverseQuantSpecularColor * udB;
							}
							if(u8Signs & 8) {
								v4ReconstructedVertColor.A() = - m_fInverseQuantSpecularColor * udA;
							} else {
								v4ReconstructedVertColor.A() = m_fInverseQuantSpecularColor * udA;
							}

							v4ReconstructedVertColor.Add(v4PredictedVertColor);
							IFXCHECKX(m_pReconstructedAuthorCLODMesh->SetSpecularColor(uVertColorIndex,&v4ReconstructedVertColor));
						}
					}
				}
			}

			// New Texture Coordinate Count
			U16 uNumNewTexCoords   = avuOriginalVertexUpdate.NumNewTexCoords;
			pBitStreamX->WriteCompressedU16X(uACContextNewTexCoordsCount,uNumNewTexCoords);
			avuReconstructedVertexUpdate.NumNewTexCoords = uNumNewTexCoords;

			// New Texture Coordinates
			if (uNumNewTexCoords > 0) {
				// Calculate predicted texture coordinate
				IFXVector4 v4PredictedTexCoord;
				{
					IFXVector4 v4Acc, v4TexCoord;
					v4Acc.Set(0.0f,0.0f,0.0f,0.0f);
					// Get the set of texture coords used at the split position
					IFXDECLARELOCAL(IFXSetX,pSplitTexCoordSet);
					pSetAdjacencyX->GetTexCoordSetX(0,uSplitPositionIndex,pSplitTexCoordSet);

					U32 uSize = 0;
					pSplitTexCoordSet->GetSizeX(uSize);

					U32 j;
					for(j=0; j < uSize; j++)
					{
						U32 uIndex = 0;
						pSplitTexCoordSet->GetMemberX(j,uIndex);
						m_pReconstructedAuthorCLODMesh->GetTexCoord(uIndex,&v4TexCoord);
						v4Acc.Add(v4TexCoord);
					}

					if(uSize > 0)
					{
						v4PredictedTexCoord.X() = v4Acc.X()/(uSize);
						v4PredictedTexCoord.Y() = v4Acc.Y()/(uSize);
						v4PredictedTexCoord.Z() = v4Acc.Z()/(uSize);
						v4PredictedTexCoord.H() = v4Acc.H()/(uSize);
					} else {
						v4PredictedTexCoord.Set(0.0f,0.0f,0.0f,0.0f);
					}
				}

				// Write quantized prediction difference and calculate reconstructed tex coord
				{
					U32 j=0;
					for(j=0;j<uNumNewTexCoords;j++)
					{
						IFXVector4 v4TexCoord;
						U32 uTexCoordIndex = pcReconstructedMeshDescription->NumTexCoords + j;
						m_pOriginalAuthorCLODMesh->GetTexCoord(uTexCoordIndex,&v4TexCoord);

						IFXVector4 v4TexCoordDifference;
						v4TexCoordDifference.Subtract(v4TexCoord,v4PredictedTexCoord);

						// Quantize color difference
						U8 u8Signs = (v4TexCoordDifference.X() < 0.0) | ((v4TexCoordDifference.Y() < 0.0) << 1)
							| ((v4TexCoordDifference.Z() < 0.0) << 2) | ((v4TexCoordDifference.H() < 0.0) << 3);
						U32 udX = (U32) (0.5f + m_fQuantTexCoord * fabs(v4TexCoordDifference.X()));
						U32 udY = (U32) (0.5f + m_fQuantTexCoord * fabs(v4TexCoordDifference.Y()));
						U32 udZ = (U32) (0.5f + m_fQuantTexCoord * fabs(v4TexCoordDifference.Z()));
						U32 udH = (U32) (0.5f + m_fQuantTexCoord * fabs(v4TexCoordDifference.H()));

						pBitStreamX->WriteCompressedU8X(uACContextTexCoordSign,u8Signs);
						pBitStreamX->WriteCompressedU32X(uACContextTexCoordDiffMagU,udX);
						pBitStreamX->WriteCompressedU32X(uACContextTexCoordDiffMagV,udY);
						pBitStreamX->WriteCompressedU32X(uACContextTexCoordDiffMagS,udZ);
						pBitStreamX->WriteCompressedU32X(uACContextTexCoordDiffMagT,udH);

						//  Calculate reconstructed position vector and store
						{
							IFXVector4 v4ReconstructedTexCoord;
							if(u8Signs & 1) {
								v4ReconstructedTexCoord.X() = - m_fInverseQuantTexCoord * udX;
							} else {
								v4ReconstructedTexCoord.X() = m_fInverseQuantTexCoord * udX;
							}
							if(u8Signs & 2) {
								v4ReconstructedTexCoord.Y() = - m_fInverseQuantTexCoord * udY;
							} else {
								v4ReconstructedTexCoord.Y() = m_fInverseQuantTexCoord * udY;
							}
							if(u8Signs & 4) {
								v4ReconstructedTexCoord.Z() = - m_fInverseQuantTexCoord * udZ;
							} else {
								v4ReconstructedTexCoord.Z() = m_fInverseQuantTexCoord * udZ;
							}
							if(u8Signs & 8) {
								v4ReconstructedTexCoord.H() = - m_fInverseQuantTexCoord * udH;
							} else {
								v4ReconstructedTexCoord.H() = m_fInverseQuantTexCoord * udH;
							}

							v4ReconstructedTexCoord.Add(v4PredictedTexCoord);
							IFXCHECKX(m_pReconstructedAuthorCLODMesh->SetTexCoord(uTexCoordIndex,&v4ReconstructedTexCoord));
						}
					}
				}
			}

			//  D. Get number of new faces
			U32 uNumNewFaces   = avuOriginalVertexUpdate.NumNewFaces;
			//  4.2 number of new faces
			pBitStreamX->WriteCompressedU32X(uACContextNumNewFaces,uNumNewFaces);
			avuReconstructedVertexUpdate.NumNewFaces = uNumNewFaces;

			IFXDECLARELOCAL(IFXSetX,pLeftThirdIndexSetX);
			IFXCHECKX(IFXCreateComponent(CID_IFXSetX,IID_IFXSetX,(void**)&pLeftThirdIndexSetX));

			IFXDECLARELOCAL(IFXSetX,pRightThirdIndexSetX);
			IFXCHECKX(IFXCreateComponent(CID_IFXSetX,IID_IFXSetX,(void**)&pRightThirdIndexSetX));

			//  E. For each new face ...
			{
				U8 u8OrientationType = 0; // 1 == left; 2 == right
				U8 u8ThirdIndexType = 0; // 1 == local; 2 == global
				U32 uThirdPositionIndex = 0;

				IFXASSERT(pcReconstructedMeshDescription->NumFaces == pcCurrentMeshDescription->NumFaces);
				U32 uNumFaces = pcReconstructedMeshDescription->NumFaces;
				U32 j;
				for(j=0; j < uNumNewFaces; j++)
				{
					// 4.4.1 Get the material id for the face
					U32 uShadingID = 0;
					m_pOriginalAuthorCLODMesh->GetFaceMaterial(uNumFaces+j,&uShadingID);
					pBitStreamX->WriteCompressedU32X(uACContextShadingID,uShadingID);
					m_pReconstructedAuthorCLODMesh->SetFaceMaterial(uNumFaces+j,uShadingID);

					IFXAuthorFace faceOriginalPositionFace;
					// Get the face
					m_pOriginalAuthorCLODMesh->GetPositionFace(uNumFaces+j, &faceOriginalPositionFace);

					// Locate the 3rd corner of each face and determine face orientation
					if(uCurrentResolution == faceOriginalPositionFace.VertexA()) {
						if(uSplitPositionIndex == faceOriginalPositionFace.VertexB()) {
							u8OrientationType = u8AMPOrientationRight;
							uThirdPositionIndex = faceOriginalPositionFace.VertexC();
						} else if(uSplitPositionIndex == faceOriginalPositionFace.VertexC()) {
							u8OrientationType = u8AMPOrientationLeft;
							uThirdPositionIndex = faceOriginalPositionFace.VertexB();
						} else {
							IFXASSERT(0);
						}
					} else if(uCurrentResolution == faceOriginalPositionFace.VertexB()) {
						if(uSplitPositionIndex == faceOriginalPositionFace.VertexC()) {
							u8OrientationType = u8AMPOrientationRight;
							uThirdPositionIndex = faceOriginalPositionFace.VertexA();
						} else if(uSplitPositionIndex == faceOriginalPositionFace.VertexA()) {
							u8OrientationType = u8AMPOrientationLeft;
							uThirdPositionIndex = faceOriginalPositionFace.VertexC();
						} else {
							IFXASSERT(0);
						}
					} else if(uCurrentResolution == faceOriginalPositionFace.VertexC()) {
						if(uSplitPositionIndex == faceOriginalPositionFace.VertexA()) {
							u8OrientationType = u8AMPOrientationRight;
							uThirdPositionIndex = faceOriginalPositionFace.VertexB();
						} else if(uSplitPositionIndex == faceOriginalPositionFace.VertexB()) {
							u8OrientationType = u8AMPOrientationLeft;
							uThirdPositionIndex = faceOriginalPositionFace.VertexA();
						} else {
							IFXASSERT(0);
						}
					} else {
						IFXASSERT(0);
					}

					// Add 3rd position index to either left or right 3rd index set
					if(u8AMPOrientationLeft == u8OrientationType) {
						pLeftThirdIndexSetX->AddX(uThirdPositionIndex);
					} else if(u8AMPOrientationRight == u8OrientationType) {
						pRightThirdIndexSetX->AddX(uThirdPositionIndex);
					} else {
						IFXASSERT(0);
					}

					// Determine 3rd index type: local or global
					BOOL bIsLocal = FALSE;
					U32 uLocalThirdPositionIndex = 0;
					pPositionSetX->GetIndexX(uThirdPositionIndex,bIsLocal,uLocalThirdPositionIndex);
					if(bIsLocal) {
						u8ThirdIndexType = u8AMPThirdIndexLocal;
					} else {
						u8ThirdIndexType = u8AMPThirdIndexGlobal;
						// Add to the position set for possible use by the next face
						pPositionSetX->AddX(uThirdPositionIndex);
					}

					//    4.4.2 face orientation: left or right
					pBitStreamX->WriteCompressedU8X(uACContextOrientationType,u8OrientationType);
					//    4.4.3 index scope: local or global
					pBitStreamX->WriteCompressedU8X(uACContextThirdIndexType,u8ThirdIndexType);
					//    3rd position index
					if(u8AMPThirdIndexLocal == u8ThirdIndexType) {
						// 4.4.4 local 3rd position index
						U32 uLocalSetSize = 0;
						pPositionSetX->GetSizeX(uLocalSetSize);
						IFXASSERT(uLocalThirdPositionIndex < uLocalSetSize);
						IFXASSERT(uLocalSetSize < 0x0000FFFF);
						pBitStreamX->WriteCompressedU32X(uACContextLocal3rdPosition,uLocalThirdPositionIndex);

					} else {
						// 4.4.5 global 3rd position index
						IFXASSERT(u8AMPThirdIndexGlobal == u8ThirdIndexType);
						U32 uGlobalSetSize = uCurrentResolution;
						//if(uGlobalSetSize < 2) uGlobalSetSize = 2;
						IFXASSERT(uThirdPositionIndex < uGlobalSetSize);
						pBitStreamX->WriteCompressedU32X(uACStaticFull+uGlobalSetSize,uThirdPositionIndex);
					}

					//Set new position face for Reconstructed Mesh.
					{
						IFXAuthorFace faceReconstructedPositionFace;
						if(u8AMPOrientationLeft == u8OrientationType) {
							faceReconstructedPositionFace.VertexA() = uSplitPositionIndex;
							faceReconstructedPositionFace.VertexB() = uCurrentResolution;
							faceReconstructedPositionFace.VertexC() = uThirdPositionIndex;
						} else if(u8AMPOrientationRight == u8OrientationType) {
							faceReconstructedPositionFace.VertexB() = uSplitPositionIndex;
							faceReconstructedPositionFace.VertexA() = uCurrentResolution;
							faceReconstructedPositionFace.VertexC() = uThirdPositionIndex;
						} else {
							IFXASSERT(0);
						}
						m_pReconstructedAuthorCLODMesh->SetPositionFace(uNumFaces+j, &faceReconstructedPositionFace);
					}
				} //For NumNewFaces
			}

			//  F. For each face in set B, predict stay or move, then classify as stay or move
			IFXDECLARELOCAL(IFXSetX,pMoveFaceSetX);
			IFXCHECKX(IFXCreateComponent(CID_IFXSetX,IID_IFXSetX,(void**)&pMoveFaceSetX));
			{
				IFXDECLARELOCAL(IFXSetX,pMovePositionSetX);
				IFXCHECKX(IFXCreateComponent(CID_IFXSetX,IID_IFXSetX,(void**)&pMovePositionSetX));
				IFXDECLARELOCAL(IFXSetX,pStayPositionSetX);
				IFXCHECKX(IFXCreateComponent(CID_IFXSetX,IID_IFXSetX,(void**)&pStayPositionSetX));

				U32 uSplitFaceSetSize = 0;
				U32 uFaceIndex = 0 ;
				pSplitFaceSetX->GetSizeX(uSplitFaceSetSize);
				// For each face using the split position
				U32 j = 0;
				for(j = 0; j < uSplitFaceSetSize; j++)
				{
					// Get the face
					IFXAuthorFace face;
					pSplitFaceSetX->GetMemberX(j,uFaceIndex);
					m_pReconstructedAuthorCLODMesh->GetPositionFace(uFaceIndex, &face);

					// Find which corner has the split position.
					// Then look for a third position at the other two corners.
					// Make the prediction (Stay=1, Move=2, NoGuess=0) based on where the
					// third position is found.
					U32 uPredictionStayMove = uAMPPredictNoGuess;
					U32 uJunk = 0;
					BOOL bIsMember = FALSE;
					if(uSplitPositionIndex == face.VertexA()) {
						pRightThirdIndexSetX->GetIndexX(face.VertexB(),bIsMember,uJunk);
						if(bIsMember) {
							uPredictionStayMove = uAMPPredictMove;
						} else {
							pRightThirdIndexSetX->GetIndexX(face.VertexC(),bIsMember,uJunk);
							if(bIsMember) {
								uPredictionStayMove = uAMPPredictStay;
							} else {
								pLeftThirdIndexSetX->GetIndexX(face.VertexB(),bIsMember,uJunk);
								if(bIsMember) {
									uPredictionStayMove = uAMPPredictStay;
								} else {
									pLeftThirdIndexSetX->GetIndexX(face.VertexC(),bIsMember,uJunk);
									if(bIsMember) {
										uPredictionStayMove = uAMPPredictMove;
									}
								}
							}
						}
					} else if(uSplitPositionIndex == face.VertexB()) {
						pRightThirdIndexSetX->GetIndexX(face.VertexC(),bIsMember,uJunk);
						if(bIsMember) {
							uPredictionStayMove = uAMPPredictMove;
						} else {
							pRightThirdIndexSetX->GetIndexX(face.VertexA(),bIsMember,uJunk);
							if(bIsMember) {
								uPredictionStayMove = uAMPPredictStay;
							} else {
								pLeftThirdIndexSetX->GetIndexX(face.VertexC(),bIsMember,uJunk);
								if(bIsMember) {
									uPredictionStayMove = uAMPPredictStay;
								} else {
									pLeftThirdIndexSetX->GetIndexX(face.VertexA(),bIsMember,uJunk);
									if(bIsMember) {
										uPredictionStayMove = uAMPPredictMove;
									}
								}
							}
						}
					} else if(uSplitPositionIndex == face.VertexC()) {
						pRightThirdIndexSetX->GetIndexX(face.VertexA(),bIsMember,uJunk);
						if(bIsMember) {
							uPredictionStayMove = uAMPPredictMove;
						} else {
							pRightThirdIndexSetX->GetIndexX(face.VertexB(),bIsMember,uJunk);
							if(bIsMember) {
								uPredictionStayMove = uAMPPredictStay;
							} else {
								pLeftThirdIndexSetX->GetIndexX(face.VertexA(),bIsMember,uJunk);
								if(bIsMember) {
									uPredictionStayMove = uAMPPredictStay;
								} else {
									pLeftThirdIndexSetX->GetIndexX(face.VertexB(),bIsMember,uJunk);
									if(bIsMember) {
										uPredictionStayMove = uAMPPredictMove;
									}
								}
							}
						}
					} else {
						IFXASSERT(0);
					}

					// If no prediction based on third corners
					// try making a guess based on other faces
					if(uAMPPredictNoGuess == uPredictionStayMove) {
						pMovePositionSetX->GetIndexX(face.VertexA(),bIsMember,uJunk);
						if(bIsMember) {
							uPredictionStayMove = uAMPPredictMove2;
						}
					}
					if(uAMPPredictNoGuess == uPredictionStayMove) {
						pMovePositionSetX->GetIndexX(face.VertexB(),bIsMember,uJunk);
						if(bIsMember) {
							uPredictionStayMove = uAMPPredictMove2;
						}
					}
					if(uAMPPredictNoGuess == uPredictionStayMove) {
						pMovePositionSetX->GetIndexX(face.VertexC(),bIsMember,uJunk);
						if(bIsMember) {
							uPredictionStayMove = uAMPPredictMove2;
						}
					}
					if(uAMPPredictNoGuess == uPredictionStayMove) {
						pStayPositionSetX->GetIndexX(face.VertexA(),bIsMember,uJunk);
						if(bIsMember) {
							uPredictionStayMove = uAMPPredictStay2;
						}
					}
					if(uAMPPredictNoGuess == uPredictionStayMove) {
						pStayPositionSetX->GetIndexX(face.VertexB(),bIsMember,uJunk);
						if(bIsMember) {
							uPredictionStayMove = uAMPPredictStay2;
						}
					}
					if(uAMPPredictNoGuess == uPredictionStayMove) {
						pStayPositionSetX->GetIndexX(face.VertexC(),bIsMember,uJunk);
						if(bIsMember) {
							uPredictionStayMove = uAMPPredictStay2;
						}
					}

					// Look for a position face update for this face
					BOOL bPositionFaceUpdateFound = FALSE;
					U32 k = 0;
					for(k=0; k < avuOriginalVertexUpdate.NumFaceUpdates && !bPositionFaceUpdateFound; k++)
					{
						IFXASSERT(avuOriginalVertexUpdate.pFaceUpdates);
						if(IFXAuthorFaceUpdate::Position == avuOriginalVertexUpdate.pFaceUpdates[k].Attribute )
						{
							if(uFaceIndex == avuOriginalVertexUpdate.pFaceUpdates[k].FaceIndex)
							{
								bPositionFaceUpdateFound = TRUE;
								IFXASSERT((uSplitPositionIndex) == avuOriginalVertexUpdate.pFaceUpdates[k].DecrValue);
								IFXASSERT((uCurrentResolution) == avuOriginalVertexUpdate.pFaceUpdates[k].IncrValue);
							}
						}
					}

					// Update Move Face set, Move Position set, and Stay Position set
					U8 u8StayMove = u8AMPStay;
					if(bPositionFaceUpdateFound) {
						pMoveFaceSetX->AddX(uFaceIndex);
						u8StayMove = u8AMPMove;
						pMovePositionSetX->AddX(face.VertexA());
						pMovePositionSetX->AddX(face.VertexB());
						pMovePositionSetX->AddX(face.VertexC());
						pMovePositionSetX->RemoveX(uSplitPositionIndex);
					} else {
						pStayPositionSetX->AddX(face.VertexA());
						pStayPositionSetX->AddX(face.VertexB());
						pStayPositionSetX->AddX(face.VertexC());
						pStayPositionSetX->RemoveX(uSplitPositionIndex);
					}

					// 4.5 Stay or Move
					pBitStreamX->WriteCompressedU8X(uACContextStayMove+uPredictionStayMove,u8StayMove);

				} //End for each face using split position index (uSplitFaceSetSize)
			} // End write stay or move

			// Create reconstructed face updates for positions
			// For each face in the move set, create a face update and add to vertex update
			{
				U32 uMoveSize = 0;
				pMoveFaceSetX->GetSizeX(uMoveSize);

				IFXAuthorFaceUpdate* pFaceUpdates = NULL;

				U32 uFaceUpdateArraySize = uMoveSize;
				if (uFaceUpdateArraySize>0)
				{
					pFaceUpdates = new IFXAuthorFaceUpdate[uFaceUpdateArraySize];
					if(pFaceUpdates == NULL) {
						IFXCHECKX(IFX_E_OUT_OF_MEMORY);
					}
				}
				U32 j = 0;
				U32 uFaceUpdatesIndex = 0;
				for(j=0;j<uMoveSize;j++)
				{
					// Get the face
					IFXAuthorFace face;
					U32 uMoveFaceIndex = 0;
					pMoveFaceSetX->GetMemberX(j,uMoveFaceIndex);

					// Determine the update corner
					U32 uUpdateCorner = 3;
					m_pReconstructedAuthorCLODMesh->GetPositionFace(uMoveFaceIndex, &face);
					if(face.VertexA() == uSplitPositionIndex) {
						uUpdateCorner = 0;
					} else if(face.VertexB() == uSplitPositionIndex) {
						uUpdateCorner = 1;
					} else if(face.VertexC() == uSplitPositionIndex) {
						uUpdateCorner = 2;
					} else {
						IFXASSERT(0);
					}

					// Fill in the position face update
					{
						if(uFaceUpdatesIndex >= uFaceUpdateArraySize) {
							// Reallocate array if needed
							IFXAuthorFaceUpdate* pOldFaceUpdates = pFaceUpdates;
							U32 uOldFaceUpdateArraySize = uFaceUpdateArraySize;
							uFaceUpdateArraySize = uFaceUpdatesIndex + 10; /// @todo: Better memory allocation
							pFaceUpdates = new IFXAuthorFaceUpdate[uFaceUpdateArraySize];
							if(pFaceUpdates == NULL) {
								IFXCHECKX(IFX_E_OUT_OF_MEMORY);
							}
							memset(pFaceUpdates,0,uFaceUpdateArraySize*sizeof(IFXAuthorFaceUpdate));
							if(NULL != pOldFaceUpdates) {
								memcpy(pFaceUpdates,pOldFaceUpdates,uOldFaceUpdateArraySize*sizeof(IFXAuthorFaceUpdate));
								delete [] pOldFaceUpdates;
							}
						}

						pFaceUpdates[uFaceUpdatesIndex].Attribute = IFXAuthorFaceUpdate::Position;
						pFaceUpdates[uFaceUpdatesIndex].FaceIndex = uMoveFaceIndex;
						pFaceUpdates[uFaceUpdatesIndex].Corner = uUpdateCorner;
						pFaceUpdates[uFaceUpdatesIndex].IncrValue = uCurrentResolution;
						pFaceUpdates[uFaceUpdatesIndex].DecrValue = uSplitPositionIndex;
						uFaceUpdatesIndex++;
					}

					U32 uShadingID = 0;
					m_pReconstructedAuthorCLODMesh->GetFaceMaterial(uMoveFaceIndex,&uShadingID);
					IFXAuthorMaterial ammMeshMaterial;
					IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetMaterial(uShadingID,&ammMeshMaterial));
					BOOL bDiffuseColors = ammMeshMaterial.m_uDiffuseColors;
					BOOL bSpecularColors = ammMeshMaterial.m_uSpecularColors;
					U8 u8NumTexLayers = ammMeshMaterial.m_uNumTextureLayers;

					if(bDiffuseColors)
					{
						// Determine if this face diffuse color is a Keep or Change

						// Look for a diffuse face update for this face
						BOOL bFaceUpdateFound = FALSE;
#ifdef _DEBUG
						U32 uDecrValue = 0;
#endif
						U32 uIncrValue = 0;
						U32 k = 0;
						for(k=0; k < avuOriginalVertexUpdate.NumFaceUpdates && !bFaceUpdateFound; k++) {
							IFXASSERT(avuOriginalVertexUpdate.pFaceUpdates);
							if(IFXAuthorFaceUpdate::Diffuse == avuOriginalVertexUpdate.pFaceUpdates[k].Attribute ) {
								if(uMoveFaceIndex == avuOriginalVertexUpdate.pFaceUpdates[k].FaceIndex) {
									bFaceUpdateFound = TRUE;
#ifdef _DEBUG
									uDecrValue = avuOriginalVertexUpdate.pFaceUpdates[k].DecrValue;
#endif
									uIncrValue = avuOriginalVertexUpdate.pFaceUpdates[k].IncrValue;
								}
							}
						}

						U8 uChangeKeep = 0;
						U8 uChangeIndexType = 0;
						U32 uChangeIndex = 0;

						// Determine change index if change
						if(bFaceUpdateFound)
						{
							uChangeKeep = uAMPUpdateChange;

							// Check for new
							if(uIncrValue >= pcReconstructedMeshDescription->NumDiffuseColors) {
								uChangeIndexType = uAMPUpdateNew;
								uChangeIndex = uIncrValue - pcReconstructedMeshDescription->NumDiffuseColors;
							} else {
								// Check for local
								IFXDECLARELOCAL(IFXSetX,pDiffuseColorSetX);
								pSetAdjacencyX->GetDiffuseColorSetX(uSplitPositionIndex,pDiffuseColorSetX);

								BOOL bFound = FALSE;
								pDiffuseColorSetX->GetIndexX(uIncrValue,bFound,uChangeIndex);
								if(bFound) {
									uChangeIndexType = uAMPUpdateLocal;
								} else {
									// Else global (not new and not local)
									uChangeIndexType = uAMPUpdateGlobal;
									uChangeIndex = uIncrValue;
								}
							}
						} else {
							uChangeKeep = uAMPUpdateKeep;
						}

						// Write change/keep and change index
						pBitStreamX->WriteCompressedU8X(uACContextDiffuseKeepChange,uChangeKeep);
						if(uAMPUpdateChange == uChangeKeep) {
							pBitStreamX->WriteCompressedU8X(uACContextDiffuseChangeType,uChangeIndexType);
							if(uAMPUpdateNew == uChangeIndexType) {
								pBitStreamX->WriteCompressedU32X(uACContextDiffuseChangeIndexNew,uChangeIndex);
							} else if (uAMPUpdateLocal == uChangeIndexType) {
								pBitStreamX->WriteCompressedU32X(uACContextDiffuseChangeIndexLocal,uChangeIndex);
							} else {
								IFXASSERT(uAMPUpdateGlobal == uChangeIndexType);
								pBitStreamX->WriteCompressedU32X(uACContextDiffuseChangeIndexGlobal,uChangeIndex);
							}
						}

						if(uAMPUpdateChange == uChangeKeep)
						{

							U32 uReconstructedIncrValue = 0;
							U32 uReconstructedDecrValue = 0;

							if(uAMPUpdateNew == uChangeIndexType) {
								uReconstructedIncrValue = uChangeIndex + pcReconstructedMeshDescription->NumDiffuseColors;
							} else if(uAMPUpdateLocal == uChangeIndexType) {
								IFXDECLARELOCAL(IFXSetX,pDiffuseColorSetX);
								pSetAdjacencyX->GetDiffuseColorSetX(uSplitPositionIndex,pDiffuseColorSetX);
								pDiffuseColorSetX->GetMemberX(uChangeIndex,uReconstructedIncrValue);
							} else {
								IFXASSERT(uAMPUpdateGlobal == uChangeIndexType);
								uReconstructedIncrValue = uChangeIndex;
							}

							{
								IFXAuthorFace faceDiffuse;
								IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetDiffuseFace(uMoveFaceIndex,&faceDiffuse));
								uReconstructedDecrValue = faceDiffuse.corner[uUpdateCorner];
							}

							IFXASSERT(uReconstructedIncrValue == uIncrValue);
							IFXASSERT(uReconstructedDecrValue == uDecrValue);

							// Fill in diffuse face update
							if(uFaceUpdatesIndex >= uFaceUpdateArraySize)
							{
								// Reallocate array if needed
								IFXAuthorFaceUpdate* pOldFaceUpdates = pFaceUpdates;
								U32 uOldFaceUpdateArraySize = uFaceUpdateArraySize;
								uFaceUpdateArraySize = uFaceUpdatesIndex + 10; /// @todo: Better memory allocation
								pFaceUpdates = new IFXAuthorFaceUpdate[uFaceUpdateArraySize];
								if(pFaceUpdates == NULL) {
									IFXCHECKX(IFX_E_OUT_OF_MEMORY);
								}
								memset(pFaceUpdates,0,uFaceUpdateArraySize*sizeof(IFXAuthorFaceUpdate));
								if(NULL != pOldFaceUpdates) {
									memcpy(pFaceUpdates,pOldFaceUpdates,uOldFaceUpdateArraySize*sizeof(IFXAuthorFaceUpdate));
									delete [] pOldFaceUpdates;
								}
							}

							pFaceUpdates[uFaceUpdatesIndex].Attribute = IFXAuthorFaceUpdate::Diffuse;
							pFaceUpdates[uFaceUpdatesIndex].FaceIndex = uMoveFaceIndex;
							pFaceUpdates[uFaceUpdatesIndex].Corner = uUpdateCorner;
							pFaceUpdates[uFaceUpdatesIndex].IncrValue = uReconstructedIncrValue;
							pFaceUpdates[uFaceUpdatesIndex].DecrValue = uReconstructedDecrValue;
							uFaceUpdatesIndex++;
						}
					}

					if(bSpecularColors)
					{
						// Determine if this face specular color is a Keep or Change

						// Look for a specular face update for this face
						BOOL bFaceUpdateFound = FALSE;
#ifdef _DEBUG
						U32 uDecrValue = 0;
#endif
						U32 uIncrValue = 0;
						U32 k = 0;
						for(k=0; k < avuOriginalVertexUpdate.NumFaceUpdates && !bFaceUpdateFound; k++)
						{
							IFXASSERT(avuOriginalVertexUpdate.pFaceUpdates);
							if(IFXAuthorFaceUpdate::Specular == avuOriginalVertexUpdate.pFaceUpdates[k].Attribute )
							{
								if(uMoveFaceIndex == avuOriginalVertexUpdate.pFaceUpdates[k].FaceIndex)
								{
									bFaceUpdateFound = TRUE;
#ifdef _DEBUG
									uDecrValue = avuOriginalVertexUpdate.pFaceUpdates[k].DecrValue;
#endif
									uIncrValue = avuOriginalVertexUpdate.pFaceUpdates[k].IncrValue;
								}
							}
						}

						U8 uChangeKeep = 0;
						U8 uChangeIndexType = 0;
						U32 uChangeIndex = 0;

						// Determine change index if change
						if(bFaceUpdateFound)
						{
							uChangeKeep = uAMPUpdateChange;

							// Check for new
							if(uIncrValue >= pcReconstructedMeshDescription->NumSpecularColors) {
								uChangeIndexType = uAMPUpdateNew;
								uChangeIndex = uIncrValue - pcReconstructedMeshDescription->NumSpecularColors;
							} else {
								// Check for local
								IFXDECLARELOCAL(IFXSetX,pSpecularColorSetX);
								pSetAdjacencyX->GetSpecularColorSetX(uSplitPositionIndex,pSpecularColorSetX);

								BOOL bFound = FALSE;
								pSpecularColorSetX->GetIndexX(uIncrValue,bFound,uChangeIndex);
								if(bFound) {
									uChangeIndexType = uAMPUpdateLocal;
								} else {
									// Else global (not new and not local)
									uChangeIndexType = uAMPUpdateGlobal;
									uChangeIndex = uIncrValue;
								}
							}
						} else {
							uChangeKeep = uAMPUpdateKeep;
						}

						// Write change/keep and change index
						pBitStreamX->WriteCompressedU8X(uACContextSpecularKeepChange,uChangeKeep);
						if(uAMPUpdateChange == uChangeKeep)
						{
							pBitStreamX->WriteCompressedU8X(uACContextSpecularChangeType,uChangeIndexType);
							if(uAMPUpdateNew == uChangeIndexType) {
								pBitStreamX->WriteCompressedU32X(uACContextSpecularChangeIndexNew,uChangeIndex);
							} else if (uAMPUpdateLocal == uChangeIndexType) {
								pBitStreamX->WriteCompressedU32X(uACContextSpecularChangeIndexLocal,uChangeIndex);
							} else {
								IFXASSERT(uAMPUpdateGlobal == uChangeIndexType);
								pBitStreamX->WriteCompressedU32X(uACContextSpecularChangeIndexGlobal,uChangeIndex);
							}
						}

						if(uAMPUpdateChange == uChangeKeep)
						{

							U32 uReconstructedIncrValue = 0;
							U32 uReconstructedDecrValue = 0;

							if(uAMPUpdateNew == uChangeIndexType) {
								uReconstructedIncrValue = uChangeIndex + pcReconstructedMeshDescription->NumSpecularColors;
							} else if(uAMPUpdateLocal == uChangeIndexType) {
								IFXDECLARELOCAL(IFXSetX,pSpecularColorSetX);
								pSetAdjacencyX->GetSpecularColorSetX(uSplitPositionIndex,pSpecularColorSetX);
								pSpecularColorSetX->GetMemberX(uChangeIndex,uReconstructedIncrValue);
							} else {
								IFXASSERT(uAMPUpdateGlobal == uChangeIndexType);
								uReconstructedIncrValue = uChangeIndex;
							}

							{
								IFXAuthorFace faceSpecular;
								IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetSpecularFace(uMoveFaceIndex,&faceSpecular));
								uReconstructedDecrValue = faceSpecular.corner[uUpdateCorner];
							}

							IFXASSERT(uReconstructedIncrValue == uIncrValue);
							IFXASSERT(uReconstructedDecrValue == uDecrValue);

							// Fill in specular face update
							if(uFaceUpdatesIndex >= uFaceUpdateArraySize)
							{
								// Reallocate array if needed
								IFXAuthorFaceUpdate* pOldFaceUpdates = pFaceUpdates;
								U32 uOldFaceUpdateArraySize = uFaceUpdateArraySize;
								uFaceUpdateArraySize = uFaceUpdatesIndex + 10; /// @todo: Better memory allocation
								pFaceUpdates = new IFXAuthorFaceUpdate[uFaceUpdateArraySize];
								if(pFaceUpdates == NULL) {
									IFXCHECKX(IFX_E_OUT_OF_MEMORY);
								}
								memset(pFaceUpdates,0,uFaceUpdateArraySize*sizeof(IFXAuthorFaceUpdate));
								if(NULL != pOldFaceUpdates) {
									memcpy(pFaceUpdates,pOldFaceUpdates,uOldFaceUpdateArraySize*sizeof(IFXAuthorFaceUpdate));
									delete [] pOldFaceUpdates;
								}
							}

							pFaceUpdates[uFaceUpdatesIndex].Attribute = IFXAuthorFaceUpdate::Specular;
							pFaceUpdates[uFaceUpdatesIndex].FaceIndex = uMoveFaceIndex;
							pFaceUpdates[uFaceUpdatesIndex].Corner = uUpdateCorner;
							pFaceUpdates[uFaceUpdatesIndex].IncrValue = uReconstructedIncrValue;
							pFaceUpdates[uFaceUpdatesIndex].DecrValue = uReconstructedDecrValue;
							uFaceUpdatesIndex++;
						}
					}

					U32 uLayer = 0;
					for(uLayer = 0; uLayer < u8NumTexLayers; uLayer++)
					{
						// Determine if this face texture coordinate is a Keep or Change

						// Look for a tex face update for this face
						BOOL bFaceUpdateFound = FALSE;
#ifdef _DEBUG
						U32 uDecrValue = 0;
#endif
						U32 uIncrValue = 0;
						U32 k = 0;
						for(k=0; k < avuOriginalVertexUpdate.NumFaceUpdates && !bFaceUpdateFound; k++)
						{
							IFXASSERT(avuOriginalVertexUpdate.pFaceUpdates);
							BOOL bAttributeMatch = FALSE;
							{
								if (uLayer < IFX_MAX_TEXUNITS)
								{
									bAttributeMatch = ((IFXAuthorFaceUpdate::Tex0 + (I32)uLayer) == avuOriginalVertexUpdate.pFaceUpdates[k].Attribute);
								}
								else
								{
									IFXASSERT(0);
								}
							}
							if(bAttributeMatch)
							{
								if(uMoveFaceIndex == avuOriginalVertexUpdate.pFaceUpdates[k].FaceIndex)
								{
									bFaceUpdateFound = TRUE;
#ifdef _DEBUG
									uDecrValue = avuOriginalVertexUpdate.pFaceUpdates[k].DecrValue;
#endif
									uIncrValue = avuOriginalVertexUpdate.pFaceUpdates[k].IncrValue;
								}
							}
						}

						U8 uChangeKeep = 0;
						U8 uChangeIndexType = 0;
						U32 uChangeIndex = 0;

						// Determine change index if change
						if(bFaceUpdateFound)
						{
							uChangeKeep = uAMPUpdateChange;

							// Check for new
							if(uIncrValue >= pcReconstructedMeshDescription->NumTexCoords) {
								uChangeIndexType = uAMPUpdateNew;
								uChangeIndex = uIncrValue - pcReconstructedMeshDescription->NumTexCoords;
							} else {
								// Check for local
								IFXDECLARELOCAL(IFXSetX,pTexCoordSetX);
								pSetAdjacencyX->GetTexCoordSetX(uLayer,uSplitPositionIndex,pTexCoordSetX);

								BOOL bFound = FALSE;
								pTexCoordSetX->GetIndexX(uIncrValue,bFound,uChangeIndex);
								if(bFound) {
									uChangeIndexType = uAMPUpdateLocal;
								} else {
									// Else global (not new and not local)
									uChangeIndexType = uAMPUpdateGlobal;
									uChangeIndex = uIncrValue;
								}
							}
						} else {
							uChangeKeep = uAMPUpdateKeep;
						}

						// Write change/keep and change index
						pBitStreamX->WriteCompressedU8X(uACContextTexCoordKeepChange,uChangeKeep);
						if(uAMPUpdateChange == uChangeKeep) {
							pBitStreamX->WriteCompressedU8X(uACContextTexCoordChangeType,uChangeIndexType);
							if(uAMPUpdateNew == uChangeIndexType) {
								pBitStreamX->WriteCompressedU32X(uACContextTexCoordChangeIndexNew,uChangeIndex);
							} else if (uAMPUpdateLocal == uChangeIndexType) {
								pBitStreamX->WriteCompressedU32X(uACContextTexCoordChangeIndexLocal,uChangeIndex);
							} else {
								IFXASSERT(uAMPUpdateGlobal == uChangeIndexType);
								pBitStreamX->WriteCompressedU32X(uACContextTexCoordChangeIndexGlobal,uChangeIndex);
							}
						}

						if(uAMPUpdateChange == uChangeKeep) {

							U32 uReconstructedIncrValue = 0;
							U32 uReconstructedDecrValue = 0;

							if(uAMPUpdateNew == uChangeIndexType) {
								uReconstructedIncrValue = uChangeIndex + pcReconstructedMeshDescription->NumTexCoords;
							} else if(uAMPUpdateLocal == uChangeIndexType) {
								IFXDECLARELOCAL(IFXSetX,pTexCoordSetX);
								pSetAdjacencyX->GetTexCoordSetX(uLayer,uSplitPositionIndex,pTexCoordSetX);
								pTexCoordSetX->GetMemberX(uChangeIndex,uReconstructedIncrValue);
							} else {
								IFXASSERT(uAMPUpdateGlobal == uChangeIndexType);
								uReconstructedIncrValue = uChangeIndex;
							}

							{
								IFXAuthorFace faceTexCoord;
								IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetTexFace(uLayer,uMoveFaceIndex,&faceTexCoord));
								uReconstructedDecrValue = faceTexCoord.corner[uUpdateCorner];
							}

							IFXASSERT(uReconstructedIncrValue == uIncrValue);
							IFXASSERT(uReconstructedDecrValue == uDecrValue);

							// Fill in texture face update
							if(uFaceUpdatesIndex >= uFaceUpdateArraySize) {
								// Reallocate array if needed
								IFXAuthorFaceUpdate* pOldFaceUpdates = pFaceUpdates;
								U32 uOldFaceUpdateArraySize = uFaceUpdateArraySize;
								uFaceUpdateArraySize = uFaceUpdatesIndex + 10; /// @todo: Better memory allocation
								pFaceUpdates = new IFXAuthorFaceUpdate[uFaceUpdateArraySize];
								if(pFaceUpdates == NULL) {
									IFXCHECKX(IFX_E_OUT_OF_MEMORY);
								}
								memset(pFaceUpdates,0,uFaceUpdateArraySize*sizeof(IFXAuthorFaceUpdate));
								if(NULL != pOldFaceUpdates) {
									memcpy(pFaceUpdates,pOldFaceUpdates,uOldFaceUpdateArraySize*sizeof(IFXAuthorFaceUpdate));
									delete [] pOldFaceUpdates;
								}
							}

							if (uLayer < IFX_MAX_TEXUNITS)
							{
								pFaceUpdates[uFaceUpdatesIndex].Attribute = (IFXAuthorFaceUpdate::AttributeType)(IFXAuthorFaceUpdate::Tex0 + uLayer);
							}
							else
							{
								IFXASSERT(0);
							}
							pFaceUpdates[uFaceUpdatesIndex].FaceIndex = uMoveFaceIndex;
							pFaceUpdates[uFaceUpdatesIndex].Corner = uUpdateCorner;
							pFaceUpdates[uFaceUpdatesIndex].IncrValue = uReconstructedIncrValue;
							pFaceUpdates[uFaceUpdatesIndex].DecrValue = uReconstructedDecrValue;
							uFaceUpdatesIndex++;
						}
					}
				}

				avuReconstructedVertexUpdate.pFaceUpdates = pFaceUpdates;
				avuReconstructedVertexUpdate.NumFaceUpdates = uFaceUpdatesIndex;
				IFXASSERT(i == uCurrentResolution);
				IFXCHECKX(m_pReconstructedAuthorCLODMesh->SetVertexUpdate(i,&avuReconstructedVertexUpdate));
			}

			// For each new face
			{
				IFXASSERT(pcReconstructedMeshDescription->NumFaces == pcCurrentMeshDescription->NumFaces);
				U32 uNumFaces = pcReconstructedMeshDescription->NumFaces;
				U32 j;
				for(j=0; j < uNumNewFaces; j++)
				{

					// Identify corners
					U8 uOriginalSplitCorner = 0;
					U8 uOriginalNewCorner = 0;
					U8 uOriginalThirdCorner = 0;
					U8 uReconSplitCorner = 0;
					U8 uReconNewCorner = 0;
					U8 uReconThirdCorner = 0;
					U32 uThirdPositionIndex = 0;
					{
						IFXAuthorFace faceOriginalPositionFace;
						m_pOriginalAuthorCLODMesh->GetPositionFace(uNumFaces+j, &faceOriginalPositionFace);

						if(uCurrentResolution == faceOriginalPositionFace.VertexA()) {
							uOriginalNewCorner = 0;
							if(uSplitPositionIndex == faceOriginalPositionFace.VertexB()) {
								uOriginalSplitCorner = 1;
								uOriginalThirdCorner = 2;
							} else {
								IFXASSERT(uSplitPositionIndex == faceOriginalPositionFace.VertexC());
								uOriginalSplitCorner = 2;
								uOriginalThirdCorner = 1;
							}
						} else if (uCurrentResolution == faceOriginalPositionFace.VertexB()) {
							uOriginalNewCorner = 1;
							if(uSplitPositionIndex == faceOriginalPositionFace.VertexA()) {
								uOriginalSplitCorner = 0;
								uOriginalThirdCorner = 2;
							} else {
								IFXASSERT(uSplitPositionIndex == faceOriginalPositionFace.VertexC());
								uOriginalSplitCorner = 2;
								uOriginalThirdCorner = 0;
							}
						} else if (uCurrentResolution == faceOriginalPositionFace.VertexC()) {
							uOriginalNewCorner = 2;
							if(uSplitPositionIndex == faceOriginalPositionFace.VertexB()) {
								uOriginalSplitCorner = 1;
								uOriginalThirdCorner = 0;
							} else {
								IFXASSERT(uSplitPositionIndex == faceOriginalPositionFace.VertexA());
								uOriginalSplitCorner = 0;
								uOriginalThirdCorner = 1;
							}
						} else {
							IFXASSERT(0);
						}
					}

					{
						IFXAuthorFace faceReconPositionFace;
						m_pReconstructedAuthorCLODMesh->GetPositionFace(uNumFaces+j, &faceReconPositionFace);

						if(uCurrentResolution == faceReconPositionFace.VertexA()) {
							uReconNewCorner = 0;
							if(uSplitPositionIndex == faceReconPositionFace.VertexB()) {
								uReconSplitCorner = 1;
								uReconThirdCorner = 2;
							} else {
								IFXASSERT(uSplitPositionIndex == faceReconPositionFace.VertexC());
								uReconSplitCorner = 2;
								uReconThirdCorner = 1;
								IFXASSERT(0); // Expect third corner == 2
							}
						} else if (uCurrentResolution == faceReconPositionFace.VertexB()) {
							uReconNewCorner = 1;
							if(uSplitPositionIndex == faceReconPositionFace.VertexA()) {
								uReconSplitCorner = 0;
								uReconThirdCorner = 2;
							} else {
								IFXASSERT(uSplitPositionIndex == faceReconPositionFace.VertexC());
								uReconSplitCorner = 2;
								uReconThirdCorner = 0;
								IFXASSERT(0); // Expect third corner == 2
							}
						} else if (uCurrentResolution == faceReconPositionFace.VertexC()) {
							uReconNewCorner = 2;
							if(uSplitPositionIndex == faceReconPositionFace.VertexB()) {
								uReconSplitCorner = 1;
								uReconThirdCorner = 0;
								IFXASSERT(0); // Expect third corner == 2
							} else {
								IFXASSERT(uSplitPositionIndex == faceReconPositionFace.VertexA());
								uReconSplitCorner = 0;
								uReconThirdCorner = 1;
								IFXASSERT(0); // Expect third corner == 2
							}
						} else {
							IFXASSERT(0);
						}

						uThirdPositionIndex = faceReconPositionFace.corner[uReconThirdCorner];
					}

					// Get material info
					U32 uShadingID = 0;
					IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetFaceMaterial(uNumFaces+j,&uShadingID));

					IFXAuthorMaterial ammMeshMaterial;
					IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetMaterial(uShadingID,&ammMeshMaterial));
					BOOL bDiffuseColors = ammMeshMaterial.m_uDiffuseColors;
					BOOL bSpecularColors = ammMeshMaterial.m_uSpecularColors;
					U8 u8NumTexLayers = ammMeshMaterial.m_uNumTextureLayers;

					if(bDiffuseColors)
					{
						IFXAuthorFace faceColor, faceReconstructedColor;
						IFXCHECKX(m_pOriginalAuthorCLODMesh->GetDiffuseFace(uNumFaces+j, &faceColor));
						U32 uSplitColor = faceColor.corner[uOriginalSplitCorner];
						U32 uUpdatedColor = faceColor.corner[uOriginalNewCorner];
						U32 uThirdColor = faceColor.corner[uOriginalThirdCorner];

						U8 u8DupFaceIndexFlags = 0;
						{
							if (uSplitColor == uPrevSplitDiffColor)
								u8DupFaceIndexFlags |= u8AMPSplitColorDup;
							if (uUpdatedColor == uPrevUpdateDiffColor)
								u8DupFaceIndexFlags |= u8AMPUpdateColorDup;
							if (uThirdColor == uPrevThirdDiffColor)
								u8DupFaceIndexFlags |= u8AMPThirdColorDup;
						}
						// 4.4.6.1 Diffuse Duplicate Flag
						pBitStreamX->WriteCompressedU8X(uACContextVertColorDupType,u8DupFaceIndexFlags);

						IFXDECLARELOCAL(IFXSetX,pDiffuseSplitSetX);
						pSetAdjacencyX->GetDiffuseColorSetX(uSplitPositionIndex,pDiffuseSplitSetX);

						if (!(u8DupFaceIndexFlags & u8AMPSplitColorDup))
						{
							BOOL bLocal = FALSE;
							U32 uLocalIndex = 0;
							pDiffuseSplitSetX->GetIndexX(uSplitColor,bLocal,uLocalIndex);
							if(bLocal) {
								pBitStreamX->WriteCompressedU8X(uACContextVertColorSplitType,uAMPUpdateLocal);
								pBitStreamX->WriteCompressedU32X(uACContextVertColorSplitIndexLocal,uLocalIndex);
							} else {
								pBitStreamX->WriteCompressedU8X(uACContextVertColorSplitType,uAMPUpdateGlobal);
								pBitStreamX->WriteCompressedU32X(uACContextVertColorSplitIndexGlobal,uSplitColor);
							}
						}

						if (!(u8DupFaceIndexFlags & u8AMPUpdateColorDup))
						{
							BOOL bLocal = FALSE;
							U32 uLocalIndex = 0;
							pDiffuseSplitSetX->GetIndexX(uUpdatedColor,bLocal,uLocalIndex);
							if(bLocal) {
								pBitStreamX->WriteCompressedU8X(uACContextVertColorSplitType,uAMPUpdateLocal);
								pBitStreamX->WriteCompressedU32X(uACContextVertColorSplitIndexLocal,uLocalIndex);
							} else {
								pBitStreamX->WriteCompressedU8X(uACContextVertColorSplitType,uAMPUpdateGlobal);
								pBitStreamX->WriteCompressedU32X(uACContextVertColorSplitIndexGlobal,uUpdatedColor);
							}
						}

						IFXDECLARELOCAL(IFXSetX,pDiffuseThirdSetX);
						pSetAdjacencyX->GetDiffuseColorSetX(uThirdPositionIndex,pDiffuseThirdSetX);

						if (!(u8DupFaceIndexFlags & u8AMPThirdColorDup))
						{
							BOOL bLocal = FALSE;
							U32 uLocalIndex = 0;
							pDiffuseThirdSetX->GetIndexX(uThirdColor,bLocal,uLocalIndex);
							if(bLocal) {
								pBitStreamX->WriteCompressedU8X(uACContextVertColorSplitType,uAMPUpdateLocal);
								pBitStreamX->WriteCompressedU32X(uACContextVertColorSplitIndexLocal,uLocalIndex);
							} else {
								pBitStreamX->WriteCompressedU8X(uACContextVertColorSplitType,uAMPUpdateGlobal);
								pBitStreamX->WriteCompressedU32X(uACContextVertColorSplitIndexGlobal,uThirdColor);
							}
						}

						faceReconstructedColor.corner[uReconSplitCorner] = uSplitColor;
						faceReconstructedColor.corner[uReconNewCorner] = uUpdatedColor;
						faceReconstructedColor.corner[uReconThirdCorner] = uThirdColor;

						IFXCHECKX(m_pReconstructedAuthorCLODMesh->SetDiffuseFace(uNumFaces+j, &faceReconstructedColor));

						uPrevSplitDiffColor = uSplitColor;
						uPrevUpdateDiffColor = uUpdatedColor;
						uPrevThirdDiffColor = uThirdColor;

					}

					if(bSpecularColors)
					{

						IFXAuthorFace faceColor, faceReconstructedColor;
						IFXCHECKX(m_pOriginalAuthorCLODMesh->GetSpecularFace(uNumFaces+j, &faceColor));
						U32 uSplitColor = faceColor.corner[uOriginalSplitCorner];
						U32 uUpdatedColor = faceColor.corner[uOriginalNewCorner];
						U32 uThirdColor = faceColor.corner[uOriginalThirdCorner];

						U8 u8DupFaceIndexFlags = 0;
						{
							if (uSplitColor == uPrevSplitSpecColor)
								u8DupFaceIndexFlags |= u8AMPSplitColorDup;
							if (uUpdatedColor == uPrevUpdateSpecColor)
								u8DupFaceIndexFlags |= u8AMPUpdateColorDup;
							if (uThirdColor == uPrevThirdSpecColor)
								u8DupFaceIndexFlags |= u8AMPThirdColorDup;
						}
						// 4.4.6.1 Specular Duplicate Flag
						pBitStreamX->WriteCompressedU8X(uACContextVertColorDupType,u8DupFaceIndexFlags);

						IFXDECLARELOCAL(IFXSetX,pSpecularSplitSetX);
						pSetAdjacencyX->GetSpecularColorSetX(uSplitPositionIndex,pSpecularSplitSetX);

						if (!(u8DupFaceIndexFlags & u8AMPSplitColorDup))
						{
							BOOL bLocal = FALSE;
							U32 uLocalIndex = 0;
							pSpecularSplitSetX->GetIndexX(uSplitColor,bLocal,uLocalIndex);
							if(bLocal) {
								pBitStreamX->WriteCompressedU8X(uACContextVertColorSplitType,uAMPUpdateLocal);
								pBitStreamX->WriteCompressedU32X(uACContextVertColorSplitIndexLocal,uLocalIndex);
							} else {
								pBitStreamX->WriteCompressedU8X(uACContextVertColorSplitType,uAMPUpdateGlobal);
								pBitStreamX->WriteCompressedU32X(uACContextVertColorSplitIndexGlobal,uSplitColor);
							}
						}

						if (!(u8DupFaceIndexFlags & u8AMPUpdateColorDup))
						{
							BOOL bLocal = FALSE;
							U32 uLocalIndex = 0;
							pSpecularSplitSetX->GetIndexX(uUpdatedColor,bLocal,uLocalIndex);
							if(bLocal) {
								pBitStreamX->WriteCompressedU8X(uACContextVertColorSplitType,uAMPUpdateLocal);
								pBitStreamX->WriteCompressedU32X(uACContextVertColorSplitIndexLocal,uLocalIndex);
							} else {
								pBitStreamX->WriteCompressedU8X(uACContextVertColorSplitType,uAMPUpdateGlobal);
								pBitStreamX->WriteCompressedU32X(uACContextVertColorSplitIndexGlobal,uUpdatedColor);
							}
						}

						IFXDECLARELOCAL(IFXSetX,pSpecularThirdSetX);
						pSetAdjacencyX->GetSpecularColorSetX(uThirdPositionIndex,pSpecularThirdSetX);

						if (!(u8DupFaceIndexFlags & u8AMPThirdColorDup))
						{
							BOOL bLocal = FALSE;
							U32 uLocalIndex = 0;
							pSpecularThirdSetX->GetIndexX(uThirdColor,bLocal,uLocalIndex);
							if(bLocal) {
								pBitStreamX->WriteCompressedU8X(uACContextVertColorSplitType,uAMPUpdateLocal);
								pBitStreamX->WriteCompressedU32X(uACContextVertColorSplitIndexLocal,uLocalIndex);
							} else {
								pBitStreamX->WriteCompressedU8X(uACContextVertColorSplitType,uAMPUpdateGlobal);
								pBitStreamX->WriteCompressedU32X(uACContextVertColorSplitIndexGlobal,uThirdColor);
							}
						}

						faceReconstructedColor.corner[uReconSplitCorner] = uSplitColor;
						faceReconstructedColor.corner[uReconNewCorner] = uUpdatedColor;
						faceReconstructedColor.corner[uReconThirdCorner] = uThirdColor;

						IFXCHECKX(m_pReconstructedAuthorCLODMesh->SetSpecularFace(uNumFaces+j, &faceReconstructedColor));

						uPrevSplitSpecColor = uSplitColor;
						uPrevUpdateSpecColor = uUpdatedColor;
						uPrevThirdSpecColor = uThirdColor;
					}

					U32 uLayer = 0;
					for(uLayer = 0; uLayer < u8NumTexLayers; uLayer++)
					{
						IFXAuthorFace faceTexCoord, faceReconstructedTexCoord;
						IFXCHECKX(m_pOriginalAuthorCLODMesh->GetTexFace(uLayer,uNumFaces+j, &faceTexCoord));
						U32 uSplitTexCoord = faceTexCoord.corner[uOriginalSplitCorner];
						U32 uUpdatedTexCoord = faceTexCoord.corner[uOriginalNewCorner];
						U32 uThirdTexCoord = faceTexCoord.corner[uOriginalThirdCorner];

						U8 u8DupFaceIndexFlags = 0;
						{
							if (uSplitTexCoord == uPrevSplitTexCoord)
								u8DupFaceIndexFlags |= u8AMPSplitTexCoordDup;
							if (uUpdatedTexCoord == uPrevUpdateTexCoord)
								u8DupFaceIndexFlags |= u8AMPUpdateTexCoordDup;
							if (uThirdTexCoord == uPrevThirdTexCoord)
								u8DupFaceIndexFlags |= u8AMPThirdTexCoordDup;
						}
						// 4.4.6.1 TexCoord Duplicate Flag
						pBitStreamX->WriteCompressedU8X(uACContextTexCoordDupType,u8DupFaceIndexFlags);

						IFXDECLARELOCAL(IFXSetX,pTexCoordSplitSetX);
						pSetAdjacencyX->GetTexCoordSetX(uLayer,uSplitPositionIndex,pTexCoordSplitSetX);

						if (!(u8DupFaceIndexFlags & u8AMPSplitTexCoordDup))
						{
							BOOL bLocal = FALSE;
							U32 uLocalIndex = 0;
							pTexCoordSplitSetX->GetIndexX(uSplitTexCoord,bLocal,uLocalIndex);
							if(bLocal) {
								pBitStreamX->WriteCompressedU8X(uACContextTexCoordSplitType,uAMPUpdateLocal);
								pBitStreamX->WriteCompressedU32X(uACContextTexCoordSplitIndexLocal,uLocalIndex);
							} else {
								pBitStreamX->WriteCompressedU8X(uACContextTexCoordSplitType,uAMPUpdateGlobal);
								pBitStreamX->WriteCompressedU32X(uACContextTexCoordSplitIndexGlobal,uSplitTexCoord);
							}
						}

						if (!(u8DupFaceIndexFlags & u8AMPUpdateTexCoordDup))
						{
							BOOL bLocal = FALSE;
							U32 uLocalIndex = 0;
							pTexCoordSplitSetX->GetIndexX(uUpdatedTexCoord,bLocal,uLocalIndex);
							if(bLocal) {
								pBitStreamX->WriteCompressedU8X(uACContextTexCoordSplitType,uAMPUpdateLocal);
								pBitStreamX->WriteCompressedU32X(uACContextTexCoordSplitIndexLocal,uLocalIndex);
							} else {
								pBitStreamX->WriteCompressedU8X(uACContextTexCoordSplitType,uAMPUpdateGlobal);
								pBitStreamX->WriteCompressedU32X(uACContextTexCoordSplitIndexGlobal,uUpdatedTexCoord);
							}
						}

						IFXDECLARELOCAL(IFXSetX,pTexCoordThirdSetX);
						pSetAdjacencyX->GetTexCoordSetX(uLayer,uThirdPositionIndex,pTexCoordThirdSetX);

						if (!(u8DupFaceIndexFlags & u8AMPThirdTexCoordDup))
						{
							BOOL bLocal = FALSE;
							U32 uLocalIndex = 0;
							pTexCoordThirdSetX->GetIndexX(uThirdTexCoord,bLocal,uLocalIndex);
							if(bLocal) {
								pBitStreamX->WriteCompressedU8X(uACContextTexCoordSplitType,uAMPUpdateLocal);
								pBitStreamX->WriteCompressedU32X(uACContextTexCoordSplitIndexLocal,uLocalIndex);
							} else {
								pBitStreamX->WriteCompressedU8X(uACContextTexCoordSplitType,uAMPUpdateGlobal);
								pBitStreamX->WriteCompressedU32X(uACContextTexCoordSplitIndexGlobal,uThirdTexCoord);
							}
						}

						faceReconstructedTexCoord.corner[uReconSplitCorner] = uSplitTexCoord;
						faceReconstructedTexCoord.corner[uReconNewCorner] = uUpdatedTexCoord;
						faceReconstructedTexCoord.corner[uReconThirdCorner] = uThirdTexCoord;

						IFXCHECKX(m_pReconstructedAuthorCLODMesh->SetTexFace(uLayer,uNumFaces+j, &faceReconstructedTexCoord));

						uPrevSplitTexCoord = uSplitTexCoord;
						uPrevUpdateTexCoord = uUpdatedTexCoord;
						uPrevThirdTexCoord = uThirdTexCoord;
					}
				}
			}

			//  G. Update set adjacency for move faces
			{
				U32 j = 0;
				U32 uMoveSize = 0;
				pMoveFaceSetX->GetSizeX(uMoveSize);
				for(j=0;j<uMoveSize;j++)
				{
					U32 uMoveFaceIndex = 0;
					pMoveFaceSetX->GetMemberX(j,uMoveFaceIndex);
					pSetAdjacencyX->RemoveX(uSplitPositionIndex,uMoveFaceIndex);
					pSetAdjacencyX->AddX(uCurrentResolution,uMoveFaceIndex);
				}
			}

			//  H. Update set adjacency for new faces
			{
				IFXASSERT(pcReconstructedMeshDescription->NumFaces == pcCurrentMeshDescription->NumFaces);
				U32 uNumFaces = pcReconstructedMeshDescription->NumFaces;
				U32 j;
				for(j=0; j < uNumNewFaces; j++)
				{
					IFXAuthorFace face;
					// Get the face
					m_pReconstructedAuthorCLODMesh->GetPositionFace(uNumFaces+j, &face);
					// Add the face to the set adjacency object
					pSetAdjacencyX->AddX(face.VertexA(),uNumFaces+j);
					pSetAdjacencyX->AddX(face.VertexB(),uNumFaces+j);
					pSetAdjacencyX->AddX(face.VertexC(),uNumFaces+j);
				}
			}

			//  I.  Get set of faces using new position (set I)
			IFXDECLARELOCAL(IFXSetX,pNewFaceSetX);
			pSetAdjacencyX->GetFaceSetX(uCurrentResolution,pNewFaceSetX);

			//  J.  Get set of positions used by set I (set J)
			IFXRELEASE(pPositionSetX);
			IFXDECLARELOCAL(IFXSetX,pNewPositionSetX);
			pSetAdjacencyX->GetPositionSetX(*pNewFaceSetX,pNewPositionSetX);

			//  K. Predict position vector from set J.
			IFXVector3 vPredictedPosition;
			if(uCurrentResolution > 0) {
				m_pReconstructedAuthorCLODMesh->GetPosition(uSplitPositionIndex , &vPredictedPosition);
			} else {
				vPredictedPosition.Set(0.0,0.0,0.0);
			}

			//  L. Calculate position difference (new position vector - predicted position vector)
			IFXVector3 vPositionDifference, vPosition;
			m_pOriginalAuthorCLODMesh->GetPosition(uCurrentResolution,&vPosition);
			vPositionDifference.Subtract(vPosition,vPredictedPosition);

			//  M. Quantize position difference
			{
				U8 u8Signs = (vPositionDifference.X() < 0.0) | ((vPositionDifference.Y() < 0.0) << 1) | ((vPositionDifference.Z() < 0.0) << 2);
				U32 udX = (U32) (0.5f + m_fQuantPosition * fabs(vPositionDifference.X()));
				U32 udY = (U32) (0.5f + m_fQuantPosition * fabs(vPositionDifference.Y()));
				U32 udZ = (U32) (0.5f + m_fQuantPosition * fabs(vPositionDifference.Z()));
				pBitStreamX->WriteCompressedU8X(uACContextPositionDiffSigns,u8Signs);
				pBitStreamX->WriteCompressedU32X(uACContextPositionDiffMagX,udX);
				pBitStreamX->WriteCompressedU32X(uACContextPositionDiffMagY,udY);
				pBitStreamX->WriteCompressedU32X(uACContextPositionDiffMagZ,udZ);

				//  N. Calculate reconstructed position vector and store
				IFXVector3 vReconstructedPosition;
				if(u8Signs & 1) {
					vReconstructedPosition.X() = - m_fInverseQuantPosition * udX;
				} else {
					vReconstructedPosition.X() = m_fInverseQuantPosition * udX;
				}
				if(u8Signs & 2) {
					vReconstructedPosition.Y() = - m_fInverseQuantPosition * udY;
				} else {
					vReconstructedPosition.Y() = m_fInverseQuantPosition * udY;
				}
				if(u8Signs & 4) {
					vReconstructedPosition.Z() = - m_fInverseQuantPosition * udZ;
				} else {
					vReconstructedPosition.Z() = m_fInverseQuantPosition * udZ;
				}

				vReconstructedPosition.Add(vPredictedPosition);

				IFXCHECKX(m_pReconstructedAuthorCLODMesh->SetPosition(uCurrentResolution,&vReconstructedPosition));
			}

			// Increment the maximum resolution
			{
				// Note: the mesh cannot be locked for SetResolution() to succeed.
#ifdef _DEBUG
				U32 uTempResolution = 
#endif
					m_pReconstructedAuthorCLODMesh->SetMaxResolution(uCurrentResolution+1);
				IFXASSERT(uTempResolution == uCurrentResolution+1);
			}

			// Calculate Normals and Normal Face Updates for each position in set J
			// Note: Face Normals should be calculated with the positions already updated (resolution incremented)
			// Note: Vertex updates and face updates should only be modified above the current resolution (resolution not incremented)
			if(!bExcludeNormals)
			{
				// Declare temp storage arrays (allocated below)
				IFXVector3* pvNewNormal = NULL;
				U32* puNewNormalIndex = NULL;
				U32 uNormalArraySize = 10;
				U32 uNormalArrayGrowthSize = 6;
				U32 uNumNewNormals = 0;

				IFXAuthorFaceUpdate* pafuNormalFaceUpdates = NULL;
				U32 uFaceUpdateArraySize = 10;
				U32 uFaceUpdateArrayGrowthSize = 6;
				U32 uNumFaceUpdates = 0;

				// Allocate temp storage arrays
				pvNewNormal = new IFXVector3[uNormalArraySize];
				if(NULL == pvNewNormal ) {
					IFXCHECKX(IFX_E_OUT_OF_MEMORY);
				}
				puNewNormalIndex = new U32[uNormalArraySize];
				if(NULL == puNewNormalIndex ) {
					IFXCHECKX(IFX_E_OUT_OF_MEMORY);
				}
				pafuNormalFaceUpdates = new IFXAuthorFaceUpdate[uFaceUpdateArraySize];
				if(NULL == pafuNormalFaceUpdates ) {
					IFXCHECKX(IFX_E_OUT_OF_MEMORY);
				}
				memset(puNewNormalIndex,0,sizeof(U32)*uNormalArraySize);
				memset(pvNewNormal,0,sizeof(IFXVector3)*uNormalArraySize);
				memset(pafuNormalFaceUpdates,0,sizeof(IFXAuthorFaceUpdate)*uFaceUpdateArraySize);
				/// @todo: Proper try/catch structure, or use IFXAutoPtr

				U32 uNumValidNormalFaces = pcReconstructedMeshDescription->NumFaces;
				U32 uNumOldNormals = pcReconstructedMeshDescription->NumNormals;

				// Increment the current resolution
				{
					++uCurrentResolution;
					// Note: the mesh cannot be locked for SetResolution() to succeed.
#ifdef _DEBUG
					U32 uTempResolution = 
#endif
						m_pOriginalAuthorCLODMesh->SetResolution(uCurrentResolution);
					IFXASSERT(uTempResolution == uCurrentResolution);
#ifdef _DEBUG
					uTempResolution = 
#endif
						m_pReconstructedAuthorCLODMesh->SetResolution(uCurrentResolution);
					IFXASSERT(uTempResolution == uCurrentResolution);
				}

				
				// Steps for coding the normals
				//
				// For each position in the position set:
				// - Calculate prediction normal by averaging the face normals of all faces using the position
				// - Count number of unique normals at the position in the original mesh (use the crease parameter)
				// - Write the number of new normals at this position (count of unique normals from above)
				// - For each new normal
				// - - Write the quantized prediction difference
				// - - Store reconstructed normals in local new normal array
				// - For each face using the position
				// - - Write local index into local new normal array for normal used by this face
				// - - Update reconstructed normal face
				// - - If the change is an old face
				// - - - Create a normal face update
				// - - - Add the normal face update to the master face update array
				// - Copy local new normal array to master new normal array
				// Modify reconstructed vertex update
				// - Set number of new normals
				// - Append master face update array to face updates
				// Set the new normals to the reconstructed mesh
				// Set the reconstucted vertex update

				// For each position in the position set:
				U32 j = 0;
				U32 uPositionSetSize = 0;
				pNewPositionSetX->GetSizeX(uPositionSetSize);
				for(j=0; j < uPositionSetSize; j++)
				{
					// Get the position index and set of faces using that position
					U32 uPositionIndex = 0;
					pNewPositionSetX->GetMemberX(j,uPositionIndex);
					IFXDECLARELOCAL(IFXSetX,pNormalFaceSetX);
					pSetAdjacencyX->GetFaceSetX(uPositionIndex,pNormalFaceSetX);

					U32 uNormalFaceSetSize = 0;
					pNormalFaceSetX->GetSizeX(uNormalFaceSetSize);

					// Set size of local normal arrays
					U32 uLocalNormalArraySize = uNormalFaceSetSize;

					// - Count number of unique normals at the position in the original mesh (use the crease parameter)
					U32 uNumOriginalLocalNormals = 0;

					IFXVector3* pvOriginalLocalNormal = new IFXVector3[uLocalNormalArraySize];
					if(NULL == pvOriginalLocalNormal) {
						IFXCHECKX(IFX_E_OUT_OF_MEMORY);
					}
					memset(pvOriginalLocalNormal,0,sizeof(IFXVector3)*uLocalNormalArraySize);

					U32* puOriginalLocalNormalIndex = new U32[uLocalNormalArraySize];
					if(NULL == puOriginalLocalNormalIndex) {
						IFXCHECKX(IFX_E_OUT_OF_MEMORY);
					}
					memset(puOriginalLocalNormalIndex,0,sizeof(U32)*uLocalNormalArraySize);

					{
						U32 k = 0;
						// For each face
						for(k=0; k < uNormalFaceSetSize; k++)
						{
							// Get the normal
							IFXVector3 vOriginalNormal;
							U32 uNormalFaceIndex = 0;
							pNormalFaceSetX->GetMemberX(k,uNormalFaceIndex);
							IFXAuthorFace faceOriginalPosition, faceOriginalNormal;
							IFXCHECKX(m_pOriginalAuthorCLODMesh->GetPositionFace(uNormalFaceIndex,&faceOriginalPosition));
							IFXCHECKX(m_pOriginalAuthorCLODMesh->GetNormalFace(uNormalFaceIndex,&faceOriginalNormal));
							U32 uOriginalNormalIndex = 0;
							if(uPositionIndex == faceOriginalPosition.VertexA()) {
								uOriginalNormalIndex = faceOriginalNormal.VertexA();
							} else if(uPositionIndex == faceOriginalPosition.VertexB()) {
								uOriginalNormalIndex = faceOriginalNormal.VertexB();
							} else if(uPositionIndex == faceOriginalPosition.VertexC()) {
								uOriginalNormalIndex = faceOriginalNormal.VertexC();
							} else {
								IFXASSERT(0);
							}
							IFXCHECKX(m_pOriginalAuthorCLODMesh->GetNormal(uOriginalNormalIndex,&vOriginalNormal));

							// Compare the normal with the normals already in the array
							BOOL bMatchFound = FALSE;
							U32 l = 0;
							for(l=0; l < uNumOriginalLocalNormals; l++)
							{
								if(vOriginalNormal.DotProduct(pvOriginalLocalNormal[l]) > m_fNormalCreaseParameter) {
									bMatchFound = TRUE;
								}
							}
							if(!bMatchFound) {
								// Re-allocate if needed
								IFXASSERT(uNumOriginalLocalNormals < uLocalNormalArraySize);

								// Add the normal to the local array
								pvOriginalLocalNormal[uNumOriginalLocalNormals] = vOriginalNormal;
								puOriginalLocalNormalIndex[uNumOriginalLocalNormals] = uOriginalNormalIndex;
								uNumOriginalLocalNormals++;
							}
						}
					}

					// - Write the number of new normals at this position (count of unique normals from above)
					pBitStreamX->WriteCompressedU32X(uACContextNumLocalNormals,uNumOriginalLocalNormals);

					U32 uNumPredictedLocalNormals = 0;
					IFXVector3* pvPredictedLocalNormal = new IFXVector3[uLocalNormalArraySize];
					if(NULL == pvPredictedLocalNormal) {
						IFXCHECKX(IFX_E_OUT_OF_MEMORY);
					}
					memset(pvPredictedLocalNormal,0,sizeof(IFXVector3)*uLocalNormalArraySize);

					// - Calculate prediction normals from face normals of all faces using the position
					{
						U32 uNumFaceLocalNormals = 0;
						IFXVector3* pvFaceLocalNormal = new IFXVector3[uLocalNormalArraySize];
						if(NULL == pvFaceLocalNormal) {
							IFXCHECKX(IFX_E_OUT_OF_MEMORY);
						}
						memset(pvFaceLocalNormal,0,sizeof(IFXVector3)*uLocalNormalArraySize);

						// Put all the face normals into the face local normal array
						U32 k = 0;
						for(k=0; k < uNormalFaceSetSize; k++)
						{
							IFXVector3 vFaceNormal;
							U32 uNormalFaceIndex = 0;
							pNormalFaceSetX->GetMemberX(k,uNormalFaceIndex);

							CalculateFaceNormalX(uNormalFaceIndex,vFaceNormal);

							if(vFaceNormal.X() > -2.0f)
							{ // Check for valid normal
								IFXASSERT(uNumFaceLocalNormals < uLocalNormalArraySize);
								pvFaceLocalNormal[uNumFaceLocalNormals] = vFaceNormal;
								uNumFaceLocalNormals++;
							}
						}

						// Put the first face normal into the local normal array
						if(uNumOriginalLocalNormals > 0 && uNumFaceLocalNormals > 0)
						{
							pvPredictedLocalNormal[uNumPredictedLocalNormals] = pvFaceLocalNormal[0];
							uNumPredictedLocalNormals++;
						}

						// While number of local normals is less than the number of original local normals
						while(uNumPredictedLocalNormals < uNumOriginalLocalNormals)
						{
							// Find the face local normal which is farthest from any normal in the local normal array
							// "Distance" is measured by dot product.  1.0 is closest, -1.0 is farthest.

							F32 fFarthestDistance = 1.0f;
							U32 uIndexFarthestNormal = 0;

							for(k=0; k < uNumFaceLocalNormals; k++)
							{
								// calculate minimum distance to an element in the local normal array
								F32 fMinimumDistance = -2.0f;
								IFXVector3 vTemp = pvFaceLocalNormal[k];
								U32 l = 0;
								for(l=0; l < uNumPredictedLocalNormals; l++)
								{
									F32 fDistance = vTemp.DotProduct(pvPredictedLocalNormal[l]);
									if(fDistance > fMinimumDistance) {
										fMinimumDistance = fDistance;
									}
								}

								// update farthest distance if needed
								if(fMinimumDistance < fFarthestDistance)
								{
									fFarthestDistance = fMinimumDistance;
									uIndexFarthestNormal = k;
								}
							}

							// Add the farthest face local normal to the local normal array
							pvPredictedLocalNormal[uNumPredictedLocalNormals] = pvFaceLocalNormal[uIndexFarthestNormal];
							uNumPredictedLocalNormals++;
						}

						// Allocate prediction contribution count array
						U32* puPredictedContributionCount = new U32[uLocalNormalArraySize];
						if(NULL == puPredictedContributionCount)
							IFXCHECKX(IFX_E_OUT_OF_MEMORY);

						memset(puPredictedContributionCount,0,sizeof(U32)*uLocalNormalArraySize);

						// For each face normal in local face normal array
						for(k=0; k < uNumFaceLocalNormals; k++)
						{
							// Find closest predicted normal to the face normal
							U32 uIndexClosestPrediction = 0;
							IFXVector3 vFaceNormal = pvFaceLocalNormal[k];
							{
								F32 fMinimumDistance = -2.0f;
								U32 l = 0;
								for(l=0; l < uNumPredictedLocalNormals; l++)
								{
									F32 fDistance = vFaceNormal.DotProduct(pvPredictedLocalNormal[l]);
									if(fDistance > fMinimumDistance)
									{
										fMinimumDistance = fDistance;
										uIndexClosestPrediction = l;
									}
								}
							}

							// Integrate face normal into the closest predicted normal
							{
								IFXQuaternion qFaceNormal(vFaceNormal);
								IFXQuaternion qPredictedNormal(pvPredictedLocalNormal[uIndexClosestPrediction]);
								IFXQuaternion qUpdatedPredictedNormal;

								F32 fWeight = 1.0f / (puPredictedContributionCount[uIndexClosestPrediction] + 1);
								qUpdatedPredictedNormal.Interpolate(fWeight, qPredictedNormal, qFaceNormal);

								IFXVector3 vUpdatedPredictedNormal;
								vUpdatedPredictedNormal.X() = qUpdatedPredictedNormal[1];
								vUpdatedPredictedNormal.Y() = qUpdatedPredictedNormal[2];
								vUpdatedPredictedNormal.Z() = qUpdatedPredictedNormal[3];
								pvPredictedLocalNormal[uIndexClosestPrediction] = vUpdatedPredictedNormal;
							}

							// Increment prediction contribution count
							puPredictedContributionCount[uIndexClosestPrediction] ++;

						}

						IFXDELETE_ARRAY( puPredictedContributionCount );
						IFXDELETE_ARRAY( pvFaceLocalNormal );
					}

					// - Set up local normal array
					// Local normal array contains original normals sorted to align with predicted normals

					U32 uNumLocalNormals = 0;
					IFXVector3* pvLocalNormal = new IFXVector3[uLocalNormalArraySize];
					if(NULL == pvLocalNormal)
						IFXCHECKX(IFX_E_OUT_OF_MEMORY);

					memset(pvLocalNormal,0,sizeof(IFXVector3)*uLocalNormalArraySize);

					U32* puLocalNormalIndex = new U32[uLocalNormalArraySize];
					if(NULL == puLocalNormalIndex)
						IFXCHECKX(IFX_E_OUT_OF_MEMORY);

					memset(puLocalNormalIndex,0,sizeof(U32)*uLocalNormalArraySize);

					{
						while (uNumLocalNormals < uNumOriginalLocalNormals)
						{
							U32 uBestMatch = 0;
							F32 fBestMatchValue = -2.0f;
							U32  k=0;
							IFXVector3 vTemp = pvPredictedLocalNormal[uNumLocalNormals];
							for(k = 0; k < uNumOriginalLocalNormals; k++)
							{
								F32 fTemp = vTemp.DotProduct(pvOriginalLocalNormal[k]);
								if(fTemp > fBestMatchValue)
								{
									fBestMatchValue = fTemp;
									uBestMatch = k;
								}
							}

							pvLocalNormal[uNumLocalNormals] = pvOriginalLocalNormal[uBestMatch];
							puLocalNormalIndex[uNumLocalNormals] = puOriginalLocalNormalIndex[uBestMatch];
							uNumLocalNormals++;
						}
					}

					// - For each new normal
					{
						U32 k = 0;
						// For each new normal
						for(k=0; k < uNumLocalNormals; k++)
						{
							IFXQuaternion qPredictedNormal(pvPredictedLocalNormal[k]);
							IFXQuaternion qPredictedNormalInverted(pvPredictedLocalNormal[k]);
							qPredictedNormalInverted.Invert();
							IFXQuaternion qOriginalNormal(pvLocalNormal[k]);
							IFXQuaternion qPredictedDifference;
							qPredictedDifference.Multiply(qPredictedNormalInverted,qOriginalNormal);
							// - - Write the quantized prediction difference
							U8 u8Signs = (qPredictedDifference[0] < 0.0) | ((qPredictedDifference[1] < 0.0) << 1) | ((qPredictedDifference[2] < 0.0) << 2) | ((qPredictedDifference[3] < 0.0) << 3);
							U32 udX = (U32) (0.5f + m_fQuantNormal * fabs(qPredictedDifference[1]));
							U32 udY = (U32) (0.5f + m_fQuantNormal * fabs(qPredictedDifference[2]));
							U32 udZ = (U32) (0.5f + m_fQuantNormal * fabs(qPredictedDifference[3]));
							{
								// Short cut for small differences
								if(pvPredictedLocalNormal[k].DotProduct(pvLocalNormal[k]) > m_fNormalUpdateParameter)
								{
									u8Signs = 0;
									udX = 0;
									udY = 0;
									udZ = 0;
								} else {
									//IFXASSERT(0);
								}
							}
							pBitStreamX->WriteCompressedU8X(uACContextNormalDiffSigns,u8Signs);
							pBitStreamX->WriteCompressedU32X(uACContextNormalDiffMagX,udX);
							pBitStreamX->WriteCompressedU32X(uACContextNormalDiffMagY,udY);
							pBitStreamX->WriteCompressedU32X(uACContextNormalDiffMagZ,udZ);

							// - - Store reconstructed normals in local new normal array
							F32 fdW, fdX, fdY, fdZ;

							if(u8Signs & 2) {
								fdX = - m_fInverseQuantNormal * udX;
							} else {
								fdX = m_fInverseQuantNormal * udX;
							}
							if(u8Signs & 4) {
								fdY = - m_fInverseQuantNormal * udY;
							} else {
								fdY = m_fInverseQuantNormal * udY;
							}
							if(u8Signs & 8) {
								fdZ = - m_fInverseQuantNormal * udZ;
							} else {
								fdZ = m_fInverseQuantNormal * udZ;
							}
							F32 fTemp = (fdX*fdX) + (fdY*fdY) + (fdZ*fdZ);
							if(fTemp > 1.0f) {
								fTemp = 1.0f;
							}
							if(u8Signs & 1) {
								fdW = - sqrtf(1.0f - fTemp);
							} else {
								fdW = sqrtf(1.0f - fTemp);
							}
							IFXQuaternion qReconstructedDifference(fdW,fdX,fdY,fdZ);
							//qReconstructedDifference = qPredictedDifference; // DEBUG ONLY
							IFXQuaternion qReconstructedNormal;
							qReconstructedNormal.Multiply(qPredictedNormal,qReconstructedDifference);

							pvLocalNormal[k].Set(qReconstructedNormal[1],qReconstructedNormal[2],qReconstructedNormal[3]);
						}
					}

					// - For each face using the position
					{
						U32 k = 0;
						for(k=0; k < uNormalFaceSetSize; k++)
						{
							IFXVector3 vOriginalNormal;
							U32 uNormalFaceIndex = 0;
							pNormalFaceSetX->GetMemberX(k,uNormalFaceIndex);

							IFXAuthorFace faceOriginalPosition, faceOriginalNormal;
							IFXCHECKX(m_pOriginalAuthorCLODMesh->GetPositionFace(uNormalFaceIndex,&faceOriginalPosition));
							IFXCHECKX(m_pOriginalAuthorCLODMesh->GetNormalFace(uNormalFaceIndex,&faceOriginalNormal));
							U32 uOriginalNormalIndex = 0;
							if(uPositionIndex == faceOriginalPosition.VertexA())
							{
								uOriginalNormalIndex = faceOriginalNormal.VertexA();
							}
							else if(uPositionIndex == faceOriginalPosition.VertexB())
							{
								uOriginalNormalIndex = faceOriginalNormal.VertexB();
							}
							else if(uPositionIndex == faceOriginalPosition.VertexC())
							{
								uOriginalNormalIndex = faceOriginalNormal.VertexC();
							}
							else
							{
								IFXASSERT(0);
							}
							IFXCHECKX(m_pOriginalAuthorCLODMesh->GetNormal(uOriginalNormalIndex,&vOriginalNormal));

							// Compare the normal with the normals already in the array
							F32 fBestMatch = -2.0f;
							U32 uBestLocalIndex = (U32) -1;
							U32 l = 0;
							for(l=0; l < uNumLocalNormals; l++)
							{
								F32 fMatchValue = vOriginalNormal.DotProduct(pvLocalNormal[l]);
								if(fMatchValue > fBestMatch)
								{
									fBestMatch = fMatchValue;
									uBestLocalIndex = l;
								}
							}

							// - - Write local index into local new normal array for normal used by this face
							pBitStreamX->WriteCompressedU32X(uACContextNormalLocalIndex,uBestLocalIndex);
							// - - If the normal face is a new normal face
							if(uNormalFaceIndex >= uNumValidNormalFaces )
							{
								// - - - Update reconstructed normal face
								IFXAuthorFace faceReconstructedPosition, faceReconstructedNormal;
								IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetPositionFace(uNormalFaceIndex,&faceReconstructedPosition));
								IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetNormalFace(uNormalFaceIndex,&faceReconstructedNormal));
								U32 uReconstructedNormalIndex = uNumOldNormals + uNumNewNormals + uBestLocalIndex;
								if(uPositionIndex == faceReconstructedPosition.VertexA())
								{
									faceReconstructedNormal.VertexA() = uReconstructedNormalIndex;
								}
								else if(uPositionIndex == faceReconstructedPosition.VertexB())
								{
									faceReconstructedNormal.VertexB() = uReconstructedNormalIndex;
								}
								else if(uPositionIndex == faceReconstructedPosition.VertexC())
								{
									faceReconstructedNormal.VertexC() = uReconstructedNormalIndex;
								}
								else
								{
									IFXASSERT(0);
								}
								IFXCHECKX(m_pReconstructedAuthorCLODMesh->SetNormalFace(uNormalFaceIndex,&faceReconstructedNormal));
							}
							else
							{ // - - Else If the normal face is an old normal face
								// - - - Create a normal face update
								IFXAuthorFace faceReconstructedPosition, faceReconstructedNormal;
								IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetPositionFace(uNormalFaceIndex,&faceReconstructedPosition));
								IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetNormalFace(uNormalFaceIndex,&faceReconstructedNormal));
								U32 uReconstructedNormalIndex = uNumOldNormals + uNumNewNormals + uBestLocalIndex;
								IFXAuthorFaceUpdate afuNewUpdate;

								afuNewUpdate.FaceIndex = uNormalFaceIndex;
								afuNewUpdate.Attribute = IFXAuthorFaceUpdate::Normal;
								afuNewUpdate.IncrValue = uReconstructedNormalIndex;

								if(uPositionIndex == faceReconstructedPosition.VertexA())
								{
									afuNewUpdate.Corner = 0;
									afuNewUpdate.DecrValue = faceReconstructedNormal.VertexA();
								}
								else if(uPositionIndex == faceReconstructedPosition.VertexB())
								{
									afuNewUpdate.Corner = 1;
									afuNewUpdate.DecrValue = faceReconstructedNormal.VertexB();
								}
								else if(uPositionIndex == faceReconstructedPosition.VertexC())
								{
									afuNewUpdate.Corner = 2;
									afuNewUpdate.DecrValue = faceReconstructedNormal.VertexC();
								}
								else
								{
									IFXASSERT(0);
								}

								// - - - Add the normal face update to the master face update array
								{
									// Re-allocate if needed
									if(uNumFaceUpdates == uFaceUpdateArraySize)
									{
										IFXAuthorFaceUpdate* pafuTemp = pafuNormalFaceUpdates;
										pafuNormalFaceUpdates = new IFXAuthorFaceUpdate[uFaceUpdateArraySize+uFaceUpdateArrayGrowthSize];
										if(NULL == pafuNormalFaceUpdates)
											IFXCHECKX(IFX_E_OUT_OF_MEMORY);

										memset(pafuNormalFaceUpdates+uFaceUpdateArraySize,0,sizeof(IFXAuthorFaceUpdate)*uFaceUpdateArrayGrowthSize);
										memcpy(pafuNormalFaceUpdates,pafuTemp,sizeof(IFXAuthorFaceUpdate)*uFaceUpdateArraySize);
										uFaceUpdateArraySize += uFaceUpdateArrayGrowthSize;

										IFXDELETE_ARRAY(pafuTemp);
									}

									// Add the normal face update to the master face update array
									pafuNormalFaceUpdates[uNumFaceUpdates] = afuNewUpdate;
									uNumFaceUpdates++;
								}
							}
						}
					}

					// - Copy local new normal array to master new normal array
					{
						U32 k = 0;

						for(k = 0; k < uNumLocalNormals; k++ )
						{
							// Re-allocate if needed
							if(uNumNewNormals == uNormalArraySize)
							{
								IFXVector3* pvTemp = pvNewNormal;
								pvNewNormal = new IFXVector3[uNormalArraySize+uNormalArrayGrowthSize];
								if(NULL == pvNewNormal)
								{
									IFXCHECKX(IFX_E_OUT_OF_MEMORY);
								}
								memset(pvNewNormal+uNormalArraySize,0,sizeof(IFXVector3)*uNormalArrayGrowthSize);
								memcpy(pvNewNormal,pvTemp,sizeof(IFXVector3)*uNormalArraySize);

								U32* puTemp = puNewNormalIndex;
								puNewNormalIndex = new U32[uNormalArraySize+uNormalArrayGrowthSize];
								if(NULL == puNewNormalIndex)
									IFXCHECKX(IFX_E_OUT_OF_MEMORY);

								memset(puNewNormalIndex+uNormalArraySize,0,sizeof(U32)*uNormalArrayGrowthSize);
								memcpy(puNewNormalIndex,puTemp,sizeof(U32)*uNormalArraySize);

								uNormalArraySize += uNormalArrayGrowthSize;

								IFXDELETE_ARRAY(pvTemp);
								IFXDELETE_ARRAY(puTemp);
							}

							// Add the normal to the local array
							pvNewNormal[uNumNewNormals] = pvLocalNormal[k];
							puNewNormalIndex[uNumNewNormals] = puLocalNormalIndex[k];
							uNumNewNormals++;
						}
					}

					IFXDELETE_ARRAY(pvLocalNormal);
					IFXDELETE_ARRAY(puLocalNormalIndex);
					IFXDELETE_ARRAY(pvOriginalLocalNormal);
					IFXDELETE_ARRAY(puOriginalLocalNormalIndex);
					IFXDELETE_ARRAY(pvPredictedLocalNormal);
				}

				// Modify reconstructed vertex update
				{
					// - Set number of new normals
					avuReconstructedVertexUpdate.NumNewNormals = uNumNewNormals;
					// - Append master face update array to face updates
					U32 uOldNumFaceUpdates = avuReconstructedVertexUpdate.NumFaceUpdates;

					if (uNumFaceUpdates+uOldNumFaceUpdates>0)
					{
						IFXAuthorFaceUpdate* pafuOldFaceUpdates = avuReconstructedVertexUpdate.pFaceUpdates;

						avuReconstructedVertexUpdate.pFaceUpdates = new IFXAuthorFaceUpdate[uNumFaceUpdates+uOldNumFaceUpdates];
						if(NULL == avuReconstructedVertexUpdate.pFaceUpdates)
						{
							IFXCHECKX(IFX_E_OUT_OF_MEMORY);
						}
						//memset(avuReconstructedVertexUpdate.pFaceUpdates,0,sizeof(IFXAuthorFaceUpdate)*(uNumFaceUpdates+uOldNumFaceUpdates));
						memcpy(avuReconstructedVertexUpdate.pFaceUpdates,pafuOldFaceUpdates,sizeof(IFXAuthorFaceUpdate)*(uOldNumFaceUpdates));
						memcpy(avuReconstructedVertexUpdate.pFaceUpdates+uOldNumFaceUpdates,pafuNormalFaceUpdates,sizeof(IFXAuthorFaceUpdate)*(uNumFaceUpdates));

						avuReconstructedVertexUpdate.NumFaceUpdates += uNumFaceUpdates;

						IFXDELETE_ARRAY(pafuOldFaceUpdates);
					}
				}

				U32 k = 0;
				for(k=0; k < uNumNewNormals; k++)
				{
					IFXCHECKX(m_pReconstructedAuthorCLODMesh->SetNormal(uNumOldNormals+k, pvNewNormal +k));
					IFXCHECKX(pNewMeshMap->AddMappingToMap(2,puNewNormalIndex[k],0,uNumOldNormals+k));
				}

				IFXDELETE_ARRAY(pvNewNormal);
				IFXDELETE_ARRAY(puNewNormalIndex);
				IFXDELETE_ARRAY(pafuNormalFaceUpdates);

				// Decrement the current resolution
				{
					--uCurrentResolution;
					// Note: the mesh cannot be locked for SetResolution() to succeed.
#ifdef _DEBUG
					U32 uTempResolution = 
#endif
						m_pOriginalAuthorCLODMesh->SetResolution(uCurrentResolution);
					IFXASSERT(uTempResolution == uCurrentResolution);
#ifdef _DEBUG
					uTempResolution = 
#endif
						m_pReconstructedAuthorCLODMesh->SetResolution(uCurrentResolution);
					IFXASSERT(uTempResolution == uCurrentResolution);
				}
			}

			IFXCHECKX(m_pReconstructedAuthorCLODMesh->SetVertexUpdate(uCurrentResolution,&avuReconstructedVertexUpdate));

			// Increment the current resolution
			{
				++uCurrentResolution;
				// Note: the mesh cannot be locked for SetResolution() to succeed.
				// ASSERT ( the meshes are not locked )
#ifdef _DEBUG
				U32 uTempResolution = 
#endif
					m_pOriginalAuthorCLODMesh->SetResolution(uCurrentResolution);
				IFXASSERT(uTempResolution == uCurrentResolution);
#ifdef _DEBUG
				uTempResolution = 
#endif
					m_pReconstructedAuthorCLODMesh->SetResolution(uCurrentResolution);
				IFXASSERT(uTempResolution == uCurrentResolution);
			}
		}

		// Get the data block
		IFXDECLARELOCAL(IFXDataBlockX,pDataBlockX);
		pBitStreamX->GetDataBlockX(pDataBlockX);
		IFXASSERT(pDataBlockX);

		// Set block type
		pDataBlockX->SetBlockTypeX( BlockType_GeneratorCLODProgressiveMeshContinuationU3D );
		// Set block priority
		pDataBlockX->SetPriorityX(m_uPriorityCurrent);
		m_uPriorityCurrent += m_uPriorityIncrement;

		// Add block to queue
		rDataBlockQueue.AppendBlockX(*pDataBlockX);

		uNumResCoded = uEndResolution;
	}

	// Update the mesh map
	{
		IFXDECLARELOCAL( IFXMeshMap, pOutputMeshMap );
		IFXCHECKX( IFXCreateComponent( CID_IFXMeshMap, IID_IFXMeshMap, (void**)&pOutputMeshMap ) );

		IFXCHECKX( pOutputMeshMap->Allocate( rpMeshMap ) );

		IFXCHECKX(pNewMeshMap->ConcatenateMeshMap(rpMeshMap,&pOutputMeshMap));

		rpMeshMap->Release();
		rpMeshMap = pOutputMeshMap;
		rpMeshMap->AddRef();
	}
}
