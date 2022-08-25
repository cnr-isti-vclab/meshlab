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
	@file	CIFXFileReference.h

			This class implements common FileReference functionality. It is not
			ment to be instantiated.

	@note	The intent is to simplify the development of new FileReferences by
			inheriting and forwarding calls to this implementation.

*/
#ifndef __CIFXFILEREFERENCE_H__
#define __CIFXFILEREFERENCE_H__

#include "IFXSceneGraph.h"
#include "CIFXMarker.h"
#include "IFXFileReference.h"

class CIFXFileReference : private CIFXMarker, public IFXFileReference
{
public:
	// IFXUnknown
	U32 IFXAPI        AddRef ();
	U32 IFXAPI        Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID riid, void** ppv);

	// IFXMarkerX
	void IFXAPI  GetEncoderX (IFXEncoderX*& rpEncoderX);

	// IFXFileReference
	virtual void IFXAPI  SetScopeName(const IFXString& ScopeName);
	virtual void IFXAPI  GetScopeName(IFXString& rScopeName);
	virtual void IFXAPI  SetFileURLs(const IFXFileURLs& FileURLs);
	virtual void IFXAPI  GetFileURLs(IFXFileURLs& rFileURLs);
	virtual void IFXAPI  SetObjectFilters(const IFXObjectFilters& ObjectFilters);
	virtual void IFXAPI  GetObjectFilters(IFXObjectFilters& rObjectFilters);
	virtual void IFXAPI  SetCollisionPolicy(const IFXCollisionPolicy& CollisionPolicy);
	virtual void IFXAPI  GetCollisionPolicy(IFXCollisionPolicy& rCollisionPolicy);
	virtual void IFXAPI  SetWorldAlias(const IFXString& WorldAlias);
	virtual void IFXAPI  GetWorldAlias(IFXString& rWorldAlias);

	friend IFXRESULT IFXAPI_CALLTYPE CIFXFileReference_Factory(IFXREFIID iid, void** ppv);

protected:
	CIFXFileReference();
	virtual ~CIFXFileReference();

	IFXFileReferenceParams m_FileReferenceParams;

	// IFXUnknown
	U32 m_uRefCount; 
};

#endif
