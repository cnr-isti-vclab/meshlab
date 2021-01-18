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
	@file	CIFXLineSetEncoder.cpp
*/

#include "CIFXLineSetEncoder.h"
#include "IFXCoreCIDs.h"
#include "IFXExportingCIDs.h"
#include "IFXBitStreamCompressedX.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"
#include "IFXMetaDataX.h"
#include "IFXAutoRelease.h"
#include "IFXSkeleton.h"
#include "IFXACContext.h"

//-----------------------------------------------------------------------------
// IFXUnknown
//-----------------------------------------------------------------------------
U32 CIFXLineSetEncoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXLineSetEncoder::Release()
{
	if (1 == m_uRefCount) {
		delete this;
		return 0;
	}
	return --m_uRefCount;
}

IFXRESULT CIFXLineSetEncoder::QueryInterface( IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result  = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXEncoderX )
		{
			*ppInterface = ( IFXEncoderX* ) this;
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
			result = IFX_E_UNSUPPORTED;
		}
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

//-----------------------------------------------------------------------------
// IFXEncoderX
//-----------------------------------------------------------------------------

// Provide the encoder with a pointer to the object which is to be encoded.
void CIFXLineSetEncoder::SetObjectX(IFXUnknown &rObject)
{
	// Release any previous LineSetResource
	IFXRELEASE(m_pLineSetResource);

	// If possible, store the object's IFXAuthorLineSetResource interface
	IFXCHECKX(rObject.QueryInterface(IID_IFXAuthorLineSetResource,(void**)&m_pLineSetResource));

//	IFXCHECKX(m_pLineSetResource->GetAuthorLineSet(m_pAuthorLineSet));
}

// Initialize and get a reference to the core services
void CIFXLineSetEncoder::InitializeX(IFXCoreServices &rCoreServices)
{
	// Release any previous core services pointer
	// and store the new core services reference
	rCoreServices.AddRef();
	IFXRELEASE(m_pCoreServices);
	m_pCoreServices = &rCoreServices;
}

// Encode data into data blocks and place these blocks in the queue
void CIFXLineSetEncoder::EncodeX(IFXString &rName, IFXDataBlockQueueX &rDataBlockQueue, F64 units )
{
	// Verify m_pLineSet is valid
	if(NULL == m_pLineSetResource) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	if( units > 0.0f )
		m_unitScale = units;

	// If a datablock queue cache exists, then simply copy blocks from that queue to
	// the priority queue passed in to this method.
	IFXDECLARELOCAL(IFXDataBlockQueueX,pResourceDataBlockQueueX);
	m_pLineSetResource->GetDataBlockQueueX( pResourceDataBlockQueueX );

	if ( pResourceDataBlockQueueX )
	{
		// make a copy of the resource's datablock queue
		IFXDECLARELOCAL(IFXDataBlockQueueX,pCopyOfResourceQueueX);
		pResourceDataBlockQueueX->CopyX( pCopyOfResourceQueueX );

		// copy blocks from the copy of the resource data block queue to the priority queue
		BOOL bDone = FALSE;
		while ( FALSE == bDone )
		{
			// Get the next data block
			IFXDECLARELOCAL(IFXDataBlockX,pDataBlockX);
			pCopyOfResourceQueueX->GetNextBlockX( pDataBlockX, bDone );

			if(pDataBlockX)
			{
				rDataBlockQueue.AppendBlockX( *pDataBlockX );
			}
		} // proceed to the next data block
	}
	// A datablock queue does NOT exist, so encode the resource to retrieve the datablocks.
	else
	{
		// Get priority increment
		{
			IFXASSERT(m_pLineSetResource);
			IFXDECLARELOCAL(IFXMarkerX,pMarker);
			IFXCHECKX(m_pLineSetResource->QueryInterface(IID_IFXMarkerX,(void**)&pMarker));
			m_uPriorityIncrement = pMarker->GetPriority();
		}

		IFXCHECKX( 
			IFXCreateComponent( 
				CID_IFXAuthorLineSetAnalyzer, 
				IID_IFXAuthorLineSetAnalyzer, 
				(void**)&m_pLineSetAnalyzer ));

		IFXVector3 scale;
		const IFXMatrix4x4 &matrix = m_pLineSetResource->GetTransform();
		matrix.CalcTRS( NULL, NULL, &scale );

		// Get the original author line set from the author line set resource
		IFXCHECKX( m_pLineSetResource->GetAuthorLineSet( m_pAuthorLineSet ) );

		// Apply scale from Author CLOD Resource transform to the mesh geometry. Do it
		// only if scale != (1.0f, 1.0f, 1.0f) or m_unitScale != 1.0f
		if( scale.X() != 1.0f || scale.Y() != 1.0f || scale.Z() != 1.0f || m_unitScale != 1.0f )
		{
			IFXVector3 *pPositions = NULL;
			const IFXAuthorLineSetDesc *pMaxLineDesc = NULL;
			pMaxLineDesc = m_pAuthorLineSet->GetMaxLineSetDesc();
			if( NULL == pMaxLineDesc )
				IFXCHECKX( IFX_E_NOT_INITIALIZED );
			U32 i, positionCnt = pMaxLineDesc->m_numPositions;

			IFXCHECKX( m_pAuthorLineSet->GetPositions( &pPositions ) );

			if( m_unitScale == 1.0f )
			{
				for( i = 0; i < positionCnt; i++ )
				{
					pPositions[i].Multiply( scale );
				}
			}
			else
			{
				F32 invScale = 1/(F32)m_unitScale;
				for( i = 0; i < positionCnt; i++ )
				{
					pPositions[i].Multiply( scale );
					pPositions[i].Scale( invScale );
				}
			}
		}

		MakeDeclarationBlockX(rName, rDataBlockQueue);

		MakeContinuationBlocksX(rName, rDataBlockQueue);

		// Revert scale applied to mesh during encoding
		if( scale.X() != 1.0f || scale.Y() != 1.0f || scale.Z() != 1.0f || m_unitScale != 1.0f )
		{
			if( scale.X() == 0.0f || scale.Y() == 0.0f || scale.Z() == 0.0f )
				IFXCHECKX( IFX_E_BAD_PARAM );

			IFXVector3 invScale = scale.Reciprocate();
			IFXVector3 *pPositions = NULL;
			const IFXAuthorLineSetDesc *pMaxLinesDesc = NULL;
			pMaxLinesDesc = m_pAuthorLineSet->GetMaxLineSetDesc();
			if( NULL == pMaxLinesDesc )
				IFXCHECKX( IFX_E_NOT_INITIALIZED );
			U32 i, positionCnt = pMaxLinesDesc->m_numPositions;

			IFXCHECKX( m_pAuthorLineSet->GetPositions( &pPositions ) );

			if( m_unitScale == 1.0f )
			{
				for( i = 0; i < positionCnt; i++ )
				{
					pPositions[i].Multiply( invScale );
				}
			}
			else
			{
				for( i = 0; i < positionCnt; i++ )
				{
					pPositions[i].Multiply( invScale );
					pPositions[i].Scale( (F32)m_unitScale );
				}
			}
		}

		IFXRELEASE( m_pLineSetAnalyzer );
	}
}

//-----------------------------------------------------------------------------
// Factory Method
//-----------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXLineSetEncoder_Factory(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		CIFXLineSetEncoder *pComponent = new CIFXLineSetEncoder;
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

