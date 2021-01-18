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
	@file	CIFXAuthorCLODResource.cpp
*/

#include "CIFXAuthorCLODResource.h"
#include "IFXNeighborMesh.h"
//#include "IFXMesh.h"
#include "IFXModifierDataPacket.h"
#include "IFXDids.h"
#include "IFXCoreCIDs.h"
#include "IFXDids.h"
#include "IFXExportingCIDs.h"

CIFXAuthorCLODResource::CIFXAuthorCLODResource() :
IFXDEFINEMEMBER(m_pDataBlockQueueX)
{
	m_refCount = 0;

	m_transformDataElementIndex    = (U32)-1;
	m_meshGroupDataElementIndex    = (U32)-1;
	m_neighborMeshDataElementIndex = (U32)-1;
	m_boundSphereDataElementIndex  = (U32)-1;

	m_CLODControllerDataElementIndex  = (U32)-1;
	m_neighborResControllerDataElementIndex  = (U32)-1;
	m_bonesManagerDataElementIndex    = (U32)-1;

	m_fCreaseAngle = 2.0f;
	m_normalCreaseParameter = 0.9f; /// @todo: Tune default value
	m_normalUpdateParameter = 0.5f; /// @todo: Tune default value
	m_normalTolerance = 0.985f;

	m_pAuthorMesh = NULL;
	m_pAuthorMeshMap = NULL;

	m_pRenderMeshMap = NULL;

	m_bMeshGroupDirty = FALSE;
	m_pMeshCompiler = NULL;

	m_pMeshGroup = NULL;
	m_pNeighborMesh = NULL;

	m_pUpdatesGroup = NULL;
	m_pCLODController = NULL;

	m_bBuiltNeighborMesh = FALSE;
	m_pNeighborResController = NULL;

	m_fCLODLevel = 1.0f;  // Resource LOD

	// These two only make sense to the CLOD modifier
	m_bCLODAuto = TRUE;   // Here for backward compatibility (e.g. CLODResource)
	m_fCLODBias = 1.00f;  // Ditto

	m_pMeshSizes = NULL;
	m_NumMeshSizes = 0;

	m_transform.CreateNewElement();
	m_transform[0].Reset();

	m_pBoundSphereDataElement = NULL;

	// Bones support
	m_pBones = NULL;

	m_bExcludeNormals = FALSE;
}

CIFXAuthorCLODResource::~CIFXAuthorCLODResource()
{
	IFXRELEASE(m_pCLODController);

	if(m_pNeighborResController)
	{
		IFXDeleteNeighborResController(m_pNeighborResController);
	}

	IFXRELEASE(m_pMeshCompiler);
	IFXRELEASE(m_pAuthorMesh);
	IFXRELEASE(m_pMeshGroup);
	IFXRELEASE(m_pNeighborMesh);
	IFXRELEASE(m_pAuthorMeshMap)
	IFXRELEASE(m_pRenderMeshMap)
	IFXRELEASE(m_pBoundSphereDataElement);
	IFXRELEASE(m_pUpdatesGroup);
	IFXRELEASE(m_pBones);
}


//---------------------------------------------------------------------------
//  CIFXAuthorCLODResource_Factory
//---------------------------------------------------------------------------

IFXRESULT IFXAPI_CALLTYPE CIFXAuthorCLODResource_Factory( 
									IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) 
	{
		// Create the CIFXAuthorCLODResource component.
		CIFXAuthorCLODResource  *pComponent = new CIFXAuthorCLODResource;

		if ( pComponent ) 
		{
			pComponent->AddRef();

			rc = pComponent->QueryInterface( interfaceId, ppInterface );

			pComponent->Release();
		} 
		else 
		{
			rc = IFX_E_OUT_OF_MEMORY;
		}
	} 
	else 
	{
		rc = IFX_E_INVALID_POINTER;
	}

	return rc;
}


U32 CIFXAuthorCLODResource::AddRef()
{
	return ++m_refCount;
}


U32 CIFXAuthorCLODResource::Release()
{
	if( m_refCount == 1 )
	{
		CIFXModifier::PreDestruct();
		delete this;
		return 0;
	}
	else
		return --m_refCount;
}

