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
	@file	CIFXPointSetDecoder.cpp

			The CIFXPointSetDecoder is used by the CIFXLoadManager to load
			Point Set primitive generator into the scene graph. CIFXPointSetDecoder
			exposes an IFXDecoder interface to the CIFXLoadManager for this
			purpose.
*/

//#define KEEP_DATA_BLOCKS

#include "IFXCoreCIDs.h"
#include "CIFXPointSetDecoder.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"
#include "IFXAuthor.h"
#include "IFXSkeleton.h"
#include "IFXACContext.h"
#include "IFXModel.h"
#include "IFXBonesManager.h"


//#define DEBUG_VERBOSE
//void DumpAuthorPointSet();

const U32 CIFXPointSetDecoder::m_uMaximumUpdatesPerCall = 64;

CIFXPointSetDecoder::CIFXPointSetDecoder() :
	IFXDEFINEMEMBER(m_pBitStreamX),
	IFXDEFINEMEMBER(m_pPointSetResource),
	IFXDEFINEMEMBER(m_pAuthorPointSet)
{
	m_uRefCount = 0;
	m_uLoadId = 0;
	m_bDone = TRUE;
	m_eWIPType = WIP_NONE;


	m_fMinResolution = 1.0f;
	m_fMaxResolution = 1.0f;

	m_fInverseQuantPosition = 1.0f;
	m_fInverseQuantNormal	= 1.0f;
	m_fInverseQuantTexCoord = 1.0f;

	m_uReservedPointSetParameter0	= 0;
	m_uReservedPointSetParameter1	= 0;
	m_uReservedPointSetParameter2	= 0;
	m_uReservedPointSetParameter3	= 0;

	m_pMaxPointSetDesc			= NULL;
	m_pPointSetDesc				= NULL;
	m_uPositionsRead			= (U32)-1;

	m_uStartResolution =0;
	m_uEndResolution   =0;
	m_currentIndex     =0;

	m_unitScale = 1.0f;
}


CIFXPointSetDecoder::~CIFXPointSetDecoder()
{
}

U32 CIFXPointSetDecoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXPointSetDecoder::Release()
{
	if ( 1 == m_uRefCount ) {
		delete this;
		return 0;
	}
	return --m_uRefCount;
}

IFXRESULT CIFXPointSetDecoder::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
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

// Initialize and get a reference to the core services object
void CIFXPointSetDecoder::InitializeX(const IFXLoadConfig &lc)
{
	if( lc.m_units > 0.0f )
		m_unitScale = lc.m_units;
	IFXModifierBaseDecoder::InitializeX(lc);																			
}

// Provide next block of data to the loader
void CIFXPointSetDecoder::PutNextBlockX( IFXDataBlockX &rDataBlockX )
{
	if(NULL == m_pDataBlockQueueX) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	m_pDataBlockQueueX->AppendBlockX( rDataBlockX );

	if (NULL == m_pObject) 
	{
		U32 uBlockType;
		rDataBlockX.GetBlockTypeX(uBlockType);
		CreateObjectX(rDataBlockX, CID_IFXAuthorPointSetResource);

		IFXRELEASE(m_pPointSetResource);
		IFXCHECKX(m_pObject->QueryInterface(IID_IFXAuthorPointSetResource,(void**)&m_pPointSetResource));

#ifdef KEEP_DATA_BLOCKS
		IFXCHECKX(m_pPointSetResource->BuildDataBlockQueue());
#endif
	}

	m_bDone = FALSE;

#ifdef KEEP_DATA_BLOCKS
	// Add the data block to the generator. This is used by the generator as a
	// cached copy of itself in encoded form thus saving potential encoding at
	// save or export time.
	IFXDECLARELOCAL(IFXDataBlockQueueX,pGeneratorDataBlockQueueX);
	m_pPointSetResource->GetDataBlockQueueX( pGeneratorDataBlockQueueX );
	if ( pGeneratorDataBlockQueueX )
		pGeneratorDataBlockQueueX->AppendBlockX( rDataBlockX );
#endif
}

void CIFXPointSetDecoder::TransferX(IFXRESULT &rWarningPartialTransfer)
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
		case BlockType_GeneratorPointSetU3D :
			{
				// set metadata
				IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
				IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
				pDataBlockX->QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
				m_pPointSetResource->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
				pObjectMD->AppendX(pBlockMD);
			}
			m_eWIPType = WIP_DECLARATION_U3D;
			break;   
        case BlockType_GeneratorPointSetContinuationU3D :
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
		ProcessAuthorPointSetDeclarationBlockX();
		IFXRELEASE(m_pBitStreamX);
		break;
    case WIP_PROGRESSIVE_U3D:
		ProcessAuthorPointSetContinuationBlockX();
		break;
    case WIP_NONE:
		break;
	default:
		break;
	}

	if (wip != WIP_NONE) {
		IFXCHECKX(m_pPointSetResource->SetAuthorPointSet(m_pAuthorPointSet));
	}
    
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


