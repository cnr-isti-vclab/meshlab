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
	@file	CIFXAuthorPointSet.cpp
*/

#include "CIFXAuthorPointSet.h"
#include "IFXDebug.h"
#include "IFXAuthor.h"
#include "IFXCoreCIDs.h"
#include "IFXMarker.h"
#include "IFXModifier.h"
#include "IFXMetaDataX.h"

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

	// causes deletion of old data and AutoPtr takes ownership of new data
	rData = pNewData;
}


template<class T>
inline void DeallocateDataBlock(IFXAutoPtr<T>& rData)
{
	IFXAutoPtr<T> pOldData = rData;
	IFXASSERT(!rData);
}


CIFXAuthorPointSet::CIFXAuthorPointSet() 
:	m_CurPointSetDesc(),
	m_MaxPointSetDesc() 
{
	m_uRefCount   = 0;
}


CIFXAuthorPointSet::~CIFXAuthorPointSet()
{
}


//---------------------------------------------------------------------------
//  CIFXAuthorPointSet_Factory
//
//  This is the CIFXClassName component factory function.  The
//  CIFXClassName component can be instaniated multiple times.
//---------------------------------------------------------------------------


IFXRESULT IFXAPI_CALLTYPE CIFXAuthorPointSet_Factory( 
											IFXREFIID interfaceId,  
											void**   ppInterface )
{
	IFXRESULT result;

	if ( ppInterface )
	{
		// Create the CIFXAuthorMeshGroup component.
		CIFXAuthorPointSet* pComponent = new CIFXAuthorPointSet;

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


U32 CIFXAuthorPointSet::AddRef()
{
	return ++m_uRefCount;
}


U32 CIFXAuthorPointSet::Release()
{
	if( 1 == m_uRefCount )
	{
		delete this;
		return 0;
	}
	return --m_uRefCount;
}


IFXRESULT CIFXAuthorPointSet::QueryInterface( 
									IFXREFIID interfaceId, void **ppInterface )
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
		else if ( interfaceId == IID_IFXAuthorPointSet ) 
		{
			*ppInterface = ( IFXAuthorPointSet* )this;
		} 
		else if ( interfaceId == IID_IFXAuthorPointSetAccess ) 
		{
			*ppInterface = ( IFXAuthorPointSetAccess* )this;
		} 
		else  
		{
			*ppInterface = NULL;
			result = IFX_E_UNSUPPORTED;
		}
		if ( IFXSUCCESS( result ) ) 
		{
			AddRef();
		}
	} 
	else 
	{
		result = IFX_E_INVALID_POINTER;
	}

	return result;
}

IFXAuthorPointSetDesc* CIFXAuthorPointSet::GetPointSetDesc()
{
	return &m_CurPointSetDesc;
}

IFXRESULT CIFXAuthorPointSet::SetPointSetDesc(
									const IFXAuthorPointSetDesc* pPointSetDesc)
{
	IFXRESULT result = IFX_OK;

	if( NULL == pPointSetDesc)
	{
		result = IFX_E_INVALID_POINTER;
	}
	else if(	pPointSetDesc->m_numPoints > m_MaxPointSetDesc.m_numPoints || 
		pPointSetDesc->m_numPositions > m_MaxPointSetDesc.m_numPositions || 
		pPointSetDesc->m_numNormals > m_MaxPointSetDesc.m_numNormals || 
		pPointSetDesc->m_numDiffuseColors > m_MaxPointSetDesc.m_numDiffuseColors ||
		pPointSetDesc->m_numSpecularColors > m_MaxPointSetDesc.m_numSpecularColors ||
		pPointSetDesc->m_numTexCoords > m_MaxPointSetDesc.m_numTexCoords || 
		pPointSetDesc->m_numMaterials != m_MaxPointSetDesc.m_numMaterials )
	{
		result = IFX_E_INVALID_RANGE;
	}
	else
	{
		m_CurPointSetDesc = *pPointSetDesc;
	}

	return result;
}


IFXAuthorPointSetDesc* CIFXAuthorPointSet::GetMaxPointSetDesc()
{
	return &m_MaxPointSetDesc;
}


