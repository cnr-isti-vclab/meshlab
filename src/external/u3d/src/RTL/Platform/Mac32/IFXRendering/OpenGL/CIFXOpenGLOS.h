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
// CIFXOpenGLOS.h
#ifndef CIFXOPENGLOS_H
#define CIFXOPENGLOS_H

#include "CIFXOpenGL.h"

class CIFXOpenGLOS : public IFXOpenGLOS, public CIFXOpenGL
{
	U32 m_refCount;
public:
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	friend IFXRESULT CIFXOpenGLOSFactory(IFXREFIID intId, void** ppUnk);

	// IFXOpenGL Methods
	virtual IFXRESULT IFXAPI LoadOpenGL();
	virtual IFXRESULT IFXAPI UnloadOpenGL();

	IFXRESULT IFXAPI LoadOSGL();

	IFXRESULT IFXAPI LoadExtensions();

	static void IFXAPI Shutdown()
	{
		if(ms_pSingleton)
			ms_pSingleton->UnloadOpenGL();
		IFXDELETE(ms_pSingleton);
	}

protected:
	CIFXOpenGLOS();
	virtual ~CIFXOpenGLOS() {}

	IFXRESULT IFXAPI Construct();

	static CIFXOpenGLOS* GetSingleton()
	{
		return ms_pSingleton;
	}

	virtual IFXOGLPROC IFXAPI GetOpenGLFunc(const char* szFuncName);
	virtual IFXOGLPROC IFXAPI GetOGLExtensionFunc(const char* szFuncName);

	static CIFXOpenGLOS* ms_pSingleton;
};

#endif
