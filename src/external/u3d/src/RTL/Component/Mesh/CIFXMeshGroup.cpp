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
// CIFXMeshGroup.cpp

#include "CIFXMeshGroup.h"
#include "CIFXAABBHierarchyBuilder.h"
#include <float.h>

//=============================
// Factory Function
//=============================

IFXRESULT IFXAPI_CALLTYPE CIFXMeshGroupFactory(IFXREFIID intId, void** ppUnk)
{
  IFXRESULT result;

  if ( ppUnk )
  {
    // Create the CIFXClassName component.
    CIFXMeshGroup *pPtr = new CIFXMeshGroup;
    if ( pPtr )
    {
      // Perform a temporary AddRef for our usage of the component.
      pPtr->AddRef();

      // Attempt to obtain a pointer to the requested interface.
      result = pPtr->QueryInterface( intId, ppUnk );

      // Perform a Release since our usage of the component is now
      // complete.  Note:  If the QI fails, this will cause the
      // component to be destroyed.
      pPtr->Release();
    }
    else
      result = IFX_E_OUT_OF_MEMORY;
  }
  else
    result = IFX_E_INVALID_POINTER;

  return result;
}

// IFXUnknown
U32 CIFXMeshGroup::AddRef(void)
{
  return ++m_uRefCount;
}


U32 CIFXMeshGroup::Release(void)
{
  if ( 1 == m_uRefCount )
  {
    delete this;
    return 0;
  }

  return --m_uRefCount;
}


