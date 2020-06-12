//***************************************************************************
//
//  Copyright (c) 1999 - 2006 Intel Corporation
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
	@file	CIFXAuthorLineSetResource.cpp
*/

#include "CIFXAuthorLineSetResource.h"
#include "IFXModifierDataPacket.h"
#include "IFXNeighborMesh.h"
#include "IFXExportingCIDs.h"
#include "IFXDids.h"

#if LINESET_DEBUG_VERBOSE
IFXRESULT VerifyRenderableData(IFXAuthorLineSet* pLineSet, IFXMeshGroup* pMeshGroup);
#endif

CIFXAuthorLineSetResource::CIFXAuthorLineSetResource() :
	IFXDEFINEMEMBER(m_pDataBlockQueueX)
{
	m_uRefCount = 0;

	m_uTransformDataElementIndex    = (U32)-1;
	m_uRenderableGroupDataElementIndex    = (U32)-1;
	m_uNeighborMeshDataElementIndex = (U32)-1;
	m_uBoundSphereDataElementIndex  = (U32)-1;

	m_uNeighborResControllerDataElementIndex  = (U32)-1;
	m_uBonesManagerDataElementIndex  = (U32)-1;

	m_pAuthorLineSet = NULL;
	m_pAuthorLineSetMap = NULL;
	m_pVertexHash = NULL;
	m_pRenderLineSetMap = NULL;
	m_pLineMap = NULL;
	m_pPositionMap = NULL;
	m_pNormalMap = NULL;
	m_pTextureMap = NULL;
	m_pDiffuseMap = NULL;
	m_pSpecularMap = NULL;
	m_pQV = NULL;
	m_bMeshGroupDirty = FALSE;
	m_pIteratorCache = NULL;
	m_pMeshGroup = NULL;
	m_pNeighborMesh = NULL;

	m_pUpdatesGroup = NULL;

	m_bBuiltNeighborMesh = FALSE;
	m_pNeighborResController = NULL;

	m_pLineSetSizes = NULL;

	m_transform.CreateNewElement();
	m_transform[0].Reset();

	m_pBoundSphereDataElement = NULL;

	m_pPositions      = NULL;
	m_pNormals      = NULL;
	m_pTextures     = NULL;
	m_pSpeculars      = NULL;
	m_pDiffuses     = NULL;
	m_pLineMaterials    = NULL;

	m_pPositionLines    = NULL;
	m_pNormalLines    = NULL;
	U32 i;
	for(i = 0; i < IFX_MAX_TEXUNITS; ++i)
	{
		m_pTextureLines[i] = NULL;
	}
	m_pSpecularLines    = NULL;
	m_pDiffuseLines   = NULL;

	// Bones support
	m_pBones = NULL;
}

CIFXAuthorLineSetResource::~CIFXAuthorLineSetResource()
{
	if(m_pNeighborResController)
	{
		IFXDeleteNeighborResController(m_pNeighborResController);
	}

	IFXRELEASE(m_pAuthorLineSet);
	IFXRELEASE(m_pMeshGroup);
	IFXRELEASE(m_pNeighborMesh);
	IFXRELEASE(m_pAuthorLineSetMap)
	IFXRELEASE(m_pRenderLineSetMap)
	IFXRELEASE(m_pBoundSphereDataElement);
	IFXRELEASE(m_pUpdatesGroup);

	IFXDELETE(m_pVertexHash);
	IFXDELETE_ARRAY(m_pLineSetSizes);
	IFXDELETE_ARRAY(m_pQV);
	IFXDELETE_ARRAY(m_pIteratorCache);
	IFXRELEASE(m_pBones);
}
//---------------------------------------------------------------------------
//  CIFXAuthorCLODResource_Factory
//---------------------------------------------------------------------------

IFXRESULT IFXAPI_CALLTYPE CIFXAuthorLineSetResource_Factory( 
												IFXREFIID interfaceId, 
												void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) 
	{
		// Create the CIFXAuthorLineSetResource component.
		CIFXAuthorLineSetResource  *pComponent = new CIFXAuthorLineSetResource;

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


U32 CIFXAuthorLineSetResource::AddRef()
{
	return ++m_uRefCount;
}


U32 CIFXAuthorLineSetResource::Release()
{
	if( 1 == m_uRefCount )
	{
		CIFXModifier::PreDestruct();
		delete this;
		return 0;
	}
	else
		return --m_uRefCount;
}

IFXRESULT CIFXAuthorLineSetResource::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXAuthorLineSetResource)
			*ppInterface = ( IFXAuthorLineSetResource* ) this;
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


//
// IFXMarker
IFXRESULT CIFXAuthorLineSetResource::SetSceneGraph( IFXSceneGraph* pInSceneGraph )
{
	IFXRESULT result = IFX_OK;

	if (!m_pBoundSphereDataElement)
		result = IFXCreateComponent(CID_IFXBoundSphereDataElement, IID_IFXBoundSphereDataElement, (void**) &m_pBoundSphereDataElement);
	if ( IFXSUCCESS(result) )
		result = CIFXMarker::SetSceneGraph(pInSceneGraph);

	return result;
}

//
// IFXMarkerX
void CIFXAuthorLineSetResource::GetEncoderX(IFXEncoderX*& rpEncoderX)
{
	CIFXMarker::GetEncoderX( CID_IFXLineSetEncoderX, rpEncoderX );
}

