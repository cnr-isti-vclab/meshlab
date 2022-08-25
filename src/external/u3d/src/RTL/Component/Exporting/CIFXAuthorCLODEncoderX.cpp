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
//  CIFXAuthorCLODEncoderX.cpp
//
//  DESCRIPTION
//
//  NOTES
//
//***************************************************************************

#include "CIFXAuthorCLODEncoderX.h"
#include "IFXCoreCIDs.h"
#include "IFXExportingCIDs.h"
#include "IFXBitStreamCompressedX.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"
#include "IFXMetaDataX.h"
#include "IFXAutoRelease.h"
#include "IFXAuthorMeshScrub.h"
#include "IFXAuthorMeshMap.h"
#include "IFXSkeleton.h"

#include "IFXAuthorGeomCompiler.h"

const U32 CIFXAuthorCLODEncoderX::m_puNumResChangesPerGeomBlock[] = { 255, 255, 511, 1023, 2047 };
const U32 CIFXAuthorCLODEncoderX::m_uNumGeomBlockResChanges = sizeof(m_puNumResChangesPerGeomBlock) / sizeof(m_puNumResChangesPerGeomBlock[0]);

//-----------------------------------------------------------------------------
// IFXUnknown
//-----------------------------------------------------------------------------
U32 CIFXAuthorCLODEncoderX::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXAuthorCLODEncoderX::Release()
{
	if (1 == m_uRefCount) {
		delete this;
		return 0;
	}
	return --m_uRefCount;
}

IFXRESULT CIFXAuthorCLODEncoderX::QueryInterface( IFXREFIID interfaceId,
												 void** ppInterface)
{
	IFXRESULT result  = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXEncoderX )
		{
			*ppInterface = ( IFXEncoderX* )this;
		}
		else if ( interfaceId == IID_IFXUnknown )
		{
			*ppInterface = ( IFXUnknown* ) this;
		}
		else
		{
			*ppInterface = NULL;
			result = IFX_E_UNSUPPORTED;
		}

		if ( IFXSUCCESS( result ) )
			AddRef();
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

//-----------------------------------------------------------------------------
// IFXEncoderX
//-----------------------------------------------------------------------------

// Provide the encoder with a pointer to the object which is to be encoded.
void CIFXAuthorCLODEncoderX::SetObjectX(IFXUnknown &rObject)
{
	// Release any previous AuthorCLODResource
	IFXRELEASE(m_pAuthorCLODResource);

	// If possible, store the object's IFXAuthorCLODResource interface
	IFXCHECKX(rObject.QueryInterface(IID_IFXAuthorCLODResource,(void**)&m_pAuthorCLODResource));

	return;
}

// Initialize and get a reference to the core services
void CIFXAuthorCLODEncoderX::InitializeX(IFXCoreServices &rCoreServices)
{
	// Release any previous core services pointer
	// and store the new core services reference
	rCoreServices.AddRef();
	IFXRELEASE(m_pCoreServices);
	m_pCoreServices = &rCoreServices;
}