IFXRESULT CIFXAuthorCLODResource::QueryInterface( 
										IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXAuthorCLODResource)
			*ppInterface = ( IFXAuthorCLODResource* ) this;
		else if(interfaceId == IID_IFXUnknown)
			*ppInterface = ( IFXUnknown* ) this;
		else if(interfaceId == IID_IFXMarker)
			*ppInterface = ( IFXMarker* ) this;
		else if(interfaceId == IID_IFXMarkerX)
			*ppInterface = ( IFXMarkerX* ) this;
		else if ( interfaceId == IID_IFXModifier )
			*ppInterface = (IFXModifier*)this;
		else if ( interfaceId == IID_IFXSubject )
			*ppInterface = (IFXSubject*)this;
		else if ( interfaceId == IID_IFXGenerator)
			*ppInterface = (IFXGenerator*)this;
		else if ( interfaceId == IID_IFXMetaDataX )
			*ppInterface = ( IFXMetaDataX* ) this;
		else
		{
			*ppInterface = NULL;
			rc = IFX_E_UNSUPPORTED;
		}
		if(IFXSUCCESS(rc))
			AddRef();

	}
	else
	{
		rc = IFX_E_INVALID_POINTER;
	}

	return rc;
}


// IFXMarker
IFXRESULT CIFXAuthorCLODResource::SetSceneGraph( IFXSceneGraph* pInSceneGraph )
{
	IFXRESULT result = IFX_OK;

	if (!m_pBoundSphereDataElement)
		result = IFXCreateComponent(
							CID_IFXBoundSphereDataElement, 
							IID_IFXBoundSphereDataElement, 
							(void**) &m_pBoundSphereDataElement);
	if ( IFXSUCCESS(result) )
		result = CIFXMarker::SetSceneGraph(pInSceneGraph);

	return result;
}

// IFXMarkerX
void CIFXAuthorCLODResource::GetEncoderX(IFXEncoderX*& rpEncoderX)
{
	CIFXMarker::GetEncoderX( CID_IFXAuthorCLODEncoderX, rpEncoderX );
}

// IFXModifier

static const IFXGUID* s_scpOutputDIDsBones[] =
{
	&DID_IFXTransform,
	&DID_IFXRenderableGroupBounds,
	&DID_IFXRenderableGroup,
	&DID_IFXCLODController,
	&DID_IFXNeighborMesh,
	&DID_IFXNeighborResController,
	&DID_IFXBonesManager
};


static const IFXGUID* s_scpNeighborMeshDeps[] =
{
	&DID_IFXRenderableGroup
};


static const IFXGUID* s_scpCLODControllerOutputDepDIDs[] =
{
	&DID_IFXRenderableGroup
};


static const U32 s_scuCLODControllerOutputDepAttrs[] =
{
	IFX_MESHGROUP_CB_NUMMESHES
		| IFX_MESHGROUP_CB_NORMALS
		| IFX_MESHGROUP_CB_VERTEXCOUNT
		| IFX_MESHGROUP_CB_FACECOUNT
};


static const IFXGUID* s_scpNeighborResControllerOutputDepDIDs[] =
{
	&DID_IFXNeighborMesh,
	&DID_IFXCLODController,
};


IFXRESULT CIFXAuthorCLODResource::GetOutputs( IFXGUID**& rpOutOutputs,
											  U32&       rOutNumberOfOutputs,
											  U32*&      rpOutOutputDepAttrs )
{
	IFXRESULT result = IFX_OK;

	{
		rOutNumberOfOutputs  = sizeof(s_scpOutputDIDsBones)/sizeof(IFXGUID*);
		rpOutOutputs         = (IFXGUID**)&s_scpOutputDIDsBones;
	}

	rpOutOutputDepAttrs = NULL;

	return result;
}

void CIFXAuthorCLODResource::GetDataBlockQueueX(IFXDataBlockQueueX*& rpDataBlockQueueX)
{
	rpDataBlockQueueX = m_pDataBlockQueueX;
	if(rpDataBlockQueueX) 
	{
		rpDataBlockQueueX->AddRef();
	}
}