//-----------------------------------------------------------------------------
//  Private methods
//-----------------------------------------------------------------------------

// Constuctor
CIFXLineSetEncoder::CIFXLineSetEncoder() :
IFXDEFINEMEMBER(m_pCoreServices),
IFXDEFINEMEMBER(m_pLineSetResource),
IFXDEFINEMEMBER(m_pAuthorLineSet)
{

	m_fInverseQuantPosition     = 1.0f;
	m_fInverseQuantNormal     = 1.0f;
	m_fInverseQuantTexCoord     = 1.0f;
	m_fInverseQuantDiffuseColor   = 1.0f;
	m_fInverseQuantSpecularColor  = 1.0f;

	m_uRefCount = 0;
	m_uPriorityIncrement = 0;
	m_uPriorityCurrent = 0;

	m_bBaseBlockPresent       = FALSE;

	m_pLineSetDescription     = NULL;
}

// Destructor
CIFXLineSetEncoder::~CIFXLineSetEncoder()
{
}

// Writes the model resource declaration block
void CIFXLineSetEncoder::MakeDeclarationBlockX(IFXString &rName, IFXDataBlockQueueX &rDataBlockQueue)
{

	// Create a bitstream
	IFXDECLARELOCAL(IFXBitStreamX,pBitStreamX);
	IFXCHECKX(IFXCreateComponent(CID_IFXBitStreamX,IID_IFXBitStreamX,(void**)&pBitStreamX));
	IFXASSERT(pBitStreamX);

	
	//
	// Write the block info for AuthorLineSetDeclaration:
	//
	//  1. IFXString Name
	//  2. U32 Chain Index (always 0 for this block)
	//  3. Max Mesh Description
	//    3.1 U32 NumLines;
	//    3.2 U32 NumPositions;
	//    3.3 U32 NumNormals;
	//    3.4 U32 NumDiffuseColors;
	//    3.5 U32 NumSpecularColors;
	//    3.6 U32 NumTexCoords;
	//    3.7 U32 NumShaders;
	//    3.8 Shaders Descriptions [NumShaders]
	//      3.8.1 U32 Shading Attributes (collection of flags: BOOL m_uDiffuseColors; BOOL m_uSpecularColors;)
	//      3.8.2 U32 NumTextureLayers;
	//      3.8.3 U32 m_uNumTextureLayers[NumTextureLayers]; (IFXMAX_TEX_COORD_LAYERS == 8)
	//      3.8.4 U32 OriginalShadingID;  // the mesh scrubber may have removed unused materials an repacked them.
	//  4. Resource Description
	//    4.1 Quality Factors
	//          4.1.1 U32 Position Quality Factor
	//          4.1.2 U32 Normal Quality Factor
	//          4.1.3 U32 Texture Coord Quality Factor
	//    4.2 Inverse Quantization
	//          4.2.1 F32 Position Inverse Quant
	//          4.2.2 F32 Normal Inverse Quant
	//          4.2.3 F32 Texture Coord Inverse Quant
	//          4.2.4 F32 Diffuse Color Inverse Quant
	//          4.2.4 F32 Specular Color Inverse Quant
	//    4.3 Resource parameters
	//          4.3.1 U32 Normal Crease Param
	//          4.3.2 U32 Normal Update Param
	//          4.3.3 U32 Normal Tolerance Param
	//  5. Skeleton Description
	//
	

	//  1. Name
	pBitStreamX->WriteIFXStringX(rName);

	//  2. Chain Index
	pBitStreamX->WriteU32X(0);

	//  3. Max Mesh Description
	const IFXAuthorLineSetDesc* pMaxLineSetDescription = m_pAuthorLineSet->GetMaxLineSetDesc();
	IFXASSERT(pMaxLineSetDescription);

	//  Line Set Reserved is a reserved field and shall have the value 0.
	pBitStreamX->WriteU32X(0);
	//    3.1 U32 NumLines;
	pBitStreamX->WriteU32X(pMaxLineSetDescription->m_numLines);
	//    3.2 U32 NumPositions;
	pBitStreamX->WriteU32X(pMaxLineSetDescription->m_numPositions);
	//    3.3 U32 NumNormals;
	pBitStreamX->WriteU32X(pMaxLineSetDescription->m_numNormals);
	//    3.4 U32 NumDiffuseColors;
	pBitStreamX->WriteU32X(pMaxLineSetDescription->m_numDiffuseColors);
	//    3.5 U32 NumSpecularColors;
	pBitStreamX->WriteU32X(pMaxLineSetDescription->m_numSpecularColors);
	//    3.6 U32 NumTexCoords;
	pBitStreamX->WriteU32X(pMaxLineSetDescription->m_numTexCoords);
	//    3.7 U32 NumShaders;
	pBitStreamX->WriteU32X(pMaxLineSetDescription->m_numMaterials);

	//    3.8 Shaders Descriptions
	{
		IFXAuthorMaterial* pAuthorMaterial = NULL;
		IFXCHECKX(m_pAuthorLineSet->GetMaterials(&pAuthorMaterial));
		U32 i = 0;

		for(i=0; i < pMaxLineSetDescription->m_numMaterials; i++) {
			//  3.8.1 U32 Shading Attributes (BOOL m_uDiffuseColors; BOOL m_uSpecularColors;)
			U32 uMaterialAttributes = 0;
			if(pAuthorMaterial[i].m_uDiffuseColors) {
				uMaterialAttributes |= 1;
			}
			if(pAuthorMaterial[i].m_uSpecularColors) {
				uMaterialAttributes |= 2;
			}
			pBitStreamX->WriteU32X(uMaterialAttributes);
			//  3.8.2 U32 NumTextureLayers;
			pBitStreamX->WriteU32X(pAuthorMaterial[i].m_uNumTextureLayers);
			//  3.8.3 U32 m_uNumTextureLayers[NumTextureLayers]; (IFX_MAX_TEXUNITS == 8)
			IFXASSERT(IFX_MAX_TEXUNITS >= pAuthorMaterial[i].m_uNumTextureLayers);
			U32 j = 0;
			for(j = 0; j < pAuthorMaterial[i].m_uNumTextureLayers; j++) {
				pBitStreamX->WriteU32X(pAuthorMaterial[i].m_uTexCoordDimensions[j]);
			}
			//  3.8.4 U32 OriginalShadingID;  // the mesh scrubber may have removed unused materials an repacked them.
			pBitStreamX->WriteU32X(pAuthorMaterial[i].m_uOriginalMaterialID);
		}
	}

	//  4. Extra resource info
	//    4.1 Quality Factors  -> not relevant for Author Line Set for now
	//          4.1.1 U32 Position Quality Factor
	pBitStreamX->WriteU32X( 0 );
	//          4.1.2 U32 Normal Quality Factor
	pBitStreamX->WriteU32X( 0 );
	//          4.1.3 U32 Texture Coord Quality Factor
	pBitStreamX->WriteU32X( 0 );

	//    4.2 Inverse Quantization Factors
	CalculateQuantizationFactorsX();
	pBitStreamX->WriteF32X(m_fInverseQuantPosition);
	pBitStreamX->WriteF32X(m_fInverseQuantNormal);
	pBitStreamX->WriteF32X(m_fInverseQuantTexCoord);
	pBitStreamX->WriteF32X(m_fInverseQuantDiffuseColor);
	pBitStreamX->WriteF32X(m_fInverseQuantSpecularColor);

	//    4.3 Resource Parameters
	pBitStreamX->WriteU32X( 0 );
	pBitStreamX->WriteU32X( 0 );
	pBitStreamX->WriteU32X( 0 );

	//  5. Skeleton Description

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

	// 3. Write the BoneCount
	U32 uBoneCount = 0;
	IFXSkeleton *pSkeleton = m_pLineSetResource->GetBones();
	IFXRESULT rc = IFX_OK;

	if( pSkeleton )
	{
		if( m_unitScale == 0.0f )
			IFXCHECKX( IFX_E_BAD_PARAM );
		F32 invUnits = 1/(F32)m_unitScale;

		rc = pSkeleton->GetNumBones(uBoneCount);

		if (IFXFAILURE( rc))
		{
			throw IFXException( rc );
		}

		// 5.1. Bones Count
		pBitStreamX->WriteU32X(uBoneCount);

		// 5.2. Write the Bone (repeated BoneCount times)
		U32 i;
		for ( i = 0; i < uBoneCount; i++ )
		{
			IFXBoneInfo boneInfo;
			IFXCHECKX( pSkeleton->GetBoneInfo(i, &boneInfo) );

			// 5.2.1 BoneName
			pBitStreamX->WriteIFXStringX((boneInfo.stringBoneName));

			// 5.2.2 BoneParentName
			pBitStreamX->WriteIFXStringX(boneInfo.stringParentName);

			// 5.2.3 BoneAttributes
			pBitStreamX->WriteU32X(boneInfo.uBoneAttributes);

			// 5.2.4 BoneLength
			pBitStreamX->WriteF32X(boneInfo.fBoneLength * invUnits);

			// 5.2.5 BoneDisplacement
			pBitStreamX->WriteF32X(boneInfo.v3BoneDisplacement.X() * invUnits);
			pBitStreamX->WriteF32X(boneInfo.v3BoneDisplacement.Y() * invUnits);
			pBitStreamX->WriteF32X(boneInfo.v3BoneDisplacement.Z() * invUnits);

			// 5.2.6 BoneRotation
			pBitStreamX->WriteF32X(boneInfo.v4BoneRotation.R());
			pBitStreamX->WriteF32X(boneInfo.v4BoneRotation.G());
			pBitStreamX->WriteF32X(boneInfo.v4BoneRotation.B());
			pBitStreamX->WriteF32X(boneInfo.v4BoneRotation.A());

			if(IFXSKELETON_LINKPRESENT & boneInfo.uBoneAttributes)
			{
				// 5.2.7 NumberLinks
				pBitStreamX->WriteU32X(boneInfo.uNumLinks);

				// 5.2.8 LinkLength
				pBitStreamX->WriteF32X(boneInfo.fLinkLength * invUnits);
			}

			if(IFXSKELETON_JOINTPRESENT & boneInfo.uBoneAttributes)
			{
				// 5.2.9 StartJoint
				pBitStreamX->WriteF32X(boneInfo.v2StartJointCenter.U() * invUnits);
				pBitStreamX->WriteF32X(boneInfo.v2StartJointCenter.V() * invUnits);
				pBitStreamX->WriteF32X(boneInfo.v2StartJointScale.U());
				pBitStreamX->WriteF32X(boneInfo.v2StartJointScale.V());

				// 5.2.10 EndJoint
				pBitStreamX->WriteF32X(boneInfo.v2EndJointCenter.U() * invUnits);
				pBitStreamX->WriteF32X(boneInfo.v2EndJointCenter.V() * invUnits);
				pBitStreamX->WriteF32X(boneInfo.v2EndJointScale.U());
				pBitStreamX->WriteF32X(boneInfo.v2EndJointScale.V());
			}

			// 5.2.11 RotationalConstraint
			pBitStreamX->WriteF32X(boneInfo.fRotationConstraintXMax * invUnits);
			pBitStreamX->WriteF32X(boneInfo.fRotationConstraintXMin * invUnits);
			pBitStreamX->WriteF32X(boneInfo.fRotationConstraintYMax * invUnits);
			pBitStreamX->WriteF32X(boneInfo.fRotationConstraintYMin * invUnits);
			pBitStreamX->WriteF32X(boneInfo.fRotationConstraintZMax * invUnits);
			pBitStreamX->WriteF32X(boneInfo.fRotationConstraintZMin * invUnits);

		} // end for (every bone)
	}
	else
	{
		//  5.1 Bones Count
		pBitStreamX->WriteU32X(0);
	}

	// Get the data block
	IFXDECLARELOCAL(IFXDataBlockX,pDataBlockX);
	pBitStreamX->GetDataBlockX(pDataBlockX);
	IFXASSERT(pDataBlockX);

	// Set block type (U3D)
	pDataBlockX->SetBlockTypeX( BlockType_GeneratorLineSetU3D );
	// Set block priority
	pDataBlockX->SetPriorityX(0);

	// set metadata
	IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
	IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
	pDataBlockX->QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
	m_pLineSetResource->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
	pBlockMD->AppendX(pObjectMD);

	// Add block to queue
	rDataBlockQueue.AppendBlockX(*pDataBlockX);
}


