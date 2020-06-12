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
#include "COctreeNode.h"
#include "IFXRenderingCIDs.h"
#include "CIFXOctreeCollection.h"
#define MAX_LIGHT_RADIUS 100000

#include "IFXNode.h"
#include "IFXAutoRelease.h"

CIFXOctreeCollection::~CIFXOctreeCollection()
{
	CleanUp();
}

void CIFXOctreeCollection::CleanUp()
{
	IFXDELETE(m_pRootNode);
	IFXDELETE_ARRAY(DepthTotals);
}

IFXRESULT CIFXOctreeCollection::Initialize(
	IFXVector4 worldBound,
	F32 k,
	U32 maxD )
{
	m_looseK    = k;
	m_worldBound = worldBound;
	//  pos.Radius()*2;
	m_maxDepth  = maxD;
	m_overflowCount = 0;

	U32 i =0;
	m_pRootNode = new COctreeNode(this,0,0, worldBound);
	DepthTotals = new U32[m_maxDepth];
	for(i=0;i<m_maxDepth;i++)
		DepthTotals[i] =0;

	return IFX_OK;
}

BOOL CIFXOctreeCollection::FitsInBox(
									 const IFXVector4 & inSpatialBound,
									 F32 cx,
									 F32 cy,
									 F32 cz,
									 F32 octHalfSize) const
									 // Tests whether the given object can fit in the box centered at (cx,cy cz),
									 // with side dimensions of octtant HalfSize * 2.
{
	BOOL result = true;
	if (inSpatialBound.X() - inSpatialBound.Radius() < cx - octHalfSize ||
		inSpatialBound.X() + inSpatialBound.Radius() > cx + octHalfSize ||
		inSpatialBound.Y() - inSpatialBound.Radius() < cy - octHalfSize ||
		inSpatialBound.Y() + inSpatialBound.Radius() > cy + octHalfSize ||
		inSpatialBound.Z() - inSpatialBound.Radius() < cz - octHalfSize ||
		inSpatialBound.Z() + inSpatialBound.Radius() > cz + octHalfSize)
	{
		result = false;
	}
	return result;
}



BOOL CIFXOctreeCollection::FitsInBox(
									 IFXSpatial* spatial,
									 U32 Instance,
									 const IFXVector4& c ) const
									 // Tests whether the given object can fit in the box centered at (cx,cy cz),
									 // with side dimensions of octtant HalfSize * 2.
{
	BOOL result = TRUE;
	IFXVector4 inSpatialBound;

	spatial->GetSpatialBound(inSpatialBound, Instance);
	if (inSpatialBound.X() - inSpatialBound.Radius() < c.X() - c.Radius() ||
		inSpatialBound.X() + inSpatialBound.Radius() > c.X() + c.Radius() ||
		inSpatialBound.Y() - inSpatialBound.Radius() < c.Y() - c.Radius() ||
		inSpatialBound.Y() + inSpatialBound.Radius() > c.Y() + c.Radius() ||
		inSpatialBound.Z() - inSpatialBound.Radius() < c.Z() - c.Radius() ||
		inSpatialBound.Z() + inSpatialBound.Radius() > c.Z() + c.Radius() )
	{
		result = FALSE;
	}
	return result;
}

BOOL CIFXOctreeCollection::FitsInBox(
									 const IFXVector4& inSpatialBound,
									 const IFXVector4& c ) const
									 // Tests whether the given object can fit in the box centered at (cx,cy cz),
									 // with side dimensions of octtant HalfSize * 2.
{
	BOOL result = TRUE;
	if (inSpatialBound.X() - inSpatialBound.Radius() < c.X() - c.Radius() ||
		inSpatialBound.X() + inSpatialBound.Radius() > c.X() + c.Radius() ||
		inSpatialBound.Y() - inSpatialBound.Radius() < c.Y() - c.Radius() ||
		inSpatialBound.Y() + inSpatialBound.Radius() > c.Y() + c.Radius() ||
		inSpatialBound.Z() - inSpatialBound.Radius() < c.Z() - c.Radius() ||
		inSpatialBound.Z() + inSpatialBound.Radius() > c.Z() + c.Radius() )
	{
		result = FALSE;
	}
	return result;
}

