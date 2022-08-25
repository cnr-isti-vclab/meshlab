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
//  CIFXAuthorCLODDecoder_Progressive.cpp
//
//  DESCRIPTION
//
//  NOTES
//
//***************************************************************************

#include "CIFXAuthorCLODDecoder.h"
#include "IFXCoreCIDs.h"
#include "IFXBlockTypes.h"
#include "IFXACContext.h"
#include "IFXCheckX.h"
#include "IFXExportingCIDs.h"

const U32 CIFXAuthorCLODDecoder::m_uMaximumUpdatesPerCall = 64;

void CIFXAuthorCLODDecoder::CalculateFaceNormalX(U32 uFaceIndex, IFXVector3& rvFaceNormal)
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


// Process the model resource progressive geometry block

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

void CIFXAuthorCLODDecoder::ProcessAuthorCLODProgressiveGeometryBlocksX( U32 uBlockType )
{
	IFXASSERT(m_pBitStreamX);

	if (m_uWIPi == 0) {
		// Get exclude normals flag (set during declaration block processing)
		m_bExcludeNormals = FALSE;
		IFXASSERT(m_pAuthorCLODResource);
		m_pAuthorCLODResource->GetExcludeNormals(m_bExcludeNormals);

		//  1. Name
		IFXString stringName;
		m_pBitStreamX->ReadIFXStringX(stringName);

		// Set resolution to minimum and get the mesh description
		U32 uMinResolution = m_pReconstructedAuthorCLODMesh->GetMinResolution();
		m_uCurrentResolution = m_pReconstructedAuthorCLODMesh->SetResolution(uMinResolution);
		IFXASSERT(uMinResolution == m_uCurrentResolution);

		samdMinMeshDescription = * (m_pReconstructedAuthorCLODMesh->GetMeshDesc());
		samdMaxMeshDescription = * (m_pReconstructedAuthorCLODMesh->GetMaxMeshDesc());
		pcReconstructedMeshDescription = m_pReconstructedAuthorCLODMesh->GetMeshDesc();

		//  2. Chain Index (always zero for this block)
		IFXASSERT( uBlockType == BlockType_GeneratorCLODProgressiveMeshContinuationU3D );
		{
			U32 uChainIndex;
			m_pBitStreamX->ReadU32X( uChainIndex );
			IFXASSERT(0 == uChainIndex);
		}

		//  3. Resolution Update Range
		m_uCurrentTexCoordCount = 0;
		m_uCurrentNumDiffuseColors = 0;
		m_uCurrentNumSpecularColors = 0;

		m_uPrevSplitDiffColor  = 0;
		m_uPrevUpdateDiffColor = 0;
		m_uPrevThirdDiffColor  = 0;
		m_uPrevSplitSpecColor  = 0;
		m_uPrevUpdateSpecColor = 0;
		m_uPrevThirdSpecColor  = 0;
		m_uPrevSplitTexCoord  = 0;
		m_uPrevUpdateTexCoord = 0;
		m_uPrevThirdTexCoord  = 0;

		m_pBitStreamX->ReadU32X(m_uStartResolution);
		m_pBitStreamX->ReadU32X(m_uEndResolution);
		m_uCurrentResolution = m_pReconstructedAuthorCLODMesh->SetResolution(m_uStartResolution);
		IFXASSERT(m_uStartResolution == m_uCurrentResolution);

		m_uWIPi = m_uStartResolution;
	}

	U32 i, i0 = m_uWIPi;
	for (i = m_uWIPi; (i < m_uEndResolution) && (i < i0+m_uMaximumUpdatesPerCall); i++, m_uWIPi++) {

		// Get the vertex updates
		IFXAuthorVertexUpdate avuReconstructedVertexUpdate;

		// 4. Resolution Updates
		const IFXAuthorVertexUpdate *pConstVertexUpdate = NULL;
   
   		// 4. Resolution Updates
		IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetVertexUpdate(i,pConstVertexUpdate));
		avuReconstructedVertexUpdate = *pConstVertexUpdate;

		//  4.1 split position index
		U32 uSplitPositionIndex = 0;
		m_pBitStreamX->ReadCompressedU32X(uACStaticFull+i,uSplitPositionIndex);

		//  B. Get set of faces using split position (set B)
		IFXDECLARELOCAL(IFXSetX,pSplitFaceSetX);
		m_pSetAdjacencyX->GetFaceSetX(uSplitPositionIndex,pSplitFaceSetX);

		//  C. Get set of positions used by set B (set C)
		IFXDECLARELOCAL(IFXSetX,pPositionSetX);
		m_pSetAdjacencyX->GetPositionSetX(*pSplitFaceSetX,pPositionSetX);
		pPositionSetX->RemoveX(uSplitPositionIndex);

		// New Diffuse Color Count
		U16 uNumNewDiffuseColors = 0;
		m_pBitStreamX->ReadCompressedU16X(uACContextNewDiffuseColorsCount,uNumNewDiffuseColors);
		avuReconstructedVertexUpdate.NumNewDiffuseColors = uNumNewDiffuseColors;

		// New Diffuse Colors
		if(uNumNewDiffuseColors > 0) {
			// Calculate predicted Diffuse vertex color
			IFXVector4 v4PredictedVertColor;
			{
				IFXVector4 v4Acc, v4VertColor;
				v4Acc.Set(0.0f,0.0f,0.0f,0.0f);
				// Get the set of diffuse colors used at the split position
				IFXDECLARELOCAL(IFXSetX,pSplitDiffuseColorSet);
				m_pSetAdjacencyX->GetDiffuseColorSetX(uSplitPositionIndex,pSplitDiffuseColorSet);
				U32 uSize = 0;
				pSplitDiffuseColorSet->GetSizeX(uSize);

				U32 j;
				for(j=0; j < uSize; j++) {
					U32 uIndex = 0;
					pSplitDiffuseColorSet->GetMemberX(j,uIndex);
					m_pReconstructedAuthorCLODMesh->GetDiffuseColor(uIndex,&v4VertColor);
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

			// read quantized prediction difference and calculate reconstructed color
			{
				U32 j=0;
				for(j=0;j<uNumNewDiffuseColors;j++) {
					U32 uVertColorIndex = pcReconstructedMeshDescription->NumDiffuseColors + j;

					U8 u8Signs = 0;
					U32 udR = 0;
					U32 udG = 0;
					U32 udB = 0;
					U32 udA = 0;

					m_pBitStreamX->ReadCompressedU8X(uACContextDiffuseColorSign,u8Signs);
					m_pBitStreamX->ReadCompressedU32X(uACContextVertexColorDiffMagR,udR);
					m_pBitStreamX->ReadCompressedU32X(uACContextVertexColorDiffMagG,udG);
					m_pBitStreamX->ReadCompressedU32X(uACContextVertexColorDiffMagB,udB);
					m_pBitStreamX->ReadCompressedU32X(uACContextVertexColorDiffMagA,udA);

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

		// New Spcular Color Count
		U16 uNumNewSpecularColors = 0;
		m_pBitStreamX->ReadCompressedU16X(uACContextNewSpecularColorsCount,uNumNewSpecularColors);
		avuReconstructedVertexUpdate.NumNewSpecularColors = uNumNewSpecularColors;

		// New Specular Colors
		if(uNumNewSpecularColors > 0) {
			// Calculate predicted Specular vertex color
			IFXVector4 v4PredictedVertColor;
			{
				IFXVector4 v4Acc, v4VertColor;
				v4Acc.Set(0.0f,0.0f,0.0f,0.0f);
				// Get the set of specular colors used at the split position
				IFXDECLARELOCAL(IFXSetX,pSplitSpecularColorSet);
				m_pSetAdjacencyX->GetSpecularColorSetX(uSplitPositionIndex,pSplitSpecularColorSet);
				U32 uSize = 0;
				pSplitSpecularColorSet->GetSizeX(uSize);

				U32 j;
				for(j=0; j < uSize; j++) {
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
				for(j=0;j<uNumNewSpecularColors;j++) {
					U32 uVertColorIndex = pcReconstructedMeshDescription->NumSpecularColors + j;

					U8 u8Signs = 0;
					U32 udR = 0;
					U32 udG = 0;
					U32 udB = 0;
					U32 udA = 0;

					m_pBitStreamX->ReadCompressedU8X(uACContextSpecularColorSign,u8Signs);
					m_pBitStreamX->ReadCompressedU32X(uACContextVertexColorDiffMagR,udR);
					m_pBitStreamX->ReadCompressedU32X(uACContextVertexColorDiffMagG,udG);
					m_pBitStreamX->ReadCompressedU32X(uACContextVertexColorDiffMagB,udB);
					m_pBitStreamX->ReadCompressedU32X(uACContextVertexColorDiffMagA,udA);

					//  Calculate reconstructed color vector and store
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
		U16 uNumNewTexCoords = 0;
		m_pBitStreamX->ReadCompressedU16X(uACContextNewTexCoordsCount,uNumNewTexCoords);
		avuReconstructedVertexUpdate.NumNewTexCoords = uNumNewTexCoords;

		// New Texture Coordinates
		if(uNumNewTexCoords > 0) {
			// Calculate predicted texture coordinate
			IFXVector4 v4PredictedTexCoord;
			{
				IFXVector4 v4Acc, v4TexCoord;
				v4Acc.Set(0.0f,0.0f,0.0f,0.0f);
				// Get the set of texture coords used at the split position
				IFXDECLARELOCAL(IFXSetX,pSplitTexCoordSet);
				m_pSetAdjacencyX->GetTexCoordSetX(0,uSplitPositionIndex,pSplitTexCoordSet);

				U32 uSize = 0;
				pSplitTexCoordSet->GetSizeX(uSize);

				U32 j;
				for(j=0; j < uSize; j++) {
					U32 uIndex = 0;
					pSplitTexCoordSet->GetMemberX(j,uIndex);
					m_pReconstructedAuthorCLODMesh->GetTexCoord(uIndex,&v4TexCoord);
					v4Acc.Add(v4TexCoord);
				}

				if(uSize > 0) {
					v4PredictedTexCoord.X() = v4Acc.X()/(uSize);
					v4PredictedTexCoord.Y() = v4Acc.Y()/(uSize);
					v4PredictedTexCoord.Z() = v4Acc.Z()/(uSize);
					v4PredictedTexCoord.H() = v4Acc.H()/(uSize);
				} else {
					v4PredictedTexCoord.Set(0.0f,0.0f,0.0f,0.0f);
				}
			}

			// Read quantized prediction difference and calculate reconstructed tex coord
			{
				U32 j=0;
				for(j=0;j<uNumNewTexCoords;j++) {
					U32 uTexCoordIndex = pcReconstructedMeshDescription->NumTexCoords + j;

					U8 u8Signs = 0;
					U32 udX = 0;
					U32 udY = 0;
					U32 udZ = 0;
					U32 udH = 0;

					m_pBitStreamX->ReadCompressedU8X(uACContextTexCoordSign,u8Signs);
					m_pBitStreamX->ReadCompressedU32X(uACContextTexCoordDiffMagU,udX);
					m_pBitStreamX->ReadCompressedU32X(uACContextTexCoordDiffMagV,udY);
					m_pBitStreamX->ReadCompressedU32X(uACContextTexCoordDiffMagS,udZ);
					m_pBitStreamX->ReadCompressedU32X(uACContextTexCoordDiffMagT,udH);

					//  Calculate reconstructed tex coord vector and store
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
		U32 uNumNewFaces   = 0;
		//  4.2 number of new faces
		m_pBitStreamX->ReadCompressedU32X(uACContextNumNewFaces,uNumNewFaces);
		avuReconstructedVertexUpdate.NumNewFaces = uNumNewFaces;

		IFXDECLARELOCAL(IFXSetX,pLeftThirdIndexSetX);
		IFXCHECKX(IFXCreateComponent(CID_IFXSetX,IID_IFXSetX,(void**)&pLeftThirdIndexSetX));
		IFXDECLARELOCAL(IFXSetX,pRightThirdIndexSetX);
		IFXCHECKX(IFXCreateComponent(CID_IFXSetX,IID_IFXSetX,(void**)&pRightThirdIndexSetX));

		//  E. For each new face ...
		//  4.4 for each new face
		{
			U8 u8OrientationType = 0; // 1 == left; 2 == right
			U8 u8ThirdIndexType  = 0; // 1 == local; 2 == global
			U32 uThirdPositionIndex = 0;

			U32 uNumFaces = pcReconstructedMeshDescription->NumFaces;
			U32 j;
			for(j=0; j < uNumNewFaces; j++) {

				// 4.4.1 Get the Shading id for the face
				U32 uShadingID = 0;
				m_pBitStreamX->ReadCompressedU32X(uACContextShadingID,uShadingID);
				m_pReconstructedAuthorCLODMesh->SetFaceMaterial(uNumFaces+j,uShadingID);

				//    4.4.2 face orientation: left or right
				m_pBitStreamX->ReadCompressedU8X(uACContextOrientationType,u8OrientationType);
				//    4.4.3 3rd position type: local or global
				m_pBitStreamX->ReadCompressedU8X(uACContextThirdIndexType,u8ThirdIndexType);
				//    3rd position index
				if(u8AMPThirdIndexLocal == u8ThirdIndexType) {
					//  4.4.4 Local 3rd position index
					U32 uLocalIndex = 0;
					U32 uLocalSetSize = 0;
					pPositionSetX->GetSizeX(uLocalSetSize);
					IFXASSERT(uLocalSetSize < 0x0000FFFF);
					m_pBitStreamX->ReadCompressedU32X(uACContextLocal3rdPosition,uLocalIndex);
					IFXASSERT(uLocalIndex < uLocalSetSize);
					// Convert the local index into a 3rd position index
					pPositionSetX->GetMemberX(uLocalIndex,uThirdPositionIndex);
				} else {
					//  4.4.5 Global 3rd position index
					IFXASSERT(u8AMPThirdIndexGlobal == u8ThirdIndexType);
					U32 uGlobalSetSize = m_uCurrentResolution;
					//if(uGlobalSetSize < 2) uGlobalSetSize = 2;
					m_pBitStreamX->ReadCompressedU32X(uACStaticFull+uGlobalSetSize,uThirdPositionIndex);
					IFXASSERT(uThirdPositionIndex < uGlobalSetSize);
					// Add to the position set for possible use by the next face
					pPositionSetX->AddX(uThirdPositionIndex);
				}

				// Add 3rd position index to either left or right 3rd index set
				if(u8AMPOrientationLeft == u8OrientationType) {
					pLeftThirdIndexSetX->AddX(uThirdPositionIndex);
				} else if(u8AMPOrientationRight == u8OrientationType) {
					pRightThirdIndexSetX->AddX(uThirdPositionIndex);
				} else {
					IFXASSERT(0);
				}

				// Put the face into the mesh
				//Set new position face for Reconstructed Mesh.
				{
					IFXAuthorFace faceReconstructedPositionFace;
					if(u8AMPOrientationLeft == u8OrientationType) {
						faceReconstructedPositionFace.VertexA() = uSplitPositionIndex;
						faceReconstructedPositionFace.VertexB() = m_uCurrentResolution;
						faceReconstructedPositionFace.VertexC() = uThirdPositionIndex;
					} else if(u8AMPOrientationRight == u8OrientationType) {
						faceReconstructedPositionFace.VertexB() = uSplitPositionIndex;
						faceReconstructedPositionFace.VertexA() = m_uCurrentResolution;
						faceReconstructedPositionFace.VertexC() = uThirdPositionIndex;
					} else {
						IFXASSERT(0);
					}
					m_pReconstructedAuthorCLODMesh->SetPositionFace(uNumFaces+j, &faceReconstructedPositionFace);
				}
			}
		}

		//    3.4 for each face using the split position
		//      3.4.1 stay or move
		IFXDECLARELOCAL(IFXSetX,pMoveFaceSetX);
		IFXCHECKX(IFXCreateComponent(CID_IFXSetX,IID_IFXSetX,(void**)&pMoveFaceSetX));

		// For each face in split position face set,
		// predict stay or move then read stay or move given that prediction
		// If move, add face to the move set.
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
			for(j = 0; j < uSplitFaceSetSize; j++) {
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

				// Read the Stay/Move value
				U8 u8StayMove = u8AMPStay;
				m_pBitStreamX->ReadCompressedU8X(uACContextStayMove+uPredictionStayMove,u8StayMove);

				// Update Move Face set, Move Position set, and Stay Position set
				if(u8AMPMove == u8StayMove ) {
					pMoveFaceSetX->AddX(uFaceIndex);
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

			}
		}

		// For each face in the move set, create a face update and add to vertex update
		{
			U32 uMoveSize = 0;
			pMoveFaceSetX->GetSizeX(uMoveSize);

			IFXAuthorFaceUpdate* pFaceUpdates = NULL;

			U32 uFaceUpdateArraySize = uMoveSize;
			if (uFaceUpdateArraySize>0)
			{
				pFaceUpdates = new IFXAuthorFaceUpdate[uFaceUpdateArraySize];
				if(NULL == pFaceUpdates) {
					IFXCHECKX(IFX_E_OUT_OF_MEMORY);
				}
			}
			U32 j = 0;
			U32 uFaceUpdatesIndex = 0;
			for(j=0;j<uMoveSize;j++) {
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
						if(NULL == pFaceUpdates) {
							IFXCHECKX(IFX_E_OUT_OF_MEMORY);
						}
						memset(pFaceUpdates,0,uFaceUpdateArraySize*sizeof(IFXAuthorFaceUpdate));
						if(NULL != pOldFaceUpdates) {
							memcpy(pFaceUpdates,pOldFaceUpdates,uOldFaceUpdateArraySize*sizeof(IFXAuthorFaceUpdate));
						}
						IFXDELETE_ARRAY(pOldFaceUpdates);
					}

					pFaceUpdates[uFaceUpdatesIndex].Attribute = IFXAuthorFaceUpdate::Position;
					pFaceUpdates[uFaceUpdatesIndex].FaceIndex = uMoveFaceIndex;
					pFaceUpdates[uFaceUpdatesIndex].Corner = uUpdateCorner;
					pFaceUpdates[uFaceUpdatesIndex].IncrValue = m_uCurrentResolution;
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

				if(bDiffuseColors) {
					// Determine if this face diffuse color is a Keep or Change

					U8 uChangeKeep = 0;
					U8 uChangeIndexType = 0;
					U32 uChangeIndex = 0;

					// Read change/keep and change index
					m_pBitStreamX->ReadCompressedU8X(uACContextDiffuseKeepChange,uChangeKeep);
					if(uAMPUpdateChange == uChangeKeep) {
						m_pBitStreamX->ReadCompressedU8X(uACContextDiffuseChangeType,uChangeIndexType);
						if(uAMPUpdateNew == uChangeIndexType) {
							m_pBitStreamX->ReadCompressedU32X(uACContextDiffuseChangeIndexNew,uChangeIndex);
						} else if (uAMPUpdateLocal == uChangeIndexType) {
							m_pBitStreamX->ReadCompressedU32X(uACContextDiffuseChangeIndexLocal,uChangeIndex);
						} else {
							IFXASSERT(uAMPUpdateGlobal == uChangeIndexType);
							m_pBitStreamX->ReadCompressedU32X(uACContextDiffuseChangeIndexGlobal,uChangeIndex);
						}
					}

					if(uAMPUpdateChange == uChangeKeep) {

						U32 uReconstructedIncrValue = 0;
						U32 uReconstructedDecrValue = 0;

						if(uAMPUpdateNew == uChangeIndexType) {
							uReconstructedIncrValue = uChangeIndex + pcReconstructedMeshDescription->NumDiffuseColors;
						} else if(uAMPUpdateLocal == uChangeIndexType) {
							IFXDECLARELOCAL(IFXSetX,pDiffuseColorSetX);
							m_pSetAdjacencyX->GetDiffuseColorSetX(uSplitPositionIndex,pDiffuseColorSetX);
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

						// Fill in diffuse face update
						if(uFaceUpdatesIndex >= uFaceUpdateArraySize) {
							// Reallocate array if needed
							IFXAuthorFaceUpdate* pOldFaceUpdates = pFaceUpdates;
							U32 uOldFaceUpdateArraySize = uFaceUpdateArraySize;
							uFaceUpdateArraySize = uFaceUpdatesIndex + 10; /// @todo: Better memory allocation
							pFaceUpdates = new IFXAuthorFaceUpdate[uFaceUpdateArraySize];
							if(NULL == pFaceUpdates) {
								IFXCHECKX(IFX_E_OUT_OF_MEMORY);
							}
							memset(pFaceUpdates,0,uFaceUpdateArraySize*sizeof(IFXAuthorFaceUpdate));
							if(NULL != pOldFaceUpdates) {
								memcpy(pFaceUpdates,pOldFaceUpdates,uOldFaceUpdateArraySize*sizeof(IFXAuthorFaceUpdate));
							}
							IFXDELETE_ARRAY(pOldFaceUpdates);
						}

						pFaceUpdates[uFaceUpdatesIndex].Attribute = IFXAuthorFaceUpdate::Diffuse;
						pFaceUpdates[uFaceUpdatesIndex].FaceIndex = uMoveFaceIndex;
						pFaceUpdates[uFaceUpdatesIndex].Corner = uUpdateCorner;
						pFaceUpdates[uFaceUpdatesIndex].IncrValue = uReconstructedIncrValue;
						pFaceUpdates[uFaceUpdatesIndex].DecrValue = uReconstructedDecrValue;
						uFaceUpdatesIndex++;
					}
				}

				if(bSpecularColors) {
					// Determine if this face specular color is a Keep or Change

					U8 uChangeKeep = 0;
					U8 uChangeIndexType = 0;
					U32 uChangeIndex = 0;

					// Read change/keep and change index
					m_pBitStreamX->ReadCompressedU8X(uACContextSpecularKeepChange,uChangeKeep);
					if(uAMPUpdateChange == uChangeKeep) {
						m_pBitStreamX->ReadCompressedU8X(uACContextSpecularChangeType,uChangeIndexType);
						if(uAMPUpdateNew == uChangeIndexType) {
							m_pBitStreamX->ReadCompressedU32X(uACContextSpecularChangeIndexNew,uChangeIndex);
						} else if (uAMPUpdateLocal == uChangeIndexType) {
							m_pBitStreamX->ReadCompressedU32X(uACContextSpecularChangeIndexLocal,uChangeIndex);
						} else {
							IFXASSERT(uAMPUpdateGlobal == uChangeIndexType);
							m_pBitStreamX->ReadCompressedU32X(uACContextSpecularChangeIndexGlobal,uChangeIndex);
						}
					}

					if(uAMPUpdateChange == uChangeKeep) {

						U32 uReconstructedIncrValue = 0;
						U32 uReconstructedDecrValue = 0;

						if(uAMPUpdateNew == uChangeIndexType) {
							uReconstructedIncrValue = uChangeIndex + pcReconstructedMeshDescription->NumSpecularColors;
						} else if(uAMPUpdateLocal == uChangeIndexType) {
							IFXDECLARELOCAL(IFXSetX,pSpecularColorSetX);
							m_pSetAdjacencyX->GetSpecularColorSetX(uSplitPositionIndex,pSpecularColorSetX);
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

						// Fill in specular face update
						if(uFaceUpdatesIndex >= uFaceUpdateArraySize) {
							// Reallocate array if needed
							IFXAuthorFaceUpdate* pOldFaceUpdates = pFaceUpdates;
							U32 uOldFaceUpdateArraySize = uFaceUpdateArraySize;
							uFaceUpdateArraySize = uFaceUpdatesIndex + 10; /// @todo: Better memory allocation
							pFaceUpdates = new IFXAuthorFaceUpdate[uFaceUpdateArraySize];
							if(NULL == pFaceUpdates) {
								IFXCHECKX(IFX_E_OUT_OF_MEMORY);
							}
							memset(pFaceUpdates,0,uFaceUpdateArraySize*sizeof(IFXAuthorFaceUpdate));
							if(NULL != pOldFaceUpdates) {
								memcpy(pFaceUpdates,pOldFaceUpdates,uOldFaceUpdateArraySize*sizeof(IFXAuthorFaceUpdate));
							}
							IFXDELETE_ARRAY(pOldFaceUpdates);
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
				for(uLayer = 0; uLayer < u8NumTexLayers; uLayer++) {

					// Determine if this face texture coordinate is a Keep or Change

					U8 uChangeKeep = 0;
					U8 uChangeIndexType = 0;
					U32 uChangeIndex = 0;

					// Read change/keep and change index
					m_pBitStreamX->ReadCompressedU8X(uACContextTexCoordKeepChange,uChangeKeep);
					if(uAMPUpdateChange == uChangeKeep) {
						m_pBitStreamX->ReadCompressedU8X(uACContextTexCoordChangeType,uChangeIndexType);
						if(uAMPUpdateNew == uChangeIndexType) {
							m_pBitStreamX->ReadCompressedU32X(uACContextTexCoordChangeIndexNew,uChangeIndex);
						} else if (uAMPUpdateLocal == uChangeIndexType) {
							m_pBitStreamX->ReadCompressedU32X(uACContextTexCoordChangeIndexLocal,uChangeIndex);
						} else {
							IFXASSERT(uAMPUpdateGlobal == uChangeIndexType);
							m_pBitStreamX->ReadCompressedU32X(uACContextTexCoordChangeIndexGlobal,uChangeIndex);
						}
					}

					if(uAMPUpdateChange == uChangeKeep) {

						U32 uReconstructedIncrValue = 0;
						U32 uReconstructedDecrValue = 0;

						if(uAMPUpdateNew == uChangeIndexType) {
							uReconstructedIncrValue = uChangeIndex + pcReconstructedMeshDescription->NumTexCoords;
						} else if(uAMPUpdateLocal == uChangeIndexType) {
							IFXDECLARELOCAL(IFXSetX,pTexCoordSetX);
							m_pSetAdjacencyX->GetTexCoordSetX(uLayer,uSplitPositionIndex,pTexCoordSetX);
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

						// Fill in texture face update
						if(uFaceUpdatesIndex >= uFaceUpdateArraySize) {
							// Reallocate array if needed
							IFXAuthorFaceUpdate* pOldFaceUpdates = pFaceUpdates;
							U32 uOldFaceUpdateArraySize = uFaceUpdateArraySize;
							uFaceUpdateArraySize = uFaceUpdatesIndex + 10; /// @todo: Better memory allocation
							pFaceUpdates = new IFXAuthorFaceUpdate[uFaceUpdateArraySize];
							if(NULL == pFaceUpdates) {
								IFXCHECKX(IFX_E_OUT_OF_MEMORY);
							}
							memset(pFaceUpdates,0,uFaceUpdateArraySize*sizeof(IFXAuthorFaceUpdate));
							if(NULL != pOldFaceUpdates) {
								memcpy(pFaceUpdates,pOldFaceUpdates,uOldFaceUpdateArraySize*sizeof(IFXAuthorFaceUpdate));
							}
							IFXDELETE_ARRAY(pOldFaceUpdates);
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

			IFXDELETE_ARRAY(avuReconstructedVertexUpdate.pFaceUpdates);
			avuReconstructedVertexUpdate.pFaceUpdates = pFaceUpdates;
			avuReconstructedVertexUpdate.NumFaceUpdates = uFaceUpdatesIndex;
			IFXASSERT(i == m_uCurrentResolution);
			IFXCHECKX(m_pReconstructedAuthorCLODMesh->SetVertexUpdate(i,&avuReconstructedVertexUpdate));
		}

		// For each new face
		{
			U32 uNumFaces = pcReconstructedMeshDescription->NumFaces;
			U32 j;
			for(j=0; j < uNumNewFaces; j++) {

				// Identify corners
				U8 uReconSplitCorner = 0;
				U8 uReconNewCorner = 0;
				U8 uReconThirdCorner = 0;
				U32 uThirdPositionIndex = 0;
				{
					IFXAuthorFace faceReconPositionFace;
					m_pReconstructedAuthorCLODMesh->GetPositionFace(uNumFaces+j, &faceReconPositionFace);

					if(m_uCurrentResolution == faceReconPositionFace.VertexA()) {
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
					} else if (m_uCurrentResolution == faceReconPositionFace.VertexB()) {
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
					} else if (m_uCurrentResolution == faceReconPositionFace.VertexC()) {
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

				if(bDiffuseColors) {

					IFXAuthorFace faceReconstructedColor;
					U32 uSplitColor = 0;
					U32 uUpdatedColor = 0;
					U32 uThirdColor = 0;

					// Diffuse Duplicate Flag
					U8 u8DupFaceIndexFlags = 0;
					m_pBitStreamX->ReadCompressedU8X(uACContextVertColorDupType,u8DupFaceIndexFlags);

					IFXDECLARELOCAL(IFXSetX,pDiffuseSplitSetX);
					m_pSetAdjacencyX->GetDiffuseColorSetX(uSplitPositionIndex,pDiffuseSplitSetX);

					if (!(u8DupFaceIndexFlags & u8AMPSplitColorDup)) {
						U8 uType = 0;
						m_pBitStreamX->ReadCompressedU8X(uACContextVertColorSplitType,uType);
						if(uAMPUpdateLocal == uType) {
							U32 uLocalIndex = 0;
							m_pBitStreamX->ReadCompressedU32X(uACContextVertColorSplitIndexLocal,uLocalIndex);
							pDiffuseSplitSetX->GetMemberX(uLocalIndex,uSplitColor);
						} else {
							IFXASSERT(uAMPUpdateGlobal == uType);
							m_pBitStreamX->ReadCompressedU32X(uACContextVertColorSplitIndexGlobal,uSplitColor);
						}
					} else {
						uSplitColor = m_uPrevSplitDiffColor;
					}

					if (!(u8DupFaceIndexFlags & u8AMPUpdateColorDup)) {
						U8 uType = 0;
						m_pBitStreamX->ReadCompressedU8X(uACContextVertColorSplitType,uType);
						if(uAMPUpdateLocal == uType) {
							U32 uLocalIndex = 0;
							m_pBitStreamX->ReadCompressedU32X(uACContextVertColorSplitIndexLocal,uLocalIndex);
							pDiffuseSplitSetX->GetMemberX(uLocalIndex,uUpdatedColor);
						} else {
							IFXASSERT(uAMPUpdateGlobal == uType);
							m_pBitStreamX->ReadCompressedU32X(uACContextVertColorSplitIndexGlobal,uUpdatedColor);
						}
					} else {
						uUpdatedColor = m_uPrevUpdateDiffColor;
					}

					IFXDECLARELOCAL(IFXSetX,pDiffuseThirdSetX);
					m_pSetAdjacencyX->GetDiffuseColorSetX(uThirdPositionIndex,pDiffuseThirdSetX);

					if (!(u8DupFaceIndexFlags & u8AMPThirdColorDup)) {
						U8 uType = 0;
						m_pBitStreamX->ReadCompressedU8X(uACContextVertColorSplitType,uType);
						if(uAMPUpdateLocal == uType) {
							U32 uLocalIndex = 0;
							m_pBitStreamX->ReadCompressedU32X(uACContextVertColorSplitIndexLocal,uLocalIndex);
							pDiffuseThirdSetX->GetMemberX(uLocalIndex,uThirdColor);
						} else {
							IFXASSERT(uAMPUpdateGlobal == uType);
							m_pBitStreamX->ReadCompressedU32X(uACContextVertColorSplitIndexGlobal,uThirdColor);
						}
					} else {
						uThirdColor = m_uPrevThirdDiffColor;
					}

					faceReconstructedColor.corner[uReconSplitCorner] = uSplitColor;
					faceReconstructedColor.corner[uReconNewCorner] = uUpdatedColor;
					faceReconstructedColor.corner[uReconThirdCorner] = uThirdColor;

					IFXCHECKX(m_pReconstructedAuthorCLODMesh->SetDiffuseFace(uNumFaces+j, &faceReconstructedColor));

					m_uPrevSplitDiffColor = uSplitColor;
					m_uPrevUpdateDiffColor = uUpdatedColor;
					m_uPrevThirdDiffColor = uThirdColor;

				}

				if(bSpecularColors) {

					IFXAuthorFace faceReconstructedColor;
					U32 uSplitColor = 0;
					U32 uUpdatedColor = 0;
					U32 uThirdColor = 0;

					U8 u8DupFaceIndexFlags = 0;
					// Specular Duplicate Flag
					m_pBitStreamX->ReadCompressedU8X(uACContextVertColorDupType,u8DupFaceIndexFlags);

					IFXDECLARELOCAL(IFXSetX,pSpecularSplitSetX);
					m_pSetAdjacencyX->GetSpecularColorSetX(uSplitPositionIndex,pSpecularSplitSetX);

					if (!(u8DupFaceIndexFlags & u8AMPSplitColorDup)) {
						U8 uType = 0;
						m_pBitStreamX->ReadCompressedU8X(uACContextVertColorSplitType,uType);
						if(uAMPUpdateLocal == uType) {
							U32 uLocalIndex = 0;
							m_pBitStreamX->ReadCompressedU32X(uACContextVertColorSplitIndexLocal,uLocalIndex);
							pSpecularSplitSetX->GetMemberX(uLocalIndex,uSplitColor);
						} else {
							IFXASSERT(uAMPUpdateGlobal == uType);
							m_pBitStreamX->ReadCompressedU32X(uACContextVertColorSplitIndexGlobal,uSplitColor);
						}
					} else {
						uSplitColor = m_uPrevSplitSpecColor;
					}

					if (!(u8DupFaceIndexFlags & u8AMPUpdateColorDup)) {
						U8 uType = 0;
						m_pBitStreamX->ReadCompressedU8X(uACContextVertColorSplitType,uType);
						if(uAMPUpdateLocal == uType) {
							U32 uLocalIndex = 0;
							pSpecularSplitSetX->GetMemberX(uLocalIndex,uUpdatedColor);
							m_pBitStreamX->ReadCompressedU32X(uACContextVertColorSplitIndexLocal,uLocalIndex);
						} else {
							IFXASSERT(uAMPUpdateGlobal == uType);
							m_pBitStreamX->ReadCompressedU32X(uACContextVertColorSplitIndexGlobal,uUpdatedColor);
						}
					} else {
						uUpdatedColor = m_uPrevUpdateSpecColor;
					}

					IFXDECLARELOCAL(IFXSetX,pSpecularThirdSetX);
					m_pSetAdjacencyX->GetSpecularColorSetX(uThirdPositionIndex,pSpecularThirdSetX);

					if (!(u8DupFaceIndexFlags & u8AMPThirdColorDup)) {
						U8 uType = 0;
						m_pBitStreamX->ReadCompressedU8X(uACContextVertColorSplitType,uType);
						if(uAMPUpdateLocal == uType) {
							U32 uLocalIndex = 0;
							m_pBitStreamX->ReadCompressedU32X(uACContextVertColorSplitIndexLocal,uLocalIndex);
							pSpecularThirdSetX->GetMemberX(uLocalIndex,uThirdColor);
						} else {
							IFXASSERT(uAMPUpdateGlobal == uType);
							m_pBitStreamX->ReadCompressedU32X(uACContextVertColorSplitIndexGlobal,uThirdColor);
						}
					} else {
						uThirdColor = m_uPrevThirdSpecColor;
					}

					faceReconstructedColor.corner[uReconSplitCorner] = uSplitColor;
					faceReconstructedColor.corner[uReconNewCorner] = uUpdatedColor;
					faceReconstructedColor.corner[uReconThirdCorner] = uThirdColor;

					IFXCHECKX(m_pReconstructedAuthorCLODMesh->SetSpecularFace(uNumFaces+j, &faceReconstructedColor));

					m_uPrevSplitSpecColor = uSplitColor;
					m_uPrevUpdateSpecColor = uUpdatedColor;
					m_uPrevThirdSpecColor = uThirdColor;
				}

				U32 uLayer = 0;
				for(uLayer = 0; uLayer < u8NumTexLayers; uLayer++) {

					IFXAuthorFace faceReconstructedTexCoord;
					U32 uSplitTexCoord = 0;
					U32 uUpdatedTexCoord = 0;
					U32 uThirdTexCoord = 0;

					U8 u8DupFaceIndexFlags = 0;
					// TexCoord Duplicate Flag
					m_pBitStreamX->ReadCompressedU8X(uACContextTexCoordDupType,u8DupFaceIndexFlags);

					IFXDECLARELOCAL(IFXSetX,pTexCoordSplitSetX);
					m_pSetAdjacencyX->GetTexCoordSetX(uLayer,uSplitPositionIndex,pTexCoordSplitSetX);

					if (!(u8DupFaceIndexFlags & u8AMPSplitTexCoordDup)) {
						U8 uType = 0;
						m_pBitStreamX->ReadCompressedU8X(uACContextTexCoordSplitType,uType);
						if(uAMPUpdateLocal == uType) {
							U32 uLocalIndex = 0;
							m_pBitStreamX->ReadCompressedU32X(uACContextTexCoordSplitIndexLocal,uLocalIndex);
							pTexCoordSplitSetX->GetMemberX(uLocalIndex,uSplitTexCoord);
						} else {
							IFXASSERT(uAMPUpdateGlobal == uType);
							m_pBitStreamX->ReadCompressedU32X(uACContextTexCoordSplitIndexGlobal,uSplitTexCoord);
						}
					} else {
						uSplitTexCoord = m_uPrevSplitTexCoord;
					}

					if (!(u8DupFaceIndexFlags & u8AMPUpdateTexCoordDup)) {
						U8 uType = 0;
						m_pBitStreamX->ReadCompressedU8X(uACContextTexCoordSplitType,uType);
						if(uAMPUpdateLocal == uType) {
							U32 uLocalIndex = 0;
							m_pBitStreamX->ReadCompressedU32X(uACContextTexCoordSplitIndexLocal,uLocalIndex);
							pTexCoordSplitSetX->GetMemberX(uLocalIndex,uUpdatedTexCoord);
						} else {
							IFXASSERT(uAMPUpdateGlobal == uType);
							m_pBitStreamX->ReadCompressedU32X(uACContextTexCoordSplitIndexGlobal,uUpdatedTexCoord);
						}
					} else {
						uUpdatedTexCoord = m_uPrevUpdateTexCoord;
					}

					IFXDECLARELOCAL(IFXSetX,pTexCoordThirdSetX);
					m_pSetAdjacencyX->GetTexCoordSetX(uLayer,uThirdPositionIndex,pTexCoordThirdSetX);

					if (!(u8DupFaceIndexFlags & u8AMPThirdTexCoordDup)) {
						U8 uType = 0;
						m_pBitStreamX->ReadCompressedU8X(uACContextTexCoordSplitType,uType);
						if(uAMPUpdateLocal == uType) {
							U32 uLocalIndex = 0;
							m_pBitStreamX->ReadCompressedU32X(uACContextTexCoordSplitIndexLocal,uLocalIndex);
							pTexCoordThirdSetX->GetMemberX(uLocalIndex,uThirdTexCoord);
						} else {
							IFXASSERT(uAMPUpdateGlobal == uType);
							m_pBitStreamX->ReadCompressedU32X(uACContextTexCoordSplitIndexGlobal,uThirdTexCoord);
						}
					} else {
						uThirdTexCoord = m_uPrevThirdTexCoord;
					}

					faceReconstructedTexCoord.corner[uReconSplitCorner] = uSplitTexCoord;
					faceReconstructedTexCoord.corner[uReconNewCorner] = uUpdatedTexCoord;
					faceReconstructedTexCoord.corner[uReconThirdCorner] = uThirdTexCoord;

					IFXCHECKX(m_pReconstructedAuthorCLODMesh->SetTexFace(uLayer,uNumFaces+j, &faceReconstructedTexCoord));

					m_uPrevSplitTexCoord = uSplitTexCoord;
					m_uPrevUpdateTexCoord = uUpdatedTexCoord;
					m_uPrevThirdTexCoord = uThirdTexCoord;
				}
			}
		}

		//  G. Update set adjacency for move faces
		{
			U32 j = 0;
			U32 uMoveSize = 0;
			pMoveFaceSetX->GetSizeX(uMoveSize);
			for(j=0;j<uMoveSize;j++) {
				U32 uMoveFaceIndex = 0;
				pMoveFaceSetX->GetMemberX(j,uMoveFaceIndex);
				m_pSetAdjacencyX->RemoveX(uSplitPositionIndex,uMoveFaceIndex);
				m_pSetAdjacencyX->AddX(m_uCurrentResolution,uMoveFaceIndex);
			}
		}

		//  H. Update set adjacency for new faces
		{
			U32 uNumFaces = pcReconstructedMeshDescription->NumFaces;
			U32 j;
			for(j=0; j < uNumNewFaces; j++) {
				IFXAuthorFace face;
				// Get the face
				m_pReconstructedAuthorCLODMesh->GetPositionFace(uNumFaces+j, &face);
				// Add the face to the set adjacency object
				m_pSetAdjacencyX->AddX(face.VertexA(),uNumFaces+j);
				m_pSetAdjacencyX->AddX(face.VertexB(),uNumFaces+j);
				m_pSetAdjacencyX->AddX(face.VertexC(),uNumFaces+j);
			}
		}

		//  I.  Get set of faces using new position (set I)
		IFXDECLARELOCAL(IFXSetX,pNewFaceSetX);
		m_pSetAdjacencyX->GetFaceSetX(m_uCurrentResolution,pNewFaceSetX);

		//  J.  Get set of positions used by set I (set J)
		IFXRELEASE(pPositionSetX);
		IFXDECLARELOCAL(IFXSetX,pNewPositionSetX);
		m_pSetAdjacencyX->GetPositionSetX(*pNewFaceSetX,pNewPositionSetX);

		//  K. Predict position vector from set J.
		IFXVector3 vPredictedPosition;
		if(m_uCurrentResolution > 0) {
			m_pReconstructedAuthorCLODMesh->GetPosition(uSplitPositionIndex , &vPredictedPosition);
		} else {
			vPredictedPosition.Set(0.0,0.0,0.0);
		}

		//  M. Read Quantize position difference
		{
			U8 u8Signs = 0;
			U32 udX = 0;
			U32 udY = 0;
			U32 udZ = 0;
			//    3.5 quantized position difference from prediction
			//      3.5.1 Sign bits
			//      3.5.2 quantized magnitude:|dX|, |dY|, |dZ|
			m_pBitStreamX->ReadCompressedU8X(uACContextPositionDiffSigns,u8Signs);
			m_pBitStreamX->ReadCompressedU32X(uACContextPositionDiffMagX,udX);
			m_pBitStreamX->ReadCompressedU32X(uACContextPositionDiffMagY,udY);
			m_pBitStreamX->ReadCompressedU32X(uACContextPositionDiffMagZ,udZ);

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

			IFXCHECKX(m_pReconstructedAuthorCLODMesh->SetPosition(m_uCurrentResolution,&vReconstructedPosition));
		}

		// Increment the maximum resolution
		{
			// Note: the mesh cannot be locked for SetResolution() to succeed.
#ifdef _DEBUG
			U32 uTempResolution = 
#endif
				m_pReconstructedAuthorCLODMesh->SetMaxResolution(m_uCurrentResolution+1);
			IFXASSERT(uTempResolution == m_uCurrentResolution+1);
		}

		// Calculate Normals and Normal Face Updates for each position in set J
		// Note: Face Normals should be calculated with the positions already updated (resolution incremented)
		// Note: Vertex updates and face updates should only be modified above the current resolution (resolution not incremented)
		if(!m_bExcludeNormals)
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
				++m_uCurrentResolution;
				// Note: the mesh cannot be locked for SetResolution() to succeed.
#ifdef _DEBUG
				U32 uTempResolution = 
#endif
					m_pReconstructedAuthorCLODMesh->SetResolution(m_uCurrentResolution);
				IFXASSERT(uTempResolution == m_uCurrentResolution);
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
			for(j=0; j < uPositionSetSize; j++) {

				// Get the position index and set of faces using that position
				U32 uPositionIndex = 0;
				pNewPositionSetX->GetMemberX(j,uPositionIndex);
				IFXDECLARELOCAL(IFXSetX,pNormalFaceSetX);
				m_pSetAdjacencyX->GetFaceSetX(uPositionIndex,pNormalFaceSetX);

				U32 uNormalFaceSetSize = 0;
				pNormalFaceSetX->GetSizeX(uNormalFaceSetSize);

				// Set size of local normal arrays
				U32 uLocalNormalArraySize = uNormalFaceSetSize;

				// - Count number of unique normals at the position in the original mesh (use the crease parameter)

				// - Write the number of new normals at this position (count of unique normals from above)
				U32 uNumLocalNormals = 0;
				m_pBitStreamX->ReadCompressedU32X(uACContextNumLocalNormals,uNumLocalNormals);

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
					for(k=0; k < uNormalFaceSetSize; k++) {
						IFXVector3 vFaceNormal;
						U32 uNormalFaceIndex = 0;
						pNormalFaceSetX->GetMemberX(k,uNormalFaceIndex);

						CalculateFaceNormalX(uNormalFaceIndex,vFaceNormal);

						if(vFaceNormal.X() > -2.0f) { // Check for valid normal
							IFXASSERT(uNumFaceLocalNormals < uLocalNormalArraySize);
							pvFaceLocalNormal[uNumFaceLocalNormals] = vFaceNormal;
							uNumFaceLocalNormals++;
						}
					}

					// Put the first face normal into the local normal array
					if(uNumFaceLocalNormals > 0) {
						pvPredictedLocalNormal[uNumPredictedLocalNormals] = pvFaceLocalNormal[0];
						uNumPredictedLocalNormals++;
					}

					// While number of local normals is less than the number of original local normals
					while(uNumPredictedLocalNormals < uNumLocalNormals) {
						// Find the face local normal which is farthest from any normal in the local normal array
						// "Distance" is measured by dot product.  1.0 is closest, -1.0 is farthest.

						F32 fFarthestDistance = 1.0f;
						U32 uIndexFarthestNormal = 0;

						for(k=0; k < uNumFaceLocalNormals; k++) {
							// calculate minimum distance to an element in the local normal array
							F32 fMinimumDistance = -2.0f;
							IFXVector3 vTemp = pvFaceLocalNormal[k];
							U32 l = 0;
							for(l=0; l < uNumPredictedLocalNormals; l++) {
								F32 fDistance = vTemp.DotProduct(pvPredictedLocalNormal[l]);
								if(fDistance > fMinimumDistance) {
									fMinimumDistance = fDistance;
								}
							}

							// update farthest distance if needed
							if(fMinimumDistance < fFarthestDistance) {
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
					if(NULL == puPredictedContributionCount) {
						IFXCHECKX(IFX_E_OUT_OF_MEMORY);
					}
					memset(puPredictedContributionCount,0,sizeof(U32)*uLocalNormalArraySize);

					// For each face normal in local face normal array
					for(k=0; k < uNumFaceLocalNormals; k++) {
						// Find closest predicted normal to the face normal
						U32 uIndexClosestPrediction = 0;
						IFXVector3 vFaceNormal = pvFaceLocalNormal[k];
						{
							F32 fMinimumDistance = -2.0f;
							U32 l = 0;
							for(l=0; l < uNumPredictedLocalNormals; l++) {
								F32 fDistance = vFaceNormal.DotProduct(pvPredictedLocalNormal[l]);
								if(fDistance > fMinimumDistance) {
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

				IFXVector3* pvLocalNormal = new IFXVector3[uLocalNormalArraySize];
				if(NULL == pvLocalNormal) {
					IFXCHECKX(IFX_E_OUT_OF_MEMORY);
				}
				memset(pvLocalNormal,0,sizeof(IFXVector3)*uLocalNormalArraySize);

				// - For each new normal
				{
					U32 k = 0;
					// For each face
					for(k=0; k < uNumLocalNormals; k++) {
						IFXQuaternion qPredictedNormal(pvPredictedLocalNormal[k]);
						//IFXQuaternion qPredictedNormalInverted(pvPredictedLocalNormal[k]);
						//qPredictedNormalInverted.Invert();
						// - - Read the quantized prediction difference
						U8 u8Signs = 0;
						U32 udX = 0;
						U32 udY = 0;
						U32 udZ = 0;
						m_pBitStreamX->ReadCompressedU8X(uACContextNormalDiffSigns,u8Signs);
						m_pBitStreamX->ReadCompressedU32X(uACContextNormalDiffMagX,udX);
						m_pBitStreamX->ReadCompressedU32X(uACContextNormalDiffMagY,udY);
						m_pBitStreamX->ReadCompressedU32X(uACContextNormalDiffMagZ,udZ);

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
						IFXQuaternion qReconstructedNormal;
						qReconstructedNormal.Multiply(qPredictedNormal,qReconstructedDifference);

						pvLocalNormal[k].Set(qReconstructedNormal[1],qReconstructedNormal[2],qReconstructedNormal[3]);

					}
				}

				// - For each face using the position
				{
					U32 k = 0;
					for(k=0; k < uNormalFaceSetSize; k++) {
						U32 uNormalFaceIndex = 0;
						pNormalFaceSetX->GetMemberX(k,uNormalFaceIndex);

						// - - Read local index into local new normal array for normal used by this face
						U32 uBestLocalIndex = 0;
						m_pBitStreamX->ReadCompressedU32X(uACContextNormalLocalIndex,uBestLocalIndex);
						// - - If the normal face is a new normal face
						if(uNormalFaceIndex >= uNumValidNormalFaces ) {
							// - - - Update reconstructed normal face
							IFXAuthorFace faceReconstructedPosition, faceReconstructedNormal;
							IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetPositionFace(uNormalFaceIndex,&faceReconstructedPosition));
							IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetNormalFace(uNormalFaceIndex,&faceReconstructedNormal));
							U32 uReconstructedNormalIndex = uNumOldNormals + uNumNewNormals + uBestLocalIndex;
							if(uPositionIndex == faceReconstructedPosition.VertexA()) {
								faceReconstructedNormal.VertexA() = uReconstructedNormalIndex;
							} else if(uPositionIndex == faceReconstructedPosition.VertexB()) {
								faceReconstructedNormal.VertexB() = uReconstructedNormalIndex;
							} else if(uPositionIndex == faceReconstructedPosition.VertexC()) {
								faceReconstructedNormal.VertexC() = uReconstructedNormalIndex;
							} else {
								IFXASSERT(0);
							}
							IFXCHECKX(m_pReconstructedAuthorCLODMesh->SetNormalFace(uNormalFaceIndex,&faceReconstructedNormal));
						} else { // - - Else If the normal face is an old normal face
							// - - - Create a normal face update
							IFXAuthorFace faceReconstructedPosition, faceReconstructedNormal;
							IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetPositionFace(uNormalFaceIndex,&faceReconstructedPosition));
							IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetNormalFace(uNormalFaceIndex,&faceReconstructedNormal));
							U32 uReconstructedNormalIndex = uNumOldNormals + uNumNewNormals + uBestLocalIndex;
							IFXAuthorFaceUpdate afuNewUpdate;

							afuNewUpdate.FaceIndex = uNormalFaceIndex;
							afuNewUpdate.Attribute = IFXAuthorFaceUpdate::Normal;
							afuNewUpdate.IncrValue = uReconstructedNormalIndex;

							if(uPositionIndex == faceReconstructedPosition.VertexA()) {
								afuNewUpdate.Corner = 0;
								afuNewUpdate.DecrValue = faceReconstructedNormal.VertexA();
							} else if(uPositionIndex == faceReconstructedPosition.VertexB()) {
								afuNewUpdate.Corner = 1;
								afuNewUpdate.DecrValue = faceReconstructedNormal.VertexB();
							} else if(uPositionIndex == faceReconstructedPosition.VertexC()) {
								afuNewUpdate.Corner = 2;
								afuNewUpdate.DecrValue = faceReconstructedNormal.VertexC();
							} else {
								IFXASSERT(0);
							}

							// - - - Add the normal face update to the master face update array
							{
								// Re-allocate if needed
								if(uNumFaceUpdates == uFaceUpdateArraySize) {
									IFXAuthorFaceUpdate* pafuTemp = pafuNormalFaceUpdates;
									pafuNormalFaceUpdates = new IFXAuthorFaceUpdate[uFaceUpdateArraySize+uFaceUpdateArrayGrowthSize];
									if(NULL == pafuNormalFaceUpdates) {
										IFXCHECKX(IFX_E_OUT_OF_MEMORY);
									}
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

					for(k = 0; k < uNumLocalNormals; k++ ) {

						// Re-allocate if needed
						if(uNumNewNormals == uNormalArraySize) {
							IFXVector3* pvTemp = pvNewNormal;
							pvNewNormal = new IFXVector3[uNormalArraySize+uNormalArrayGrowthSize];
							if(NULL == pvNewNormal) {
								IFXCHECKX(IFX_E_OUT_OF_MEMORY);
							}
							memset(pvNewNormal+uNormalArraySize,0,sizeof(IFXVector3)*uNormalArrayGrowthSize);
							memcpy(pvNewNormal,pvTemp,sizeof(IFXVector3)*uNormalArraySize);
							uNormalArraySize += uNormalArrayGrowthSize;

							IFXDELETE_ARRAY(pvTemp);
						}

						// Add the normal to the local array
						pvNewNormal[uNumNewNormals] = pvLocalNormal[k];
						uNumNewNormals++;

					}
				}

				IFXDELETE_ARRAY(pvLocalNormal);
				IFXDELETE_ARRAY(pvPredictedLocalNormal);

			}

			// Modify reconstructed vertex update
			{
				// - Set number of new normals
				avuReconstructedVertexUpdate.NumNewNormals = uNumNewNormals;
				// - Append master face update array to face updates
				U32 uOldNumFaceUpdates = avuReconstructedVertexUpdate.NumFaceUpdates;

				if(uNumFaceUpdates+uOldNumFaceUpdates>0)
				{
					IFXAuthorFaceUpdate* pafuOldFaceUpdates = avuReconstructedVertexUpdate.pFaceUpdates;

					avuReconstructedVertexUpdate.pFaceUpdates = new IFXAuthorFaceUpdate[uNumFaceUpdates+uOldNumFaceUpdates];
					if(NULL == avuReconstructedVertexUpdate.pFaceUpdates) {
						IFXCHECKX(IFX_E_OUT_OF_MEMORY);
					}
					//memset(avuReconstructedVertexUpdate.pFaceUpdates,0,sizeof(IFXAuthorFaceUpdate)*(uNumFaceUpdates+uOldNumFaceUpdates));
					memcpy(avuReconstructedVertexUpdate.pFaceUpdates,pafuOldFaceUpdates,sizeof(IFXAuthorFaceUpdate)*(uOldNumFaceUpdates));
					memcpy(avuReconstructedVertexUpdate.pFaceUpdates+uOldNumFaceUpdates,pafuNormalFaceUpdates,sizeof(IFXAuthorFaceUpdate)*(uNumFaceUpdates));

					avuReconstructedVertexUpdate.NumFaceUpdates += uNumFaceUpdates;

					IFXDELETE_ARRAY(pafuOldFaceUpdates);
				}
			}

			// Set the new normals to the reconstructed mesh
			{
				for(I32 k = uNumNewNormals-1; k >= 0; k--) {
					IFXCHECKX(m_pReconstructedAuthorCLODMesh->SetNormal(uNumOldNormals+k, pvNewNormal +k));
				}
			}

			IFXDELETE_ARRAY(pvNewNormal);
			IFXDELETE_ARRAY(puNewNormalIndex);
			IFXDELETE_ARRAY(pafuNormalFaceUpdates);

			// Decrement the current resolution
			{
				--m_uCurrentResolution;
				// Note: the mesh cannot be locked for SetResolution() to succeed.
#ifdef _DEBUG
				U32 uTempResolution = 
#endif
					m_pReconstructedAuthorCLODMesh->SetResolution(m_uCurrentResolution);
				IFXASSERT(uTempResolution == m_uCurrentResolution);
			}

		}

		IFXCHECKX(m_pReconstructedAuthorCLODMesh->SetVertexUpdate(m_uCurrentResolution,&avuReconstructedVertexUpdate));

		// Increment the current resolution
		{
			++m_uCurrentResolution;
			// Note: the mesh cannot be locked for SetResolution() to succeed.
#ifdef _DEBUG
			U32 uTempResolution = 
#endif
				m_pReconstructedAuthorCLODMesh->SetResolution(m_uCurrentResolution);
			IFXASSERT(uTempResolution == m_uCurrentResolution);
		}
	}

	if (m_uWIPi == m_uEndResolution) {
		IFXRELEASE(m_pBitStreamX);
		m_eWIPType = WIP_NONE;
	}
}
