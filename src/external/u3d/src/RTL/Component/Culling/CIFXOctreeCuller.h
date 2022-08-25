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
#ifndef _CIFXOCTREE_CULLER_
#define _CIFXOCTREE_CULLER_

#include <math.h>

#include "IFXSpatial.h"
#include "IFXRay.h"
#include "IFXFrustum.h"
#include "IFXSpatialSetQuery.h"
#include "IFXSpatialAssociation.h"
#include "CIFXOctreeCollection.h"
#include "CSort.h"
#include "COctreeNode.h"

#define MAX_CLIP_PLANES 7
#define DEFAULT_ALLOC_PAD 8


enum Visibility { NO_CLIP = 0, SOME_CLIP, NOT_VISIBLE };


class CIFXOctreeCuller : public IFXSpatialSetQuery
{
	// For node store last rejecting VF plane and parent node Mask, 
	// use AABB intersection tests.
	// For models use nodeMask / last rejected face
	// If node 
	CIFXOctreeCollection* m_pOctreeCollection;
	IFXVector3 *m_pMinDir;
	U32 iter;
	SPATIALINSTANCE_LIST *m_pChildLightList;
	SPATIALINSTANCE_LIST *m_pParentLightInstancesList;
	U32 m_uRefCount;
	U32 m_uNodeMask[MAX_CLIP_PLANES+1];

	// Data to reduce alloc overhead in light associations
	SPATIALINSTANCE_LIST* m_pAttenuatedLights;
	SPATIALINSTANCE_LIST* m_pDirLightInstancesList;
	U32 m_uDirLightCnt;
	F64 * m_pSortKeys;
	U32 m_uAllocatedSortKeys;
	SPATIALINSTANCE_LIST* m_pOpaqueSpatials, *m_pTranslucentSpatials;
	U32 m_allocPad;

	BOOL m_bInitialized;
	
	CIFXOctreeCuller():m_pOctreeCollection(0),m_pMinDir(0),m_pChildLightList(0),m_pParentLightInstancesList(0),
					   m_uRefCount(0),m_pAttenuatedLights(0), m_pDirLightInstancesList(0),
					   m_pSortKeys(0), m_uAllocatedSortKeys(0),
					   m_pOpaqueSpatials(0), m_pTranslucentSpatials(0), m_allocPad(DEFAULT_ALLOC_PAD),
					   m_bInitialized(FALSE){};

	virtual ~CIFXOctreeCuller();

	
	IFXINLINE Visibility	CheckOctantAgainstFrustum(const IFXVector4& ,const IFXRay* ,
														const U32,U32* nodeMask );
	IFXINLINE Visibility	CheckSpatialAgainstFrustum(IFXSpatial* , U32 Instance, const IFXRay* ,
														const U32,const U32* nodeMask ) const;
	
	void CullNode(	const COctreeNode*		 q,
					const IFXRay*			 pFrustum,
					const U32				 uInNumberOfPlanes, 
					Visibility				 vis );


	IFXRESULT GetVisibleNodeList();

	void DoLightAssociations( SPATIALINSTANCE_LIST *pOpaque,
							  U32 uSpatialStart,   U32 uSpatialEnd, 
							  U32 uChildIndexStart,U32 uChildIndexEnd);
public :

	// Factory Function for Culler Creation
	friend IFXRESULT IFXAPI_CALLTYPE CIFXOctreeCuller_Factory(	IFXREFIID interfaceId, 
												void** ppInterface );

	void Initialize();
	
	
	// IFXUnknown
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (	IFXREFIID	interfaceId,
								void **ppv);	
	

	// IFXSpatialSetQuery
	IFXRESULT IFXAPI 	QueryForSpatialSet( 
											const IFXFrustum*	pFrustum, 
											SPATIALINSTANCE_LIST*&		rpOpaque,
											SPATIALINSTANCE_LIST*&		rpTranslucent,
											F32&				ruOutNearClipPlane,
											F32&				ruOutFarClipPlane );
	
	IFXRESULT IFXAPI 	SetCollection( IFXCollection* pInCollection ); 
	IFXRESULT IFXAPI 	GetCollection( IFXCollection*& rpOutCollection ) const; 
	const IFXCID& IFXAPI GetCollectionCID() const
		{return CID_IFXOctreeCollection;};
};

#endif
