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

/**
	@file	IFXAuthorLineSet.cpp

    Implementation of the CFXAuthorLinetSet class
*/

#include "CIFXAuthorLineSet.h"
#include "IFXModifier.h"
#include "IFXDebug.h"
#include "IFXAuthor.h"
#include "IFXCoreCIDs.h"
#include <float.h>


// Helper template functions
//
template<class T>
void ReallocDataBlock(IFXAutoPtr<T>& rData, U32 curSize, U32 newSize )
{
	T* pNewData = NULL;
	T* pOldData = rData;

	if(newSize)
	{
		pNewData = new T[newSize];
	}

	if(pNewData && pOldData)
	{
		int blocksToCopy = curSize<newSize?curSize:newSize;
		memcpy(pNewData, pOldData, sizeof(T) * blocksToCopy);
	}

	rData = pNewData; // causes deletion of old data and AutoPtr takes ownership of new data
}

template<class T>
inline void DeallocateDataBlock(IFXAutoPtr<T>& rData)
{
	IFXAutoPtr<T> pOldData = rData;
	IFXASSERT(! rData);
}

/* *********************************************************************************************
*
*       CIFXAuthorLineSet
*
************************************************************************************************/
CIFXAuthorLineSet::CIFXAuthorLineSet() : 
	m_CurLineSetDesc(), m_MaxLineSetDesc() 
{
	m_uRefCount   = 0;
}


CIFXAuthorLineSet::~CIFXAuthorLineSet()
{
}


//---------------------------------------------------------------------------
//  CIFXAuthorLineSet_Factory
//
//  This is the CIFXClassName component factory function.  The
//  CIFXClassName component can be instaniated multiple times.
//---------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXAuthorLineSet_Factory( IFXREFIID interfaceId,  void**   ppInterface )
{
	IFXRESULT result;

	if ( ppInterface )
	{
		// Create the CIFXAuthorMeshGroup component.
		CIFXAuthorLineSet  *pComponent = new CIFXAuthorLineSet;

		if ( pComponent )
		{
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		}
		else
			result = IFX_E_OUT_OF_MEMORY;
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

/**************************************************************************************************
// IFXUnknown methods
**************************************************************************************************/

U32 CIFXAuthorLineSet::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXAuthorLineSet::Release()
{
	if( 1 == m_uRefCount )
	{
		delete this;
		return 0;
	}

	return --m_uRefCount;
}


IFXRESULT CIFXAuthorLineSet::QueryInterface( IFXREFIID interfaceId, void **ppInterface ) 
{
	IFXRESULT result = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXModifier )
		{
			*ppInterface = ( IFXModifier* )this;
		} 
		else if ( interfaceId == IID_IFXMarker) 
		{
			*ppInterface = ( IFXMarker* )this;
		} 
		else if ( interfaceId == IID_IFXMetaDataX ) 
		{
			*ppInterface = ( IFXMetaDataX* )this;
		} 
		else if ( interfaceId == IID_IFXUnknown ) 
		{
			*ppInterface = ( IFXUnknown* )this;
		} 
		else if ( interfaceId == IID_IFXAuthorLineSet ) 
		{
			*ppInterface = ( IFXAuthorLineSet* )this;
		} 
		else if ( interfaceId == IID_IFXAuthorLineSetAccess ) 
		{
			*ppInterface = ( IFXAuthorLineSetAccess* )this;
		} 
		else  
		{
			*ppInterface = NULL;
			result = IFX_E_UNSUPPORTED;
		}

		if ( IFXSUCCESS( result ) )
			AddRef();
	} 
	else 
	{
		result = IFX_E_INVALID_POINTER;
	}

	return result;
}

IFXAuthorLineSetDesc* CIFXAuthorLineSet::GetLineSetDesc()
{
	return &m_CurLineSetDesc;
}

