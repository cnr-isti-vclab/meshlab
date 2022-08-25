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
	@file	CIFXAuthorPointSetResource.cpp
*/

#include "CIFXAuthorPointSetResource.h"
#include "IFXModifierDataPacket.h"
#include "IFXNeighborMesh.h"
#include "IFXCoreCIDs.h"
#include "IFXExportingCIDs.h"
#include "IFXDids.h"

CIFXAuthorPointSetResource::CIFXAuthorPointSetResource():
IFXDEFINEMEMBER(m_pDataBlockQueueX)
{
	m_uRefCount = 0;

	m_uTransformDataElementIndex    = (U32)-1;
	m_uRenderableGroupDataElementIndex    = (U32)-1;
	m_uNeighborMeshDataElementIndex = (U32)-1;
	m_uBoundSphereDataElementIndex  = (U32)-1;

	m_uNeighborResControllerDataElementIndex	= (U32)-1;
	m_uBonesManagerDataElementIndex				= (U32)-1;

	m_pAuthorPointSet	= NULL;
	m_pAuthorPointSetMap = NULL;
	m_pRenderPointSetMap = NULL;
	m_pPointMap			= NULL;
	m_pPositionMap		= NULL;
	m_pNormalMap		= NULL;
	m_pTextureMap		= NULL;
	m_pDiffuseMap		= NULL;
	m_pSpecularMap		= NULL;
	m_pQV				= NULL;
	m_pVertexHash		= NULL;
	m_pIteratorCache	= NULL;

	m_bMeshGroupDirty	= FALSE;

	m_pMeshGroup		= NULL;
	m_pNeighborMesh		= NULL;
	m_pUpdatesGroup		= NULL;

	m_bBuiltNeighborMesh	 = FALSE;
	m_pNeighborResController = NULL;
	m_pPointSetSizes		= NULL;

	m_transform.CreateNewElement();
	m_transform[0].Reset();

	m_pBoundSphereDataElement = NULL;

	m_pPositions		= NULL;
	m_pNormals			= NULL;
	m_pTextures			= NULL;
	m_pSpeculars		= NULL;
	m_pDiffuses			= NULL;
	m_pPointMaterials   = NULL;

	m_pPositionPoints   = NULL;
	m_pNormalPoints		= NULL;

	U32 i;
	for( i = 0; i < IFX_MAX_TEXUNITS; ++i )
	{
		m_pTexturePoints[i] = NULL;
	}

	m_pSpecularPoints   = NULL;
	m_pDiffusePoints    = NULL;

	m_pBones = NULL;
}


CIFXAuthorPointSetResource::~CIFXAuthorPointSetResource()
{
	if(m_pNeighborResController)
	{
		IFXDeleteNeighborResController(m_pNeighborResController);
	}

	IFXRELEASE(m_pAuthorPointSet);
	IFXRELEASE(m_pMeshGroup);
	IFXRELEASE(m_pNeighborMesh);
	IFXRELEASE(m_pBoundSphereDataElement);
	IFXRELEASE(m_pUpdatesGroup);

	IFXDELETE(m_pVertexHash);
	IFXDELETE_ARRAY(m_pPointSetSizes);
	IFXDELETE_ARRAY(m_pQV);
	IFXDELETE_ARRAY(m_pIteratorCache);
	IFXRELEASE(m_pAuthorPointSetMap)
	IFXRELEASE(m_pRenderPointSetMap)
	IFXRELEASE(m_pBones);
}

//---------------------------------------------------------------------------
//  CIFXAuthorCLODResource_Factory
//---------------------------------------------------------------------------

