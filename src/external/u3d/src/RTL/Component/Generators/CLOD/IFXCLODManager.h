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
	@file	IFXCLODManager.h
*/

#ifndef CIFXCLODManager_H
#define CIFXCLODManager_H

#include "IFXMesh.h"
#include "IFXUpdatesGroup.h"
#include "IFXCLODManagerInterface.h"


/**
	This changes the resolution of a single IFXMesh.
	If an instance of this class is being used by an IFXCLODManager then
	you should not change it's resolution here since you may throw off the 
	synchronization provided by the CLODManager.

	If your mesh has only one material then you should use this class to change it's
	resolution directly since it is faster and smaller than IFXCLODManager.
*/
class CIFXResManager : public IFXResManager
{
public:
	// Default constructor:
	CIFXResManager();
	virtual ~CIFXResManager();

	/** 
		@warning	Initialize must be called before any other method is called.
		@warning	The input mesh is assumed to be at full resolution.
		@todo	Consistency checking, resolution option.
	*/
	IFXRESULT   Initialize (IFXCLODManager* pInCLODMgr, U32 meshIndex);
	IFXRESULT   Initialize_ResolutionZero (IFXCLODManager* pInCLODMgr, U32 meshIndex);

	/// Hands back the modified input mesh.
	IFXRESULT   UpdateMesh(IFXMesh* pIn, IFXMesh** ppOut);

	/// The resolution range is from 0 to the number of vertices in the original IMESH.
	U32       SetResolution(U32 r);

	U32       GetResolution()   
	{ 
		return resolution; 
	}

	U32       GetMaxResolution();
	IFXUpdates*   GetUpdates();

	void IncreaseResolution(U32 delta);
	void DecreaseResolution(U32 delta);

private:

	/// Pointer to the parent multi-res mgr:
	IFXCLODManager* m_pCLODMgr;
	U32             m_meshIndex;

	U32 resolution;    ///< number of resolution change records currently applied.
	U32 numFaceUpdatesApplied;
};


/**
	This object is used to change the resolution of an IFXMeshGroup.
	It does this by adjusting the resolution on each individual IFXMesh in a synchronous
	manner so that cracks do not develop between materials.
	The resolution range is from 0 to the number of vertices in the original IMESH.

	@note	If your mesh has only one material then you should use just the 
			IFXResManger class above as it does away with overhead associated 
			with keeping the resolution of each IFXMesh synchronized.
*/
class CIFXCLODManager : public IFXCLODManager
{
public:
	U32 IFXAPI 			AddRef ();
	U32 IFXAPI 			Release ();
	IFXRESULT IFXAPI 	QueryInterface ( IFXREFIID interfaceId, void** ppInterface );

	/**
		@warning: the input mesh is assumed to be at full resolution.
		@todo: check consistancy, add resolution option?
	*/
	IFXRESULT IFXAPI Initialize (
							IFXMeshGroup* pInMeshGroup, 
							IFXUpdatesGroup* pInUpdatesGroup);
	IFXRESULT IFXAPI Initialize(
							IFXCLODManager* pCLODManager, 
							IFXMeshGroup* pInMeshGroup);
	IFXRESULT IFXAPI Initialize_ResolutionZero(
							IFXMeshGroup* pInMeshGroup, 
							IFXUpdatesGroup* pInUpdatesGroup);

	/// Hands back the modified input mesh.
	IFXRESULT IFXAPI UpdateMesh(IFXMeshGroup* pIn, IFXMeshGroup** ppOut);

	U32     IFXAPI 	PrevResolution();
	U32     IFXAPI 	SetResolution(U32 r);
	U32     IFXAPI 	GetResolution();
	U32     IFXAPI 	GetMaxResolution();

	IFXResManager*     IFXAPI GetResManager(U32 i);
	IFXMeshGroup*      IFXAPI GetMeshGroup(void);
	IFXUpdatesGroup*   IFXAPI GetUpdatesGroup(void);

	friend IFXRESULT IFXAPI_CALLTYPE CIFXCLODManager_Factory( 
											IFXREFIID interfaceId,
											void**   ppInterface );
private:
	CIFXCLODManager();
	virtual ~CIFXCLODManager();

	void IncreaseTo(U32 r);
	void DecreaseTo(U32 r);

	IFXMeshGroup*		m_pMeshGroup;
	IFXUpdatesGroup*    m_pUpdatesGroup;

