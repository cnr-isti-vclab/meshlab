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
// CIFXNeighborMesh.h

#ifndef CIFX_NEIGHBORMESH_H
#define CIFX_NEIGHBORMESH_H

#include "IFXUnknown.h"
#include "IFXNeighborMesh.h"

class CIFXNeighborMesh : virtual public IFXNeighborMesh, virtual public IFXUnknown
{
	U32 m_refCount;
public:
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	friend IFXRESULT IFXAPI_CALLTYPE CIFXNeighborMeshFactory(IFXREFIID intId, void** ppPtr);

	// Allocate memory for neighbor mesh in a parallel array
	// fashion, according to the sizes in IFXMeshGroup.
	//
	// Will return IFX_E_OUT_OF_MEMORY if a memory allocation fails.
    IFXRESULT IFXAPI Allocate(IFXMeshGroup& rMeshGroup);
	
	// Frees memory for neighbor mesh.
	void IFXAPI Deallocate();

	// Builds the neighbormesh from the specified Meshgroup and 
	// vertex map
	IFXRESULT IFXAPI Build(IFXMeshGroup& rInMeshGroup, IFXVertexMapGroup* pInVertexMap);
	
	// Something needed for Subdiv
	IFXRESULT IFXAPI MarkAttributeDiscontinuities(IFXMeshGroup& rMeshGroup);
			
protected:
	CIFXNeighborMesh();
	virtual ~CIFXNeighborMesh();
	
	virtual IFXRESULT IFXAPI Construct();

	IFXRESULT IFXAPI BuildLinks();

	IFXVertexMapGroup* m_pVertexMapGroup;
};


#endif