IFXRESULT CIFXAuthorCLODResource::GetDependencies(
										IFXGUID*   pInOutputDID,
										IFXGUID**& rppOutInputDependencies,
										U32&       rOutNumberInputDependencies,
										IFXGUID**& rppOutOutputDependencies,
										U32&       ruOutNumberOfOutputDependencies,
										U32*&      rpOutOutputDepAttrs )
{
	IFXRESULT result = IFX_OK;

	if ( (pInOutputDID == &DID_IFXRenderableGroup)
		|| (pInOutputDID == &DID_IFXTransform))
	{
		rppOutInputDependencies = NULL;
		rOutNumberInputDependencies = 0;
		rppOutOutputDependencies = NULL;
		ruOutNumberOfOutputDependencies = 0;
		rpOutOutputDepAttrs = NULL;
	}
	else if ( pInOutputDID == &DID_IFXCLODController )
	{
		rppOutInputDependencies = NULL;
		rOutNumberInputDependencies = 0;

		rppOutOutputDependencies = (IFXGUID**)&s_scpCLODControllerOutputDepDIDs;
		ruOutNumberOfOutputDependencies = 
			sizeof(s_scpCLODControllerOutputDepDIDs)/sizeof(IFXGUID*);
		rpOutOutputDepAttrs = (U32*)&s_scuCLODControllerOutputDepAttrs;
	}
	else if (pInOutputDID == &DID_IFXRenderableGroupBounds)
	{
		rppOutInputDependencies = NULL;
		rOutNumberInputDependencies = 0;
		rppOutOutputDependencies = NULL;
		ruOutNumberOfOutputDependencies = 0;
		rpOutOutputDepAttrs = NULL;
	}
	else if (pInOutputDID == &DID_IFXNeighborMesh)
	{
		rppOutInputDependencies = NULL;
		rOutNumberInputDependencies = 0;
		rppOutOutputDependencies = (IFXGUID**)s_scpNeighborMeshDeps;
		ruOutNumberOfOutputDependencies = sizeof(s_scpNeighborMeshDeps) / sizeof(IFXGUID*);
		rpOutOutputDepAttrs = NULL;
	}
	else if ( pInOutputDID == &DID_IFXNeighborResController )
	{
		rppOutInputDependencies = NULL;
		rOutNumberInputDependencies = 0;

		rppOutOutputDependencies = (IFXGUID**)&s_scpNeighborResControllerOutputDepDIDs;
		ruOutNumberOfOutputDependencies = 
			sizeof(s_scpNeighborResControllerOutputDepDIDs)/sizeof(IFXGUID*);
		rpOutOutputDepAttrs = NULL;
	}
	else if ( pInOutputDID == &DID_IFXBonesManager )
	{
		rppOutInputDependencies = NULL;
		rOutNumberInputDependencies = 0;
		rppOutOutputDependencies = NULL;
		ruOutNumberOfOutputDependencies = 0;
		rpOutOutputDepAttrs = NULL;
	}
	else
	{
		IFXASSERTBOX( 1, "CIFXAuthorCLODResource::GetDependencies() called with \
						 unknown output!" );
		result = IFX_E_UNDEFINED;
	}

	return result;
}

IFXRESULT CIFXAuthorCLODResource::GenerateOutput( U32    inOutputDataElementIndex,
												 void*& rpOutData, BOOL& rNeedRelease )
{
	IFXRESULT result = IFX_OK;

	if ( inOutputDataElementIndex == m_meshGroupDataElementIndex )
	{   // Cases
		// No AuthorMesh - Error
		// AuthorMesh and No MeshGroup -> BuildMeshGroup -> InitCompile or PreviewCompile
		// AuthorMesh and MeshGroup (Dirty) -> Stream Compile
		// AuthorMesh and MeshGroup (OK) -> Change Res
		if(!m_pAuthorMesh)
		{
			result = IFX_E_NOT_INITIALIZED;
		}

		if( IFXSUCCESS(result) && (!m_pMeshGroup || m_bMeshGroupDirty))
		{
			result = BuildMeshGroup();
		}

		if ( IFXSUCCESS(result) && m_pMeshGroup && m_pCLODController )
		{

			/** 
				@todo	CLOD Calculations are wrong for streaming here, lod, and CLODResource. 
						Change to clamped, scaled, finalMax
			*/

			m_pCLODController->SetResolution(
				(U32)((F32)m_pCLODController->GetMaxResolution() * m_fCLODLevel));
			m_pMeshGroup->QueryInterface( IID_IFXUnknown, &rpOutData );
			rNeedRelease = TRUE;
		}
		else
		{
			result = IFX_E_NOT_INITIALIZED;
		}
	}
	else if ( inOutputDataElementIndex == m_CLODControllerDataElementIndex )
	{ // Cases
		result = BuildCLODController();

		if(IFXSUCCESS(result) && m_pCLODController)
		{
			m_pCLODController->QueryInterface(IID_IFXUnknown, &rpOutData);
			rNeedRelease = TRUE;
		}
	}
	else if ( inOutputDataElementIndex == m_neighborResControllerDataElementIndex)
	{
		result = BuildNeighborResController();

		if(IFXSUCCESS(result) && m_pCLODController)
		{
			m_pNeighborResController->SetResolution(
				(U32)(m_pCLODController->GetMaxResolution() * m_fCLODLevel));
			m_pCLODController->QueryInterface(IID_IFXUnknown, &rpOutData);
			rNeedRelease = TRUE;
		}
	}
	else if ( inOutputDataElementIndex == m_transformDataElementIndex )
	{
		rpOutData = (void*)&m_transform;
	}
	else if ( inOutputDataElementIndex == m_boundSphereDataElementIndex )
	{
		/** 
			@todo	The following should be simplified using the appropriate 
					implicit function.

			@todo	This call is expensive - should be caching the bounding sphere
					from the last call to the build mesh group
		*/
		if (m_pAuthorMesh) 
		{
			m_pBoundSphereDataElement->Bound() = m_pAuthorMesh->CalcBoundSphere();
			m_pBoundSphereDataElement->QueryInterface( IID_IFXUnknown, &rpOutData );
			rNeedRelease = TRUE;
		}
	}
	else if ( inOutputDataElementIndex == m_neighborMeshDataElementIndex )
	{
		if(!m_pNeighborMesh)
		{
			result = BuildNeighborMesh();
		}

		if(m_pNeighborMesh && IFXSUCCESS(result))
		{
			result = m_pNeighborMesh->QueryInterface( IID_IFXUnknown, &rpOutData );
			rNeedRelease = TRUE;
		}
		else
		{
			result = IFX_E_NOT_INITIALIZED;
		}
	}
	else if ( inOutputDataElementIndex == m_bonesManagerDataElementIndex )
	{
		if( m_pBones )
		{
			IFXBonesManager *pBM = m_pBones->GetBonesManagerNR();
			if( pBM )
			{
				rpOutData = pBM;
			}
			else
			{
				result = IFX_E_NOT_INITIALIZED;
			}
		}
		else
		{
			result = IFX_E_NOT_INITIALIZED;
		}
	}
	else
	{
		IFXASSERTBOX( 1, "CIFXAuthorCLODResource::GetOutput() not implemented!" );
		result = IFX_E_UNDEFINED;
	}

	return result;
}