IFXRESULT CIFXAuthorPointSet::Allocate(const IFXAuthorPointSetDesc* pPointSetDesc)
{
	IFXRESULT ir = IFX_OK;

	if(!pPointSetDesc)   
	{
		ir = IFX_E_INVALID_POINTER;
	}

	// Should potentially Validate the PointSetDescription
	if(IFXSUCCESS(ir))   
	{
		if( pPointSetDesc->m_numPositions < 1  || pPointSetDesc->m_numPoints < 1 ) 
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
		m_CurPointSetDesc = *pPointSetDesc;
		m_MaxPointSetDesc = *pPointSetDesc;
	}

	if(IFXSUCCESS(ir) && m_MaxPointSetDesc.m_numPositions)
	{
		m_pPositionPoints = new U32[m_MaxPointSetDesc.m_numPoints];
		if(!m_pPositionPoints)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxPointSetDesc.m_numNormals)
	{
		m_pNormalPoints = new U32[m_MaxPointSetDesc.m_numPoints];
		if(!m_pNormalPoints)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxPointSetDesc.m_numDiffuseColors)
	{
		m_pDiffusePoints = new U32[m_MaxPointSetDesc.m_numPoints];
		if(!m_pDiffusePoints)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxPointSetDesc.m_numSpecularColors)
	{
		m_pSpecularPoints = new U32[m_MaxPointSetDesc.m_numPoints];
		if(!m_pSpecularPoints)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) )
	{
		m_pMaterialsPoints = new U32[m_MaxPointSetDesc.m_numPoints];
		if(!m_pMaterialsPoints)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	// TexPoints are allocated on demand
	U32 i;
	for( i = 0; i < IFX_MAX_TEXUNITS; ++i)
	{
		if (m_pTexCoordPoints[i])
		{
			m_pTexCoordPoints[i] = NULL;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxPointSetDesc.m_numPositions)
	{
		m_pPositions = new IFXVector3[m_MaxPointSetDesc.m_numPositions];
		if(!m_pPositions)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxPointSetDesc.m_numNormals)
	{
		m_pNormals= new IFXVector3[m_MaxPointSetDesc.m_numNormals];
		if(!m_pNormals)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxPointSetDesc.m_numDiffuseColors)
	{
		m_pDiffuseColors = new IFXVector4[m_MaxPointSetDesc.m_numDiffuseColors];
		if(!m_pDiffuseColors)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxPointSetDesc.m_numSpecularColors)
	{
		m_pSpecularColors = new IFXVector4[m_MaxPointSetDesc.m_numSpecularColors];
		if(!m_pSpecularColors)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir)  && m_MaxPointSetDesc.m_numTexCoords)
	{
		m_pTexCoords = new IFXVector4[m_MaxPointSetDesc.m_numTexCoords];
		if(!m_pTexCoords)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxPointSetDesc.m_numMaterials)
	{
		m_pMaterials = new IFXAuthorMaterial[m_MaxPointSetDesc.m_numMaterials];

		if(!m_pMaterials)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir)) 
	{
		SetPointSetDesc(&m_MaxPointSetDesc);
	}

	if(IFXFAILURE(ir))
	{
		Deallocate();
	}

	return ir;
}