void CIFXPointSetDecoder::ProcessAuthorPointSetDeclarationBlockX()
{

	IFXASSERT(m_pBitStreamX);
#ifdef _DEBUG
	IFXRESULT rc = IFX_OK;
#endif
	
    //
	// Read the block info for AuthorPointSetDeclaration:
    //
	//	1. IFXString Name
    //  2. U32 Chain Index (always 0 for this block)
	//	3. Max PointSet Description
	//		3.1	U32 NumPoints;
	//		3.2 U32 NumPositions;
	//		3.3 U32 NumNormals;	
	//		3.4 U32 NumDiffuseColors;
	//		3.5 U32 NumSpecularColors;
	//		3.6 U32 NumTexCoords;
	//		3.7 U32 NumShaders;
	//		3.8 Shaders Descriptions [NumShaders]
	//			3.8.1	U32 Shading Attributes (collection of flags: BOOL DiffuseColors; BOOL SpecularColors;)
	//			3.8.2	U32 NumTextureLayers;
	//			3.8.3	U32 TexCoordDimensions[NumTextureLayers]; (IFXMAX_TEX_COORD_LAYERS == 8)
	//			3.8.4	U32 OriginalShadingID;  
	//	4. Resource Description
	//		4.1 Quality Factors
    //          4.1.1 U32 Position Quality Factor
    //          4.1.2 U32 Normal Quality Factor
    //          4.1.3 U32 Texture Coord Quality Factor
	//		4.2 Inverse Quantization
    //          4.2.1 F32 Position Inverse Quant
    //          4.2.2 F32 Normal Inverse Quant
    //          4.2.3 F32 Texture Coord Inverse Quant
    //          4.2.4 F32 Diffuse Color Inverse Quant
    //          4.2.5 F32 Specular Color Inverse Quant
	//		4.3 Resource parameters
    //          4.3.1 U32 Normal Crease Param
    //          4.3.2 U32 Normal Update Param
    //          4.3.3 u32 Normal Tolerance Param
    //  5. Skeleton Description
    //

	//	1. Name
	IFXString stringName;
	m_pBitStreamX->ReadIFXStringX(stringName);

	//	2. Chain Index
	U32 uChainIndex;
    m_pBitStreamX->ReadU32X(uChainIndex);

	//	3. Max Point Description
	IFXAuthorPointSetDesc sMaxPointSetDesc;
	IFXAuthorPointSetDesc* pMaxPointSetDesc = &sMaxPointSetDesc;

	if( m_unitScale > 0.0f && m_unitScale != 1.0f )
	{
		IFXMatrix4x4 matrix = m_pPointSetResource->GetTransform();
		IFXVector3 scale( (F32)m_unitScale, (F32)m_unitScale, (F32)m_unitScale );
		matrix.Scale( scale );
		m_pPointSetResource->SetTransform( matrix );
	}

	IFXASSERT(pMaxPointSetDesc);
	//      3.0 U32 Point Set Reserved
	m_pBitStreamX->ReadU32X(m_uReservedPointSetParameter0);
	//		3.1	U32 NumPoints;
	m_pBitStreamX->ReadU32X(pMaxPointSetDesc->m_numPoints);
	//		3.2 U32 NumPositions;
	m_pBitStreamX->ReadU32X(pMaxPointSetDesc->m_numPositions);
	//		3.3 U32 NumNormals;
	m_pBitStreamX->ReadU32X(pMaxPointSetDesc->m_numNormals);
	//		3.4 U32 NumDiffuseColors;
	m_pBitStreamX->ReadU32X(pMaxPointSetDesc->m_numDiffuseColors);
	//		3.5 U32 NumSpecularColors;
	m_pBitStreamX->ReadU32X(pMaxPointSetDesc->m_numSpecularColors);
	//		3.6 U32 NumTexCoords;
	m_pBitStreamX->ReadU32X(pMaxPointSetDesc->m_numTexCoords);
	//		3.7 U32 NumShader;
	m_pBitStreamX->ReadU32X(pMaxPointSetDesc->m_numMaterials);


	// Create the reconstructed author Points and allocate
	{
		IFXAuthorPointSetDesc sZeroPointSetDesc;
		sZeroPointSetDesc.m_numPoints			=0;
		sZeroPointSetDesc.m_numDiffuseColors	=0;
		sZeroPointSetDesc.m_numNormals			=0;
		sZeroPointSetDesc.m_numPositions		=0;
		sZeroPointSetDesc.m_numSpecularColors	=0;
		sZeroPointSetDesc.m_numTexCoords		=0;
		sZeroPointSetDesc.m_numMaterials       =pMaxPointSetDesc->m_numMaterials;

		IFXRELEASE(m_pAuthorPointSet);
		IFXCHECKX(IFXCreateComponent(CID_IFXAuthorPointSet,IID_IFXAuthorPointSet,(void**)&m_pAuthorPointSet));
		IFXASSERT(m_pAuthorPointSet);
		
		//Streatch arrays to accomodate full info - this is temp - need to find out how to do more effectively
		if (pMaxPointSetDesc->m_numNormals && pMaxPointSetDesc->m_numNormals < 2*pMaxPointSetDesc->m_numPoints) 
			pMaxPointSetDesc->m_numNormals = 2*pMaxPointSetDesc->m_numPoints;
		if (pMaxPointSetDesc->m_numDiffuseColors && pMaxPointSetDesc->m_numDiffuseColors < 2*pMaxPointSetDesc->m_numPoints) 
			pMaxPointSetDesc->m_numDiffuseColors = 2*pMaxPointSetDesc->m_numPoints;
		if (pMaxPointSetDesc->m_numSpecularColors && pMaxPointSetDesc->m_numSpecularColors < 2*pMaxPointSetDesc->m_numPoints) 
			pMaxPointSetDesc->m_numSpecularColors = 2*pMaxPointSetDesc->m_numPoints;
		if (pMaxPointSetDesc->m_numTexCoords && pMaxPointSetDesc->m_numTexCoords < 2*pMaxPointSetDesc->m_numPoints) 
			pMaxPointSetDesc->m_numTexCoords = 2*pMaxPointSetDesc->m_numPoints;

        IFXCHECKX(m_pAuthorPointSet->Allocate(&sMaxPointSetDesc));

#ifdef _DEBUG
		rc =
#endif
		m_pAuthorPointSet->SetPointSetDesc(&sZeroPointSetDesc);
		IFXASSERT(rc == IFX_OK);
	}

	// 3.8 Shaders Descriptions [NumShaders]
	{
		IFXAuthorMaterial* pMaterial = NULL;
		IFXCHECKX(m_pAuthorPointSet->GetMaterials(&pMaterial));

		U32 i = 0;
		for( i=0; i < pMaxPointSetDesc->m_numMaterials; i++) {
			
			//	3.8.1	Shading Attributes (BOOL DiffuseColors; BOOL SpecularColors;)
			U32 uMaterialAttributes = 0;
			m_pBitStreamX->ReadU32X(uMaterialAttributes);
			if(uMaterialAttributes & 1) {
				pMaterial[i].m_uDiffuseColors = TRUE;
			} else {
				pMaterial[i].m_uDiffuseColors = FALSE;
			}
			if(uMaterialAttributes & 2) {
				pMaterial[i].m_uSpecularColors = TRUE;
			} else {
				pMaterial[i].m_uSpecularColors = FALSE;
			}

			if(pMaxPointSetDesc->m_numNormals)
				pMaterial[i].m_uNormals = TRUE;
			else
				pMaterial[i].m_uNormals = FALSE;

			//	3.8.2	U32 NumTextureLayers;
			m_pBitStreamX->ReadU32X(pMaterial[i].m_uNumTextureLayers );
			//	3.8.3	U32 TexCoordDimensions[NumTextureLayers]; (IFXMAX_TEX_COORD_LAYERS == 8)
			IFXASSERT(IFX_MAX_TEXUNITS >= pMaterial[i].m_uNumTextureLayers);
			
			U32 j = 0;
			for(j = 0; j < pMaterial[i].m_uNumTextureLayers; j++) {
				m_pBitStreamX->ReadU32X(pMaterial[i].m_uTexCoordDimensions[j]);
			}
			//	3.8.4	U32 OriginalShadingID;  
			m_pBitStreamX->ReadU32X(pMaterial[i].m_uOriginalMaterialID);
		}
	}

	//	4. Extra resource info
	//		4.1 Quality Factors
	U32 uQualityFactor = 0;
			// 4.1.1
	m_pBitStreamX->ReadU32X(uQualityFactor); 
			// 4.1.2
	m_pBitStreamX->ReadU32X(uQualityFactor); 
			// 4.1.3
	m_pBitStreamX->ReadU32X(uQualityFactor);

    //		4.2 Inverse Quantization Factors
	m_pBitStreamX->ReadF32X(m_fInverseQuantPosition);
	m_pBitStreamX->ReadF32X(m_fInverseQuantNormal);
	m_pBitStreamX->ReadF32X(m_fInverseQuantTexCoord);
	m_pBitStreamX->ReadF32X(m_fInverseQuantDiffuseColor);
	m_pBitStreamX->ReadF32X(m_fInverseQuantSpecularColor);

    //		4.3 Resource Parameters
	m_pBitStreamX->ReadU32X(m_uReservedPointSetParameter1);
	m_pBitStreamX->ReadU32X(m_uReservedPointSetParameter2);
	m_pBitStreamX->ReadU32X(m_uReservedPointSetParameter3);
    
    //  5. Skeleton Description
	U32 uBoneCount;
	IFXBoneInfo *pBoneInfo = NULL;

	// The skeleton modifier block consists of following fields:
	// 5.1. BoneCount
	// 5.2. Bone (repeated BoneCount times):
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

	// 5.1. Read the number of bones
	// Some 1st edition U3D files were incorrectly written without a bone count
	// if the bone count was zero and no empty skeleton structure was available,
	// but bit stream component was designed to handle this by handing back zero
	// when reads are done past the end of a block. So nothing is broken here.
	m_pBitStreamX->ReadU32X( uBoneCount );

	// Don't attempt to allocate this if
	// there are no bones:
	if ( 0 != uBoneCount)
	{
		IFXSkeleton *pSkeleton = NULL;

		pBoneInfo = new IFXBoneInfo[uBoneCount];
		if ( NULL == pBoneInfo ) {
			IFXCHECKX(IFX_E_OUT_OF_MEMORY);
		}

		// 5.2. Read the bones
		U32 i = 0;
		F32 x,y,z;
		for ( i = 0; i < uBoneCount ; i++ ) 
		{
			// 5.2.1 BoneName
			m_pBitStreamX->ReadIFXStringX((pBoneInfo[i].stringBoneName));

			// 5.2.2 BoneParentID
			m_pBitStreamX->ReadIFXStringX((pBoneInfo[i].stringParentName));

			// 5.2.3 BoneAttributes
			U32 uBoneAttributes = 0;
			m_pBitStreamX->ReadU32X(uBoneAttributes);
			pBoneInfo[i].uBoneAttributes = uBoneAttributes;

			// 5.2.4 BoneLength
			m_pBitStreamX->ReadF32X((pBoneInfo[i].fBoneLength));

			// 5.2.5 BoneDisplacement
			m_pBitStreamX->ReadF32X(x);
			m_pBitStreamX->ReadF32X(y);
			m_pBitStreamX->ReadF32X(z);
			pBoneInfo[i].v3BoneDisplacement.Set(x,y,z);

			// 5.2.6 BoneRotation
			F32 a,b,c,d;
			m_pBitStreamX->ReadF32X(a);
			m_pBitStreamX->ReadF32X(b);
			m_pBitStreamX->ReadF32X(c);
			m_pBitStreamX->ReadF32X(d);
			pBoneInfo[i].v4BoneRotation.Set(a,b,c,d);

			if(IFXSKELETON_LINKPRESENT & uBoneAttributes)
			{
				// 5.2.7 NumberLinks
				m_pBitStreamX->ReadU32X((pBoneInfo[i].uNumLinks));

				// 5.2.8 LinkLength
				m_pBitStreamX->ReadF32X((pBoneInfo[i].fLinkLength));
			}

			if(IFXSKELETON_JOINTPRESENT & uBoneAttributes)
			{
				// 5.2.9 StartJoint
				m_pBitStreamX->ReadF32X(a);
				m_pBitStreamX->ReadF32X(b);
				m_pBitStreamX->ReadF32X(c);
				m_pBitStreamX->ReadF32X(d);

				pBoneInfo[i].v2StartJointCenter.Set(a,b);
				pBoneInfo[i].v2StartJointScale.Set(c,d);

				// 5.2.10 EndJoint
				m_pBitStreamX->ReadF32X(a);
				m_pBitStreamX->ReadF32X(b);
				m_pBitStreamX->ReadF32X(c);
				m_pBitStreamX->ReadF32X(d);

				pBoneInfo[i].v2EndJointCenter.Set(a,b);
				pBoneInfo[i].v2EndJointScale.Set(c,d);
			}

			// 5.2.11 RotationalConstraint
			m_pBitStreamX->ReadF32X((pBoneInfo[i].fRotationConstraintXMax));
			m_pBitStreamX->ReadF32X((pBoneInfo[i].fRotationConstraintXMin));
			m_pBitStreamX->ReadF32X((pBoneInfo[i].fRotationConstraintYMax));
			m_pBitStreamX->ReadF32X((pBoneInfo[i].fRotationConstraintYMin));
			m_pBitStreamX->ReadF32X((pBoneInfo[i].fRotationConstraintZMax));
			m_pBitStreamX->ReadF32X((pBoneInfo[i].fRotationConstraintZMin));
		}

		IFXCHECKX(IFXCreateComponent( CID_IFXSkeleton, IID_IFXSkeleton, (void **)&pSkeleton ));

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

		m_pPointSetResource->SetBones( pSkeleton );

        IFXBonesManager *pBonesManager = pSkeleton->GetBonesManagerNR();
        IFXCHECKX(NULL != pBonesManager);

        IFXRESULT result = pBonesManager->UpdateReferencePositions();
        IFXCHECKX(IFX_OK == result);
        
        IFXRELEASE( pSkeleton );
		delete [] pBoneInfo;
	}

	m_eWIPType = WIP_NONE;	
}