// Encode data into data blocks and place these blocks in the queue
void CIFXAuthorCLODEncoderX::EncodeX(IFXString &rName, IFXDataBlockQueueX &rDataBlockQueue, F64 units)
{
	// Verify m_pAuthorCLODResource is valid
	if(NULL == m_pAuthorCLODResource)
	{
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	if( units > 0.0f )
		m_unitScale = units;

	/** @todo:
	Define what to do with block caching
	For better memory management with large CAD models, it is nice to remove block caching
	support for here and elsewhere.
	*/

	// If a datablock queue cache exists, then simply copy blocks from that queue to
	// the priority queue passed in to this method.
	IFXDECLARELOCAL(IFXDataBlockQueueX,pResourceDataBlockQueueX);
	m_pAuthorCLODResource->GetDataBlockQueueX( pResourceDataBlockQueueX );

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
			IFXASSERT(m_pAuthorCLODResource);
			IFXDECLARELOCAL(IFXMarkerX,pMarker);
			IFXCHECKX(m_pAuthorCLODResource->QueryInterface(IID_IFXMarkerX,(void**)&pMarker));
			m_uPriorityIncrement = pMarker->GetPriority();
		}

		// Get the original author mesh from the author CLOD resource
		IFXVector3 scale;
		const IFXMatrix4x4 &matrix = m_pAuthorCLODResource->GetTransform();
		matrix.CalcTRS( NULL, NULL, &scale );
		IFXCHECKX(m_pAuthorCLODResource->GetAuthorMesh(m_pOriginalAuthorCLODMesh));
		// GetAuthorMesh returns success even if there's no mesh, so
		// validate that there really is one.
		IFXCHECKX_RESULT( m_pOriginalAuthorCLODMesh, IFX_E_INVALID_POINTER );

		// Apply scale from Author CLOD Resource transform to the mesh geometry. Do it
		// only if scale != (1.0f, 1.0f, 1.0f) or m_unitScale != 1.0f
		if( scale.X() != 1.0f || scale.Y() != 1.0f || scale.Z() != 1.0f || m_unitScale != 1.0f )
		{
			m_pOriginalAuthorCLODMesh->Lock();

			IFXVector3* pPositions = NULL;
			const IFXAuthorMeshDesc* pMaxMeshDesc = NULL;

			pMaxMeshDesc = m_pOriginalAuthorCLODMesh->GetMaxMeshDesc();
			if( NULL == pMaxMeshDesc )
				IFXCHECKX( IFX_E_NOT_INITIALIZED );

			U32 i, positionCnt = pMaxMeshDesc->NumPositions;

			IFXCHECKX( m_pOriginalAuthorCLODMesh->GetPositions( &pPositions ) );

			if( m_unitScale == 1.0f )
			{
				for( i = 0; i < positionCnt; i++ )
				{
					pPositions[i].Multiply( scale );
				}
			}
			else
			{
				F32 invScale = 1.0f/(F32)m_unitScale;

				if( scale.X() != 1.0f || scale.Y() != 1.0f || scale.Z() != 1.0f )
				{
					for( i = 0; i < positionCnt; i++ )
					{
						pPositions[i].Multiply( scale );
						pPositions[i].Scale( invScale );
					}
				}
				else
				{
					for( i = 0; i < positionCnt; i++ )
					{
						pPositions[i].Scale( invScale );
					}
				}
			}

			m_pOriginalAuthorCLODMesh->Unlock();
		}

		IFXDECLARELOCAL(IFXMeshMap,pMeshMap);
		m_pAuthorCLODResource->GetAuthorMeshMap( &pMeshMap );
		//    IFXCHECKX_RESULT( pMeshMap, IFX_E_INVALID_POINTER );

		if( NULL == pMeshMap )
		{
			// Create Author Geom Compiler to create Author Mesh Map
			IFXDECLARELOCAL(IFXAuthorGeomCompiler,pAGC);
			IFXDECLARELOCAL(IFXAuthorCLODResource,pACR);
			IFXCHECKX(IFXCreateComponent( CID_IFXAuthorGeomCompiler, IID_IFXAuthorGeomCompiler, (void **)&pAGC ));
			IFXDECLARELOCAL(IFXSceneGraph,pSG);
			IFXCHECKX( m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSG ) );
			IFXCHECKX( pAGC->SetSceneGraph( pSG ) );
			IFXCHECKX( pAGC->Compile( rName, m_pOriginalAuthorCLODMesh, &pACR, FALSE ) );

			// Get the MeshMap from the author CLOD resource
			IFXCHECKX( pACR->GetAuthorMeshMap( &pMeshMap ) );
		}

		// Calcuate quantization factors
		CalculateQuantizationFactorsX();

		PrepareForMakingBlocksX();

		// Make the model resource declaration block
		MakeAuthorCLODDeclarationBlockX(rName, rDataBlockQueue);

		// Make the static geometry block
		MakeAuthorCLODStaticGeometryBlockX(rName, rDataBlockQueue, *pMeshMap);

		MakeAuthorCLODProgressiveGeometryBlocksX(rName, rDataBlockQueue, pMeshMap);
		// Set the reconstructed author mesh
		//    IFXASSERT(m_pReconstructedAuthorCLODMesh);
		//    IFXCHECKX(m_pAuthorCLODResource->SetAuthorMeshFinal(m_pReconstructedAuthorCLODMesh));

		// Set the newly-created mesh map into the AuthorCLODResource
		IFXCHECKX(m_pAuthorCLODResource->SetAuthorMeshMap( pMeshMap ));

		// Revert scale applied to mesh during encoding
		if( scale.X() != 1.0f || scale.Y() != 1.0f || scale.Z() != 1.0f || m_unitScale != 1.0f )
		{
			m_pOriginalAuthorCLODMesh->Lock();

			if( scale.X() == 0.0f || scale.Y() == 0.0f || scale.Z() == 0.0f )
				IFXCHECKX( IFX_E_BAD_PARAM );

            IFXVector3 invScale = scale.Reciprocate();
			IFXVector3* pPositions = NULL;
			const IFXAuthorMeshDesc* pMaxMeshDesc = NULL;

			pMaxMeshDesc = m_pOriginalAuthorCLODMesh->GetMaxMeshDesc();
			if( NULL == pMaxMeshDesc )
				IFXCHECKX( IFX_E_NOT_INITIALIZED );

			U32 i, positionCnt = pMaxMeshDesc->NumPositions;

			IFXCHECKX( m_pOriginalAuthorCLODMesh->GetPositions( &pPositions ) );

			if( m_unitScale == 1.0f )
			{
				for( i = 0; i < positionCnt; i++ )
				{
					pPositions[i].Multiply( invScale );
				}
			}
			else if( scale.X() != 1.0f || scale.Y() != 1.0f || scale.Z() != 1.0f )
			{
				for( i = 0; i < positionCnt; i++ )
				{
					pPositions[i].Multiply( invScale );
					pPositions[i].Scale( (F32)m_unitScale );
				}
			}
			else
			{
				for( i = 0; i < positionCnt; i++ )
				{
					pPositions[i].Scale( (F32)m_unitScale );
				}
			}

			m_pOriginalAuthorCLODMesh->Unlock();
		}

		//IFXRELEASE( pMeshMap );
	}
}