// Insert the given object into the tree given by octNode.
// Returns the depth of the node the object was placed in.

U32 CIFXOctreeCollection::InsertRecursivelyIntoBestFitOctreeNode(
	COctreeNode* pOctreeNode,
	IFXSpatial*  pInSpatial, U32 Instance )
{

	/* another Possible Optimization is compute the depth level
	Refer function RemoveSpatial()*/

	// Indices to one of the 8 children
	U32 i,j,k,depth;
	// Center of Octant
	F32 cx,cy,cz;

	IFXVector4 inSpatialBound;
	pInSpatial->GetSpatialBound(inSpatialBound, Instance);

	depth = pOctreeNode->GetNodeDepth();
	// Check child nodes to see if object fits in one of them.
	if (depth + 1 < m_maxDepth)
	{

		F32 childRelativeCenterOffset = m_worldBound.Radius()*2 / (4 << depth);
		F32 childOctantRadius = m_looseK * childRelativeCenterOffset;

		// Pick child octant based on object's center point.
		i = (inSpatialBound.X() <= pOctreeNode->m_center.X()) ? 0 : 1;
		j = (inSpatialBound.Y() <= pOctreeNode->m_center.Y()) ? 0 : 1;
		k = (inSpatialBound.Z() <= pOctreeNode->m_center.Z()) ? 0 : 1;

		cx = pOctreeNode->m_center.X() + ((i == 0) ? -childRelativeCenterOffset : childRelativeCenterOffset);
		cy = pOctreeNode->m_center.Y() + ((j == 0) ? -childRelativeCenterOffset : childRelativeCenterOffset);
		cz = pOctreeNode->m_center.Z() + ((k == 0) ? -childRelativeCenterOffset : childRelativeCenterOffset);

		if (FitsInBox(inSpatialBound,cx, cy, cz, childOctantRadius))
		{
			if (pOctreeNode->m_pChildren[i][j][k] == 0)
			{
				pOctreeNode->m_pChildren[i][j][k] = new COctreeNode(this,pOctreeNode,pOctreeNode->GetNodeDepth()+1,cx, cy, cz,childOctantRadius);
			}
			// Keep recursing till the box fits.
			// Tail Recursion need to replace by iteration.
			return InsertRecursivelyIntoBestFitOctreeNode(pOctreeNode->m_pChildren[i][j][k], pInSpatial, Instance);
		}
	}

	pOctreeNode->InsertObjectInNode(pInSpatial, pInSpatial->GetSpatialType(), Instance);
	DepthTotals[depth]++;

	return depth;


}
IFXINLINE U32 CIFXOctreeCollection::Insert( IFXSpatial* pInSpatial, U32 Instance )
{
	return Insert(m_pRootNode, pInSpatial, Instance);
}


U32 CIFXOctreeCollection::Insert( COctreeNode* pOctreeNode,
								 IFXSpatial* pInSpatial, U32 Instance)
{
	U32 depthOfInsertedSpatial =0;
	IFXVector4 objectBoundingSphere;
	pInSpatial->GetSpatialBound(objectBoundingSphere, Instance);

	if(FitsInBox(objectBoundingSphere,pOctreeNode->m_center))
	{
		depthOfInsertedSpatial = InsertRecursivelyIntoBestFitOctreeNode(pOctreeNode,pInSpatial, Instance);
	}
	else
	{
		// Check If Object fits in the root

		if(FitsInBox(objectBoundingSphere,m_pRootNode->m_center))
		{
			depthOfInsertedSpatial = InsertRecursivelyIntoBestFitOctreeNode(pOctreeNode->m_pParent, pInSpatial, Instance);
		}
		else
		{
			m_overflowCount++;
			depthOfInsertedSpatial =0;
			m_pRootNode->InsertObjectInNode(pInSpatial, pInSpatial->GetSpatialType(), Instance);
		}
	}


	return depthOfInsertedSpatial;
}