IFXRESULT CIFXAuthorCLODResource::SetDataPacket(
	IFXModifierDataPacket* pInInputDataPacket,
	IFXModifierDataPacket* pInDataPacket )
{
	IFXRESULT result = IFX_OK;

	IFXRELEASE(m_pInputDataPacket);
	IFXRELEASE(m_pModifierDataPacket);

	if(pInInputDataPacket &&  pInDataPacket)
	{
		if( IFXSUCCESS(result) )
		{
			m_pInputDataPacket = pInInputDataPacket;
			IFXADDREF(m_pInputDataPacket);
			m_pModifierDataPacket = pInDataPacket;
			IFXADDREF(m_pModifierDataPacket);
		}

		if( IFXSUCCESS(result) )
			result = pInDataPacket->GetDataElementIndex(
			DID_IFXTransform,
			m_transformDataElementIndex );

		if( IFXSUCCESS(result) )
			result = pInDataPacket->GetDataElementIndex(
			DID_IFXRenderableGroup,
			m_meshGroupDataElementIndex );

		if( IFXSUCCESS(result) )
			m_pBoundSphereDataElement->RenderableIndex() =
			m_meshGroupDataElementIndex;

		if( IFXSUCCESS(result) )
			result = pInDataPacket->GetDataElementIndex(
			DID_IFXCLODController,
			m_CLODControllerDataElementIndex );

		if( IFXSUCCESS(result) )
			result = pInDataPacket->GetDataElementIndex(
			DID_IFXNeighborMesh,
			m_neighborMeshDataElementIndex );

		if( IFXSUCCESS(result) )
			result = pInDataPacket->GetDataElementIndex(
			DID_IFXRenderableGroupBounds,
			m_boundSphereDataElementIndex );

		if( IFXSUCCESS(result) /*&& m_pBones*/ )
			result = pInDataPacket->GetDataElementIndex(
			DID_IFXBonesManager,
			m_bonesManagerDataElementIndex );

	}
	return result;
}


IFXRESULT CIFXAuthorCLODResource::Notify( IFXModifierMessage eInMessage,
										  void*              pMessageContext )
{
	IFXRESULT result = IFX_OK;

	return result;
}


// IFXAuthorCLODResource
IFXRESULT CIFXAuthorCLODResource::GetAuthorMesh(IFXAuthorCLODMesh*& rpAuthorCLODMesh)
{
	IFXRESULT rc = IFX_OK;

	if(m_pAuthorMesh)
	{
		m_pAuthorMesh->AddRef();
	}
	rpAuthorCLODMesh = m_pAuthorMesh;

	IFXRETURN(rc);
}