void CIFXPointSetDecoder::ProcessAuthorPointSetContinuationBlockX()
{
	// grab the IFXPointSet handle to the current modifier component:
	IFXASSERT(m_pBitStreamX);
	IFXASSERT(m_pAuthorPointSet);

	if (m_uWIPi == 0) {
		// get the point set description
		m_pMaxPointSetDesc = m_pAuthorPointSet->GetMaxPointSetDesc();
		m_pPointSetDesc    = m_pAuthorPointSet->GetPointSetDesc();

		//
		// Read the Continutaion block info for AuthorPointSet
		//
		//	1. Name
		//  2. Chain Index (always 0 for this block)
		//	3. Resolution Range 
		//		3.1	U32 m_uStartResolution; - have to be 0 
		//		3.2 U32 EndResulution; - have to be PointSetDesc->numPositions
		//	4. Point Description (= Verstex Update) [NumVertexUpdates]
		//      4.1 U32[rCurrentPositionCount] Split Positions index 
		//      4.2 New Poistion Info  (as quantized position difference)
		//          4.2.1 U8[cDiffPosSign]  position Difference Signs
		//          4.2.2 U32[cDiffPos] position difference X
		//          4.2.3 U32[cDiffPos] position difference Y
		//          4.2.4 U32[cDiffPos] position difference Z
		//	    4.3 U32[cNormlCnt]:New Normal Count
		//      4.4 New Normal Info [New Normal Count]
		//          4.4.1 U8[cDiffNormalSign]  position Difference Signs
		//          4.4.2 U32[cDiffNormal] normal difference X
		//          4.4.3 U32[cDiffNormal] normal difference Y
		//          4.4.4 U32[cDiffNormal] normal difference Z
		//	    4.5 U32[cPointCnt]:New Point Count
		//      4.6 New Point Info [New Point Count]
		//          4.6.1 U32[cShading]  Shading ID 
		//          4.6.2 U32[cNormlIndx] normal local index
		//          4.6.3 New Point Diffuse Colors (presented only if Shading uses diffuse color)
		//          4.6.4 New Point Specular Colors (presented only if Shading uses specular color)
		//          4.6.5 New Point Texture Colors
		//
		
		//	1. Name
		IFXString stringName;
		m_pBitStreamX->ReadIFXStringX(stringName);

		//  2. Chain Index
		U32 uChainIndex;
		m_pBitStreamX->ReadU32X( uChainIndex );
		
		//	3. Base PointSet Description
		//		3.1	U32 m_uStartResolution;
		//		3.2 U32 m_uEndResolution;
		
		m_pBitStreamX->ReadU32X(m_uStartResolution);
		m_pBitStreamX->ReadU32X(m_uEndResolution);

	}
	IFXCHECKX( m_uEndResolution <= m_pMaxPointSetDesc->m_numPositions );

	m_uPositionsRead = 0;
    U32 i, i0 = m_uWIPi;
	for(i = m_uWIPi; (i < m_uEndResolution)&&(i<i0+m_uMaximumUpdatesPerCall)&&(m_currentIndex <m_uEndResolution ); i++,m_uWIPi++,m_currentIndex++ ) 
	{
		ReadPointUpdateX(m_currentIndex);

#ifdef DEBUG_VERBOSE
	DumpAuthorPointSet();
#endif 

	}

	if (m_uEndResolution == m_pAuthorPointSet->GetMaxPointSetDesc()->m_numPositions) {
		m_bDone = TRUE;
	} 

	if ( (m_uWIPi + m_uStartResolution) == m_uEndResolution) {
		IFXRELEASE(m_pBitStreamX);
		m_eWIPType = WIP_NONE;
	}
}