IFXRESULT CIFXAuthorLineSet::SetLineSetDesc(const IFXAuthorLineSetDesc* pLineSetDesc)
{
	IFXRESULT result = IFX_OK;

	if(pLineSetDesc->m_numLines > m_MaxLineSetDesc.m_numLines
		|| pLineSetDesc->m_numPositions > m_MaxLineSetDesc.m_numPositions
		|| pLineSetDesc->m_numNormals > m_MaxLineSetDesc.m_numNormals
		|| pLineSetDesc->m_numDiffuseColors > m_MaxLineSetDesc.m_numDiffuseColors
		|| pLineSetDesc->m_numSpecularColors > m_MaxLineSetDesc.m_numSpecularColors
		|| pLineSetDesc->m_numTexCoords > m_MaxLineSetDesc.m_numTexCoords
		|| pLineSetDesc->m_numMaterials != m_MaxLineSetDesc.m_numMaterials ) 
	{
		result = IFX_E_INVALID_RANGE;
	}
	else
	{
		m_CurLineSetDesc = *pLineSetDesc;
	}
	
	return result;
}


IFXAuthorLineSetDesc* CIFXAuthorLineSet::GetMaxLineSetDesc()
{
	return &m_MaxLineSetDesc;
}


IFXRESULT CIFXAuthorLineSet::Allocate(const IFXAuthorLineSetDesc* pLineSetDesc)
{
	IFXRESULT ir = IFX_OK;
	if(!pLineSetDesc)   
	{
		ir = IFX_E_INVALID_POINTER;
	}

	// Should potentially Validate the LineSetDescription
	if(IFXSUCCESS(ir))   
	{
		if( pLineSetDesc->m_numPositions < 1  || pLineSetDesc->m_numLines < 1 )
		{
			ir = IFX_E_INVALID_RANGE;
		}
	}

	// clean up any preexisting memory allocation
	if(IFXSUCCESS(ir))
	{
		Deallocate();
	}

	if(IFXSUCCESS(ir))
	{
		// Set the cur mesh desc to have no attribs.
		m_CurLineSetDesc = *pLineSetDesc;
		m_MaxLineSetDesc = *pLineSetDesc;
	}

	if(IFXSUCCESS(ir) && m_MaxLineSetDesc.m_numPositions)
	{
		m_pPositionLines= new IFXU32Line[m_MaxLineSetDesc.m_numLines];
		if(!m_pPositionLines)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxLineSetDesc.m_numNormals)
	{
		//m_pNormalLines= new IFXU32Line[m_MaxLineSetDesc.m_numNormals];
		m_pNormalLines= new IFXU32Line[m_MaxLineSetDesc.m_numLines];
		if(!m_pNormalLines)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxLineSetDesc.m_numDiffuseColors)
	{
		m_pDiffuseLines= new IFXU32Line[m_MaxLineSetDesc.m_numLines];
		if(!m_pDiffuseLines)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxLineSetDesc.m_numSpecularColors)
	{
		m_pSpecularLines= new IFXU32Line[m_MaxLineSetDesc.m_numLines];
		if(!m_pSpecularLines)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) )
	{
		m_pMaterialsLines = new U32[m_MaxLineSetDesc.m_numLines];
		if(!m_pMaterialsLines)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	// TexLines are allocated on demand
	U32 i;
	for( i = 0; i < IFX_MAX_TEXUNITS; ++i)
	{
		if (m_pTexCoordLines[i])
		{
			m_pTexCoordLines[i] = NULL;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxLineSetDesc.m_numPositions)
	{
		m_pPositions = new IFXVector3[m_MaxLineSetDesc.m_numPositions];
		if(!m_pPositions)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxLineSetDesc.m_numNormals)
	{
		m_pNormals= new IFXVector3[m_MaxLineSetDesc.m_numNormals];
		if(!m_pNormals)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxLineSetDesc.m_numDiffuseColors)
	{
		m_pDiffuseColors= new IFXVector4[m_MaxLineSetDesc.m_numDiffuseColors];
		if(!m_pDiffuseColors)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxLineSetDesc.m_numSpecularColors)
	{
		m_pSpecularColors = new IFXVector4[m_MaxLineSetDesc.m_numSpecularColors];
		if(!m_pSpecularColors)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir)  && m_MaxLineSetDesc.m_numTexCoords)
	{
		m_pTexCoords = new IFXVector4[m_MaxLineSetDesc.m_numTexCoords];
		if(!m_pTexCoords)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxLineSetDesc.m_numMaterials)
	{
		m_pMaterials = new IFXAuthorMaterial[m_MaxLineSetDesc.m_numMaterials];

		if(!m_pMaterials)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir)) 
	{
		SetLineSetDesc(&m_MaxLineSetDesc);
	}

	if(IFXFAILURE(ir))
	{
		Deallocate();
	}

	return ir;
}