IFXRESULT CIFXAuthorCLODResource::SetAuthorMesh(IFXAuthorCLODMesh* pAuthorCLODMesh)
{
	IFXRESULT rc = IFX_OK;

	if(m_pAuthorMesh != pAuthorCLODMesh)
	{
		ClearMeshGroup();
	}

	if(pAuthorCLODMesh)
	{
		pAuthorCLODMesh->AddRef();
	}

	IFXRELEASE(m_pAuthorMesh);
	m_pAuthorMesh = pAuthorCLODMesh;

	m_bMeshGroupDirty = TRUE;

	if(m_pModifierDataPacket) 
	{
		m_pModifierDataPacket->InvalidateDataElement(m_meshGroupDataElementIndex);
		m_pModifierDataPacket->InvalidateDataElement(m_boundSphereDataElementIndex);
	}

	IFXRETURN(rc);
}

IFXRESULT CIFXAuthorCLODResource::GetAuthorMeshMap(IFXMeshMap **ppAuthorMeshMap)
{
	IFXRESULT rc = IFX_OK;

	if (ppAuthorMeshMap)
	{
		if(m_pAuthorMeshMap)
		{
			m_pAuthorMeshMap->AddRef();
		}
		else
		{
			rc = IFX_E_NOT_INITIALIZED;
		}
		*ppAuthorMeshMap = m_pAuthorMeshMap;
	}
	else
		rc = IFX_E_INVALID_POINTER;

	IFXRETURN(rc);
}

IFXRESULT CIFXAuthorCLODResource::SetAuthorMeshMap(IFXMeshMap *pAuthorMeshMap)
{
	IFXRESULT rc = IFX_OK;

	if(pAuthorMeshMap)
	{
		pAuthorMeshMap->AddRef();

		IFXRELEASE(m_pAuthorMeshMap);

		m_pAuthorMeshMap = pAuthorMeshMap;
	}
	else
		rc = IFX_E_INVALID_POINTER;

	IFXRETURN(rc);
}

IFXRESULT CIFXAuthorCLODResource::GetRenderMeshMap(IFXMeshMap **ppRenderMeshMap)
{
	IFXRESULT rc = IFX_OK;

	if (ppRenderMeshMap)
	{
		// Make sure we have the mesh compiled before we go looking for
		// the render mesh map.
		if(!m_pMeshGroup || m_bMeshGroupDirty)
		{
			rc = BuildMeshGroup();
		}

		if (IFXSUCCESS(rc))
		{
			if(m_pRenderMeshMap)
			{
				m_pRenderMeshMap->AddRef();
			}
			else
				rc = IFX_E_NOT_INITIALIZED;
		}

		*ppRenderMeshMap = m_pRenderMeshMap;
	}
	else
		rc = IFX_E_INVALID_POINTER;

	IFXRETURN(rc);
}

IFXRESULT CIFXAuthorCLODResource::SetRenderMeshMap(IFXMeshMap *pRenderMeshMap)
{
	IFXRESULT rc = IFX_OK;

	if(pRenderMeshMap)
	{
		pRenderMeshMap->AddRef();

		IFXRELEASE(m_pRenderMeshMap);

		m_pRenderMeshMap = pRenderMeshMap;
	}
	else
		rc = IFX_E_INVALID_POINTER;

	IFXRETURN(rc);
}

IFXRESULT CIFXAuthorCLODResource::SetAuthorMeshFinal(IFXAuthorCLODMesh* pAuthorCLODMesh)
{
	IFXRESULT rc = SetAuthorMesh(pAuthorCLODMesh);

	IFXRETURN(rc);
}


IFXRESULT CIFXAuthorCLODResource::GetCreaseAngle(F32& rCreaseAngle)
{
	IFXRESULT rc = IFX_OK;
	rCreaseAngle = m_fCreaseAngle;
	IFXRETURN(rc);
}

IFXRESULT CIFXAuthorCLODResource::SetCreaseAngle(F32 creaseAngle)
{
	IFXRESULT rc = IFX_OK;
	m_fCreaseAngle = creaseAngle;
	IFXRETURN(rc);
}

void CIFXAuthorCLODResource::GetNormalCreaseParameter(F32& rNormalCreaseParameter)
{
	rNormalCreaseParameter = m_normalCreaseParameter;
}

void CIFXAuthorCLODResource::SetNormalCreaseParameter(F32 normalCreaseParameter)
{
	m_normalCreaseParameter = normalCreaseParameter;
}

void CIFXAuthorCLODResource::GetNormalUpdateParameter(F32& rNormalUpdateParameter)
{
	rNormalUpdateParameter = m_normalUpdateParameter;
}

void CIFXAuthorCLODResource::SetNormalUpdateParameter(F32 normalUpdateParameter)
{
	m_normalUpdateParameter = normalUpdateParameter;
}

void CIFXAuthorCLODResource::GetNormalTolerance(F32& rNormalTolerance)
{
	rNormalTolerance = m_normalTolerance;
}