IFXRESULT CIFXAuthorPointSet::Reallocate(const IFXAuthorPointSetDesc* pPointSetDesc)
{
	IFXRESULT ir = IFX_OK;

	if(!pPointSetDesc)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(IFXSUCCESS(ir) && m_MaxPointSetDesc.m_numPoints != pPointSetDesc->m_numPoints)
	{
		ReallocDataBlock(
				m_pPositionPoints, 
				m_MaxPointSetDesc.m_numPoints, 
				pPointSetDesc->m_numPoints );

		if(!m_pPositionPoints)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}

		ReallocDataBlock(
				m_pNormalPoints, 
				m_MaxPointSetDesc.m_numPoints, 
				pPointSetDesc->m_numPoints);
		if(!m_pNormalPoints)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}

		ReallocDataBlock(
				m_pDiffusePoints, 
				m_MaxPointSetDesc.m_numPoints, 
				pPointSetDesc->m_numPoints);
		if(!m_pDiffusePoints)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}

		ReallocDataBlock(
				m_pSpecularPoints, 
				m_MaxPointSetDesc.m_numPoints, 
				pPointSetDesc->m_numPoints);
		if(!m_pSpecularPoints)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}

		ReallocDataBlock(
				m_pMaterialsPoints, 
				m_MaxPointSetDesc.m_numPoints, 
				pPointSetDesc->m_numPoints);
		if(!m_pMaterialsPoints)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}

		if(m_MaxPointSetDesc.m_numTexCoords)
		{
			U32 numLayers;

			GetNumAllocatedTexPointLayers(&numLayers);

			U32 i;
			for( i = 0; i < numLayers; ++i)
			{
				if(m_pTexCoordPoints[i].IsValid())
				{
					ReallocDataBlock(
							m_pTexCoordPoints[i], 
							m_MaxPointSetDesc.m_numPoints, 
							pPointSetDesc->m_numPoints);
					if(!m_pTexCoordPoints[i])
					{
						ir = IFX_E_OUT_OF_MEMORY;
					}
				}
			}
		}
	}

	if( IFXSUCCESS(ir) && 
		m_MaxPointSetDesc.m_numPositions != pPointSetDesc->m_numPositions)
	{
		ReallocDataBlock(
				m_pPositions, 
				m_MaxPointSetDesc.m_numPositions, 
				pPointSetDesc->m_numPositions);
		if(!m_pPositions)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}


	if( IFXSUCCESS(ir) && 
		m_MaxPointSetDesc.m_numNormals != pPointSetDesc->m_numNormals)
	{
		ReallocDataBlock(
				m_pNormals, 
				m_MaxPointSetDesc.m_numNormals, 
				pPointSetDesc->m_numNormals);
		if(!m_pNormals)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if( IFXSUCCESS(ir) && 
		m_MaxPointSetDesc.m_numDiffuseColors  != pPointSetDesc->m_numDiffuseColors)
	{
		ReallocDataBlock(
				m_pDiffuseColors, 
				m_MaxPointSetDesc.m_numDiffuseColors, 
				pPointSetDesc->m_numDiffuseColors);
		if(!m_pDiffuseColors)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if( IFXSUCCESS(ir) && 
		m_MaxPointSetDesc.m_numSpecularColors != pPointSetDesc->m_numSpecularColors)
	{
		ReallocDataBlock(
				m_pSpecularColors, 
				m_MaxPointSetDesc.m_numSpecularColors, 
				pPointSetDesc->m_numSpecularColors);
		if(!m_pSpecularColors)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if( IFXSUCCESS(ir) && 
		m_MaxPointSetDesc.m_numTexCoords != pPointSetDesc->m_numTexCoords)
	{
		ReallocDataBlock(
				m_pTexCoords, 
				m_MaxPointSetDesc.m_numTexCoords, 
				pPointSetDesc->m_numTexCoords);
		if(!m_pTexCoords)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if( IFXSUCCESS(ir) && 
		m_MaxPointSetDesc.m_numMaterials != pPointSetDesc->m_numMaterials)
	{
		ReallocDataBlock(
				m_pMaterials, 
				m_MaxPointSetDesc.m_numMaterials, 
				pPointSetDesc->m_numMaterials);
		if(!m_pMaterials)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) )
	{
		m_MaxPointSetDesc = *pPointSetDesc;
	}

	if(IFXFAILURE(ir))
	{
		Deallocate();
	}

	return ir;
}

IFXRESULT CIFXAuthorPointSet::Deallocate()
{
	U32 i = 0;

	if (m_pPositionPoints)  
		DeallocateDataBlock(m_pPositionPoints);
	if (m_pNormalPoints)    
		DeallocateDataBlock(m_pNormalPoints);
	if (m_pDiffusePoints) 
		DeallocateDataBlock(m_pDiffusePoints);
	if (m_pSpecularPoints)  
		DeallocateDataBlock(m_pSpecularPoints);
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
	if (m_pMaterialsPoints) 
		DeallocateDataBlock(m_pMaterialsPoints);
	if (m_pMaterials)   
		DeallocateDataBlock(m_pMaterials);

	m_pPositionPoints  = NULL;
	m_pNormalPoints    = NULL;
	m_pDiffusePoints   = NULL;
	m_pSpecularPoints  = NULL;

	for(i = 0; i < IFX_MAX_TEXUNITS; ++i)
	{
		if (m_pTexCoordPoints[i])
		{
			DeallocateDataBlock(m_pTexCoordPoints[i]);
			m_pTexCoordPoints[i] = NULL;
		}
	}
	m_pMaterialsPoints  = NULL;

	m_pPositions    = NULL;
	m_pNormals      = NULL;
	m_pDiffuseColors  = NULL;
	m_pSpecularColors = NULL;
	m_pTexCoords    = NULL;
	m_pMaterials    = NULL;

	IFXInitStruct(&m_CurPointSetDesc);
	IFXInitStruct(&m_MaxPointSetDesc);

	return IFX_OK;
}


IFXRESULT CIFXAuthorPointSet::Copy(IFXREFIID, void** )
{
	IFXRESULT ir = IFX_OK;
	return ir;
}


IFXRESULT CIFXAuthorPointSet::GetPositionPoint(U32 index,  U32* pPositionPoint)
{
	IFXRESULT ir = IFX_OK;

	if(!pPositionPoint ) 
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if( index > m_MaxPointSetDesc.m_numPoints ) 
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(ir))
	{
		*pPositionPoint = m_pPositionPoints[index];
	}

	return ir;
}

IFXRESULT CIFXAuthorPointSet::SetPositionPoint(U32 index, const U32* pPositionPoint )
{
	IFXRESULT ir = IFX_OK;

	if (!pPositionPoint)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxPointSetDesc.m_numPoints)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			m_pPositionPoints[index] = *pPositionPoint;
		}
	}

	return ir;
}

