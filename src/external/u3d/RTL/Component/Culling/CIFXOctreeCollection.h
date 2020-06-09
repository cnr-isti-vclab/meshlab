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
#ifndef _CIFXOCTREE_COLLECTION_H_
#define _CIFXOCTREE_COLLECTION_H_

#include "IFXVector4.h"
#include "IFXCollection.h"
#include "IFXRenderingCIDs.h"

class COctreeNode;
  
class CIFXOctreeCollection : public IFXCollection
{
private :
protected :

	// Max World size that is being partitioned by the tree
	IFXVector4 m_worldBound;
		// needs to be Computed depending on Scene data distribution
	U32 m_minDepth;
		// needs to be Computed depending on Scene data distribution
		// the max upper limit specified by user to limit recursion 
	U32 m_maxDepth;
	// To adjust the Looseness i.e overlap of the nodes.
	F32 m_looseK;
	U32 m_uRefCount;
	// directional Ligths are not spatials bcos they are infinite 
	// so we will store these at the root node,
	COctreeNode * m_pRootNode;
	U32 m_overflowCount;
	IFXRESULT InCorporateSpatialBound(  IFXSpatial**    pInSpatials,   
										U32             uInNumberOfSpatials,
										IFXVector4&		rSphere );

	BOOL FitsInBox( const IFXVector4 & inSpatialBound, 
					F32 cx, F32 cy, F32 cz, 
					F32 octHalfSize) const;

	BOOL FitsInBox( IFXSpatial*  o, 
					U32 Instance,
					const IFXVector4& c ) const;

	BOOL FitsInBox( const IFXVector4& in, 
					const IFXVector4& c ) const;
	
	U32 InsertRecursivelyIntoBestFitOctreeNode(	COctreeNode* q, 
												IFXSpatial* spatial, U32 Instance);

	void ResursivelyAddSpatialsToList( COctreeNode *pCurrNode,     IFXSpatial**& rpOutSpatials, 
									   U32& ruOutNumberOfSpatials, U32&			 ruListStart, 
									   IFXSpatial::eType eIntype        );

	CIFXOctreeCollection(): m_maxDepth(0),m_looseK(0),
							m_uRefCount(0),m_pRootNode(0),DepthTotals(0){}

	virtual ~CIFXOctreeCollection();
	void CleanUp();
public :
// Statistical / Debug  info
	U32 * DepthTotals;

	IFXRESULT Initialize(IFXVector4 worldBound = IFXVector4(0,0,0,1024.0),F32 k = 1, U32 maxD = 4);
	
	COctreeNode* GetRootNode()  const{return m_pRootNode;}
	F32	 GetWorldSize() const{return m_worldBound.Radius()*2;}
	U32  GetMaxDepth () const{return m_maxDepth;}
	F32	 GetLooseK()    const{return m_looseK;}
		
	U32	Insert(IFXSpatial* o, U32 Instance);
	U32 Insert(COctreeNode* q, IFXSpatial* pInSpatial, U32 Instance);
	
	void TraverseNodes(COctreeNode *on,void (*nf)(COctreeNode * on));
	void TraverseNodes(COctreeNode *on,void (*nf)(COctreeNode * on,void * data));

	void TraverseNodes(COctreeNode *on,void (*pre)(COctreeNode * on),void (*post)(COctreeNode * on));
	void TraverseTree(void (*nf)(COctreeNode * on)){ TraverseNodes(m_pRootNode, nf);}

	// IFXUnknown
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface ( IFXREFIID	interfaceId,void **ppv);
	
	
	// Factory Function for Octree Creation
	friend IFXRESULT IFXAPI_CALLTYPE CIFXOctreeCollection_Factory(IFXREFIID interfaceId, void** ppinterface);
	
	
	
	// Collection Functionality
	IFXRESULT IFXAPI 	InitializeCollection( IFXSpatial**      pInSpatials,
									U32               uInNumberOfSpatials,
		                            IFXSpatial::eType eIntype );
	IFXRESULT IFXAPI 	InitializeCollection( IFXCollection*      pInCollection );
	IFXRESULT IFXAPI 	AddSpatials(			IFXSpatial**       pInSpatials,   
										U32               uInNumberOfSpatials,
										IFXSpatial::eType eIntype );
	IFXRESULT IFXAPI 	RemoveSpatials(		IFXSpatial**      pInSpatials,   
										U32               uInNumberOfSpatials,
										IFXSpatial::eType eIntype );
	IFXRESULT IFXAPI 	GetSpatials(			IFXSpatial**&     rpOutSpatials, 
										U32&			  ruOutNumberOfSpatials,
										IFXSpatial::eType eIntype );
	const IFXGUID& GetCID() { return CID_IFXOctreeCollection; }

};

#endif
