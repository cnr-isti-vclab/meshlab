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
	@file	IFXCLODManager.cpp
*/

#include "IFXCLODManager.h"


U32 CIFXResManager::SetResolution(U32 r)
{
	if(r > resolution)
		IncreaseResolution(r - resolution);
	else
	if(r < resolution)
		DecreaseResolution(resolution - r);
	
	resolution = r;
	return r;
}


void CIFXResManager::IncreaseResolution(U32 delta)
{
    // Get the mesh:
    IFXMeshGroup*   pMeshGroup  = m_pCLODMgr->GetMeshGroup();
    IFXMesh*        pMesh       = 0;
	pMeshGroup->GetMesh(m_meshIndex, pMesh);

    // Access the updates structure:
    IFXUpdatesGroup*    pUpdatesGroup   = m_pCLODMgr->GetUpdatesGroup();
    IFXUpdates*         pUpdates        = pUpdatesGroup->GetUpdates(m_meshIndex);

	U32 numFaces = pMesh->GetNumFaces();
	U32 numVerts = pMesh->GetNumVertices();
	IFXFaceIter faceIter;
	
	pMesh->GetFaceIter(faceIter);
	while(delta--)
	{
		IFXResolutionChange *rc = &pUpdates->pResChanges[resolution++];
		numFaces += rc->deltaFaces;
		numVerts += rc->deltaVerts;
		U32 i;
		for( i = 0; i < rc->numFaceUpdates; i++)
		{
			IFXFaceUpdate *fu = &pUpdates->pFaceUpdates[numFaceUpdatesApplied++];
			faceIter.Index(fu->face)->Set(fu->corner, fu->newUp);
		}
	}
	pMesh->SetNumFaces(numFaces);
	pMesh->SetNumVertices(numVerts);

	pMesh->UpdateVersionWord(IFX_MESH_POSITION);
	pMesh->UpdateVersionWord(IFX_MESH_FACE);

	IFXRELEASE(pMesh);
}


void CIFXResManager::DecreaseResolution(U32 delta)
{	
    // Get the mesh:
    IFXMeshGroup*   pMeshGroup  = m_pCLODMgr->GetMeshGroup();
    IFXMesh*        pMesh       = 0;
	pMeshGroup->GetMesh(m_meshIndex, pMesh);

    // Access the updates structure:
    IFXUpdatesGroup*    pUpdatesGroup   = m_pCLODMgr->GetUpdatesGroup();
    IFXUpdates*         pUpdates        = pUpdatesGroup->GetUpdates(m_meshIndex);
	
	U32 numFaces = pMesh->GetNumFaces();
	U32 numVerts = pMesh->GetNumVertices();
	IFXFaceIter faceIter;

	pMesh->GetFaceIter(faceIter);
	while(delta--)
	{
		IFXResolutionChange *rc = &pUpdates->pResChanges[--resolution];
		numFaces -= rc->deltaFaces;
		numVerts -= rc->deltaVerts;
		U32 i;
		for( i = 0; i < rc->numFaceUpdates; i++)
		{
			IFXFaceUpdate *fu = &pUpdates->pFaceUpdates[--numFaceUpdatesApplied];
			faceIter.Index(fu->face)->Set(fu->corner, fu->newDown);
		}
	}

	pMesh->SetNumFaces(numFaces);
	pMesh->SetNumVertices(numVerts);

	pMesh->UpdateVersionWord(IFX_MESH_POSITION);
	pMesh->UpdateVersionWord(IFX_MESH_FACE);

	IFXRELEASE(pMesh);
}

U32	CIFXCLODManager::PrevResolution()
{
	return SetResolution(m_prevResolution);
}


U32 CIFXCLODManager::SetResolution(U32 r)
{
	if(r > m_pUpdatesGroup->GetMaxResolution()) 
	{
		r = m_pUpdatesGroup->GetMaxResolution();
	}

	m_prevResolution = m_resolution;

	if(r > m_resolution)
		IncreaseTo(r);
	else
	if(r < m_resolution)
		DecreaseTo(r);
	
	m_resolution = r;
	return r;
}