//-----------------------------------------------------------------------------
// Factory Method
//-----------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXAuthorCLODEncoderX_Factory(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		CIFXAuthorCLODEncoderX *pComponent  = new CIFXAuthorCLODEncoderX;

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
CIFXAuthorCLODEncoderX::CIFXAuthorCLODEncoderX() :
IFXDEFINEMEMBER(m_pCoreServices),
IFXDEFINEMEMBER(m_pAuthorCLODResource),
IFXDEFINEMEMBER(m_pOriginalAuthorCLODMesh),
IFXDEFINEMEMBER(m_pReconstructedAuthorCLODMesh)
{
	m_uRefCount = 0;
	m_uPriorityIncrement = 0;
	m_uPriorityCurrent = 0;

	m_fQuantPosition = 1.0f;
	m_fQuantNormal = 1.0f;
	m_fQuantTexCoord = 1.0f;
	m_fInverseQuantPosition = 1.0f;
	m_fInverseQuantNormal = 1.0f;
	m_fInverseQuantTexCoord = 1.0f;

	m_fNormalCreaseParameter = 0.90f; /// @todo: Tune this value?
	m_fNormalUpdateParameter = -0.50f; /// @todo: Tune this value?
	m_fNormalTolerance = 0.985f;

	m_unitScale = 1.0f;
}

// Destuctor
CIFXAuthorCLODEncoderX::~CIFXAuthorCLODEncoderX()
{

}

