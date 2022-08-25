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

/**
	@file  CIFXAuthorCLODDecoder.cpp
*/

#include "CIFXAuthorCLODDecoder.h"
#include "IFXCoreCIDs.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"
#include "IFXEnums.h"
#include "IFXModifierChain.h"
#include "IFXModifierDataPacket.h"
#include "IFXExportingCIDs.h"
#include "IFXSkeleton.h"
#include "IFXModel.h"
#include "IFXBonesManager.h"


// IFXUnknown
U32 CIFXAuthorCLODDecoder::AddRef( void )
{
	return ++m_uRefCount;
}

U32 CIFXAuthorCLODDecoder::Release( void )
{
	if ( 1 == m_uRefCount ) 
	{
		delete this;
		return 0;
	}
	return --m_uRefCount;
}

IFXRESULT CIFXAuthorCLODDecoder::QueryInterface( IFXREFIID  interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		if ( interfaceId == IID_IFXDecoderX )
		{
			*ppInterface = ( IFXDecoderX* ) this;
			this->AddRef();
		}
		else if ( interfaceId == IID_IFXUnknown )
		{
			*ppInterface = ( IFXUnknown* ) this;
			this->AddRef();
		}
		else
		{
			*ppInterface = NULL;
			rc = IFX_E_UNSUPPORTED;
		}
	} else {
		rc = IFX_E_INVALID_POINTER;
	}

	return rc;
}

// IFXDecoderX
void CIFXAuthorCLODDecoder::InitializeX( const IFXLoadConfig &lc )
{
	if( lc.m_units > 0.0f )
		m_unitScale = lc.m_units;
	IFXModifierBaseDecoder::InitializeX(lc);
}

void CIFXAuthorCLODDecoder::PutNextBlockX( IFXDataBlockX &rDataBlockX )
{
	if(NULL == m_pDataBlockQueueX) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	m_pDataBlockQueueX->AppendBlockX( rDataBlockX );

	if (NULL == m_pObject)
	{
		U32 uBlockType;
		rDataBlockX.GetBlockTypeX(uBlockType);
		CreateObjectX(rDataBlockX, CID_IFXAuthorCLODResource);

		IFXRELEASE(m_pAuthorCLODResource);
		IFXCHECKX(m_pObject->QueryInterface(IID_IFXAuthorCLODResource,(void**)&m_pAuthorCLODResource));
#ifdef KEEP_DATA_BLOCKS
		IFXCHECKX(m_pAuthorCLODResource->BuildDataBlockQueue());
#endif
	}

	m_bDone = FALSE;

#ifdef KEEP_DATA_BLOCKS
	// Add the data block to the generator. This is used by the generator as a
	// cached copy of itself in encoded form thus saving potential encoding at
	// save or export time.
	IFXDECLARELOCAL(IFXDataBlockQueueX,pGeneratorDataBlockQueueX);
	m_pAuthorCLODResource->GetDataBlockQueueX( pGeneratorDataBlockQueueX );
	if ( pGeneratorDataBlockQueueX )
		pGeneratorDataBlockQueueX->AppendBlockX( rDataBlockX );
#endif
}

