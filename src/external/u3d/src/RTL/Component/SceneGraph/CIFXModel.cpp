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

/*
@file  CIFXModel.cpp                                                         
*/

#include "IFXSceneGraphPCH.h"
#include "CIFXModel.h"
#include "IFXModifierChain.h"
#include "IFXModifierDataPacket.h"
#include "IFXLightSet.h"

#include "IFXCheckX.h"
#include "IFXEncoderX.h"
#include "IFXException.h"
#include "IFXDids.h"
#include "IFXShaderLitTexture.h"
#include "IFXMaterialResource.h"

#include "IFXCoreCIDs.h"
#include "IFXRenderingCIDs.h"
#include "IFXExportingCIDs.h"

#define   IFX_MAX_FLOAT     (F32)(U32)(-1)
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

IFXFrustum CIFXModel::frustum;
IFXMeshPtr CIFXModel::m_sUnitSphereMesh;
U32        CIFXModel::m_suBoundFaceCount = 0;

IFXRESULT IFXAPI CIFXModel::Shutdown()
{
  IFXASSERTBOXEX(!m_sUnitSphereMesh.IsValid(), "CIFXModel is leaked!", FALSE, TRUE);
  m_sUnitSphereMesh = 0;

  return IFX_OK;
}

CIFXModel::CIFXModel() :
  IFXDEFINEMEMBER(m_pLightSet),
  IFXDEFINEMEMBER(m_pBoundFrameBounds),
  IFXDEFINEMEMBER(m_BoundFrameMeshGroupRndr)
{
  m_sUnitSphereMesh.IncRef();
  m_uLightSetDataElementIndex         = (U32)-1;
  m_uFrustumDataElementIndex          = (U32)-1;
  m_uViewSizeDataElementIndex         = (U32)-1;
  m_uBoundFrameDataElementIndex       = (U32)-1;
  m_uBoundFrameBoundsDataElementIndex = (U32)-1;

  m_bDebug                            = FALSE;
  m_uVisible                          = 1<<FRONT_FACE_VISIBILITY;

  m_pFrustum                          = &frustum; // Do not deallocate as this pointer refers to a strucutre owned by a CIFXView.

  m_uPickable                         = 1; // Front

  m_bProcessingBoundFrameBounds       = FALSE;
  m_uBoundFrameMaterial               = (U32)-1;
  m_uBoundFrameShader                 = (U32)-1;
  m_bBoundFrameAllocated              = FALSE;

  m_bAttachedToModChain       = FALSE;
  m_bWorldSphereDirty         = TRUE;

  m_pResourceSubjectNR = NULL;
}


CIFXModel::~CIFXModel()
{
  if( m_pResourceSubjectNR )
  {
    m_pResourceSubjectNR->Detach(this);
    m_pResourceSubjectNR = NULL;
  }

  m_sUnitSphereMesh.DecRef();
}


IFXRESULT IFXAPI_CALLTYPE CIFXModel_Factory(IFXREFIID riid, void **ppv)
{
  IFXRESULT result;

  if ( ppv )
  {
    // Create the CIFXClassName component.
    CIFXModel *pModel = new CIFXModel;

    if ( pModel )
    {
      // Perform a temporary AddRef for our usage of the component.
      pModel->AddRef();

      // Attempt to obtain a pointer to the requested interface.
      result = pModel->QueryInterface( riid, ppv );

      // Perform a Release since our usage of the component is now
      // complete.  Note:  If the QI fails, this will cause the
      // component to be destroyed.
      pModel->Release();
    }
    else result = IFX_E_OUT_OF_MEMORY;
  }
  else result = IFX_E_INVALID_POINTER;

  return result;
}


// IFXUnknown
U32 CIFXModel::AddRef()
{

  return ++m_uRefCount;
}


U32 CIFXModel::Release()
{

  if( m_uRefCount == 1 )
  {
    CIFXNode::PreDestruct();
    delete this;
    return 0;
  }
  else
    return --m_uRefCount;
}