void CIFXCLODManager::IncreaseTo(U32 r)
{
	U32 numMaterials = m_pUpdatesGroup->GetNumUpdates();
	U32 m;
	for( m = 0; m < numMaterials; m++)
	{
		U32* synchTable = m_pUpdatesGroup->GetSyncTable(m);
		U32  localRes = m_pControllers[m].GetResolution();
		U32  maxLocalRes = m_pControllers[m].GetMaxResolution();
		
		U32 i = localRes;
		while (i < maxLocalRes && r > synchTable[i]) 
			i++;
		if(i - localRes) 
			m_pControllers[m].IncreaseResolution(i - localRes);
	}
}

void CIFXCLODManager::DecreaseTo(U32 r)
{
U32 numMaterials = m_pUpdatesGroup->GetNumUpdates();
	U32 m;
	for( m = 0; m < numMaterials; m++)
	{
		U32* synchTable = m_pUpdatesGroup->GetSyncTable(m);
		U32  localRes = m_pControllers[m].GetResolution();
		//U32  maxLocalRes = m_pControllers[m].GetMaxResolution();
		
		U32 i = localRes;
		while (i > 0 && r <= synchTable[i-1] ) 
			i--;
		
		if(localRes - i) 
			m_pControllers[m].DecreaseResolution(localRes - i);
	}

}


IFXRESULT CIFXCLODManager::Initialize(
								IFXMeshGroup* pInMeshGroup, 
								IFXUpdatesGroup* pInUpdatesGroup)
{
	IFXRESULT result = IFX_OK;

	IFXASSERT(pInMeshGroup);
	IFXASSERT(pInUpdatesGroup);

	if(pInMeshGroup->GetNumMeshes() != pInUpdatesGroup->GetNumUpdates())
	{
		IFXTRACE_GENERIC(L"Error: Number of meshes IFXMeshGroup"
						 L"doesn't match number of Updates in IFXUpdatesGroup\n");
		return IFX_E_UNDEFINED;
	}

	m_pMeshGroup            = pInMeshGroup; 
	m_pUpdatesGroup         = pInUpdatesGroup; 
	IFXADDREF(m_pUpdatesGroup);
	m_resolution            = pInUpdatesGroup->GetMaxResolution();
	m_finalMaxResolution    = pInUpdatesGroup->GetFinalMaxResolution();

	U32 numMesh = pInMeshGroup->GetNumMeshes();
	
	if (m_pControllers) 
		delete [] m_pControllers;

	m_pControllers = new CIFXResManager[numMesh];

	if( m_pControllers != NULL )
	{
		U32 i;
		for(i = 0; i < numMesh && IFXSUCCESS(result); i++)
		{
			result = m_pControllers[i].Initialize(this, i);
		}
	}
	else
		result = IFX_E_OUT_OF_MEMORY;

	return result; 
}


IFXRESULT CIFXCLODManager::Initialize(
								IFXCLODManager* pInCLODManager, 
								IFXMeshGroup* pInMeshGroup)
{
	IFXRESULT result = IFX_OK;
	IFXASSERT(pInCLODManager);

	m_pMeshGroup            = pInMeshGroup; 
	m_pUpdatesGroup         = pInCLODManager->GetUpdatesGroup(); 
	IFXADDREF(m_pUpdatesGroup);
	m_resolution            = pInCLODManager->GetResolution();
	m_finalMaxResolution    = m_pUpdatesGroup->GetFinalMaxResolution();

	U32 numMesh = m_pMeshGroup->GetNumMeshes();
	
	if (m_pControllers) 
		delete [] m_pControllers;

	m_pControllers = new CIFXResManager[numMesh];

	if( m_pControllers != NULL )
	{
		U32 i;
		for(i = 0; i < numMesh && IFXSUCCESS(result); i++)
		{
			result = m_pControllers[i].Initialize(this, i);
		}
	}
	else
		result = IFX_E_OUT_OF_MEMORY;

	return result; 
}