IFXRESULT CIFXMeshGroup::QueryInterface( IFXREFIID interfaceId,
                                         void**    ppInterface )
{
  IFXRESULT result = IFX_OK;

  if ( ppInterface )
  {
    if ( interfaceId == IID_IFXMeshGroup )
      *ppInterface = ( IFXMeshGroup* ) this;
    else if ( interfaceId == IID_IFXRenderable )
      *ppInterface = ( IFXRenderable* ) this;
    else if ( interfaceId == IID_IFXUnknown )
      *ppInterface = ( IFXUnknown* ) this;
      else if ( interfaceId == IID_IFXBoundHierarchyMgr)
        *ppInterface = ( IFXBoundHierarchyMgr* ) this;
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

//================================
// CIFXMeshGroup Methods
//================================
CIFXMeshGroup::CIFXMeshGroup()
{
  m_bound.Set(0,0,0,0);
  m_pBoundHierarchy = NULL;
  m_ppMeshes = NULL;
  m_uRefCount = 0;
}

CIFXMeshGroup::~CIFXMeshGroup()
{
  Deallocate();
}


//===============================
// Protected Methods
//===============================
void CIFXMeshGroup::DeallocateObject()
{
  // Release the BoundHierarchy
  IFXRELEASE(m_pBoundHierarchy);

  U32 i;
  for( i = 0; i < m_uNumElements; i++)
  {
    IFXRELEASE( m_ppMeshes[i] );
  }
  IFXDELETE( m_ppMeshes );

}

IFXRESULT CIFXMeshGroup::Allocate(U32 uNumMeshes)
{
  IFXRESULT result = IFX_OK;

//  IFXASSERTBOX(0 != uNumMeshes, "Can't allocate 0 meshes!");
  if(0 == uNumMeshes)
  {
    result = IFX_E_INVALID_RANGE;
  }

  if(IFXSUCCESS(result))
  {
    Deallocate();

    m_uNumElements = uNumMeshes;
    m_ppMeshes = new IFXMesh*[m_uNumElements];
    if(m_ppMeshes)
    {
      U32 i;
      for( i = 0; i < m_uNumElements; i++)
      {
        m_ppMeshes[i] = NULL;
      }
    }
    else
    {
      result = IFX_E_OUT_OF_MEMORY;
    }
  }

  if(IFXSUCCESS(result))
  {
    result = AllocateShaders(NULL);
  }

  if(!IFXSUCCESS(result))
  {
    Deallocate();
  }

  return result;
}

IFXRESULT IFXAPI CIFXMeshGroup::TransferData(IFXMeshGroup& in_rMeshGroup,
                IFXMeshAttributes in_uTransferAttr, IFXMeshAttributes in_uCopyAttr,
                BOOL bCopyShaders)
{
  IFXRESULT result = IFX_OK;

  if(in_rMeshGroup.GetNumMeshes() != GetNumMeshes())
  {
    Deallocate();
    m_uNumElements = in_rMeshGroup.GetNumMeshes();
    m_ppMeshes = new IFXMesh*[m_uNumElements];

    if(m_ppMeshes)
    {
      U32 i;
      for( i = 0; i < m_uNumElements; i++)
      {
        m_ppMeshes[i] = 0;
      }
    }
    else
      result = IFX_E_OUT_OF_MEMORY;
  }

  if(IFXSUCCESS(result))
  {
    U32 i;
    for( i = 0; IFXSUCCESS(result) && i < m_uNumElements; i++)
    {
      if(!m_ppMeshes[i])
        result = IFXCreateComponent(CID_IFXMesh, IID_IFXMesh, (void**)&(m_ppMeshes[i]));

      if(IFXSUCCESS(result))
      {
        IFXMesh* pTmpMesh = NULL;
        in_rMeshGroup.GetMesh(i, pTmpMesh);
        result = m_ppMeshes[i]->TransferData(*pTmpMesh, in_uTransferAttr, in_uCopyAttr);
        IFXRELEASE(pTmpMesh);
      }
    }
  }

  if(IFXSUCCESS(result))
  {
    IFXShaderList** ppShaders = NULL;
    in_rMeshGroup.GetShaderDataArray(&ppShaders);
    result = AllocateShaders(ppShaders, bCopyShaders);
  }

  if(!IFXSUCCESS(result))
  {
    Deallocate();
  }

  return result;
}

IFXRESULT CIFXMeshGroup::TransferData(IFXRenderable& in_rSrc,
                    BOOL bCopyElements, BOOL bCopyShaders)
{
  IFXRESULT result = IFX_OK;
  IFXMeshGroup* pMeshGroup = NULL;
  if(IFXSUCCESS(result))
  {
    result = in_rSrc.QueryInterface(IID_IFXMeshGroup, (void**)&pMeshGroup);
  }

  if(IFXSUCCESS(result) && (pMeshGroup->GetNumElements() != m_uNumElements))
  {
    Deallocate();
  }

  if(IFXSUCCESS(result))
  {
    m_uNumElements = pMeshGroup->GetNumMeshes();
    IFXASSERT( !m_ppMeshes );
    m_ppMeshes = new IFXMesh*[m_uNumElements];

    if(m_ppMeshes)
    {
      if(bCopyElements)
      {
        U32 i;
        for( i = 0; IFXSUCCESS(result) && i < m_uNumElements; i++)
        {
          result = IFXCreateComponent(CID_IFXMesh, IID_IFXMesh, (void**)&(m_ppMeshes[i]));
          if(IFXSUCCESS(result))
          {
            IFXMesh* pTmpMesh = NULL;
            pMeshGroup->GetMesh(i, pTmpMesh);
            result = m_ppMeshes[i]->Copy(*pTmpMesh);
            IFXRELEASE(pTmpMesh);
          }
        }
      }
      else
      {
        U32 i;
        for( i = 0; IFXSUCCESS(result) && i < m_uNumElements; i++)
        {
          result = pMeshGroup->GetMesh(i, m_ppMeshes[i]);
        }
      }
    }
    else
    {
      result = IFX_E_OUT_OF_MEMORY;
    }
  }

  if(IFXSUCCESS(result))
  {
    IFXShaderList** ppShaders = NULL;
    pMeshGroup->GetShaderDataArray(&ppShaders);
    result = AllocateShaders(ppShaders, bCopyShaders);
  }
  IFXRELEASE(pMeshGroup);

  if(!IFXSUCCESS(result))
  {
    Deallocate();
  }

  return result;
}


IFXRESULT CIFXMeshGroup::GetMesh(U32 uMeshNum, IFXMesh*& pMesh)
{
  IFXRESULT result = IFX_OK;

  IFXASSERTBOX(uMeshNum < m_uNumElements, "Invalid mesh request");
  if(uMeshNum >= m_uNumElements)
  {
    result = IFX_E_INVALID_RANGE;
  }
  else
  {
    pMesh = m_ppMeshes[uMeshNum];
    if(pMesh)
    {
      pMesh->AddRef();
    }
  }

  return result;
}

IFXRESULT CIFXMeshGroup::SetMesh(U32 uMeshNum, IFXMesh* pMesh)
{
  IFXRESULT result = IFX_OK;

  IFXASSERTBOX(uMeshNum < m_uNumElements, "Invalid mesh request");
  if(uMeshNum >= m_uNumElements)
  {
    result = IFX_E_INVALID_RANGE;
  }
  else
  {
    if(pMesh)
    {
      pMesh->AddRef();
    }
    IFXRELEASE(m_ppMeshes[uMeshNum]);
    m_ppMeshes[uMeshNum] = pMesh;
  }

  return result;
}


IFXRESULT CIFXMeshGroup::ReverseNormals()
{
  IFXMesh* pMesh = 0;
  IFXVector3Iter normIter;
  IFXVector3* pNorm = 0;

  U32 i;
  for( i = 0; i < m_uNumElements; i++)
  {
    pMesh = m_ppMeshes[i];

    pMesh->GetNormalIter(normIter);

    U32 n;
    for( n = 0; n < pMesh->GetNumVertices(); n++)
    {
      pNorm = normIter.Next();

      pNorm->X() = -pNorm->X();
      pNorm->Y() = -pNorm->Y();
      pNorm->Z() = -pNorm->Z();
    }
  }

  return IFX_OK;
}


const IFXVector4& CIFXMeshGroup::CalcBoundingSphere()
{
  const float minFloats[] = { FLT_MAX, FLT_MAX, FLT_MAX, 0 };
  const float maxFloats[] = {-FLT_MAX,-FLT_MAX,-FLT_MAX, 0 };
  IFXVector4 vmin(minFloats);
  IFXVector4 vmax(maxFloats);

  U32 uTotalVertexCount = 0;

  // Determine the axis aligned bounding box and the number of verticies.
  IFXVector3* vertex;
  IFXVector3Iter vIter;
  U32 m;
  for(  m = 0; m < m_uNumElements; m++ )
  {
    const U32 uVertexCount = m_ppMeshes[m]->GetNumVertices();
    uTotalVertexCount += uVertexCount;

    m_ppMeshes[m]->GetPositionIter(vIter);
    U32 i;
    for(  i = 0; i < uVertexCount; i++ )
    {
      vertex = vIter.Next();
      if ( vertex->X() < vmin.X() ) vmin.X() = vertex->X();
      if ( vertex->X() > vmax.X() ) vmax.X() = vertex->X();
      if ( vertex->Y() < vmin.Y() ) vmin.Y() = vertex->Y();
      if ( vertex->Y() > vmax.Y() ) vmax.Y() = vertex->Y();
      if ( vertex->Z() < vmin.Z() ) vmin.Z() = vertex->Z();
      if ( vertex->Z() > vmax.Z() ) vmax.Z() = vertex->Z();
    }
  }

  // If there are any verticies, find the average position as the center,
  // and the distance to the furthest point as the radius.
  if ( uTotalVertexCount )
  {
    vmin.Add(vmax);
    vmin.Scale3(0.5f);
    m_bound = vmin;
    F32 fMaxSquaredDistance = -FLT_MAX;
    F32 fSquaredDistance;
    IFXVector3 d;
    U32 m;
    for(  m = 0; m < m_uNumElements; m++ )
    {
      const U32 uVertexCount = m_ppMeshes[m]->GetNumVertices();

      m_ppMeshes[m]->GetPositionIter(vIter);
      U32 i;
      for(  i = uVertexCount; i--; )
      {
        vertex = vIter.Next();
        d.X() = vertex->X() - m_bound.X();
        d.Y() = vertex->Y() - m_bound.Y();
        d.Z() = vertex->Z() - m_bound.Z();
        fSquaredDistance = d.DotProduct(d);
        if ( fSquaredDistance > fMaxSquaredDistance )
          fMaxSquaredDistance = fSquaredDistance;
      }
    }
    m_bound.A() = sqrtf( fMaxSquaredDistance );
  }
  else
    m_bound.Set( 0.0, 0.0, 0.0, 0.0 );

  return m_bound;
}


IFXRESULT CIFXMeshGroup::GetBoundHierarchy( IFXBoundHierarchy** ppOutBH )
{
  IFXRESULT result = IFX_OK;

  if( ppOutBH )
  {
    CIFXAABBHierarchyBuilder aabbBuilder;
    BOOL                     bRebuild         = FALSE;
    U32*                     puPositionCounts = NULL;
    U32*                     puFaceCounts     = NULL;
    U32                      uPositionCount;
    U32                      uFaceCount;
    U32                      i;

    if( m_pBoundHierarchy )
    {
      // Check the change counts to determine if we must
      // rebuild the existing boundHierarchy

      m_pBoundHierarchy->GetPositionCounts(&puPositionCounts);
      m_pBoundHierarchy->GetFaceCounts(&puFaceCounts);

	  if (puPositionCounts && puFaceCounts) {
		  // Check each count
		  for(i=0; i<m_uNumElements && IFXSUCCESS(result); i++)
		  {
			  result = m_ppMeshes[i]->GetVersionWord(IFX_MESH_POSITION, uPositionCount);
			  if( IFXSUCCESS(result) )
				  result = m_ppMeshes[i]->GetVersionWord(IFX_MESH_FACE, uFaceCount);
			  if( IFXSUCCESS(result) )
			  {
				  if( (uPositionCount != puPositionCounts[i]) ||
					  (uFaceCount     != puFaceCounts[i])      )
				  {
					  bRebuild = TRUE;
					  break;
				  }
			  }
		  }
	  } else
		  result = IFX_E_NOT_INITIALIZED;
    }
    else
    {
      // No boundHierarchy exists for this meshGroup
      bRebuild = TRUE;
    }

    if( bRebuild && IFXSUCCESS(result) )
    {
      IFXRELEASE(m_pBoundHierarchy);

      // Construct the boundHierarchy
      result = aabbBuilder.Build(&m_pBoundHierarchy, IFX_BV_AABB, this);

      // If result == IFX_CANCEL then it means the current IFXMeshGroup
      // has geometry information, but no actual geometry - this could be
      // the case if a large model is being streamed - i.e. vertex/face
      // information is available for each mesh, but the actual vertex data
      // has not yet streamed in. If this is the case we simply return a
      // valid boundHierarchy pointer, with no information.  This will
      // insure that a valid boundHierachy is created - but has no mesh
      // information in it.

      if( IFXSUCCESS(result) && result != IFX_CANCEL )
      {
        m_pBoundHierarchy->GetPositionCounts(&puPositionCounts);
        m_pBoundHierarchy->GetFaceCounts(&puFaceCounts);

        for(i=0; i<m_uNumElements && IFXSUCCESS(result); i++)
        {
          result = m_ppMeshes[i]->GetVersionWord(IFX_MESH_POSITION, uPositionCount);
          if( IFXSUCCESS(result) )
            result = m_ppMeshes[i]->GetVersionWord(IFX_MESH_FACE, uFaceCount);
          if( IFXSUCCESS(result) )
          {
            puPositionCounts[i] = uPositionCount;
            puFaceCounts[i]     = uFaceCount;
          }
        }
      }
    }

    if( IFXSUCCESS(result) )
    {
      *ppOutBH  = m_pBoundHierarchy;
      (*ppOutBH)->AddRef();
    }
  }
  else
    result = IFX_E_INVALID_POINTER;

  return result;
}