IFXRESULT IFXAPI_CALLTYPE CIFXPointSetDecoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		// Create the CIFXLoadManager component.
		CIFXPointSetDecoder *pComponent = new CIFXPointSetDecoder;

		if ( pComponent ) {
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			rc = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		} else {
			rc = IFX_E_OUT_OF_MEMORY;
		}
	} else {
		rc = IFX_E_INVALID_POINTER;
	}

	IFXRETURN(rc);
}

void CIFXPointSetDecoder::CalculatePredictedNormalAtSplitPos(U32 currPosInd, U32 splitPosInd, IFXVector3& vPredictedNormal)
{
	IFXASSERT(m_pPointSetDesc);

	vPredictedNormal = IFXVector3(0.f, 0.f, 0.f);

	if (m_pPointSetDesc->m_numPoints == 0 || m_pPointSetDesc->m_numPositions == 0 || currPosInd == 0)
		return;

	// get Points at split postion
	U32 Point;
	m_pAuthorPointSet->GetNormalPoint(splitPosInd, &Point); 
	m_pAuthorPointSet->GetNormal(Point,&vPredictedNormal);
}


void CIFXPointSetDecoder::CalculatePredictedColorAtSplitPos(BOOL bDiffuse, U32 currPosInd, U32 splitPosInd,
											IFXVector4& v4PredictedVertColor)
{
	IFXASSERT(m_pPointSetDesc);

	if (m_pPointSetDesc->m_numPoints == 0 || m_pPointSetDesc->m_numPositions == 0) 
		return;
	
	v4PredictedVertColor = IFXVector4(0.f, 0.f, 0.f, 0.f);
	if (currPosInd == 0) return; // no Points defined yet - so predicted is just zero normal

	if ( (m_pPointSetDesc->m_numDiffuseColors ==0 && bDiffuse) || 
		 (m_pPointSetDesc->m_numSpecularColors==0 && (!bDiffuse) )) return;

	// get Points at split postion
	U32 Point;
	if (bDiffuse) {
		m_pAuthorPointSet->GetDiffusePoint(splitPosInd, &Point); 
	} else {
		m_pAuthorPointSet->GetSpecularPoint(splitPosInd, &Point); 
	}

	if (bDiffuse) {
		m_pAuthorPointSet->GetDiffuseColor(Point, &v4PredictedVertColor);
	} else {
		m_pAuthorPointSet->GetSpecularColor(Point, &v4PredictedVertColor);
	}
	v4PredictedVertColor.Normalize3();
}


