//***************************************************************************
//
//  Copyright (c) 2000 - 2006 Intel Corporation
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
	@file	CIFXAuthorLineSetAnalyzer.cpp

			Implementation of helper class to analyze connectivity of 
			IFXAuthorLineSet
*/

#include "CIFXAuthorLineSetAnalyzer.h"
#include "IFXCheckX.h"
#include "IFXVector3.h"

U32 CIFXAuthorLineSetAnalyzer::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXAuthorLineSetAnalyzer::Release()
{
	if( 1 == m_uRefCount )
	{
		delete this;
		return 0;
	}
	return --m_uRefCount;
}

IFXRESULT CIFXAuthorLineSetAnalyzer::QueryInterface( 
											IFXREFIID interfaceId, 
											void** ppInterface )
{
	IFXRESULT result = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXUnknown ) 
		{
			*ppInterface = ( IFXUnknown* )this;
		} 
		else if ( interfaceId == IID_IFXAuthorLineSetAnalyzer ) 
		{
			*ppInterface = ( IFXAuthorLineSetAnalyzer* )this;	
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


CIFXAuthorLineSetAnalyzer::CIFXAuthorLineSetAnalyzer() : m_uRefCount(0)
{
	m_pLineSet		= NULL;
	m_initialized	= FALSE;
	m_pPosInfo		= NULL;
	m_uMaxPositionCount	= (U32)-1;
}

CIFXAuthorLineSetAnalyzer::~CIFXAuthorLineSetAnalyzer() 
{
	IFXRELEASE(m_pLineSet);
	IFXDELETE_ARRAY(m_pPosInfo);
} 

IFXRESULT CIFXAuthorLineSetAnalyzer::Initialize(IFXAuthorLineSet* pLineSet)
{
	IFXRESULT rc = IFX_OK;

	if (pLineSet == NULL) 
	{
		return IFX_E_NOT_INITIALIZED;
	}

	if( pLineSet->GetMaxLineSetDesc()->m_numPositions <= 0 ) 
	{
		return IFX_E_INVALID_RANGE;
	}

	if( pLineSet->GetMaxLineSetDesc()->m_numPositions < 
		pLineSet->GetLineSetDesc()->m_numPositions) 
	{
		IFXASSERT(0);
		return IFX_E_INVALID_RANGE;
	}
	
	m_initialized = FALSE;
	
	// this is max number of postions that could be handled via
	//  on Initialize and sequence of Update calls 
	m_uMaxPositionCount = pLineSet->GetMaxLineSetDesc()->m_numPositions; 

	pLineSet->AddRef();
	if (m_pLineSet) 
	{
		m_pLineSet->Release();
	}
	m_pLineSet      =   pLineSet;
    m_LineSetDesc	=	*(pLineSet->GetLineSetDesc());

	m_pPosInfo = new IFXPositionConnectivity[m_uMaxPositionCount];
	if(m_pPosInfo == NULL) return IFX_E_OUT_OF_MEMORY;

	U32 i;
	for (i=0;i<m_uMaxPositionCount;i++) 
	{
		m_pPosInfo[i].m_uLineCount = U32(-1);
	}

	rc = GenerateConnectivity();
	
	IFXASSERT(rc == IFX_OK);

	m_initialized = TRUE;

	return rc;
}

IFXRESULT CIFXAuthorLineSetAnalyzer::GetLines(U32 pointInd, IFXArray <U32>& lines, 
											  IFXArray <U32>& endPoints)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized) 
	{
		return IFX_E_NOT_INITIALIZED;
	}

	if (pointInd>=m_LineSetDesc.m_numPositions) 
	{
		return IFX_E_INVALID_RANGE;
	}

	lines		= m_pPosInfo[pointInd].m_LineIndex;
	endPoints	= m_pPosInfo[pointInd].m_EndPosIndex;
	return rc;
}

