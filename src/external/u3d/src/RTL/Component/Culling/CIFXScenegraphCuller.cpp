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
/**
@file  CIFXSceneGraphCuller.cpp

The implementation file of the CIFXSceneGraphCuller component.
*/

#include "CIFXScenegraphCuller.h"
#include "IFXSpatial.h"
#include "IFXRay.h"
#include "IFXSpatialSetQuery.h"
#include "IFXSpatialAssociation.h"
#include "IFXFrustum.h"
#include "IFXNode.h"
#include "IFXAutoRelease.h"
#include <float.h>


F64* CIFXSceneGraphCuller::m_pHugeSortKeys = NULL;
U32  CIFXSceneGraphCuller::m_uHugeSortKeys = 0;
U32  CIFXSceneGraphCuller::m_uHugeSortKeysAllocated = 0;


CIFXSceneGraphCuller::CIFXSceneGraphCuller()
{
	m_pCollection = NULL;
	m_uRefCount = 0;
}


CIFXSceneGraphCuller::~CIFXSceneGraphCuller()
{
	IFXRELEASE( m_pCollection );
	if (m_pHugeSortKeys)
		IFXDeallocate( m_pHugeSortKeys );
	m_pHugeSortKeys = NULL;
	m_uHugeSortKeysAllocated = 0;
}