// Calculate the quantization factors
void CIFXLineSetEncoder::CalculateQuantizationFactorsX()
{
	U32 uQualityFactor = 1000;

	m_pLineSetResource->GetQualityFactorX(uQualityFactor,IFXAuthorLineSetResource::POSITION_QUALITY);
	if(1000 == uQualityFactor) {
		m_fQuantPosition = (F32) pow(2.0,18.0);
	} else {
		m_fQuantPosition = (F32) pow(1.0076537604105041221998506395494,uQualityFactor+545.0);
	}

	// Scale m_fQuantPosition by the radius of the bounding sphere
	IFXASSERT(m_pAuthorLineSet);
	// get the line set description
	const IFXAuthorLineSetDesc* pLineSetDescription = m_pAuthorLineSet->GetMaxLineSetDesc();
	IFXASSERT(pLineSetDescription);
	IFXASSERT(pLineSetDescription->m_numPositions > 0);

	IFXVector3* pPosition = NULL;
	IFXCHECKX(m_pAuthorLineSet->GetPositions(&pPosition));

	// Calculate the center of the bounding sphere
	IFXVector3 vMin(pPosition[0]), vMax(pPosition[0]);
	U32 i = 0;
	U32 uNumPositions = pLineSetDescription->m_numPositions;
	for(i = 1; i < uNumPositions ; i++) {
		vMin.X() += (pPosition[i].X() - vMin.X()) * (pPosition[i].X() < vMin.X());
		vMin.Y() += (pPosition[i].Y() - vMin.Y()) * (pPosition[i].Y() < vMin.Y());
		vMin.Z() += (pPosition[i].Z() - vMin.Z()) * (pPosition[i].Z() < vMin.Z());
		vMax.X() += (pPosition[i].X() - vMax.X()) * (pPosition[i].X() > vMax.X());
		vMax.Y() += (pPosition[i].Y() - vMax.Y()) * (pPosition[i].Y() > vMax.Y());
		vMax.Z() += (pPosition[i].Z() - vMax.Z()) * (pPosition[i].Z() > vMax.Z());
	}
	IFXVector3 vCenter(vMin);
	vCenter.Add(vMax);
	vCenter.Scale(0.5f);

	// Calculate the radius of the bounding sphere
	F32 fRadiusSquared = 0.0f;
	F32 fDistanceSquared = 0.0f;
	for(i=0; i < uNumPositions; i++) {
		fDistanceSquared = vCenter.CalcDistanceSquaredFrom(pPosition[i]);
		fRadiusSquared += (fDistanceSquared - fRadiusSquared) * (fDistanceSquared > fRadiusSquared);
	}

	// Scale the position quantization factor by the radius of the bounding sphere
	if(fRadiusSquared > 0.0f) {
		m_fQuantPosition /= sqrtf(fRadiusSquared);
	}

	// Limit quantization factor to fit quantized results in 32 bit unsigned integer
	{
		// Find position coordinate farthest from the origin
		F32 fMaxPositionCoordinate;
		fMaxPositionCoordinate = IFXMAX(fabs(vMax.X()),fabs(vMax.Y()));
		fMaxPositionCoordinate = IFXMAX(fMaxPositionCoordinate,fabs(vMax.Z()));
		fMaxPositionCoordinate = IFXMAX(fMaxPositionCoordinate,fabs(vMin.X()));
		fMaxPositionCoordinate = IFXMAX(fMaxPositionCoordinate,fabs(vMin.Y()));
		fMaxPositionCoordinate = IFXMAX(fMaxPositionCoordinate,fabs(vMin.Z()));

		F32 fLimit = (F32) (0xFFFFFE00); // (F32) (0xFFFFFE00)
		F32 fMaxQuantPosition = fLimit / fMaxPositionCoordinate;

		// Clamp position quantization factor
		m_fQuantPosition = IFXMIN(m_fQuantPosition,fMaxQuantPosition);
	}

	m_pLineSetResource->GetQualityFactorX(uQualityFactor,IFXAuthorLineSetResource::NORMAL_QUALITY);
	if(1000 == uQualityFactor) {
		m_fQuantNormal = (F32) pow(2.0,14.0);
	} else {
		m_fQuantNormal = (F32) pow(1.0048638204237854409678879459798,uQualityFactor+857.0);
	}

	m_pLineSetResource->GetQualityFactorX(uQualityFactor,IFXAuthorLineSetResource::TEXCOORD_QUALITY);
	if(1000 == uQualityFactor) {
		m_fQuantTexCoord = (F32) pow(2.0,14.0);
	} else {
        m_fQuantTexCoord = (F32) pow(1.0048638204237854409678879459798,uQualityFactor+857.0);
	}

	m_pLineSetResource->GetQualityFactorX(uQualityFactor,IFXAuthorLineSetResource::DIFFUSE_QUALITY);
	if(1000 == uQualityFactor) {
		m_fQuantDiffuseColor = (F32) pow(2.0,14.0);
	} else {
		m_fQuantDiffuseColor = (F32) pow(1.0022294514890519310704865897552,uQualityFactor+1741.0);
	}

	m_pLineSetResource->GetQualityFactorX(uQualityFactor,IFXAuthorLineSetResource::SPECULAR_QUALITY);
	if(1000 == uQualityFactor) {
		m_fQuantSpecularColor = (F32) pow(2.0,14.0);
	} else {
		m_fQuantSpecularColor = (F32) pow(1.0022294514890519310704865897552,uQualityFactor+1741.0);
	}

	m_fInverseQuantPosition = (F32) 1.0 / m_fQuantPosition;
	m_fInverseQuantNormal = (F32) 1.0 / m_fQuantNormal;
	m_fInverseQuantTexCoord = (F32) 1.0 / m_fQuantTexCoord;
	m_fInverseQuantDiffuseColor = (F32) 1.0 /m_fQuantDiffuseColor;
	m_fInverseQuantSpecularColor = (F32) 1.0 /m_fQuantSpecularColor;
}