void CIFXPointSetDecoder::ReconstructColorDifference(BOOL bDiffuseColor, U8& u8Signs, 
							U32& udR, U32&udG, U32& udB, U32& udA, IFXVector4& v4VertColorDifference)
{
	F32 fInverseQuantVertColor;
	if (bDiffuseColor)
	{
		fInverseQuantVertColor = m_fInverseQuantDiffuseColor;
	}
	else
	{
		fInverseQuantVertColor = m_fInverseQuantSpecularColor;
	}

	if(u8Signs & 1) {
		v4VertColorDifference.R() = - fInverseQuantVertColor * udR;
	} else {
		v4VertColorDifference.R() = fInverseQuantVertColor * udR;
	}
	if(u8Signs & 2) {
		v4VertColorDifference.G() = - fInverseQuantVertColor * udG;
	} else {
		v4VertColorDifference.G() = fInverseQuantVertColor * udG;
	}
	if(u8Signs & 4) {
		v4VertColorDifference.B() = - fInverseQuantVertColor * udB;
	} else {
		v4VertColorDifference.B() = fInverseQuantVertColor * udB;
	}
	if(u8Signs & 8) {
		v4VertColorDifference.A() = - fInverseQuantVertColor * udA;
	} else {
		v4VertColorDifference.A() = fInverseQuantVertColor * udA;
	}
}


