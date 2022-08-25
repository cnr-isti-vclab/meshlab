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
	@name  CIFXPointSetEncoder.cpp
*/

#include "CIFXPointSetEncoder.h"
#include "IFXCoreCIDs.h"
#include "IFXExportingCIDs.h"
#include "IFXBitStreamCompressedX.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"
#include "IFXMetaDataX.h"
#include "IFXAutoRelease.h"
#include "IFXSkeleton.h"
#include "IFXACContext.h"

//#define DEBUG_VERBOSE
//void DumpAuthorPointSet();

//-----------------------------------------------------------------------------
// IFXUnknown
//-----------------------------------------------------------------------------
U32 CIFXPointSetEncoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXPointSetEncoder::Release()
{
	if (1 == m_uRefCount) {
		delete this;
		return 0;
	}
	return --m_uRefCount;
}

IFXRESULT CIFXPointSetEncoder::QueryInterface( IFXREFIID interfaceId, void** ppInterface)
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
void CIFXPointSetEncoder::SetObjectX(IFXUnknown &rObject)
{
	// Release any previous PointSetResource
	IFXRELEASE(m_pPointSetResource);

	// If possible, store the object's IFXAuthorPointSetResource interface

	//!!!
	IFXCHECKX(rObject.QueryInterface(IID_IFXAuthorPointSetResource,(void**)&m_pPointSetResource));
	//IFXCHECKX(rObject.QueryInterface(IID_IFXNode,(void**)&m_pPointSetResource));

//	IFXCHECKX(m_pPointSetResource->GetAuthorPointSet(m_pAuthorPointSet));
}

// Initialize and get a reference to the core services
void CIFXPointSetEncoder::InitializeX(IFXCoreServices &rCoreServices)
{
	// Release any previous core services pointer
	// and store the new core services reference
	rCoreServices.AddRef();
	IFXRELEASE(m_pCoreServices);
	m_pCoreServices = &rCoreServices;
}

// Encode data into data blocks and place these blocks in the queue
void CIFXPointSetEncoder::EncodeX(IFXString &rName, IFXDataBlockQueueX &rDataBlockQueue, F64 units )
{
	// Verify m_pPointSet is valid
	if(NULL == m_pPointSetResource) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	if( units > 0.0f )
		m_unitScale = units;

	// If a datablock queue cache exists, then simply copy blocks from that queue to
	// the priority queue passed in to this method.
	IFXDECLARELOCAL(IFXDataBlockQueueX,pResourceDataBlockQueueX);
	m_pPointSetResource->GetDataBlockQueueX( pResourceDataBlockQueueX );

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
			IFXASSERT(m_pPointSetResource);
			IFXDECLARELOCAL(IFXMarkerX,pMarker);
			IFXCHECKX(m_pPointSetResource->QueryInterface(IID_IFXMarkerX,(void**)&pMarker));
			m_uPriorityIncrement = pMarker->GetPriority();
		}

		// Get the original author mesh from the author CLOD resource
		IFXVector3 scale;
		const IFXMatrix4x4 &matrix = m_pPointSetResource->GetTransform();
		matrix.CalcTRS( NULL, NULL, &scale );

		IFXCHECKX( m_pPointSetResource->GetAuthorPointSet( m_pAuthorPointSet ) );
		// Apply scale from Author CLOD Resource transform to the mesh geometry. Do it
		// only if scale != (1.0f, 1.0f, 1.0f) or m_unitScale != 1.0f
		if( scale.X() != 1.0f || scale.Y() != 1.0f || scale.Z() != 1.0f || m_unitScale != 1.0f )
		{
			IFXVector3 *pPositions = NULL;
			const IFXAuthorPointSetDesc *pMaxPointDesc = NULL;
			pMaxPointDesc = m_pAuthorPointSet->GetMaxPointSetDesc();
			if( NULL == pMaxPointDesc )
				IFXCHECKX( IFX_E_NOT_INITIALIZED );
			U32 i, positionCnt = pMaxPointDesc->m_numPositions;

			IFXCHECKX( m_pAuthorPointSet->GetPositions( &pPositions ) );

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

		MakeContinuationBlockX(rName, rDataBlockQueue);

		// Revert scale applied to mesh during encoding
		if( scale.X() != 1.0f || scale.Y() != 1.0f || scale.Z() != 1.0f || m_unitScale != 1.0f )
		{
			if( scale.X() == 0.0f || scale.Y() == 0.0f || scale.Z() == 0.0f )
				IFXCHECKX( IFX_E_BAD_PARAM );

			IFXVector3 invScale = scale.Reciprocate();
			IFXVector3 *pPositions = NULL;
			const IFXAuthorPointSetDesc *pMaxPointsDesc = NULL;
			pMaxPointsDesc = m_pAuthorPointSet->GetMaxPointSetDesc();
			if( NULL == pMaxPointsDesc )
				IFXCHECKX( IFX_E_NOT_INITIALIZED );
			U32 i, positionCnt = pMaxPointsDesc->m_numPositions;

			IFXCHECKX( m_pAuthorPointSet->GetPositions( &pPositions ) );

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
	}
}