IFXRESULT CIFXAuthorLineSet::Reallocate(const IFXAuthorLineSetDesc* pLineSetDesc)
{
	IFXRESULT ir = IFX_OK;

	if(!pLineSetDesc)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(IFXSUCCESS(ir) && m_MaxLineSetDesc.m_numLines != pLineSetDesc->m_numLines)
	{
		ReallocDataBlock(m_pPositionLines, m_MaxLineSetDesc.m_numLines, pLineSetDesc->m_numLines );
		if(!m_pPositionLines)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}

		ReallocDataBlock(m_pNormalLines, m_MaxLineSetDesc.m_numLines, pLineSetDesc->m_numLines);
		if(!m_pNormalLines)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}

		ReallocDataBlock(m_pDiffuseLines, m_MaxLineSetDesc.m_numLines, pLineSetDesc->m_numLines);
		if(!m_pDiffuseLines)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}

		ReallocDataBlock(m_pSpecularLines, m_MaxLineSetDesc.m_numLines, pLineSetDesc->m_numLines);
		if(!m_pSpecularLines)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}

		ReallocDataBlock(m_pMaterialsLines, m_MaxLineSetDesc.m_numLines, pLineSetDesc->m_numLines);
		if(!m_pMaterialsLines)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}

		if(m_MaxLineSetDesc.m_numTexCoords)
		{
			U32 numLayers;

			GetNumAllocatedTexLineLayers(&numLayers);

			U32 i;
			for( i = 0; i < numLayers; ++i)
			{
				if(m_pTexCoordLines[i].IsValid())
				{
					ReallocDataBlock(m_pTexCoordLines[i], m_MaxLineSetDesc.m_numLines, pLineSetDesc->m_numLines);
					if(!m_pTexCoordLines[i])
					{
						ir = IFX_E_OUT_OF_MEMORY;
					}
				}
			}
		}
	}

	if(IFXSUCCESS(ir) && m_MaxLineSetDesc.m_numPositions != pLineSetDesc->m_numPositions)
	{
		ReallocDataBlock(m_pPositions, m_MaxLineSetDesc.m_numPositions, pLineSetDesc->m_numPositions);
		if(!m_pPositions)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxLineSetDesc.m_numNormals != pLineSetDesc->m_numNormals)
	{
		ReallocDataBlock(m_pNormals, m_MaxLineSetDesc.m_numNormals, pLineSetDesc->m_numNormals);
		if(!m_pNormals)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxLineSetDesc.m_numDiffuseColors  != pLineSetDesc->m_numDiffuseColors)
	{
		ReallocDataBlock(m_pDiffuseColors, m_MaxLineSetDesc.m_numDiffuseColors, pLineSetDesc->m_numDiffuseColors);
		if(!m_pDiffuseColors)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxLineSetDesc.m_numSpecularColors  != pLineSetDesc->m_numSpecularColors)
	{
		ReallocDataBlock(m_pSpecularColors, m_MaxLineSetDesc.m_numSpecularColors, pLineSetDesc->m_numSpecularColors);
		if(!m_pSpecularColors)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir)  && m_MaxLineSetDesc.m_numTexCoords != pLineSetDesc->m_numTexCoords)
	{
		ReallocDataBlock(m_pTexCoords, m_MaxLineSetDesc.m_numTexCoords, pLineSetDesc->m_numTexCoords);
		if(!m_pTexCoords)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxLineSetDesc.m_numMaterials != pLineSetDesc->m_numMaterials)
	{
		ReallocDataBlock(m_pMaterials, m_MaxLineSetDesc.m_numMaterials, pLineSetDesc->m_numMaterials);
		if(!m_pMaterials)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) )
	{
		m_MaxLineSetDesc = *pLineSetDesc;
	}

	if(IFXFAILURE(ir))
	{
		Deallocate();
	}

	return ir;
}