IFXRESULT IFXAPI_CALLTYPE CIFXSceneGraphCuller_Factory( IFXREFIID interfaceId,
									   void**    ppInterface )
{
	IFXRESULT result;

	if ( ppInterface )
	{
		// It doesn't exist, so try to create it.
		CIFXSceneGraphCuller *pComponent = new CIFXSceneGraphCuller();

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


// IFXUnknown
U32 CIFXSceneGraphCuller::AddRef(void)
{
	return ++m_uRefCount;
}


U32 CIFXSceneGraphCuller::Release(void)
{
	if( 1 == m_uRefCount )
	{
		delete this;
		return 0;
	}
	else
		return --m_uRefCount;
}


IFXRESULT CIFXSceneGraphCuller::QueryInterface( IFXREFIID interfaceId,
											   void**    ppInterface )
{
	IFXRESULT result = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXUnknown* ) this;
		else if ( interfaceId == IID_IFXSpatialSetQuery )
			*ppInterface = ( IFXSpatialSetQuery* ) this;
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


// IFXSpatialSetQuery
static inline void Swap( IFXSpatial** pEntries, F64* pKeys, U32 i, U32 j )
{
	IFXSpatial* pTemp = pEntries[i];
	F64 fTemp = pKeys[i];
	pEntries[i] = pEntries[j];
	pKeys[i] = pKeys[j];
	pEntries[j] = pTemp;
	pKeys[j] = fTemp;
}


static void Sort( IFXSpatial** pEntries, F64* pKeys, U32 left, U32 right )
{
	if (( left < right ) && ( right != (U32)-1) ) 
	{
		Swap( pEntries, pKeys, left, (left + right)>>1 );
		U32 i, last = left;
		for (i=left+1; i<=right; i++)
			if ( pKeys[i] < pKeys[left] )
				Swap( pEntries, pKeys, ++last, i );
		Swap( pEntries, pKeys, left, last );
		Sort( pEntries, pKeys, left, last-1 );
		Sort( pEntries, pKeys, last+1, right );
	}
}

static inline void Swap( IFXSpatial**& pEntries,
						F32* pKeys, U32 i, U32 j )
{
	IFXSpatial* temp;
	temp = pEntries[i];

	pEntries[i] = pEntries[j];
	pEntries[j] = temp;
	F32 fTemp = pKeys[i];
	pKeys[i] = pKeys[j];
	pKeys[j] = fTemp;
}


static void Sort( IFXSpatial**& pEntries,
				 F32* pKeys, U32 left, U32 right )
{
	if (( left < right ) && ( right != (U32)-1) )
	{
		Swap( pEntries, pKeys, left, (left + right)>>1 );
		U32 i, last = left;
		for (i=left+1; i<=right; i++)
			if ( pKeys[i] < pKeys[left] )
				Swap( pEntries, pKeys, ++last, i );
		Swap( pEntries, pKeys, left, last );
		Sort( pEntries, pKeys, left, last-1 );
		Sort( pEntries, pKeys, last+1, right );
	}
}

static void Sort( SPATIALINSTANCE_LIST* pEntries, F32* pKeys, U32 left, U32 right )
{
	if (( left < right ) && ( right != (U32)-1) ) 
	{
		pEntries->swap( pKeys, left, (left + right)>>1 );
		U32 i, last = left;
		for (i=left+1; i<=right; i++)
			if ( pKeys[i] < pKeys[left] )
				pEntries->swap( pKeys, ++last, i );
		pEntries->swap( pKeys, left, last );
		Sort( pEntries, pKeys, left, last-1 );
		Sort( pEntries, pKeys, last+1, right );
	}
}

// IFXSpatialSetQuery

static void Sort( SPATIALINSTANCE_LIST* pEntries, F64* pKeys, U32 left, U32 right )
{
	if (( left < right ) && ( right != (U32)-1) )
	{
		pEntries->swap( pKeys, left, (left + right)>>1 );
		U32 i, last = left;
		for (i=left+1; i<=right; i++)
			if ( pKeys[i] < pKeys[left] )
				pEntries->swap( pKeys, ++last, i );
		pEntries->swap( pKeys, left, last );
		Sort( pEntries, pKeys, left, last-1 );
		Sort( pEntries, pKeys, last+1, right );
	}
}

IFXRESULT CIFXSceneGraphCuller::QueryForSpatialSet(
	const IFXFrustum* pInFrustum,
	SPATIALINSTANCE_LIST *&   rpOutOpaques,
	SPATIALINSTANCE_LIST *&   rpOutTranslucents,
	F32&              rfOutNearClipPlane,
	F32&              rfOutFarClipPlane )
{
	IFXRESULT     result = IFX_OK;
	IFXSpatial**  pSpatials;
	U32           nSpatials;
	U32           spatial;
	IFXVector4    Bound,dist;
	U32           visibilityBits;
	U32           intersectionBits;
	U32           uLights;
	const IFXRay* pInConvexPlanes = pInFrustum->GetPlanes();
	const U32     uInNumberOfConvexPlanes = pInFrustum->GetPlaneCount();
	F32           fOutNearClipPlane = FLT_MAX;
	F32           fOutFarClipPlane = -FLT_MAX;
	F32           nearClip,farClip;
	U32 instance;

	if ( !m_pCollection )
	{
		return IFX_E_NOT_INITIALIZED;
	}

	// Get the infinite lights(pre-sorted by intensity)
	SPATIALINSTANCE_LIST InfiniteLightInstances, AttunuatedLightInstances;
	IFXSpatial** pInfiniteLights;
	U32          i, j, uInfiniteLights, uInfiniteLightSpatials, numInstances;
	result = m_pCollection->GetSpatials( pInfiniteLights,
		uInfiniteLights,
		IFXSpatial::INFINITE_LIGHT );
	
	InfiniteLightInstances.clear();
	IFXSpatialInstance spatialLightInstance;
	uInfiniteLightSpatials = 0;
	for (i = 0; i < uInfiniteLights && IFXSUCCESS(result); i++) 
	{
		IFXDECLARELOCAL(IFXNode, pSpatialNode);
		result = pInfiniteLights[i]->QueryInterface(IID_IFXNode, (void**)&pSpatialNode);
		if (IFXSUCCESS(result)) 
			result = pSpatialNode->GetNumberOfInstances(&numInstances);
		if (IFXSUCCESS(result)) 
		{
			spatialLightInstance.m_pSpatial = pInfiniteLights[i];
			for (j = 0; j < numInstances; j++) 
			{
				spatialLightInstance.m_Instance = j;
				InfiniteLightInstances.push_back(spatialLightInstance);
				uInfiniteLightSpatials++;
			}
		}
	}

	// Get the attenuated lights
	IFXSpatial** pAttenuatedLights;
	U32          uAttenuatedLights;
	U32 spatialNodeInstances = 0;
	m_pCollection->GetSpatials( pAttenuatedLights,
		uAttenuatedLights,
		IFXSpatial::ATTENUATED_LIGHT );

	uLights = uInfiniteLights + uAttenuatedLights;

	U32 bOpaque;
	// First process the opaque models, and then translucent models
	for ( bOpaque = 2; bOpaque--; )
	{
		SPATIALINSTANCE_LIST * pVisibleSpatials;
		if ( bOpaque )
		{
			m_pCollection->GetSpatials( pSpatials,
				nSpatials,
				IFXSpatial::OPAQUE_MODEL );
			pVisibleSpatials = rpOutOpaques;
		}
		else // translucents
		{
			m_pCollection->GetSpatials( pSpatials,
				nSpatials,
				IFXSpatial::TRANSLUCENT_MODEL );
			pVisibleSpatials = rpOutTranslucents;
		}

		// Determine visibility and light associations
		nearClip=0;
		farClip=0;

		IFXVector4 clipPlanePosition, clipPlaneNormal;
		for ( spatial = 0; spatial < nSpatials; spatial++ )
		{
			IFXDECLARELOCAL(IFXNode, pSpatialNode);
			result = pSpatials[spatial]->QueryInterface(IID_IFXNode, (void**)&pSpatialNode);
			if (IFXSUCCESS(result))
				 result = pSpatialNode->GetNumberOfInstances(&spatialNodeInstances);

			for (instance = 0; instance < spatialNodeInstances && IFXSUCCESS(result); instance++) 
			{
				result = pSpatials[spatial]->GetSpatialBound( Bound, instance );
				IFXASSERT(IFXSUCCESS(result));

				if (IFXSUCCESS(result)) 
				{
					visibilityBits = 0;
					intersectionBits = 0;
					U32 plane;
					for (  plane = 0 ; plane < uInNumberOfConvexPlanes; plane++)
					{
						clipPlaneNormal = pInConvexPlanes[plane].GetDirection();
						clipPlanePosition = pInConvexPlanes[plane].GetPosition();
						dist.Subtract(Bound,clipPlanePosition);
						F32 r = dist.DotProduct3(clipPlaneNormal);
						if (r<Bound.Radius())
						{
							U32 inside = (r<(-Bound.Radius()));
							visibilityBits |= (inside<<plane); // fully visible
							intersectionBits |= (!inside<<plane); // partially visible
						}
						else
						{
							// Not visible even if rejected by one of the  frustum planes
							visibilityBits =0;
							intersectionBits = 0;
							break;
						}
					}
					//IFXTRACE_DEBUG( "VisibilityBits:%x \t IntersectionBits:%x\n", visibilityBits, intersectionBits );

					if ( visibilityBits | intersectionBits )
					{
						// Compute Optimal near & far Clip planes
						// The -ve sign is due to the direction of the clip plane normal.
						// clipplane 0 normal is  <-| but we want |->
						dist.Subtract(pInConvexPlanes[IFX_FRUSTUM_FRONT].position,Bound);
						nearClip = dist.DotProduct3(pInConvexPlanes[IFX_FRUSTUM_FRONT].direction) - Bound.Radius();
						if (fOutNearClipPlane > nearClip)
							fOutNearClipPlane = nearClip < 0 ? 0:nearClip;

						dist.Subtract(pInConvexPlanes[IFX_FRUSTUM_FRONT].position,Bound);
						farClip = dist.DotProduct3(pInConvexPlanes[IFX_FRUSTUM_FRONT].direction) + Bound.Radius();
						if (fOutFarClipPlane < farClip)
							fOutFarClipPlane = farClip;

						// Add the model to the visible set
						IFXSpatialInstance spatialInstance;
						spatialInstance.m_pSpatial = pSpatials[spatial];
						spatialInstance.m_Instance = instance;
						pVisibleSpatials->push_back(spatialInstance);

						U32 AttLightInstances = uAttenuatedLights;
						AttunuatedLightInstances.clear();
						// Build light associations
						if ( pAttenuatedLights )
						{
							U32 i, 
								light, lightInst = 0, 
								numInstances = 0;
							for (i = 0; i < uAttenuatedLights && IFXSUCCESS(result); i++) 
							{
								IFXDECLARELOCAL(IFXNode, pSpatialNode);
								result = pAttenuatedLights[i]->QueryInterface(IID_IFXNode, (void**)&pSpatialNode);

								if (IFXSUCCESS(result))
									result = pSpatialNode->GetNumberOfInstances(&numInstances);

								if (IFXSUCCESS(result))
									AttLightInstances += numInstances-1;
							}
							// First, ensure that we have allocated enough sort keys
							if ( m_uHugeSortKeysAllocated < AttLightInstances && IFXSUCCESS(result))
							{
								IFXDeallocate( m_pHugeSortKeys );
								m_pHugeSortKeys = (F64*)IFXAllocate( AttLightInstances * sizeof(F64) );
								m_uHugeSortKeysAllocated = AttLightInstances;
							}

							// Next, determine the sort key values
							if (IFXSUCCESS(result))
							{
								IFXVector4 distance;
								for ( light = uAttenuatedLights; light--; ) 
								{
									IFXDECLARELOCAL(IFXNode, pSpatialNode);
									result = pAttenuatedLights[light]->QueryInterface(IID_IFXNode, (void**)&pSpatialNode);

									U32 numInstances = 0;

									if (IFXSUCCESS(result))
										result = pSpatialNode->GetNumberOfInstances(&numInstances);

									for (i = 0; i < numInstances && IFXSUCCESS(result); i++ ) 
									{
										result = pAttenuatedLights[light]->GetSpatialBound(distance, i);
										if (IFXSUCCESS(result)) 
										{
											distance.X() -= Bound.X();
											distance.Y() -= Bound.Y();
											distance.Z() -= Bound.Z();

											m_pHugeSortKeys[lightInst] = -Bound.Radius() - distance.Radius();
											m_pHugeSortKeys[lightInst] += sqrt(distance.DotProduct3(distance));

											spatialLightInstance.m_pSpatial = pAttenuatedLights[light];
											spatialLightInstance.m_Instance = i;
											AttunuatedLightInstances.push_back(spatialLightInstance);
											lightInst++;
										}
									}
									if (IFXFAILURE(result))
										break;
								}
								IFXASSERT(AttLightInstances == lightInst);
							}

							/** @todo: 
							Cull lights who's extent doesn't reach the model's bound.
							Limit the number of lights that get associated(sorted) to model.lightCount 
							Order the attenuated lights with respect to the model
							*/

							if (IFXSUCCESS(result))
								Sort( &AttunuatedLightInstances, m_pHugeSortKeys, 0, AttLightInstances-1 );
						}

						if (IFXSUCCESS(result)) 
						{
							// Copy the lightSet as the model's secondary spatials
							IFXSpatialAssociation * pSA = NULL;
							pSpatials[spatial]->QueryInterface(IID_IFXSpatialAssociation,(void**) &pSA);

							pSA->SetAssociatedSpatials(&InfiniteLightInstances,uInfiniteLightSpatials);
							pSA->AppendAssociatedSpatials(&AttunuatedLightInstances,AttLightInstances);

							IFXRELEASE( pSA );
						}
					}
				}
			}
		}

		// Order and return visible spatials
		/// @todo: Investigate if we need to do something special for Opaque, Translucent models
	}

	if ( IFXSUCCESS(result) )
	{
		rfOutNearClipPlane = fOutNearClipPlane;
		rfOutFarClipPlane = fOutFarClipPlane;
	}

	return result;
}


IFXRESULT CIFXSceneGraphCuller::SetCollection( IFXCollection* pInCollection )
{
	IFXRELEASE(m_pCollection);
	m_pCollection = pInCollection;
	if (m_pCollection)
		m_pCollection->AddRef();
	return IFX_OK;
}


IFXRESULT CIFXSceneGraphCuller::GetCollection( IFXCollection*& rpOutCollection ) const
{
	rpOutCollection = m_pCollection;
	if (m_pCollection)
		m_pCollection->AddRef();
	return IFX_OK;
}