IFXRESULT CIFXCLODManager::Initialize_ResolutionZero(
            IFXMeshGroup    *pInMeshGroup, 
            IFXUpdatesGroup *pInUpdatesGroup)
{
	IFXRESULT result = IFX_OK;

	IFXASSERT(pInMeshGroup);
	IFXASSERT(pInUpdatesGroup);

	if(pInMeshGroup->GetNumMeshes() != pInUpdatesGroup->GetNumUpdates())
	{
		IFXTRACE_GENERIC(L"Error: Number of meshes IFXMeshGroup "
						 L"doesn't match number of Updates in IFXUpdatesGroup\n");  
		return IFX_E_UNDEFINED;
	}
	m_pMeshGroup            = pInMeshGroup; 
	m_pUpdatesGroup         = pInUpdatesGroup; 
	IFXADDREF(m_pUpdatesGroup);
	m_resolution            = 0;
	m_finalMaxResolution    = pInUpdatesGroup->GetFinalMaxResolution();
	U32 numMesh             = pInMeshGroup->GetNumMeshes();
	
	if (m_pControllers) 
		delete [] m_pControllers;

	m_pControllers = new CIFXResManager[numMesh];

	if( m_pControllers != NULL )
	{
		U32 i;
		for(i = 0; i < numMesh && IFXSUCCESS(result); i++)
		{
			result = m_pControllers[i].Initialize(this, i);
		}
	}
	else
		result = IFX_E_OUT_OF_MEMORY;

	return result; 
}

IFXRESULT CIFXCLODManager::UpdateMesh(
								IFXMeshGroup* pInMeshGroup, 
								IFXMeshGroup** ppOutMeshGroup)	
{
	IFXASSERT(pInMeshGroup);
	IFXASSERT(ppOutMeshGroup);

	if(m_pMeshGroup != pInMeshGroup)
	{
		IFXRESULT rval;
		if(IFXFAILURE( rval = Initialize(pInMeshGroup, m_pUpdatesGroup) ))
        {
			IFXASSERT(0);
			return rval;
		}
	}
	*ppOutMeshGroup = m_pMeshGroup; 
	return IFX_OK; 
}

CIFXCLODManager::~CIFXCLODManager()	
{ 
	if (m_pControllers) delete [] m_pControllers; 

	IFXRELEASE(m_pUpdatesGroup);
}

//---------------------------------------------------------------------------
//  CIFXCLODManager_Factory
//
//  This is the CIFXCLODManager component factory function.  The
//  CIFXCLODManager component can be instaniated multiple times.
//---------------------------------------------------------------------------

IFXRESULT IFXAPI_CALLTYPE CIFXCLODManager_Factory( 
									IFXREFIID interfaceId,
									void**   ppInterface )
{
	IFXRESULT result;

	if ( ppInterface )
	{
		// Create the CIFXAuthorMeshGroup component.
		CIFXCLODManager  *pComponent = new CIFXCLODManager;

		if ( pComponent )
		{
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		}
		else
			result = IFX_E_OUT_OF_MEMORY;
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

//---------------------------------------------------------------------------
//  CIFXCLODManager::AddRef
//
//  This method increments the reference count for an interface on a
//  component.  It should be called for every new copy of a pointer to an
//  interface on a given component.  It returns a U32 that contains a value
//  from 1 to 2^32 - 1 that defines the new reference count.  The return
//  value should only be used for debugging purposes.
//---------------------------------------------------------------------------

U32 CIFXCLODManager::AddRef()
{
	return ++m_refCount;
}

//---------------------------------------------------------------------------
//  CIFXCLODManager::Release
//
//  This method decrements the reference count for the calling interface on a
//  component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//  that defines the new reference count.  The return value should only be
//  used for debugging purposes.  If the reference count on a component falls
//  to zero, the component is destroyed.
//---------------------------------------------------------------------------

U32 CIFXCLODManager::Release()
{
	if ( !( --m_refCount ) )
	{

		delete this;
		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return m_refCount;
}

//---------------------------------------------------------------------------
//  CIFXCLODManager::QueryInterface
//
//  This method provides access to the various interfaces supported by a
//  component.  Upon success, it increments the component's reference count,
//  hands back a pointer to the specified interface and returns IFX_OK.
//  Otherwise, it returns either IFX_E_INVALID_POINTER or IFX_E_UNSUPPORTED.
//
//  A number of rules must be adhered to by all implementations of
//  QueryInterface.  For a list of such rules, refer to the Microsoft COM
//  description of the IUnknown::QueryInterface method.
//---------------------------------------------------------------------------

IFXRESULT CIFXCLODManager::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT result  = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXUnknown )
		{
			*ppInterface = ( IFXUnknown* ) this;
		}
		else if ( interfaceId == IID_IFXCLODManager)
		{
			*ppInterface = ( IFXCLODManager* ) this;
		}
		else
		{
			*ppInterface = NULL;

			result = IFX_E_UNSUPPORTED;
		}

		if (IFXSUCCESS(result))
			AddRef();
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}
