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
//  CIFXShaderList.cpp
//
//  DESCRIPTION
//
//  NOTES
//
//***************************************************************************

#include "IFXSceneGraphPCH.h"
#include "CIFXShaderList.h"

IFXRESULT IFXAPI_CALLTYPE CIFXShaderList_Factory(IFXREFIID riid, void **ppv)
{

	IFXRESULT result;

	if ( ppv )
	{
		// Create the CIFXClassName component.
		CIFXShaderList *pObj = new CIFXShaderList;

		if ( pObj )
		{
			// Perform a temporary AddRef for our usage of the component.
			pObj->AddRef();

			result = pObj->Allocate(1, 0);
			if(IFXSUCCESS(result))
			{
				// Attempt to obtain a pointer to the requested interface.
				result = pObj->QueryInterface( riid, ppv );
			}

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pObj->Release();
		}
		else result = IFX_E_OUT_OF_MEMORY;
	}
	else result = IFX_E_INVALID_POINTER;

	return result;
}


CIFXShaderList::CIFXShaderList()
{
	m_uRefCount =0;
	m_DefaultValue = 0;
	m_pShaders = NULL;
	m_NumShaders = 0;
	m_MaxShaders = 0;
}


CIFXShaderList::~CIFXShaderList()
{
	IFXDELETE_ARRAY(m_pShaders);
}

// IFXUnknown
U32 CIFXShaderList::AddRef()
{
	return ++m_uRefCount;
}


U32 CIFXShaderList::Release()
{
	if (m_uRefCount == 1)
	{
		delete this ;
		return 0 ;
	}
	else
		return (--m_uRefCount);
}


IFXRESULT CIFXShaderList::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result  = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXUnknown* ) this;
		else if ( interfaceId == IID_IFXShaderList)
			*ppInterface = ( IFXShaderList* ) this;
		else
		{
			*ppInterface = NULL;
			result = IFX_E_UNSUPPORTED;
		}

		if ( IFXSUCCESS( result ) )
			AddRef();
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


// IFXShaderList
IFXRESULT CIFXShaderList::Allocate(U32 in_Size, U32 DefaultValue)
{
	m_DefaultValue = DefaultValue;

	Realloc(in_Size);

	U32 i;
	for( i = 0; i < m_NumShaders; ++i)
	{
		m_pShaders[i] = DefaultValue;
	}
	return IFX_OK;
}

IFXRESULT CIFXShaderList::Copy(IFXShaderList* in_pSrc)
{
	U32 NewNumShaders = in_pSrc->GetNumShaders();
	Realloc(NewNumShaders);

	m_NumShaders = NewNumShaders;
	U32 i;
	for( i = 0; i < m_NumShaders; ++i)
	{
		in_pSrc->GetShader(i, m_pShaders + i);
	}
	return IFX_OK;
}

IFXRESULT CIFXShaderList::Overlay(IFXShaderList* in_pSrc)
{
	U32 NewNumShaders = in_pSrc->GetNumShaders();
	Realloc(IFXMAX(NewNumShaders, m_NumShaders));

	U32 i;
	for( i = 0; i < NewNumShaders; ++i)
	{
		U32 ShaderId;
		in_pSrc->GetShader(i, &ShaderId);
		if(ShaderId == IFXSHADERLIST_HARD_END)
		{
			Realloc(i);
			return IFX_OK;
		}
		else if(ShaderId != IFXSHADERLIST_DEFAULT_VALUE)
		{
			m_pShaders[i] = ShaderId;
		}

	}
	return IFX_OK;
}

IFXRESULT CIFXShaderList::Equals(IFXShaderList* in_Source)
{
	U32 ShCnt = in_Source->GetNumShaders();
	if(ShCnt != m_NumShaders)
	{ /// @todo: there is a flaw in this logic if one shader set is longer due to
		/// a Hard end.
		return IFX_E_UNDEFINED;
	}

	U32 i;
	for( i = 0; i < m_NumShaders; ++i)
	{
		U32 ShaderId = 0;
		in_Source->GetShader(i, &ShaderId);
		if(ShaderId != m_pShaders[i])
		{
			return IFX_E_UNDEFINED;
		}
	}

	return IFX_OK;
}

U32 CIFXShaderList::GetNumShaders()
{
	return m_NumShaders;
}

IFXRESULT CIFXShaderList::SetNumShaders(U32 in_Size)
{
	return Realloc(in_Size);
}

U32 CIFXShaderList::GetNumActualShaders()
{
	U32 cnt = 0;
	U32 i;
	for( i = 0; i < m_NumShaders; ++i)
	{
		if(m_pShaders[i] != IFXSHADERLIST_DEFAULT_VALUE)
		{
			cnt++;
		}
	}
	return cnt;
}


IFXRESULT CIFXShaderList::SetShader(U32 in_Idx, U32 in_Shader)
{
	U32 oldSize = m_NumShaders;
	if(in_Idx >= m_NumShaders)
	{
		Realloc(in_Idx+1);
	}

	m_pShaders[in_Idx] = in_Shader;

	if(in_Shader == IFXSHADERLIST_HARD_END)
	{
		Realloc(in_Idx+1);
	}
	else if(in_Shader == IFXSHADERLIST_DEFAULT_VALUE
		&& in_Idx < oldSize -1) // wack all of the defaults off the end of the list
	{
		while(in_Idx && m_pShaders[in_Idx] == IFXSHADERLIST_DEFAULT_VALUE)
		{
			--in_Idx;
		}
		m_NumShaders = in_Idx + 1;
		Realloc(in_Idx+1);
	}


	return IFX_OK;
}

IFXRESULT CIFXShaderList::GetShader(U32 in_Idx, U32* out_pShader)
{
	IFXASSERT(out_pShader);
	if(in_Idx > m_NumShaders)
	{
		return IFX_E_INVALID_INDEX;
	}

	*out_pShader = m_pShaders[in_Idx];

	return IFX_OK;
}


IFXRESULT CIFXShaderList::Realloc(U32 in_Size)
{
	if(in_Size > m_MaxShaders || in_Size < (m_MaxShaders -4))
	{
		U32 Size = in_Size + (4 - (in_Size % 4));
		U32* pNewShaderArray = new U32[Size];
		if(m_pShaders)
		{
			memcpy(pNewShaderArray, m_pShaders, sizeof(U32) * IFXMIN(m_NumShaders, in_Size));
		}

		if(in_Size > m_NumShaders)
		{
			U32 i;
			for( i = m_NumShaders; i < in_Size; ++i)
			{
				pNewShaderArray[i] = m_DefaultValue;
			}
		}

		IFXDELETE_ARRAY(m_pShaders);
		m_pShaders = pNewShaderArray;
		m_NumShaders = in_Size;
		m_MaxShaders = Size;
	}
	else
	{ // Old array big enough just resize it
		if(in_Size > m_NumShaders)
		{
			U32 i;
			for( i = m_NumShaders; i < in_Size; ++i)
			{
				m_pShaders[i] = m_DefaultValue;
			}
		}
		m_NumShaders = in_Size;
	}

	return IFX_OK;
}
