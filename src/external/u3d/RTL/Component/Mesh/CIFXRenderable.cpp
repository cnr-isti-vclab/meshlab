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
//
//  CIFXRenderable.cpp
//
//  DESCRIPTION
//
//  NOTES
//
//***************************************************************************
#include "CIFXRenderable.h"
#include "IFXCoreCIDs.h"
#include "IFXCOM.h"

CIFXRenderable::CIFXRenderable()
{
	m_uRefCount = 0;
	m_uNumElements = 0;
	m_ppShaders = NULL;
	m_bEnabled = TRUE;
}

CIFXRenderable::~CIFXRenderable()
{
	Deallocate();
}


IFXRESULT CIFXRenderable::GetElementShaderList( U32 uInElement, IFXShaderList** out_ppShaderList )
{
	IFXRESULT result = IFX_OK;
	IFXASSERT(out_ppShaderList);
	if(uInElement >= m_uNumElements)
	{
		return IFX_E_INVALID_RANGE;
	}

	*out_ppShaderList = m_ppShaders[uInElement];
	IFXASSERT(m_ppShaders[uInElement]);
	m_ppShaders[uInElement]->AddRef();

	return result;
}

IFXRESULT CIFXRenderable::SetElementShaderList( U32 uInElement, IFXShaderList* in_pShaderList )
{
  IFXRESULT result = IFX_OK;

  if(uInElement >= m_uNumElements)
  {
    result = IFX_E_INVALID_RANGE;
  }
  if(in_pShaderList == NULL)
  {
    result = IFX_E_INVALID_POINTER;
  }

  if(IFXSUCCESS(result))
  {
    IFXRELEASE(m_ppShaders[uInElement]);
    m_ppShaders[uInElement] = in_pShaderList;
    in_pShaderList->AddRef();
  }

  return result;
}


void CIFXRenderable::Deallocate()
{
	U32 i;
	for(i = 0; i < m_uNumElements; ++i)
	{
		IFXRELEASE(m_ppShaders[i]);
	}
	IFXDELETE_ARRAY(m_ppShaders);
	DeallocateObject();
	m_uNumElements = 0;
}


IFXRESULT CIFXRenderable::AllocateShaders(IFXShaderList** in_ppData, BOOL in_bCopy)
{
	IFXRESULT result = IFX_OK;
	U32 i;

	if(!m_ppShaders)
		m_ppShaders = new IFXShaderList*[m_uNumElements];

	if(m_ppShaders)
	{
		for (i = 0; i < m_uNumElements; ++i)
			m_ppShaders[i] = NULL;

		if(in_ppData)
		{
			for(i = 0; i < m_uNumElements; ++i)
			{
				IFXRELEASE(m_ppShaders[i]);
				if(in_bCopy)
				{
					IFXCreateComponent(CID_IFXShaderList, IID_IFXShaderList, (void**)&(m_ppShaders[i]));
					if(m_ppShaders[i])
						m_ppShaders[i]->Copy(in_ppData[i]);
				}
				else
				{
					m_ppShaders[i] = in_ppData[i];
					m_ppShaders[i]->AddRef();
				}
			}
		}
		else
		{
			for(i = 0; i < m_uNumElements && IFXSUCCESS( result ); ++i)
			{
				IFXCreateComponent(CID_IFXShaderList, IID_IFXShaderList, (void**)&(m_ppShaders[i]));
				if(m_ppShaders[i])
					result = m_ppShaders[i]->Allocate(1);
			}
		}
	}
	else
		result = IFX_E_OUT_OF_MEMORY;

  return result;
}

IFXRESULT IFXAPI CIFXRenderable::GetShaderDataArray(IFXShaderList*** out_pppShaders)
{
	IFXASSERT(out_pppShaders);
	*out_pppShaders = m_ppShaders;

	return IFX_OK;
}