//
// IFXModifier
//
static const IFXGUID* s_scpOutputDIDs[] =
{
	&DID_IFXTransform,
	&DID_IFXRenderableGroupBounds,
	&DID_IFXRenderableGroup,
	&DID_IFXNeighborMesh,
	&DID_IFXNeighborResController,
	&DID_IFXBonesManager,
};

static const IFXGUID* s_scpNeighborMeshDeps[] =
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
	&DID_IFXNeighborMesh
};


IFXRESULT CIFXAuthorLineSetResource::GetOutputs( IFXGUID**& rpOutOutputs,
												U32&       rOutNumberOfOutputs,
												U32*&    rpOutOutputDepAttrs )
{
	IFXRESULT result = IFX_OK;
	rOutNumberOfOutputs = sizeof(s_scpOutputDIDs)/sizeof(IFXGUID*);
	rpOutOutputs         = (IFXGUID**)&s_scpOutputDIDs;
	rpOutOutputDepAttrs = NULL;

	return result;
}

void CIFXAuthorLineSetResource::GetDataBlockQueueX(IFXDataBlockQueueX*& rpDataBlockQueueX)
{
	rpDataBlockQueueX = m_pDataBlockQueueX;
	if(rpDataBlockQueueX) 
	{
		rpDataBlockQueueX->AddRef();
	}
}

IFXRESULT CIFXAuthorLineSetResource::GetDependencies(
	IFXGUID*   pInOutputDID,
	IFXGUID**& rppOutInputDependencies,
	U32&       rOutNumberInputDependencies,
	IFXGUID**& rppOutOutputDependencies,
	U32&       ruOutNumberOfOutputDependencies,
	U32*&	rpOutOutputDepAttrs )
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
		ruOutNumberOfOutputDependencies = 
			sizeof(s_scpNeighborMeshDeps) / sizeof(IFXGUID*);
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
		IFXASSERTBOX( 0, "CIFXAuthorLineSetResource::GetDependencies() called with \
						 unknown output!" );
		result = IFX_E_UNDEFINED;
	}

	return result;
}


IFXRESULT CIFXAuthorLineSetResource::GenerateOutput( U32    inOutputDataElementIndex,
													void*& rpOutData, BOOL& rNeedRelease )
{
	IFXRESULT result = IFX_OK;

	if ( inOutputDataElementIndex == m_uRenderableGroupDataElementIndex )
	{   // Cases
		// No AuthorMesh - Error
		// AuthorLineSet and No MeshGroup -> BuildMeshGroup -> InitCompile or Preview Compile
		// AuthorLineSet and MeshGroup (Dirty) -> Stream Compile
		// AuthorLineSet and MeshGroup (OK) -> Change Res
		if(!m_pAuthorLineSet)
		{
			result = IFX_E_NOT_INITIALIZED;
		}

		if(IFXSUCCESS(result) && (!m_pMeshGroup || m_bMeshGroupDirty))
		{
			result = BuildMeshGroup();
		}

		if ( IFXSUCCESS(result) && m_pMeshGroup )
		{
			m_pMeshGroup->QueryInterface( IID_IFXUnknown, &rpOutData );
			rNeedRelease = TRUE;
		}
		else
		{
			result = IFX_E_NOT_INITIALIZED;
		}
	}
	else if ( inOutputDataElementIndex == m_uNeighborResControllerDataElementIndex)
	{
		result = BuildNeighborResController();
	}
	else if ( inOutputDataElementIndex == m_uTransformDataElementIndex )
	{
		rpOutData = (void*)&m_transform;
	}
	else if ( inOutputDataElementIndex == m_uBoundSphereDataElementIndex )
	{
		/** @todo: this call is expensive - should be caching the bounding sphere
		from the last call to the build mesh group*/
		if( m_pAuthorLineSet && IFXSUCCESS(result))
			m_pBoundSphereDataElement->Bound() = m_pAuthorLineSet->CalcBoundSphere();

		m_pBoundSphereDataElement->QueryInterface( IID_IFXUnknown, &rpOutData );
		rNeedRelease = TRUE;
	}
	else if ( inOutputDataElementIndex == m_uNeighborMeshDataElementIndex )
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
	else if ( inOutputDataElementIndex == m_uBonesManagerDataElementIndex )
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
		IFXASSERTBOX( 0, "CIFXAuthorLineSetResource::GetOutput() not implemented!" );
		result = IFX_E_UNDEFINED;
	}

	return result;
}


IFXRESULT CIFXAuthorLineSetResource::SetDataPacket(
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
											m_uTransformDataElementIndex );

		if( IFXSUCCESS(result) )
			result = pInDataPacket->GetDataElementIndex(
											DID_IFXRenderableGroup,
											m_uRenderableGroupDataElementIndex );

		if( IFXSUCCESS(result) )
			m_pBoundSphereDataElement->RenderableIndex() =
			m_uRenderableGroupDataElementIndex;

		if( IFXSUCCESS(result) )
			result = pInDataPacket->GetDataElementIndex(
											DID_IFXNeighborMesh,
											m_uNeighborMeshDataElementIndex );

		if( IFXSUCCESS(result) )
			result = pInDataPacket->GetDataElementIndex(
											DID_IFXRenderableGroupBounds,
											m_uBoundSphereDataElementIndex );

		if( IFXSUCCESS(result) )
			result = pInDataPacket->GetDataElementIndex(
											DID_IFXBonesManager,
											m_uBonesManagerDataElementIndex );

	}
	return result;
}