void CIFXAuthorCLODResource::SetNormalTolerance(F32 normalTolerance)
{
	IFXASSERT(normalTolerance >= -1.0f);
	IFXASSERT(normalTolerance <= 1.0f);
	m_normalTolerance = normalTolerance;
}

IFXRESULT CIFXAuthorCLODResource::SetCLODLevel( F32 inCLODRatio )
{
	IFXRESULT result = IFX_OK ;

	if (( inCLODRatio >= 0 )&&( inCLODRatio <= 1 ))
	{
		m_fCLODLevel = inCLODRatio;
		if(m_pModifierDataPacket)
		{
			m_pModifierDataPacket->InvalidateDataElement(m_meshGroupDataElementIndex);
		}
	}
	else
		result =  IFX_E_INVALID_RANGE;

	return result;
}


IFXRESULT CIFXAuthorCLODResource::Transfer()
{
	IFXRESULT rc = IFX_OK;
	IFXRETURN(rc);
}


// ---------------------------------------------------------------
// Private Methods n stuff
// ---------------------------------------------------------------
void CIFXAuthorCLODResource::ClearMeshGroup()
{
	if(m_pMeshGroup)
	{
		IFXRELEASE(m_pMeshCompiler)
		IFXRELEASE(m_pMeshGroup)
		IFXRELEASE(m_pAuthorMeshMap);
		IFXRELEASE(m_pRenderMeshMap);
		if(m_pModifierDataPacket)
		{
			m_pModifierDataPacket->InvalidateDataElement(m_meshGroupDataElementIndex);
		}
		ClearCLODManager();
		ClearNeighborResController();
	}
}


void CIFXAuthorCLODResource::ClearCLODManager()
{
	if(m_pCLODController)
	{
		IFXRELEASE(m_pCLODController);
		m_pCLODController = NULL;
		if(m_pModifierDataPacket)
		{
			m_pModifierDataPacket->InvalidateDataElement(
										m_meshGroupDataElementIndex);
			m_pModifierDataPacket->InvalidateDataElement(
										m_CLODControllerDataElementIndex);
		}
	}
}

void CIFXAuthorCLODResource::ClearNeighborResController()
{
	if(m_pCLODController)
	{
		IFXRELEASE(m_pCLODController);
		m_pCLODController = NULL;
		if(m_pModifierDataPacket)
		{
			m_pModifierDataPacket->InvalidateDataElement(
										m_neighborResControllerDataElementIndex);
			m_pModifierDataPacket->InvalidateDataElement(
										m_neighborMeshDataElementIndex);
		}
	}
}


IFXRESULT CIFXAuthorCLODResource::BuildDataBlockQueue()
{
	IFXRESULT result =  IFX_OK;

	// create a datablock queue
	IFXRELEASE( m_pDataBlockQueueX );
	result = IFXCreateComponent( 
					CID_IFXDataBlockQueueX, IID_IFXDataBlockQueueX,
					(void**)&m_pDataBlockQueueX );

	return result;
}