void CIFXPointSetDecoder::ReadPointUpdateX(U32 currPosInd)
{
	//	4. Point Description (= Verstex Update) [NumVertexUpdates]
    //      4.1 U32[rCurrentPositionCount] Split Positions index 
	//      4.2 New Poistion Info  (as quantized position difference)
    //          4.2.1 U8[cDiffPosSign]  position Difference Signs
    //          4.2.2 U32[cDiffPos] position difference |dX|
    //          4.2.3 U32[cDiffPos] position difference |dY|
    //          4.2.4 U32[cDiffPos] position difference |dZ|
	U32 splitPosInd = 0;
	U32 i;
	if(currPosInd > 1) {
		m_pBitStreamX->ReadCompressedU32X(uACStaticFull+currPosInd,splitPosInd);
	} else {
		m_pBitStreamX->ReadCompressedU32X(uACStaticFull+1,splitPosInd);
		IFXASSERT(splitPosInd==0);
	}
	if (currPosInd>0) {
		IFXASSERT(splitPosInd<currPosInd);
	}

	U8 u8Signs = 0;
	U32 udX = 0;
	U32 udY = 0;
	U32 udZ = 0;
	m_pBitStreamX->ReadCompressedU8X(uACContextPositionDiffSigns,u8Signs);
	m_pBitStreamX->ReadCompressedU32X(uACContextPositionDiffMagX,udX);
	m_pBitStreamX->ReadCompressedU32X(uACContextPositionDiffMagY,udY);
	m_pBitStreamX->ReadCompressedU32X(uACContextPositionDiffMagZ,udZ);

	//	calculate reconstructed position vector and store
	IFXVector3 vReconstructedPosition, vPredictedPosition(0.f, 0.f, 0.f);
	if (currPosInd>0) {
		m_pAuthorPointSet->GetPosition(splitPosInd, &vPredictedPosition);
	}

	ReconstructPosition(u8Signs, udX, udY, udZ, vReconstructedPosition); 
	vReconstructedPosition.Add(vPredictedPosition);

	// Save position to Author Point Set under reconstruction 

	m_pPointSetDesc->m_numPositions++;
	IFXCHECKX(m_pAuthorPointSet->SetPosition(currPosInd,&vReconstructedPosition));

	//	    4.3 U32[cNormlCnt]:New Normal Count
    //      4.4 New Normal Info [New Normal Count]
    //          4.4.1 U8[cDiffNormalSign]  position Difference Signs
    //          4.4.2 U32[cDiffNormal] normal difference X
    //          4.4.3 U32[cDiffNormal] normal difference Y
    //          4.4.4 U32[cDiffNormal] normal difference Z
	U32 uNumLocalNormals = 0;

	m_pBitStreamX->ReadCompressedU32X(uACContextNumLocalNormals, uNumLocalNormals);

	if (uNumLocalNormals>0) {
		IFXVector3 vPredictedNormal(0.f, 0.f, 0.f), vReconstructedNormal;
		CalculatePredictedNormalAtSplitPos(currPosInd, splitPosInd, vPredictedNormal);
		for (i=0; i< uNumLocalNormals; i++) {
			m_pBitStreamX->ReadCompressedU8X(uACContextNormalDiffSigns,u8Signs);
			m_pBitStreamX->ReadCompressedU32X(uACContextNormalDiffMagX,udX);
			m_pBitStreamX->ReadCompressedU32X(uACContextNormalDiffMagY,udY);
			m_pBitStreamX->ReadCompressedU32X(uACContextNormalDiffMagZ,udZ);
			ReconstructNormal(u8Signs, udX, udY, udZ, vReconstructedNormal); 
			vReconstructedNormal.Add(vPredictedNormal);

			// Save normal to Author Point Set under reconstruction 
			IFXCHECKX(m_pAuthorPointSet->SetNormal(m_pPointSetDesc->m_numNormals, &vReconstructedNormal));
			m_pPointSetDesc->m_numNormals++;
			IFXASSERT(m_pPointSetDesc->m_numNormals <= m_pAuthorPointSet->GetMaxPointSetDesc()->m_numNormals);
		}
	}

	//	    4.5 U32[cPointCnt]:New Point Count
    //      4.6 New Point Info [New Point Count]
	U32 uNumLocalPoints = 0;
	m_pBitStreamX->ReadCompressedU32X(uACContextNumNewFaces,uNumLocalPoints);
	
	if (uNumLocalPoints>0) {
    //          4.6.3 New Point Details [2]
	//          4.6.3.4 New Point Texture Colors
		
		U32 Point;
		U32	NormLocalIndex, DiffColorLocalIndex, SpecColorLocalIndex;
		U32 TexCoordLocalIndex[IFX_MAX_TEXUNITS];
		U32 uMaterialID, iTex;

		for (i=0; i< uNumLocalPoints; i++) {
			
			U32	PointInd = m_pPointSetDesc->m_numPoints;
			//          4.6.1 U32[cShading]  Shading ID 
			m_pBitStreamX->ReadCompressedU32X(uACContextPointShadingID,uMaterialID);
			IFXASSERT(uMaterialID<m_pPointSetDesc->m_numMaterials);
			IFXCHECKX(m_pAuthorPointSet->SetPointMaterial(PointInd, uMaterialID));

			// prepare for reading colors
			IFXAuthorMaterial currMaterial;
			IFXCHECKX(m_pAuthorPointSet->GetMaterial(uMaterialID, &currMaterial)); 

		    //          4.6.2 U32[cNormlIndx] normal local index
			m_pBitStreamX->ReadCompressedU32X(uACContextNormalLocalIndex,NormLocalIndex);

			U8 u8DupColorPointIndexFlag = 0;
			IFXVector4 v4PredictedVertColor(0.f, 0.f, 0.f, 0.f), v4VertColorDifference;
			U32 udR, udG, udB, udA;
	
			//          4.6.3.2 New Point Diffuse Colors (presented only if Shading uses diffuse color)
			if (currMaterial.m_uDiffuseColors) {

				m_pBitStreamX->ReadCompressedU8X(uACContextVertColorDupType,u8DupColorPointIndexFlag);
				
				if (u8DupColorPointIndexFlag==0) {
					m_pBitStreamX->ReadCompressedU8X(uACContextNormalDiffSigns,u8Signs);
					m_pBitStreamX->ReadCompressedU32X(uACContextVertexColorDiffMagR,udR);
					m_pBitStreamX->ReadCompressedU32X(uACContextVertexColorDiffMagG,udG);
					m_pBitStreamX->ReadCompressedU32X(uACContextVertexColorDiffMagB,udB);
					m_pBitStreamX->ReadCompressedU32X(uACContextVertexColorDiffMagA,udA);

					CalculatePredictedColorAtSplitPos(TRUE, currPosInd, splitPosInd, v4PredictedVertColor);
					ReconstructColorDifference(TRUE, u8Signs, udR, udG, udB, udA, v4VertColorDifference);
					v4PredictedVertColor.Add(v4VertColorDifference);
					IFXCHECKX(m_pAuthorPointSet->SetDiffuseColor(m_pPointSetDesc->m_numDiffuseColors, &v4PredictedVertColor));
					DiffColorLocalIndex = m_pPointSetDesc->m_numDiffuseColors;
					m_pPointSetDesc->m_numDiffuseColors++;

				} else {
					DiffColorLocalIndex = m_pPointSetDesc->m_numDiffuseColors-1;
				}
			} 
			//          4.6.3.3 New Point Specular Colors (presented only if Shading uses specular color)
			if (currMaterial.m_uSpecularColors) {

				m_pBitStreamX->ReadCompressedU8X(uACContextVertColorDupType,u8DupColorPointIndexFlag);
				
				if (u8DupColorPointIndexFlag==0) {
					m_pBitStreamX->ReadCompressedU8X(uACContextNormalDiffSigns,u8Signs);
					m_pBitStreamX->ReadCompressedU32X(uACContextVertexColorDiffMagR,udR);
					m_pBitStreamX->ReadCompressedU32X(uACContextVertexColorDiffMagG,udG);
					m_pBitStreamX->ReadCompressedU32X(uACContextVertexColorDiffMagB,udB);
					m_pBitStreamX->ReadCompressedU32X(uACContextVertexColorDiffMagA,udA);

					CalculatePredictedColorAtSplitPos(FALSE, currPosInd, splitPosInd, v4PredictedVertColor);
					ReconstructColorDifference(FALSE, u8Signs, udR, udG, udB, udA, v4VertColorDifference);
					v4PredictedVertColor.Add(v4VertColorDifference);
					IFXCHECKX(m_pAuthorPointSet->SetSpecularColor(m_pPointSetDesc->m_numSpecularColors, &v4PredictedVertColor));
					SpecColorLocalIndex = m_pPointSetDesc->m_numSpecularColors;
					m_pPointSetDesc->m_numSpecularColors++;

				} else {
					SpecColorLocalIndex = m_pPointSetDesc->m_numSpecularColors-1;
				}
			
			} 

			//          4.6.3.4 New Line Tex Coord (presented only if Shading has tex coord)
			if (m_pMaxPointSetDesc->m_numTexCoords) {
				IFXVector4	v4PredictedTexCoord( 0.f, 0.f, 0.f, 0.f);
				IFXVector4	v4VertTexCoordDifference;
			
				for (iTex = 0; iTex < currMaterial.m_uNumTextureLayers; iTex++ ) {
				
					m_pBitStreamX->ReadCompressedU8X(uACContextTexCoordDupType, u8DupColorPointIndexFlag);
					
					if (u8DupColorPointIndexFlag == 0) {
						m_pBitStreamX->ReadCompressedU8X(uACContextTexCoordDiffSigns,u8Signs);
						m_pBitStreamX->ReadCompressedU32X(uACContextTexCoordDiffMagU,udR);
						m_pBitStreamX->ReadCompressedU32X(uACContextTexCoordDiffMagV,udG);
						m_pBitStreamX->ReadCompressedU32X(uACContextTexCoordDiffMagS,udB);
						m_pBitStreamX->ReadCompressedU32X(uACContextTexCoordDiffMagT,udA);

						CalculatePredictedTexCoordAtSplitPos(iTex, currPosInd, splitPosInd, v4PredictedTexCoord);
						ReconstructTexCoord( u8Signs, udR, udG, udB, udA, v4VertTexCoordDifference);
						v4PredictedTexCoord.Add(v4VertTexCoordDifference);
						IFXCHECKX(m_pAuthorPointSet->SetTexCoord(m_pPointSetDesc->m_numTexCoords, 
																	&v4PredictedTexCoord));
						TexCoordLocalIndex[iTex] = m_pPointSetDesc->m_numSpecularColors;
						m_pPointSetDesc->m_numTexCoords++;

					} else {
						TexCoordLocalIndex[iTex] = m_pPointSetDesc->m_numTexCoords-1;
					}
				}
			}

			if (currMaterial.m_uNormals) {
				U32 oldNormCount = m_pPointSetDesc->m_numNormals - uNumLocalNormals;
				
				Point = oldNormCount+NormLocalIndex;
				IFXCHECKX(m_pAuthorPointSet->SetNormalPoint(PointInd, &Point)); // Save in Author PS
			}
			
			m_pAuthorPointSet->SetPositionPoint( m_pPointSetDesc->m_numPoints, &currPosInd );
			
			m_pPointSetDesc->m_numPoints++;
			
			if (currMaterial.m_uDiffuseColors) {
				IFXCHECKX(m_pAuthorPointSet->SetDiffusePoint(PointInd, &DiffColorLocalIndex)); // Save in Author PS
			}

			if (currMaterial.m_uSpecularColors) {
				IFXCHECKX(m_pAuthorPointSet->SetSpecularPoint(PointInd, &SpecColorLocalIndex)); // Save in Author PS
			}

			if (m_pPointSetDesc->m_numTexCoords) {
				for (iTex = 0; iTex < currMaterial.m_uNumTextureLayers; iTex++ ) {
					IFXCHECKX(m_pAuthorPointSet->SetTexPoint(iTex, PointInd, &(TexCoordLocalIndex[iTex]) )); // Save in Author PS
				}
			}
		}
	}
}

