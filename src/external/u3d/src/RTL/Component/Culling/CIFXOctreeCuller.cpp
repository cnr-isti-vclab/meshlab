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
#include "CIFXOctreeCuller.h"
#include "IFXRenderingCIDs.h"

#include "IFXNode.h"
#include "IFXAutoRelease.h"

void CIFXOctreeCuller::Initialize()
{
	m_pParentLightInstancesList = new SPATIALINSTANCE_LIST();
	m_uDirLightCnt = 0;
	m_pChildLightList =  new SPATIALINSTANCE_LIST();
	m_pAttenuatedLights = new SPATIALINSTANCE_LIST();
	m_pMinDir = new IFXVector3[MAX_CLIP_PLANES+1];
	if(!m_pSortKeys)
	{
		m_uAllocatedSortKeys = DEFAULT_ALLOC_PAD;
		m_pSortKeys = (F64*)IFXAllocate(sizeof(F64)*m_uAllocatedSortKeys);
	}
	m_bInitialized = TRUE;

}


CIFXOctreeCuller::~CIFXOctreeCuller()
{
	IFXRELEASE(m_pOctreeCollection);
	IFXDELETE(m_pParentLightInstancesList);
	IFXDELETE(m_pChildLightList);
	IFXDELETE(m_pAttenuatedLights);
	IFXDeallocate(m_pSortKeys);
	IFXDELETE_ARRAY(m_pMinDir)
	m_uAllocatedSortKeys = 0;
}


// IFXSpatialSetQuery
IFXRESULT CIFXOctreeCuller::SetCollection( IFXCollection* pInCollection )
{
	IFXRESULT iResult = IFX_E_INVALID_POINTER;
	if(pInCollection != m_pOctreeCollection)
	{
		//Check if Collection is of Type Octree
		CIFXOctreeCollection * pOctreeCollection;
		iResult = pInCollection->QueryInterface(CID_IFXOctreeCollection,(void **)&pOctreeCollection);
		if(IFXSUCCESS(iResult))
		{
			IFXRELEASE(m_pOctreeCollection);
			m_pOctreeCollection = pOctreeCollection;
		}


		if(!m_bInitialized&&IFXSUCCESS(iResult))
			Initialize();
	}

	return iResult;
}


IFXRESULT CIFXOctreeCuller::GetCollection( IFXCollection*& rpOutCollection ) const
{
	IFXRESULT iResult;
	iResult = m_pOctreeCollection->QueryInterface(IID_IFXCollection,(void **) &rpOutCollection);
	return iResult;
}


IFXRESULT CIFXOctreeCuller::QueryForSpatialSet(
	const IFXFrustum*  pFrustum,
	SPATIALINSTANCE_LIST *&  rpOpaque,
	SPATIALINSTANCE_LIST *&  rpTranslucent,
	F32&       ruOutNearClipPlane,
	F32&       ruOutFarClipPlane )
{
	ruOutNearClipPlane = 1;
	ruOutFarClipPlane =  1000000;
	m_pOpaqueSpatials = rpOpaque;
	m_pTranslucentSpatials = rpTranslucent;
	const IFXRay*  pInConvexPlanes = pFrustum->GetPlanes();
	const U32      uInNumberOfConvexPlanes = pFrustum->GetPlaneCount();

	U32 cnt=0;

	m_pOpaqueSpatials->clear();
	m_pTranslucentSpatials->clear();
	m_pParentLightInstancesList->clear();
	m_pChildLightList->clear();
	I32 x,y,z;
	for(cnt = 0; cnt < uInNumberOfConvexPlanes;++cnt )
	{
		x = pInConvexPlanes[cnt].direction.RawConst()[0] >0 ? 1:-1;
		y = pInConvexPlanes[cnt].direction.RawConst()[1] >0 ? 1:-1;
		z = pInConvexPlanes[cnt].direction.RawConst()[2] >0 ? 1:-1;
		m_pMinDir[cnt].Set((F32)x,(F32)y,(F32)z);
	}

	m_uDirLightCnt = m_pOctreeCollection->GetRootNode()->m_pDataBuffer[IFXSpatial::INFINITE_LIGHT].size();
	if(m_uDirLightCnt)
	{
		m_pDirLightInstancesList = &m_pOctreeCollection->GetRootNode()->m_pDataBuffer[IFXSpatial::INFINITE_LIGHT];
	}

	CullNode( m_pOctreeCollection->GetRootNode(),
		pInConvexPlanes, uInNumberOfConvexPlanes,
		SOME_CLIP );

	return IFX_OK;
}