IFXRESULT CIFXAuthorPointSet::GetNormalPoint(U32 index, U32* pNormalPoint )
{
	IFXRESULT ir = IFX_OK;

	if(!pNormalPoint)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxPointSetDesc.m_numPoints)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			*pNormalPoint = m_pNormalPoints[index];
		}
	}

	return ir;
}

IFXRESULT CIFXAuthorPointSet::SetNormalPoint(U32 index, const U32* pNormalPoint )
{
	IFXRESULT ir = IFX_OK;

	if (!pNormalPoint)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxPointSetDesc.m_numPoints)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			m_pNormalPoints[index] = *pNormalPoint;
		}
	}

	return ir;
}

IFXRESULT CIFXAuthorPointSet::GetDiffusePoint(U32 index, U32* pDiffusePoint)
{
	IFXRESULT ir = IFX_OK;

	if(!pDiffusePoint)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxPointSetDesc.m_numPoints)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			*pDiffusePoint = m_pDiffusePoints[index];
		}
	}

	return ir;
}

IFXRESULT CIFXAuthorPointSet::SetDiffusePoint(U32 index, const U32* pDiffusePoint)
{
	IFXRESULT ir = IFX_OK;

	if (!pDiffusePoint)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxPointSetDesc.m_numPoints)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			m_pDiffusePoints[index] = *pDiffusePoint;
		}
	}

	return ir;
}

IFXRESULT CIFXAuthorPointSet::GetSpecularPoint(U32 index, U32* pSpecularPoint)
{
	IFXRESULT ir = IFX_OK;

	if(!pSpecularPoint)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxPointSetDesc.m_numPoints)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			*pSpecularPoint = m_pSpecularPoints[index];
		}
	}

	return ir;
}

IFXRESULT CIFXAuthorPointSet::SetSpecularPoint(U32 index, const U32* pSpecularPoint)
{
	IFXRESULT ir = IFX_OK;

	if (!pSpecularPoint)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxPointSetDesc.m_numPoints)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			m_pSpecularPoints[index] = *pSpecularPoint;
		}
	}

	return ir;
}


IFXRESULT CIFXAuthorPointSet::GetNumAllocatedTexPointLayers(U32* pLayers)
{
	IFXRESULT result = IFX_OK;
	int     valid_Layers = 0;

	if(!pLayers)
	{
		result = IFX_E_INVALID_POINTER;
	}

	if(IFXSUCCESS(result))
	{
		U32 i;
		for( i = 0; i < IFX_MAX_TEXUNITS; ++i)
		{
			if(m_pTexCoordPoints[i].IsValid())
			{
				valid_Layers++;
			}
		}
	}

	*pLayers = valid_Layers;

	return result;
}


IFXRESULT CIFXAuthorPointSet::GetPositionPoints(U32** ppPositionPoints )
{
	IFXRESULT ir = IFX_OK;

	if(!ppPositionPoints)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(IFXSUCCESS(ir))
	{
		*ppPositionPoints = m_pPositionPoints;
	}

	return ir;
}