void CIFXAuthorCLODDecoder::TransferX(IFXRESULT &rWarningPartialTransfer)
{
	// If necessary and possible,
	// get a new block for processing
	// and set up the bitstream to access that block
	if ((NULL == m_pBitStreamX) && (FALSE == m_bDone) && (m_eWIPType != WIP_PROGRESSIVE_U3D))
	{
		IFXDataBlockX *pDataBlockX = NULL;
		m_pDataBlockQueueX->GetNextBlockX( pDataBlockX, m_bDone );
		IFXASSERT(pDataBlockX);

		IFXCHECKX(IFXCreateComponent(CID_IFXBitStreamX,IID_IFXBitStreamCompressedX,(void**)&m_pBitStreamX));
		IFXDECLARELOCAL(IFXNameMap, pNameMap);
		m_pCoreServices->GetNameMap(IID_IFXNameMap, (void**)&pNameMap);
		U32 uProfile;
		pNameMap->GetProfile(m_uLoadId, uProfile);
		m_pBitStreamX->SetNoCompressionMode((uProfile & IFXPROFILE_NOCOMPRESSION) ? TRUE : FALSE);

		m_pBitStreamX->SetDataBlockX(*pDataBlockX);

		U32 uBlockType = 0;
		pDataBlockX->GetBlockTypeX(uBlockType);

		switch( uBlockType )
		{
		case BlockType_GeneratorCLODMeshU3D :
			{
				// set metadata
				IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
				IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
				pDataBlockX->QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
				m_pAuthorCLODResource->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
				pObjectMD->AppendX(pBlockMD);
			}
			m_eWIPType = WIP_DECLARATION_U3D;
			break;
		case BlockType_GeneratorCLODBaseMeshContinuationU3D :
			m_eWIPType = WIP_STATIC_U3D;
			break;
		case BlockType_GeneratorCLODProgressiveMeshContinuationU3D :
			m_eWIPType = WIP_PROGRESSIVE_U3D;
			m_uWIPi = 0;
			break;
		default :
			m_eWIPType = WIP_NONE;
			IFXRELEASE(m_pBitStreamX);
			break;
		}

		IFXRELEASE( pDataBlockX );
	}

	EIFXWIPType wip = m_eWIPType;

	// Process the block
	switch(m_eWIPType)
	{
	case WIP_DECLARATION_U3D:
		ProcessAuthorCLODDeclarationBlockX( BlockType_GeneratorCLODMeshU3D );
		break;
	case WIP_STATIC_U3D:
		ProcessAuthorCLODStaticGeometryBlockX( BlockType_GeneratorCLODBaseMeshContinuationU3D );
		break;
	case WIP_PROGRESSIVE_U3D:
		ProcessAuthorCLODProgressiveGeometryBlocksX( BlockType_GeneratorCLODProgressiveMeshContinuationU3D );
		break;
	case WIP_NONE:
	default:
		break;
	}

	// Give the reconstructed author mesh to the author CLOD resource
	if (wip != WIP_NONE)
		IFXCHECKX(m_pAuthorCLODResource->SetAuthorMesh(m_pReconstructedAuthorCLODMesh));

	if (wip == WIP_DECLARATION_U3D) {
		IFXDECLARELOCAL(IFXSceneGraph,pSG);
		IFXDECLARELOCAL(IFXPalette,pPalette);
		IFXDECLARELOCAL(IFXPalette,pGP);
		U32 index = 0;
		IFXCHECKX(m_pCoreServices->GetSceneGraph(IID_IFXSceneGraph,(void**)&pSG));
		pSG->GetPalette( IFXSceneGraph::NODE, &pPalette );
		pSG->GetPalette( IFXSceneGraph::GENERATOR, &pGP );
		pGP->Find(m_stringObjectName, &index);
		U32 size = 0;
		pPalette->GetPaletteSize(&size);
		U32 i;
		for ( i = 0; i < size; i++) {
			IFXDECLARELOCAL(IFXModel,pObject);
			pPalette->GetResourcePtr( i, IID_IFXModel, (void**)&pObject );
			if (pObject && (pObject->GetResourceIndex() == index))
				pObject->SetResourceIndex(index);
		}
	}

	// Check for partial transfer
	if(NULL != m_pBitStreamX || m_bDone == FALSE) {
		rWarningPartialTransfer = IFX_W_PARTIAL_TRANSFER;
	} else {
		rWarningPartialTransfer = IFX_OK;
	}
}


//-----------------------------------------------------------------------------
//  Private methods
//-----------------------------------------------------------------------------

// Constuctor
CIFXAuthorCLODDecoder::CIFXAuthorCLODDecoder() :
IFXDEFINEMEMBER(m_pBitStreamX),
IFXDEFINEMEMBER(m_pAuthorCLODResource),
IFXDEFINEMEMBER(m_pReconstructedAuthorCLODMesh),
IFXDEFINEMEMBER(m_pSetAdjacencyX)
{
	m_uRefCount = 0;
	m_uLoadId = 0;
	m_bDone = TRUE;
	m_eWIPType = WIP_NONE;
	m_fInverseQuantPosition = 1.0f;
	m_fInverseQuantNormal = 1.0f;
	m_fInverseQuantTexCoord = 1.0f;
	m_fNormalCreaseParameter = 0.25f; /// @todo: Tune this value?
	m_fNormalUpdateParameter = 0.5f; /// @todo: Tune this value?
	m_fNormalTolerance = 0.985f;

	m_unitScale = 1.0f;
}