void CIFXOctreeCuller::CullNode( const COctreeNode* q,
								const IFXRay* pFrustum,
								const U32 uInNumberOfPlanes,
								Visibility nodeVisibility )
{
	if (nodeVisibility == SOME_CLIP)
		nodeVisibility = CheckOctantAgainstFrustum(q->m_center, pFrustum,uInNumberOfPlanes,m_uNodeMask);
	if (nodeVisibility == NOT_VISIBLE)
		return;


	// Octant is Visible
	// Add Lights to parent list
	// Store Start Child Light list Index
	U32 uLightCnt = q->m_pDataBuffer[IFXSpatial::ATTENUATED_LIGHT].size();
	if(uLightCnt)
	{
		SPATIALINSTANCE_LIST& rLightList = q->m_pDataBuffer[IFXSpatial::ATTENUATED_LIGHT];
		// If Lights are present in this node add them to the parent List
		for(iter = 0; iter != uLightCnt;++iter)
			m_pParentLightInstancesList->push_back(rLightList[iter]);
	}

	U32 childListStartIndex = m_pChildLightList->size();

	// Render children.
	int i, j, k;
	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < 2; j++)
		{
			for (k = 0; k < 2; k++)
			{
				if (q->m_pChildren[i][j][k])
				{
					CullNode(q->m_pChildren[i][j][k], pFrustum, uInNumberOfPlanes, nodeVisibility);
				}
			}
		}

	}

	U32 childListEndIndex = m_pChildLightList->size();

	///@todo: find old commented code in Perforce archive and examine it 
	// Store End Child Light List Index
	// Light model association
	// Get all Parent lights  + child lights from start to end index.

	// Render objects in this node.`
	// Get Child Light List
	// Lights affecting node ll  = Parent + Child Light List

	// If octant is completely nodeVisibilityible then all the models contained by it are Visible too.


	U32  startOutSpatial = m_pOpaqueSpatials->size();
	BOOL bDoLightAssociations = FALSE;
	SPATIALINSTANCE_LIST* pOctantSpatialList = &q->m_pDataBuffer[IFXSpatial::OPAQUE_MODEL];
	U32 octantSpatialCnt =  q->m_pDataBuffer[IFXSpatial::OPAQUE_MODEL].size();


	if (nodeVisibility == NO_CLIP)
	{
		if(octantSpatialCnt)
		{
			m_pOpaqueSpatials->append(pOctantSpatialList);
			DoLightAssociations( m_pOpaqueSpatials,
								startOutSpatial, m_pOpaqueSpatials->size(),
								childListStartIndex, childListEndIndex );
		}

		octantSpatialCnt = q->m_pDataBuffer[IFXSpatial::TRANSLUCENT_MODEL].size();
		if(octantSpatialCnt)
		{
			pOctantSpatialList = &q->m_pDataBuffer[IFXSpatial::TRANSLUCENT_MODEL];
			startOutSpatial = m_pTranslucentSpatials->size();
			m_pTranslucentSpatials->append(pOctantSpatialList);
			DoLightAssociations( m_pTranslucentSpatials,
				startOutSpatial, m_pTranslucentSpatials->size(),
				childListStartIndex, childListEndIndex );
		}

	}
	else  // PARTIAL CLIP
	{
		IFXSpatial* pSpatial=0;
		Visibility spatialVisibility = NO_CLIP;
		float dist,radius;
		IFXVector4 bound;
		IFXVector3  spatialPos;

		// Set up for Pass 1 Opaques.
		SPATIALINSTANCE_LIST* pOutSpatialList = m_pOpaqueSpatials;

		U32 pass;
		for( pass =0;pass<2;++pass)
		{
			if(octantSpatialCnt)
			{
				U32 spIter;
				for(  spIter=0; spIter < octantSpatialCnt; ++spIter)
				{
					spatialVisibility = NO_CLIP;

					pSpatial = (*pOctantSpatialList)[spIter].m_pSpatial;

					pSpatial->GetSpatialBound(bound, (*pOctantSpatialList)[spIter].m_Instance);
					radius = bound.Radius();
					U32 plane;
					for( plane =0; plane< uInNumberOfPlanes;plane++ )
					{
						spatialPos.Subtract( bound,pFrustum[plane].position );
						dist = spatialPos.DotProduct( pFrustum[plane].direction );
						if (dist > radius)
						{
							// Object is completely on wrong side of this frustum plane.
							spatialVisibility = NOT_VISIBLE;
							break;
						}
					}

					if (spatialVisibility != NOT_VISIBLE)
					{
						IFXSpatialInstance spatialInstance;
						spatialInstance.m_pSpatial = pSpatial;
						spatialInstance.m_Instance = (*pOctantSpatialList)[spIter].m_Instance;
						pOutSpatialList->push_back( spatialInstance );
						bDoLightAssociations = TRUE;
					}
				}
				if(bDoLightAssociations) {
					DoLightAssociations( pOutSpatialList,
										startOutSpatial, pOutSpatialList->size(),
										childListStartIndex, childListEndIndex );
				}
			}
			// Set up pass 2 for Translucents
			octantSpatialCnt = q->m_pDataBuffer[IFXSpatial::TRANSLUCENT_MODEL].size();
			pOutSpatialList = m_pTranslucentSpatials;
			startOutSpatial = m_pTranslucentSpatials->size();
			bDoLightAssociations = FALSE;


		}
	}




	// Pop my lights off the parent list.
	// Push onto the child light list

	// If Lights are present in this node pop them off parent list
	// and add them to the child List
	uLightCnt =  q->m_pDataBuffer[IFXSpatial::ATTENUATED_LIGHT].size();
	IFXSpatialInstance LightInstances;
	for(iter = 0; iter != uLightCnt; ++iter)
	{
		LightInstances = m_pParentLightInstancesList->pop();
		m_pChildLightList->push_back(LightInstances);
	}


}
void CIFXOctreeCuller::DoLightAssociations( SPATIALINSTANCE_LIST *pSpatialInstances,
										   U32 uSpatialStart,U32 uSpatialEnd,
										   U32 uChildIndexStart, U32 uChildIndexEnd)