// Calculate the quantization factors
void CIFXAuthorCLODEncoderX::CalculateQuantizationFactorsX()
{
	U32 uQualityFactor = 1000;

	m_pAuthorCLODResource->GetQualityFactorX(uQualityFactor,IFXAuthorCLODResource::POSITION_QUALITY);
	if(1000 == uQualityFactor) {
		m_fQuantPosition = (F32) pow(2.0,18.0);
	} else {
		m_fQuantPosition = (F32) pow(1.0076537604105041221998506395494,uQualityFactor+545.0);
	}

	// Scale m_fQuantPosition by the radius of the bounding sphere
	const IFXAuthorMeshDesc* pAuthorMeshDesc = m_pOriginalAuthorCLODMesh->GetMaxMeshDesc();
	IFXASSERT(pAuthorMeshDesc);
	IFXASSERT(pAuthorMeshDesc->NumPositions > 0);
	IFXCHECKX(m_pOriginalAuthorCLODMesh->Lock());
	{
		IFXVector3* pPosition = NULL;
		IFXCHECKX(m_pOriginalAuthorCLODMesh->GetPositions(&pPosition));

		// Calculate the center of the bounding sphere
		IFXVector3 vMin(pPosition[0]), vMax(pPosition[0]);
		U32 i = 0;
		U32 uNumPositions = pAuthorMeshDesc->NumPositions;
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
	}
	IFXCHECKX(m_pOriginalAuthorCLODMesh->Unlock());

	m_pAuthorCLODResource->GetQualityFactorX(uQualityFactor,IFXAuthorCLODResource::NORMAL_QUALITY);
	if(1000 == uQualityFactor) {
		m_fQuantNormal = (F32) pow(2.0,14.0);
	} else {
		m_fQuantNormal = (F32) pow(1.0048638204237854409678879459798,uQualityFactor+857.0);
	}

	m_pAuthorCLODResource->GetQualityFactorX(uQualityFactor,IFXAuthorCLODResource::TEXCOORD_QUALITY);
	if(1000 == uQualityFactor) {
		m_fQuantTexCoord = (F32) pow(2.0,14.0);
	} else {
		m_fQuantTexCoord = (F32) pow(1.0048638204237854409678879459798,uQualityFactor+857.0);
	}

	m_pAuthorCLODResource->GetQualityFactorX(uQualityFactor,IFXAuthorCLODResource::DIFFUSE_QUALITY);
	if(1000 == uQualityFactor) {
		m_fQuantDiffuseColor = (F32) pow(2.0,14.0);
	} else {
		m_fQuantDiffuseColor = (F32) pow(1.0022294514890519310704865897552,uQualityFactor+1741.0);
	}

	m_pAuthorCLODResource->GetQualityFactorX(uQualityFactor,IFXAuthorCLODResource::SPECULAR_QUALITY);
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

void CIFXAuthorCLODEncoderX::PrepareForMakingBlocksX()
{
	// The m_pReconstructedAuthorCLODMesh is allocated and the material information is filled in.

	const IFXAuthorMeshDesc* pMaxMeshDescription = m_pOriginalAuthorCLODMesh->GetMaxMeshDesc();
	U32 maxRes = m_pOriginalAuthorCLODMesh->GetMaxResolution();
	U32 minRes = m_pOriginalAuthorCLODMesh->GetMinResolution();

	IFXASSERT(pMaxMeshDescription);
	// Create the reconstructed author CLOD mesh and allocate
	{
		IFXAuthorMeshDesc samdMaxMeshDescription = *pMaxMeshDescription;
		IFXRELEASE(m_pReconstructedAuthorCLODMesh);
		IFXCHECKX(IFXCreateComponent(CID_IFXAuthorMesh,IID_IFXAuthorCLODMesh,(void**)&m_pReconstructedAuthorCLODMesh));
		IFXASSERT(m_pReconstructedAuthorCLODMesh);
		IFXCHECKX(m_pReconstructedAuthorCLODMesh->Allocate(&samdMaxMeshDescription));

		IFXCHECKX(m_pReconstructedAuthorCLODMesh->SetMaxResolution(maxRes));
		IFXCHECKX(m_pReconstructedAuthorCLODMesh->SetMinResolution(minRes));
	}

	// Material Descriptions
	{
		IFXAuthorMaterial* pAuthorMeshMaterial = NULL;
		IFXCHECKX(m_pOriginalAuthorCLODMesh->Lock());
		IFXCHECKX(m_pOriginalAuthorCLODMesh->GetMaterials(&pAuthorMeshMaterial));
		U32 i = 0;

		// Copy Material descriptions to the reconstructed mesh
		{
			IFXAuthorMaterial* pReconstructedAuthorMeshMaterial = NULL;
			CIFXAuthorMeshLocker amlReconstructedAuthorCLODMesh(m_pReconstructedAuthorCLODMesh);
			IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetMaterials(&pReconstructedAuthorMeshMaterial));
			for(i=0; i < pMaxMeshDescription->NumMaterials; i++) {
				pReconstructedAuthorMeshMaterial[i] = pAuthorMeshMaterial[i];
			}
		}

		IFXCHECKX(m_pOriginalAuthorCLODMesh->Unlock());
	}

}