IFXRESULT CIFXAuthorLineSetAnalyzer::GenerateConnectivity()
{
	IFXRESULT rc = IFX_OK;
	U32		i;
	U32 start_num, end_num;

	IFXASSERT(m_pPosInfo);

	IFXU32Line Line;
	for (i=0; i<m_LineSetDesc.m_numLines; i++) 
	{
		start_num =  end_num = (U32)-1;
		rc = m_pLineSet->GetPositionLine(i, &Line);
		IFXASSERT(rc == IFX_OK);

		start_num = Line.VertexA();			
		end_num   = Line.VertexB();

		if (m_pPosInfo[start_num].m_uLineCount == U32(-1)) 
		{
			m_pPosInfo[start_num].m_uLineCount = 0 ;
		}
		if (m_pPosInfo[end_num].m_uLineCount == U32(-1)) 
		{
			m_pPosInfo[end_num].m_uLineCount = 0 ;
		}

		m_pPosInfo[start_num].m_uLineCount++;
		m_pPosInfo[end_num].m_uLineCount++;

		m_pPosInfo[start_num].m_LineIndex.CreateNewElement() = i;
		m_pPosInfo[end_num].m_LineIndex.CreateNewElement() = i;

		m_pPosInfo[start_num].m_EndPosIndex.CreateNewElement() = end_num;
		m_pPosInfo[end_num].m_EndPosIndex.CreateNewElement() = start_num;

	}

	return rc;
}

IFXRESULT CIFXAuthorLineSetAnalyzer::Update()
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized) 
	{
		return IFX_E_NOT_INITIALIZED;
	}

	if (m_pLineSet == NULL) 
	{
		return IFX_E_NOT_INITIALIZED;
	}

	IFXAuthorLineSetDesc* pUpdatedLineSetDesc = m_pLineSet->GetLineSetDesc();

	if (m_uMaxPositionCount < pUpdatedLineSetDesc->m_numPositions) {
		IFXASSERT(0);
		return IFX_E_INVALID_RANGE;
	}

	if (m_LineSetDesc.m_numPositions > pUpdatedLineSetDesc->m_numPositions || 
		m_LineSetDesc.m_numLines > pUpdatedLineSetDesc->m_numLines) 
	{
		IFXASSERT(0);
		return IFX_E_INVALID_RANGE;
	}

	U32	i;
	U32 start_num, end_num;

	IFXASSERT(m_pPosInfo);

	IFXU32Line Line;
	
	for (i=m_LineSetDesc.m_numLines; i<pUpdatedLineSetDesc->m_numLines; i++) 
	{
		start_num =  end_num = (U32)-1;
		rc = m_pLineSet->GetPositionLine(i, &Line);
		IFXASSERT(rc == IFX_OK);

		start_num = Line.VertexA();			
		end_num   = Line.VertexB();

		if (m_pPosInfo[start_num].m_uLineCount == U32(-1)) 
		{
			m_pPosInfo[start_num].m_uLineCount = 0 ;
		}

		if (m_pPosInfo[end_num].m_uLineCount == U32(-1)) 
		{
			m_pPosInfo[end_num].m_uLineCount = 0 ;
		}

		m_pPosInfo[start_num].m_uLineCount++;
		m_pPosInfo[end_num].m_uLineCount++;

		m_pPosInfo[start_num].m_LineIndex.CreateNewElement() = i;
		m_pPosInfo[end_num].m_LineIndex.CreateNewElement() = i;

		m_pPosInfo[start_num].m_EndPosIndex.CreateNewElement() = end_num;
		m_pPosInfo[end_num].m_EndPosIndex.CreateNewElement() = start_num;

	}
	
	m_LineSetDesc = *(pUpdatedLineSetDesc);

	return rc;
}