void CIFXLineSetEncoder::CalculatePredictedNormalAtSplitPosX(U32 currPosInd, U32 splitPosInd,
															 IFXVector3& vPredictedNormal)
{
	IFXASSERT(m_pLineSetDescription);
	IFXASSERT(m_pLineSetAnalyzer);
	IFXASSERT(m_pLineSetAnalyzer->IsInitialized());

	if (m_pLineSetDescription->m_numLines == 0 ||
		m_pLineSetDescription->m_numPositions == 0) return;

	vPredictedNormal = IFXVector3(0.f, 0.f, 0.f);
	if (currPosInd == 0) return; // no lines deifed yet - so predicted is juat zero normal

	// get lines for split postion
	IFXArray <U32> lines, endPositions, newLines, newEnds;
	lines.Clear(); endPositions.Clear(); newLines.Clear(); newEnds.Clear();

	IFXCHECKX(IFX_OK== m_pLineSetAnalyzer->GetLines(splitPosInd, lines, endPositions));
	U32 numLines = lines.GetNumberElements();
	IFXASSERT(numLines == endPositions.GetNumberElements());

	if (numLines==0) return; // just stand alone position = no lines connected w/ it

	//  mark lines and their ends that are going to be used at normal predictionin this update
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

	//  calculate predicted normal using those lines that alrady written
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
		// in line aleady normal indices
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

void CIFXLineSetEncoder::CalculatePredictedColorAtSplitPosX(BOOL bDiffuseColor, U32 currPosInd,
															U32 splitPosInd, IFXVector4& v4PredictedVertColor)
{
	IFXASSERT(m_pLineSetDescription);
	IFXASSERT(m_pLineSetAnalyzer);
	IFXASSERT(m_pLineSetAnalyzer->IsInitialized());

	if (m_pLineSetDescription->m_numLines == 0 ||
		m_pLineSetDescription->m_numPositions == 0) return;

	v4PredictedVertColor = IFXVector4(0.f, 0.f, 0.f, 0.f);
	if (currPosInd == 0) return; // no lines defined yet - so predicted is just zero normal

	if ( (m_uCurrentNumDiffuseColors==0 && bDiffuseColor) ||
		(m_uCurrentNumSpecularColors==0 && (!bDiffuseColor))) return;

	// get lines for split postion
	IFXArray <U32> lines, endPositions, newLines, newEnds;
	lines.Clear(); endPositions.Clear(); newLines.Clear(); newEnds.Clear();

	IFXCHECKX(IFX_OK== m_pLineSetAnalyzer->GetLines(splitPosInd, lines, endPositions));
	U32 numLines = lines.GetNumberElements();
	IFXASSERT(numLines == endPositions.GetNumberElements());

	if (numLines==0) return; // just stand alone position = no lines connected w/ it

	//  mark lines and their ends that are going to be used at color predictionin this update
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

	//  calculate predicted normal using those lines that alrady written
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
		if (bDiffuseColor) {
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

		if (bDiffuseColor) {
			m_pAuthorLineSet->GetDiffuseColor(splitInd, &color);
		} else {
			m_pAuthorLineSet->GetSpecularColor(splitInd, &color);
		}
		v4PredictedVertColor.Add(color);
	}
	v4PredictedVertColor.Scale((F32)linesForPredict);
}

void CIFXLineSetEncoder::CalculatePredictedTexCoordAtSplitPosX(U32 texInd, U32 currPosInd, U32 splitPosInd,
															   IFXVector4& v4PredictedTexCoord)
{
	IFXASSERT(m_pLineSetDescription);
	IFXASSERT(m_pLineSetAnalyzer);
	IFXASSERT(m_pLineSetAnalyzer->IsInitialized());
	v4PredictedTexCoord = IFXVector4(0.f, 0.f, 0.f, 0.f);

	if (m_pLineSetDescription->m_numLines == 0 ||
		m_pLineSetDescription->m_numPositions == 0) return;


	if (currPosInd == 0) return; // no lines defined yet - so predicted is just zero normal

	if ( texInd >= m_pLineSetDescription->m_numTexCoords ) return;

	// get lines for split postion
	IFXArray <U32> lines, endPositions, newLines, newEnds;
	lines.Clear(); endPositions.Clear(); newLines.Clear(); newEnds.Clear();

	IFXCHECKX(IFX_OK== m_pLineSetAnalyzer->GetLines(splitPosInd, lines, endPositions));
	U32 numLines = lines.GetNumberElements();
	IFXASSERT(numLines == endPositions.GetNumberElements());

	if (numLines==0) return; // just stand alone position = no lines connected w/ it

	//  mark lines and their ends that are going to be used at color predictionin this update
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

	//  calculate predicted normal using those lines that alrady written
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

void CIFXLineSetEncoder::GetLineAttribIndexes(U32 lineInd, U32 splitPos,
											  CIFXLineSetEncoder::AttribType attrib,
											  U32& splitInd, U32& endInd )
{
	IFXU32Line  line, attribLine;
	m_pAuthorLineSet->GetPositionLine(lineInd, &line);
	U32 endA = line.VertexA();
#ifdef _DEBUG
	U32 endB = line.VertexB();
#endif
	switch (attrib)
	{
	case NORMAL:
		m_pAuthorLineSet->GetNormalLine(lineInd, &attribLine);
		break;
	case DIFFUSECOLOR:
		m_pAuthorLineSet->GetDiffuseLine(lineInd, &attribLine);
		break;
	case SPECULARCOLOR:
		m_pAuthorLineSet->GetSpecularLine(lineInd, &attribLine);
		break;
	default:
		IFXASSERTBOX(0, "Unsupported attribute!");
		break;
	}

	IFXASSERT(endA==splitPos || endB==splitPos);
	// in line aleady normal indices
	if (endA == splitPos) {
		splitInd = attribLine.VertexA();
		endInd   = attribLine.VertexB();
	} else { // endB == splitPos
		splitInd = attribLine.VertexB();
		endInd   = attribLine.VertexA();
	}
}

void CIFXLineSetEncoder::GetLineTexCoord(U32 texLayer, U32 lineInd, U32 currPos,
										 U32& currInd, U32& endInd )
{
	IFXU32Line  line, texLine;
	m_pAuthorLineSet->GetPositionLine(lineInd, &line);
	U32 endA = line.VertexA();
#ifdef _DEBUG
	U32 endB = line.VertexB();
#endif
	m_pAuthorLineSet->GetTexLine(texLayer, lineInd, &texLine);

	IFXASSERT(endA==currPos || endB==currPos);
	// in line aleady normal indices
	if (endA == currPos) {
		currInd  = texLine.VertexA();
		endInd   = texLine.VertexB();
	} else { // endB == currPos
		currInd = texLine.VertexB();
		endInd   = texLine.VertexA();
	}
}


void CIFXLineSetEncoder::QuantizeColorForWrite(BOOL bDiffuseColor, IFXVector4& color,
											   U8& u8Signs, U32& udR, U32& udG, U32& udB, U32& udA)
{
	F32 fQuantVertColor;
	if (bDiffuseColor)
	{
		fQuantVertColor = m_fQuantDiffuseColor;
	}
	else
	{
		fQuantVertColor = m_fQuantSpecularColor;
	}
	u8Signs = (color.R() < 0.0) | ((color.G() < 0.0) << 1) | ((color.B() < 0.0) << 2) | ((color.A() < 0.0) << 3);
	udR = (U32) (0.5f + fQuantVertColor * fabs(color.R()));
	udG = (U32) (0.5f + fQuantVertColor * fabs(color.G()));
	udB = (U32) (0.5f + fQuantVertColor * fabs(color.B()));
	udA = (U32) (0.5f + fQuantVertColor * fabs(color.A()));
}

void CIFXLineSetEncoder::QuantizeTexCoordForWrite(IFXVector4& texCoord,
												  U8& u8Signs, U32& udR, U32& udG, U32& udB, U32& udA)
{
	u8Signs = (texCoord.R() < 0.0) | ((texCoord.G() < 0.0) << 1) |
		((texCoord.B() < 0.0) << 2) | ((texCoord.A() < 0.0) << 3);
	udR = (U32) (0.5f + m_fQuantTexCoord * fabs(texCoord.R()));
	udG = (U32) (0.5f + m_fQuantTexCoord * fabs(texCoord.G()));
	udB = (U32) (0.5f + m_fQuantTexCoord * fabs(texCoord.B()));
	udA = (U32) (0.5f + m_fQuantTexCoord * fabs(texCoord.A()));
}

void CIFXLineSetEncoder::MakeContinuationBlocksX(IFXString &rName, IFXDataBlockQueueX &rDataBlockQueue)
{
	// get the mesh description
	m_pLineSetDescription = m_pAuthorLineSet->GetMaxLineSetDesc();

	// Analyzer will help deal with line segments connectivity
	IFXASSERT(m_pLineSetAnalyzer);
	IFXCHECKX(IFX_OK==m_pLineSetAnalyzer->Initialize(m_pAuthorLineSet));

	//#define DEBUG_VERBOSE

#ifdef DEBUG_VERBOSE
	U32 indBlock = 0;
	m_pLineSetAnalyzer->DumpAuthorLineSet();
#endif

	if(m_pLineSetDescription->m_numNormals)
		PrepareForWriting();

	U32 uMinResolution = 0 ;
	U32 uMaxResolution = m_pLineSetDescription->m_numPositions ;
	U32 uNumResCoded = uMinResolution;

	while (uNumResCoded < uMaxResolution)
	{
		U32 uStartResolution = uNumResCoded;
		U32 uEndResolution = uStartResolution + 4096;
		if(uEndResolution > uMaxResolution)
			uEndResolution = uMaxResolution;

		// Create a bitstream
		IFXDECLARELOCAL(IFXBitStreamCompressedX,pBitStreamX);
		IFXCHECKX(IFXCreateComponent(CID_IFXBitStreamX,IID_IFXBitStreamCompressedX,(void**)&pBitStreamX));
		U32 uProfile;
		m_pCoreServices->GetProfile(uProfile);
		pBitStreamX->SetNoCompressionMode((uProfile & IFXPROFILE_NOCOMPRESSION) ? TRUE : FALSE);

		//
		// Write the Continutaion block info for AuthorLineSet
		//
		//  1. Name
		//  2. Chain Index (always 0 for this block)
		//  3. Resolution Range
		//    3.1 U32 StartResultion;
		//    3.2 U32 EndResulution;
		//  4. Line Description (= Verstex Update) [NumVertexUpdates]
		//      4.1 U32[rCurrentPositionCount] Split Positions index
		//      4.2 New Poistion Info  (as quantized position difference)
		//          4.2.1 U8[cDiffPosSign]  position Difference Signs
		//          4.2.2 U32[cDiffPos] position difference X
		//          4.2.3 U32[cDiffPos] position difference Y
		//          4.2.4 U32[cDiffPos] position difference Z
		//      4.3 U32[cNormlCnt]:New Normal Count
		//      4.4 New Normal Info [New Normal Count]
		//          4.4.1 U8[cDiffNormalSign]  position Difference Signs
		//          4.4.2 U32[cDiffNormal] normal difference X
		//          4.4.3 U32[cDiffNormal] normal difference Y
		//          4.4.4 U32[cDiffNormal] normal difference Z
		//      4.5 U32[cLineCnt]:New Line Count
		//      4.6 New Line Info [New Line Count]
		//          4.6.1 U32[cShading]  Shading ID
		//          4.6.2 U32[rCurrentPositionCount] first postion index
		//          4.6.3 New Line Details [2]
		//          4.6.3.1 U32[cNormlIndx] normal local index
		//          4.6.3.2 New Line Diffuse Colors (presented only if Shading uses diffuse color)
		//          4.6.3.3 New Line Specular Colors (presented only if Shading uses specular color)
		//          4.6.3.4 New Line Texture Colors
		//
		
		//  1. Name
		pBitStreamX->WriteIFXStringX(rName);

		//  2. Chain Index
		pBitStreamX->WriteU32X( 0 );

		//  3. Base Mesh Description
		//    3.1 U32 StartResultion;
		pBitStreamX->WriteU32X(uStartResolution);
		//    3.2 U32 EndResultion;
		pBitStreamX->WriteU32X(uEndResolution);

		U32 currPosInd;
		m_uPositionsWritten = (U32)-1;

		m_uCurrentTexCoordCount     = 0;
		m_uCurrentNumDiffuseColors    = 0;
		m_uCurrentNumSpecularColors   = 0;
		m_uCurrentNumTexCoord           = 0;
		m_uLastDiffuseColorInd      = (U32)-1;
		m_uLastSpecularColorInd     = (U32)-1;
		m_uLastTexCoordInd        = (U32)-1;
#ifdef DEBUG_VERBOSE
		IFXTRACE_GENERIC(L"[LineSet Encoding Cont block] %i \n", indBlock);
#endif
		for (currPosInd=uStartResolution; currPosInd < uEndResolution; currPosInd++)
		{
			WriteLineUpdateX(currPosInd, pBitStreamX);
		}

		// Get the data block
		IFXDECLARELOCAL(IFXDataBlockX,pDataBlockX);
		pBitStreamX->GetDataBlockX(pDataBlockX);
		IFXASSERT(pDataBlockX);

		// Set block type
		pDataBlockX->SetBlockTypeX( BlockType_GeneratorLineSetContinuationU3D );
		// Set block priority
		pDataBlockX->SetPriorityX( m_uPriorityCurrent );
		m_uPriorityCurrent += m_uPriorityIncrement;

		// set metadata
		IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
		IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
		pDataBlockX->QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
		m_pLineSetResource->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
		pBlockMD->AppendX(pObjectMD);

		// Add block to queue
		rDataBlockQueue.AppendBlockX(*pDataBlockX);

		uNumResCoded = uEndResolution;
#ifdef DEBUG_VERBOSE
		indBlock++;
#endif
	}

}// MakeContinuationBlockX

#if _DEBUG
void CIFXLineSetEncoder::ReconstructPosition(U8 u8Signs, U32 udX, U32 udY, U32 udZ,
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
#endif

U32 CIFXLineSetEncoder::WriteLineUpdateX(U32 currPosInd, IFXBitStreamCompressedX* pBitStreamX)
{
	IFXVector3 vPositionDifference, vPosition, vPredictedPosition;
	U8 u8Signs;
	U32 udX, udY, udZ;
	U32 writtenLines = 0;
	U32 splitPosInd;

	IFXASSERT(m_pLineSetDescription);

	if (currPosInd==0)  {
		//      4.1 U32[rCurrentPositionCount] Split Positions index
		//    this is an exception - for 0 index - this is no split index and no difference
		//      this is just 0
		//      instead of position diff - it will be position itself
		pBitStreamX->WriteCompressedU32X(uACStaticFull+1,0);

		//      4.2 New Poistion Info  (as quantized position difference)
		//          4.2.1 U8[cDiffPosSign]  position Difference Signs
		//          4.2.2 U32[cDiffPos] position difference X
		//          4.2.3 U32[cDiffPos] position difference Y
		//          4.2.4 U32[cDiffPos] position difference Z

		//  Get 0-position
		m_pAuthorLineSet->GetPosition(0,&vPosition);

		//  Quantize position difference and write it
		QuantizePositionForWrite(vPosition, u8Signs, udX, udY, udZ);
		pBitStreamX->WriteCompressedU8X(uACContextPositionDiffSigns,u8Signs);
		pBitStreamX->WriteCompressedU32X(uACContextPositionDiffMagX,udX);
		pBitStreamX->WriteCompressedU32X(uACContextPositionDiffMagY,udY);
		pBitStreamX->WriteCompressedU32X(uACContextPositionDiffMagZ,udZ);

		//      4.3 U32[cNormlCnt]:New Normal Count
		//      4.4 New Normal Info [New Normal Count]
		//          4.4.1 ...
		//      4.5 U32[cLineCnt]:New Line Count
		//      4.6 New Line Info [New Line Count]

		pBitStreamX->WriteCompressedU32X(uACContextNumLocalNormals, 0);
		pBitStreamX->WriteCompressedU32X(uACContextNumNewFaces, 0); // ??? Faces ?? -> should do for Lines

		m_uPositionsWritten = 1;

		return 0;
	}

	splitPosInd = currPosInd-1;
	//      4.1 U32[rCurrentPositionCount] Split Positions index
	pBitStreamX->WriteCompressedU32X(uACStaticFull+currPosInd, splitPosInd);
	//      4.2 New Poistion Info  (as quantized position difference)
	//          4.2.1 U8[cDiffPosSign]  position Difference Signs
	//          4.2.2 U32[cDiffPos] position difference X
	//          4.2.3 U32[cDiffPos] position difference Y
	//          4.2.4 U32[cDiffPos] position difference Z
	m_pAuthorLineSet->GetPosition(currPosInd,&vPosition);
	m_pAuthorLineSet->GetPosition(splitPosInd,&vPredictedPosition);
	vPositionDifference.Subtract(vPosition,vPredictedPosition);

	//  Quantize position difference and write it
	QuantizePositionForWrite(vPositionDifference, u8Signs, udX, udY, udZ);
	pBitStreamX->WriteCompressedU8X(uACContextPositionDiffSigns,u8Signs);
	pBitStreamX->WriteCompressedU32X(uACContextPositionDiffMagX,udX);
	pBitStreamX->WriteCompressedU32X(uACContextPositionDiffMagY,udY);
	pBitStreamX->WriteCompressedU32X(uACContextPositionDiffMagZ,udZ);

#if _DEBUG
	IFXVector3 vectCheck;
	ReconstructPosition(u8Signs, udX, udY, udZ, vectCheck);
#endif

	// Evaluate lines to write for this update
	IFXArray <U32> lines, endPositions, newLines, newEnds;
	IFXCHECKX(IFX_OK== m_pLineSetAnalyzer->GetLines(currPosInd, lines, endPositions));
	U32 numLines = lines.GetNumberElements();
	IFXASSERT(numLines == endPositions.GetNumberElements());

	//  mark lines and their ends that are going to be in this update
	U32 iLine;
	writtenLines = 0;
	for(iLine = 0; iLine<numLines; iLine++)
	{
		if (endPositions.GetElement(iLine)<currPosInd) {
			newLines.CreateNewElement() = lines.GetElement(iLine);
			newEnds.CreateNewElement()  = endPositions.GetElement(iLine);
			writtenLines++;
		}
	}

	//      4.3 U32[cNormlCnt]:New Normal Count
	//      4.4 New Normal Info [New Normal Count]
	//          4.4.1 U8[cDiffNormalSign]  position Difference Signs
	//          4.4.2 U32[cDiffNormal] normal difference X
	//          4.4.3 U32[cDiffNormal] normal difference Y
	//          4.4.4 U32[cDiffNormal] normal difference Z

	if( m_pLineSetDescription->m_numNormals )
	{
		IFXVector3 vNormalDifference, vPredictedNormal;
		CalculatePredictedNormalAtSplitPosX(currPosInd, splitPosInd, vPredictedNormal);
		U32 normalsCountWritten = writtenLines*2;
		/// @todo: implement writting only unique normals
		//....
		// - Write the number of new normals at this position (count of unique normals from above)
		//
		pBitStreamX->WriteCompressedU32X(uACContextNumLocalNormals,normalsCountWritten);
		for(iLine = 0; iLine<writtenLines; iLine++)
		{
			U32 lineInd;
			IFXU32Line line;
			IFXVector3 normCurr, normEnd;

			lineInd = newLines.GetElement(iLine);
			m_pAuthorLineSet->GetPositionLine(lineInd, &line);
			U32 endA = line.VertexA();
			U32 endB = line.VertexB();
			m_pAuthorLineSet->GetNormalLine(lineInd, &line);
			IFXASSERT(endA==currPosInd || endB==currPosInd);
			// in line aleady normal indices
			if (endA == currPosInd) {
				endA = line.VertexA();
				endB = line.VertexB();
				m_pAuthorLineSet->GetNormal(endA,&normCurr);
				m_pAuthorLineSet->GetNormal(endB,&normEnd);
			} else { // endB == currPosInd
				endB = line.VertexB();
				endA = line.VertexA();
				m_pAuthorLineSet->GetNormal(endB,&normCurr);
				m_pAuthorLineSet->GetNormal(endA,&normEnd);
			}

			// this normal at the line "end"
			vNormalDifference.Subtract(normEnd,vPredictedNormal);
			//  Quantize position difference and write it
			QuantizeNormalForWrite(vNormalDifference, u8Signs, udX, udY, udZ);
			pBitStreamX->WriteCompressedU8X(uACContextNormalDiffSigns,u8Signs);
			pBitStreamX->WriteCompressedU32X(uACContextNormalDiffMagX,udX);
			pBitStreamX->WriteCompressedU32X(uACContextNormalDiffMagY,udY);
			pBitStreamX->WriteCompressedU32X(uACContextNormalDiffMagZ,udZ);

			// this at the line in current position
			// get the "normCurr" normal difference
			vNormalDifference.Subtract(normCurr,vPredictedNormal);
			//  Quantize position difference and write it
			QuantizeNormalForWrite(vNormalDifference, u8Signs, udX, udY, udZ);
			pBitStreamX->WriteCompressedU8X(uACContextNormalDiffSigns,u8Signs);
			pBitStreamX->WriteCompressedU32X(uACContextNormalDiffMagX,udX);
			pBitStreamX->WriteCompressedU32X(uACContextNormalDiffMagY,udY);
			pBitStreamX->WriteCompressedU32X(uACContextNormalDiffMagZ,udZ);

		}
	}
	else
		pBitStreamX->WriteCompressedU32X(uACContextNumLocalNormals,0);
	//      4.5 U32[cLineCnt]:New Line Count
	//      4.6 New Line Info [New Line Count]

	pBitStreamX->WriteCompressedU32X(uACContextNumNewFaces,writtenLines);
	for(iLine = 0; iLine<writtenLines; iLine++)
	{
		U32 lineInd = newLines.GetElement(iLine);

		//  4.6.1 U32[cShading]  Shading ID
		// Get the material id for the line and write it
		U32 uMaterialID;
		m_pAuthorLineSet->GetLineMaterial(lineInd,&uMaterialID);
		IFXASSERT(uMaterialID<m_pLineSetDescription->m_numMaterials);
		pBitStreamX->WriteCompressedU32X(uACContextLineShadingID,uMaterialID);

		//  4.6.2 U32[rCurrentPositionCount] first postion index
		// Get and write first pos index (end, not current position - > it is another one)
		U32 endInd = newEnds.GetElement(iLine);
		IFXASSERT(endInd<m_pLineSetDescription->m_numPositions);
		pBitStreamX->WriteCompressedU32X(uACStaticFull+currPosInd,endInd);

		// prepare for writing colors
		IFXAuthorMaterial currMaterial;
		U32 currDiffColorInd = 0, endDiffColorInd = 0, currSpecColorInd = 0, endSpecColorInd =0;

		IFXCHECKX(m_pAuthorLineSet->GetMaterial(uMaterialID, &currMaterial));

		if (currMaterial.m_uDiffuseColors) 
		{
			GetLineAttribIndexes(lineInd, currPosInd, DIFFUSECOLOR, currDiffColorInd, endDiffColorInd);
		}
		if (currMaterial.m_uSpecularColors) 
		{
			GetLineAttribIndexes(lineInd, currPosInd, SPECULARCOLOR, currSpecColorInd, endSpecColorInd);
		}

		U32 iPos;
		for ( iPos=0; iPos<2; iPos++)
		{
			//  4.6.3.1 U32[cNormlIndx] normal local index
			U32 NormLocalIndex = 2*iLine + iPos;
			pBitStreamX->WriteCompressedU32X(uACContextNormalLocalIndex,NormLocalIndex);

			//  4.6.3.2 New Line Diffuse Colors (presented only if Shading uses diffuse color)
			//  4.6.3.3 New Line Specular Colors (presented only if Shading uses specular color)

			U8 u8DupAttribLineIndexFlag = 0;
			IFXVector4 v4PredictedVertColor(0.f, 0.f, 0.f, 0.f), v4VertColorDifference, v4VertColor;
			U32 udR, udG, udB, udA;

			if (currMaterial.m_uDiffuseColors) 
			{
				u8DupAttribLineIndexFlag = 1;

				if (iPos==0) 
				{	
					// end color
					if (m_uLastDiffuseColorInd != endDiffColorInd) 
					{
						m_pAuthorLineSet->GetDiffuseColor(endDiffColorInd,&v4VertColor);
						CalculatePredictedColorAtSplitPosX(TRUE, currPosInd, splitPosInd, v4PredictedVertColor);
						m_uCurrentNumDiffuseColors++;
						u8DupAttribLineIndexFlag = 0; // new color will be added
						m_uLastDiffuseColorInd = endDiffColorInd;
					}
				} 
				else 
				{       
					// current color
					if (m_uLastDiffuseColorInd != currDiffColorInd) 
					{
						m_pAuthorLineSet->GetDiffuseColor(currDiffColorInd,&v4VertColor);
						CalculatePredictedColorAtSplitPosX(TRUE, currPosInd, splitPosInd, v4PredictedVertColor);
						m_uCurrentNumDiffuseColors++;
						u8DupAttribLineIndexFlag = 0; // new color will be added
						m_uLastDiffuseColorInd = currDiffColorInd;
					}
				}

				pBitStreamX->WriteCompressedU8X(uACContextVertColorDupType,u8DupAttribLineIndexFlag);
				if (u8DupAttribLineIndexFlag==0) 
				{
					v4VertColorDifference.Subtract(v4VertColor,v4PredictedVertColor);
					QuantizeColorForWrite(TRUE, v4VertColorDifference,
						u8Signs, udR, udG, udB, udA);
					pBitStreamX->WriteCompressedU8X(uACContextNormalDiffSigns,u8Signs);
					pBitStreamX->WriteCompressedU32X(uACContextVertexColorDiffMagR,udR);
					pBitStreamX->WriteCompressedU32X(uACContextVertexColorDiffMagG,udG);
					pBitStreamX->WriteCompressedU32X(uACContextVertexColorDiffMagB,udB);
					pBitStreamX->WriteCompressedU32X(uACContextVertexColorDiffMagA,udA);
				}
			}


			if (currMaterial.m_uSpecularColors) 
			{
				u8DupAttribLineIndexFlag = 1;

				if (iPos==0) 
				{ 
					// end color
					if (m_uLastSpecularColorInd != endSpecColorInd) 
					{
						m_pAuthorLineSet->GetSpecularColor(endSpecColorInd,&v4VertColor);
						CalculatePredictedColorAtSplitPosX(FALSE, currPosInd, splitPosInd,
							v4PredictedVertColor);
						m_uCurrentNumSpecularColors++;
						u8DupAttribLineIndexFlag = 0; // new color will be added
						m_uLastSpecularColorInd = endSpecColorInd;
					}

				} 
				else 
				{       
					// current color
					if (m_uLastSpecularColorInd != currSpecColorInd) 
					{
						m_pAuthorLineSet->GetSpecularColor(currSpecColorInd,&v4VertColor);
						CalculatePredictedColorAtSplitPosX(FALSE, currPosInd, splitPosInd, v4PredictedVertColor);
						m_uCurrentNumSpecularColors++;
						u8DupAttribLineIndexFlag = 0; // new color will be added
						m_uLastSpecularColorInd = currSpecColorInd;
					}
				}

				pBitStreamX->WriteCompressedU8X(uACContextVertColorDupType,u8DupAttribLineIndexFlag);
				if (u8DupAttribLineIndexFlag==0) {
					v4VertColorDifference.Subtract(v4VertColor,v4PredictedVertColor);
					QuantizeColorForWrite(FALSE, v4VertColorDifference,
						u8Signs, udR, udG, udB, udA);
					pBitStreamX->WriteCompressedU8X(uACContextNormalDiffSigns,u8Signs);
					pBitStreamX->WriteCompressedU32X(uACContextVertexColorDiffMagR,udR);
					pBitStreamX->WriteCompressedU32X(uACContextVertexColorDiffMagG,udG);
					pBitStreamX->WriteCompressedU32X(uACContextVertexColorDiffMagB,udB);
					pBitStreamX->WriteCompressedU32X(uACContextVertexColorDiffMagA,udA);

				}
			}

			// Texture coords

			if (m_pLineSetDescription->m_numTexCoords) 
			{
				IFXASSERT(IFX_MAX_TEXUNITS>=currMaterial.m_uNumTextureLayers);
				U32 endTexCoordInd, currTexCoordInd;
				IFXVector4  texVector( 0.f, 0.f, 0.f, 0.f), v4PredictedTexCoord( 0.f, 0.f, 0.f, 0.f);
				IFXVector4  v4VertTexCoordDifference;
				U32 iTex;
				for ( iTex = 0 ; iTex < currMaterial.m_uNumTextureLayers; iTex++ ) 
				{
					GetLineTexCoord(iTex, lineInd, currPosInd, currTexCoordInd, endTexCoordInd);

					u8DupAttribLineIndexFlag = 1;

					if (iPos==0) 
					{ 
						// end point tex coord
						if (m_uLastTexCoordInd != endTexCoordInd) 
						{
							m_pAuthorLineSet->GetTexCoord(endTexCoordInd, &texVector);
							CalculatePredictedTexCoordAtSplitPosX(iTex, currPosInd, splitPosInd, v4PredictedTexCoord);
							m_uCurrentNumTexCoord++;
							u8DupAttribLineIndexFlag = 0; // new text coord will be added
							m_uLastTexCoordInd = endTexCoordInd;
						}

					} 
					else 
					{       
						// current text coord
						if (m_uLastTexCoordInd != currTexCoordInd) 
						{
							m_pAuthorLineSet->GetTexCoord(currTexCoordInd, &texVector);
							CalculatePredictedTexCoordAtSplitPosX(iTex, currPosInd, splitPosInd, v4PredictedTexCoord);
							m_uCurrentNumTexCoord++;
							u8DupAttribLineIndexFlag = 0; // new tex coord will be added
							m_uLastTexCoordInd = currTexCoordInd;
						}
					}

					pBitStreamX->WriteCompressedU8X(uACContextTexCoordDupType,u8DupAttribLineIndexFlag);
					if (u8DupAttribLineIndexFlag==0) 
					{
						v4VertTexCoordDifference.Subtract(texVector,v4PredictedTexCoord);
						QuantizeTexCoordForWrite(v4VertTexCoordDifference, u8Signs, udR, udG, udB, udA);
						pBitStreamX->WriteCompressedU8X(uACContextTexCoordDiffSigns,u8Signs);
						pBitStreamX->WriteCompressedU32X(uACContextTexCoordDiffMagU,udR);
						pBitStreamX->WriteCompressedU32X(uACContextTexCoordDiffMagV,udG);
						pBitStreamX->WriteCompressedU32X(uACContextTexCoordDiffMagS,udB);
						pBitStreamX->WriteCompressedU32X(uACContextTexCoordDiffMagT,udA);

					}
				}
			}
		}
	}

	m_uPositionsWritten++;

	return writtenLines;
}

void CIFXLineSetEncoder::PrepareForWriting()
{
	IFXVector3* normals = NULL;
	U32 ind;
	IFXASSERT(m_pAuthorLineSet);

	IFXRESULT iRes = m_pAuthorLineSet->GetNormals(&normals);
	if (IFXSUCCESS(iRes)) 
	{
		for (ind = 0; ind < (m_pAuthorLineSet->GetLineSetDesc())->m_numNormals; ind++ ) 
		{
			normals[ind].Normalize();
		}
	}
}

void CIFXLineSetEncoder::QuantizePositionForWrite(IFXVector3& pos, U8& u8Signs, U32& udX, U32& udY, U32& udZ) 
{
	u8Signs = (pos.X() < 0.0) | ((pos.Y() < 0.0) << 1) | ((pos.Z() < 0.0) << 2);
	udX = (U32) (0.5f + m_fQuantPosition * fabs(pos.X()));
	udY = (U32) (0.5f + m_fQuantPosition * fabs(pos.Y()));
	udZ = (U32) (0.5f + m_fQuantPosition * fabs(pos.Z()));
}

void CIFXLineSetEncoder::QuantizeNormalForWrite(IFXVector3& pos, U8& u8Signs, U32& udX, U32& udY, U32& udZ) 
{
	u8Signs = (pos.X() < 0.0) | ((pos.Y() < 0.0) << 1) | ((pos.Z() < 0.0) << 2);
	udX = (U32) (0.5f + m_fQuantNormal * fabs(pos.X()));
	udY = (U32) (0.5f + m_fQuantNormal * fabs(pos.Y()));
	udZ = (U32) (0.5f + m_fQuantNormal * fabs(pos.Z()));
}