IFXRESULT CIFXAuthorPointSet::GetNormalPoints(U32** ppPoints )
{
	IFXRESULT ir = IFX_OK;

	if(!ppPoints)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(m_MaxPointSetDesc.m_numNormals == 0)
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(ir))
	{
		*ppPoints = m_pNormalPoints;
	}

	return ir;
}

IFXRESULT CIFXAuthorPointSet::GetDiffusePoints(U32** ppPoints)
{
	IFXRESULT ir = IFX_OK;

	if(!ppPoints)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(m_MaxPointSetDesc.m_numDiffuseColors == 0)
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(ir))
	{
		*ppPoints = m_pDiffusePoints;
	}

	return ir;
}

IFXRESULT CIFXAuthorPointSet::GetSpecularPoints(U32** ppPoints )
{
	IFXRESULT ir = IFX_OK;

	if(!ppPoints)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(m_MaxPointSetDesc.m_numSpecularColors == 0)
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(ir))
	{
		*ppPoints = m_pSpecularPoints;
	}

	return ir;
}

IFXRESULT CIFXAuthorPointSet::GetTexCoordsPoints (U32** ppTexCoordPoints)
{
	IFXRESULT ir = IFX_OK;

	if(!ppTexCoordPoints)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(m_MaxPointSetDesc.m_numTexCoords == 0 )
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(ir))
	{
		*ppTexCoordPoints = m_pTexCoordPoints[0]; // likely invalid value, but the function is never called
	}

	return ir;
}


IFXRESULT CIFXAuthorPointSet::GetMaterialsPoints(U32** ppPointMaterials )
{
	IFXRESULT ir = IFX_OK;

	if(!ppPointMaterials)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	//If Points are not exist, then we expected that Materials
	//should not be there either.  We may not need this check.
	if(m_MaxPointSetDesc.m_numPoints == 0)
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(ir))
	{
		*ppPointMaterials = m_pMaterialsPoints;
	}

	return ir;
}


IFXRESULT CIFXAuthorPointSet::GetTexPoints(U32 index, U32** ppTexPoints )
{
	IFXRESULT ir = IFX_OK;

	//If there's a posibility that the user passes in multiple bad arg.
	//Then we need to look into additional checks, which can slow down
	//this at runtime.
	if(!ppTexPoints)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(index >= IFX_MAX_TEXUNITS)
	{
		ir = IFX_E_INVALID_RANGE;
	}
	else
	{
		if(m_MaxPointSetDesc.m_numTexCoords == 0)
		{
			ir = IFX_E_INVALID_RANGE;
		}
	}

	if(IFXSUCCESS(ir) && m_pTexCoordPoints[index].IsNull())
	{
		m_pTexCoordPoints[index] = new U32[m_MaxPointSetDesc.m_numPoints];
		if(!m_pTexCoordPoints[index])
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
		else
			memset(m_pTexCoordPoints[index], 0,
			m_MaxPointSetDesc.m_numPoints * sizeof(U32));
	}

	if(IFXSUCCESS(ir))
	{
		*ppTexPoints = m_pTexCoordPoints[index];
	}

	return ir;
}

IFXRESULT CIFXAuthorPointSet::GetTexPoint(
										  U32 in_Layer, U32 index,
										  U32* pPoint)
{
	IFXRESULT ir = IFX_OK;

	if (in_Layer > IFX_MAX_TEXUNITS)
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if(!pPoint)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (index >= m_MaxPointSetDesc.m_numPoints || !m_MaxPointSetDesc.m_numTexCoords )
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(ir) && m_pTexCoordPoints[in_Layer].IsNull())
	{
		m_pTexCoordPoints[in_Layer] = new U32[m_MaxPointSetDesc.m_numPoints];
		if(!m_pTexCoordPoints[in_Layer])
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if (IFXSUCCESS(ir))
	{
		*pPoint = m_pTexCoordPoints[in_Layer][index];
	}

	return ir;
}

