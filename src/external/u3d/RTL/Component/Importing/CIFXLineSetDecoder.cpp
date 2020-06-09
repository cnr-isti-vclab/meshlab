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
	@file	CIFXLineSetDecoder.cpp

			The CIFXLineSetDecoder is used by the CIFXLoadManager to load
			Line Set primitive generator into the scene graph. CIFXLineSetDecoder
			exposes an IFXDecoder interface to the CIFXLoadManager for this
			purpose.
*/

#include "CIFXLineSetDecoder.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"
#include "IFXAuthor.h"
#include "IFXSkeleton.h"
#include "IFXACContext.h"
#include "IFXModel.h"
#include "IFXBonesManager.h"

//#define DEBUG_VERBOSE 

const U32 CIFXLineSetDecoder::m_uMaximumUpdatesPerCall = 64;

CIFXLineSetDecoder::CIFXLineSetDecoder() :
	IFXDEFINEMEMBER(m_pBitStreamX),
	IFXDEFINEMEMBER(m_pLineSetResource),
	IFXDEFINEMEMBER(m_pAuthorLineSet),
	IFXDEFINEMEMBER(m_pLineSetAnalyzer)
{
	m_uRefCount = 0;
	m_uLoadId = 0;
	m_bDone = TRUE;
	m_eWIPType = WIP_NONE;

	m_uReservedLineSetParameter0 = 0;

	m_fMinResolution = 1.0f;
	m_fMaxResolution = 1.0f;

	m_fInverseQuantPosition = 1.0f;
	m_fInverseQuantNormal	= 1.0f;
	m_fInverseQuantTexCoord = 1.0f;

	m_uReservedLineSetParameter1	= 0;
	m_uReservedLineSetParameter2	= 0;
	m_uReservedLineSetParameter3	= 0;

	m_pMaxLineSetDesc			= NULL;
	m_pLineSetDesc				= NULL;
	m_uPositionsRead			= (U32)-1;

	m_uStartResolution =0;
	m_uEndResolution   =0;
	m_uCurrentIndex     =0;

	m_unitScale = 1.0f;
}


CIFXLineSetDecoder::~CIFXLineSetDecoder()
{
}

U32 CIFXLineSetDecoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXLineSetDecoder::Release()
{
	if ( 1 == m_uRefCount ) {
		delete this;
		return 0;
	}
	return --m_uRefCount;
}

IFXRESULT CIFXLineSetDecoder::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
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
void CIFXLineSetDecoder::InitializeX(const IFXLoadConfig &lc)
{
	if( lc.m_units > 0.0f )
		m_unitScale = lc.m_units;
	IFXModifierBaseDecoder::InitializeX(lc);																			
}

// Provide next block of data to the loader
void CIFXLineSetDecoder::PutNextBlockX( IFXDataBlockX &rDataBlockX )
{
	if(NULL == m_pDataBlockQueueX) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	m_pDataBlockQueueX->AppendBlockX( rDataBlockX );

	if (NULL == m_pObject) 
	{
		U32 uBlockType;
		rDataBlockX.GetBlockTypeX(uBlockType);
		CreateObjectX(rDataBlockX, CID_IFXAuthorLineSetResource);

		IFXRELEASE(m_pLineSetResource);
		IFXCHECKX(m_pObject->QueryInterface(IID_IFXAuthorLineSetResource,(void**)&m_pLineSetResource));

#ifdef KEEP_DATA_BLOCKS
		IFXCHECKX(m_pLineSetResource->BuildDataBlockQueue());
#endif
	}

	m_bDone = FALSE;

#ifdef KEEP_DATA_BLOCKS
	// Add the data block to the generator. This is used by the generator as a
	// cached copy of itself in encoded form thus saving potential encoding at
	// save or export time.
	IFXDECLARELOCAL(IFXDataBlockQueueX,pGeneratorDataBlockQueueX);
	m_pLineSetResource->GetDataBlockQueueX( pGeneratorDataBlockQueueX );
	if ( pGeneratorDataBlockQueueX )
		pGeneratorDataBlockQueueX->AppendBlockX( rDataBlockX );
#endif

}