//-----------------------------------------------------------------------------
// Factory Method
//-----------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXPointSetEncoder_Factory(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		CIFXPointSetEncoder *pComponent = new CIFXPointSetEncoder;
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
CIFXPointSetEncoder::CIFXPointSetEncoder() :
IFXDEFINEMEMBER(m_pCoreServices),
IFXDEFINEMEMBER(m_pPointSetResource),
IFXDEFINEMEMBER(m_pAuthorPointSet)
{
	m_fInverseQuantPosition     = 1.0f;
	m_fInverseQuantNormal     = 1.0f;
	m_fInverseQuantTexCoord     = 1.0f;
	m_fInverseQuantDiffuseColor   = 1.0f;
	m_fInverseQuantSpecularColor  = 1.0f;


	m_uReservedPointSetParameter0   =0;
	m_uReservedPointSetParameter1   =0;
	m_uReservedPointSetParameter2   =0;
	m_uReservedPointSetParameter3   =0;

	m_uRefCount = 0;
	m_uPriorityIncrement = 0;
	m_uPriorityCurrent = 0;

	m_bBaseBlockPresent       = FALSE;

	m_pPointSetDescription      = NULL;
}

// Destructor
CIFXPointSetEncoder::~CIFXPointSetEncoder()
{
}

// Writes the model resource declaration block
void CIFXPointSetEncoder::MakeDeclarationBlockX(IFXString &rName, IFXDataBlockQueueX &rDataBlockQueue)
{
	// Create a bitstream
	IFXDECLARELOCAL(IFXBitStreamX,pBitStreamX);
	IFXCHECKX(IFXCreateComponent(CID_IFXBitStreamX,IID_IFXBitStreamX,(void**)&pBitStreamX));
	IFXASSERT(pBitStreamX);

	
	//
	// Write the block info for AuthorPointSetDeclaration:
	//
	//  1. IFXString Name
	//  2. U32 Chain Index (always 0 for this block)
	//  3. Max PointSet Description
	//      3.0 U32 Point Set Reserved
	//    3.1 U32 NumPoints;
	//    3.2 U32 NumPositions;
	//    3.3 U32 NumNormals;
	//    3.4 U32 NumDiffuseColors;
	//    3.5 U32 NumSpecularColors;
	//    3.6 U32 NumTexCoords;
	//    3.7 U32 NumShaders;
	//    3.8 Shaders Descriptions [NumShaders]
	//      3.8.1 U32 Shading Attributes (collection of flags: BOOL DiffuseColors; BOOL SpecularColors;)
	//      3.8.2 U32 NumTextureLayers;
	//      3.8.3 U32 TexCoordDimensions[NumTextureLayers]; (IFXMAX_TEX_COORD_LAYERS == 8)
	//      3.8.4 U32 OriginalShadingID;
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

	//  3. Max PointSet Description
	const IFXAuthorPointSetDesc* pMaxPointSetDescription = m_pAuthorPointSet->GetMaxPointSetDesc();
	IFXASSERT(pMaxPointSetDescription);

	//Point Set Reserved is a reserved field and shall have the value 0.
	pBitStreamX->WriteU32X(m_uReservedPointSetParameter0);
	//    3.1 U32 NumPoints;
	pBitStreamX->WriteU32X(pMaxPointSetDescription->m_numPoints);
	//    3.2 U32 NumPositions;
	pBitStreamX->WriteU32X(pMaxPointSetDescription->m_numPositions);
	//    3.3 U32 NumNormals;
	pBitStreamX->WriteU32X(pMaxPointSetDescription->m_numNormals);
	//    3.4 U32 NumDiffuseColors;
	pBitStreamX->WriteU32X(pMaxPointSetDescription->m_numDiffuseColors);
	//    3.5 U32 NumSpecularColors;
	pBitStreamX->WriteU32X(pMaxPointSetDescription->m_numSpecularColors);
	//    3.6 U32 NumTexCoords;
	pBitStreamX->WriteU32X(pMaxPointSetDescription->m_numTexCoords);
	//    3.7 U32 NumShaders;
	pBitStreamX->WriteU32X(pMaxPointSetDescription->m_numMaterials);

	//    3.8 Shaders Descriptions
	{
		IFXAuthorMaterial* pAuthorMaterial = NULL;
		IFXCHECKX(m_pAuthorPointSet->GetMaterials(&pAuthorMaterial));
		U32 i = 0;

		for(i=0; i < pMaxPointSetDescription->m_numMaterials; i++) {
			//  3.8.1 U32 Shading Attributes (BOOL DiffuseColors; BOOL SpecularColors;)
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
			//  3.8.3 U32 TexCoordDimensions[NumTextureLayers]; (IFX_MAX_TEXUNITS == 8)
			IFXASSERT(IFX_MAX_TEXUNITS >= pAuthorMaterial[i].m_uNumTextureLayers);
			U32 j = 0;
			for(j = 0; j < pAuthorMaterial[i].m_uNumTextureLayers; j++) {
				pBitStreamX->WriteU32X(pAuthorMaterial[i].m_uTexCoordDimensions[j]);
			}
			//  3.8.4 U32 OriginalShadingID;
			pBitStreamX->WriteU32X(pAuthorMaterial[i].m_uOriginalMaterialID);
		}
	}

	//  4. Extra resource info
	//    4.1 Quality Factors  -> not relevant for Author Point Set for now
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
	pBitStreamX->WriteU32X( m_uReservedPointSetParameter1 );
	pBitStreamX->WriteU32X( m_uReservedPointSetParameter2 );
	pBitStreamX->WriteU32X( m_uReservedPointSetParameter3 );

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
	IFXSkeleton *pSkeleton = m_pPointSetResource->GetBones();
	IFXRESULT rc = IFX_OK;

	if( pSkeleton )
	{
		if( m_unitScale == 0.0f )
			IFXCHECKX( IFX_E_BAD_PARAM );
		F32 invUnits = 1/(F32)m_unitScale;

		pSkeleton->GetNumBones(uBoneCount);

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
		//  5.1 Zero Bones Count
		pBitStreamX->WriteU32X(0);
	}

	IFXRELEASE(pSkeleton);

	// Get the data block
	IFXDECLARELOCAL(IFXDataBlockX,pDataBlockX);
	pBitStreamX->GetDataBlockX(pDataBlockX);
	IFXASSERT(pDataBlockX);

	// Set block type (U3D)
	pDataBlockX->SetBlockTypeX( BlockType_GeneratorPointSetU3D );
	// Set block priority
	pDataBlockX->SetPriorityX(0);

	// set metadata
	IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
	IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
	pDataBlockX->QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
	m_pPointSetResource->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
	pBlockMD->AppendX(pObjectMD);

	// Add block to queue
	rDataBlockQueue.AppendBlockX(*pDataBlockX);
}