IFXRESULT CIFXAuthorPointSet::SetTexPoint(U32 in_Layer, U32 index,
										  const U32* pPoint)
{
	IFXRESULT ir = IFX_OK;

	if (in_Layer > IFX_MAX_TEXUNITS || !m_MaxPointSetDesc.m_numTexCoords)
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if (!pPoint)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(IFXSUCCESS(ir) && m_pTexCoordPoints[in_Layer].IsNull())
	{
		m_pTexCoordPoints[in_Layer] = new U32[m_MaxPointSetDesc.m_numPoints];
		if(!m_pTexCoordPoints[in_Layer])
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if (IFXSUCCESS(ir))
	{
		m_pTexCoordPoints[in_Layer][index] = *pPoint;
	}

	return ir;
}

IFXRESULT CIFXAuthorPointSet::GetPointMaterials(U32** ppPointMat)
{
	IFXRESULT ir = IFX_OK;
	*ppPointMat = m_pMaterialsPoints;
	return ir;
}

IFXRESULT CIFXAuthorPointSet::GetPositions (IFXVector3** pPos )
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

IFXRESULT CIFXAuthorPointSet::GetNormals (IFXVector3** pNorm )
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

IFXRESULT CIFXAuthorPointSet::GetDiffuseColors( IFXVector4** pDiffColor )
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

IFXRESULT CIFXAuthorPointSet::GetSpecularColors( IFXVector4** pSpecColor )
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

IFXRESULT CIFXAuthorPointSet::GetTexCoords ( IFXVector4** pTexCoord )
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

IFXRESULT CIFXAuthorPointSet::GetMaterials ( IFXAuthorMaterial** pMaterial )
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

IFXRESULT CIFXAuthorPointSet::GetMaterial(U32 index, IFXAuthorMaterial* pMaterial)
{
	IFXRESULT ir = IFX_OK;

	if(!pMaterial)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		if (index >= m_MaxPointSetDesc.m_numMaterials)
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


IFXRESULT CIFXAuthorPointSet::SetMaterial(U32 index, const IFXAuthorMaterial* pMaterial)
{
	IFXRESULT ir = IFX_OK;

	if (index >= m_MaxPointSetDesc.m_numMaterials )
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

IFXRESULT CIFXAuthorPointSet::GetPosition(U32 index,IFXVector3* pVector3)
{
	IFXRESULT ir = IFX_OK;

	if (!pVector3)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		if (index >= m_MaxPointSetDesc.m_numPositions)
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

IFXRESULT CIFXAuthorPointSet::SetPosition(U32 index,const IFXVector3* pVector3)
{
	IFXRESULT ir = IFX_OK;

	if (!pVector3)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		if (index >= m_MaxPointSetDesc.m_numPositions )
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


IFXRESULT CIFXAuthorPointSet::GetNormal(U32 index, IFXVector3* pVector3)
{
	IFXRESULT ir = IFX_OK;

	if (!pVector3)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		if (index >= m_MaxPointSetDesc.m_numNormals)
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

IFXRESULT CIFXAuthorPointSet::SetNormal(U32 index, const IFXVector3* pVector3)
{
	IFXRESULT ir = IFX_OK;

	if (!pVector3)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		if (index >= m_MaxPointSetDesc.m_numNormals )
		{
			ReallocDataBlock(m_pNormals, m_MaxPointSetDesc.m_numNormals, index + 1);
			if(!m_pNormals)
			{
				ir = IFX_E_OUT_OF_MEMORY;
			}
			m_MaxPointSetDesc.m_numNormals = index + 1;
		}
		if (IFXSUCCESS(ir))
		{
			m_pNormals[index] = *pVector3;
		}
	}

	return ir;
}


IFXRESULT CIFXAuthorPointSet::GetDiffuseColor(U32 index, IFXVector4* pColor)
{
	IFXRESULT ir = IFX_OK;

	if (!pColor)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		if (index >= m_MaxPointSetDesc.m_numDiffuseColors)
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

IFXRESULT CIFXAuthorPointSet::SetDiffuseColor(U32 index,IFXVector4* pColor)
{
	IFXRESULT ir = IFX_OK;

	if (!pColor)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		if (index >= m_MaxPointSetDesc.m_numDiffuseColors)
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


IFXRESULT CIFXAuthorPointSet::GetSpecularColor(U32 index, IFXVector4* pColor)
{
	IFXRESULT ir = IFX_OK;

	if (!pColor)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		if (index >= m_MaxPointSetDesc.m_numSpecularColors)
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

IFXRESULT CIFXAuthorPointSet::SetSpecularColor(U32 index, IFXVector4* pColor)
{
	IFXRESULT ir = IFX_OK;

	if (!pColor)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		if (index >= m_MaxPointSetDesc.m_numSpecularColors)
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


IFXRESULT CIFXAuthorPointSet::GetTexCoord(U32 index, IFXVector4* pVector4)
{
	IFXRESULT ir = IFX_OK;

	if (!pVector4)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(IFXSUCCESS(ir) && m_MaxPointSetDesc.m_numTexCoords )
	{
		*pVector4 = (m_pTexCoords)[index];
	}

	return ir;
}

IFXRESULT CIFXAuthorPointSet::SetTexCoord(U32 index, const IFXVector4* pVector4)
{
	IFXRESULT ir = IFX_OK;

	if (!pVector4)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		if(m_MaxPointSetDesc.m_numTexCoords)
		{
			(m_pTexCoords)[index] = *pVector4;
		}
	}

	return ir;
}


// IFXAuthorPointsetAccess interface

IFXRESULT CIFXAuthorPointSet::SetPositionArray(IFXVector3* pData, U32 p)
{
	IFXRESULT ir = IFX_OK;

	if (!pData)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		m_pPositions = pData;
		m_MaxPointSetDesc.m_numPositions = p;
	}

	return ir;
}


IFXRESULT CIFXAuthorPointSet::SetNormalArray(IFXVector3* pData, U32 p)
{
	IFXRESULT ir = IFX_OK;

	if (!pData)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		m_pNormals = pData;
		m_MaxPointSetDesc.m_numNormals = p;
	}

	return ir;
}


IFXRESULT CIFXAuthorPointSet::SetDiffuseColorArray(IFXVector4* pData, U32 p)
{
	IFXRESULT ir = IFX_OK;

	if (!pData)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		m_pDiffuseColors = pData;
		m_MaxPointSetDesc.m_numDiffuseColors = p;
	}

	return ir;
}


IFXRESULT CIFXAuthorPointSet::SetSpecularColorArray(IFXVector4* pData, U32 p)
{
	IFXRESULT ir = IFX_OK;

	if (!pData)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		m_pSpecularColors = pData;
		m_MaxPointSetDesc.m_numSpecularColors = p;
	}

	return ir;
}


IFXRESULT CIFXAuthorPointSet::SetTexCoordArray(IFXVector4* pData, U32 p)
{
	IFXRESULT ir = IFX_OK;

	if (!pData)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		m_pTexCoords = pData;
		m_MaxPointSetDesc.m_numTexCoords = p;
	}

	return ir;
}


IFXRESULT CIFXAuthorPointSet::GetPointMaterial(U32 index, U32* pPointMaterial)
{
	IFXRESULT ir = IFX_OK;

	if(!pPointMaterial)
	{
		ir = IFX_E_INVALID_POINTER;
	}
	else
	{
		if (index >= m_MaxPointSetDesc.m_numPoints)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			*pPointMaterial = m_pMaterialsPoints[index];
		}
	}

	return ir;
}


IFXRESULT CIFXAuthorPointSet::SetPointMaterial(U32 index, U32 pointMaterialID)
{
	IFXRESULT ir = IFX_OK;

	if (index >= m_MaxPointSetDesc.m_numPoints)
	{
		ir = IFX_E_INVALID_RANGE;
	}
	else
	{
		m_pMaterialsPoints[index] = pointMaterialID;
	}

	return ir;
}

IFXVector4 CIFXAuthorPointSet::CalcBoundSphere()
{
	const float minFloats[] = { FLT_MAX, FLT_MAX, FLT_MAX, 0 };
	const float maxFloats[] = {-FLT_MAX,-FLT_MAX,-FLT_MAX, 0 };
	IFXVector4 vmin(minFloats);
	IFXVector4 vmax(maxFloats);

	IFXVector4 bound;

	// Determine the axis aligned bounding box and the number of verticies.
	IFXVector3* pVertex;

	U32 uVertexCount = m_CurPointSetDesc.m_numPositions;

	U32 i;
	for(  i = 0; i < uVertexCount; i++ )
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

		const U32 uVertexCount = m_CurPointSetDesc.m_numPositions;

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