// Destructor
CIFXAuthorCLODDecoder::~CIFXAuthorCLODDecoder()
{
	if(m_pAuthorCLODResource) {
		m_pAuthorCLODResource->SetAuthorMeshFinal(m_pReconstructedAuthorCLODMesh);
	}
}

//
//
// Read the block info for AuthorCLODDeclaration
//
//  1. IFXString Name
//  2. U32 Chain Index (always 0 for this block)
//  3. Max Mesh Description
//    3.1 U32 NumFaces;
//    3.2 U32 NumPositions;
//    3.3 U32 NumNormals;
//    3.4 U32 NumDiffuseColors;
//    3.5 U32 NumSpecularColors;
//    3.6 U32 NumTexCoords;
//    3.7 U32 NumShaders;
//    3.8 Shaders Descriptions [NumShaders]
//      3.8.1 U32 Shading Attributes (collection of flags: BOOL m_uDiffuseColors; BOOL m_uSpecularColors;)
//      3.8.2 U32 m_uNumTextureLayers;
//      3.8.3 U32 m_uNumTextureLayers[m_uNumTextureLayers]; (IFXMAX_TEX_COORD_LAYERS == 8)
//      3.8.4 U32 OriginalShadingID;  // the mesh scrubber may have removed unused materials an repacked them.
//  4. CLOD Description
//    4.1 U32 Min Resolution
//    4.2 U32 Final Max Resolution
//  5. Resource Description
//    5.1 Quality Factors
//          5.1.1 U32 Position Quality Factor
//          5.1.2 U32 Normal Quality Factor
//          5.1.3 U32 Texture Coord Quality Factor
//    5.2 Inverse Quantization
//          5.2.1 F32 Position Inverse Quant
//          5.2.2 F32 Normal Inverse Quant
//          5.2.3 F32 Texture Coord Inverse Quant
//          5.2.4 F32 Diffuse Color Inverse Quant
//          5.2.5 F32 Specular Color Inverse Quant
//    5.3 Resource parameters
//          5.3.1 F32 Normal Crease Param
//          5.3.2 F32 Normal Update Param
//          5.3.3 F32 Normal Tolerance Param
//  6. Skeleton Description (details below)
//
//
// The m_pReconstructedAuthorCLODMesh is also allocated and the shader
// information is filled in.