IFXRESULT CIFXAuthorCLODResource::BuildMeshGroup()
{
	IFXRESULT result =  IFX_OK;

	if(!m_pMeshGroup)
	{ // this means we are starting a stream or or are doing a static compile
		IFXRELEASE(m_pMeshCompiler);
		result = IFXCreateComponent(CID_IFXMeshCompiler, IID_IFXMeshCompiler,
			(void**)&m_pMeshCompiler);
		if(IFXSUCCESS(result))
		{
			m_pMeshCompiler->SetCosMaxNormalError(m_normalTolerance);
			/// @todo: Configure the compiler

			if(!m_pMeshSizes) // no Mesh sizes must need to do a static compile
			{
				result = m_pMeshCompiler->PreviewCompile(m_pAuthorMesh);

				// Save the Mesh group and the mesh sizes
				if(IFXSUCCESS(result))
				{
					IFXRELEASE(m_pMeshGroup);
					m_pMeshGroup = m_pMeshCompiler->GetMeshGroup();

#if 0
					{
						// DEBUG
						IFXVector3Iter piter, niter;
						IFXFaceIter fiter;
						IFXMesh* pMesh = NULL;
						m_pMeshGroup->GetMesh(0,pMesh);

						pMesh->GetFaceIter(fiter);

						IFXFace* pFace = fiter.Get();

						pMesh->GetPositionIter(piter);
						pMesh->GetNormalIter(niter);

						IFXVector3 posA, posB, posC;
						IFXVector3 norA, norB, norC;

						posA = *(piter.Index(pFace->VertexA()));
						posB = *(piter.Index(pFace->VertexB()));
						posC = *(piter.Index(pFace->VertexC()));

						norA = *(niter.Index(pFace->VertexA()));
						norB = *(niter.Index(pFace->VertexB()));
						norC = *(niter.Index(pFace->VertexC()));
					}
#endif

					IFXRELEASE(m_pUpdatesGroup);
					m_pUpdatesGroup = m_pMeshCompiler->GetUpdatesGroup();

					IFXRELEASE(m_pRenderMeshMap);
					m_pRenderMeshMap = m_pMeshCompiler->GetMeshMap();

					result = m_pMeshCompiler->GetIFXMeshSizes(
														&m_NumMeshSizes,
														&m_pMeshSizes);

					m_bMeshGroupDirty = FALSE;

				}
				IFXRELEASE(m_pMeshCompiler);
			}
			else // lets start a streaming compile
			{
				result = m_pMeshCompiler->InitCompile(m_pAuthorMesh,
					m_NumMeshSizes, m_pMeshSizes);
				if(IFXSUCCESS(result))
				{
					result = m_pMeshCompiler->Compile();
					m_bMeshGroupDirty = FALSE;
				}

				m_pMeshGroup = m_pMeshCompiler->GetMeshGroup();
			}
			/** @todo: make be done on demand
			only build when needed -- get a request for generateOutput for a
			clod controller.  Must be here in case someone sets the mesh
			resolution - we aren't detecting this happening.
			*/
			if(IFXSUCCESS(result))
			{
				result = BuildCLODController();
			}
		}
	}  // we have no mesh group
	else
	{  // we have a mesh group - a new block of the mesh just streamed in.
		if(m_pMeshCompiler && m_bMeshGroupDirty)
		{
			result = m_pMeshCompiler->Compile();

			if(result == IFX_W_FINISHED)
			{
				IFXRELEASE(m_pMeshCompiler);
			}

			m_bMeshGroupDirty = FALSE;
		}
		else
		{
			// The author mesh has been reset, either due to replacement
			// during streaming, or due to placing the encoded mesh back
			// into the resource as a result of its being encoded.
			// This behavior is really expensive, and may be behind
			// our slow loads.
			IFXRELEASE(m_pMeshCompiler);
			result = IFXCreateComponent(CID_IFXMeshCompiler,
				IID_IFXMeshCompiler, (void**)&m_pMeshCompiler);

			/// @todo: Configure the compiler

			if(IFXSUCCESS(result))
			{
				m_pMeshCompiler->SetCosMaxNormalError(m_normalTolerance);
				result = m_pMeshCompiler->PreviewCompile(m_pAuthorMesh);
			}

			// Save the Mesh group and the mesh sizes
			if(IFXSUCCESS(result))
			{
				IFXRELEASE(m_pMeshGroup);
				m_pMeshGroup = m_pMeshCompiler->GetMeshGroup();

#if 0
				{
					// DEBUG
					IFXVector3Iter piter, niter;
					IFXFaceIter fiter;
					IFXMesh* pMesh = NULL;
					m_pMeshGroup->GetMesh(0,pMesh);

					pMesh->GetFaceIter(fiter);

					IFXFace* pFace = fiter.Get();

					pMesh->GetPositionIter(piter);
					pMesh->GetNormalIter(niter);

					IFXVector3 posA, posB, posC;
					IFXVector3 norA, norB, norC;

					posA = *(piter.Index(pFace->VertexA()));
					posB = *(piter.Index(pFace->VertexB()));
					posC = *(piter.Index(pFace->VertexC()));

					norA = *(niter.Index(pFace->VertexA()));
					norB = *(niter.Index(pFace->VertexB()));
					norC = *(niter.Index(pFace->VertexC()));
				}
#endif

				IFXRELEASE(m_pUpdatesGroup);
				m_pUpdatesGroup = m_pMeshCompiler->GetUpdatesGroup();

				IFXRELEASE(m_pRenderMeshMap);
				m_pRenderMeshMap = m_pMeshCompiler->GetMeshMap();

				result = m_pMeshCompiler->GetIFXMeshSizes(
												&m_NumMeshSizes,
												&m_pMeshSizes);

				m_bMeshGroupDirty = FALSE;

			}
			IFXRELEASE(m_pMeshCompiler);
		}
	}
	return result;
}

//Build* should potentially do invalidation of the respective data elements
IFXRESULT CIFXAuthorCLODResource::BuildNeighborMesh()
{
	IFXRESULT result =  IFX_OK;
	if(!m_pMeshGroup || m_bMeshGroupDirty)
	{
		result = BuildMeshGroup();
	}
	if(IFXSUCCESS(result))
	{
		m_bBuiltNeighborMesh = TRUE;
		IFXRELEASE(m_pNeighborMesh)
		result =  IFXCreateComponent(
							CID_IFXNeighborMesh,
							IID_IFXNeighborMesh,
							(void**)&(m_pNeighborMesh)); }

	if (IFXSUCCESS(result))
		result = m_pNeighborMesh->Allocate(*m_pMeshGroup);

	if (IFXSUCCESS(result))
		result = m_pNeighborMesh->Build(*m_pMeshGroup, NULL);

	return result;
}