	/// array of individual mesh resolution controllers
	CIFXResManager*     m_pControllers;

	U32	m_refCount;	// Number of outstanding references to the object.
	U32 m_prevResolution;
	U32 m_resolution;
	U32 m_finalMaxResolution;
};


// Inline implementations of IFXResManager and IFXCLODManager:

IFXINLINE CIFXResManager::CIFXResManager()
{
	m_pCLODMgr  = NULL;
	m_meshIndex     = 0;

	resolution = 0;
}

IFXINLINE CIFXResManager::~CIFXResManager()
{
//	IFXRELEASE(m_pCLODMgr);
}


IFXINLINE IFXRESULT CIFXResManager::Initialize(
										IFXCLODManager* pInCLODMgr, 
										U32 meshIndex)
{
//	IFXRELEASE(m_pCLODMgr);
	m_pCLODMgr  = pInCLODMgr;
//	m_pCLODMgr->AddRef();
	m_meshIndex     = meshIndex;

	// Access the updates structure:
	IFXUpdatesGroup*    pUpdatesGroup   = m_pCLODMgr->GetUpdatesGroup();
	IFXUpdates*         pUpdates        = pUpdatesGroup->GetUpdates(m_meshIndex);

	resolution              = pUpdates->numResChanges;
	numFaceUpdatesApplied   = pUpdates->numFaceUpdates;

	return IFX_OK;
}


IFXINLINE IFXRESULT CIFXResManager::Initialize_ResolutionZero(
										IFXCLODManager* pInCLODMgr, 
										U32 meshIndex)
{
//	IFXRELEASE(m_pCLODMgr);
	m_pCLODMgr  = pInCLODMgr;
//	m_pCLODMgr->AddRef();
	m_meshIndex     = meshIndex;

	resolution  = 0;
	numFaceUpdatesApplied = 0;

	return IFX_OK;
}


IFXINLINE U32 CIFXResManager::GetMaxResolution()
{
	IFXASSERT (m_pCLODMgr);

	// Access the updates structure:
	IFXUpdatesGroup*    pUpdatesGroup   = m_pCLODMgr->GetUpdatesGroup();
	IFXUpdates*         pUpdates        = pUpdatesGroup->GetUpdates(m_meshIndex);

	return pUpdates->numResChanges;
}


IFXINLINE IFXUpdates* CIFXResManager::GetUpdates()
{
	IFXASSERT (m_pCLODMgr);

	// Access the updates structure:
	IFXUpdatesGroup*    pUpdatesGroup   = m_pCLODMgr->GetUpdatesGroup();
	IFXUpdates*         pUpdates        = pUpdatesGroup->GetUpdates(m_meshIndex);
	IFXRELEASE(pUpdatesGroup);

	return pUpdates;
}


// Hands back the modified input mesh.
IFXINLINE IFXRESULT CIFXResManager::UpdateMesh(IFXMesh* pIn, IFXMesh** ppOut)
{
	// Access the mesh structure:
	IFXMeshGroup*   pMeshGroup  = m_pCLODMgr->GetMeshGroup();
	IFXMesh*        pMesh       = 0;
	pMeshGroup->GetMesh(m_meshIndex, pMesh);

	if(pMesh != pIn)
	{
		IFXRESULT rval;
		if(IFXFAILURE( rval = Initialize(m_pCLODMgr, m_meshIndex) ))
			return rval;
	}
	*ppOut = pMesh;
	IFXRELEASE(pMesh);

	return IFX_OK;
}

IFXINLINE CIFXCLODManager::CIFXCLODManager()
{
	m_pMeshGroup = NULL;
	m_pUpdatesGroup = NULL;
	m_resolution = 0; m_finalMaxResolution = 0;
	m_pControllers = NULL;
	m_refCount = 0;
}


IFXINLINE U32 CIFXCLODManager::GetResolution()
{
	return m_resolution;
}


IFXINLINE U32 CIFXCLODManager::GetMaxResolution()
{
	return m_finalMaxResolution;
}


IFXINLINE IFXResManager* CIFXCLODManager::GetResManager(U32 i)
{
	return &m_pControllers[i];
}


IFXINLINE IFXMeshGroup* CIFXCLODManager::GetMeshGroup(void)
{
	return m_pMeshGroup;
}


IFXINLINE IFXUpdatesGroup* CIFXCLODManager::GetUpdatesGroup(void)
{
	return m_pUpdatesGroup;
}

#endif