void CIFXOctreeCollection::TraverseNodes(
	COctreeNode * on,
	void (*fpNodeFunc)(COctreeNode * on))
{
	I32 x,y,z;
	if(on){
		// apply f() to the node
		fpNodeFunc(on);
		for ( x = 0; x < MAX_CHILD_X; x++)
			for ( y = 0; y < MAX_CHILD_Y; y++)
				for ( z = 0; z < MAX_CHILD_Z; z++)
					if(on->m_pChildren[x][y][z])
						TraverseNodes(on->m_pChildren[x][y][z],fpNodeFunc);
	}
	return;
}
void CIFXOctreeCollection::TraverseNodes(
	COctreeNode* on,
	void (*fpNodePreFunc)(COctreeNode * on),
	void (*fpNodePostFunc)(COctreeNode * on))
{
	I32 x,y,z;
	if(on){
		// apply f() to the node
		fpNodePreFunc(on);
		for ( x = 0; x < MAX_CHILD_X; x++)
			for ( y = 0; y < MAX_CHILD_Y; y++)
				for ( z = 0; z < MAX_CHILD_Z; z++)
					if(on->m_pChildren[x][y][z])
						TraverseNodes(on->m_pChildren[x][y][z],fpNodePreFunc, fpNodePostFunc);
		fpNodePostFunc(on);

	}
	return;
}

IFXRESULT CIFXOctreeCollection::InCorporateSpatialBound( IFXSpatial** pInSpatials,   
								  						 U32          spatialCnt,
														 IFXVector4& rSphere )
{
	IFXRESULT	 result = IFX_OK;
	IFXVector4  spatialBound;

	while(IFXSUCCESS(result) && spatialCnt--)
	{
		IFXDECLARELOCAL(IFXNode, pSpatialNode);
		result = pInSpatials[spatialCnt]->QueryInterface(IID_IFXNode, (void**)&pSpatialNode);

		if (IFXSUCCESS(result)) {
			U32 i, numInst;
			result  = pSpatialNode->GetNumberOfInstances(&numInst);
			for (i = 0; i < numInst && IFXSUCCESS(result); i++) {
				// Hack need to fix as the collection type is not right.
				result = pInSpatials[spatialCnt]->GetSpatialBound( spatialBound, i );
				if(IFXSUCCESS(result) && spatialBound.Radius() < MAX_LIGHT_RADIUS )
					rSphere.IncorporateSphere( &spatialBound );
			}
		}

	}
	return result;
}


// IFXCollection
IFXRESULT CIFXOctreeCollection::InitializeCollection(
	IFXSpatial**      pInSpatials,
	U32               uInNumberOfSpatials,
	IFXSpatial::eType eIntype )
{
	// To be called only after an Initialize
	CleanUp();
	Initialize();
	return AddSpatials( pInSpatials, uInNumberOfSpatials, eIntype );
}
IFXRESULT CIFXOctreeCollection::InitializeCollection(
	IFXCollection* pInCollection)
{
	CleanUp();
	// compute world extents as needed by octree.
	IFXSpatial ** pSpatials=0;
	U32 uSpatialCnt =0,i=0;
	IFXVector4 worldBound;
	IFXRESULT iResult = IFX_OK;
	U32 uSpatialTypeCnt = IFXSpatial::TYPE_COUNT;
	for(i =0;i<uSpatialTypeCnt&&IFXSUCCESS(iResult);i++)
	{
		if(( i==IFXSpatial::OPAQUE_MODEL )
			|| ( i==IFXSpatial::ATTENUATED_LIGHT )
			|| ( i==IFXSpatial::TRANSLUCENT_MODEL ))

		{
			// GetSpatials returns a ptr to the array of spatials in SimpleCollection
			iResult = pInCollection->GetSpatials(pSpatials,uSpatialCnt,IFXSpatial::eType(i));
			if(uSpatialCnt&&IFXSUCCESS(iResult))
			{
				iResult = InCorporateSpatialBound(pSpatials, uSpatialCnt, worldBound);
			}
		}
	}
	//worldBound.Radius() *=2;
	Initialize(worldBound,1,8);
	for(i =0;i<uSpatialTypeCnt&&IFXSUCCESS(iResult);i++)
	{
		iResult = pInCollection->GetSpatials(pSpatials,uSpatialCnt,IFXSpatial::eType(i));
		if(uSpatialCnt&&IFXSUCCESS(iResult))
		{

			iResult = AddSpatials( pSpatials, uSpatialCnt, IFXSpatial::eType(i));
		}
	}
	return iResult;
}