//
// IFXAuthorLineSetResource
//
IFXRESULT CIFXAuthorLineSetResource::GetAuthorLineSet(IFXAuthorLineSet*& rpAuthorLineSet)
{
	IFXRESULT rc = IFX_OK;

	if(m_pAuthorLineSet)
	{
		m_pAuthorLineSet->AddRef();
	}
	rpAuthorLineSet = m_pAuthorLineSet;

	IFXRETURN(rc);
}

IFXRESULT CIFXAuthorLineSetResource::SetAuthorLineSet(IFXAuthorLineSet* pAuthorLineSet)
{
	IFXRESULT rc = IFX_OK;

	if (!pAuthorLineSet) 
	{
		IFXASSERTBOX(0, "AuthorLineSet is Zero / Not expected here!");
		return IFX_E_UNDEFINED;
	}

	if(m_pAuthorLineSet != pAuthorLineSet)
	{
		ClearMeshGroup();
	}

	if(pAuthorLineSet )
	{
		pAuthorLineSet->AddRef();
	}

	IFXRELEASE(m_pAuthorLineSet);
	m_pAuthorLineSet = pAuthorLineSet;

	m_bMeshGroupDirty = TRUE;

	if(m_pModifierDataPacket) 
	{
		m_pModifierDataPacket->InvalidateDataElement(m_uRenderableGroupDataElementIndex);
		m_pModifierDataPacket->InvalidateDataElement(m_uBoundSphereDataElementIndex);
	}

	IFXRETURN(rc);
}


IFXRESULT CIFXAuthorLineSetResource::GetAuthorMeshMap(IFXMeshMap **ppAuthorLineSetMap)
{
	IFXRESULT rc = IFX_OK;

	if (ppAuthorLineSetMap) 
	{
		if(m_pAuthorLineSetMap)	
		{
			m_pAuthorLineSetMap->AddRef();
		} else{
		        rc = IFX_E_NOT_INITIALIZED;
		}
		*ppAuthorLineSetMap = m_pAuthorLineSetMap;

	} 
	else 
	{
		rc = IFX_E_INVALID_POINTER;
	}

	IFXRETURN(rc);
}

IFXRESULT CIFXAuthorLineSetResource::SetAuthorMeshMap(IFXMeshMap *pAuthorLineSetMap)
{
	IFXRESULT rc = IFX_OK;

	if(pAuthorLineSetMap){
		pAuthorLineSetMap->AddRef();

		IFXRELEASE(m_pAuthorLineSetMap);

		m_pAuthorLineSetMap = pAuthorLineSetMap;
	} else {
		rc = IFX_E_INVALID_POINTER;
	}

	IFXRETURN(rc);
}

IFXRESULT CIFXAuthorLineSetResource::GetRenderMeshMap(IFXMeshMap** ppRenderLineSetMap)
{
	IFXRESULT rc = IFX_OK;

	if (ppRenderLineSetMap)	
	{
		// Make sure we have the mesh compiled before we go looking for
		// the render mesh map.
		if(!m_pMeshGroup || m_bMeshGroupDirty)	
		{
			rc = BuildMeshGroup();
		}

		if (IFXSUCCESS(rc))	
		{
			if(m_pRenderLineSetMap) 
			{
				m_pRenderLineSetMap->AddRef();
			}
		} 
		else 
		{
			rc = IFX_E_NOT_INITIALIZED;
		}

		*ppRenderLineSetMap = m_pRenderLineSetMap;
	} 
	else 
	{
		rc = IFX_E_INVALID_POINTER;
	}

	IFXRETURN(rc);
}

IFXRESULT CIFXAuthorLineSetResource::SetRenderMeshMap(IFXMeshMap* pRenderLineSetMap)
{
	IFXRESULT rc = IFX_OK;

	if(pRenderLineSetMap)
	{
		pRenderLineSetMap->AddRef();

		IFXRELEASE(m_pRenderLineSetMap);

		m_pRenderLineSetMap = pRenderLineSetMap;
	} 
	else 
	{
		rc = IFX_E_INVALID_POINTER;
	}

	IFXRETURN(rc);
}


IFXRESULT CIFXAuthorLineSetResource::Transfer()
{
	IFXRESULT rc = IFX_OK;
	IFXRETURN(rc);
}


// ---------------------------------------------------------------
// Private Methods n stuff
// ---------------------------------------------------------------
void CIFXAuthorLineSetResource::ClearMeshGroup()
{
	if(m_pMeshGroup)
	{
		IFXRELEASE(m_pMeshGroup)
		IFXRELEASE(m_pRenderLineSetMap);
		IFXRELEASE(m_pAuthorLineSetMap);

		IFXDELETE_ARRAY(m_pLineSetSizes);

		if(m_pModifierDataPacket)
		{
			m_pModifierDataPacket->InvalidateDataElement(m_uRenderableGroupDataElementIndex);
		}
	}
}


IFXRESULT CIFXAuthorLineSetResource::BuildDataBlockQueue()
{
	IFXRESULT result =  IFX_OK;

	// create a datablock queue
	IFXRELEASE( m_pDataBlockQueueX );
	result = IFXCreateComponent( 
						CID_IFXDataBlockQueueX, IID_IFXDataBlockQueueX,
						(void**)&m_pDataBlockQueueX );

	return result;
}