{

	// Memcpy child, parent lights to // check for intersection pAttenuatedlightList
	// Next, determine the sort key values

	IFXVector4 lightModelDistance,modelBound;
	DEFAULT_LESSER<F64> comparator;

	IFXSpatialAssociation *pSpatialAssoc =0;

	U32 lightCnt =0,modelCnt=0;
	F32 fLightExtent;

	U32 uParentLListCnt = m_pParentLightInstancesList->size();
	U32 uChildLListCnt = uChildIndexEnd - uChildIndexStart;

	U32 uAttenuatedLightCnt = uParentLListCnt+ uChildLListCnt;

	m_pAttenuatedLights->clear();
	m_pAttenuatedLights->append(m_pParentLightInstancesList);

	if(uChildLListCnt)
	{
		m_pAttenuatedLights->append(&((*m_pChildLightList)[uChildIndexStart]),uChildLListCnt);
	}

	// AttenuatedLightCnt > m_allocatedAttenuatedLights  realloc keys, attenuated light array
	// Compute Keys for Light Association
	// memcpy pattenuatedlightlist = m_pParentLightList+m_pChildLightList

	for(modelCnt = uSpatialStart; modelCnt < uSpatialEnd; modelCnt++)
	{
		// Grow Sort-key array if needed.
		if(m_uAllocatedSortKeys < uAttenuatedLightCnt)
		{
			m_uAllocatedSortKeys = (uAttenuatedLightCnt/DEFAULT_ALLOC_PAD +1)*DEFAULT_ALLOC_PAD;
			m_pSortKeys =   (F64 *)IFXReallocate(m_pSortKeys, sizeof(F64)*m_uAllocatedSortKeys);
		}

		(*pSpatialInstances)[modelCnt].m_pSpatial->GetSpatialBound(modelBound, (*pSpatialInstances)[modelCnt].m_Instance);

		// Compute model Light intersection
		for (lightCnt =0; lightCnt < uAttenuatedLightCnt; lightCnt++ )
		{
			(*m_pAttenuatedLights)[lightCnt].m_pSpatial->GetSpatialBound(lightModelDistance, (*m_pAttenuatedLights)[lightCnt].m_Instance);
			fLightExtent = lightModelDistance.Radius();
			lightModelDistance.Subtract(modelBound);
			m_pSortKeys[lightCnt] = sqrt(lightModelDistance.DotProduct3(lightModelDistance))
				- (F64)modelBound.Radius()
				- (F64)fLightExtent;
		}
		/**
			@todo Decide what to use as 2nd input parameter of SelectBestN function
		*/
		//SelectBestN(m_pSortKeys, m_pAttenuatedLights->getDataPtr(), comparator, uAttenuatedLightCnt, uAttenuatedLightCnt);
		SelectBestN(m_pSortKeys, m_pAttenuatedLights, comparator, uAttenuatedLightCnt, uAttenuatedLightCnt);
		(*pSpatialInstances)[modelCnt].m_pSpatial->QueryInterface(IID_IFXSpatialAssociation,(void **) &pSpatialAssoc);
		IFXASSERT(pSpatialAssoc);

		if(m_uDirLightCnt)
		{
			pSpatialAssoc->SetAssociatedSpatials(m_pDirLightInstancesList,m_uDirLightCnt);
			pSpatialAssoc->AppendAssociatedSpatials(m_pAttenuatedLights,uAttenuatedLightCnt);

		}
		else
			pSpatialAssoc->SetAssociatedSpatials(m_pAttenuatedLights, uAttenuatedLightCnt);

		IFXRELEASE(pSpatialAssoc);
	}
}