IFXRESULT CIFXOctreeCollection::AddSpatials(
	IFXSpatial**      pInSpatials,
	U32               uInNumberOfSpatials,
	IFXSpatial::eType eIntype )
{
	// To be called only after Initialize()
	IFXRESULT result = IFX_OK;

	U32 i=0, j, numInst;
	IFXSpatial::eType type;
	for(i=0;i<uInNumberOfSpatials;i++)
	{
		IFXDECLARELOCAL(IFXNode, pSpatialNode);
		result = pInSpatials[i]->QueryInterface(IID_IFXNode, (void**)&pSpatialNode);
		if (IFXSUCCESS(result)) {
			type = pInSpatials[i]->GetSpatialType();
			result = pSpatialNode->GetNumberOfInstances(&numInst);
			for (j = 0; j < numInst && IFXSUCCESS(result); j++) {
				if(type== IFXSpatial::INFINITE_LIGHT)
					m_pRootNode->InsertObjectInNode(pInSpatials[i],IFXSpatial::INFINITE_LIGHT, j);
				else if(( type==IFXSpatial::OPAQUE_MODEL ) // As other spatial types have invalid bounds
					|| ( type==IFXSpatial::ATTENUATED_LIGHT )
					|| ( type==IFXSpatial::TRANSLUCENT_MODEL ))
					Insert(m_pRootNode,pInSpatials[i], j);
			}
		} else
			break;
	}

	return result;
}

IFXRESULT CIFXOctreeCollection::RemoveSpatials(
	IFXSpatial**    pInSpatials,
	U32         uInNumberOfSpatials,
	IFXSpatial::eType eIntype )
{

	// Models & Lights are being observed by the Octree Node
	// and thus at shutdown they will get a notification and will
	//  be able to delete themselves more efficiently


	// Use predictive logic of Octree
	IFXRESULT result = IFX_OK;
	U32 i;

	///@todo: try to improve it
	for (i = 0; i < uInNumberOfSpatials; i++) {
		result = m_pRootNode->RemoveObjectFromNode(pInSpatials[i], -1, eIntype);
	}

#if 0 ///@todo: investigate this commented code
	U32 num_models = 0;
	int i;
	for( i = 0;i<uInNumberOfSpatials;++i)
		if( pInSpatials[i]->GetSpatialType()==IFXSpatial::OPAQUE_MODEL )
			num_models++;

	if((pInSpatials&&uInNumberOfSpatials > 0)&&(eIntype != IFXSpatial::VIEW)&&(eIntype != IFXSpatial::GROUP))
	{


		result = IFX_E_CANNOT_FIND;
		F64 tmpConst = (m_looseK*m_worldSize)/4;
		F64 log2 = log10(2.0);
		U32 i,j,k,cnt,depth =0;
		IFXVector4 spatialBound;
		COctreeNode * pCurrNode = m_pRootNode, *pPrevNode=0;
		for(cnt=0;cnt<uInNumberOfSpatials;cnt++)
		{
			if(pInSpatials[cnt]->GetSpatialType()== IFXSpatial::INFINITE_LIGHT)
				result = m_pRootNode->RemoveObjectFromNode(pInSpatials[cnt],IFXSpatial::INFINITE_LIGHT);
			else
			{
				pInSpatials[cnt]->GetSpatialBound(spatialBound);
				depth = (U32)floor(log10(tmpConst/spatialBound.Radius())/log2);
				depth = depth > m_maxDepth? m_maxDepth : depth;
				// the depth calclated above is not the tightest posible fit for the spatial
				while((depth>0)&&pCurrNode)
				{
					// Pick child octant based on object's center point.
					i = (spatialBound.X() <= pCurrNode->m_center.X()) ? 0 : 1;
					j = (spatialBound.Y() <= pCurrNode->m_center.Y()) ? 0 : 1;
					k = (spatialBound.Z() <= pCurrNode->m_center.Z()) ? 0 : 1;
					if(pCurrNode->m_pChildren[i][j][k])
						pCurrNode = pCurrNode->m_pChildren[i][j][k];
					else
						break;
					depth--;
				}
				// we now search children for a best fit Octant for the spatial.
				pPrevNode = pCurrNode;
				while(FitsInBox(pInSpatials[cnt], pCurrNode->m_center))
				{
					i = (spatialBound.X() <= pCurrNode->m_center.X()) ? 0 : 1;
					j = (spatialBound.Y() <= pCurrNode->m_center.Y()) ? 0 : 1;
					k = (spatialBound.Z() <= pCurrNode->m_center.Z()) ? 0 : 1;
					if(pCurrNode->m_pChildren[i][j][k])
					{

						pPrevNode = pCurrNode;
						pCurrNode = pCurrNode->m_pChildren[i][j][k];
					}
					else
						break;
				}
				result = pPrevNode->RemoveObjectFromNode(pInSpatials[cnt],pInSpatials[cnt]->GetSpatialType());
				//      if(result != IFX_OK)
				//        break;
			}

		}
	}
#endif
	return result;
}

