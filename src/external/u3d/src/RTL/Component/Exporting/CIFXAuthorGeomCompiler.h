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
//
//  CIFXAuthorGeomCompiler.h
//
//	DESCRIPTION
//      Implementation of CIFXAuthorGeomCompiler.
//
//	NOTES
//
//***************************************************************************

#ifndef __CIFXAuthorGeomCompiler_H__
#define __CIFXAuthorGeomCompiler_H__

#include "IFXAuthorGeomCompiler.h"

class CIFXAuthorGeomCompilerProgress;

class CIFXAuthorGeomCompiler : public IFXAuthorGeomCompiler
{
public:
	CIFXAuthorGeomCompiler();
	virtual ~CIFXAuthorGeomCompiler();
	// Member functions:  IFXUnknown.
	U32 IFXAPI 			AddRef ();
	U32 IFXAPI 			Release ();
	IFXRESULT IFXAPI 	QueryInterface ( IFXREFIID interfaceId, void** ppInterface );

	// IFXAuthorGeomCompiler
	IFXRESULT IFXAPI	SetSceneGraph(IFXSceneGraph*);
	IFXRESULT IFXAPI	 Compile(IFXString& rName, IFXAuthorMesh*, 
					IFXAuthorCLODResource** out_ppResource,
					BOOL forceCompress,
					IFXAuthorGeomCompilerParams*);

	IFXRESULT IFXAPI	 Recompile(IFXAuthorCLODResource* in_pResource,
					IFXAuthorMeshMap* in_pMeshMap, 
					IFXAuthorMeshMap** out_ppMeshMap);


private:
	U32	m_refCount;	// Number of outstanding references to the object.

	IFXAuthorGeomCompilerParams* m_pParams;
	IFXSceneGraph* m_pSceneGraph;

	IFXRESULT ValidateParams(IFXAuthorGeomCompilerParams*);

	friend class CIFXAuthorGeomCompilerProgress;
};

#endif