IFXRESULT CIFXAuthorCLODResource::BuildCLODController()
{
	IFXRESULT result =  IFX_OK;
	if(!m_pMeshGroup || m_bMeshGroupDirty)
	{
		result = BuildMeshGroup();
	}

	if(IFXSUCCESS(result))
	{
		IFXRELEASE(m_pCLODController);
		result = IFXCreateComponent(
						CID_IFXCLODManager, IID_IFXCLODManager, 
						(void**)&m_pCLODController);
	}

	if(IFXSUCCESS(result))
	{
		// Initialize
		result = m_pCLODController->Initialize(m_pMeshGroup, m_pUpdatesGroup);
	}

	return result;
}

IFXRESULT CIFXAuthorCLODResource::BuildNeighborResController()
{
	IFXRESULT result =  IFX_OK;
	if(!m_pNeighborMesh)
	{
		result = BuildNeighborMesh();
	}

	if(!m_pCLODController)
	{
		result = BuildCLODController();
	}

	if(IFXSUCCESS(result) && m_pMeshGroup && m_pUpdatesGroup  && m_pCLODController)
	{
		m_pNeighborResController = IFXCreateNeighborResController();
		if (!m_pNeighborResController)
			result = IFX_E_OUT_OF_MEMORY;
		else
		{
			// Initialize
			result = m_pNeighborResController->Initialize(
													m_pNeighborMesh, m_pUpdatesGroup);
		}
	}
	return result;
}


IFXCLODManager* CIFXAuthorCLODResource::GetCLODController()
{
	BuildCLODController();
	m_pCLODController->AddRef();

	return m_pCLODController;
}

IFXNeighborResControllerInterface* CIFXAuthorCLODResource::GetNeighborResController()
{
	BuildNeighborResController();
	return m_pNeighborResController;
}


IFXRESULT CIFXAuthorCLODResource::GetUpdatesGroup(IFXUpdatesGroup** out_ppUpdatesGroup)
{
	if(!out_ppUpdatesGroup)
	{
		return IFX_E_INVALID_POINTER;
	}

	IFXRESULT result = BuildCLODController();
	if(IFXSUCCESS(result))
	{
		m_pUpdatesGroup->AddRef();
		*out_ppUpdatesGroup = m_pUpdatesGroup;
	}

	return result;
}

IFXRESULT CIFXAuthorCLODResource::GetNeighborMesh(IFXNeighborMesh** out_ppNeighborMesh)
{
	if(!out_ppNeighborMesh)
	{
		return IFX_E_INVALID_POINTER;
	}

	IFXRESULT ir = BuildNeighborResController();

	if(IFXSUCCESS(ir))
	{
		*out_ppNeighborMesh = m_pNeighborMesh;
		IFXADDREF(m_pNeighborMesh);
	}

	return ir;
}


IFXRESULT CIFXAuthorCLODResource::GetMeshGroup(IFXMeshGroup** out_ppMeshGroup)
{
	IFXRESULT ir = IFX_OK;

	if(!out_ppMeshGroup)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(IFXSUCCESS(ir) && !m_pMeshGroup)
	{
		ir = BuildMeshGroup();
	}

	if(IFXSUCCESS(ir))
	{
		*out_ppMeshGroup = m_pMeshGroup;
		IFXADDREF(m_pMeshGroup);
	}

	return ir;
}


IFXRESULT CIFXAuthorCLODResource::SetTransform(const IFXMatrix4x4& tInTransform)
{
	m_transform[0] = tInTransform;
	if(m_pModifierDataPacket)
	{
		return m_pModifierDataPacket->InvalidateDataElement(
											m_transformDataElementIndex);
	}
	return IFX_OK;
}


void CIFXAuthorCLODResource::GetExcludeNormals(BOOL &bExcludeNormals)
{
	bExcludeNormals = m_bExcludeNormals;
}

void CIFXAuthorCLODResource::SetExcludeNormals(BOOL bExcludeNormals)
{
	m_bExcludeNormals = bExcludeNormals;
}


IFXRESULT CIFXAuthorCLODResource::InvalidateTransform()
{
	if(m_pModifierDataPacket)
	{
		m_pModifierDataPacket->InvalidateDataElement(m_transformDataElementIndex);
	}
	return IFX_OK;
}