IFXRESULT CIFXOctreeCollection::GetSpatials(
	IFXSpatial**&     rpOutSpatials,
	U32&        ruOutNumberOfSpatials,
	IFXSpatial::eType eIntype        )

{
	IFXRESULT iResult = IFX_E_UNSUPPORTED;
#if 0
	IFXRESULT iResult = IFX_E_INVALID_RANGE;
	U32 startIndex =0;
	if(eIntype < IFXSpatial::TYPE_COUNT)
	{

		// Traverse the tree
		ResursivelyAddSpatialsToList(m_pRootNode, rpOutSpatials,
			ruOutNumberOfSpatials,startIndex,eIntype);
		iResult = IFX_OK;
	}
#endif
	return iResult;
}

void CIFXOctreeCollection::ResursivelyAddSpatialsToList(
	COctreeNode *   pCurrNode,
	IFXSpatial**&   rpOutSpatials,
	U32&        ruOutNumberOfSpatials,
	U32&        ruListStart,
	IFXSpatial::eType eIntype        )

{
#if 0
	U32 x,y,z;
	if(pCurrNode)
	{
		// apply f() to the node
		//  fpNodeFunc(on);
		pCurrNode->GetObjectsFromNode(rpOutSpatials, ruOutNumberOfSpatials, ruListStart,eIntype);
		ruListStart += ruOutNumberOfSpatials;
		for ( x = 0; x < MAX_CHILD_X; x++)
			for ( y = 0; y < MAX_CHILD_Y; y++)
				for ( z = 0; z < MAX_CHILD_Z; z++)
					if(pCurrNode->m_pChildren[x][y][z])
					{
						ResursivelyAddSpatialsToList(pCurrNode->m_pChildren[x][y][z],rpOutSpatials,
							ruOutNumberOfSpatials, ruListStart,eIntype);

					}
	}
#endif
	return;
}

// IFXUnknown
U32 CIFXOctreeCollection::AddRef(void)
{
	return ++m_uRefCount;
}

U32 CIFXOctreeCollection::Release(void)
{
	if (--m_uRefCount == 0)
	{
		delete this;
		return 0;
	}
	return m_uRefCount;
}

IFXRESULT CIFXOctreeCollection::QueryInterface( IFXREFIID interfaceId,
											   void**    ppInterface )
{
	IFXRESULT iResult = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXUnknown* ) this;
		else if ( interfaceId == IID_IFXCollection )
			*ppInterface = ( IFXCollection* ) this;
		else if( interfaceId == CID_IFXOctreeCollection )
		{
			*ppInterface = (CIFXOctreeCollection* )this;

		}
		else
		{
			*ppInterface = NULL;
			iResult = IFX_E_UNSUPPORTED;
		}

		if ( IFXSUCCESS( iResult ) )
			AddRef();
	}
	else
		iResult = IFX_E_INVALID_POINTER;

	return iResult;
}


IFXRESULT IFXAPI_CALLTYPE CIFXOctreeCollection_Factory( IFXREFIID interfaceId,
									   void**    ppInterface )
{
	IFXRESULT iResult = IFX_E_INVALID_POINTER;

	if ( ppInterface )
	{
		// It doesn't exist, so try to create it.
		CIFXOctreeCollection *pComponent = new CIFXOctreeCollection();

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
