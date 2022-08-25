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
//  CIFXShaderList.h
//
//	DESCRIPTION
//		This class implements common Shader functionality. It is not
//		ment to be instantiated.  
//
//	NOTES
//
//***************************************************************************
#ifndef __CIFXShaderList_H__
#define __CIFXShaderList_H__

#include "IFXShaderList.h"

class CIFXShaderList : virtual public   IFXShaderList
{
private:
	CIFXShaderList();
	virtual ~CIFXShaderList();
	friend  IFXRESULT IFXAPI_CALLTYPE CIFXShaderList_Factory(IFXREFIID iid, void** ppv);

public:
	
	// IFXUnknown
	U32 IFXAPI        AddRef ();
	U32 IFXAPI        Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID riid, void** ppv);


	// IFXShaderList
	IFXRESULT  IFXAPI 	 Allocate(U32, U32 DefaultValue = 0);
	IFXRESULT  IFXAPI 	 Copy(IFXShaderList*);
	IFXRESULT  IFXAPI 	 Overlay(IFXShaderList*);
	IFXRESULT  IFXAPI 	 Equals(IFXShaderList* in_Source);

	U32 IFXAPI GetNumShaders();
	IFXRESULT  IFXAPI 	 SetNumShaders(U32 );
	U32 IFXAPI GetNumActualShaders(); // shaders values not set to IFXShaderList_DEFAULT_VALUE
	IFXRESULT  IFXAPI 	 SetShader(U32 in_Idx, U32 in_Shader);
	IFXRESULT  IFXAPI 	 GetShader(U32 in_Idx, U32* out_pShader);

private:
	IFXRESULT Realloc(U32 in_Size);
private:
	U32 m_uRefCount;
	U32 m_DefaultValue;
	U32* m_pShaders;
	U32 m_NumShaders;
	U32 m_MaxShaders;
};


#endif