void CIFXAuthorLineSetAnalyzer::DumpAuthorLineSet()
{
	if (!m_initialized) return;

	IFXASSERT(m_pLineSet);

	IFXAuthorLineSetDesc* pLSDesc = m_pLineSet->GetLineSetDesc();
	IFXTRACE_GENERIC(L"[Author Line Set Dump] AuthorLineSet at  %x\n", m_pLineSet);
	IFXTRACE_GENERIC(L"Positions: %i, Lines: %i, Normals: %i\n",
		pLSDesc->m_numPositions, pLSDesc->m_numLines, pLSDesc->m_numNormals);

	IFXTRACE_GENERIC(L" Diffuse colors: %i, Specular Colors: %i, Materials: %i\n",
		pLSDesc->m_numDiffuseColors, pLSDesc->m_numSpecularColors, 
		pLSDesc->m_numMaterials);

	IFXVector3 pnt;
	IFXVector4 color1, color2;
	IFXU32Line line;
	IFXAuthorMaterial lineMaterial;
	U32 j, indMat;
  	for (j=0; j<pLSDesc->m_numLines; j++) 
	{
		IFXTRACE_GENERIC(L" Line index: %i\n", j);
		if (pLSDesc->m_numPositions>0) 
		{
			IFXCHECKX(m_pLineSet->GetPositionLine(j, &line));
		}
		IFXTRACE_GENERIC(L"\t Positions: %i  %i",	line.VertexA(), line.VertexB());

		m_pLineSet->GetPosition(line.VertexA(), &pnt);
		IFXTRACE_GENERIC(L"\t Vert A: %f %f %f ", pnt.X(), pnt.Y(), pnt.Z() );

		m_pLineSet->GetPosition(line.VertexB(), &pnt);
		IFXTRACE_GENERIC(L"\t Vert B: %f %f %f\n", pnt.X(), pnt.Y(), pnt.Z() );
		
		m_pLineSet->GetLineMaterial(j ,&indMat);
		IFXTRACE_GENERIC(L"\t Material: %i \n", indMat);
		
		m_pLineSet->GetMaterial(indMat, &lineMaterial);

		if (pLSDesc->m_numNormals>0 && lineMaterial.m_uNormals) 
		{
			IFXCHECKX(m_pLineSet->GetNormalLine(j, &line));
			IFXTRACE_GENERIC(L"\t Normals: %i %i ",	line.VertexA(), line.VertexB());

			m_pLineSet->GetNormal(line.VertexA(), &pnt);
			IFXTRACE_GENERIC(L"\t Vert A: %f %f %f ", pnt.X(), pnt.Y(), pnt.Z() );

			m_pLineSet->GetNormal(line.VertexB(), &pnt);
			IFXTRACE_GENERIC(L"\t Vert B: %f %f %f\n", pnt.X(), pnt.Y(), pnt.Z() );
		}
		if (pLSDesc->m_numDiffuseColors>0 && lineMaterial.m_uDiffuseColors) 
		{
			IFXCHECKX(m_pLineSet->GetDiffuseLine(j, &line));
			IFXTRACE_GENERIC(L"\t Line Diffuse colors: %i %i ",	
				line.VertexA(), line.VertexB());

			m_pLineSet->GetDiffuseColor(line.VertexA(), &color1);
			m_pLineSet->GetDiffuseColor(line.VertexB(), &color2);
			IFXTRACE_GENERIC(L"\t Values: %ls %ls \n", 
				color1.Out().Raw(), color2.Out().Raw());
		}

		if (pLSDesc->m_numSpecularColors>0 && lineMaterial.m_uSpecularColors) 
		{
			IFXCHECKX(m_pLineSet->GetSpecularLine(j, &line));
			IFXTRACE_GENERIC(L"\t Line Specular colors: %i %i ",
				line.VertexA(), line.VertexB());

			m_pLineSet->GetSpecularColor(line.VertexA(), &color1);
			m_pLineSet->GetSpecularColor(line.VertexB(), &color2);
			IFXTRACE_GENERIC(L"\t Values: %ls %ls \n", 
				color1.Out().Raw(), color2.Out().Raw());
		}
	}
}

//-----------------------------------------------------------------------------
// Factory function
//-----------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXAuthorLineSetAnalyzer_Factory( 
												IFXREFIID	interfaceId, 
												void**	ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) 	
	{
		// Create the CIFXAuthorLineSetAnalyzer component.
		CIFXAuthorLineSetAnalyzer *pComponent = new CIFXAuthorLineSetAnalyzer;

		if ( pComponent ) 
		{
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			rc = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		} 
		else 
		{
			rc = IFX_E_OUT_OF_MEMORY;
		}
	} 
	else 
	{
		rc = IFX_E_INVALID_POINTER;
	}

	return rc;
}