// Calculate the quantization factors
void CIFXPointSetEncoder::CalculateQuantizationFactorsX()
{
	U32 uQualityFactor = 1000;

	m_pPointSetResource->GetQualityFactorX(uQualityFactor,IFXAuthorPointSetResource::POSITION_QUALITY);
	if(1000 == uQualityFactor) {
		m_fQuantPosition = (F32) pow(2.0,18.0);
	} else {
		m_fQuantPosition = (F32) pow(1.0076537604105041221998506395494,uQualityFactor+545.0);
	}

	// Scale m_fQuantPosition by the radius of the bounding sphere
	IFXASSERT(m_pAuthorPointSet);
	// get the Point set description
	const IFXAuthorPointSetDesc* pPointSetDescription = m_pAuthorPointSet->GetMaxPointSetDesc();
	IFXASSERT(pPointSetDescription);
	IFXASSERT(pPointSetDescription->m_numPositions > 0);

	IFXVector3* pPosition = NULL;
	IFXCHECKX(m_pAuthorPointSet->GetPositions(&pPosition));

	// Calculate the center of the bounding sphere
	IFXVector3 vMin(pPosition[0]), vMax(pPosition[0]);
	U32 i = 0;
	U32 uNumPositions = pPointSetDescription->m_numPositions;
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

	m_pPointSetResource->GetQualityFactorX(uQualityFactor,IFXAuthorPointSetResource::NORMAL_QUALITY);
	if(1000 == uQualityFactor) {
		m_fQuantNormal = (F32) pow(2.0,14.0);
	} else {
		m_fQuantNormal = (F32) pow(1.0048638204237854409678879459798,uQualityFactor+857.0);
	}

	m_pPointSetResource->GetQualityFactorX(uQualityFactor,IFXAuthorPointSetResource::TEXCOORD_QUALITY);
	if(1000 == uQualityFactor) {
		m_fQuantTexCoord = (F32) pow(2.0,14.0);
	} else {
		m_fQuantTexCoord = (F32) pow(1.0048638204237854409678879459798,uQualityFactor+857.0);
	}

	m_pPointSetResource->GetQualityFactorX(uQualityFactor,IFXAuthorPointSetResource::DIFFUSE_QUALITY);
	if(1000 == uQualityFactor) {
		m_fQuantDiffuseColor = (F32) pow(2.0,14.0);
	} else {
		m_fQuantDiffuseColor = (F32) pow(1.0022294514890519310704865897552,uQualityFactor+1741.0);
	}

	m_pPointSetResource->GetQualityFactorX(uQualityFactor,IFXAuthorPointSetResource::SPECULAR_QUALITY);
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


void CIFXPointSetEncoder::CalculatePredictedNormalAtSplitPosX(U32 currPosInd, U32 splitPosInd,
															  IFXVector3& vPredictedNormal)
{
	vPredictedNormal = IFXVector3(0.f, 0.f, 0.f);
	IFXASSERT(m_pPointSetDescription);

	if (m_pPointSetDescription->m_numPoints == 0 ||
		m_pPointSetDescription->m_numPositions == 0) return;

	if (currPosInd == 0) return; // no Points deifed yet - so predicted is just zero normal

	U32 Point, pointInd;
	IFXVector3 normal;
	IFXArray<U32> Points;

	GetPointsAtPosition(splitPosInd, Points);
	U32 numPoints = Points.GetNumberElements();
	if (numPoints == 0) return;

	U32 i;
	for ( i = 0; i<numPoints; i++) {
		pointInd = Points.GetElement(i);
		m_pAuthorPointSet->GetNormalPoint(pointInd, &Point);
		m_pAuthorPointSet->GetNormal(Point,&normal);
		vPredictedNormal.Add(normal);
	}
	vPredictedNormal.Normalize();
}

void CIFXPointSetEncoder::CalculatePredictedColorAtSplitPosX(BOOL bDiffuseColor, U32 currPosInd,
															 U32 splitPosInd, IFXVector4& v4PredictedVertColor)
{
	v4PredictedVertColor = IFXVector4(0.f, 0.f, 0.f, 0.f);
	IFXASSERT(m_pPointSetDescription);

	if (m_pPointSetDescription->m_numPoints == 0 ||
		m_pPointSetDescription->m_numPositions == 0) return;

	if (currPosInd == 0) return; // no Points defined yet - so predicted is just zero color

	if ( (m_uCurrentNumDiffuseColors==0 && bDiffuseColor) ||
		(m_uCurrentNumSpecularColors==0 && (!bDiffuseColor))) return;

	IFXVector4 color;
	U32 Point, pointInd;
	IFXArray<U32> Points;

	GetPointsAtPosition(splitPosInd, Points);
	U32 numPoints = Points.GetNumberElements();
	if (numPoints == 0) return;

	U32 i;
	for ( i = 0; i<numPoints; i++) {
		pointInd = Points.GetElement(i);

		if (bDiffuseColor) {
			m_pAuthorPointSet->GetDiffusePoint(pointInd, &Point);
		} else {
			m_pAuthorPointSet->GetSpecularPoint(pointInd, &Point);
		}

		if (bDiffuseColor) {
			m_pAuthorPointSet->GetDiffuseColor(Point, &color);
		} else {
			m_pAuthorPointSet->GetSpecularColor(Point, &color);
		}
		v4PredictedVertColor.Add(color);

	}

	v4PredictedVertColor.Normalize3();
	v4PredictedVertColor.A() = v4PredictedVertColor.A()/((F32)numPoints);
}

void CIFXPointSetEncoder::CalculatePredictedTexCoordAtSplitPosX(U32 texInd, U32 currPosInd, U32 splitPosInd,
																IFXVector4& v4PredictedTexCoord)
{
	IFXASSERT(m_pPointSetDescription);
	v4PredictedTexCoord = IFXVector4(0.f, 0.f, 0.f, 0.f);

	if (m_pPointSetDescription->m_numPoints == 0 ||
		m_pPointSetDescription->m_numPositions == 0) return;

	// no Points defined yet - so predicted is just zero normal
	if (currPosInd == 0) return;

	if ( texInd > m_pPointSetDescription->m_numTexCoords ) return;

	// get Points at split postion
	IFXArray <U32> Points;
	GetPointsAtPosition(splitPosInd, Points);

	U32 numPoints = Points.GetNumberElements();
	if (numPoints == 0) return;

	U32 iPoint, pointInd, Point;
	//  calculate predicted normal using those Points that alrady written
	for(iPoint = 0; iPoint<numPoints; iPoint++)
	{
		pointInd = Points.GetElement(iPoint);
		IFXVector4 v4Temp;

		m_pAuthorPointSet->GetTexPoint(texInd, pointInd, &Point);
		m_pAuthorPointSet->GetTexCoord(Point, &v4Temp);
		v4PredictedTexCoord.Add(v4Temp);
	}
	v4PredictedTexCoord.Scale((F32)numPoints); // !!!
}

void CIFXPointSetEncoder::QuantizeTexCoordForWrite(IFXVector4& texCoord,
												   U8& u8Signs, U32& udR, U32& udG, U32& udB, U32& udA)
{
	u8Signs = (texCoord.R() < 0.0) | ((texCoord.G() < 0.0) << 1) |
		((texCoord.B() < 0.0) << 2) | ((texCoord.A() < 0.0) << 3);
	udR = (U32) (0.5f + m_fQuantTexCoord * fabs(texCoord.R()));
	udG = (U32) (0.5f + m_fQuantTexCoord * fabs(texCoord.G()));
	udB = (U32) (0.5f + m_fQuantTexCoord * fabs(texCoord.B()));
	udA = (U32) (0.5f + m_fQuantTexCoord * fabs(texCoord.A()));
}

void CIFXPointSetEncoder::GetPointAttribIndexes(U32 PointInd, CIFXPointSetEncoder::AttribType attrib,
												U32& attrInd )
{
	if (-1>=(int)PointInd || PointInd >= (m_pAuthorPointSet->GetPointSetDesc()->m_numPoints))
		return;

	switch (attrib)
	{
	case NORMAL:
		m_pAuthorPointSet->GetNormalPoint(PointInd, &attrInd);
		break;
	case DIFFUSECOLOR:
		m_pAuthorPointSet->GetDiffusePoint(PointInd, &attrInd);
		break;
	case SPECULARCOLOR:
		m_pAuthorPointSet->GetSpecularPoint(PointInd, &attrInd);
		break;
	default:
		IFXASSERTBOX(0, "Unsupported attribute!");
		break;
	}
}

void CIFXPointSetEncoder::QuantizeColorForWrite(BOOL bDiffuseColor, IFXVector4& color,
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

void CIFXPointSetEncoder::MakeContinuationBlockX(IFXString &rName, IFXDataBlockQueueX &rDataBlockQueue)
{
	// get the pointset description
	m_pPointSetDescription = m_pAuthorPointSet->GetMaxPointSetDesc();

	if(m_pPointSetDescription->m_numNormals)
		PrepareForWriting();

	U32 uMinResolution = 0 ;
	U32 uMaxResolution = m_pPointSetDescription->m_numPositions ;
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
		// Write the Continutaion block info for AuthorPointSet
		//
		//  1. Name
		//  2. Chain Index (always 0 for this block)
		//  3. Resolution Range
		//    3.1 U32 StartResultion;
		//    3.2 U32 EndResulution;
		//  4. Point Description (= Verstex Update) [NumVertexUpdates]
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
		//      4.5 U32[cPointCnt]:New Point Count
		//      4.6 New Point Info [New Point Count]
		//          4.6.1 U32[cShading]  Shading ID
		//          4.6.2 U32[cNormlIndx] normal local index
		//          4.6.3 New Point Diffuse Colors (presented only if Shading uses diffuse color)
		//          4.6.4 New Point Specular Colors (presented only if Shading uses specular color)
		//          4.6.5 New Point Texture Colors
		//
		

		//  1. Name
		pBitStreamX->WriteIFXStringX(rName);

		//  2. Chain Index
		pBitStreamX->WriteU32X( 0 );

		//  3. Base PointSet Description
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
		DumpAuthorPointSet();
#endif

		for (currPosInd=uStartResolution; currPosInd < uEndResolution; currPosInd++)
		{
			WritePointUpdateX(currPosInd, pBitStreamX);
			IFXTRACE_GENERIC(L"[PointSet ENCODING UPDATE] %i \n", currPosInd);
		}

		// Get the data block
		IFXDECLARELOCAL(IFXDataBlockX,pDataBlockX);
		pBitStreamX->GetDataBlockX(pDataBlockX);
		IFXASSERT(pDataBlockX);

		// Set block type
		pDataBlockX->SetBlockTypeX( BlockType_GeneratorPointSetContinuationU3D );
		// Set block priority
		pDataBlockX->SetPriorityX( m_uPriorityCurrent );
		m_uPriorityCurrent += m_uPriorityIncrement;

		// set metadata
		IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
		IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
		pDataBlockX->QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
		m_pPointSetResource->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
		pBlockMD->AppendX(pObjectMD);

		// Add block to queue
		rDataBlockQueue.AppendBlockX(*pDataBlockX);

		uNumResCoded = uEndResolution;
	}
}


U32 CIFXPointSetEncoder::WritePointUpdateX(U32 currPosInd, IFXBitStreamCompressedX* pBitStreamX)
{
	IFXVector3 vPositionDifference, vPosition, vPredictedPosition( 0.f, 0.f, 0.f);
	U8 u8Signs;
	U32 udX, udY, udZ;
	U32 writtenPoints = 0;
	U32 splitPosInd;

	IFXASSERT(m_pPointSetDescription);

	splitPosInd = currPosInd-1;
	//      4.1 U32[rCurrentPositionCount] Split Positions index
	if ( (int)splitPosInd==-1 )  {
		//    this is an exception - for 0 index - this is no split index and no difference
		//      this is just 0
		//      instead of position diff - it will be position itself
		pBitStreamX->WriteCompressedU32X(uACStaticFull+1,0);
	} else {
		pBitStreamX->WriteCompressedU32X(uACStaticFull+currPosInd, splitPosInd);
	}
	//      4.2 New Poistion Info  (as quantized position difference)
	//          4.2.1 U8[cDiffPosSign]  position Difference Signs
	//          4.2.2 U32[cDiffPos] position difference X
	//          4.2.3 U32[cDiffPos] position difference Y
	//          4.2.4 U32[cDiffPos] position difference Z
	m_pAuthorPointSet->GetPosition(currPosInd,&vPosition);

	if ( (int)splitPosInd>=0 )
		m_pAuthorPointSet->GetPosition(splitPosInd,&vPredictedPosition);

	vPositionDifference.Subtract(vPosition,vPredictedPosition);

	//  Quantize position difference and write it
	QuantizePositionForWrite(vPositionDifference, u8Signs, udX, udY, udZ);
	pBitStreamX->WriteCompressedU8X(uACContextPositionDiffSigns,u8Signs);
	pBitStreamX->WriteCompressedU32X(uACContextPositionDiffMagX,udX);
	pBitStreamX->WriteCompressedU32X(uACContextPositionDiffMagY,udY);
	pBitStreamX->WriteCompressedU32X(uACContextPositionDiffMagZ,udZ);

	// Evaluate Points to write for this update
	IFXArray <U32> newPoints;
	GetPointsAtPosition(currPosInd, newPoints);

	writtenPoints = newPoints.GetNumberElements();
	if (writtenPoints == 0) {
		return 0; // no points to write
	}

	//      4.3 U32[cNormlCnt]:New Normal Count
	//      4.4 New Normal Info [New Normal Count]
	//          4.4.1 U8[cDiffNormalSign]  position Difference Signs
	//          4.4.2 U32[cDiffNormal] normal difference X
	//          4.4.3 U32[cDiffNormal] normal difference Y
	//          4.4.4 U32[cDiffNormal] normal difference Z
	U32 PointInd, iPoint;

	//4.3
	if( m_pPointSetDescription->m_numNormals )
	{
		IFXVector3 vNormalDifference, vPredictedNormal( 0.f, 0.f, 0.f);

		if ( (int)splitPosInd>=0 )
			CalculatePredictedNormalAtSplitPosX(currPosInd, splitPosInd, vPredictedNormal);

		pBitStreamX->WriteCompressedU32X(uACContextNumLocalNormals, writtenPoints);

		for(iPoint = 0; iPoint < writtenPoints; iPoint++)
		{
			U32 Point;
			IFXVector3 normal;

			PointInd = newPoints.GetElement(iPoint);
			m_pAuthorPointSet->GetNormalPoint(PointInd, &Point);

			m_pAuthorPointSet->GetNormal(Point,&normal);

			vNormalDifference.Subtract(normal,vPredictedNormal);
			//  Quantize normal difference and write it
			QuantizeNormalForWrite(vNormalDifference, u8Signs, udX, udY, udZ);

			//4.4.1 - 4.4.4
			pBitStreamX->WriteCompressedU8X(uACContextNormalDiffSigns,u8Signs);
			pBitStreamX->WriteCompressedU32X(uACContextNormalDiffMagX,udX);
			pBitStreamX->WriteCompressedU32X(uACContextNormalDiffMagY,udY);
			pBitStreamX->WriteCompressedU32X(uACContextNormalDiffMagZ,udZ);
		}
	}
	else
		pBitStreamX->WriteCompressedU32X(uACContextNumLocalNormals,0);


	//      4.5 U32[cPointCnt]:New Point Count
	//      4.6 New Point Info [New Point Count]

	// 4.5
	pBitStreamX->WriteCompressedU32X(uACContextNumNewFaces,writtenPoints);
	for(iPoint = 0; iPoint<writtenPoints; iPoint++)
	{
		PointInd = newPoints.GetElement(iPoint);

		//  4.6.1 U32[cShading]  Shading ID
		// Get the material id for the Point and write it
		U32 uMaterialID;
		m_pAuthorPointSet->GetPointMaterial(PointInd,&uMaterialID);
		IFXASSERT(uMaterialID<m_pPointSetDescription->m_numMaterials);
		pBitStreamX->WriteCompressedU32X(uACContextPointShadingID,uMaterialID);

		//  4.6.2 U32[cNormlIndx] normal local index
		pBitStreamX->WriteCompressedU32X(uACContextNormalLocalIndex, iPoint);

		// prepare for writing colors
		IFXAuthorMaterial currMaterial;
		U32 currDiffColorInd, currSpecColorInd;

		IFXCHECKX(m_pAuthorPointSet->GetMaterial(uMaterialID, &currMaterial));

		if (currMaterial.m_uDiffuseColors) {
			GetPointAttribIndexes(PointInd, DIFFUSECOLOR, currDiffColorInd);
		}
		if (currMaterial.m_uSpecularColors) {
			GetPointAttribIndexes(PointInd, SPECULARCOLOR, currSpecColorInd);
		}

		//  4.6.3 New Point Diffuse Colors (presented only if Shading uses diffuse color)
		//  4.6.4 New Point Specular Colors (presented only if Shading uses specular color)

		U8 u8DupAttribPointIndexFlag = 0;
		IFXVector4 v4PredictedVertColor(0.f, 0.f, 0.f, 0.f), v4VertColorDifference, v4VertColor;
		U32 udR, udG, udB, udA;

		if (currMaterial.m_uDiffuseColors) {

			u8DupAttribPointIndexFlag = 1;

			if (m_uLastDiffuseColorInd != currDiffColorInd) {

				m_pAuthorPointSet->GetDiffuseColor(currDiffColorInd,&v4VertColor);
				if ( (int)splitPosInd>=0 )
					CalculatePredictedColorAtSplitPosX(TRUE, currPosInd, splitPosInd, v4PredictedVertColor);
				m_uCurrentNumDiffuseColors++;
				u8DupAttribPointIndexFlag = 0;
				m_uLastDiffuseColorInd = currDiffColorInd;
			}

			// Diffuse Duplicate Flag
			pBitStreamX->WriteCompressedU8X(uACContextVertColorDupType,u8DupAttribPointIndexFlag);
			if (u8DupAttribPointIndexFlag==0) {
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

		if (currMaterial.m_uSpecularColors) {

			u8DupAttribPointIndexFlag = 1;

			if (m_uLastSpecularColorInd != currSpecColorInd) {

				m_pAuthorPointSet->GetSpecularColor(currSpecColorInd,&v4VertColor);
				if ( (int)splitPosInd>=0 )
					CalculatePredictedColorAtSplitPosX(FALSE, currPosInd, splitPosInd, v4PredictedVertColor);
				m_uCurrentNumSpecularColors++;
				u8DupAttribPointIndexFlag = 0; // new color will be added
				m_uLastSpecularColorInd = currSpecColorInd;
			}

			// Specular Duplicate Flag
			pBitStreamX->WriteCompressedU8X(uACContextVertColorDupType,u8DupAttribPointIndexFlag);
			if (u8DupAttribPointIndexFlag==0) {
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

		if (m_pPointSetDescription->m_numTexCoords) {
			IFXASSERT(IFX_MAX_TEXUNITS>=currMaterial.m_uNumTextureLayers);
			U32 currTexCoordInd;
			IFXVector4  texVector( 0.f, 0.f, 0.f, 0.f), v4PredictedTexCoord( 0.f, 0.f, 0.f, 0.f);
			IFXVector4  v4VertTexCoordDifference;
			U32 iTex;
			for ( iTex = 0 ; iTex < currMaterial.m_uNumTextureLayers; iTex++ ) {
				IFXCHECKX(m_pAuthorPointSet->GetTexPoint(iTex, currPosInd, &currTexCoordInd));

				u8DupAttribPointIndexFlag = 1;

				if (m_uLastTexCoordInd != currTexCoordInd) {

					m_pAuthorPointSet->GetTexCoord(currTexCoordInd, &texVector);
					CalculatePredictedTexCoordAtSplitPosX(iTex, currPosInd, splitPosInd, v4PredictedTexCoord);
					m_uCurrentNumTexCoord++;
					u8DupAttribPointIndexFlag = 0; // new tex coord will be added
					m_uLastTexCoordInd = currTexCoordInd;
				}

				pBitStreamX->WriteCompressedU8X(uACContextTexCoordDupType, u8DupAttribPointIndexFlag);
				if (u8DupAttribPointIndexFlag==0) {
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

	m_uPositionsWritten++;
	return writtenPoints;
}

void CIFXPointSetEncoder::PrepareForWriting()
{
	IFXVector3* normals = NULL;
	IFXASSERT(m_pAuthorPointSet);

	IFXRESULT iRes = m_pAuthorPointSet->GetNormals(&normals);
	if (IFXSUCCESS(iRes)) 
	{
		U32 ind;
		for ( ind = 0; ind < (m_pAuthorPointSet->GetPointSetDesc())->m_numNormals; ind++ ) 
		{
			normals[ind].Normalize();
		}
	} 
}

void CIFXPointSetEncoder::GetPointsAtPosition(U32 posInd, IFXArray<U32>& Points)
{
	IFXASSERT(m_pAuthorPointSet);
	if (-1>=(int)posInd) {
		Points.Clear();
		return;
	}
	U32* posPoints;
	m_pAuthorPointSet->GetPositionPoints(&posPoints);
	U32 iPos;
	for ( iPos = 0; iPos< m_pAuthorPointSet->GetPointSetDesc()->m_numPoints; iPos++) {
		if (posInd == posPoints[iPos] ) {
			Points.CreateNewElement() = posPoints[iPos];
		}
	}
}

void CIFXPointSetEncoder::QuantizePositionForWrite(IFXVector3& pos, U8& u8Signs, U32& udX, U32& udY, U32& udZ) {
	u8Signs = (pos.X() < 0.0) | ((pos.Y() < 0.0) << 1) | ((pos.Z() < 0.0) << 2);
	udX = (U32) (0.5f + m_fQuantPosition * fabs(pos.X()));
	udY = (U32) (0.5f + m_fQuantPosition * fabs(pos.Y()));
	udZ = (U32) (0.5f + m_fQuantPosition * fabs(pos.Z()));
}

void CIFXPointSetEncoder::QuantizeNormalForWrite(IFXVector3& pos, U8& u8Signs, U32& udX, U32& udY, U32& udZ) {
	u8Signs = (pos.X() < 0.0) | ((pos.Y() < 0.0) << 1) | ((pos.Z() < 0.0) << 2);
	udX = (U32) (0.5f + m_fQuantNormal * fabs(pos.X()));
	udY = (U32) (0.5f + m_fQuantNormal * fabs(pos.Y()));
	udZ = (U32) (0.5f + m_fQuantNormal * fabs(pos.Z()));
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
		IFXTRACE_GENERIC(L"\t Point position:  %i\n", point);
		if (pPSDesc->m_numNormals>0) {
			IFXCHECKX(m_pAuthorPointSet->GetNormalPoint(j, &point));
			IFXTRACE_GENERIC(L"\t Point normal:   %i \n", point);

			m_pAuthorPointSet->GetPosition(point, &pnt);
			IFXTRACE_GENERIC(L"\t Normal vector: %f  %f  %f\n", pnt.X(), pnt.Y(), pnt.Z() );

		}
		if (pPSDesc->m_numDiffuseColors>0) {
			IFXCHECKX(m_pAuthorPointSet->GetDiffusePoint(j, &point));
			IFXTRACE_GENERIC(L"\t Point Diffuse colors:   %i  \n", point);
		}
		if (pPSDesc->m_numSpecularColors>0) {
			IFXCHECKX(m_pAuthorPointSet->GetSpecularPoint(j, &point));
			IFXTRACE_GENERIC(L"\t Point Specular colors:  %i  \n", point);
		}
		if (pPSDesc->m_numMaterials>0) {
			U32 uMaterial;
			IFXCHECKX(m_pAuthorPointSet->GetPointMaterial(j, &uMaterial));
			IFXTRACE_GENERIC(L"\t Point material:  %i  \n", uMaterial);
		}
	}

	IFXVector3 vert;
	for (j=0; j<pPSDesc->m_numPositions; j++) {
		IFXTRACE_GENERIC(L" Position index: %i", j);

		IFXCHECKX(m_pAuthorPointSet->GetPosition(j, &vert));
		IFXTRACE_GENERIC(L"\t    %f  %f  %f\n", vert.X(), vert.Y(), vert.Z());
	}
	for (j=0; j<pPSDesc->m_numNormals; j++) {
		IFXTRACE_GENERIC(L" Normal index: %i\n", j);

		IFXCHECKX(m_pAuthorPointSet->GetNormal(j, &vert));
		IFXTRACE_GENERIC(L"\t    %f  %f  %f\n", vert.X(), vert.Y(), vert.Z());
	}
}

#endif