IFXRESULT CIFXAuthorLineSet::Deallocate()
{
	IFXRESULT ir = IFX_OK;
	U32 i = 0;

	if (m_pPositionLines) 
		DeallocateDataBlock(m_pPositionLines);
	if (m_pNormalLines)   
		DeallocateDataBlock(m_pNormalLines);
	if (m_pDiffuseLines)  
		DeallocateDataBlock(m_pDiffuseLines);
	if (m_pSpecularLines) 
		DeallocateDataBlock(m_pSpecularLines);
	if (m_pPositions)   
		DeallocateDataBlock(m_pPositions);
	if (m_pNormals)     
		DeallocateDataBlock(m_pNormals);
	if (m_pDiffuseColors) 
		DeallocateDataBlock(m_pDiffuseColors);
	if (m_pSpecularColors)  
		DeallocateDataBlock(m_pSpecularColors);
	if (m_pTexCoords)   
		DeallocateDataBlock(m_pTexCoords);
	if (m_pMaterialsLines)  
		DeallocateDataBlock(m_pMaterialsLines);
	if (m_pMaterials)   
		DeallocateDataBlock(m_pMaterials);

	m_pPositionLines  = NULL;
	m_pNormalLines    = NULL;
	m_pDiffuseLines   = NULL;
	m_pSpecularLines  = NULL;

	for(i = 0; i < IFX_MAX_TEXUNITS; ++i)
	{
		if (m_pTexCoordLines[i])
		{
			DeallocateDataBlock(m_pTexCoordLines[i]);
			m_pTexCoordLines[i] = NULL;
		}
	}
	m_pMaterialsLines  = NULL;

	m_pPositions    = NULL;
	m_pNormals      = NULL;
	m_pDiffuseColors  = NULL;
	m_pSpecularColors = NULL;
	m_pTexCoords    = NULL;
	m_pMaterials    = NULL;

	IFXInitStruct(&m_CurLineSetDesc);
	IFXInitStruct(&m_MaxLineSetDesc);

	return ir;
}


IFXRESULT CIFXAuthorLineSet::Copy(IFXREFIID, void** )
{
	IFXRESULT ir = IFX_OK;
	return ir;
}


IFXRESULT CIFXAuthorLineSet::GetPositionLine(U32 index,  IFXU32Line* pPositionLine)
{
	IFXRESULT ir = IFX_OK;

	if(!pPositionLine ) 
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if( index > m_MaxLineSetDesc.m_numLines ) 
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(ir))
	{
		*pPositionLine = m_pPositionLines[index];
	}

	return ir;
}

IFXRESULT CIFXAuthorLineSet::SetPositionLine(U32 index, const IFXU32Line* pPositionLine )
{
	IFXRESULT ir = IFX_OK;

	if (!pPositionLine)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxLineSetDesc.m_numLines)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			m_pPositionLines[index] = *pPositionLine;
		}
	}

	return ir;
}

IFXRESULT CIFXAuthorLineSet::GetNormalLine(U32 index,  IFXU32Line* pNormalLine )
{
	IFXRESULT ir = IFX_OK;

	if(!pNormalLine)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxLineSetDesc.m_numLines)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			*pNormalLine = m_pNormalLines[index];
		}
	}

	return ir;
}

IFXRESULT CIFXAuthorLineSet::SetNormalLine(U32 index,  const IFXU32Line* pNormalLine )
{
	IFXRESULT ir = IFX_OK;

	if (!pNormalLine)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxLineSetDesc.m_numLines)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			m_pNormalLines[index] = *pNormalLine;
		}
	}

	return ir;
}

IFXRESULT CIFXAuthorLineSet::GetDiffuseLine(U32 index, IFXU32Line* pDiffuseLine)
{
	IFXRESULT ir = IFX_OK;

	if(!pDiffuseLine)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxLineSetDesc.m_numLines)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			*pDiffuseLine = m_pDiffuseLines[index];
		}
	}

	return ir;
}

IFXRESULT CIFXAuthorLineSet::SetDiffuseLine(U32 index, const IFXU32Line* pDiffuseLine)
{
	IFXRESULT ir = IFX_OK;

	if (!pDiffuseLine)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxLineSetDesc.m_numLines)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			m_pDiffuseLines[index] = *pDiffuseLine;
		}
	}

	return ir;
}
IFXRESULT CIFXAuthorLineSet::GetSpecularLine(U32 index,  IFXU32Line* pSpecularLine)
{
	IFXRESULT ir = IFX_OK;

	if(!pSpecularLine)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxLineSetDesc.m_numLines)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			*pSpecularLine = m_pSpecularLines[index];
		}
	}

	return ir;
}