IFXRESULT IFXAPI_CALLTYPE CIFXAuthorPointSetResource_Factory( 
											IFXREFIID interfaceId, 
											void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) 
	{
		// Create the CIFXAuthorPointSetResource component.
		CIFXAuthorPointSetResource  *pComponent = new CIFXAuthorPointSetResource;

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


U32 CIFXAuthorPointSetResource::AddRef()
{
	return ++m_uRefCount;
}


U32 CIFXAuthorPointSetResource::Release()
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

IFXRESULT CIFXAuthorPointSetResource::QueryInterface( 
											IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXAuthorPointSetResource)
			*ppInterface = ( IFXAuthorPointSetResource* ) this;
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
//
IFXRESULT CIFXAuthorPointSetResource::SetSceneGraph( IFXSceneGraph* pInSceneGraph )
{
	IFXRESULT result = IFX_OK;

	if (!m_pBoundSphereDataElement)
		result = IFXCreateComponent(
						CID_IFXBoundSphereDataElement, 
						IID_IFXBoundSphereDataElement, 
						(void**)&m_pBoundSphereDataElement);
	if ( IFXSUCCESS(result) )
		result = CIFXMarker::SetSceneGraph(pInSceneGraph);

	return result;
}

//
// IFXMarkerX
//
void CIFXAuthorPointSetResource::GetEncoderX(IFXEncoderX*& rpEncoderX)
{
	CIFXMarker::GetEncoderX( CID_IFXPointSetEncoder, rpEncoderX );
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

static const IFXGUID* s_scpNeighborResControllerOutputDepDIDs[] =
{
	&DID_IFXNeighborMesh,
};


IFXRESULT CIFXAuthorPointSetResource::GetOutputs( IFXGUID**& rpOutOutputs,
												 U32&       rOutNumberOfOutputs,
												 U32*&    rpOutOutputDepAttrs )
{
	IFXRESULT result = IFX_OK;
	rOutNumberOfOutputs = sizeof(s_scpOutputDIDs)/sizeof(IFXGUID*);
	rpOutOutputs         = (IFXGUID**)&s_scpOutputDIDs;
	rpOutOutputDepAttrs = NULL;

	return result;
}

void CIFXAuthorPointSetResource::GetDataBlockQueueX(
										IFXDataBlockQueueX*& rpDataBlockQueueX)
{
	rpDataBlockQueueX = m_pDataBlockQueueX;
	if(rpDataBlockQueueX) 
	{
		rpDataBlockQueueX->AddRef();
	}
}

IFXRESULT CIFXAuthorPointSetResource::GetDependencies(
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
		IFXASSERTBOX( 0, "CIFXAuthorPointSetResource::GetDependencies() called with \
						 unknown output!" );
		result = IFX_E_UNDEFINED;
	}

	return result;
}


IFXRESULT CIFXAuthorPointSetResource::GenerateOutput( 
											 U32    inOutputDataElementIndex,
											 void*& rpOutData, BOOL& rNeedRelease )
{
	IFXRESULT result = IFX_OK;

	if ( inOutputDataElementIndex == m_uRenderableGroupDataElementIndex )
	{   // Cases
		// No AuthorMesh - Error
		// AuthorPointSet and No MeshGroup -> BuildMeshGroup -> InitCompile or 
		// Preview Compile
		// AuthorPointSet and MeshGroup (Dirty) -> Stream Compile
		// AuthorPointSet and MeshGroup (OK) -> Change Res
		if(!m_pAuthorPointSet)
		{
			result = IFX_E_NOT_INITIALIZED;
		}

		if(IFXSUCCESS(result) && (!m_pAuthorPointSet || m_bMeshGroupDirty))
		{
			result = BuildMeshGroup();
		}

		if ( IFXSUCCESS(result) && m_pMeshGroup )
		{
			/**
				@todo: CLOD Calculations are wrong for streaming here, lod, 
				and CLODResource.
				Change to clamped, scaled, finalMax
			*/
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
		from the last call to the build mesh group */
		if( m_pAuthorPointSet && IFXSUCCESS(result))
			m_pBoundSphereDataElement->Bound() = m_pAuthorPointSet->CalcBoundSphere();

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
		IFXASSERTBOX( 0, "CIFXAuthorPointSetResource::GetOutput() not implemented!" );
		result = IFX_E_UNDEFINED;
	}

	return result;
}


IFXRESULT CIFXAuthorPointSetResource::SetDataPacket(
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
// IFXAuthorPointSetResource
//

IFXRESULT CIFXAuthorPointSetResource::GetAuthorPointSet(
										IFXAuthorPointSet*& rpAuthorPointSet)
{
	IFXRESULT rc = IFX_OK;

	if(m_pAuthorPointSet)
	{
		m_pAuthorPointSet->AddRef();
	}
	rpAuthorPointSet = m_pAuthorPointSet;

	IFXRETURN(rc);
}

IFXRESULT CIFXAuthorPointSetResource::SetAuthorPointSet(
										IFXAuthorPointSet* pAuthorPointSet)
{
	IFXRESULT rc = IFX_OK;

	if(m_pAuthorPointSet != pAuthorPointSet)
	{
		ClearMeshGroup();
	}

	if(pAuthorPointSet )
	{
		pAuthorPointSet->AddRef();
	}

	IFXRELEASE(m_pAuthorPointSet);
	m_pAuthorPointSet = pAuthorPointSet;

	m_bMeshGroupDirty = TRUE;

	if(m_pModifierDataPacket) 
	{
		m_pModifierDataPacket->InvalidateDataElement(m_uRenderableGroupDataElementIndex);
		m_pModifierDataPacket->InvalidateDataElement(m_uBoundSphereDataElementIndex);
	}

	IFXRETURN(rc);
}



IFXRESULT CIFXAuthorPointSetResource::GetAuthorMeshMap(IFXMeshMap **ppAuthorPointSetMap)
{
	IFXRESULT rc = IFX_OK;

	if (ppAuthorPointSetMap) 
	{
		if(m_pAuthorPointSetMap)	
		{
			m_pAuthorPointSetMap->AddRef();
		} 
		else
		{
			rc = IFX_E_NOT_INITIALIZED;
		}

		*ppAuthorPointSetMap = m_pAuthorPointSetMap;
	} 
	else 
	{
		rc = IFX_E_INVALID_POINTER;
	}

	IFXRETURN(rc);
}

IFXRESULT CIFXAuthorPointSetResource::SetAuthorMeshMap(IFXMeshMap *pAuthorPointSethMap)
{
	IFXRESULT rc = IFX_OK;

	if(pAuthorPointSethMap)
	{
		pAuthorPointSethMap->AddRef();

		IFXRELEASE(m_pAuthorPointSetMap);

		m_pAuthorPointSetMap = pAuthorPointSethMap;
	} 
	else 
	{
		rc = IFX_E_INVALID_POINTER;
	}

	IFXRETURN(rc);
}

IFXRESULT CIFXAuthorPointSetResource::GetRenderMeshMap(IFXMeshMap** ppRenderPointSetMap)
{
	IFXRESULT rc = IFX_OK;

	if (ppRenderPointSetMap)	
	{
		// Make sure we have the mesh compiled before we go looking for
		// the render mesh map.
		if(!m_pMeshGroup || m_bMeshGroupDirty)	
		{
			rc = BuildMeshGroup();
		}

		if (IFXSUCCESS(rc))	
		{
			if(m_pRenderPointSetMap) 
			{
				m_pRenderPointSetMap->AddRef();
			}
		} 
		else 
		{
			rc = IFX_E_NOT_INITIALIZED;
		}

		*ppRenderPointSetMap = m_pRenderPointSetMap;
	} 
	else 
	{
		rc = IFX_E_INVALID_POINTER;
	}

	IFXRETURN(rc);
}

IFXRESULT CIFXAuthorPointSetResource::SetRenderMeshMap(IFXMeshMap* pRenderPointSetMap)
{
	IFXRESULT rc = IFX_OK;

	if(pRenderPointSetMap)	
	{
		pRenderPointSetMap->AddRef();

		IFXRELEASE(m_pRenderPointSetMap);

		m_pRenderPointSetMap = pRenderPointSetMap;
	} 
	else 
	{
		rc = IFX_E_INVALID_POINTER;
	}

	IFXRETURN(rc);
}



IFXRESULT CIFXAuthorPointSetResource::Transfer()
{
	IFXRESULT rc = IFX_OK;
	IFXRETURN(rc);
}


// ---------------------------------------------------------------
// Private Methods n stuff
// ---------------------------------------------------------------
void CIFXAuthorPointSetResource::ClearMeshGroup()
{
	if(m_pMeshGroup)
	{
		IFXRELEASE(m_pMeshGroup)
		IFXRELEASE(m_pRenderPointSetMap);
 		IFXRELEASE(m_pAuthorPointSetMap);
		IFXDELETE_ARRAY(m_pPointSetSizes);
		if(m_pModifierDataPacket)
		{
			m_pModifierDataPacket->InvalidateDataElement(
										m_uRenderableGroupDataElementIndex);
		}
	}
}


IFXRESULT CIFXAuthorPointSetResource::BuildDataBlockQueue()
{
	IFXRESULT result =  IFX_OK;

	// create a datablock queue
	IFXRELEASE( m_pDataBlockQueueX );
	result = IFXCreateComponent( CID_IFXDataBlockQueueX, IID_IFXDataBlockQueueX,
		(void**)&m_pDataBlockQueueX );

	return result;
}


//Build* should potentially do invalidation of the respective data elements
IFXRESULT CIFXAuthorPointSetResource::BuildNeighborMesh()
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
						(void**)&(m_pNeighborMesh)); 
	}

	if (IFXSUCCESS(result))
		result = m_pNeighborMesh->Allocate(*m_pMeshGroup);

	if (IFXSUCCESS(result))
		result = m_pNeighborMesh->Build(*m_pMeshGroup, NULL);

	return result;
}



