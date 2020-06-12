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
// CIFXMeshGroup.h
#ifndef CIFX_MESHGROUP_H
#define CIFX_MESHGROUP_H

#include "IFXMesh.h"
#include "IFXBoundHierarchyMgr.h"
#include "CIFXSubject.h"
#include "CIFXRenderable.h"
#include "IFXCoreCIDs.h"

class CIFXMeshGroup : private CIFXSubject,
              virtual public  IFXMeshGroup,
              virtual public  CIFXRenderable,
			  virtual public  IFXBoundHierarchyMgr
{
public:
	friend IFXRESULT IFXAPI_CALLTYPE CIFXMeshGroupFactory(IFXREFIID intId, void** ppUnk);
	
	// IFXUnknown  
	U32 IFXAPI        AddRef ();
	U32 IFXAPI        Release ();
	IFXRESULT IFXAPI  QueryInterface (     IFXREFIID           riid, 
	                              void**              ppv );
	// IFXRenderable
	IFXRESULT IFXAPI   TransferData(IFXRenderable&, BOOL, BOOL);
	IFXREFIID IFXAPI   GetElementType() { return IID_IFXMesh; }
	void**    GetElementList() { return (void**)m_ppMeshes; }

	// IFXMeshGroup
	IFXRESULT IFXAPI Allocate(U32 uNumMeshes);
	IFXRESULT IFXAPI TransferData(IFXMeshGroup&, IFXMeshAttributes, IFXMeshAttributes, BOOL);

	U32 IFXAPI GetNumMeshes() { return m_uNumElements; };
	
	IFXRESULT IFXAPI GetMesh(U32 uMeshNum, IFXMesh*& pMesh);
	IFXRESULT IFXAPI SetMesh(U32 uMeshNum, IFXMesh* pMesh);
	
	IFXRESULT IFXAPI ReverseNormals();

	const IFXVector4& IFXAPI CalcBoundingSphere();

	// IFXBoundHierarchyMgr
	IFXRESULT IFXAPI   GetBoundHierarchy(  IFXBoundHierarchy** ppOutBH );

private:
	CIFXMeshGroup();
	virtual ~CIFXMeshGroup();

	virtual void IFXAPI  DeallocateObject();
private:
	
	U32 m_uRefCount;

	IFXMesh** m_ppMeshes;

	// IFXBoundHierarchyMgr
	IFXBoundHierarchy* m_pBoundHierarchy;

	IFXVector4 m_bound;
};

#endif