IFXRESULT CIFXAuthorLineSet::SetSpecularLine(U32 index,  const IFXU32Line* pSpecularLine)
{
	IFXRESULT ir = IFX_OK;

	if (!pSpecularLine)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxLineSetDesc.m_numLines)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			m_pSpecularLines[index] = *pSpecularLine;
		}
	}

	return ir;
}


IFXRESULT CIFXAuthorLineSet::GetNumAllocatedTexLineLayers(U32* pLayers)
{
	IFXRESULT result = IFX_OK;

	U32 valid_Layers = 0;

	if(!pLayers)
	{
		result = IFX_E_INVALID_POINTER;
	}

	if(IFXSUCCESS(result))
	{
		U32 i;
		for( i = 0; i < IFX_MAX_TEXUNITS; ++i)
		{
			if(m_pTexCoordLines[i].IsValid())
			{
				valid_Layers++;
			}
		}
	}

	*pLayers = valid_Layers;

	return result;
}


IFXRESULT CIFXAuthorLineSet::GetPositionLines(IFXU32Line** ppPositionLines )
{
	IFXRESULT ir = IFX_OK;

	if(!ppPositionLines)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(IFXSUCCESS(ir))
	{
		*ppPositionLines = m_pPositionLines;
	}

	return ir;
}

IFXRESULT CIFXAuthorLineSet::GetNormalLines(IFXU32Line** ppLines )
{
	IFXRESULT ir = IFX_OK;

	if(!ppLines)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(m_MaxLineSetDesc.m_numNormals == 0)
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(ir))
	{
		*ppLines = m_pNormalLines;
	}

	return ir;
}

IFXRESULT CIFXAuthorLineSet::GetDiffuseLines(IFXU32Line** ppLines)
{
	IFXRESULT ir = IFX_OK;

	if(!ppLines)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(m_MaxLineSetDesc.m_numDiffuseColors == 0)
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(ir))
	{
		*ppLines = m_pDiffuseLines;
	}

	return ir;
}

IFXRESULT CIFXAuthorLineSet::GetSpecularLines(IFXU32Line** ppLines )
{
	IFXRESULT ir = IFX_OK;

	if(!ppLines)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(m_MaxLineSetDesc.m_numSpecularColors == 0)
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(ir))
	{
		*ppLines = m_pSpecularLines;
	}

	return ir;
}

IFXRESULT CIFXAuthorLineSet::GetTexCoordsLines (IFXU32Line** ppTexCoordLines)
{
	IFXRESULT ir = IFX_OK;

	if(!ppTexCoordLines)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(m_MaxLineSetDesc.m_numTexCoords == 0 )
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(ir))
	{
		*ppTexCoordLines = m_pTexCoordLines[0]; // likely invalid value, but the function is never called
	}

	return ir;
}


IFXRESULT CIFXAuthorLineSet::GetMaterialsLines(U32** ppLineMaterials )
{
	IFXRESULT ir = IFX_OK;

	if(!ppLineMaterials)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	//If Lines are not exist, then we expected that Materials
	//should not be there either.  We may not need this check.
	if(m_MaxLineSetDesc.m_numLines == 0)
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(ir))
	{
		*ppLineMaterials = m_pMaterialsLines;
	}

	return ir;
}


IFXRESULT CIFXAuthorLineSet::GetTexLines(U32 index, IFXU32Line** ppTexLines )
{
	IFXRESULT ir = IFX_OK;
	//If there's a posibility that the user passes in multiple bad arg.
	//Then we need to look into additional checks, which can slow down
	//this at runtime.
	if(!ppTexLines)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(index >= IFX_MAX_TEXUNITS)
	{
		ir = IFX_E_INVALID_RANGE;
	}
	else
	{
		if(m_MaxLineSetDesc.m_numTexCoords == 0)
		{
			ir = IFX_E_INVALID_RANGE;
		}
	}

	if(IFXSUCCESS(ir) && m_pTexCoordLines[index].IsNull())
	{
		m_pTexCoordLines[index] = new IFXU32Line[m_MaxLineSetDesc.m_numLines];
		if(!m_pTexCoordLines[index])
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
		else
			memset(m_pTexCoordLines[index], 0,
			m_MaxLineSetDesc.m_numLines * sizeof(IFXU32Line));
	}

	if(IFXSUCCESS(ir))
	{
		*ppTexLines = m_pTexCoordLines[index];
	}

	return ir;
}