IFXRESULT CIFXAuthorPointSetResource::BuildNeighborResController()
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
													m_pNeighborMesh, 
													m_pUpdatesGroup);
		}
	}

	return result;
}


IFXNeighborResControllerInterface* 
CIFXAuthorPointSetResource::GetNeighborResController()
{
	BuildNeighborResController();
	return m_pNeighborResController;
}


IFXRESULT CIFXAuthorPointSetResource::GetUpdatesGroup(
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

IFXRESULT CIFXAuthorPointSetResource::GetNeighborMesh(
											IFXNeighborMesh** ppNeighborMesh)
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


IFXRESULT CIFXAuthorPointSetResource::GetMeshGroup(IFXMeshGroup** ppMeshGroup)
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


IFXRESULT CIFXAuthorPointSetResource::SetTransform(const IFXMatrix4x4& rTransform)
{
	m_transform[0] = rTransform;
	if(m_pModifierDataPacket)
	{
		return m_pModifierDataPacket->InvalidateDataElement(
											m_uTransformDataElementIndex);
	}

	return IFX_OK;
}


IFXRESULT CIFXAuthorPointSetResource::InvalidateTransform()
{
	if(m_pModifierDataPacket)
	{
		m_pModifierDataPacket->InvalidateDataElement(m_uTransformDataElementIndex);
	}
	return IFX_OK;
}


IFXRESULT CIFXAuthorPointSetResource::BuildMeshGroup()
{
	IFXRESULT ir =  IFX_OK;

	if (!m_pAuthorPointSet) return IFX_E_NOT_INITIALIZED;

	if(!m_pMeshGroup || m_bMeshGroupDirty)
	{	
		// this means it has not built so far

		if(m_pPointSetSizes) 
		{
			ClearMeshGroup();
		}

		// initialize PointSetSizes

		/*
			evaluate number of vertices at each PointSet, 
			exact number will be set up later
		*/
		ir = cpl_EvaluatePointSetSizes();

		if (IFXSUCCESS(ir))
		{
			// allocate Point Set Groups and PointSets there
			ir = cpl_AllocateOutput();
		} 

		if (IFXSUCCESS(ir))
		{
			// allocate PointSetMap as "helper" object for Point Set convertion
			ir = cpl_AllocatePointSetMap();
		} 

		if (IFXSUCCESS(ir))
		{
			// allocate PointSetap as "helper" object for Point Set convertion
			ir = cpl_AllocateVertexHashAndVertexDescQuery();
		} 

		if (IFXSUCCESS(ir))
		{
			// get direct pointers to AuthorPointSet data 
			// (crack open the author Pointset)
			m_pAuthorPointSet->GetPositions(&m_pPositions);
			m_pAuthorPointSet->GetNormals(&m_pNormals);
			m_pAuthorPointSet->GetTexCoords(&m_pTextures);
			m_pAuthorPointSet->GetDiffuseColors(&m_pDiffuses);
			m_pAuthorPointSet->GetSpecularColors(&m_pSpeculars);
			m_pAuthorPointSet->GetPositionPoints(&m_pPositionPoints);
			m_pAuthorPointSet->GetNormalPoints(&m_pNormalPoints);
			m_pAuthorPointSet->GetDiffusePoints(&m_pDiffusePoints);
			m_pAuthorPointSet->GetSpecularPoints(&m_pSpecularPoints);

			// find max num texture layers
			U32 maxLayers = 0;
			U32 i;
			for( i=0;i<m_numMaterials; i++)
			{
				if(m_pMaterials[i].m_uNumTextureLayers > maxLayers)
					maxLayers = m_pMaterials[i].m_uNumTextureLayers;
			}

			for(i=0; i<maxLayers; i++)
				m_pAuthorPointSet->GetTexPoints(i, &m_pTexturePoints[i]);

			// proceed w/ "static" compile (there is not another compile at
			// AuthorPointSetResource for now: no LOD supproted for PointSets
			ir = cpl_Compile();
		} 

		m_bMeshGroupDirty = FALSE;
	}

	return ir;
}

// private
IFXRESULT CIFXAuthorPointSetResource::cpl_EvaluatePointSetSizes()
{
	IFXRESULT ir = IFX_OK;

	if (!m_pAuthorPointSet)
	{
		IFXASSERTBOX(0, "AuthorPointSet is not defined / Not expected here!");
		ir = IFX_E_UNDEFINED;
	}

	if(IFXSUCCESS(ir))
	{
		// num of Materials(Shaders) = num PointSets in Group = num of PointSetSizes
		m_numMaterials = m_pAuthorPointSet->GetMaxPointSetDesc()->m_numMaterials;
		IFXASSERT(m_numMaterials);
		m_pPointSetSizes = new IFXPointSetSize[m_numMaterials];

		// initialize Sizes by Zeros
		U32 i;
		for( i = 0; i < m_numMaterials; i++)
		{
			m_pPointSetSizes[i].m_numVertices = 0;
		}

		// count number of points per material

		m_numPointSetPoints = m_pAuthorPointSet->GetPointSetDesc()->m_numPoints;
		m_pAuthorPointSet->GetPointMaterials(&m_pPointMaterials);

		for( i = 0; i < m_numPointSetPoints; i++)
		{
			// material index less then number of Materials
			IFXASSERT(m_pPointMaterials[i]<m_numMaterials);
			m_pPointSetSizes[m_pPointMaterials[i]].m_numVertices++;
		}

		// fill out vertex attribute info, estimate number of vertices

		m_pAuthorPointSet->GetMaterials(&m_pMaterials);

		for(i = 0; i < m_numMaterials; i++)
		{
			// only there are Points with specific material
			// Note ! : this needs to be revised if decide implement CLOD for PointSet
			if ( m_pPointSetSizes[i].m_numVertices >0 ) 
			{
				m_pPointSetSizes[i].m_vertexAttributes.m_uData.m_bHasDiffuseColors = 
					m_pMaterials[i].m_uDiffuseColors;
				m_pPointSetSizes[i].m_vertexAttributes.m_uData.m_bHasSpecularColors = 
					m_pMaterials[i].m_uSpecularColors;
				m_pPointSetSizes[i].m_vertexAttributes.m_uData.m_uNumTexCoordLayers = 
					m_pMaterials[i].m_uNumTextureLayers;
				m_pPointSetSizes[i].m_vertexAttributes.m_uData.m_bHasPositions = TRUE;
				m_pPointSetSizes[i].m_vertexAttributes.m_uData.m_bHasNormals =
					m_pAuthorPointSet->GetPointSetDesc()->m_numNormals > 0? TRUE: FALSE;

			}
		}
	}

	return ir;
}

IFXRESULT CIFXAuthorPointSetResource::cpl_AllocateOutput()
{
	IFXRESULT ir = IFX_OK;
	IFXRELEASE(m_pMeshGroup);
	ir = IFXCreateComponent(CID_IFXMeshGroup, IID_IFXMeshGroup, (void**) &m_pMeshGroup);

	if(IFXSUCCESS(ir))
	{
		// num of Materials(Shaders) = num PointSets in Group = num of PointSetSizes
		ir = m_pMeshGroup->Allocate(m_numMaterials);
	}

	if(IFXSUCCESS(ir))
	{
		U32 i;
		for( i = 0; i < m_numMaterials && IFXSUCCESS(ir); i++)
		{
			IFXMesh* pMesh = NULL;

			ir = IFXCreateComponent(CID_IFXMesh, IID_IFXMesh, (void**) &pMesh);

			if(IFXSUCCESS(ir))
			{
				ir = pMesh->Allocate(m_pPointSetSizes[i].m_vertexAttributes,
					m_pPointSetSizes[i].m_numVertices, 0);
			} 

			if(IFXSUCCESS(ir))
			{
				m_pMeshGroup->SetMesh(i,pMesh);
				pMesh->SetRenderableType( IFXRENDERABLE_ELEMENT_TYPE_POINTSET );
				pMesh->SetNumVertices(0); // bump this up as we fill in the data
			} 

			IFXRELEASE(pMesh);
		}
	}

	return ir;
}

IFXRESULT CIFXAuthorPointSetResource::cpl_AllocatePointSetMap()
{
	IFXRESULT ir = IFX_OK;

	// allocate Render PointSet map
	IFXRELEASE(m_pRenderPointSetMap);

	ir = IFXCreateComponent(
				CID_IFXMeshMap,	IID_IFXMeshMap, (void**)&m_pRenderPointSetMap);

	if( IFXSUCCESS( ir ) )
	{
		ir = m_pRenderPointSetMap->Allocate(m_pAuthorPointSet);
	}

	m_pPointMap		= m_pRenderPointSetMap->GetFaceMap();  
	m_pPositionMap  = m_pRenderPointSetMap->GetPositionMap();
	m_pNormalMap	= m_pRenderPointSetMap->GetNormalMap();
	m_pTextureMap	= m_pRenderPointSetMap->GetTextureMap();
	m_pDiffuseMap	= m_pRenderPointSetMap->GetDiffuseMap();
	m_pSpecularMap	= m_pRenderPointSetMap->GetSpecularMap();

	return ir;
}

IFXRESULT CIFXAuthorPointSetResource::cpl_AllocateVertexHashAndVertexDescQuery()
{
	IFXRESULT result;

	// allocate vertex hash
	IFXDELETE(m_pVertexHash);
	m_pVertexHash = new VertexHash;
	if(m_pVertexHash == NULL) return IFX_E_OUT_OF_MEMORY;

	result = m_pVertexHash->Allocate(
							m_pAuthorPointSet->GetMaxPointSetDesc()->m_numPositions);

	if(result != IFX_OK) return IFX_E_OUT_OF_MEMORY;

	IFXAuthorMaterial *pMats, *pMat;
	U32 i,j,numAttributes;
	m_pAuthorPointSet->GetMaterials(&pMats);

	m_numMaterials = m_pAuthorPointSet->GetMaxPointSetDesc()->m_numMaterials;
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

	if(m_pIteratorCache == NULL) return IFX_E_OUT_OF_MEMORY;

	IFXMesh* pMesh;

	for(i=0; i<m_numMaterials; i++)
	{
		m_pMeshGroup->GetMesh(i, pMesh);
		pMesh->GetVertexIter(m_pIteratorCache[i]);
		IFXRELEASE(pMesh);
	}

	return result;
}


IFXRESULT CIFXAuthorPointSetResource::cpl_Compile()
{
	IFXRESULT ir = IFX_OK;

	U32       numPoints, mat, pointAuthor, vertIndex;
	IFXMesh*    pMesh;
	IFXVertexIter vertIter;
	// IFXVertexMap* pPointMap;

	// pPointMap = m_pRenderPointSetMap->GetFaceMap();

	numPoints = m_pAuthorPointSet->GetPointSetDesc()->m_numPoints;

	for(pointAuthor = 0; pointAuthor < numPoints; pointAuthor++)
	{
		mat = m_pPointMaterials[pointAuthor];
		m_pMeshGroup->GetMesh(mat, pMesh);

		pMesh->GetVertexIter(vertIter);

		// look for existing vertex, if none create new vertex
		// returning vertIndex that is postion index in the current LineSet (pLS)
		ir = cpl_FindAndBuildVertex(pointAuthor, mat, &vertIndex);

		if(ir != IFX_OK)
		{
			IFXASSERT(0);
			return IFX_FALSE;
		} 
		else 
		{
			IFXASSERT(vertIndex < m_pPointSetSizes[mat].m_numVertices);
		}

		IFXRELEASE(pMesh);
	}

	return ir;
}

IFXRESULT CIFXAuthorPointSetResource::cpl_FindAndBuildVertex(U32 authorPointIndex,
															 U32 mat, U32* pOutIFXIndex)
{
	IFXRESULT ir = IFX_OK;

	VertexDescriptor* pQ      = &m_pQV[mat];  //grab pre allocated query vertex
	IFXAuthorMaterial*  pMatDesc  = &m_pMaterials[mat];
	U32 layer, numDim;

	// the first one is always the material id which is 
	// already set when the query verts were allocated.
	U32 attributeIndex = 1;
	
	U32 AuthorIndex   = U32(-1);
	IFXMesh*      pMesh = NULL;

	// pQ is used at intermetiate storage for all vertex atributes being copied to
	// appropriate place of PointSet

	// fill in query vertex descriptor (was partially filled in at 
	// allocate time for this material).
	if(pMatDesc->m_uNormals)
	{
		pQ->pAttributes[attributeIndex++] = m_pNormalPoints[authorPointIndex];
	}

	for(layer = 0; layer < pMatDesc->m_uNumTextureLayers; layer++)
		pQ->pAttributes[attributeIndex++] = m_pTexturePoints[layer][authorPointIndex];

	if(pMatDesc->m_uDiffuseColors)
		pQ->pAttributes[attributeIndex++] = m_pDiffusePoints[authorPointIndex];

	if(pMatDesc->m_uSpecularColors)
		pQ->pAttributes[attributeIndex++] = m_pSpecularPoints[authorPointIndex];

	pQ->AuthorIndex = m_pPositionPoints[authorPointIndex];

	m_pMeshGroup->GetMesh(mat, pMesh);
	U32 numVerts = pMesh->GetNumVertices();
	pQ->IFXIndex = numVerts;
	// bump vertex count on output mesh.
	IFXASSERT(numVerts < m_pPointSetSizes[mat].m_numVertices);
	pMesh->SetNumVertices(numVerts + 1);

	// m_pVertexHash->Insert(pNewVertex);

	// Fill out ifx Point set vertex.

	// get the iterator for this mesh, it is pre-set already
	IFXVertexIter *vi = &(m_pIteratorCache[mat]);

	U32 IFXVertIndex = pQ->IFXIndex;
	attributeIndex = 1;  // get past 0 which is always matid;

	// vertex postion is in the "point section" of the Mesh !!!!
	// *vi - iterator
	*vi->GetPosition() = m_pPositions[ pQ->AuthorIndex ];

	// update vertex position map
	m_pPositionMap->AddVertex(pQ->AuthorIndex, mat, IFXVertIndex);

	if(pMatDesc->m_uNormals)
	{
		AuthorIndex = pQ->pAttributes[attributeIndex++];

		// vertex postion is in the Point set!
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
				IFXASSERTBOX(0, "Wrong number of texture dimention!");  
			}
		}
	}

	if(pMatDesc->m_uDiffuseColors)
	{
		AuthorIndex = pQ->pAttributes[attributeIndex++];
		if(IFXSUCCESS(m_pDiffuseMap->AddVertex(AuthorIndex, mat, IFXVertIndex)))
			*vi->GetColor(0) = m_pDiffuses[ AuthorIndex ].GetRGBA();
	}

	if(pMatDesc->m_uSpecularColors)
	{
		AuthorIndex = pQ->pAttributes[attributeIndex++];
		if(IFXSUCCESS(m_pSpecularMap->AddVertex(AuthorIndex, mat, IFXVertIndex)))
			*vi->GetColor(1) = m_pSpeculars[ AuthorIndex ].GetRGBA();
	}

	vi->Next(); // point to the next vertex in the Mesh

	*pOutIFXIndex = pQ->IFXIndex;  // wil return it but not sure if we need it ....
	IFXRELEASE(pMesh);

	return ir;
}