//Build* should potentially do invalidation of the respective data elements
IFXRESULT CIFXAuthorLineSetResource::BuildNeighborMesh()
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
							CID_IFXNeighborMesh, IID_IFXNeighborMesh,
							(void**)&(m_pNeighborMesh)); 
	}

	if (IFXSUCCESS(result))
		result = m_pNeighborMesh->Allocate(*m_pMeshGroup);

	if (IFXSUCCESS(result))
		result = m_pNeighborMesh->Build(*m_pMeshGroup, NULL);

	return result;
}

IFXRESULT CIFXAuthorLineSetResource::BuildNeighborResController()
{
	IFXRESULT result =  IFX_OK;

	if(!m_pNeighborMesh)
	{
		result = BuildNeighborMesh();
	}

	if(IFXSUCCESS(result) && m_pMeshGroup && m_pUpdatesGroup )
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

IFXNeighborResControllerInterface* CIFXAuthorLineSetResource::GetNeighborResController()
{
	BuildNeighborResController();
	return m_pNeighborResController;
}


IFXRESULT CIFXAuthorLineSetResource::GetUpdatesGroup(
											IFXUpdatesGroup** ppUpdatesGroup)
{
	if(!ppUpdatesGroup)
	{
		return IFX_E_INVALID_POINTER;
	}

	IFXRESULT result = IFX_OK;

	if(IFXSUCCESS(result))
	{
		m_pUpdatesGroup->AddRef();
		*ppUpdatesGroup = m_pUpdatesGroup;
	}

	return result;
}

IFXRESULT CIFXAuthorLineSetResource::GetNeighborMesh(IFXNeighborMesh** ppNeighborMesh)
{
	if(!ppNeighborMesh)
	{
		return IFX_E_INVALID_POINTER;
	}

	IFXRESULT ir = BuildNeighborResController();

	if(IFXSUCCESS(ir))
	{
		*ppNeighborMesh = m_pNeighborMesh;
		IFXADDREF(m_pNeighborMesh);
	}

	return ir;
}


IFXRESULT CIFXAuthorLineSetResource::GetMeshGroup(IFXMeshGroup** ppMeshGroup)
{
	IFXRESULT ir = IFX_OK;

	if(!ppMeshGroup)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(IFXSUCCESS(ir) && !m_pMeshGroup)
	{
		ir = BuildMeshGroup();
	}

	if(IFXSUCCESS(ir))
	{
		*ppMeshGroup = m_pMeshGroup;
		IFXADDREF(m_pMeshGroup);
	}

	return ir;
}


IFXRESULT CIFXAuthorLineSetResource::SetTransform(const IFXMatrix4x4& rTransform)
{
	m_transform[0] = rTransform;

	if(m_pModifierDataPacket)
	{
		return m_pModifierDataPacket->InvalidateDataElement(
												m_uTransformDataElementIndex);
	}

	return IFX_OK;
}


IFXRESULT CIFXAuthorLineSetResource::InvalidateTransform()
{
	if(m_pModifierDataPacket)
	{
		m_pModifierDataPacket->InvalidateDataElement(m_uTransformDataElementIndex);
	}

	return IFX_OK;
}


IFXRESULT CIFXAuthorLineSetResource::BuildMeshGroup()
{
	IFXRESULT ir =  IFX_OK;

	if (!m_pAuthorLineSet) return IFX_E_NOT_INITIALIZED;

	if(!m_pMeshGroup || m_bMeshGroupDirty)
	{ 
		// this means it has not built so far

		if(m_pLineSetSizes) 
		{
			ClearMeshGroup();
		}

		// initialize LineSetSizes

		/* 
			evaluate number of vertices at eash LineSet and put them to LineSetSizes
			exact number will be set up later
		*/

		ir = cpl_EvaluateLineSetSizes();

		if (IFXSUCCESS(ir))
		{
			// allocate Line Set Groups and LineSets there
			ir = cpl_AllocateOutput();
		} 
		else 
		{
			IFXASSERTBOX(0,"Not expected / Not implemented branch!");
		}

		if (IFXSUCCESS(ir))
		{
			// allocate LineSetMap as "helper" object for Line Set convertion
			ir = cpl_AllocateLineSetMap();
		} 
		else 
		{
			IFXASSERTBOX(0,"Not expected / Not implemented branch!");
		}

		if (IFXSUCCESS(ir))
		{
			// allocate LineSetap as "helper" object for Line Set convertion
			ir = cpl_AllocateVertexHashAndVertexDescQuery();
		} 
		else 
		{
			IFXASSERTBOX(0,"Not expected / Not implemented branch!");
		}

		// get direct pointers to AuthorLineSet data (crack open the author lineset)
		m_pAuthorLineSet->GetPositions(&m_pPositions);
		m_pAuthorLineSet->GetNormals(&m_pNormals);
		m_pAuthorLineSet->GetTexCoords(&m_pTextures);
		m_pAuthorLineSet->GetDiffuseColors(&m_pDiffuses);
		m_pAuthorLineSet->GetSpecularColors(&m_pSpeculars);
		m_pAuthorLineSet->GetPositionLines(&m_pPositionLines);
		m_pAuthorLineSet->GetNormalLines(&m_pNormalLines);
		m_pAuthorLineSet->GetDiffuseLines(&m_pDiffuseLines);
		m_pAuthorLineSet->GetSpecularLines(&m_pSpecularLines);

		// find max num texture layers
		U32 maxLayers = 0;
		U32 i;
		for( i=0;i<m_numMaterials; i++)
		{
			if(m_pMaterials[i].m_uNumTextureLayers > maxLayers)
				maxLayers = m_pMaterials[i].m_uNumTextureLayers;
		}

		for(i=0; i<maxLayers; i++)
			m_pAuthorLineSet->GetTexLines(i, &m_pTextureLines[i]);

		if (IFXSUCCESS(ir))
		{
			// proceed w/ "static" compile (there is not another compile at
			// AuthorLineSetResource for now: no LOD supproted for LineSets
			ir = cpl_Compile();
		} 
		else 
		{
			IFXASSERTBOX(0,"Not expected / Not implemented branch!");
		}

#if LINESET_DEBUG_VERBOSE
		ir = VerifyRenderableData(m_pAuthorLineSet, m_pMeshGroup);

		if ( ir != IFX_OK)  
		{
			IFXASSERTBOX(0, "Internal test BuildMeshGroup failed!");
		}
#endif

		m_bMeshGroupDirty = FALSE;
		cpl_Clean();
	}

	return ir;
}

// private
IFXRESULT CIFXAuthorLineSetResource::cpl_EvaluateLineSetSizes()
{
	IFXRESULT ir = IFX_OK;

	if (!m_pAuthorLineSet)
	{
		IFXASSERTBOX(0, "AuthorLineSet is not defined / Not expected here!");
		ir = IFX_E_UNDEFINED;
	}

	if(IFXSUCCESS(ir))
	{
		// num of Materials(Shaders) = num LineSets in Group = num of LineSetSizes

		m_numMaterials = m_pAuthorLineSet->GetMaxLineSetDesc()->m_numMaterials;
		IFXASSERT(m_numMaterials);
		m_pLineSetSizes = new IFXLineSetSize[m_numMaterials];

		// initialize Sizes by Zeros
		U32 i;
		for( i = 0; i < m_numMaterials; i++)
		{
			m_pLineSetSizes[i].m_numLines = 0;
			m_pLineSetSizes[i].m_numVertices = 0;
		}

		// count number of faces per material

		m_numLineSetLines = m_pAuthorLineSet->GetLineSetDesc()->m_numLines;
		m_pAuthorLineSet->GetLineMaterials(&m_pLineMaterials);

		for( i = 0; i < m_numLineSetLines; i++)
		{
			// material index less then number of Materials
			IFXASSERT(m_pLineMaterials[i]<m_numMaterials);
			m_pLineSetSizes[m_pLineMaterials[i]].m_numLines++;
		}

		// fill out vertex attribute info, estimate number of vertices

		m_pAuthorLineSet->GetMaterials(&m_pMaterials);

		for(i = 0; i < m_numMaterials; i++)
		{
			// only there are lines with specific material
			// Note ! : this needs to be revised if decide implement CLOD for LineSet
			if ( m_pLineSetSizes[i].m_numLines >0 ) 
			{
				m_pLineSetSizes[i].m_vertexAttributes.m_uData.m_bHasDiffuseColors = 
					m_pMaterials[i].m_uDiffuseColors;
				m_pLineSetSizes[i].m_vertexAttributes.m_uData.m_bHasSpecularColors = 
					m_pMaterials[i].m_uSpecularColors;
				m_pLineSetSizes[i].m_vertexAttributes.m_uData.m_uNumTexCoordLayers = 
					m_pMaterials[i].m_uNumTextureLayers;
				m_pLineSetSizes[i].m_vertexAttributes.m_uData.m_bHasPositions = TRUE;
				m_pLineSetSizes[i].m_vertexAttributes.m_uData.m_bHasNormals =
					m_pAuthorLineSet->GetLineSetDesc()->m_numNormals > 0? TRUE: FALSE;

				// over estimate number of vertices, then trim back later
				m_pLineSetSizes[i].m_numVertices = m_pLineSetSizes[i].m_numLines * 2 + 50;
			}
		}
	}

	return ir;
}

IFXRESULT CIFXAuthorLineSetResource::cpl_AllocateOutput()
{
	IFXRESULT ir = IFX_OK;
	IFXRELEASE(m_pMeshGroup);
	ir = IFXCreateComponent(CID_IFXMeshGroup, IID_IFXMeshGroup, (void**) &m_pMeshGroup);

	if(IFXSUCCESS(ir))
	{
		// num of Materials(Shaders) = num LineSets in Group = num of LineSetSizes
		ir = m_pMeshGroup->Allocate(m_numMaterials);
	}

	if(IFXSUCCESS(ir))
	{
		U32 i;
		for( i = 0; i < m_numMaterials; i++)
		{
			IFXMesh* pMesh = NULL;

			ir = IFXCreateComponent(CID_IFXMesh, IID_IFXMesh, (void**) &pMesh);

			if(IFXSUCCESS(ir))
			{
				ir = pMesh->Allocate(m_pLineSetSizes[i].m_vertexAttributes,
					m_pLineSetSizes[i].m_numVertices, 0);
			}

			if(IFXSUCCESS(ir))
			{
				ir = pMesh->AllocateLines(m_pLineSetSizes[i].m_numLines);
			}

			if(IFXSUCCESS(ir))
			{
				m_pMeshGroup->SetMesh(i,pMesh);
				pMesh->SetRenderableType( IFXRENDERABLE_ELEMENT_TYPE_LINESET );
				pMesh->SetNumVertices(0);
				pMesh->SetNumLines(0);    // bump this up as we fill in the data
			}

			IFXRELEASE(pMesh);
		}
	}

	return ir;
}

IFXRESULT CIFXAuthorLineSetResource::cpl_AllocateLineSetMap()
{
	IFXRESULT ir = IFX_OK;

	// allocate Render LineSet map
	if (m_pRenderLineSetMap)
		IFXRELEASE(m_pRenderLineSetMap);

	ir = IFXCreateComponent(
				CID_IFXMeshMap,	IID_IFXMeshMap, (void**)&m_pRenderLineSetMap);

	if( IFXSUCCESS( ir ) )
	{
		ir = m_pRenderLineSetMap->Allocate(m_pAuthorLineSet);
	}

	// use Face map to serve for Line map
	m_pLineMap		= m_pRenderLineSetMap->GetFaceMap();

	m_pPositionMap  = m_pRenderLineSetMap->GetPositionMap();
	m_pNormalMap	= m_pRenderLineSetMap->GetNormalMap();
	m_pTextureMap	= m_pRenderLineSetMap->GetTextureMap();
	m_pDiffuseMap	= m_pRenderLineSetMap->GetDiffuseMap();
	m_pSpecularMap	= m_pRenderLineSetMap->GetSpecularMap();

	return ir;
}

IFXRESULT CIFXAuthorLineSetResource::cpl_AllocateVertexHashAndVertexDescQuery()
{
	IFXRESULT result;

	// allocate vertex hash
	IFXDELETE(m_pVertexHash);
	m_pVertexHash = new VertexHash;
	if(m_pVertexHash == NULL) return IFX_E_OUT_OF_MEMORY;

	result = m_pVertexHash->Allocate(
								m_pAuthorLineSet->GetMaxLineSetDesc()->m_numPositions);
	if(result != IFX_OK) 
		return IFX_E_OUT_OF_MEMORY;

	IFXAuthorMaterial *pMats, *pMat;
	U32 i,j,numAttributes;
	m_pAuthorLineSet->GetMaterials(&pMats);

	m_numMaterials = m_pAuthorLineSet->GetMaxLineSetDesc()->m_numMaterials;
	IFXDELETE_ARRAY(m_pQV);
	m_pQV = new VertexDescriptor[m_numMaterials];
	if(m_pQV == NULL) return IFX_E_OUT_OF_MEMORY;

	for(i=0;i<m_numMaterials; i++)
	{
		pMat = &pMats[i];

		// the 1 is for material id which always exists, other attributes are optional
		numAttributes = 1 + (pMat->m_uNormals?1:0) + pMat->m_uNumTextureLayers +
			(pMat->m_uDiffuseColors?1:0) + (pMat->m_uSpecularColors?1:0);

		m_pQV[i].NumAttributes  = numAttributes;
		m_pQV[i].pAttributes    = new U32[numAttributes];
		if(m_pQV[i].pAttributes == NULL) return IFX_E_OUT_OF_MEMORY;

		m_pQV[i].pAttributes[0] = i;  // material id is always the first attribute
		for(j=1; j < numAttributes; j++)
		{
			// Init with bad value so we can tell if we failed to fill it in
			m_pQV[i].pAttributes[j] = IFX_BAD_GEN_INDEX32;  
		}
	}

	if(result != IFX_OK) return IFX_E_OUT_OF_MEMORY;

	IFXDELETE_ARRAY(m_pIteratorCache);
	m_pIteratorCache = new IFXVertexIter[m_numMaterials];

	if(m_pIteratorCache == NULL) 
		return IFX_E_OUT_OF_MEMORY;

	IFXMesh* pMesh = NULL;

	for(i=0; i<m_numMaterials; i++)
	{
		m_pMeshGroup->GetMesh(i, pMesh);
		pMesh->GetVertexIter(m_pIteratorCache[i]);
		IFXRELEASE(pMesh);
	}

	return result;
}

IFXRESULT CIFXAuthorLineSetResource::cpl_Compile()
{
	IFXRESULT ir = IFX_OK;

	U32       numLines, mat, line, corner, IFXLineIndex, vertIndex;
	IFXMesh*    pMesh;
	IFXLineIter   LineIter;
	IFXLine*    pIFXLine;
	IFXVertexMap* pLineMap;

	pLineMap = m_pRenderLineSetMap->GetFaceMap(); // map facs serves for line map

	numLines = m_pAuthorLineSet->GetLineSetDesc()->m_numLines;

	for(line = 0; line < numLines; line++)
	{
		mat = m_pLineMaterials[line];
		m_pMeshGroup->GetMesh(mat, pMesh);

		pMesh->GetLineIter(LineIter);

		IFXLineIndex = pMesh->GetNumLines();
		pIFXLine   = LineIter.Index(IFXLineIndex);
		// update line map
		pLineMap->AddVertex(line, mat, IFXLineIndex);

		for(corner = 0; corner < 2; corner++)
		{
			// look for existing vertex, if none create new vertex
			// returning vertIndex that is position index in the current LineSet (pLS)
			ir = cpl_FindAndBuildVertex(corner, line, mat, &vertIndex);
			if(ir != IFX_OK)
			{
				return IFX_E_OUT_OF_MEMORY;
			}
			// fill out ifxLine data
			pIFXLine->SetAtIndex(corner, vertIndex); 
		}

		ir = pMesh->SetNumLines(pMesh->GetNumLines() + 1);

		if ( !IFXSUCCESS(ir) ) 
		{
			IFXASSERT(0);
		}

		IFXRELEASE(pMesh);
	}

	return ir;
}

IFXRESULT CIFXAuthorLineSetResource::cpl_FindAndBuildVertex(
												U32 corner, U32 authorLineIndex,
												U32 mat, U32* pOutIFXIndex)
{
	IFXRESULT ir = IFX_OK;

	VertexDescriptor* pQ  = &m_pQV[mat];  //grab pre allocated query vertex
	IFXAuthorMaterial*  pMatDesc  = &m_pMaterials[mat];
	U32 layer, numDim;

	// the first one is always the material id which is already set when 
	// the query verts were allocated.
	U32 attributeIndex  = 1;  
	U32 AuthorIndex   = U32(-1);
	IFXMesh*      pMesh = NULL;

	// pQ is used at intermetiate storage for all vertex atributes being copied to
	// appropriate place of LineSet

	// fill in query vertex descriptor (was partially filled in at allocate time 
	// for this material).
	if(pMatDesc->m_uNormals)
	{
		pQ->pAttributes[attributeIndex++] = m_pNormalLines[authorLineIndex].Vertex(corner);
	}

	for(layer = 0; layer < pMatDesc->m_uNumTextureLayers; layer++)
		pQ->pAttributes[attributeIndex++] = m_pTextureLines[layer][authorLineIndex].Vertex(corner);

	if(pMatDesc->m_uDiffuseColors)
		pQ->pAttributes[attributeIndex++] = m_pDiffuseLines[authorLineIndex].Vertex(corner);

	if(pMatDesc->m_uSpecularColors)
		pQ->pAttributes[attributeIndex++] = m_pSpecularLines[authorLineIndex].Vertex(corner);

	pQ->AuthorIndex = m_pPositionLines[authorLineIndex].Vertex(corner);

	m_pMeshGroup->GetMesh(mat, pMesh);
	U32 numVerts = pMesh->GetNumVertices();
	pQ->IFXIndex = numVerts;
	// bump vertex count on output mesh.
	IFXASSERT(numVerts < m_pLineSetSizes[mat].m_numVertices);
	pMesh->SetNumVertices(numVerts + 1);

	// Fill out ifx line set vertex.

	// get the iterator for this mesh, it is pre-set already
	IFXVertexIter *vi = &(m_pIteratorCache[mat]);

	U32 IFXVertIndex = pQ->IFXIndex;
	attributeIndex = 1;  // get past 0 which is always matid;

	// vertex postion is in the Line set!
	*vi->GetPosition() = m_pPositions[ pQ->AuthorIndex ];

	// update vertex position map
	m_pPositionMap->AddVertex(pQ->AuthorIndex, mat, IFXVertIndex);

	if(pMatDesc->m_uNormals)
	{
		AuthorIndex = pQ->pAttributes[attributeIndex++];

		// vertex postion is in the Line set!
		*vi->GetNormal() = m_pNormals[ AuthorIndex ];

		// update vertex normal map
		m_pNormalMap->AddVertex(AuthorIndex, mat, IFXVertIndex);
	}

	IFXVector2 *pV2;
	IFXVector3 *pV3;
	IFXVector4 *pAuthorV4;

	for(layer = 0; layer < pMatDesc->m_uNumTextureLayers; layer++)
	{
		numDim = pMatDesc->m_uTexCoordDimensions[layer];
		AuthorIndex = pQ->pAttributes[attributeIndex++];
		if(IFXSUCCESS(m_pTextureMap->AddVertex(AuthorIndex, mat, IFXVertIndex)))
		{
			switch(numDim)
			{
			case 1:
				*vi->GetTexCoordV1(layer) = m_pTextures[ AuthorIndex ].Value(0);
				break;
			case 2:
				pV2 = vi->GetTexCoordV2(layer);
				pAuthorV4 = &m_pTextures[ AuthorIndex ];
				pV2->U() = pAuthorV4->U();
				pV2->V() = pAuthorV4->V();
				break;
			case 3:
				pV3 = vi->GetTexCoordV3(layer);
				pAuthorV4 = &m_pTextures[ AuthorIndex ];
				pV3->X() = pAuthorV4->U();
				pV3->Y() = pAuthorV4->V();
				pV3->Z() = pAuthorV4->W();
				break;
			case 4:
				*vi->GetTexCoordV4(layer) = m_pTextures[ AuthorIndex ];
				break;
			default:
				// bogus number of texture dimensions
				IFXASSERTBOX(0, "Not expected / Bogus number of tex dim!");
			}
		}
	}

	if(pMatDesc->m_uDiffuseColors)
	{
		AuthorIndex = pQ->pAttributes[attributeIndex++];
		// update diffuse color map
		if(IFXSUCCESS(m_pDiffuseMap->AddVertex(AuthorIndex, mat, IFXVertIndex)))
			*vi->GetColor(0) = m_pDiffuses[ AuthorIndex ].GetRGBA();
	}

	if(pMatDesc->m_uSpecularColors)
	{
		AuthorIndex = pQ->pAttributes[attributeIndex++];
		// update specular color map
		if(IFXSUCCESS(m_pSpecularMap->AddVertex(AuthorIndex, mat, IFXVertIndex)))
			*vi->GetColor(1) = m_pSpeculars[ AuthorIndex ].GetRGBA();
	}
	vi->Next(); // point to the next vertex.

	*pOutIFXIndex = pQ->IFXIndex;
	IFXRELEASE(pMesh);

	return ir;
}

IFXRESULT CIFXAuthorLineSetResource::cpl_Clean()
{
	IFXRESULT ir = IFX_OK;
	IFXDELETE_ARRAY(m_pLineSetSizes);
	IFXDELETE_ARRAY(m_pIteratorCache);

	return ir;
}

#if LINESET_DEBUG_VERBOSE

IFXRESULT VerifyRenderableData(IFXAuthorLineSet* pLineSet, IFXMeshGroup* pMeshGroup)
{
	IFXRESULT ir  = IFX_OK;
	U32 j,k;

	if( !pMeshGroup || !pLineSet){ ir = IFX_FALSE; return ir;}

	U32 numGroup = pMeshGroup->GetNumMeshes();
	if( numGroup != pLineSet->GetLineSetDesc()->m_numMaterials )
	{ 
		ir = IFX_FALSE; 
		return ir; 
	}

	IFXTRACE_GENERIC(L"[LineSet Test] AuthorLineSet with \
		%i lines, %i positons, %i normales, %i materials \n",
		pLineSet->GetLineSetDesc()->m_numLines,
		pLineSet->GetLineSetDesc()->m_numPositions,
		pLineSet->GetLineSetDesc()->m_numNormals,
		pLineSet->GetLineSetDesc()->m_numMaterials);


	IFXTRACE_GENERIC(L"[LineSet Test] Mesh Group with %i meshes\n",
		numGroup);

	// checking the sum of number of lines in meshgroup
	// it should be equal to number of lines at author line set
	IFXMesh* pMesh = NULL;
	U32 numLinesLS  = 0;
	BOOL isExcludeNormals = ( pLineSet->GetLineSetDesc()->m_numNormals == 0 ) ? TRUE : FALSE;

	for ( j = 0; j < numGroup; j++ )
	{
		pMeshGroup->GetMesh(j,pMesh);
		int n = pMesh->GetNumLines();
		IFXTRACE_GENERIC(L"[LineSet Test] Mesh Group %i Lines %i Positions %i\n",
			j, n, pMesh->GetNumVertices());

		IFXInterleavedData* pData = 0;
		IFXVector3Iter vPosIter;
		IFXVector3Iter vNormIter;

		ir = pMesh->GetInterleavedVertexData(pData);
		U32 uVectorNum = 0;
		pData->GetVectorIter(uVectorNum++, vPosIter);
		if(FALSE == isExcludeNormals)
			pData->GetVectorIter(uVectorNum++, vNormIter);

		//int stride = vPosIter.GetStride();
		IFXTRACE_GENERIC(L"[Mesh] Strides:  Vert %d \n",vPosIter.GetStride());

		numLinesLS = numLinesLS + n;
		IFXRELEASE(pMesh);
		IFXRELEASE(pData);
	}

	IFXTRACE_GENERIC(L"[LineSet Test] Author Line Set: Lines %i \n",
		pLineSet->GetLineSetDesc()->m_numLines);

	if (numLinesLS != pLineSet->GetLineSetDesc()->m_numLines) 
	{
		ir = IFX_FALSE;
		return ir;
	}

	// AuthorLineSet
	F32 dist  = 0.0f;
	F32 dist2 = 0.0f;
	IFXVector3 point1, point2;
	for( j = 0; j < numLinesLS; j++ ) 
	{
		IFXU32Line Line;
		ir = pLineSet->GetPositionLine(j,&Line);
		IFXASSERT(ir==IFX_OK);

		ir = pLineSet->GetPosition(Line.VertexA(), &point1);
		IFXASSERT(ir==IFX_OK);
		ir = pLineSet->GetPosition(Line.VertexB(), &point2);
		IFXASSERT(ir==IFX_OK);
		dist = dist + point1.CalcDistanceFrom(point2);
	}

	//  Renderable distance
	for ( j = 0; j < numGroup; j++ )
	{
		pMeshGroup->GetMesh(j,pMesh);
		numLinesLS = pMesh->GetNumLines();

		IFXLineIter lineIter;
		ir = pMesh->GetLineIter(lineIter);
		IFXVector3Iter vertIter;
		IFXVector3Iter normIter;
		ir = pMesh->GetPositionIter(vertIter);
		IFXASSERT(ir==IFX_OK);
		
		if(FALSE == isExcludeNormals)
		{
			ir = pMesh->GetNormalIter(normIter);
			IFXASSERT(ir==IFX_OK);
		}

		IFXTRACE_GENERIC(L"[LineSet Test] Mesh %i\n", j);

		for( k = 0; k < numLinesLS; k++ ) {

			IFXLine* pLine = lineIter.Index(k);
			IFXVector3 *pv1 = vertIter.Index(pLine->VertexA());
			IFXVector3 *pv2 = vertIter.Index(pLine->VertexB());
			dist2 = dist2 + pv1->CalcDistanceFrom(*pv2);

			// normals
			if(FALSE == isExcludeNormals)
			{
				pv1 = normIter.Index(pLine->VertexA());
				pv2 = normIter.Index(pLine->VertexB());
			}
		}// k
		IFXRELEASE(pMesh);
	}// j

	IFXTRACE_GENERIC(L"[Distances at Lines] At AuthorLineSet %f; \
		At Renderable LineSet (MeshGroup)  %f \n",
		dist, dist2);

	if ( fabs(dist - dist2 ) >= 1E-5)
	{
		ir = IFX_FALSE;
	}

	return ir;
}

#endif