IFXRESULT CIFXAuthorLineSet::GetTexLine(U32 layer, U32 index,
										IFXU32Line* pLine)
{
	IFXRESULT ir = IFX_OK;

	if (layer > IFX_MAX_TEXUNITS)
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if(!pLine)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (index >= m_MaxLineSetDesc.m_numLines || !m_MaxLineSetDesc.m_numTexCoords )
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(ir) && m_pTexCoordLines[layer].IsNull())
	{
		m_pTexCoordLines[layer] = new IFXU32Line[m_MaxLineSetDesc.m_numLines];
		if(!m_pTexCoordLines[layer])
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if (IFXSUCCESS(ir))
	{
		*pLine = m_pTexCoordLines[layer][index];
	}

	return ir;
}

IFXRESULT CIFXAuthorLineSet::SetTexLine(U32 layer, U32 index,
										const IFXU32Line* pLine)
{
	IFXRESULT ir = IFX_OK;

	if (layer > IFX_MAX_TEXUNITS || !m_MaxLineSetDesc.m_numTexCoords)
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if (!pLine)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(IFXSUCCESS(ir) && m_pTexCoordLines[layer].IsNull())
	{
		m_pTexCoordLines[layer] = new IFXU32Line[m_MaxLineSetDesc.m_numLines];
		if(!m_pTexCoordLines[layer])
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if (IFXSUCCESS(ir))
	{
		m_pTexCoordLines[layer][index] = *pLine;
	}

	return ir;
}

IFXRESULT CIFXAuthorLineSet::GetLineMaterials(U32** ppLineMat)
{
	IFXRESULT result = IFX_OK;

	if( NULL != ppLineMat )
		*ppLineMat = m_pMaterialsLines;
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

IFXRESULT CIFXAuthorLineSet::GetPositions (IFXVector3** pPos )
{
	IFXRESULT ir = IFX_OK;

	if(!pPos || !m_pPositions )
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		*pPos = m_pPositions;
	}

	return ir;
}

IFXRESULT CIFXAuthorLineSet::GetNormals (IFXVector3** pNorm )
{
	IFXRESULT ir = IFX_OK;

	if(!pNorm || !m_pNormals )
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		*pNorm = m_pNormals;
	}

	return ir;
}

IFXRESULT CIFXAuthorLineSet::GetDiffuseColors( IFXVector4** pDiffColor )
{
	IFXRESULT ir = IFX_OK;

	if(!pDiffColor || !m_pDiffuseColors)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		*pDiffColor = m_pDiffuseColors;
	}

	return ir;
}

IFXRESULT CIFXAuthorLineSet::GetSpecularColors( IFXVector4** pSpecColor )
{
	IFXRESULT ir = IFX_OK;

	if(!pSpecColor || !m_pSpecularColors )
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		*pSpecColor = m_pSpecularColors;
	}

	return ir;
}

IFXRESULT CIFXAuthorLineSet::GetTexCoords ( IFXVector4** pTexCoord )
{
	IFXRESULT ir = IFX_OK;

	if(!pTexCoord || !m_pTexCoords )
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		*pTexCoord = m_pTexCoords;
	}

	return ir;
}

IFXRESULT CIFXAuthorLineSet::GetMaterials ( IFXAuthorMaterial** pMaterial )
{
	IFXRESULT ir = IFX_OK;

	if(!pMaterial || !m_pMaterials )
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		*pMaterial = m_pMaterials;
	}

	return ir;
}

IFXRESULT CIFXAuthorLineSet::GetMaterial(U32 index, IFXAuthorMaterial* pMaterial)
{
	IFXRESULT ir = IFX_OK;

	if(!pMaterial)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		if (index >= m_MaxLineSetDesc.m_numMaterials)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			*pMaterial = m_pMaterials[index];
		}
	}

	return ir;
}


IFXRESULT CIFXAuthorLineSet::SetMaterial(U32 index, const IFXAuthorMaterial* pMaterial)
{
	IFXRESULT ir = IFX_OK;

	if (index >= m_MaxLineSetDesc.m_numMaterials )
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if (!pMaterial)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		m_pMaterials[index] = *pMaterial;
	}

	return ir;
}