void CIFXPointSetDecoder::GetPointsAtPosition(U32& posInd, IFXArray<U32>& Points) 
{
	IFXASSERT(m_pAuthorPointSet);
	if (-1>=(int)posInd) {
		Points.Clear();
		return;
	}
	U32* posPoints;
	m_pAuthorPointSet->GetPositionPoints(&posPoints);
	U32 iPos;
	for ( iPos = 0; iPos<m_pAuthorPointSet->GetPointSetDesc()->m_numPoints; iPos++) {
		if (posInd == posPoints[iPos] ) {
			Points.CreateNewElement() = posPoints[iPos];
			break;
		}
	}
}


void CIFXPointSetDecoder::CalculatePredictedTexCoordAtSplitPos(U32 texInd, U32 currPosInd, U32 splitPosInd, 
															  IFXVector4& v4PredictedTexCoord)
{
	IFXASSERT(m_pPointSetDesc);
	v4PredictedTexCoord = IFXVector4(0.f, 0.f, 0.f, 0.f);

	if (m_pPointSetDesc->m_numPoints == 0 || 
		m_pPointSetDesc->m_numPositions == 0) return;
	

	if (currPosInd == 0) return; // no Points defined yet - so predicted is just zero normal

	if ( texInd > m_pPointSetDesc->m_numTexCoords ) return;

	// get Points at split postion
#ifdef _BEFORE_OPT_
	IFXArray <U32> Points;
	GetPointsAtPosition(splitPosInd, Points);

	U32 numPoints = Points.GetNumberElements();
	if (numPoints == 0) return;

	U32 iPoint, pointInd, Point;
	//	calculate predicted normal using those Points that alrady written
	for(iPoint = 0; iPoint<numPoints; iPoint++) 
	{
		pointInd = Points.GetElement(iPoint);
		IFXVector4 v4Temp;

		m_pAuthorPointSet->GetTexPoint(texInd, pointInd, &Point); 
		m_pAuthorPointSet->GetTexCoord(Point, &v4Temp); 
		v4PredictedTexCoord.Add(v4Temp);
	}
	v4PredictedTexCoord.Scale((F32)numPoints); 
#else 
	U32 Point;

	m_pAuthorPointSet->GetTexPoint(texInd, splitPosInd, &Point); 
	m_pAuthorPointSet->GetTexCoord(Point, &v4PredictedTexCoord); 
#endif 
}