void CIFXLineSetDecoder::TransferX(IFXRESULT &rWarningPartialTransfer)
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
		case BlockType_GeneratorLineSetU3D :
			{
				// set metadata
				IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
				IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
				pDataBlockX->QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
				m_pLineSetResource->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
				pObjectMD->AppendX(pBlockMD);
			}
			m_eWIPType = WIP_DECLARATION_U3D;
			break;
        case BlockType_GeneratorLineSetContinuationU3D :
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
		ProcessAuthorLineSetDeclarationBlockX();
		IFXRELEASE(m_pBitStreamX);
		break;
    case WIP_STATIC_U3D:
		break;
    case WIP_PROGRESSIVE_U3D:
		ProcessAuthorLineSetContinuationBlockX();
		break;
    case WIP_NONE:
		break;
	default:
		break;
	}

	// Give the reconstructed author mesh to the author CLOD resource
	if (wip != WIP_NONE) {
		IFXCHECKX(m_pLineSetResource->SetAuthorLineSet(m_pAuthorLineSet));
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


void CIFXLineSetDecoder::ProcessAuthorLineSetDeclarationBlockX()
{

	IFXASSERT(m_pBitStreamX);
#ifdef _DEBUG
	IFXRESULT rc = IFX_OK;
#endif

	
    //
	// Read the block info for AuthorLineSetDeclaration:
    //
	//	1. IFXString Name
    //  2. U32 Chain Index (always 0 for this block)
	//	3. Max LineSet Description
	//		3.1	U32 NumLines;
	//		3.2 U32 NumPositions;
	//		3.3 U32 NumNormals;	
	//		3.4 U32 NumDiffuseColors;
	//		3.5 U32 NumSpecularColors;
	//		3.6 U32 NumTexCoords;
	//		3.7 U32 NumShaders;
	//		3.8 Shaders Descriptions [NumShaders]
	//			3.8.1	U32 Shading Attributes (collection of flags: BOOL m_uDiffuseColors; BOOL m_uSpecularColors;)
	//			3.8.2	U32 NumTextureLayers;
	//			3.8.3	U32 m_uNumTextureLayers[NumTextureLayers]; (IFXMAX_TEX_COORD_LAYERS == 8)
	//			3.8.4	U32 OriginalShadingID;  // the mesh scrubber may have removed unused materials an repacked them.
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

	//	3. Max Line Description
	IFXAuthorLineSetDesc sMaxLineSetDesc;
	IFXAuthorLineSetDesc* pMaxLineSetDesc = &sMaxLineSetDesc;

	if( m_unitScale > 0.0f && m_unitScale != 1.0f )
	{
		IFXMatrix4x4 matrix = m_pLineSetResource->GetTransform();
		IFXVector3 scale( (F32)m_unitScale, (F32)m_unitScale, (F32)m_unitScale );
		matrix.Scale( scale );
		m_pLineSetResource->SetTransform( matrix );
	}

	IFXASSERT(pMaxLineSetDesc);
	//      3.  U32 Line Set Reserved
	m_pBitStreamX->ReadU32X(m_uReservedLineSetParameter0);
	//		3.1	U32 NumLines;
	m_pBitStreamX->ReadU32X(pMaxLineSetDesc->m_numLines);
	//		3.2 U32 NumPositions;
	m_pBitStreamX->ReadU32X(pMaxLineSetDesc->m_numPositions);
	//		3.3 U32 NumNormals;
	m_pBitStreamX->ReadU32X(pMaxLineSetDesc->m_numNormals);
	//		3.4 U32 NumDiffuseColors;
	m_pBitStreamX->ReadU32X(pMaxLineSetDesc->m_numDiffuseColors);
	//		3.5 U32 NumSpecularColors;
	m_pBitStreamX->ReadU32X(pMaxLineSetDesc->m_numSpecularColors);
	//		3.6 U32 NumTexCoords;
	m_pBitStreamX->ReadU32X(pMaxLineSetDesc->m_numTexCoords);
	//		3.7 U32 NumShader;
	m_pBitStreamX->ReadU32X(pMaxLineSetDesc->m_numMaterials);


	// Create the reconstructed author Lines and allocate
	{
		IFXAuthorLineSetDesc sZeroLinSetDesc;
		sZeroLinSetDesc.m_numLines  		= 0;
		sZeroLinSetDesc.m_numPositions		= 0;
		sZeroLinSetDesc.m_numNormals		= 0;
		sZeroLinSetDesc.m_numDiffuseColors	= 0;
		sZeroLinSetDesc.m_numSpecularColors	= 0;
		sZeroLinSetDesc.m_numTexCoords		= 0;
		sZeroLinSetDesc.m_numMaterials		= pMaxLineSetDesc->m_numMaterials;		

		IFXRELEASE(m_pAuthorLineSet);
		IFXCHECKX(IFXCreateComponent(CID_IFXAuthorLineSet,IID_IFXAuthorLineSet,(void**)&m_pAuthorLineSet));
		IFXASSERT(m_pAuthorLineSet);
		//Streatch arrays to accomodate full info - this is temp - need to find out how to do more effectively
		if (pMaxLineSetDesc->m_numNormals && pMaxLineSetDesc->m_numNormals < 2*pMaxLineSetDesc->m_numLines) 
			pMaxLineSetDesc->m_numNormals = 2*pMaxLineSetDesc->m_numLines;
		if (pMaxLineSetDesc->m_numDiffuseColors && pMaxLineSetDesc->m_numDiffuseColors < 2*pMaxLineSetDesc->m_numLines) 
			pMaxLineSetDesc->m_numDiffuseColors = 2*pMaxLineSetDesc->m_numLines;
		if (pMaxLineSetDesc->m_numSpecularColors && pMaxLineSetDesc->m_numSpecularColors < 2*pMaxLineSetDesc->m_numLines) 
			pMaxLineSetDesc->m_numSpecularColors = 2*pMaxLineSetDesc->m_numLines;

		if (pMaxLineSetDesc->m_numTexCoords && pMaxLineSetDesc->m_numTexCoords < 2*pMaxLineSetDesc->m_numLines) 
			pMaxLineSetDesc->m_numTexCoords = 2*pMaxLineSetDesc->m_numLines;
		
		IFXCHECKX(m_pAuthorLineSet->Allocate(&sMaxLineSetDesc));

#ifdef _DEBUG
		rc =
#endif
		m_pAuthorLineSet->SetLineSetDesc(&sZeroLinSetDesc);
		IFXASSERT(rc == IFX_OK);
	}

	// 3.8 Shaders Descriptions [NumShaders]
	{
		IFXAuthorMaterial* pMaterial = NULL;
		IFXCHECKX(m_pAuthorLineSet->GetMaterials(&pMaterial));

		U32 i = 0;
		for( i=0; i < pMaxLineSetDesc->m_numMaterials; i++) 
		{	
			//	3.8.1	Shading Attributes (BOOL m_uDiffuseColors; BOOL m_uSpecularColors;)
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

			if(pMaxLineSetDesc->m_numNormals)
				pMaterial[i].m_uNormals = TRUE;
			else
				pMaterial[i].m_uNormals = FALSE;
			//	3.8.2	U32 NumTextureLayers;
			m_pBitStreamX->ReadU32X(pMaterial[i].m_uNumTextureLayers );
			//	3.8.3	U32 m_uNumTextureLayers[NumTextureLayers]; (IFXMAX_TEX_COORD_LAYERS == 8)
			IFXASSERT(IFX_MAX_TEXUNITS >= pMaterial[i].m_uNumTextureLayers);
			
			U32 j = 0;
			for(j = 0; j < pMaterial[i].m_uNumTextureLayers; j++) {
				m_pBitStreamX->ReadU32X(pMaterial[i].m_uTexCoordDimensions[j]);
			}
			//	3.8.4	U32 OriginalShadingID;  // the mesh scrubber may have removed unused materials an repacked them.
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
	m_pBitStreamX->ReadU32X(m_uReservedLineSetParameter1);
	m_pBitStreamX->ReadU32X(m_uReservedLineSetParameter2);
	m_pBitStreamX->ReadU32X(m_uReservedLineSetParameter3);
    
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
		if ( (0 != uBoneCount) ) 
		{
			IFXSkeleton *pSkeleton = NULL;

			pBoneInfo = new IFXBoneInfo[uBoneCount];
			if ( NULL == pBoneInfo ) 
			{
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

			m_pLineSetResource->SetBones( pSkeleton );

            IFXBonesManager *pBonesManager = pSkeleton->GetBonesManagerNR();
            IFXCHECKX(NULL != pBonesManager);

            IFXRESULT result = pBonesManager->UpdateReferencePositions();
            IFXCHECKX(IFX_OK == result);

			IFXRELEASE( pSkeleton );
			delete [] pBoneInfo;
		}

	m_eWIPType = WIP_NONE;	
}


IFXRESULT IFXAPI_CALLTYPE CIFXLineSetDecoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		// Create the CIFXLoadManager component.
		CIFXLineSetDecoder *pComponent = new CIFXLineSetDecoder;

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

void CIFXLineSetDecoder::ProcessAuthorLineSetContinuationBlockX()
{
	// grab the IFXLineSet handle to the current modifier component:
	IFXASSERT(m_pBitStreamX);
	IFXASSERT(m_pAuthorLineSet);

	if (m_uWIPi == 0) {
		// get the lineset object description
		m_pMaxLineSetDesc = m_pAuthorLineSet->GetMaxLineSetDesc();
		m_pLineSetDesc    = m_pAuthorLineSet->GetLineSetDesc();

		// Create the reconstructed Author Line Set Analyzer object 
		// that will help decode data from continuation block
		{
			IFXRELEASE(m_pLineSetAnalyzer);
			IFXCHECKX(IFXCreateComponent(CID_IFXAuthorLineSetAnalyzer,IID_IFXAuthorLineSetAnalyzer,
							(void**)&m_pLineSetAnalyzer));
			IFXASSERT(m_pLineSetAnalyzer);
			m_pLineSetAnalyzer->Initialize(m_pAuthorLineSet);
		}


		
		//
		// Read the Continutaion block info for AuthorLineSet
		//
		//	1. Name
		//  2. Chain Index (always 0 for this block)
		//	3. Resolution Range 
		//		3.1	U32 StartResultion; - have to be 0 
		//		3.2 U32 EndResulution; - have to be lineSetDesc->numPositions
		//	4. Line Description (= Verstex Update) [NumVertexUpdates]
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
		//	    4.5 U32[cLineCnt]:New Line Count
		//      4.6 New Line Info [New Line Count]
		//          4.6.1 U32[cShading]  Shading ID 
		//          4.6.2 U32[rCurrentPositionCount] first postion index
		//          4.6.3 New Line Details [2]
		//          4.6.3.1 U32[cNormlIndx] normal local index
		//          4.6.3.2 New Line Diffuse Colors (presented only if Shading uses diffuse color)
		//          4.6.3.3 New Line Specular Colors (presented only if Shading uses specular color)
		//          4.6.3.4 New Line Texture Colors
		//
		

		//	1. Name
		IFXString stringName;
		m_pBitStreamX->ReadIFXStringX(stringName);

		//  2. Chain Index
		U32 uChainIndex;
		m_pBitStreamX->ReadU32X( uChainIndex );
		
		//	3. Base Mesh Description
		//		3.1	U32 m_StartResultion;
		//		3.2 U32 m_EndResultion;
		m_pBitStreamX->ReadU32X(m_uStartResolution);
		m_pBitStreamX->ReadU32X(m_uEndResolution);
	}

	IFXCHECKX(m_uEndResolution<=m_pMaxLineSetDesc->m_numPositions);

	m_uPositionsRead = 0;
#ifdef DEBUG_VERBOSE
	static U32 indBlock = 0;
	IFXTRACE_GENERIC(L"[LineSet Decode Cont Block] %i \n", indBlock);
#endif 

	U32 i, i0 = m_uWIPi;
	for(i = m_uWIPi; (i < m_uEndResolution)&&(i<i0+m_uMaximumUpdatesPerCall)&&(m_uCurrentIndex <m_uEndResolution ); i++,m_uWIPi++,m_uCurrentIndex++ ) 
	{
		ReadLineUpdateX(m_uCurrentIndex);

		m_pLineSetAnalyzer->Update();
		NormalizeNormals();

#ifdef DEBUG_VERBOSE
	IFXTRACE_GENERIC(L"[LineSet DECODING UPDATE] %i \n", currPosInd);
	m_pLineSetAnalyzer->DumpAuthorLineSet();
	indBlock++;
#endif 
	}

	if (m_uEndResolution == m_pAuthorLineSet->GetMaxLineSetDesc()->m_numPositions) {
		m_bDone = TRUE;
	} 
	
	if ( (m_uWIPi + m_uStartResolution) == m_uEndResolution) {
		IFXRELEASE(m_pBitStreamX);
		m_eWIPType = WIP_NONE;
	}
}


void CIFXLineSetDecoder::CalculatePredictedNormalAtSplitPosX(U32 currPosInd, U32 splitPosInd, IFXVector3& vPredictedNormal)
{
	IFXASSERT(m_pLineSetDesc);
	IFXASSERT(m_pLineSetAnalyzer);
	IFXASSERT(m_pLineSetAnalyzer->IsInitialized());

	if (m_pLineSetDesc->m_numLines == 0 || m_pLineSetDesc->m_numPositions == 0) return;
	
	vPredictedNormal = IFXVector3(0.f, 0.f, 0.f);
	if (currPosInd == 0) return; // no lines deifed yet - so predicted is just zero normal

	// get lines for split postion
	IFXArray <U32> lines, endPositions, newLines, newEnds;
	lines.Clear(); endPositions.Clear(); newLines.Clear(); newEnds.Clear();

	IFXCHECKX(IFX_OK== m_pLineSetAnalyzer->GetLines(splitPosInd, lines, endPositions));
	U32 numLines = lines.GetNumberElements();
	IFXASSERT(numLines == endPositions.GetNumberElements());

	if (numLines==0) return; // just stand alone position = no lines connected w/ it

	//	mark lines and their ends that are going to be used at normal predictionin this update
	U32 iLine, linesForPredict=0;
	for(iLine = 0; iLine<numLines; iLine++) 
	{
		if (endPositions.GetElement(iLine)<currPosInd) {
			newLines.CreateNewElement()=lines.GetElement(iLine);
			newEnds.CreateNewElement()=endPositions.GetElement(iLine);
			linesForPredict++;
		}
	}
	if (linesForPredict==0) return; // no lines connected to split wrtitten so far -> use zero normal as prdiction

	IFXASSERT(linesForPredict>0);

	//	calculate predicted normal using those lines that alrady written
	for(iLine = 0; iLine<linesForPredict; iLine++) 
	{
		IFXU32Line line;
		IFXVector3 norm;
		U32 lineInd = newLines.GetElement(iLine);

		m_pAuthorLineSet->GetPositionLine(lineInd, &line); 
		U32 endA = line.VertexA();
		U32 endB = line.VertexB();
		m_pAuthorLineSet->GetNormalLine(lineInd, &line); 
		IFXASSERT(endA==splitPosInd || endB==splitPosInd);
		// "line" var aleady contains normal indices
		if (endA == splitPosInd) {
			endA = line.VertexA();
			m_pAuthorLineSet->GetNormal(endA,&norm);
		} else { // endB == splitPosInd
			endB = line.VertexB();
			m_pAuthorLineSet->GetNormal(endB,&norm);
		}
		vPredictedNormal.Add(norm);
	}
	vPredictedNormal.Normalize();
}


void CIFXLineSetDecoder::CalculatePredictedColorAtSplitPosX(BOOL bDiffuse, U32 currPosInd, U32 splitPosInd,
											IFXVector4& v4PredictedVertColor)
{
	IFXASSERT(m_pLineSetDesc);
	IFXASSERT(m_pLineSetAnalyzer);
	IFXASSERT(m_pLineSetAnalyzer->IsInitialized());
	v4PredictedVertColor = IFXVector4(0.f, 0.f, 0.f, 0.f);

	if (m_pLineSetDesc->m_numLines == 0 || m_pLineSetDesc->m_numPositions == 0) return;

	if (currPosInd == 0) return; // no lines defined yet - so predicted is just zero normal

	if ( (m_pLineSetDesc->m_numDiffuseColors ==0 && bDiffuse) || 
		 (m_pLineSetDesc->m_numSpecularColors==0 && (!bDiffuse) )) return;

	// get lines for split postion
	IFXArray <U32> lines, endPositions, newLines, newEnds;
	lines.Clear(); endPositions.Clear(); newLines.Clear(); newEnds.Clear();

	IFXCHECKX(IFX_OK== m_pLineSetAnalyzer->GetLines(splitPosInd, lines, endPositions));
	U32 numLines = lines.GetNumberElements();
	IFXASSERT(numLines == endPositions.GetNumberElements());

	if (numLines==0) return; // just stand alone position = no lines connected w/ it

	//	mark lines and their ends that are going to be used at color predictionin this update
	U32 iLine, linesForPredict=0;
	for(iLine = 0; iLine<numLines; iLine++) 
	{
		if (endPositions.GetElement(iLine)<currPosInd) {
			newLines.CreateNewElement()=lines.GetElement(iLine);
			newEnds.CreateNewElement()=endPositions.GetElement(iLine);
			linesForPredict++;
		}
	}
	if (linesForPredict==0) return; // no lines connected to split written so far -> use zero normal as prediction

	IFXASSERT(linesForPredict>0);

	//	calculate predicted color using those lines that alredy written
	for(iLine = 0; iLine<linesForPredict; iLine++) 
	{
		IFXU32Line line;
		IFXVector4 color;
		U32 lineInd = newLines.GetElement(iLine);

		m_pAuthorLineSet->GetPositionLine(lineInd, &line); 
		U32 endA = line.VertexA();
#ifdef _DEBUG
		U32 endB = line.VertexB();
#endif
		U32 splitInd;
		if (bDiffuse) {
			m_pAuthorLineSet->GetDiffuseLine(lineInd, &line); 
		} else {
			m_pAuthorLineSet->GetSpecularLine(lineInd, &line); 
		}
		IFXASSERT(endA==splitPosInd || endB==splitPosInd);
		// in line aleady normal indices
		if (endA == splitPosInd) {
			splitInd = line.VertexA();
		} else { // endB == splitPosInd
			splitInd = line.VertexB();
		}

		if (bDiffuse) {
			m_pAuthorLineSet->GetDiffuseColor(splitInd, &color); 
		} else {
			m_pAuthorLineSet->GetSpecularColor(splitInd, &color); 
		}
		v4PredictedVertColor.Add(color);
	}
	v4PredictedVertColor.Scale((F32)linesForPredict);
}


void CIFXLineSetDecoder::CalculatePredictedTexCoordAtSplitPosX(U32 texInd, U32 currPosInd, U32 splitPosInd, 
															  IFXVector4& v4PredictedTexCoord)
{
	IFXASSERT(m_pLineSetDesc);
	IFXASSERT(m_pLineSetAnalyzer);
	IFXASSERT(m_pLineSetAnalyzer->IsInitialized());
	v4PredictedTexCoord = IFXVector4(0.f, 0.f, 0.f, 0.f);

	if (m_pLineSetDesc->m_numLines == 0 || 
		m_pLineSetDesc->m_numPositions == 0) return;
	

	if (currPosInd == 0) return; // no lines defined yet - so predicted is just zero normal

	if ( texInd >= m_pLineSetDesc->m_numTexCoords ) return;

	// get lines for split postion
	IFXArray <U32> lines, endPositions, newLines, newEnds;
	lines.Clear(); endPositions.Clear(); newLines.Clear(); newEnds.Clear();

	IFXCHECKX(IFX_OK== m_pLineSetAnalyzer->GetLines(splitPosInd, lines, endPositions));
	U32 numLines = lines.GetNumberElements();
	IFXASSERT(numLines == endPositions.GetNumberElements());

	if (numLines==0) return; // just stand alone position = no lines connected w/ it

	//	mark lines and their ends that are going to be used at color predictionin this update
	U32 iLine, linesForPredict=0;
	for(iLine = 0; iLine<numLines; iLine++) 
	{
		if (endPositions.GetElement(iLine)<currPosInd) {
			newLines.CreateNewElement()=lines.GetElement(iLine);
			newEnds.CreateNewElement()=endPositions.GetElement(iLine);
			linesForPredict++;
		}
	}
	if (linesForPredict==0) return; // no lines connected to split written so far -> use zero normal as prediction

	IFXASSERT(linesForPredict>0);

	//	calculate predicted tex coord using those lines that alrady written
	for(iLine = 0; iLine<linesForPredict; iLine++) 
	{
		IFXU32Line line;
		U32 lineInd = newLines.GetElement(iLine);
		IFXVector4 v4Temp;

		m_pAuthorLineSet->GetPositionLine(lineInd, &line); 
		U32 endA = line.VertexA();
#ifdef _DEBUG
		U32 endB = line.VertexB();
#endif
		U32 splitInd;
		m_pAuthorLineSet->GetTexLine(texInd, lineInd, &line); 

		IFXASSERT(endA==splitPosInd || endB==splitPosInd);
		// in line aleady normal indices
		if (endA == splitPosInd) {
			splitInd = line.VertexA();
		} else { // endB == splitPosInd
			splitInd = line.VertexB();
		}

		m_pAuthorLineSet->GetTexCoord(splitInd, &v4Temp); 

		v4PredictedTexCoord.Add(v4Temp);
	}
	v4PredictedTexCoord.Scale((F32)linesForPredict);
}


void CIFXLineSetDecoder::ReconstructColorDifference(BOOL bDiffuseColor, U8& u8Signs, 
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


void CIFXLineSetDecoder::ReadLineUpdateX(U32 currPosInd)
{
	//	4. Line Description (= Verstex Update) [NumVertexUpdates]
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
		m_pAuthorLineSet->GetPosition(splitPosInd, &vPredictedPosition);
	}

	ReconstructPosition(u8Signs, udX, udY, udZ, vReconstructedPosition); 
	vReconstructedPosition.Add(vPredictedPosition);

	// Save position to Author Line Set under reconstruction 
	//
	IFXASSERT(currPosInd==m_pLineSetDesc->m_numPositions);
	m_pLineSetDesc->m_numPositions++;
	IFXCHECKX(m_pAuthorLineSet->SetPosition(currPosInd,&vReconstructedPosition));

	//	    4.3 U32[cNormlCnt]:New Normal Count
    //      4.4 New Normal Info [New Normal Count]
    //          4.4.1 U8[cDiffNormalSign]  position Difference Signs
    //          4.4.2 U32[cDiffNormal] normal difference X
    //          4.4.3 U32[cDiffNormal] normal difference Y
    //          4.4.4 U32[cDiffNormal] normal difference Z
	U32 uNumLocalNormals = 0;

	m_pBitStreamX->ReadCompressedU32X(uACContextNumLocalNormals,uNumLocalNormals);

	if (uNumLocalNormals>0) {
		IFXVector3 vPredictedNormal(0.f, 0.f, 0.f), vReconstructedNormal;
		CalculatePredictedNormalAtSplitPosX(currPosInd, splitPosInd, vPredictedNormal);
		for (i=0; i< uNumLocalNormals; i++) {
			m_pBitStreamX->ReadCompressedU8X(uACContextNormalDiffSigns,u8Signs);
			m_pBitStreamX->ReadCompressedU32X(uACContextNormalDiffMagX,udX);
			m_pBitStreamX->ReadCompressedU32X(uACContextNormalDiffMagY,udY);
			m_pBitStreamX->ReadCompressedU32X(uACContextNormalDiffMagZ,udZ);
			ReconstructNormal(u8Signs, udX, udY, udZ, vReconstructedNormal); 
			vReconstructedNormal.Add(vPredictedNormal);

			// Save normal to Author Line Set under reconstruction 
			//
			IFXCHECKX(m_pAuthorLineSet->SetNormal(m_pLineSetDesc->m_numNormals, &vReconstructedNormal));
			m_pLineSetDesc->m_numNormals++;
			IFXASSERT(m_pLineSetDesc->m_numNormals <= m_pAuthorLineSet->GetMaxLineSetDesc()->m_numNormals);
		}
	}

	//	    4.5 U32[cLineCnt]:New Line Count
    //      4.6 New Line Info [New Line Count]
	U32 uNumLocalLines = 0;
	m_pBitStreamX->ReadCompressedU32X(uACContextNumNewFaces,uNumLocalLines);
	
	if (uNumLocalLines>0) {
    //          4.6.3 New Line Details [2]
	//          4.6.3.4 New Line Texture Colors
		
		IFXU32Line line;
		U32	endInd, NormLocalIndex[2], DiffColorLocalIndex[2], SpecColorLocalIndex[2];
		U32 TexCoordLocalIndex[IFX_MAX_TEXUNITS][2];
		U32 uMaterialID, iTex;

		for (i=0; i< uNumLocalLines; i++) {
			U32	lineInd = m_pLineSetDesc->m_numLines;
			//          4.6.1 U32[cShading]  Shading ID 
			m_pBitStreamX->ReadCompressedU32X(uACContextLineShadingID,uMaterialID);
			IFXASSERT(uMaterialID<m_pLineSetDesc->m_numMaterials);
			IFXCHECKX(m_pAuthorLineSet->SetLineMaterial(lineInd, uMaterialID));

			//          4.6.2 U32[rCurrentPositionCount] first postion index
			m_pBitStreamX->ReadCompressedU32X(uACStaticFull+currPosInd,endInd);
			IFXASSERT(endInd<( (m_pLineSetDesc->m_numPositions) - 1) );
			line.SetA(endInd);
			line.SetB(currPosInd);
			IFXCHECKX(m_pAuthorLineSet->SetPositionLine(lineInd, &line)); // Save in Author LS
			
			// prepare for reading colors
			IFXAuthorMaterial currMaterial;
			IFXCHECKX(m_pAuthorLineSet->GetMaterial(uMaterialID, &currMaterial)); 

			U32 iPos;
			for ( iPos=0; iPos<2; iPos++){

			    //          4.6.3.1 U32[cNormlIndx] normal local index
				m_pBitStreamX->ReadCompressedU32X(uACContextNormalLocalIndex,NormLocalIndex[iPos]);

				U8 u8DupAttribLineIndexFlag = 0;
				IFXVector4 v4PredictedVertColor(0.f, 0.f, 0.f, 0.f), v4VertColorDifference;//, v4VertColor;
				U32 udR, udG, udB, udA;
	
				//          4.6.3.2 New Line Diffuse Colors (presented only if Shading uses diffuse color)

				if (currMaterial.m_uDiffuseColors) {

					m_pBitStreamX->ReadCompressedU8X(uACContextVertColorDupType,u8DupAttribLineIndexFlag);
					
					if (u8DupAttribLineIndexFlag==0) {
						m_pBitStreamX->ReadCompressedU8X(uACContextNormalDiffSigns,u8Signs);
						m_pBitStreamX->ReadCompressedU32X(uACContextVertexColorDiffMagR,udR);
						m_pBitStreamX->ReadCompressedU32X(uACContextVertexColorDiffMagG,udG);
						m_pBitStreamX->ReadCompressedU32X(uACContextVertexColorDiffMagB,udB);
						m_pBitStreamX->ReadCompressedU32X(uACContextVertexColorDiffMagA,udA);

						CalculatePredictedColorAtSplitPosX(TRUE, currPosInd, splitPosInd, v4PredictedVertColor);
						ReconstructColorDifference(TRUE, u8Signs, udR, udG, udB, udA, v4VertColorDifference);
						v4PredictedVertColor.Add(v4VertColorDifference);
						IFXCHECKX(m_pAuthorLineSet->SetDiffuseColor(m_pLineSetDesc->m_numDiffuseColors, &v4PredictedVertColor));
						DiffColorLocalIndex[iPos] = m_pLineSetDesc->m_numDiffuseColors;
						m_pLineSetDesc->m_numDiffuseColors++;

					} else {
						DiffColorLocalIndex[iPos] = m_pLineSetDesc->m_numDiffuseColors-1;
					}
				} 
				//          4.6.3.3 New Line Specular Colors (presented only if Shading uses specular color)
				if (currMaterial.m_uSpecularColors) {

					m_pBitStreamX->ReadCompressedU8X(uACContextVertColorDupType,u8DupAttribLineIndexFlag);
					
					if (u8DupAttribLineIndexFlag==0) {
						m_pBitStreamX->ReadCompressedU8X(uACContextNormalDiffSigns,u8Signs);
						m_pBitStreamX->ReadCompressedU32X(uACContextVertexColorDiffMagR,udR);
						m_pBitStreamX->ReadCompressedU32X(uACContextVertexColorDiffMagG,udG);
						m_pBitStreamX->ReadCompressedU32X(uACContextVertexColorDiffMagB,udB);
						m_pBitStreamX->ReadCompressedU32X(uACContextVertexColorDiffMagA,udA);

						CalculatePredictedColorAtSplitPosX(FALSE, currPosInd, splitPosInd, v4PredictedVertColor);
						ReconstructColorDifference(FALSE, u8Signs, udR, udG, udB, udA, v4VertColorDifference);
						v4PredictedVertColor.Add(v4VertColorDifference);
						IFXCHECKX(m_pAuthorLineSet->SetSpecularColor(m_pLineSetDesc->m_numSpecularColors, &v4PredictedVertColor));
						SpecColorLocalIndex[iPos] = m_pLineSetDesc->m_numSpecularColors;
						m_pLineSetDesc->m_numSpecularColors++;

					} else {
						SpecColorLocalIndex[iPos] = m_pLineSetDesc->m_numSpecularColors-1;
					}
				
				} 
				//          4.6.3.4 New Line Tex Coord (presented only if Shading has tex coord)
				if (m_pMaxLineSetDesc->m_numTexCoords) {
					IFXVector4	v4PredictedTexCoord( 0.f, 0.f, 0.f, 0.f);
					IFXVector4	v4VertTexCoordDifference;

					for (iTex = 0; iTex < currMaterial.m_uNumTextureLayers; iTex++ ) {
					
						m_pBitStreamX->ReadCompressedU8X(uACContextTexCoordDupType, u8DupAttribLineIndexFlag);
						
						if (u8DupAttribLineIndexFlag==0) {
							m_pBitStreamX->ReadCompressedU8X(uACContextTexCoordDiffSigns,u8Signs);
							m_pBitStreamX->ReadCompressedU32X(uACContextTexCoordDiffMagU,udR);
							m_pBitStreamX->ReadCompressedU32X(uACContextTexCoordDiffMagV,udG);
							m_pBitStreamX->ReadCompressedU32X(uACContextTexCoordDiffMagS,udB);
							m_pBitStreamX->ReadCompressedU32X(uACContextTexCoordDiffMagT,udA);

							CalculatePredictedTexCoordAtSplitPosX(iTex, currPosInd, splitPosInd, v4PredictedTexCoord);
							ReconstructTexCoord( u8Signs, udR, udG, udB, udA, v4VertTexCoordDifference);
							v4PredictedTexCoord.Add(v4VertTexCoordDifference);
							IFXCHECKX(m_pAuthorLineSet->SetTexCoord(m_pLineSetDesc->m_numTexCoords, 
																		&v4PredictedTexCoord));
							TexCoordLocalIndex[iTex][iPos] = m_pLineSetDesc->m_numSpecularColors;
							m_pLineSetDesc->m_numTexCoords++;

						} else {
							TexCoordLocalIndex[iTex][iPos] = m_pLineSetDesc->m_numTexCoords-1;
						}
					}
				}
			} // iPos


			if (currMaterial.m_uNormals) {
				U32 oldNormCount = m_pLineSetDesc->m_numNormals - uNumLocalNormals;
				line.SetA(oldNormCount+NormLocalIndex[0]);
				line.SetB(oldNormCount+NormLocalIndex[1]);
				IFXCHECKX(m_pAuthorLineSet->SetNormalLine(lineInd, &line)); // Save in Author LS
			}
			m_pLineSetDesc->m_numLines++;

			if (currMaterial.m_uDiffuseColors) {
				line.SetA(DiffColorLocalIndex[0]);
				line.SetB(DiffColorLocalIndex[1]);
				IFXCHECKX(m_pAuthorLineSet->SetDiffuseLine(lineInd, &line)); // Save in Author LS
			}

			if (currMaterial.m_uSpecularColors) {
				line.SetA(SpecColorLocalIndex[0]);
				line.SetB(SpecColorLocalIndex[1]);
				IFXCHECKX(m_pAuthorLineSet->SetSpecularLine(lineInd, &line)); // Save in Author LS
			}

			if (m_pLineSetDesc->m_numTexCoords) {
				for (iTex = 0; iTex < currMaterial.m_uNumTextureLayers; iTex++ ) {
					line.SetA(TexCoordLocalIndex[iTex][0]);
					line.SetB(TexCoordLocalIndex[iTex][1]);
					IFXCHECKX(m_pAuthorLineSet->SetTexLine(iTex, lineInd, &line)); // Save in Author LS
				}
			}
		}
	}
}

void CIFXLineSetDecoder::ReconstructPosition(U8 u8Signs, U32 udX, U32 udY, U32 udZ, 
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


void CIFXLineSetDecoder:: ReconstructNormal(U8 u8Signs, U32 udX, U32 udY, U32 udZ, 
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


void CIFXLineSetDecoder::ReconstructTexCoord(U8 u8Signs, U32 udR, U32 udG, U32 udB, U32 udA, 
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


void CIFXLineSetDecoder::NormalizeNormals()
{
	IFXVector3* normals = NULL;
	U32 ind;
	IFXASSERT(m_pAuthorLineSet);

	IFXRESULT iRes = m_pAuthorLineSet->GetNormals(&normals);
	if (IFXSUCCESS(iRes)) {
		for (ind = 0; ind < (m_pAuthorLineSet->GetLineSetDesc())->m_numNormals; ind++ ) {
			normals[ind].Normalize();
		}
	}
}