IFXRESULT CIFXAuthorLineSet::GetPosition(U32 index,IFXVector3* pVector3)
{
	IFXRESULT ir = IFX_OK;

	if (!pVector3)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		if (index >= m_MaxLineSetDesc.m_numPositions)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			*pVector3 = m_pPositions[index];
		}
	}

	return ir;
}

IFXRESULT CIFXAuthorLineSet::SetPosition(U32 index,const IFXVector3* pVector3)
{
	IFXRESULT ir = IFX_OK;

	if (!pVector3)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{

		if (index >= m_MaxLineSetDesc.m_numPositions )
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			m_pPositions[index] = *pVector3;
		}
	}

	return ir;
}


IFXRESULT CIFXAuthorLineSet::GetNormal(U32 index, IFXVector3* pVector3)
{
	IFXRESULT ir = IFX_OK;

	if (!pVector3)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		if (index >= m_MaxLineSetDesc.m_numNormals)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			*pVector3 = m_pNormals[index];
		}
	}

	return ir;
}

IFXRESULT CIFXAuthorLineSet::SetNormal(U32 index, const IFXVector3* pVector3)
{
	IFXRESULT ir = IFX_OK;

	if (!pVector3)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		if (index >= m_MaxLineSetDesc.m_numNormals )
		{
			ReallocDataBlock(m_pNormals, m_MaxLineSetDesc.m_numNormals, index + 1);
			if(!m_pNormals)
			{
				ir = IFX_E_OUT_OF_MEMORY;
			}
			m_MaxLineSetDesc.m_numNormals = index + 1;
		}
		if (IFXSUCCESS(ir))
		{
			m_pNormals[index] = *pVector3;
		}
	}

	return ir;
}


IFXRESULT CIFXAuthorLineSet::GetDiffuseColor(U32 index, IFXVector4* pColor)
{
	IFXRESULT ir = IFX_OK;

	if (!pColor)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		if (index >= m_MaxLineSetDesc.m_numDiffuseColors)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			*pColor = m_pDiffuseColors[index];
		}
	}

	return ir;
}


IFXRESULT CIFXAuthorLineSet::SetDiffuseColor(U32 index,IFXVector4* pColor)
{
	IFXRESULT ir = IFX_OK;

	if (!pColor)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		if (index >= m_MaxLineSetDesc.m_numDiffuseColors)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			m_pDiffuseColors[index] = *pColor;
		}
	}

	return ir;
}


IFXRESULT CIFXAuthorLineSet::GetSpecularColor(U32 index, IFXVector4* pColor)
{
	IFXRESULT ir = IFX_OK;

	if (!pColor)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		if (index >= m_MaxLineSetDesc.m_numSpecularColors)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			*pColor = m_pSpecularColors[index];
		}
	}

	return ir;
}

IFXRESULT CIFXAuthorLineSet::SetSpecularColor(U32 index, IFXVector4* pColor)
{
	IFXRESULT ir = IFX_OK;

	if (!pColor)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		if (index >= m_MaxLineSetDesc.m_numSpecularColors)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			m_pSpecularColors[index] = *pColor;
		}
	}

	return ir;
}


IFXRESULT CIFXAuthorLineSet::GetTexCoord(U32 index, IFXVector4* pVector4)
{
	IFXRESULT ir = IFX_OK;

	if (!pVector4)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(IFXSUCCESS(ir) && m_MaxLineSetDesc.m_numTexCoords )
	{
		*pVector4 = (m_pTexCoords)[index];
	}

	return ir;
}


IFXRESULT CIFXAuthorLineSet::SetTexCoord(U32 index, const IFXVector4* pVector4)
{
	IFXRESULT ir = IFX_OK;

	if (!pVector4)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		if(m_MaxLineSetDesc.m_numTexCoords)
		{
			(m_pTexCoords)[index] = *pVector4;
		}
	}

	return ir;
}


/*
*
* IFXAuthorLinesetAccess interface
*
*/
IFXRESULT CIFXAuthorLineSet::SetPositionArray(IFXVector3* pData, U32 p)
{
	IFXRESULT ir = IFX_OK;

	if (!pData)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		m_pPositions = pData;
		m_MaxLineSetDesc.m_numPositions = p;
	}

	return ir;
}


IFXRESULT CIFXAuthorLineSet::SetNormalArray(IFXVector3* pData, U32 p)
{
	IFXRESULT ir = IFX_OK;

	if (!pData)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		m_pNormals = pData;
		m_MaxLineSetDesc.m_numNormals = p;
	}

	return ir;
}


