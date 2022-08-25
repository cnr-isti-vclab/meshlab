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
@file  CIFXCLODAuthor.h
 */

#ifndef __CIFXCLODAUTHOR_H__
#define __CIFXCLODAUTHOR_H__

#include "CLODGenerator.h"
#include "IFXAuthorCLODGen.h"

class CIFXAuthorCLODGen : virtual public IFXAuthorCLODGen
{
	          CIFXAuthorCLODGen();
	virtual  ~CIFXAuthorCLODGen();
	friend 
	IFXRESULT IFXAPI_CALLTYPE CIFXAuthorCLODGen_Factory(IFXREFIID iid, void** ppv);

public:
	// IFXUnknown
	U32 IFXAPI        AddRef ();
	U32 IFXAPI        Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID riid, void **ppv);

	// IFXAuthorCLODGen
	IFXRESULT IFXAPI 	Generate(const IFXAuthorCLODGenParam*, IFXProgressCallback*, 
								 IFXAuthorCLODMesh*, 
								 IFXAuthorCLODMesh**, IFXAuthorMeshMap**);


private:
	U32 m_uRefCount;
	U32 m_TotalPairs;
};

#endif // __CIFXCLODAUTHOR_H__