// Make the model resource declaration block
void CIFXAuthorCLODDecoder::ProcessAuthorCLODDeclarationBlockX( U32 uBlockType )
{
	IFXASSERT(m_pBitStreamX);
	IFXDECLARELOCAL(IFXNameMap, pNameMap);
	m_pCoreServices->GetNameMap(IID_IFXNameMap, (void**)&pNameMap);

	//  1. Name
	IFXString stringName;
	m_pBitStreamX->ReadIFXStringX(stringName);
	IFXCHECKX(pNameMap->Map(m_uLoadId, IFXSceneGraph::GENERATOR, stringName));

	//  2. U32 Chain Index
	if( uBlockType == BlockType_GeneratorCLODMeshU3D )
	{
		U32 uChainIndex;
		m_pBitStreamX->ReadU32X(uChainIndex);
	}

	// 3. Mesh attributes (Exclude Normals)
	/// @todo: Renumber below comments
	BOOL bExcludeNormals = FALSE;
	{
		U32 uTemp = 0;
		m_pBitStreamX->ReadU32X(uTemp);
		if(1 & uTemp) {
			bExcludeNormals = TRUE;
		}
	}
	m_pAuthorCLODResource->SetExcludeNormals(bExcludeNormals);

	if( m_unitScale > 0.0f && m_unitScale != 1.0f )
	{
		IFXMatrix4x4 matrix = m_pAuthorCLODResource->GetTransform();
		IFXVector3 scale( (F32)m_unitScale, (F32)m_unitScale, (F32)m_unitScale );
		matrix.Scale( scale );
		m_pAuthorCLODResource->SetTransform( matrix );
	}

	//  3. Max Mesh Description
	IFXAuthorMeshDesc samdMaxMeshDescription;
	IFXAuthorMeshDesc* pMaxMeshDescription = &samdMaxMeshDescription;
	IFXASSERT(pMaxMeshDescription);
	//    3.1 U32 NumFaces;
	m_pBitStreamX->ReadU32X(pMaxMeshDescription->NumFaces);
	//    3.2 U32 NumPositions;
	m_pBitStreamX->ReadU32X(pMaxMeshDescription->NumPositions);
	//    3.3 U32 NumNormals;
	m_pBitStreamX->ReadU32X(pMaxMeshDescription->NumNormals);
	//    3.4 U32 NumDiffuseColors;
	m_pBitStreamX->ReadU32X(pMaxMeshDescription->NumDiffuseColors);
	//    3.5 U32 NumSpecularColors;
	m_pBitStreamX->ReadU32X(pMaxMeshDescription->NumSpecularColors);
	//    3.6 U32 NumTexCoords;
	m_pBitStreamX->ReadU32X(pMaxMeshDescription->NumTexCoords);
	//    3.7 U32 NumShaders;
	m_pBitStreamX->ReadU32X(pMaxMeshDescription->NumMaterials);

	// Create the reconstructed author CLOD mesh and allocate
	{
		IFXRELEASE(m_pReconstructedAuthorCLODMesh);
		IFXCHECKX(IFXCreateComponent(CID_IFXAuthorMesh,IID_IFXAuthorCLODMesh,(void**)&m_pReconstructedAuthorCLODMesh));
		IFXASSERT(m_pReconstructedAuthorCLODMesh);
		IFXCHECKX(m_pReconstructedAuthorCLODMesh->Allocate(&samdMaxMeshDescription));
	}

	//    3.8 Shaders Descriptions
	{
		IFXAuthorMaterial* pAuthorMeshMaterial = NULL;
		IFXCHECKX(m_pReconstructedAuthorCLODMesh->Lock());
		IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetMaterials(&pAuthorMeshMaterial));
		U32 i = 0;

		for(i=0; i < pMaxMeshDescription->NumMaterials; i++) {
			//  3.8.1 Shading Attributes (BOOL m_uDiffuseColors; BOOL m_uSpecularColors;)
			U32 uMaterialAttributes = 0;
			m_pBitStreamX->ReadU32X(uMaterialAttributes);
			if(uMaterialAttributes & 1) {
				pAuthorMeshMaterial[i].m_uDiffuseColors = TRUE;
			} else {
				pAuthorMeshMaterial[i].m_uDiffuseColors = FALSE;
			}
			if(uMaterialAttributes & 2) {
				pAuthorMeshMaterial[i].m_uSpecularColors = TRUE;
			} else {
				pAuthorMeshMaterial[i].m_uSpecularColors = FALSE;
			}
			if(bExcludeNormals) {
				pAuthorMeshMaterial[i].m_uNormals = FALSE;
			} else {
				pAuthorMeshMaterial[i].m_uNormals = TRUE;
			}

			//  3.8.2 U32 m_uNumTextureLayers;
			m_pBitStreamX->ReadU32X(pAuthorMeshMaterial[i].m_uNumTextureLayers);
			//  3.8.3 U32 m_uNumTextureLayers[m_uNumTextureLayers]; (IFXMAX_TEX_COORD_LAYERS == 8)
			IFXASSERT(IFX_MAX_TEXUNITS >= pAuthorMeshMaterial[i].m_uNumTextureLayers);
			U32 j = 0;
			for(j = 0; j < pAuthorMeshMaterial[i].m_uNumTextureLayers; j++) {
				m_pBitStreamX->ReadU32X(pAuthorMeshMaterial[i].m_uTexCoordDimensions[j]);
			}
			//  3.8.4 U32 OriginalShadingID;  // the mesh scrubber may have removed unused materials an repacked them.
			m_pBitStreamX->ReadU32X(pAuthorMeshMaterial[i].m_uOriginalMaterialID);
		}
		IFXCHECKX(m_pReconstructedAuthorCLODMesh->Unlock());
	}

	//  4. CLOD Description
	//    4.1 Min Resolution
	U32 uMinResolution = 0;
	m_pBitStreamX->ReadU32X(uMinResolution);

	//    4.2 Final Max Resolution
	U32 uFinalMaxResolution = 0;
	m_pBitStreamX->ReadU32X(uFinalMaxResolution);
	m_pReconstructedAuthorCLODMesh->SetFinalMaxResolution(uFinalMaxResolution);
	m_pReconstructedAuthorCLODMesh->SetMaxResolution(uFinalMaxResolution);
	m_pReconstructedAuthorCLODMesh->SetMinResolution(uMinResolution);

	// Set up the SetAdjacency object
	IFXCHECKX(IFXCreateComponent(CID_IFXSetAdjacencyX,IID_IFXSetAdjacencyX,(void**)&m_pSetAdjacencyX));
	m_pSetAdjacencyX->InitializeX(m_pReconstructedAuthorCLODMesh);

	//  5. Extra resource info
	//    5.1 Quality Factors
	U32 uQualityFactor = 0;
	m_pBitStreamX->ReadU32X(uQualityFactor);
	m_pAuthorCLODResource->SetQualityFactorX(uQualityFactor,IFXAuthorCLODResource::POSITION_QUALITY);
	m_pBitStreamX->ReadU32X(uQualityFactor);
	m_pBitStreamX->ReadU32X(uQualityFactor);
	m_pAuthorCLODResource->SetQualityFactorX(uQualityFactor,IFXAuthorCLODResource::TEXCOORD_QUALITY);

	//    5.2 Inverse Quantization Factors
	m_pBitStreamX->ReadF32X(m_fInverseQuantPosition);
	m_pBitStreamX->ReadF32X(m_fInverseQuantNormal);
	m_pBitStreamX->ReadF32X(m_fInverseQuantTexCoord);
	m_pBitStreamX->ReadF32X(m_fInverseQuantDiffuseColor);
	m_pBitStreamX->ReadF32X(m_fInverseQuantSpecularColor);

	//    5.3 Resource Parameters
	m_pBitStreamX->ReadF32X(m_fNormalCreaseParameter);
	m_pAuthorCLODResource->SetNormalCreaseParameter(m_fNormalCreaseParameter);
	m_pBitStreamX->ReadF32X(m_fNormalUpdateParameter);
	m_pAuthorCLODResource->SetNormalUpdateParameter(m_fNormalUpdateParameter);
	m_pBitStreamX->ReadF32X(m_fNormalTolerance);
	m_pAuthorCLODResource->SetNormalTolerance(m_fNormalTolerance);

	//  6. Skeleton Description
	if( uBlockType == BlockType_GeneratorCLODMeshU3D )
	{
		U32 uBoneCount;
		IFXBoneInfo *pBoneInfo = NULL;

		// The skeleton modifier block consists of following fields:
		// 6.1. BoneCount
		// 6.2. Bone (repeated BoneCount times):
		// a) BoneName
		// b) BoneParentID
		// c) BoneLength
		// d) BoneDisplacement
		// e) BoneRotation
		// f) BoneAttributes
		// g) NumberLinks
		// h) LinkLength
		// i) StartJoint
		// j) EndJoint
		// k) RotationalConstraint

		// 6.1. Read the number of bones
		// Some 1st edition U3D files were incorrectly written without a bone count
		// if the bone count was zero and no empty skeleton structure was available,
		// but bit stream component was designed to handle this by handing back zero
		// when reads are done past the end of a block. So nothing is broken here.
		m_pBitStreamX->ReadU32X( uBoneCount );

		// Don't attempt to allocate this if
		// there are no bones:
		if ( (0 != uBoneCount) )
		{
			IFXSkeleton *pSkeleton = NULL;

			pBoneInfo = new IFXBoneInfo[uBoneCount];
			if ( NULL == pBoneInfo )
			{
				IFXCHECKX(IFX_E_OUT_OF_MEMORY);
			}

			// 6.2. Read the bones
			U32 i = 0;
			F32 x,y,z;
			for ( i = 0; i < uBoneCount ; i++ )
			{
				// 6.2.1 BoneName
				m_pBitStreamX->ReadIFXStringX((pBoneInfo[i].stringBoneName));
				IFXCHECKX(pNameMap->Map(m_uLoadId, IFXSceneGraph::MOTION, pBoneInfo[i].stringBoneName));

				// 6.2.2 BoneParentID
				m_pBitStreamX->ReadIFXStringX((pBoneInfo[i].stringParentName));
				IFXCHECKX(pNameMap->Map(m_uLoadId, IFXSceneGraph::MOTION, pBoneInfo[i].stringParentName));

				// 6.2.3 BoneAttributes
				U32 uBoneAttributes = 0;
				m_pBitStreamX->ReadU32X(uBoneAttributes);
				pBoneInfo[i].uBoneAttributes = uBoneAttributes;

				// 6.2.4 BoneLength
				m_pBitStreamX->ReadF32X((pBoneInfo[i].fBoneLength));

				// 6.2.5 BoneDisplacement
				m_pBitStreamX->ReadF32X(x);
				m_pBitStreamX->ReadF32X(y);
				m_pBitStreamX->ReadF32X(z);
				pBoneInfo[i].v3BoneDisplacement.Set(x,y,z);

				// 6.2.6 BoneRotation
				F32 a,b,c,d;
				m_pBitStreamX->ReadF32X(a);
				m_pBitStreamX->ReadF32X(b);
				m_pBitStreamX->ReadF32X(c);
				m_pBitStreamX->ReadF32X(d);
				pBoneInfo[i].v4BoneRotation.Set(a,b,c,d);

				if(IFXSKELETON_LINKPRESENT & uBoneAttributes)
				{
					// 6.2.7 NumberLinks
					m_pBitStreamX->ReadU32X((pBoneInfo[i].uNumLinks));

					// 6.2.8 LinkLength
					m_pBitStreamX->ReadF32X((pBoneInfo[i].fLinkLength));
				}

				if(IFXSKELETON_JOINTPRESENT & uBoneAttributes)
				{
					// 6.2.9 StartJoint
					m_pBitStreamX->ReadF32X(a);
					m_pBitStreamX->ReadF32X(b);
					m_pBitStreamX->ReadF32X(c);
					m_pBitStreamX->ReadF32X(d);

					pBoneInfo[i].v2StartJointCenter.Set(a,b);
					pBoneInfo[i].v2StartJointScale.Set(c,d);

					// 6.2.10 EndJoint
					m_pBitStreamX->ReadF32X(a);
					m_pBitStreamX->ReadF32X(b);
					m_pBitStreamX->ReadF32X(c);
					m_pBitStreamX->ReadF32X(d);

					pBoneInfo[i].v2EndJointCenter.Set(a,b);
					pBoneInfo[i].v2EndJointScale.Set(c,d);
				}

				// 6.2.11 RotationalConstraint
				m_pBitStreamX->ReadF32X((pBoneInfo[i].fRotationConstraintXMax));
				m_pBitStreamX->ReadF32X((pBoneInfo[i].fRotationConstraintXMin));
				m_pBitStreamX->ReadF32X((pBoneInfo[i].fRotationConstraintYMax));
				m_pBitStreamX->ReadF32X((pBoneInfo[i].fRotationConstraintYMin));
				m_pBitStreamX->ReadF32X((pBoneInfo[i].fRotationConstraintZMax));
				m_pBitStreamX->ReadF32X((pBoneInfo[i].fRotationConstraintZMin));
			}

			IFXCreateComponent( CID_IFXSkeleton, IID_IFXSkeleton, (void **)&pSkeleton );

			U32 j = 0;
			for( i = 0; i < uBoneCount; i++ )
			{
				pBoneInfo[i].iParentBoneID = -1;

				for( j = 0; j < uBoneCount; j++ )
				{
					if( i != j && pBoneInfo[i].stringParentName == pBoneInfo[j].stringBoneName )
					{
						pBoneInfo[i].iParentBoneID = j;
						j = uBoneCount;
					}
				}
			}

			for( i = 0; i < uBoneCount; i++ )
			{
				pSkeleton->SetBoneInfo( i, &pBoneInfo[i] );
			}

            IFXBonesManager *pBonesManager = pSkeleton->GetBonesManagerNR();
            IFXCHECKX(NULL != pBonesManager);

            IFXRESULT result = pBonesManager->UpdateReferencePositions();
            IFXCHECKX(IFX_OK == result);

			// Delete old/default skeleton
			IFXSkeleton *pCadaver = NULL;
			pCadaver = m_pAuthorCLODResource->GetBones();
			IFXRELEASE( pCadaver );

			m_pAuthorCLODResource->SetBones( pSkeleton );
			IFXRELEASE( pSkeleton );

			delete [] pBoneInfo;
		}
	}

	IFXRELEASE(m_pBitStreamX);
	m_eWIPType = WIP_NONE;
}

//-----------------------------------------------------------------------------
// Factory Method
//-----------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXAuthorCLODDecoder_Factory(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		CIFXAuthorCLODDecoder *pComponent = new CIFXAuthorCLODDecoder;

		if ( pComponent ) {
			pComponent->AddRef();
			rc = pComponent->QueryInterface( interfaceId, ppInterface );
			pComponent->Release();
		} else {
			rc = IFX_E_OUT_OF_MEMORY;
		}
	} else {
		rc = IFX_E_INVALID_POINTER;
	}

	return rc;
}