// Make the model resource declaration block
void CIFXAuthorCLODEncoderX::MakeAuthorCLODDeclarationBlockX(IFXString &rName, IFXDataBlockQueueX &rDataBlockQueue)
{
	// Create a bitstream
	IFXDECLARELOCAL(IFXBitStreamX,pBitStreamX);
	IFXCHECKX(IFXCreateComponent(CID_IFXBitStreamX,IID_IFXBitStreamX,(void**)&pBitStreamX));
	IFXASSERT(pBitStreamX);

	
	//
	// Write the block info for AuthorCLODDeclaration:
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
	
	// The m_pReconstructedAuthorCLODMesh is allocated and
	// the shader information is filled in during PrepareForMakingBlocksX().

	//  1. Name
	pBitStreamX->WriteIFXStringX(rName);

	//  2. Chain Index
	pBitStreamX->WriteU32X(0);

	BOOL bExcludeNormals = FALSE;
	IFXASSERT(m_pAuthorCLODResource);
	m_pAuthorCLODResource->GetExcludeNormals(bExcludeNormals);

	// 3. Mesh attributes
	/// @todo: Renumber comments below
	if(bExcludeNormals) {
		pBitStreamX->WriteU32X(1);
	} else {
		pBitStreamX->WriteU32X(0);
	}

	//  3. Max Mesh Description
	const IFXAuthorMeshDesc* pMaxMeshDescription = m_pReconstructedAuthorCLODMesh->GetMaxMeshDesc();
	IFXASSERT(pMaxMeshDescription);

	//    3.1 U32 NumFaces;
	pBitStreamX->WriteU32X(pMaxMeshDescription->NumFaces);
	//    3.2 U32 NumPositions;
	pBitStreamX->WriteU32X(pMaxMeshDescription->NumPositions);
	//    3.3 U32 NumNormals;
	if(bExcludeNormals) {
		pBitStreamX->WriteU32X(0);
	} else {
		pBitStreamX->WriteU32X(pMaxMeshDescription->NumNormals);
	}
	//    3.4 U32 NumDiffuseColors;
	pBitStreamX->WriteU32X(pMaxMeshDescription->NumDiffuseColors);
	//    3.5 U32 NumSpecularColors;
	pBitStreamX->WriteU32X(pMaxMeshDescription->NumSpecularColors);
	//    3.6 U32 NumTexCoords;
	pBitStreamX->WriteU32X(pMaxMeshDescription->NumTexCoords);
	//    3.7 U32 NumShaders;
	pBitStreamX->WriteU32X(pMaxMeshDescription->NumMaterials);

	//    3.8 Shaders Descriptions
	{
		IFXAuthorMaterial* pAuthorMeshMaterial = NULL;
		IFXCHECKX(m_pReconstructedAuthorCLODMesh->Lock());
		IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetMaterials(&pAuthorMeshMaterial));
		U32 i = 0;

		for(i=0; i < pMaxMeshDescription->NumMaterials; i++) {
			//  3.8.1 U32 Shading Attributes (BOOL m_uDiffuseColors; BOOL m_uSpecularColors;)
			U32 uMaterialAttributes = 0;
			if(pAuthorMeshMaterial[i].m_uDiffuseColors) {
				uMaterialAttributes |= 1;
			}
			if(pAuthorMeshMaterial[i].m_uSpecularColors) {
				uMaterialAttributes |= 2;
			}
			pBitStreamX->WriteU32X(uMaterialAttributes);
			//  3.8.2 U32 m_uNumTextureLayers;
			pBitStreamX->WriteU32X(pAuthorMeshMaterial[i].m_uNumTextureLayers);
			//  3.8.3 U32 m_uNumTextureLayers[m_uNumTextureLayers]; (IFX_MAX_TEXUNITS == 8)
			IFXASSERT(IFX_MAX_TEXUNITS >= pAuthorMeshMaterial[i].m_uNumTextureLayers);
			U32 j = 0;
			for(j = 0; j < pAuthorMeshMaterial[i].m_uNumTextureLayers; j++) {
				pBitStreamX->WriteU32X(pAuthorMeshMaterial[i].m_uTexCoordDimensions[j]);
			}
			//  3.8.4 U32 OriginalShadingID;  // the mesh scrubber may have removed unused materials an repacked them.
			pBitStreamX->WriteU32X(pAuthorMeshMaterial[i].m_uOriginalMaterialID);
		}
		IFXCHECKX(m_pReconstructedAuthorCLODMesh->Unlock());
	}

	//  4. CLOD Description
	//    4.1 Min Resolution
	pBitStreamX->WriteU32X(m_pReconstructedAuthorCLODMesh->GetMinResolution());
	//    4.2 Final Max Resolution
	pBitStreamX->WriteU32X(m_pReconstructedAuthorCLODMesh->GetFinalMaxResolution());

	//  5. Extra resource info
	//    5.1 Quality Factors
	U32 uQualityFactor = 0;
	//          5.1.1 U32 Position Quality Factor
	m_pAuthorCLODResource->GetQualityFactorX(uQualityFactor,IFXAuthorCLODResource::POSITION_QUALITY);
	pBitStreamX->WriteU32X(uQualityFactor);
	//          5.1.2 U32 Normal Quality Factor
	m_pAuthorCLODResource->GetQualityFactorX(uQualityFactor,IFXAuthorCLODResource::NORMAL_QUALITY);
	pBitStreamX->WriteU32X(uQualityFactor);
	//          5.1.3 U32 Texture Coord Quality Factor
	m_pAuthorCLODResource->GetQualityFactorX(uQualityFactor,IFXAuthorCLODResource::TEXCOORD_QUALITY);
	pBitStreamX->WriteU32X(uQualityFactor);

	//    5.2 Inverse Quantization Factors
	pBitStreamX->WriteF32X(m_fInverseQuantPosition);
	pBitStreamX->WriteF32X(m_fInverseQuantNormal);
	pBitStreamX->WriteF32X(m_fInverseQuantTexCoord);
	pBitStreamX->WriteF32X(m_fInverseQuantDiffuseColor);
	pBitStreamX->WriteF32X(m_fInverseQuantSpecularColor);

	//    5.3 Resource Parameters
	m_pAuthorCLODResource->GetNormalCreaseParameter(m_fNormalCreaseParameter);
	pBitStreamX->WriteF32X(m_fNormalCreaseParameter);
	m_pAuthorCLODResource->GetNormalUpdateParameter(m_fNormalUpdateParameter);
	pBitStreamX->WriteF32X(m_fNormalUpdateParameter);
	m_pAuthorCLODResource->GetNormalTolerance(m_fNormalTolerance);
	pBitStreamX->WriteF32X(m_fNormalTolerance);

	
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

	// 3. Write the BoneCount
	U32 uBoneCount = 0;
	IFXSkeleton* pSkeleton = m_pAuthorCLODResource->GetBones();
	IFXRESULT rc = IFX_OK;

	if( pSkeleton )
	{
		while( IFXSUCCESS(rc) )
		{
			IFXBoneInfo boneInfo;
			rc = pSkeleton->GetBoneInfo(uBoneCount,&boneInfo);

			if(IFXSUCCESS(rc))
				uBoneCount++;
		} // end while()

		// we need to stomp on the error returned when we've
		// run past the existing bonecount:
		if (IFX_E_INVALID_RANGE == rc)
			rc = IFX_OK;
		else
			throw IFXException( rc );

		// 6.1. Bones Count
		pBitStreamX->WriteU32X(uBoneCount);
		IFXString sParentName;
		IFXString sEmpty(L"");

		// 6.2. Write the Bone (repeated BoneCount times)
		U32 i;
		if( m_unitScale == 0.0f )
			IFXCHECKX( IFX_E_BAD_PARAM );
		F32 invUnits = 1.0f/(F32)m_unitScale;

		for ( i = 0; i < uBoneCount; i++ )
		{
			IFXBoneInfo boneInfo;
			IFXCHECKX( pSkeleton->GetBoneInfo(i, &boneInfo) );

			// 6.2.1 BoneName
			pBitStreamX->WriteIFXStringX((boneInfo.stringBoneName));

			// 6.2.2 BoneParentName
			if( i > 0 ) // if not a root bone then write parent name...
				pBitStreamX->WriteIFXStringX(boneInfo.stringParentName);
			else // ...otherwise, write empty string
				pBitStreamX->WriteIFXStringX(sEmpty);

			// 6.2.3 BoneAttributes
			pBitStreamX->WriteU32X(boneInfo.uBoneAttributes);

			// 6.2.4 BoneLength
			pBitStreamX->WriteF32X(boneInfo.fBoneLength * invUnits);

			// 6.2.5 BoneDisplacement
			pBitStreamX->WriteF32X(boneInfo.v3BoneDisplacement.X() * invUnits);
			pBitStreamX->WriteF32X(boneInfo.v3BoneDisplacement.Y() * invUnits);
			pBitStreamX->WriteF32X(boneInfo.v3BoneDisplacement.Z() * invUnits);

			// 6.2.6 BoneRotation
			pBitStreamX->WriteF32X(boneInfo.v4BoneRotation.R());
			pBitStreamX->WriteF32X(boneInfo.v4BoneRotation.G());
			pBitStreamX->WriteF32X(boneInfo.v4BoneRotation.B());
			pBitStreamX->WriteF32X(boneInfo.v4BoneRotation.A());

			if(IFXSKELETON_LINKPRESENT & boneInfo.uBoneAttributes)
			{
				// 6.2.7 NumberLinks
				pBitStreamX->WriteU32X(boneInfo.uNumLinks);

				// 6.2.8 LinkLength
				pBitStreamX->WriteF32X(boneInfo.fLinkLength * invUnits);
			}

			if(IFXSKELETON_JOINTPRESENT & boneInfo.uBoneAttributes)
			{
				// 6.2.9 StartJoint
				pBitStreamX->WriteF32X(boneInfo.v2StartJointCenter.U() * invUnits);
				pBitStreamX->WriteF32X(boneInfo.v2StartJointCenter.V() * invUnits);
				pBitStreamX->WriteF32X(boneInfo.v2StartJointScale.U());
				pBitStreamX->WriteF32X(boneInfo.v2StartJointScale.V());

				// 6.2.10 EndJoint
				pBitStreamX->WriteF32X(boneInfo.v2EndJointCenter.U() * invUnits);
				pBitStreamX->WriteF32X(boneInfo.v2EndJointCenter.V() * invUnits);
				pBitStreamX->WriteF32X(boneInfo.v2EndJointScale.U());
				pBitStreamX->WriteF32X(boneInfo.v2EndJointScale.V());
			}

			// 6.2.11 RotationalConstraint
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
		pBitStreamX->WriteU32X(0); // write zero bone count if no skeleton
	}

	// Get the data block
	IFXDECLARELOCAL(IFXDataBlockX,pDataBlockX);
	pBitStreamX->GetDataBlockX(pDataBlockX);
	IFXASSERT(pDataBlockX);

	// Set block type (U3D)
	pDataBlockX->SetBlockTypeX( BlockType_GeneratorCLODMeshU3D );
	// Set block priority
	pDataBlockX->SetPriorityX(0);
	m_uPriorityCurrent += m_uPriorityIncrement;

	// set metadata
	IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
	IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
	pDataBlockX->QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
	m_pAuthorCLODResource->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
	pBlockMD->AppendX(pObjectMD);

	// Add block to queue
	rDataBlockQueue.AppendBlockX(*pDataBlockX);
}