IFXINLINE Visibility  CIFXOctreeCuller::CheckOctantAgainstFrustum( const IFXVector4& inOctantBounds,
																  const IFXRay* pClipPlane,
																  const U32 numClipPlanes,
																  U32* nodeMask )
{
	F32 dist=0.0;
	IFXVector3 minV, maxV, plane, octantBounds(inOctantBounds.RawConst());
	const float radius = inOctantBounds.Radius();
	Visibility vis =  NO_CLIP;
	// We allocated an extra bytre to store the no of Clip planes
	U32  maskCnt = 0;
	U32 i;
	for(i =0;i < numClipPlanes;i++)
	{ // change to frustum panes of node mask
		// min pt of octree node from viewfrustumPlane
		plane = m_pMinDir[i];
		plane.Scale(radius);
		minV.Subtract(octantBounds,plane);
		minV.Subtract(pClipPlane[i].position);
		dist =  minV.DotProduct(pClipPlane[i].direction);
		if (dist >  0)
		{
			vis = NOT_VISIBLE;
			break;
		}
		maxV.Add(octantBounds,plane);
		maxV.Subtract(pClipPlane[i].position);
		dist =  maxV.DotProduct(pClipPlane[i].direction);
		if (dist > 0)
		{
			vis = SOME_CLIP;
			nodeMask[maskCnt] = i;
			maskCnt++;
		}
	}

	nodeMask[MAX_CLIP_PLANES] = maskCnt;

	return vis;

}


IFXINLINE Visibility CIFXOctreeCuller::CheckSpatialAgainstFrustum( IFXSpatial* pInSpatial,
																  U32 Instance,
																  const IFXRay * pClipPlane,
																  const U32 numClipPlanes,
																  const U32* nodeMask) const
																  // Returns the visibility of the object w/r/t the frustum.
{
	// The Node Mask sheme thoug reducing work load seems to
	// reduce performance probably due to lack of predictability for parallel computation

	Visibility viz = NO_CLIP;
	//  U32 i=0;

	float dist;
	IFXVector4 bound;
	IFXVector3  spatialPos;

	pInSpatial->GetSpatialBound(bound, Instance);
	const float radius = bound.Radius();
	// We dont' care right now about Partial_clip
	// U32 maskCnt = nodeMask[MAX_CLIP_PLANES];
	// while(maskCnt--)
	U32 plane;
	for( plane =0;plane<numClipPlanes;plane++)
	{
		//    spatialPos.Subtract(bound,pClipPlane[nodeMask[maskCnt]].position);
		//    dist = spatialPos.DotProduct(pClipPlane[nodeMask[maskCnt]].direction);
		spatialPos.Subtract(bound,pClipPlane[plane].position);
		dist = spatialPos.DotProduct(pClipPlane[plane].direction);
		if (dist > radius)
		{
			// Object is completely on wrong side of this frustum plane.
			return NOT_VISIBLE;
		}
		// We dont care if its partial or completly visible at this point.
		//if((dist < radius)&& (dist > -radius))
		//  viz = SOME_CLIP;
	}
	return viz;
}


// IFXUnknown

U32 CIFXOctreeCuller::AddRef(void)
{
	return ++m_uRefCount;
}


U32 CIFXOctreeCuller::Release(void)
{
	if (--m_uRefCount == 0)
	{
		delete this;
		return 0;
	}
	return m_uRefCount;
}


IFXRESULT CIFXOctreeCuller::QueryInterface( IFXREFIID interfaceId,
										   void**   ppInterface )
{
	IFXRESULT result = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXSpatialSetQuery )
			*ppInterface = ( IFXSpatialSetQuery* ) this;
		else if ( interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXUnknown* ) this;
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



IFXRESULT IFXAPI_CALLTYPE CIFXOctreeCuller_Factory( IFXREFIID interfaceId,
								   void**    ppInterface )
{
	IFXRESULT iResult = IFX_E_INVALID_POINTER;

	if ( ppInterface )
	{
		// It doesn't exist, so try to create it.
		CIFXOctreeCuller *pComponent = new CIFXOctreeCuller();

		if ( pComponent )
		{
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			iResult = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		}
		else
			iResult = IFX_E_OUT_OF_MEMORY;
	}

	return iResult;
}
