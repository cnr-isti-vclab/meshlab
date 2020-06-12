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
	@file	CIFXAuthorMeshScrub.cpp
			
			Implementation of the CIFXAuthorMeshScrub class.
*/

#include "CIFXAuthorMeshScrub.h"
#include "IFXVertexAttributes.h"
#include "IFXCoreCIDs.h"
#include "IFXExportingCIDs.h"

//---------------------------------------------------------------------------
//  CIFXAuthorMeshScrub_Factory
//
//  This is the CIFXAuthorMeshScrub component factory function.  The
//  CIFXAuthorMeshScrub component can be instaniated multiple times.
//---------------------------------------------------------------------------

IFXRESULT IFXAPI_CALLTYPE CIFXAuthorMeshScrub_Factory( IFXREFIID  interfaceId,
                    void**   ppInterface )
{
  IFXRESULT result;

  if ( ppInterface )
  {
    // Create the CIFXAuthorMeshGroup component.
    CIFXAuthorMeshScrub *pComponent = new CIFXAuthorMeshScrub;

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

// Construction/Destruction
CIFXAuthorMeshScrub::CIFXAuthorMeshScrub()
{
  m_refCount = 0;
  m_pScrubMesh = NULL;
  m_pMeshMap = NULL;
  m_pTempBuffer = NULL;
  m_TempBufferSize = 0;
  m_pProgressCallback = NULL;
  m_pParams = NULL;
  m_pBaseVertices = NULL;

  m_NumSteps = 0.0f;
  m_CurStep = 0.0f;

  m_bLocked = FALSE;
  m_pMaterials = NULL;
  m_pPositions = NULL;
  m_pNormals = NULL;
  m_pDiffuseColors = NULL;
  m_pSpecularColors = NULL;
  m_pTextureCoords = NULL;

  m_pPositionFaces = NULL;
  m_pNormalFaces = NULL;
  m_pDiffuseFaces = NULL;
  m_pSpecularFaces = NULL;
  m_pFaceMaterials = NULL;
  m_pFaceMap = NULL;
  m_pPositionMap = NULL;
  m_pNormalMap = NULL;
  m_pTextureCoordMap = NULL;
  m_pDiffuseMap = NULL;
  m_pSpecularMap = NULL;

  U32 i = IFX_MAX_TEXUNITS;
  while(i--)
  {
    m_pTexCoordFaces[i] = NULL;
  }
}

CIFXAuthorMeshScrub::~CIFXAuthorMeshScrub()
{
  IFXASSERT(!m_pTempBuffer);
}


//---------------------------------------------------------------------------
//  CIFXAuthorMeshScrub::AddRef
//
//  This method increments the reference count for an interface on a
//  component.  It should be called for every new copy of a pointer to an
//  interface on a given component.  It returns a U32 that contains a value
//  from 1 to 2^32 - 1 that defines the new reference count.  The return
//  value should only be used for debugging purposes.
//---------------------------------------------------------------------------

U32 CIFXAuthorMeshScrub::AddRef()
{
  return ++m_refCount;
}

//---------------------------------------------------------------------------
//  CIFXAuthorMeshScrub::Release
//
//  This method decrements the reference count for the calling interface on a
//  component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//  that defines the new reference count.  The return value should only be
//  used for debugging purposes.  If the reference count on a component falls
//  to zero, the component is destroyed.
//---------------------------------------------------------------------------

U32 CIFXAuthorMeshScrub::Release()
{
  if ( !( --m_refCount ) )
  {

    delete this;
    // This second return point is used so that the deleted object's
    // reference count isn't referenced after the memory is released.
    return 0;
  }

  return m_refCount;
}

//---------------------------------------------------------------------------
//  CIFXAuthorMeshScrub::QueryInterface
//
//  This method provides access to the various interfaces supported by a
//  component.  Upon success, it increments the component's reference count,
//  hands back a pointer to the specified interface and returns IFX_OK.
//  Otherwise, it returns either IFX_E_INVALID_POINTER or IFX_E_UNSUPPORTED.
//
//  A number of rules must be adhered to by all implementations of
//  QueryInterface.  For a list of such rules, refer to the Microsoft COM
//  description of the IUnknown::QueryInterface method.
//---------------------------------------------------------------------------

IFXRESULT CIFXAuthorMeshScrub::QueryInterface( IFXREFIID interfaceId,
                        void** ppInterface )
{
  IFXRESULT result  = IFX_OK;

  if ( ppInterface )
  {
    if(interfaceId == IID_IFXUnknown)
    {
      *ppInterface = ( IFXUnknown* ) this;
    }
    if ( interfaceId == IID_IFXAuthorMeshScrub )
    {
      *ppInterface = ( IFXAuthorMeshScrub* ) this;
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

static IFXAuthorMeshScrubParams s_Params;
//---------------------------------------------------------
// IFXAuthorMeshScrub Iterface Implementation
//---------------------------------------------------------

IFXRESULT CIFXAuthorMeshScrub::Scrub(
								IFXAuthorMesh* pInAuthorMesh,
								IFXAuthorMesh** ppOutAuthorMesh,
								IFXAuthorMeshMap** ppOutMeshMap,
								IFXAuthorMeshScrubParams* pInParams,
								IFXProgressCallback* pInProgressCallback)
{
  IFXRESULT result = IFX_OK;

  if(!pInAuthorMesh || !ppOutAuthorMesh)
  {
    result = IFX_E_INVALID_POINTER;
  }
  /** 
  @todo	FIXME - deallocate any update records -- should ideally be just
		a parameter to the copy call below indicating that no CLOD/update 
		data is to be copied.
  */
  m_pParams = pInParams;
  m_pProgressCallback = pInProgressCallback;

  if(IFXSUCCESS(result) && !m_pParams)
  {
    m_pParams = &s_Params;
  }

  if(IFXSUCCESS(result))
  {
    if(m_pParams->bOperatateInPlace)
    {
      m_pScrubMesh = pInAuthorMesh;
      m_pScrubMesh->AddRef();
    }
    else
    {
      result = pInAuthorMesh->Copy(IID_IFXAuthorMesh,
        (void**)&m_pScrubMesh);
    }
  }

  // Build the mapping table
  if(IFXSUCCESS(result))
  {
    result = IFXCreateComponent(CID_IFXAuthorMeshMap,
      IID_IFXAuthorMeshMap,
      (void **)&m_pMeshMap);
  }

  if(IFXSUCCESS(result))
  {
    result = m_pMeshMap->Allocate(m_pScrubMesh);
  }

  // Get the Mesh Description
  if(IFXSUCCESS(result))
  {
    m_OriginalMeshDesc = *(m_pScrubMesh->GetMaxMeshDesc());
    m_ScrubbedMeshDesc = *(m_pScrubMesh->GetMaxMeshDesc());
  }

  if(IFXSUCCESS(result) && m_pProgressCallback)
  {
    CalcProgressSteps();
  }

  // Allocate Temp storage array
  if(IFXSUCCESS(result) )
  {
    m_TempBufferSize = sizeof(U32) * (
      IFXMAX(m_ScrubbedMeshDesc.NumFaces,
      IFXMAX(m_ScrubbedMeshDesc.NumPositions,
      IFXMAX(m_ScrubbedMeshDesc.NumNormals,
      IFXMAX(m_ScrubbedMeshDesc.NumSpecularColors,
      IFXMAX(m_ScrubbedMeshDesc.NumDiffuseColors,
      IFXMAX(m_ScrubbedMeshDesc.NumTexCoords,
      IFXMAX(m_ScrubbedMeshDesc.NumMaterials,
      m_ScrubbedMeshDesc.NumBaseVertices))))))));

    m_pTempBuffer = new U32[m_TempBufferSize];
  }

  Lock();

  if(IFXSUCCESS(result) && m_pParams->bInvalidIndices)
  {
    result = RemoveInvalidIndices();
    if(IFXSUCCESS(result))
    {
      result = UpdateProgress();
    }
  }

  if(IFXSUCCESS(result) && m_pParams->bZeroAreaFaces)
  {
    result = RemoveZeroAreaFaces();
    if(IFXSUCCESS(result))
    {
      result = UpdateProgress();
    }
  }

  if(IFXSUCCESS(result) && m_pParams->bNormalizeNormals)
  {
    result = NormalizeNormals();
    if(IFXSUCCESS(result))
    {
      result = UpdateProgress();
    }
  }

  if(IFXSUCCESS(result) && m_pParams->bUnusedAttributes)
  {
    if(IFXSUCCESS(result) && m_pParams->RemoveUnusedMaterials)
    {
      result = RemoveUnusedMaterials();
    }

    if(IFXSUCCESS(result) && m_pParams->RemoveUnusedPositions)
    {
      result = RemoveUnusedPositions();
    }

    if(IFXSUCCESS(result) && m_pParams->RemoveUnusedNormals)
    {
      result = RemoveUnusedNormals();
    }

    if(IFXSUCCESS(result) && m_pParams->RemoveUnusedDiffuseColors &&
      m_ScrubbedMeshDesc.NumDiffuseColors)
    {
      result = RemoveUnusedDiffuseColors();
    }

    if(IFXSUCCESS(result) && m_pParams->RemoveUnusedSpecularColors &&
      m_ScrubbedMeshDesc.NumSpecularColors)
    {
      result = RemoveUnusedSpecularColors();
    }

    if(IFXSUCCESS(result) && m_pParams->RemoveUnusedTextureCoordinates &&
      m_ScrubbedMeshDesc.NumTexCoords)
    {
      result = RemoveUnusedTexCoords();
    }

    if(IFXSUCCESS(result))
    {
      result = UpdateProgress();
    }
  }

  // Resize the Scrubbed Mesh - to remove wasted space
  if(IFXSUCCESS(result))
  {
    result = m_pScrubMesh->Reallocate(&m_ScrubbedMeshDesc);
  }

  if(IFXSUCCESS(result))
  {
    result = m_pScrubMesh->SetMeshDesc(m_pScrubMesh->GetMaxMeshDesc());
  }

  if(IFXSUCCESS(result))
  {
    BuildOutputMeshMap();
  }

  Unlock();
  if(IFXSUCCESS(result))
  {
    // Releasing these components is left to the caller
    *ppOutAuthorMesh = m_pScrubMesh;
    *ppOutMeshMap = m_pMeshMap;
  }
  else
  {
    *ppOutAuthorMesh = NULL;
    *ppOutMeshMap = NULL;

    IFXRELEASE(m_pScrubMesh);
    IFXRELEASE(m_pMeshMap);
  }

  IFXDELETE_ARRAY(m_pTempBuffer);
  m_TempBufferSize = 0;

  return result;
}

IFXRESULT CIFXAuthorMeshScrub::UpdateProgress()
{
  if(m_pProgressCallback)
  {
    m_CurStep++;
    if(!m_pProgressCallback->UpdateProgress(m_CurStep))
    {
      return IFX_E_ABORTED;
    }
  }
  return IFX_OK;
}

void CIFXAuthorMeshScrub::CalcProgressSteps()
{
  if(m_pProgressCallback)
  {
    m_NumSteps = 0.0f;
    m_CurStep = 0.0f;

    if(m_pParams->bInvalidIndices)
    {
      m_NumSteps++;
    }

    if(m_pParams->bZeroAreaFaces)
    {
      m_NumSteps++;
    }

    if(m_pParams->bNormalizeNormals)
    {
      m_NumSteps++;
    }

    if(m_pParams->bUnusedAttributes)
    {
      m_NumSteps++;
    }

    if(m_pParams->RemoveUnusedMaterials)
    {
      m_NumSteps++;
    }

    if(m_pParams->RemoveUnusedPositions)
    {
      m_NumSteps++;
    }

    if(m_pParams->RemoveUnusedNormals)
    {
      m_NumSteps++;
    }

    if(m_pParams->RemoveUnusedDiffuseColors)
    {
      m_NumSteps++;
    }

    if(m_pParams->RemoveUnusedSpecularColors)
    {
      m_NumSteps++;
    }

    if(m_pParams->RemoveUnusedTextureCoordinates)
    {
      m_NumSteps++;
    }
    m_pProgressCallback->InitializeProgress(m_NumSteps);
  }
}


IFXRESULT CIFXAuthorMeshScrub::RemoveInvalidIndices()
{
  IFXRESULT result = IFX_OK;
  U32 i = 0;
  IFXAuthorFace* pFace = NULL;
  IFXAuthorFace* pLastFace = NULL;
  U32* pRemoveFace = m_pTempBuffer;
  BOOL bRemovedFaces = FALSE;

  if(IFXSUCCESS(result))
  {
    memset(pRemoveFace, 0, sizeof(U32) * m_ScrubbedMeshDesc.NumFaces);
    // Position Faces
    pFace = m_pPositionFaces;
    pLastFace = pFace + m_ScrubbedMeshDesc.NumFaces;

	IFXASSERT(pFace != NULL); 
    if(NULL != pFace)
	{
	    i = 0;
		while(pFace < pLastFace)
		{
			if( pFace->VertexA() >= m_ScrubbedMeshDesc.NumPositions
				|| pFace->VertexB() >= m_ScrubbedMeshDesc.NumPositions
				|| pFace->VertexC() >= m_ScrubbedMeshDesc.NumPositions)
			{
				bRemovedFaces = TRUE;
				pRemoveFace[i] = TRUE;
			}
			++pFace;
			++i;
		}
	}

    // Normal Faces
	if (m_ScrubbedMeshDesc.NumNormals)
	{
		pFace = m_pNormalFaces;
		pLastFace = pFace + m_ScrubbedMeshDesc.NumFaces;

		IFXASSERT(pFace != NULL); 
		if(NULL != pFace)
		{
			i = 0;
			while(pFace < pLastFace)
			{
				if( pFace->VertexA() >= m_ScrubbedMeshDesc.NumNormals
					|| pFace->VertexB() >= m_ScrubbedMeshDesc.NumNormals
					|| pFace->VertexC() >= m_ScrubbedMeshDesc.NumNormals)
				{
					bRemovedFaces = TRUE;
					pRemoveFace[i] = TRUE;
				}
				++pFace;
				++i;
			}
		}
	}

    // Diffuse Faces
    if (m_ScrubbedMeshDesc.NumDiffuseColors)
    {
		pFace = m_pDiffuseFaces;
		pLastFace = pFace + m_ScrubbedMeshDesc.NumFaces;

		IFXASSERT(pFace != NULL); 
		if(NULL != pFace)
		{
			i = 0;
			while(pFace < pLastFace)
			{
				if( pFace->VertexA() >= m_ScrubbedMeshDesc.NumDiffuseColors
				|| pFace->VertexB() >= m_ScrubbedMeshDesc.NumDiffuseColors
				|| pFace->VertexC() >= m_ScrubbedMeshDesc.NumDiffuseColors)
				{
					bRemovedFaces = TRUE;
					pRemoveFace[i] = TRUE;
				}
				++pFace;
				++i;
			}
		}
    }

    // Specular Faces
    if (m_ScrubbedMeshDesc.NumSpecularColors)
    {
		pFace = m_pSpecularFaces;
		pLastFace = pFace + m_ScrubbedMeshDesc.NumFaces;
 
		IFXASSERT(pFace != NULL); 
		if(NULL != pFace)
		{
			i = 0;
			while(pFace < pLastFace)
			{
				if( pFace->VertexA() >= m_ScrubbedMeshDesc.NumSpecularColors
				|| pFace->VertexB() >= m_ScrubbedMeshDesc.NumSpecularColors
				|| pFace->VertexC() >= m_ScrubbedMeshDesc.NumSpecularColors)
				{
					bRemovedFaces = TRUE;
					pRemoveFace[i] = TRUE;
				}
				++pFace;
				++i;
			}
		}
    }

    // Tex Faces
    if (m_ScrubbedMeshDesc.NumTexCoords)
    {
		U32 j;

		m_pScrubMesh->GetNumAllocatedTexFaceLayers(&j);

		U32 t;
		for( t = 0; t < j && m_pTexCoordFaces[t]; ++t)
		{
			pFace = m_pTexCoordFaces[t];
			pLastFace = pFace + m_ScrubbedMeshDesc.NumFaces;

			IFXASSERT(pFace != NULL); 
			if(NULL != pFace)
			{
				i = 0;
				while(pFace < pLastFace)
				{
					if( pFace->VertexA() >= m_ScrubbedMeshDesc.NumTexCoords
						|| pFace->VertexB() >= m_ScrubbedMeshDesc.NumTexCoords
						|| pFace->VertexC() >= m_ScrubbedMeshDesc.NumTexCoords)
					{
						bRemovedFaces = TRUE;
						pRemoveFace[i] = TRUE;
					}
					++pFace;
					++i;
				}
			}
		}
    }

    // Face Materials
    for(i = 0; i < m_ScrubbedMeshDesc.NumFaces; ++i)
    {
      if(m_pFaceMaterials[i] >= m_ScrubbedMeshDesc.NumMaterials)
      {
        bRemovedFaces = TRUE;
        pRemoveFace[i] = TRUE;
      }
    }

    // Remove Faces that didn't make the cut
    if(bRemovedFaces)
    {
      RemoveFaces(pRemoveFace);
    }

    // Base Vertice
    U32* pRemoveBV = m_pTempBuffer;
    BOOL bRemovedBV = FALSE;
    memset(pRemoveBV, 0, sizeof(U32) * m_ScrubbedMeshDesc.NumBaseVertices);
    for(i = 0; i < m_ScrubbedMeshDesc.NumBaseVertices; ++i)
    {
      if(m_pBaseVertices[i] >= m_ScrubbedMeshDesc.NumPositions)
      {
        bRemovedBV = TRUE;
        pRemoveBV[i] = TRUE;
      }
    }
    if(bRemovedBV)
    {
      RemoveBaseVertices(pRemoveBV);
    }
  }

  return result;
}

IFXRESULT CIFXAuthorMeshScrub::RemoveZeroAreaFaces()
{
  IFXRESULT result = IFX_OK;
  U32 i = 0;
  IFXAuthorFace* pFace = m_pPositionFaces;
  IFXAuthorFace* pLastFace = pFace + m_ScrubbedMeshDesc.NumFaces;
  U32* pRemoveFace = m_pTempBuffer;
  BOOL bRemovedFaces = FALSE;
  IFXVector3 edge1, edge2, normal;

  if(IFXSUCCESS(result))
  {
    memset(pRemoveFace, 0, sizeof(U32) * m_ScrubbedMeshDesc.NumFaces);

    while(pFace < pLastFace)
    {
      if(pFace->VertexA() == pFace->VertexB()
        || pFace->VertexA() == pFace->VertexC()
        || pFace->VertexB() == pFace->VertexC())
      {
        // INVALID FACE
        bRemovedFaces = TRUE;
        pRemoveFace[i] = TRUE;
      }
      else
      {
        edge1.Subtract(
				m_pPositions[pFace->VertexB()], 
				m_pPositions[pFace->VertexA()]);
        edge2.Subtract(
				m_pPositions[pFace->VertexC()], 
				m_pPositions[pFace->VertexA()]);

        normal.CrossProduct(edge1, edge2);
        /// @todo	Play with MagnitudeSquared -- optimization
        // CalcMagnitude has a sqrt if we store 
		// the square of m_pParams->ZeroAreaFaceTolerance
        // the sqrt will not be necessary
        if (normal.CalcMagnitude() < m_pParams->ZeroAreaFaceTolerance)
        {
          bRemovedFaces = TRUE;
          pRemoveFace[i] = TRUE;
        }
      }
      ++pFace;
      ++i;
    }

    if(bRemovedFaces)
    {
      RemoveFaces(pRemoveFace);
    }
  }

  return result;
}

IFXRESULT CIFXAuthorMeshScrub::NormalizeNormals()
{
  IFXRESULT result = IFX_OK;
  U32 i = 0;
  if(IFXSUCCESS(result))
  {
    for(i = 0; i < m_ScrubbedMeshDesc.NumNormals; ++i)
    {
      m_pNormals[i].Normalize();
    }
  }

  return result;
}

IFXRESULT CIFXAuthorMeshScrub::RemoveUnusedMaterials()
{
  IFXRESULT result = IFX_OK;
  U32 i = 0;
  U32 faceMat = 0;
  U32* pUsage = m_pTempBuffer;
  U32 CpyTgt = 0;
  U32 NumRemoved = 0;

  if(IFXSUCCESS(result))
  {
    memset(pUsage, 0, sizeof(U32) * m_ScrubbedMeshDesc.NumFaces);

    // Figure out face material usage
    for (i = 0; i < m_ScrubbedMeshDesc.NumFaces; i++)
    {
      faceMat = m_pFaceMaterials[i];
      pUsage[faceMat]++;
    }

    // Remap material array for mesh
    for(i = 0; i < m_ScrubbedMeshDesc.NumMaterials; ++i)
    {
      if(pUsage[i] == 0)
      {
        // Material unused - we are about to copy over it
        NumRemoved++;
        pUsage[i] = (U32)-1;
      }
      else
      {
        // Shift the material records zero or more entries "to the left"
        m_pMaterials[CpyTgt] = m_pMaterials[i];
        pUsage[i] = CpyTgt;
        CpyTgt++;
      }
    }

    // Remap FaceMaterials
    for (i = 0; i < m_ScrubbedMeshDesc.NumFaces; i++)
    {
      faceMat = m_pFaceMaterials[i];
      IFXASSERT(pUsage[faceMat] != (U32)-1);
      m_pFaceMaterials[i] = pUsage[faceMat];
    }

    m_ScrubbedMeshDesc.NumMaterials -= NumRemoved;
  }

  return result;
}

IFXRESULT CIFXAuthorMeshScrub::RemoveUnusedPositions()
{
  IFXRESULT result = IFX_OK;
  U32 i = 0;
  U32* pUsage = m_pTempBuffer;
  U32 CpyTgt = 0;
  U32 NumRemoved = 0;

  if(IFXSUCCESS(result))
  {
    memset(pUsage, 0, sizeof(U32) * m_ScrubbedMeshDesc.NumPositions);

    DetectUnusedAttrib(m_pPositionFaces, pUsage);

    for(i = 0; i < m_ScrubbedMeshDesc.NumPositions; ++i)
    {
      if(pUsage[i] == 0)
      {
        NumRemoved++;
        pUsage[i] = IFX_AUTHOR_INVALID_INDEX;
      }
      else
      {
        m_pPositions[CpyTgt] = m_pPositions[i];
        pUsage[i] = CpyTgt;

        if (m_pPositionMap)
          m_pPositionMap[CpyTgt] = m_pPositionMap[i];

        ++CpyTgt;
      }
    }
    RemapFaces(m_pPositionFaces, pUsage);
    RemapBaseVertices(m_pBaseVertices, pUsage);
    m_ScrubbedMeshDesc.NumPositions -= NumRemoved;
  }

  return result;
}

IFXRESULT CIFXAuthorMeshScrub::RemoveUnusedNormals()
{
  IFXRESULT result = IFX_OK;
  U32 i = 0;
  U32* pUsage = m_pTempBuffer;
  U32 CpyTgt = 0;
  U32 NumRemoved = 0;
  if(IFXSUCCESS(result))
  {
    memset(pUsage, 0, sizeof(U32) * m_ScrubbedMeshDesc.NumNormals);

    DetectUnusedAttrib(m_pNormalFaces, pUsage);

    for(i = 0; i < m_ScrubbedMeshDesc.NumNormals; ++i)
    {
      if(pUsage[i] == 0)
      {
        NumRemoved++;
        pUsage[i] = IFX_AUTHOR_INVALID_INDEX;
      }
      else
      {
        m_pNormals[CpyTgt] = m_pNormals[i];
        pUsage[i] = CpyTgt;

        if (m_pNormalMap)
          m_pNormalMap[CpyTgt] = m_pNormalMap[i];

        ++CpyTgt;
      }
    }
    RemapFaces(m_pNormalFaces, pUsage);
    m_ScrubbedMeshDesc.NumNormals -= NumRemoved;
  }

  return result;
}

IFXRESULT CIFXAuthorMeshScrub::RemoveUnusedDiffuseColors()
{
  IFXRESULT result = IFX_OK;
  U32 i = 0;
  U32* pUsage = m_pTempBuffer;
  U32 CpyTgt = 0;
  U32 NumRemoved = 0;
  if(IFXSUCCESS(result))
  {
    memset(pUsage, 0, sizeof(U32) * m_ScrubbedMeshDesc.NumDiffuseColors);

    DetectUnusedAttrib(m_pDiffuseFaces, pUsage);

    for(i = 0; i < m_ScrubbedMeshDesc.NumDiffuseColors; ++i)
    {
      if(pUsage[i] == 0)
      {
        NumRemoved++;
        pUsage[i] = IFX_AUTHOR_INVALID_INDEX;
      }
      else
      {
        m_pDiffuseColors[CpyTgt] = m_pDiffuseColors[i];
        pUsage[i] = CpyTgt;

        if (m_pDiffuseMap)
          m_pDiffuseMap[CpyTgt] = m_pDiffuseMap[i];

        ++CpyTgt;
      }
    }
    RemapFaces(m_pDiffuseFaces, pUsage);
    m_ScrubbedMeshDesc.NumDiffuseColors -= NumRemoved;
  }

  return result;
}

IFXRESULT CIFXAuthorMeshScrub::RemoveUnusedSpecularColors()
{
  IFXRESULT result = IFX_OK;
  U32 i = 0;
  U32* pUsage = m_pTempBuffer;
  U32 CpyTgt = 0;
  U32 NumRemoved = 0;
  if(IFXSUCCESS(result))
  {
    memset(pUsage, 0, sizeof(U32) * m_ScrubbedMeshDesc.NumSpecularColors);

    DetectUnusedAttrib(m_pSpecularFaces, pUsage);

    for(i = 0; i < m_ScrubbedMeshDesc.NumSpecularColors; ++i)
    {
      if(pUsage[i] == 0)
      {
        NumRemoved++;
        pUsage[i] = IFX_AUTHOR_INVALID_INDEX;
      }
      else
      {
        m_pSpecularColors[CpyTgt] = m_pSpecularColors[i];
        pUsage[i] = CpyTgt;

        if (m_pSpecularMap)
          m_pSpecularMap[CpyTgt] = m_pSpecularMap[i];

        ++CpyTgt;
      }
    }
    RemapFaces(m_pSpecularFaces, pUsage);
    m_ScrubbedMeshDesc.NumSpecularColors -= NumRemoved;
  }

  return result;
}

IFXRESULT CIFXAuthorMeshScrub::RemoveUnusedTexCoords()
{
  IFXRESULT result = IFX_OK;
  U32 i = 0, l = 0;
  U32* pUsage = m_pTempBuffer;
  U32 CpyTgt = 0;
  U32 NumRemoved = 0;
  if(IFXSUCCESS(result))
  {
    memset(pUsage, 0, sizeof(U32) * m_ScrubbedMeshDesc.NumTexCoords);

    for(l = 0; l < IFX_MAX_TEXUNITS && m_pTexCoordFaces[l]; ++l)
    {
      DetectUnusedAttrib(m_pTexCoordFaces[l], pUsage);
    }

    for(i = 0; i < m_ScrubbedMeshDesc.NumTexCoords; ++i)
    {
      if(pUsage[i] == 0)
      {
        NumRemoved++;
        pUsage[i] = IFX_AUTHOR_INVALID_INDEX;
      }
      else
      {
        m_pTextureCoords[CpyTgt] = m_pTextureCoords[i];
        pUsage[i] = CpyTgt;

        if (m_pTextureCoordMap)
          m_pTextureCoordMap[CpyTgt] = m_pTextureCoordMap[i];

        ++CpyTgt;
      }
    }

    for(l = 0; l < IFX_MAX_TEXUNITS && m_pTexCoordFaces[l]; ++l)
    {
      RemapFaces(m_pTexCoordFaces[l], pUsage);
    }

    m_ScrubbedMeshDesc.NumTexCoords -= NumRemoved;
  }

  return result;
}


void CIFXAuthorMeshScrub::Lock()
{
  U32 x = 0;
  U32 i;

  if(m_pScrubMesh && !m_bLocked)
  {
    m_bLocked = TRUE;
    m_pScrubMesh->Lock();
    m_pScrubMesh->GetMaterials(&m_pMaterials);
    m_pScrubMesh->GetPositions(&m_pPositions);
    m_pScrubMesh->GetNormals(&m_pNormals);
    m_pScrubMesh->GetDiffuseColors(&m_pDiffuseColors);
    m_pScrubMesh->GetSpecularColors(&m_pSpecularColors);
    m_pScrubMesh->GetTexCoords(&m_pTextureCoords);

    m_pScrubMesh->GetBaseVertices(&m_pBaseVertices);
    m_pScrubMesh->GetFaceMaterials(&m_pFaceMaterials);

    m_pScrubMesh->GetPositionFaces(&m_pPositionFaces);
    m_pScrubMesh->GetNormalFaces(&m_pNormalFaces);
    m_pScrubMesh->GetDiffuseFaces(&m_pDiffuseFaces);
    m_pScrubMesh->GetSpecularFaces(&m_pSpecularFaces);

    // Initialize the texture layers
    m_pScrubMesh->GetNumAllocatedTexFaceLayers(&i);

    // 1.  Make sure unused layers are zeroed
    m_pScrubMesh->GetNumAllocatedTexFaceLayers(&i);
    for(x = i; i < IFX_MAX_TEXUNITS; ++i)
    {
      m_pTexCoordFaces[i] = NULL;
    }

    // 2.  Copy over the allocated layers
    m_pScrubMesh->GetNumAllocatedTexFaceLayers(&i);
    for(x = 0; x < i; ++x)
    {
      m_pScrubMesh->GetTexFaces(x, &m_pTexCoordFaces[x]);
    }

    // Build the mapping arrays
    m_pFaceMap = new U32[m_OriginalMeshDesc.NumFaces];
    for (i = 0; i < m_OriginalMeshDesc.NumFaces; i++)
      m_pFaceMap[i] = i;

    m_pPositionMap = new U32[m_OriginalMeshDesc.NumPositions];
    for (i = 0; i < m_OriginalMeshDesc.NumPositions; i++)
      m_pPositionMap[i] = i;

    m_pNormalMap = new U32[m_OriginalMeshDesc.NumNormals];
    for (i = 0; i < m_OriginalMeshDesc.NumNormals; i++)
      m_pNormalMap[i] = i;

    m_pTextureCoordMap = new U32[m_OriginalMeshDesc.NumTexCoords];
    for (i = 0; i < m_OriginalMeshDesc.NumTexCoords; i++)
      m_pTextureCoordMap[i] = i;

    m_pDiffuseMap = new U32[m_OriginalMeshDesc.NumDiffuseColors];
    for (i = 0; i < m_OriginalMeshDesc.NumDiffuseColors; i++)
      m_pDiffuseMap[i] = i;

    m_pSpecularMap = new U32[m_OriginalMeshDesc.NumSpecularColors];
    for (i = 0; i < m_OriginalMeshDesc.NumSpecularColors; i++)
      m_pSpecularMap[i] = i;
  }
}

void CIFXAuthorMeshScrub::Unlock()
{
  if(m_pScrubMesh && m_bLocked)
  {
    m_bLocked = FALSE;
    m_pScrubMesh->Unlock();
    m_pMaterials = NULL;
    m_pPositions = NULL;
    m_pNormals = NULL;
    m_pDiffuseColors = NULL;
    m_pSpecularColors = NULL;
    m_pTextureCoords = NULL;

    m_pPositionFaces = NULL;
    m_pNormalFaces = NULL;
    m_pDiffuseFaces = NULL;
    m_pSpecularFaces = NULL;
    m_pFaceMaterials = NULL;

    IFXDELETE_ARRAY(m_pFaceMap);
    IFXDELETE_ARRAY(m_pPositionMap);
    IFXDELETE_ARRAY(m_pNormalMap);
    IFXDELETE_ARRAY(m_pTextureCoordMap);
    IFXDELETE_ARRAY(m_pDiffuseMap);
    IFXDELETE_ARRAY(m_pSpecularMap);

    U32 i = IFX_MAX_TEXUNITS;
    while(i--)
    {
      m_pTexCoordFaces[i] = NULL;
    }
  }
}


void CIFXAuthorMeshScrub::RemoveFaces(U32* pInRemoveFace)
{
  // start remapping
  U32 CpyTgt = 0;
  U32 i = 0;
  U32 NumRemoved = 0;

  for(i = 0; i < m_ScrubbedMeshDesc.NumFaces; ++i)
  {
    if(!pInRemoveFace[i])
    {
      pInRemoveFace[CpyTgt] = pInRemoveFace[i];

      if(m_pPositionFaces)
      {
        m_pPositionFaces[CpyTgt] = m_pPositionFaces[i];
      }
      if(m_pNormals)
      {
        m_pNormalFaces[CpyTgt] = m_pNormalFaces[i];
      }
      if(m_pDiffuseFaces)
      {
        m_pDiffuseFaces[CpyTgt] = m_pDiffuseFaces[i];
      }
      if(m_pSpecularFaces)
      {
        m_pSpecularFaces[CpyTgt] = m_pSpecularFaces[i];
      }
      if(m_pFaceMaterials)
      {
        m_pFaceMaterials[CpyTgt] = m_pFaceMaterials[i];
      }

      U32 t;
      for( t = 0; t < IFX_MESH_MAX_NUM_TEXCOORDS; ++t)
      {
        if(m_pTexCoordFaces[t])
        {
          m_pTexCoordFaces[t][CpyTgt] = m_pTexCoordFaces[t][i];
        }
      }
      /// @todo Add Mapping  AddMapping(i, CpyTgt);
      if (m_pFaceMap)
        m_pFaceMap[CpyTgt] = m_pFaceMap[i];

      CpyTgt++;
    }
    else
    {
      // FaceRemoved
      NumRemoved++;
    }
  }
  m_ScrubbedMeshDesc.NumFaces -= NumRemoved;
}


void CIFXAuthorMeshScrub::DetectUnusedAttrib(IFXAuthorFace* pInFace, U32* pUsage)
{
  IFXAuthorFace* pFace = pInFace;
  IFXAuthorFace* pLastFace = pFace + m_ScrubbedMeshDesc.NumFaces;

  while(pFace < pLastFace)
  {
    pUsage[pFace->VertexA()]++;
    pUsage[pFace->VertexB()]++;
    pUsage[pFace->VertexC()]++;
    ++pFace;
  }
}


void CIFXAuthorMeshScrub::RemoveBaseVertices(U32* pInRemoveBV)
{
  // start remapping
  U32 CpyTgt = 0;
  U32 i = 0;
  U32 NumRemoved = 0;

  for(i = 0; i < m_ScrubbedMeshDesc.NumBaseVertices; ++i)
  {
    if(!pInRemoveBV[i])
    {
      m_pBaseVertices[CpyTgt] = m_pBaseVertices[i];

      /// @todo: need to record a mapping for this?

      CpyTgt++;
    }
    else
    {
      // BaseVertice removed
      NumRemoved++;
    }
  }
  m_ScrubbedMeshDesc.NumBaseVertices -= NumRemoved;
}


void CIFXAuthorMeshScrub::RemapFaces(IFXAuthorFace* pInFace, U32* pMap)
{
  IFXAuthorFace* pFace = pInFace;
  IFXAuthorFace* pLastFace = pFace + m_ScrubbedMeshDesc.NumFaces;

  // Remap Faces
  while(pFace < pLastFace)
  {
    pFace->VertexA() = pMap[pFace->VertexA()];
    pFace->VertexB() = pMap[pFace->VertexB()];
    pFace->VertexC() = pMap[pFace->VertexC()];
    ++pFace;
  }
}


void CIFXAuthorMeshScrub::RemapBaseVertices(U32* pInBV, U32* pMap)
{
  U32* pBV = pInBV;
  U32* pLastBV= pBV + m_ScrubbedMeshDesc.NumBaseVertices;

  // Remap Faces
  while(pBV < pLastBV)
  {
    *pBV = pMap[*pBV];
    ++pBV;
  }
}

void CIFXAuthorMeshScrub::BuildOutputMeshMap()
{
  U32     i;
  U32     *pMap = NULL;

  // ------ Face map ------
  // Get array from mesh map
  pMap = m_pMeshMap->GetFaceMap();

  // Set all map indices to invalid
  for (i = 0; i < m_OriginalMeshDesc.NumFaces; i++)
    pMap[i] = IFX_NULL_INDEX32;

  // Now build a forward map from the scrubbed map
  for (i = 0; i < m_ScrubbedMeshDesc.NumFaces; i++)
    pMap[m_pFaceMap[i]] = i;

  // ------ Position Map ------
  // Get array from mesh map
  pMap = m_pMeshMap->GetPositionMap();

  // Set all map indices to invalid
  for (i = 0; i < m_OriginalMeshDesc.NumPositions; i++)
    pMap[i] = IFX_NULL_INDEX32;

  // Now build a forward map from the scrubbed map
  for (i = 0; i < m_ScrubbedMeshDesc.NumPositions; i++)
    pMap[m_pPositionMap[i]] = i;

  // ------ Normal Map ------
  // Get array from mesh map
  pMap = m_pMeshMap->GetNormalMap();

  // Set all map indices to invalid
  for (i = 0; i < m_OriginalMeshDesc.NumNormals; i++)
    pMap[i] = IFX_NULL_INDEX32;

  // Now build a forward map from the scrubbed map
  for (i = 0; i < m_ScrubbedMeshDesc.NumNormals; i++)
    pMap[m_pNormalMap[i]] = i;

  // ------ Texture Map ------
  // Get array from mesh map
  pMap = m_pMeshMap->GetTextureMap();

  // Set all map indices to invalid
  for (i = 0; i < m_OriginalMeshDesc.NumTexCoords; i++)
    pMap[i] = IFX_NULL_INDEX32;

  // Now build a forward map from the scrubbed map
  for (i = 0; i < m_ScrubbedMeshDesc.NumTexCoords; i++)
    pMap[m_pTextureCoordMap[i]] = i;

  // ------ Diffuse Map ------
  // Get array from mesh map
  pMap = m_pMeshMap->GetDiffuseMap();

  // Set all map indices to invalid
  for (i = 0; i < m_OriginalMeshDesc.NumDiffuseColors; i++)
    pMap[i] = IFX_NULL_INDEX32;

  // Now build a forward map from the scrubbed map
  for (i = 0; i < m_ScrubbedMeshDesc.NumDiffuseColors; i++)
    pMap[m_pDiffuseMap[i]] = i;

  // ------ Specular Map ------
  // Get array from mesh map
  pMap = m_pMeshMap->GetSpecularMap();

  // Set all map indices to invalid
  for (i = 0; i < m_OriginalMeshDesc.NumSpecularColors; i++)
    pMap[i] = IFX_NULL_INDEX32;

  // Now build a forward map from the scrubbed map
  for (i = 0; i < m_ScrubbedMeshDesc.NumSpecularColors; i++)
    pMap[m_pSpecularMap[i]] = i;
}