void CIFXPointSetDecoder::ReconstructPosition(U8 u8Signs, U32 udX, U32 udY, U32 udZ, 
										      IFXVector3& vReconstructedPosition)
{
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
}

void CIFXPointSetDecoder::ReconstructNormal(U8 u8Signs, U32 udX, U32 udY, U32 udZ, 
											IFXVector3& vReconstructedNormal)
{
	if(u8Signs & 1) {
		vReconstructedNormal.X() = - m_fInverseQuantNormal * udX;
	} else {
		vReconstructedNormal.X() = m_fInverseQuantNormal * udX;
	}
	if(u8Signs & 2) {
		vReconstructedNormal.Y() = - m_fInverseQuantNormal * udY;
	} else {
		vReconstructedNormal.Y() = m_fInverseQuantNormal * udY;
	}
	if(u8Signs & 4) {
		vReconstructedNormal.Z() = - m_fInverseQuantNormal * udZ;
	} else {
		vReconstructedNormal.Z() = m_fInverseQuantNormal * udZ;
	}
}

void CIFXPointSetDecoder::ReconstructTexCoord(U8 u8Signs, U32 udR, U32 udG, U32 udB, U32 udA, 
		                        IFXVector4& v4VertTexCoord)
{
	if(u8Signs & 1) {
		v4VertTexCoord.R() = - m_fInverseQuantTexCoord * udR;
	} else {
		v4VertTexCoord.R() = m_fInverseQuantTexCoord * udR;
	}
	if(u8Signs & 2) {
		v4VertTexCoord.G() = - m_fInverseQuantTexCoord * udG;
	} else {
		v4VertTexCoord.G() = m_fInverseQuantTexCoord * udG;
	}
	if(u8Signs & 4) {
		v4VertTexCoord.B() = - m_fInverseQuantTexCoord * udB;
	} else {
		v4VertTexCoord.B() = m_fInverseQuantTexCoord * udB;
	}

	if(u8Signs & 4) {
		v4VertTexCoord.A() = - m_fInverseQuantTexCoord * udA;
	} else {
		v4VertTexCoord.A() = m_fInverseQuantTexCoord * udA;
	}
}

#ifdef DEBUG_VERBOSE

void DumpAuthorPointSet()
{
	IFXASSERT(m_pAuthorPointSet);

	IFXAuthorPointSetDesc* pPSDesc = m_pAuthorPointSet->GetPointSetDesc();
	IFXTRACE_GENERIC(L"[Author Point Set Dump] AuthorPointSet at  %x\n", m_pAuthorPointSet);
	IFXTRACE_GENERIC(L"Positions: %i, Points: %i, Normals: %i\n",
		pPSDesc->m_numPositions, pPSDesc->m_numPoints, pPSDesc->m_numNormals);

	IFXTRACE_GENERIC(L" Diffuse colors: %i, Specular Colors: %i, Materials: %i\n",
		pPSDesc->m_numDiffuseColors, pPSDesc->m_numSpecularColors, pPSDesc->m_numMaterials);

	IFXVector3 pnt;
	U32 point;
	U32 j;
  	for (j=0; j<pPSDesc->m_numPoints; j++) {
		IFXTRACE_GENERIC(L" Point index: %i\n", j);
		if (pPSDesc->m_numPositions>0) {
			IFXCHECKX(m_pAuthorPointSet->GetPositionPoint(j, &point));
		}
		IFXTRACE_GENERIC(L"\t Point position:  %i\n",	point);
		if (pPSDesc->m_numNormals>0) {
			IFXCHECKX(m_pAuthorPointSet->GetNormalPoint(j, &point));
			IFXTRACE_GENERIC(L"\t Point normal:   %i \n",	point);

			m_pAuthorPointSet->GetPosition(point, &pnt);
			IFXTRACE_GENERIC(L"\t Normal vector :%f	%f	%f\n", pnt.X(), pnt.Y(), pnt.Z() );

		}
		if (pPSDesc->m_numDiffuseColors>0) {
			IFXCHECKX(m_pAuthorPointSet->GetDiffusePoint(j, &point));
			IFXTRACE_GENERIC(L"\t Line Diffuse colors:   %i  \n",	point);
		}
		if (pPSDesc->m_numSpecularColors>0) {
			IFXCHECKX(m_pAuthorPointSet->GetSpecularPoint(j, &point));
			IFXTRACE_GENERIC(L"\t Line Specular colors:  %i  \n",	point);
		}
		if (pPSDesc->m_numMaterials>0) {
			U32 uMaterial;
			IFXCHECKX(m_pAuthorPointSet->GetPointMaterial(j, &uMaterial));
			IFXTRACE_GENERIC(L"\t Point material:  %i  \n",	uMaterial);
		}
	}

	IFXVector3 vert;
  	for (j=0; j<pPSDesc->m_numPositions; j++) {
		IFXTRACE_GENERIC(L" Position index: %i", j);
		
		IFXCHECKX(m_pAuthorPointSet->GetPosition(j, &vert));
		IFXTRACE_GENERIC(L"\t   %f  %f  %f\n",	vert.X(), vert.Y(), vert.Z());
	}
  	for (j=0; j<pPSDesc->m_numNormals; j++) {
		IFXTRACE_GENERIC(L" Normal index: %i", j);
		
		IFXCHECKX(m_pAuthorPointSet->GetNormal(j, &vert));
		IFXTRACE_GENERIC(L"\t   %f  %f  %f\n",	vert.X(), vert.Y(), vert.Z());
	}
}

#endif