IFXRESULT CIFXModel::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
  IFXRESULT result  = IFX_OK;

  if ( ppInterface )
  {
    if ( interfaceId == IID_IFXModel )
      *ppInterface = ( IFXModel* ) this;
    else if ( interfaceId == IID_IFXSpatialAssociation )
      *ppInterface = ( IFXSpatialAssociation* ) this;
    else if ( interfaceId == IID_IFXMarker )
      *ppInterface = ( IFXMarker* ) this;
    else if ( interfaceId == IID_IFXMarkerX )
      *ppInterface = ( IFXMarkerX* ) this;
    else if ( interfaceId == IID_IFXUnknown )
      *ppInterface = ( IFXUnknown* ) this;
    else if ( interfaceId == IID_IFXCollection )
      *ppInterface = ( IFXCollection* ) this;
    else if ( interfaceId == IID_IFXSpatial )
      *ppInterface = ( IFXSpatial* ) this;
    else if ( interfaceId == IID_IFXNode )
      *ppInterface = ( IFXNode* ) this;
    else if ( interfaceId == IID_IFXModifier)
      *ppInterface = ( IFXModifier* ) this;
    else if ( interfaceId == IID_IFXResourceClient)
      *ppInterface = ( IFXResourceClient* ) this;
    else if ( interfaceId == IID_IFXSubject )
      *ppInterface = ( IFXSubject* ) this;
    else if ( interfaceId == IID_IFXObserver )
      *ppInterface = ( IFXObserver* ) this;
    else if ( interfaceId == IID_IFXMetaDataX )
      *ppInterface = ( IFXMetaDataX* ) this;
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


// IFXMarker
IFXRESULT CIFXModel::SetSceneGraph( IFXSceneGraph* pInSceneGraph )
{
  IFXRESULT result = IFX_OK;

  if (!m_pBoundFrameBounds)
    result = IFXCreateComponent(CID_IFXBoundSphereDataElement, IID_IFXBoundSphereDataElement, (void**) &m_pBoundFrameBounds);
  if ( IFXSUCCESS(result) )
    result = CIFXNode::SetSceneGraph(pInSceneGraph);

  return result;
}


IFXRESULT CIFXModel::Mark(void)
{
  // NOTE:  we would...
  IFXRESULT result = IFX_OK;

  if ( m_pSceneGraph == NULL )
    result = IFX_E_NOT_INITIALIZED;

  if ( IFXSUCCESS(result) )
    result = CIFXNode::Mark();

  // mark the resource, but only if we've got a valid resourceID:
  if (IFXSUCCESS(result))
  {
    IFXPalette* pGeneratorPalette = NULL;
    result = m_pSceneGraph->GetPalette( IFXSceneGraph::GENERATOR,
      &pGeneratorPalette );

    if (IFXSUCCESS(result))
    {
      IFXMarker* pGenerator = NULL;

      result = pGeneratorPalette->GetResourcePtr( m_uResourceIndex,
        IID_IFXMarker,
        (void**)&pGenerator );
      if ( IFXSUCCESS( result ) )
      {
        pGenerator->Mark();
        IFXRELEASE( pGenerator );
      }
      else if (    (IFX_E_PALETTE_NULL_RESOURCE_POINTER == result)
        || (IFX_E_INVALID_RANGE == result) )
        result = IFX_OK ;

      IFXRELEASE( pGeneratorPalette );
    }
  }

  return result;
}


void CIFXModel::SetPriority( U32 uInPriority,
              BOOL bRecursive,
              BOOL bPromotionOnly )
{
  IFXRESULT rc = IFX_OK;

  if ( m_pSceneGraph == NULL )
    rc = IFX_E_NOT_INITIALIZED;

  if ( IFXSUCCESS( rc ) )
  {
    // the node-traversal code is in CIFXNode::SetPriority()
    CIFXNode::SetPriority( uInPriority, bRecursive, bPromotionOnly );
  }

  if ( (bRecursive == IFX_TRUE) && IFXSUCCESS( rc ) )
  {
    IFXPalette* pGeneratorPalette = NULL ;

    // Set the priority of the resource, but only if we've got a
    // valid resourceID:
    if (m_uResourceIndex != (U32)(-1))
    {
      rc = m_pSceneGraph->GetPalette( IFXSceneGraph::GENERATOR,
        &pGeneratorPalette );

      if (IFXSUCCESS(rc))
      {
        IFXMarker* pGenerator = NULL;
        rc = pGeneratorPalette->GetResourcePtr( m_uResourceIndex,
          IID_IFXMarker,
          (void**)&pGenerator );
        if (IFXSUCCESS(rc))
          pGenerator->SetPriority( uInPriority );
        else if (    (IFX_E_PALETTE_NULL_RESOURCE_POINTER == rc)
          || (IFX_E_INVALID_RANGE == rc) )
          rc = IFX_OK ;

        IFXRELEASE( pGenerator );
        IFXRELEASE( pGeneratorPalette );
      } // end if (we got the generator palette)
    } // end if (m_uModelGenerator != (U32)(-1))
  } // end if ( (bRecursive == IFX_TRUE) && IFXSUCCESS( rc ) )

  return ;

}


// IFXMarkerX interface...
void CIFXModel::GetEncoderX(IFXEncoderX*& rpEncoderX)
{
  CIFXMarker::GetEncoderX(CID_IFXModelNodeEncoder, rpEncoderX);
}

IFXRESULT CIFXModel::GetSpatialBound( IFXVector4& rOutSphere, U32 WorldInstance )
{
	IFXModifierChain* pModifierChain = NULL;
	IFXVector4 pRenderableBounds;
	IFXModifierDataPacket* pDataPacket = NULL;

	IFXRESULT result = GetModifierChain( &pModifierChain );


	if (IFXSUCCESS(result))
		result = pModifierChain->GetDataPacket( pDataPacket );

	IFXMatrix4x4* pWorldTransform = NULL;
	IFXArray<IFXMatrix4x4> *pAWorldTransform;

	if (IFXSUCCESS(result))
		result = pDataPacket->GetDataElement( m_uMyDataPacketTransformIndex,
		(void**)&pAWorldTransform );

	if (IFXSUCCESS(result))
	{
		F32 radius;
		//F32 Maxx = -IFX_MAX_FLOAT;

		IFXModifierDataElementIterator* pDEItor = NULL;
		pDataPacket->GetIterator( IFX_DID_BOUND, &pDEItor );
		IFXDECLARELOCAL(IFXBoundSphereDataElement, pBound);
		IFXDECLARELOCAL(IFXUnknown, pBoundUnk);
		pBoundUnk = (IFXUnknown*)pDEItor->First();
		if (pBoundUnk)
			pBoundUnk->QueryInterface( IID_IFXBoundSphereDataElement, (void**)&pBound );
		if ( pBound )
			rOutSphere = pBound->Bound();
		while ( pBound )
		{
			pRenderableBounds = pBound->Bound();
			rOutSphere.IncorporateSphere( &pRenderableBounds );
			IFXRELEASE(pBoundUnk);
			pBoundUnk = (IFXUnknown*)pDEItor->Next();
			IFXRELEASE(pBound);
			if (pBoundUnk)
				pBoundUnk->QueryInterface( IID_IFXBoundSphereDataElement, (void**)&pBound );
		}
		pDEItor->Release();

		radius = rOutSphere.Radius();

		pWorldTransform = &pAWorldTransform->GetElement(WorldInstance);

		IFXVector3 src(rOutSphere.RawConst()), res;
		// store previous radius to prevent an overwrite in Set
		pWorldTransform->TransformVector(src.RawConst(), res);
		rOutSphere = res;

		// Compensate for scale
		IFXVector3 scale;
		pWorldTransform->CalcAxisScale(scale);
		rOutSphere.Radius() = radius*max( scale.X(), max( scale.Y(), scale.Z() ));
	}
	else
	{
		IFXTRACE_GENERIC(L"Failed attempt to build a spatial bound without a ModifierChain!\n");
		result = IFX_OK;
	}

	IFXRELEASE(pDataPacket);
	IFXRELEASE(pModifierChain);

	return result;
}


IFXSpatial::eType CIFXModel::GetSpatialType()
{
  return IFXSpatial::OPAQUE_MODEL;
}


// IFXNode
void CIFXModel::Counter(EIFXNodeCounterType type, U32* pOutCount)

{
  if( !pOutCount )
  {
    IFXTRACE_GENERIC(L"CIFXModel::Counter() called with an invalid pointer! Line:%d\n", __LINE__);
    return;
  }

  U32 childCount = m_pChildren.GetNumberElements();
  while (childCount) 
  {
	  m_pChildren[--childCount]->Counter(type, pOutCount);
  }

  if ((type == IFX_NODE_COUNTER_NODES) ||
      (type == IFX_NODE_COUNTER_MODELS))
  {
    (*pOutCount)++;
  }
  else if (type == IFX_NODE_COUNTER_MODELS_POLYGONS)
  {
    IFXModifierChain* pModifierChain;
    if (IFXSUCCESS(GetModifierChain( &pModifierChain )))
    {
      IFXModifierDataPacket* pDataPacket;
      if (IFXSUCCESS(pModifierChain->GetDataPacket( pDataPacket )))
      {
        U32 index;
        if (IFXSUCCESS(pDataPacket->GetDataElementIndex( 
											DID_IFXRenderableGroup, index )))
        {
          IFXMeshGroup *pMeshGroup;
          if (IFXSUCCESS(pDataPacket->GetDataElement( 
											index, 
											IID_IFXMeshGroup,
											(void**)&pMeshGroup )))
          {
            const U32 numMeshes = pMeshGroup->GetNumMeshes();
            IFXMesh *pMesh;
			U32 meshNum;
            for(meshNum = 0 ; meshNum < numMeshes ; meshNum++)
            {
              pMesh = NULL;
              pMeshGroup->GetMesh(meshNum, pMesh);

              if( pMesh )
              {
                *pOutCount += pMesh->GetNumFaces();
                pMesh->Release();
              }
            }
            pMeshGroup->Release();
          }
        }

        pDataPacket->Release();
      }
      else
        IFXTRACE_GENERIC(
			L"CIFXModel::Counter() failed due to missing DataPacket. Line:%d\n", 
			__LINE__);

	  pModifierChain->Release();
    }
    else
      IFXTRACE_GENERIC(
		L"CIFXModel::Counter() failed due to missing ModifierChain. Line:%d\n", 
		__LINE__);
  }
}


// IFXModifier
const IFXGUID* s_scpOutputDIDs[] =
{
	&DID_IFXTransform,
    &DID_IFXLightSet,
    &DID_IFXFrustum,
    &DID_IFXViewSize,
    &DID_IFXBoundFrame,
    &DID_IFXBoundFrameBounds,
    &DTS_IFXRenderable,
    &DTS_IFXBound
};


static const IFXGUID* s_scpTransformInputDependencies[] =
{
  &DID_IFXTransform
};


IFXRESULT CIFXModel::GetOutputs( IFXGUID**& rpOutOutputs,
                U32&       rOutNumberOfOutputs,
                U32*&      rpOutOutputDepAttrs )
{
  rOutNumberOfOutputs = IFXARRAY_SIZE(s_scpOutputDIDs);
  rpOutOutputs = (IFXGUID**)&s_scpOutputDIDs;
  rpOutOutputDepAttrs = NULL;

  return IFX_OK;
}


IFXRESULT CIFXModel::GetDependencies(
                   IFXGUID*   pInOutputDID,
                   IFXGUID**& rppOutInputDependencies,
                   U32&       rOutNumberInputDependencies,
                   IFXGUID**& rppOutOutputDependencies,
                   U32&       rOutNumberOfOutputDependencies,
                   U32*&      rpOutOutputDepAttrs )
{
  IFXRESULT result = IFX_OK;

  if (pInOutputDID == &DID_IFXTransform)
  {
    rppOutInputDependencies = (IFXGUID**)s_scpTransformInputDependencies;
    rOutNumberInputDependencies = sizeof(s_scpTransformInputDependencies) / sizeof(IFXGUID*);
    rppOutOutputDependencies = NULL;
    rOutNumberOfOutputDependencies = 0;
    rpOutOutputDepAttrs = NULL;
  }
  else if ( (pInOutputDID == &DID_IFXLightSet)
    ||(pInOutputDID == &DID_IFXFrustum)
    ||(pInOutputDID == &DID_IFXViewSize)
    ||(pInOutputDID == &DTS_IFXRenderable)
    ||(pInOutputDID == &DTS_IFXBound)
    ||(pInOutputDID == &DID_IFXBoundFrame)
    ||(pInOutputDID == &DID_IFXBoundFrameBounds) )
  {
    rppOutInputDependencies = NULL;
    rOutNumberInputDependencies = 0;
    rppOutOutputDependencies = NULL;
    rOutNumberOfOutputDependencies = 0;
    rpOutOutputDepAttrs = NULL;
  }
  else
  {
    IFXTRACE_GENERIC( L"CIFXModel::GetDependencies() called with unknown output!" );
    result = IFX_E_UNDEFINED;
  }

  return result;
}

IFXRESULT CIFXModel::GenerateOutput( U32    inOutputDataElementIndex,
                  void*& rpOutData, BOOL& rNeedRelease )
{
  IFXRESULT result = IFX_OK;

  if ( inOutputDataElementIndex == m_uMyDataPacketTransformIndex )
  {
    // should move the node's generate output to the case where this isn't a model specific request
    result = CIFXNode::GenerateOutput( inOutputDataElementIndex, rpOutData, rNeedRelease );
	if (IFXSUCCESS(result)) 
	{
		IFXArray<IFXMatrix4x4>* pResourceTransform = NULL;
		result = m_pInputDataPacket->GetDataElement(m_uMyDataPacketTransformIndex, (void**)&pResourceTransform);
		if (IFXSUCCESS(result)) 
		{
			IFXArray<IFXMatrix4x4>* pMyTransform = (IFXArray<IFXMatrix4x4>*)rpOutData;
			U32 i;
			for (i = 0; i < pMyTransform->GetNumberElements(); i++)
			{
				/* 
					Workaround for case when resource transform matrix is not 
					identity and used in node hierarchy.

					Apply resource transform element only to the 1st model in 
					hierarchy, model which has World node as a parent.
					If parent node is not model (ex. group), apply transform element.
				*/  
				IFXNode* pParent = this->GetParentNR(i);

				if (NULL != pParent)
				{
					IFXNode* pModel = NULL;
					//U32 parentNum = pParent->GetNumberOfParents();

					if(IFXFAILURE(pParent->QueryInterface(IID_IFXModel, (void**)&pModel)))
					{
						IFXMatrix4x4* pMatrix = &pMyTransform->GetElement(i);
						IFXMatrix4x4 matrix = *pMatrix;
						pMatrix->Multiply((*pResourceTransform)[0], matrix);
					}

					IFXRELEASE(pModel);
				}
			}
		}
	}
  }
  else if ( inOutputDataElementIndex == m_uLightSetDataElementIndex )
  {
    m_pLightSet->QueryInterface( IID_IFXUnknown, (void**)&rpOutData );
	rNeedRelease = TRUE;
  }
  else if ( inOutputDataElementIndex == m_uFrustumDataElementIndex )
  {
    rpOutData = m_pFrustum;
  }
  else if ( inOutputDataElementIndex == m_uViewSizeDataElementIndex )
  {
    rpOutData = (void*)(uintptr_t)m_uViewSize;
  }
  else if ( inOutputDataElementIndex == m_uBoundFrameDataElementIndex )
  {
    if(!m_BoundFrameMeshGroup.IsValid() || !m_sUnitSphereMesh.IsValid())
    {
      result = _AllocateBoundFrame();
    }

    result = m_BoundFrameMeshGroup->QueryInterface( IID_IFXUnknown, &rpOutData );
	rNeedRelease = TRUE;
  }
  else if ( inOutputDataElementIndex == m_uBoundFrameBoundsDataElementIndex )
  {
    if (!m_bProcessingBoundFrameBounds)
    {
      m_bProcessingBoundFrameBounds = TRUE;
      IFXModifierChain* pModifierChain = NULL;
      if (IFXSUCCESS(GetModifierChain(&pModifierChain)))
      {
        IFXModifierDataPacket* pDP = NULL;
        if (IFXSUCCESS(pModifierChain->GetDataPacket(pDP)))
        {
          // Build a bound around the axis and all OTHER bounding data elements.
          IFXModifierDataElementIterator* pRenderableBoundIter = NULL;
          if (IFXSUCCESS(pDP->GetIterator( IFX_DID_BOUND,
            &pRenderableBoundIter )))
          {
            IFXDECLARELOCAL(IFXBoundSphereDataElement, pBound);
            IFXDECLARELOCAL(IFXUnknown, pBoundUnk);
			pBoundUnk = (IFXUnknown*)pRenderableBoundIter->First();
            if (pBoundUnk)
              pBoundUnk->QueryInterface( IID_IFXBoundSphereDataElement, (void**)&pBound );

            if(pBound)
            {
              // Step over the m_BoundFrameBounds DataElement (if it is first).
              if ( pRenderableBoundIter->CurIndex()==m_uBoundFrameBoundsDataElementIndex )
              {
				IFXRELEASE(pBoundUnk);
                pBoundUnk = (IFXUnknown*)pRenderableBoundIter->Next();
				IFXRELEASE(pBound);
                if (pBoundUnk)
                  pBoundUnk->QueryInterface( IID_IFXBoundSphereDataElement, (void**)&pBound );
              }

              if(pBound)
              {
                // Seed the bound.
                m_pBoundFrameBounds->Bound() = pBound->Bound();
				IFXRELEASE(pBoundUnk);
                pBoundUnk = (IFXUnknown*)pRenderableBoundIter->Next();
				IFXRELEASE(pBound);
                if (pBoundUnk)
                  pBoundUnk->QueryInterface( IID_IFXBoundSphereDataElement, (void**)&pBound );

                // Step over the m_BoundFrameBounds DataElement.
                while (pBound)
                {
                  if (pRenderableBoundIter->CurIndex()!=m_uBoundFrameBoundsDataElementIndex)
                    m_pBoundFrameBounds->Bound().IncorporateSphere( pBound->Bound() );

				  IFXRELEASE(pBoundUnk);
                  pBoundUnk = (IFXUnknown*)pRenderableBoundIter->Next();
				  IFXRELEASE(pBound);
                  if (pBoundUnk)
                    pBoundUnk->QueryInterface( IID_IFXBoundSphereDataElement, (void**)&pBound );
                }
              }
            }
            else
              m_pBoundFrameBounds->Bound().Set(0.0f, 0.0f, 0.0f, 1.0f);

            pRenderableBoundIter->Release();
          }
          pDP->Release();
        }
        pModifierChain->Release();
      }
      m_bProcessingBoundFrameBounds = FALSE;
    }
    m_pBoundFrameBounds->QueryInterface( IID_IFXUnknown, &rpOutData );
	rNeedRelease = TRUE;
  }
  else // must be a renderable or bound
  {
    IFXDECLARELOCAL(IFXUnknown, pUnknown);

    result = m_pInputDataPacket->GetDataElement(inOutputDataElementIndex, (void**) &pUnknown);

    if(IFXSUCCESS(result))
    {
      IFXRenderable* pRenderable = NULL;
      result = pUnknown->QueryInterface(IID_IFXRenderable, (void**)&pRenderable);

      if(IFXSUCCESS(result)) 
      {
		// Renderable
        pRenderable->QueryInterface( IID_IFXUnknown, &rpOutData );
		rNeedRelease = TRUE;

	    IFXRELEASE( pRenderable );
      }
      else 
      {
		// Bound
        IFXBoundSphereDataElement* pBDE = NULL;

        result = m_pInputDataPacket->GetDataElement(inOutputDataElementIndex, IID_IFXBoundSphereDataElement, (void**)&pBDE);
        IFXRELEASE( pBDE );

		if(IFXSUCCESS(result))
		  result = m_pInputDataPacket->GetDataElement(inOutputDataElementIndex, (void**)&rpOutData);

	    rNeedRelease = TRUE;

        if(IFXSUCCESS(result))
		{
          m_pModifierDataPacket->InvalidateDataElement( m_uBoundFrameDataElementIndex );
          m_pModifierDataPacket->InvalidateDataElement( m_uBoundFrameBoundsDataElementIndex );
		}
      }
    }
  }

  return result; 
}


IFXRESULT CIFXModel::SetDataPacket( IFXModifierDataPacket* pInInputDataPacket,
                   IFXModifierDataPacket* pInOutputDataPacket )
{
  IFXRESULT result = CIFXNode::SetDataPacket( pInInputDataPacket, pInOutputDataPacket );

  if( pInInputDataPacket && pInOutputDataPacket )
  {
    if(IFXSUCCESS(result))
      result = pInOutputDataPacket->GetDataElementIndex( DID_IFXLightSet,
      m_uLightSetDataElementIndex );
    if(IFXSUCCESS(result))
      result = pInOutputDataPacket->GetDataElementIndex( DID_IFXFrustum,
      m_uFrustumDataElementIndex );
    if(IFXSUCCESS(result))
      result = pInOutputDataPacket->GetDataElementIndex( DID_IFXViewSize,
      m_uViewSizeDataElementIndex );
    if(IFXSUCCESS(result))
      result = pInOutputDataPacket->GetDataElementIndex( DID_IFXBoundFrame,
      m_uBoundFrameDataElementIndex );
    if(IFXSUCCESS(result))
      result = pInOutputDataPacket->GetDataElementIndex( DID_IFXBoundFrameBounds,
      m_uBoundFrameBoundsDataElementIndex );
    if(IFXSUCCESS(result))
      m_pBoundFrameBounds->RenderableIndex() = m_uBoundFrameDataElementIndex;
  }

  return result;
}

const IFXLightSet& CIFXModel::GetLightSet()
{
  IFXLightSet* pLightSet = NULL;

  IFXModifierChain* pModChain = 0;
  GetModifierChain(&pModChain);

  if(pModChain)
  {
    IFXModifierDataPacket* pDataPacket = 0;
    pModChain->GetDataPacket(pDataPacket);

    if(pDataPacket)
    {
      pDataPacket->GetDataElement(m_uLightSetDataElementIndex, IID_IFXLightSet, (void**)&pLightSet);
      pDataPacket->Release();
    }

    if(pLightSet)
      pLightSet->Release();

    pModChain->Release();
  }

  return *pLightSet;
}

IFXRESULT CIFXModel::Notify( IFXModifierMessage eInMessage,
              void*               pMessageContext           )
{
  if( eInMessage == IFXModifier::NEW_MODCHAIN_STATE)
  {
    AttachToModChainForWorldPositionChange();
    m_bAttachedToModChain = TRUE;
  }
  return IFX_OK;
}



// IFXModel
IFXRESULT CIFXModel::SetFrustum( const IFXFrustum& rInFrustum )
{
  m_pFrustum = &(IFXFrustum&)rInFrustum;

  if( m_pModifierDataPacket )
    return m_pModifierDataPacket->InvalidateDataElement( m_uFrustumDataElementIndex );
  else
    return IFX_OK;
}


IFXRESULT CIFXModel::SetViewSize( U32 uInViewSize )
{
  m_uViewSize = uInViewSize;

  if( m_pModifierDataPacket )
    return m_pModifierDataPacket->InvalidateDataElement( m_uViewSizeDataElementIndex );
  else
    return IFX_OK;
}


// IFXSpatial Association
U32 CIFXModel::GetAssociatedSpatialCount( IFXSpatialAssociation::Type eInAssociationType )
{
  U32 cnt=0;

  if( eInAssociationType == LIGHTS )
  {
    if ( m_pLightSet )
      cnt = m_pLightSet->GetNumLights();
  }
  else
    IFXASSERT(0); // Add logic for new associated spatial type.

  return cnt;
}


IFXRESULT CIFXModel::GetAssociatedSpatial( U32 uInSpatialIndex,
                      IFXSpatial** ppOutSpatial,
                      IFXSpatialAssociation::Type eInAssociationType )
{
  IFXRESULT result = IFX_OK;

  if( eInAssociationType == LIGHTS )
  {
    if ( m_pLightSet )
    {
      IFXLight* pLight = NULL;
	  U32 lightInstance; ///@todo: (MP) investigate how to return Instance list
      result = m_pLightSet->GetLight( uInSpatialIndex, pLight, lightInstance );

      if( IFXSUCCESS(result) )
        result = pLight->QueryInterface( IID_IFXSpatial, (void**)ppOutSpatial );

      IFXRELEASE(pLight);
    }
    else
    {
      ppOutSpatial = NULL;
      result = IFX_E_INVALID_RANGE;
    }
  }
  else
    IFXASSERT(0); // Add logic for new associated spatial type.

  return result;
}


IFXRESULT CIFXModel::SetAssociatedSpatials( SPATIALINSTANCE_LIST* pInSpatials,
                       U32 uInSpatialCount,
                       IFXSpatialAssociation::Type eInAssociationType )
{
  IFXRESULT result = IFX_OK;

  if( eInAssociationType == LIGHTS )
  {
    if( !m_pLightSet )
    {
      result = IFXCreateComponent( CID_IFXLightSet,
        IID_IFXLightSet,
        (void**)&m_pLightSet );
    }

    if( IFXSUCCESS(result) )
      result = m_pLightSet->Set( uInSpatialCount, &pInSpatials );
    if( IFXSUCCESS(result) && m_pModifierDataPacket )
      result = m_pModifierDataPacket->
      InvalidateDataElement( m_uLightSetDataElementIndex );
  }
  else
    IFXASSERT(0); // Add logic for new associated spatial type.

  return result;
}


IFXRESULT CIFXModel::AppendAssociatedSpatials( SPATIALINSTANCE_LIST* pInSpatials,
                        U32 uInSpatialCount,
                        IFXSpatialAssociation::Type eInAssociationType )
{

  IFXRESULT result = IFX_OK;

  if( eInAssociationType == LIGHTS )
  {
    if(m_pLightSet)
    {
      result = m_pLightSet->Append( uInSpatialCount, &pInSpatials );

      if( IFXSUCCESS(result) && m_pModifierDataPacket )
        result = m_pModifierDataPacket->
        InvalidateDataElement( m_uLightSetDataElementIndex );
    }
    else
    {
      result = SetAssociatedSpatials( pInSpatials,
        uInSpatialCount,
        eInAssociationType);
    }
  }
  else
    IFXASSERT(0); // Add logic for new associated spatial type.

  return result;
}

// ****************************************************************************
// ***
// ****************************************************************************

IFXRESULT CIFXModel::AttachToModChainForWorldPositionChange()
{
  IFXRESULT rc = IFX_OK;

  IFXASSERT( m_pMyDataPacketAsSubjectNR && m_pModifierDataPacket );
  if( m_pMyDataPacketAsSubjectNR && m_pModifierDataPacket )
  {
    U32 uBits = 0;
    IFXModifierDataElementIterator* pDEItor = NULL;
    IFXGUID* pBoundSphereGUID =0;

    // Iterate over bound spheres
    rc = m_pModifierDataPacket->GetIterator( IFX_DID_BOUND, &pDEItor );

    U32 boundSphereIndex=0, boundSphereInterest=0;
    IFXDECLARELOCAL(IFXBoundSphereDataElement, pBound);
    IFXDECLARELOCAL(IFXUnknown, pBoundUnk);
	pBoundUnk = (IFXUnknown*)pDEItor->First();
    if (pBoundUnk)
      pBoundUnk->QueryInterface( IID_IFXBoundSphereDataElement, (void**)&pBound );

    while ( pBound && IFXSUCCESS(rc))
    {
      boundSphereIndex = pDEItor->CurIndex();
      m_pModifierDataPacket->GetDataElementGUID( boundSphereIndex, pBoundSphereGUID );
      m_pModifierDataPacket->GetDataElementAspectBit( *pBoundSphereGUID, boundSphereInterest );

      uBits |= boundSphereInterest;

	  IFXRELEASE(pBoundUnk);
	  pBoundUnk = (IFXUnknown*)pDEItor->Next();
	  IFXRELEASE(pBound);
      if (pBoundUnk)
        pBoundUnk->QueryInterface( IID_IFXBoundSphereDataElement, (void**)&pBound );
    }

    IFXRELEASE( pDEItor );

    IFXASSERT( m_pMyIFXObserverNR );
    // Attach to modchains resultant datapacket Transform
    if( IFXSUCCESS(rc) )
      rc = m_pMyDataPacketAsSubjectNR->Attach( m_pMyIFXObserverNR,
      uBits,
      IID_IFXDataPacket );
  }

  return rc;
}

// IFXObserver
IFXRESULT CIFXModel::Update( IFXSubject* pInSubject, U32 uInChangeBits, IFXREFIID rIType)
{
  IFXRESULT rc = IFX_E_NOT_INITIALIZED;

  if( ( pInSubject == m_pMyDataPacketAsSubjectNR ) &&
	  ( uInChangeBits & m_uMyDataPacketTransformAspectBit ) )
  {
    m_pModifierDataPacket->InvalidateDataElement( m_uBoundFrameDataElementIndex );
    m_pModifierDataPacket->InvalidateDataElement( m_uBoundFrameBoundsDataElementIndex );

    m_bWorldSphereDirty = TRUE;

    PostChanges( uInChangeBits ); 
  }
  else if( ( pInSubject == m_pResourceSubjectNR ) &&
		   ( IFX_SUBJECT_PALETTE_REMOVE_RESOURCE & uInChangeBits ) )
  {
    // Set the model to the NULL resource
    SetResourceIndex( IFX_NULL_RESOURCE );
  }
  else if( ( pInSubject == m_pResourceSubjectNR ) && ( 0 == uInChangeBits ) )
  {
    // if subject is destructing...
    m_pResourceSubjectNR->Detach(this);
    m_pResourceSubjectNR = NULL;
  }
  else if( IFX_SUBJECT_PALETTE_ADD_RESOURCE & uInChangeBits )
  {
    // Cache an entry to the subject
    m_pResourceSubjectNR = pInSubject;
  }
  else
    rc = CIFXNode::Update(pInSubject, uInChangeBits, rIType);

  return rc;
}

IFXRESULT CIFXModel::_AllocateBoundFrame()
{
  IFXRESULT result = IFX_OK;

    // Meshgroup
    IFXRELEASE(m_BoundFrameMeshGroupRndr);
    IFXRUN( result, m_BoundFrameMeshGroup.Create(CID_IFXMeshGroup, IID_IFXMeshGroup) );
    IFXRUN( result, m_BoundFrameMeshGroup->QueryInterface(IID_IFXRenderable, (void**)&m_BoundFrameMeshGroupRndr));
    IFXRUN( result, m_BoundFrameMeshGroupRndr->SetEnabled(m_bDebug));

    if (IFXSUCCESS(result))
      m_bBoundFrameAllocated = TRUE;

	return result;
}


void CIFXModel::SetDebug( BOOL bDebug )
{
  m_bDebug = bDebug;

  if(m_BoundFrameMeshGroupRndr)
  {
    m_BoundFrameMeshGroupRndr->SetEnabled(m_bDebug);
  }

  if(m_pModifierDataPacket)
  {
    m_pModifierDataPacket->InvalidateDataElement(m_uBoundFrameDataElementIndex);
    m_pModifierDataPacket->InvalidateDataElement(m_uBoundFrameBoundsDataElementIndex);
  }
}


IFXRESULT CIFXModel::SetResourceIndex( U32 uResourceIndex )
{
  IFXRESULT rc = IFX_OK;

  rc = CIFXResourceClient::SetResourceIndex( uResourceIndex );

  return rc;
}