IFXRESULT CIFXAuthorLineSet::SetDiffuseColorArray(IFXVector4* pData, U32 p)
{
	IFXRESULT ir = IFX_OK;

	if (!pData)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		m_pDiffuseColors = pData;
		m_MaxLineSetDesc.m_numDiffuseColors = p;
	}

	return ir;
}


IFXRESULT CIFXAuthorLineSet::SetSpecularColorArray(IFXVector4* pData, U32 p)
{
	IFXRESULT ir = IFX_OK;

	if (!pData)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		m_pSpecularColors = pData;
		m_MaxLineSetDesc.m_numSpecularColors = p;
	}

	return ir;
}


IFXRESULT CIFXAuthorLineSet::SetTexCoordArray(IFXVector4* pData, U32 p)
{
	IFXRESULT ir = IFX_OK;

	if (!pData)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		m_pTexCoords = pData;
		m_MaxLineSetDesc.m_numTexCoords = p;
	}

	return ir;
}


IFXRESULT CIFXAuthorLineSet::GetLineMaterial(U32 index, U32* pLineMaterial)
{
	IFXRESULT ir = IFX_OK;

	if(!pLineMaterial)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		if (index >= m_MaxLineSetDesc.m_numLines)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			*pLineMaterial = m_pMaterialsLines[index];
		}
	}

	return ir;
}


IFXRESULT CIFXAuthorLineSet::SetLineMaterial(U32 index, U32 lineMaterialID)
{
	IFXRESULT ir = IFX_OK;

	if (index >= m_MaxLineSetDesc.m_numLines)
	{
		ir = IFX_E_INVALID_RANGE;
	}
	else
	{
		m_pMaterialsLines[index] = lineMaterialID;
	}

	return ir;
}

IFXVector4 CIFXAuthorLineSet::CalcBoundSphere()
{
	const float minFloats[] = { FLT_MAX, FLT_MAX, FLT_MAX, 0 };
	const float maxFloats[] = {-FLT_MAX,-FLT_MAX,-FLT_MAX, 0 };
	IFXVector4 vmin(minFloats);
	IFXVector4 vmax(maxFloats);

	IFXVector4 bound;

	// Determine the axis aligned bounding box and the number of verticies.
	IFXVector3* pVertex;

	U32 uVertexCount = m_CurLineSetDesc.m_numPositions;
	if( !m_CurLineSetDesc.m_numLines )
		uVertexCount = 0;

	U32 i;
	for( i = 0; i < uVertexCount; i++ )
	{
		pVertex = &m_pPositions[i];
		if ( pVertex->X() < vmin.X() ) vmin.X() = pVertex->X();
		if ( pVertex->X() > vmax.X() ) vmax.X() = pVertex->X();
		if ( pVertex->Y() < vmin.Y() ) vmin.Y() = pVertex->Y();
		if ( pVertex->Y() > vmax.Y() ) vmax.Y() = pVertex->Y();
		if ( pVertex->Z() < vmin.Z() ) vmin.Z() = pVertex->Z();
		if ( pVertex->Z() > vmax.Z() ) vmax.Z() = pVertex->Z();
	}

	// If there are any verticies, find the average position as the center,
	// and the distance to the furthest point as the radius.
	if ( uVertexCount )
	{
		vmin.Add(vmax);
		vmin.Scale3(0.5f);
		bound = vmin;
		F32 fMaxSquaredDistance = -FLT_MAX;
		F32 fSquaredDistance;
		IFXVector3 d;

		const U32 uVertexCount = m_CurLineSetDesc.m_numPositions;

		U32 i;
		for(  i = uVertexCount; i--; )
		{
			pVertex = &m_pPositions[i];
			d.X() = pVertex->X() - bound.X();
			d.Y() = pVertex->Y() - bound.Y();
			d.Z() = pVertex->Z() - bound.Z();
			fSquaredDistance = d.DotProduct(d);
			if ( fSquaredDistance > fMaxSquaredDistance )
				fMaxSquaredDistance = fSquaredDistance;
		}

		bound.A() = sqrtf( fMaxSquaredDistance );
	}
	else
		bound.Set( 0.0, 0.0, 0.0, 0.0 );

	return bound;
}
