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

#include <stdio.h>

#include "IFXTQTTriangle.h"
#include "IFXTQTBaseTriangle.h"
#include "IFXSubdivisionManager.h"

// Note that triangle edge labeling follows 3 predicitable rules when
// neighboring triangles share the same base triangle and have subdivision
// connectivity and use Samet's naming convention.  The simple rules are
// 1) The opposite of right edge is a left edge
// 2) The opposite of a left edge is a right edge
// 3) The opposite of a base edge is a base edge!
// This table, m_usOpposite, quickly finds an opposite edge:
IFXTQTAddress::Direction IFXTQTTriangle::m_usOpposite[3] =
{IFXTQTAddress::Right, IFXTQTAddress::Base, IFXTQTAddress::Left};



IFXTQTTriangle::IFXTQTTriangle()
{
}

IFXTQTTriangle::~IFXTQTTriangle()
{
}


// LocateNeighborTriangle is the core neighbor computation method.
// Uses Samet's triangular quadtree addressing plus my extensions for finding a neighbor
// be it local or distal.   If triangle does not exists, then its nearest ancestor is
// returned instead.
// Returns:
//      pNeighAddr - the computed triangle address
//      ppNeighbor - the neighbor triangle or its ancestor
//      pOrientation - if crossing base triangle boundaries, what direction is opposite usDirection
//

IFXTQTLocality IFXTQTTriangle::LocateNeighborTriangle(IFXTQTAddress::Direction usDirection,
													  IFXTQTAddress *pNeighAddr, IFXTQTTriangle **ppNeighbor,
													  IFXTQTAddress::Direction *pOrientation)
{
	IFXTQTBaseTriangle  *pBaseNeighbor = NULL;

	*ppNeighbor = NULL;
	pNeighAddr->Reset();

	*pOrientation = IFXTQTAddress::UnDefined;

	// If subdiv depth = 0 (baseMesh), then just grab base triangles:
	if (m_address.Length() == 0)
	{
		m_pBaseTriangle->GetBaseNeighbor(usDirection, &pBaseNeighbor, pOrientation);
		*ppNeighbor = pBaseNeighbor;
		return DistalTriangle;
	}
	else
	{
		// 1) Compute local neighbor address:
		m_address.LocalNeighbor (usDirection, pNeighAddr);

		// 2) Check for address fault:
		if (pNeighAddr->Faulted())
		{
			// 3) If faulted, compute distal neighbor:

			// 3.1) Find usDirection neighbor of base triangle:
			// 3.2) Find relative orientations of base triangles:
			m_pBaseTriangle->GetBaseNeighbor(usDirection, &pBaseNeighbor, pOrientation);

			// If there's a base neighbor, then
			if (pBaseNeighbor)
			{
				// 3.4) Now compute distal neighbor triangle's address:
				m_address.DistalNeighbor(usDirection, *pOrientation, pNeighAddr);

				// 3.5) Use address to traverse down quad tree to find triangle:
				// LATER: I need not store the quad tree explicity for address lookup.
				// Some sort of global hashing scheme may work well/be faster.
				pBaseNeighbor->FindTriangle(pNeighAddr, ppNeighbor);
				return DistalTriangle;
			}
			else
				return UndefinedTriangle;
		}
		else
		{
			// Use generated neighbor address to locate my neighbor triangle.
			// Also generate the opposite direction in the neighbor triangle:
			m_pBaseTriangle->FindTriangle(pNeighAddr, ppNeighbor);
			*pOrientation = m_usOpposite[usDirection];

			return LocalTriangle;
		}
	}
}


// This method is the crux neighbor finding method.  It uses LocateTriangleNeighbor()
// to compute the neighbors address and then look it up.
// If the exact neighbor is not found, we recursively subdivide until it exists.
// The Locality return value will either be "Local" or "Distal" or "Undefined".
//
IFXTQTLocality IFXTQTTriangle::GetSubdivNeighbor (IFXSubdivisionManager *pSubdivMgr,
												  IFXTQTAddress::Direction usDirection, IFXAttributeNeighborhood *pNeighHood,
												  U32 uIndex)
{
	IFXTQTAddress   NeighAddr;

	IFXTQTTriangle *pNeighbor = NULL;
	IFXTQTAddress::Direction uReOrientedDirection = IFXTQTAddress::UnDefined;
	IFXTQTLocality locality = UndefinedTriangle;

	// Find the subdiv neighbor or it's nearest ancestor:
	locality = LocateNeighborTriangle (usDirection, &NeighAddr, &pNeighbor, &uReOrientedDirection);

	// It's possible that no neighbor triangle exists, even in the base mesh.
	// This indicate that we requesting a neighbor at a mesh boundary.
	// If so, pNeighbor will be NULL, and we'll deal with it in the subdivsion
	// masks.
	if (!pNeighbor)
	{
		return UndefinedTriangle;
	}

	// Prepare for recursion!:
	// 3) If the neighbor triangle at the computed address is not at the proper level of
	// subdivision, then subdivide it until it is.  This should only happen when adaptive is
	// enabled:

	// Did we get the desired neighbor or one of its ancestors:
	// If one of the ancestors, then subdivide until we get the target:
#ifdef _DEBUG
	U32 uIterations = 0;
#endif
#define INFINITE_LOOP   100

	while (pNeighbor->m_address.Length() != NeighAddr.Length())
	{
		IFXASSERT (++uIterations < INFINITE_LOOP);

		// Bring neighbor triangle's level of subdiv up to par by recursively subdividing it:
		pNeighbor->m_action = IFXAdaptiveMetric::Subdivide;
		IFXRESULT result = pNeighbor->SubdivideAdaptive (pSubdivMgr);

		if (result != IFX_OK)
		{
			return locality;
		}

		// Now grab the target neighbor, just generated by the above subdivision call:
		IFXTQTTriangle *pTempNeighbor;
		pNeighbor->FindTriangle (&NeighAddr, &pTempNeighbor);
		pNeighbor = pTempNeighbor;
	}

	// If neighbors are at the same level, then we're ok:
	if (pNeighbor && (NeighAddr.Length() == m_address.Length()) &&
		(uReOrientedDirection != IFXTQTAddress::UnDefined))
	{
		IFXTQTVertex *pVertex   = (pNeighbor)->m_pVertex[uReOrientedDirection];
		pNeighHood->PointAt(uIndex, pVertex, pNeighbor, &NeighAddr, uReOrientedDirection, locality);
	}
	// else Neighbor not found...doesn't exist

	return locality;
}



void IFXTQTTriangle::SubdivideLeaves(IFXSubdivisionManager *pSubdivMgr)
{
	if (m_bChildrenActive)
	{
		m_pChild[TQT_00]->SubdivideLeaves(pSubdivMgr);
		m_pChild[TQT_01]->SubdivideLeaves(pSubdivMgr);
		m_pChild[TQT_10]->SubdivideLeaves(pSubdivMgr);
		m_pChild[TQT_11]->SubdivideLeaves(pSubdivMgr);
	}
	else
	{
		SubdivideAdaptive(pSubdivMgr);
		return;
	}
}





void IFXTQTTriangle::FindCracks (IFXSubdivisionManager *pSubdivMgr)
{
	U32 uMaxRenderDepth;
	pSubdivMgr->GetInteger(IFXSubdivisionManager::MaxRenderDepth, &uMaxRenderDepth);

	if (m_bChildrenActive && (m_address.Length() < uMaxRenderDepth))
	{
		m_pChild[TQT_00]->FindCracks(pSubdivMgr);
		m_pChild[TQT_01]->FindCracks(pSubdivMgr);
		m_pChild[TQT_10]->FindCracks(pSubdivMgr);
		m_pChild[TQT_11]->FindCracks(pSubdivMgr);
	}
	else
	{
		IFXASSERT (m_pVertex[IFXTQTAddress::Left] && m_pVertex[IFXTQTAddress::Base] &&
			m_pVertex[IFXTQTAddress::Right]);

		DetectCrack(pSubdivMgr);
		return;
	}
}



/**

Class:      IFXTQTTriangle
Method:     DetermineVertexOnCrackedSide()
Parameters:   crackDirection - The side opposite the cracked side.  This will
be used to index the cracked vertex in the center triangle.
Return Value: A pointer to the vertex on the cracked side.

Visit parent triangle and then center child triangle to determine which vertex
is on the cracked side.   Convieniently, the cracked side vertex will be in the
center triangle and can be indexed with the "opposite direction".
**/
IFXTQTVertex* IFXTQTTriangle::DetermineVertexOnCrackedSide(IFXTQTAddress::Direction crackDirection)
{
	IFXASSERT (m_pParentTriangle);
	if (!m_pParentTriangle)
		return NULL;

	IFXTQTTriangle *pCenterTriangle = m_pParentTriangle->m_pChild[TQT_10];

	IFXASSERT (pCenterTriangle);
	if (!pCenterTriangle)
		return NULL;

	IFXTQTVertex *pCrackVertex = pCenterTriangle->m_pVertex[crackDirection];

	IFXASSERT (pCrackVertex);
	return (pCrackVertex);
}


/**

Class:      IFXTQTTriangle
Method:     IsCrackAcrossDiscontinuity()
Parameters:   pNeighborTriangle - Pointer to my neighbor triangle.
Return Value: true/false indicated if the crack lies on a discontinuity or not.

Grab base triangles and check to see if they belong to the same mesh or not.
Return false if they are in the same mesh, else return true.
**/
BOOL IFXTQTTriangle::IsCrackAcrossDiscontinuity(IFXTQTTriangle* pNeighborTriangle)
{
	IFXASSERT (m_pBaseTriangle);
	if (!m_pBaseTriangle)
		return IFX_FALSE;

	IFXASSERT (pNeighborTriangle);
	if (!pNeighborTriangle)
		return IFX_FALSE;

	IFXTQTBaseTriangle* pNeighborBaseTriangle = pNeighborTriangle->GetBaseTriangle();

	IFXASSERT (pNeighborBaseTriangle);
	if (!pNeighborBaseTriangle)
		return IFX_FALSE;

	U32 myMeshId, neighborsMeshId;

	m_pBaseTriangle->GetMeshIndex(&myMeshId);
	pNeighborBaseTriangle->GetMeshIndex(&neighborsMeshId);

	if ( myMeshId == neighborsMeshId)
		return IFX_FALSE;
	else
		return IFX_TRUE;
}





/**

Class:      IFXTQTTriangle
Method:     AddCrackFillData
Return Value:

Description:
AddCrackFillData() is called to respond appropriate to detected cracks due to
adaptive subdivision.  In the cases of one or two cracked sides, then
temporary fill triangles will be generated to ensure a water tight rendering.
In the case of three cracked sides, then the subdivision will be completed on
the coarse triangle at the crack site.  This is done by calling
CreateSubdivChildren() with the 3 subdivision results of the neighbors.

**/
void  IFXTQTTriangle::AddCrackFillData (IFXSubdivisionManager *pSubdivMgr,
										IFXTQTAddress::Direction crackedSide,
										IFXTQTVertex *pCrackVertex,
										BOOL bDiscontinuity)
{

	// Gaurd against bogus crackfill data:
	if (crackedSide == IFXTQTAddress::UnDefined)
		return;

	IFXASSERT ((crackedSide >= IFXTQTAddress::Left) && (crackedSide <= IFXTQTAddress::Right));

	m_action = IFXAdaptiveMetric::Sustain;
	m_uNeighborCount++;

	IFXASSERT (m_uNeighborCount <= 6);
	IFXASSERT (pCrackVertex);

	// Capture discontinuity info. If an earlier crack marked this as a discontinuity,
	// then leave that info intack (or it!).
	m_bDiscontinuousCrack |= bDiscontinuity;

	// If first or third neighbor to identify a crack:
	if (m_uNeighborCount == 1)
	{
		m_pCrackVertex[0] = pCrackVertex;
		m_crackedSide[0] = crackedSide;
	}
	else if (m_uNeighborCount == 3)
	{
		m_pCrackVertex[1] = pCrackVertex;
		m_crackedSide[1] = crackedSide;

		// We may have to swap our crack vertexs so that the crack fill triangles
		// don't overlap and the proper winding is maintained:
		if (IFXTQTAddress::m_usReOrient[m_crackedSide[0]+2] != m_crackedSide[1])
		{
			// Swap the cracked sides:
			IFXTQTAddress::Direction    tempDirection;
			IFXTQTVertex       *pTempVertex;

			tempDirection = m_crackedSide[0];
			m_crackedSide[0] = m_crackedSide[1];
			m_crackedSide[1] = tempDirection;

			pTempVertex = m_pCrackVertex[0];
			m_pCrackVertex[0] = m_pCrackVertex[1];
			m_pCrackVertex[1] = pTempVertex;
		}
	}
	else if (m_uNeighborCount == 5)
	{
		// 5 neighboring triangles can only mean that we have subdivision on 3 sides...
		// (There will be 6 neighbors total!)
		// So we handle the case where we have 3 cracked sides....the actually subdivided,
		// since it's it's neighbors have already yield the subdivision midpoints.  We
		// just need to hook them into the quadtree for this triangle.

		m_action = IFXAdaptiveMetric::Subdivide;
		SubdivideAdaptive (pSubdivMgr);

		// 4 child Triangles now generated and the crack is filled.
	}


	IFXASSERT (m_uNeighborCount <= 6);
}











// DetectCrack() will examine its neighbors and decide what crack filling
// actions need to be taken.  Several scenarios can happen:
// 1) A neighbor is 1 level of subdivision more corse.  In this case we
// simply increment the neighbor count in the courser triangle.  We'll
// use this count to determine proper crack fill triangles in FillCrack().
// FillCrack() can determine from the count left by behind by DetectCrack()
// whether 1, 2, or 3 neighbors are off by 1 level of subdivision.
// 2) The neighbor is at the same level of subdivision.  In this case,
// no action is required and we leave it alone.

void IFXTQTTriangle::DetectCrack (IFXSubdivisionManager *pSubdivMgr)
{
	IFXTQTAddress      pAddress[3];
	IFXTQTTriangle     *pTriangle[3];
	IFXTQTAddress::Direction    reOrientation[3];

	// 1) Gather 3 subdiv neighbors and their vertex data:
	LocateNeighborTriangle (IFXTQTAddress::Left, &pAddress[IFXTQTAddress::Left],
		&pTriangle[IFXTQTAddress::Left], &reOrientation[IFXTQTAddress::Left]);
	LocateNeighborTriangle (IFXTQTAddress::Base, &pAddress[IFXTQTAddress::Base],
		&pTriangle[IFXTQTAddress::Base], &reOrientation[IFXTQTAddress::Base]);
	LocateNeighborTriangle (IFXTQTAddress::Right, &pAddress[IFXTQTAddress::Right],
		&pTriangle[IFXTQTAddress::Right], &reOrientation[IFXTQTAddress::Right]);

	// Compute relative subdivision depths:
	int leftDiff = 0, baseDiff = 0, rightDiff = 0;

	if ((pTriangle[IFXTQTAddress::Left])  &&
		(reOrientation[IFXTQTAddress::Left] != IFXTQTAddress::UnDefined))
	{
		leftDiff  = m_address.Length() - pTriangle[IFXTQTAddress::Left]->m_address.Length();
	}

	if ((pTriangle[IFXTQTAddress::Base])  &&
		(reOrientation[IFXTQTAddress::Base] != IFXTQTAddress::UnDefined))
	{
		baseDiff  = m_address.Length() - pTriangle[IFXTQTAddress::Base]->m_address.Length();
	}

	if ((pTriangle[IFXTQTAddress::Right]) &&
		(reOrientation[IFXTQTAddress::Right] != IFXTQTAddress::UnDefined))
	{
		rightDiff = m_address.Length() - pTriangle[IFXTQTAddress::Right]->m_address.Length();
	}

	IFXTQTVertex *pCrackVertex = NULL;

	// Check for cracks against left neighbor:
	if (leftDiff == 1)
	{
		IFXASSERT (m_pVertex[IFXTQTAddress::Right] && m_pVertex[IFXTQTAddress::Base]);

		pCrackVertex    = DetermineVertexOnCrackedSide(IFXTQTAddress::Right);
		BOOL bDiscontinuous = IsCrackAcrossDiscontinuity (pTriangle[IFXTQTAddress::Left]);

		m_action = IFXAdaptiveMetric::Sustain;

		pTriangle[IFXTQTAddress::Left]->AddCrackFillData(pSubdivMgr,
			reOrientation[IFXTQTAddress::Left], pCrackVertex, bDiscontinuous);
	}

	// Check for cracks against base neighbor:
	if (baseDiff == 1)
	{
		IFXASSERT (m_pVertex[IFXTQTAddress::Right] && m_pVertex[IFXTQTAddress::Left]);

		pCrackVertex    = DetermineVertexOnCrackedSide(IFXTQTAddress::Base);
		BOOL bDiscontinuous = IsCrackAcrossDiscontinuity (pTriangle[IFXTQTAddress::Base]);

		m_action = IFXAdaptiveMetric::Sustain;

		pTriangle[IFXTQTAddress::Base]->AddCrackFillData(pSubdivMgr,
			reOrientation[IFXTQTAddress::Base], pCrackVertex, bDiscontinuous);
	}

	// Check for cracks against right neighbor:
	if (rightDiff == 1)
	{
		IFXASSERT (m_pVertex[IFXTQTAddress::Base] && m_pVertex[IFXTQTAddress::Left]);

		pCrackVertex    = DetermineVertexOnCrackedSide(IFXTQTAddress::Left);
		BOOL bDiscontinuous = IsCrackAcrossDiscontinuity (pTriangle[IFXTQTAddress::Right]);

		m_action = IFXAdaptiveMetric::Sustain;

		pTriangle[IFXTQTAddress::Right]->AddCrackFillData(pSubdivMgr,
			reOrientation[IFXTQTAddress::Right], pCrackVertex, bDiscontinuous);
	}

	// The subdivision level of neighbor triangles should not differ by more than 1,
	// as this is a _Restricted Triangular QuadTree_:
	//IFXASSERT ((leftDiff < 2) && (baseDiff < 2) && (rightDiff < 2));
}







// Traverse from base triangle down, making sure that all m_bChildrenActive flags are set:
void IFXTQTTriangle::ActivateParentTriangles()
{
	IFXTQTBaseTriangle *pBaseTriangle = GetBaseTriangle();

	U16     whichChild;
	U16     usDepth   = m_address.Length();
	IFXTQTTriangle *pChild    = pBaseTriangle;
	IFXTQTTriangle *pParent;

	// While bits left to process and pChild is not NULL:
	int d;
	for ( d = usDepth - 1; ((d >= 0) && pChild); d--)
	{
		pParent     = pChild;
		whichChild  = m_address.GetIthPair(d);
		pChild    = pChild->m_pChild[whichChild];

		// Activate the childern:
		if (pChild)
			pParent->m_bChildrenActive   = IFX_TRUE;
	}
}















IFXRESULT IFXTQTTriangle::CreateSubdivisionChildren(IFXSubdivisionManager *pSubdivMgr, IFXAttributeNeighborhood pNeighHood[3])
{
	// If we don't have any subdivision children, then let's subdivide:
	BOOL bAdaptiveEnabled;
	pSubdivMgr->GetBool(IFXSubdivisionManager::Adaptive, &bAdaptiveEnabled);

	if (!m_pChild[TQT_00])
	{
		if (!bAdaptiveEnabled || (bAdaptiveEnabled && (m_action == IFXAdaptiveMetric::Subdivide)))
		{
			// Create new children triangles, by grabbing a few triangles off the free list:
			int i;
			for (i = 0; i < 4; i++)
			{
				m_pChild[i] = pSubdivMgr->m_pTriangleAllocator->Allocate();
				IFXASSERT(m_pChild[i]);
				if (!m_pChild[i])
					return IFX_E_UNDEFINED;
				m_pChild[i]->Reset();
			}

			// Middle triangle (TQT_10)'s samet orientation is the opposite of it's
			// parent's samet orientation:
			SametOrientation bMiddleTriangle = (SametOrientation) !m_bSametOrientation;

			// Create 4 new child addresses:
			IFXTQTAddress childAddress[4];
			for (i = TQT_00; i <= TQT_11; i++)
			{
				// Copy parent address
				childAddress[i] = m_address;

				// Push new label:
				childAddress[i].PushLabel ((IFXTQTTriangleLabel) i);
			}

			// Grab the subdivision computed midpoints from the neighborhoods:
			IFXTQTVertex *pLeftMidpoint = pNeighHood[IFXTQTAddress::Left].GetMidpoint();
			IFXTQTVertex *pBaseMidpoint = pNeighHood[IFXTQTAddress::Base].GetMidpoint();
			IFXTQTVertex *pRightMidpoint = pNeighHood[IFXTQTAddress::Right].GetMidpoint();

			/*
			IFXASSERT (pLeftMidpoint && pBaseMidpoint && pRightMidpoint);
			if (!(pLeftMidpoint && pBaseMidpoint && pRightMidpoint))
			{
			Reset();
			return IFX_OK;
			}
			*/



			m_pChild[TQT_00]->Init (m_pBaseTriangle, this, childAddress[TQT_00], m_bSametOrientation,
				pRightMidpoint, m_pVertex[IFXTQTAddress::Base], pLeftMidpoint);

			m_pChild[TQT_01]->Init (m_pBaseTriangle, this, childAddress[TQT_01], m_bSametOrientation,
				pBaseMidpoint, pLeftMidpoint, m_pVertex[IFXTQTAddress::Right]);

			m_pChild[TQT_10]->Init (m_pBaseTriangle, this, childAddress[TQT_10], bMiddleTriangle,
				pRightMidpoint, pBaseMidpoint, pLeftMidpoint);

			m_pChild[TQT_11]->Init (m_pBaseTriangle, this, childAddress[TQT_11], m_bSametOrientation,
				m_pVertex[IFXTQTAddress::Left], pRightMidpoint, pBaseMidpoint);

			// Make sure parents above me are active:
			ActivateParentTriangles();

			m_bChildrenActive = IFX_TRUE;
			m_action = IFXAdaptiveMetric::Sustain;

		} // if m_action == Subdivide

	}
	else
	{
		/** @todo:
		Verify that current subdivision is valid...
		Suggested implementation: if child triangles are on free list and have correct address then good to go
		if Midpoints have time stamp (equal to) or (older) than parent triangle, then assume values are upto date,
		A new stamp stamp indicates that they have been recycled...this requires a time stamp in the FreeListMember
		class.
		*/

		if (m_pChild[TQT_00] && m_pChild[TQT_01] && m_pChild[TQT_10] && m_pChild[TQT_11])
		{
			// Children exist, just need to be activated:
			m_pChild[TQT_00]->Reset();
			m_pChild[TQT_00]->ResetRenderIndices();

			m_pChild[TQT_01]->Reset();
			m_pChild[TQT_01]->ResetRenderIndices();

			m_pChild[TQT_10]->Reset();
			m_pChild[TQT_10]->ResetRenderIndices();

			m_pChild[TQT_11]->Reset();
			m_pChild[TQT_11]->ResetRenderIndices();

			// Make sure parents above me are active:
			ActivateParentTriangles();

			m_bChildrenActive = IFX_TRUE;
			m_action = IFXAdaptiveMetric::Sustain;
		}
		else
		{
			IFXASSERT (IFX_FALSE);
		}
	}

	return IFX_OK;
}



IFXRESULT IFXTQTTriangle::SubdivideAdaptive (IFXSubdivisionManager *pSubdivMgr)
{
	ResetRenderIndices();

	BOOL bAdaptiveEnabled;
	pSubdivMgr->GetBool(IFXSubdivisionManager::Adaptive, &bAdaptiveEnabled);
	if (bAdaptiveEnabled && (m_action != IFXAdaptiveMetric::Subdivide))
		return IFX_OK;

	// If at user-specified depth limit, then don't subdivide, return:
	U32 uMaxComputeDepth;
	pSubdivMgr->GetInteger(IFXSubdivisionManager::MaxComputeDepth, &uMaxComputeDepth);
	if (m_address.Length() >= uMaxComputeDepth)
		return IFX_OK;

	IFXButterflyScheme *pButterflyScheme;
	pButterflyScheme = pSubdivMgr->GetSubdivisionScheme();

	// 1) Get the neighborhoods needed to subdivide each edge:
	// It's important to realize that if adaptive subdivision is enabled,
	// and the edge neighbors are not available because the some triangle
	// subdivision is too coarse, then those triangles will be recursively
	// subdivided first, before the entire neighborhood can be gathered.
	IFXAttributeNeighborhood    pNeighHood[3];
	pButterflyScheme->GatherEdgeNeighborhood(this, IFXTQTAddress::Left, &pNeighHood[IFXTQTAddress::Left]);
	pButterflyScheme->GatherEdgeNeighborhood(this, IFXTQTAddress::Base, &pNeighHood[IFXTQTAddress::Base]);
	pButterflyScheme->GatherEdgeNeighborhood(this, IFXTQTAddress::Right, &pNeighHood[IFXTQTAddress::Right]);

	// 2) Now that we have the edge neighborhoods, we perform the subdivision
	// computation.  Necessary storage for results will be allocated from the
	// free list.  A pointer to the subdivision result will be set in the
	// pNeighHood records.  In the course of gathering the edge neighborhoods,
	// we may have discovered that a neighbor's subdivision computation already
	// yielded the midpoint.  In which case, we won't subdivide the edge.  Instead,
	// we will grab a pointer to the neighbor's midpoint and bump up the midpoint's
	// reference count so that we may use it.
	IFXTQTVertex *pVertex;
	if (!pNeighHood[IFXTQTAddress::Left].GetMidpoint())
	{
		pVertex = pSubdivMgr->m_pVertexAllocator->Allocate();
		if (!pVertex)
			return IFX_E_UNDEFINED;
		pVertex->m_uRenderIndex = IFXTQTVertex::NotLabeled;
		pNeighHood[IFXTQTAddress::Left].SetMidpoint(pVertex);

		pButterflyScheme->SubdivideEdge(&pNeighHood[IFXTQTAddress::Left]);
	}
	else
		pSubdivMgr->m_pVertexAllocator->IncRefCount(pNeighHood[IFXTQTAddress::Left].GetMidpoint());

	if (!pNeighHood[IFXTQTAddress::Base].GetMidpoint())
	{
		pVertex = pSubdivMgr->m_pVertexAllocator->Allocate();
		if (!pVertex)
			return IFX_E_UNDEFINED;
		pVertex->m_uRenderIndex = IFXTQTVertex::NotLabeled;
		pNeighHood[IFXTQTAddress::Base].SetMidpoint(pVertex);

		pButterflyScheme->SubdivideEdge(&pNeighHood[IFXTQTAddress::Base]);
	}
	else
		pSubdivMgr->m_pVertexAllocator->IncRefCount(pNeighHood[IFXTQTAddress::Base].GetMidpoint());

	if (!pNeighHood[IFXTQTAddress::Right].GetMidpoint())
	{
		pVertex = pSubdivMgr->m_pVertexAllocator->Allocate();
		if (!pVertex)
			return IFX_E_UNDEFINED;
		pVertex->m_uRenderIndex = IFXTQTVertex::NotLabeled;
		pNeighHood[IFXTQTAddress::Right].SetMidpoint(pVertex);

		pButterflyScheme->SubdivideEdge(&pNeighHood[IFXTQTAddress::Right]);
	}
	else
		pSubdivMgr->m_pVertexAllocator->IncRefCount(pNeighHood[IFXTQTAddress::Right].GetMidpoint());


	// 3) Use these subdivision results to create the newly subdivided triangles:
	IFXRESULT result = CreateSubdivisionChildren(pSubdivMgr, pNeighHood);


	return result;
	/***
	// 4) Subdivision is now complete for this triangle.  As a final check, we must make
	// sure that we are not subdividing next to an attribute discontinuity.  If we are
	// and the subdivisions are not equal, then we must also subdivide the neighbor.
	if (bAdaptiveEnabled)
	{
	U32 direction;
	for (direction = 0; direction < 3; direction++)
	{
	if (pNeighHood[direction].Discontinuous())
	{
	IFXTQTTriangle *pNeighTriangle = pNeighHood[direction].GetTriangle(3);
	if (!pNeighTriangle->m_bChildrenActive)
	{
	pNeighTriangle->SubdivideAdaptive(pSubdivMgr);
	}
	}
	}
	}
	***/

}






// Decend to all computed triangles and vertexs and release them
// back to the free lists, thus resetting all buffered subdivision computations.
// Takes us back to ground zero.
void IFXTQTTriangle::ResetAndRelease(IFXSubdivisionManager *pSubdivMgr)
{
	if (m_pChild[TQT_00])
	{
		// Recurse:
		m_pChild[TQT_00]->ResetAndRelease(pSubdivMgr);
		m_pChild[TQT_01]->ResetAndRelease(pSubdivMgr);
		m_pChild[TQT_10]->ResetAndRelease(pSubdivMgr);
		m_pChild[TQT_11]->ResetAndRelease(pSubdivMgr);

		// Deactivate this triangle:
		m_bChildrenActive = IFX_FALSE;
		m_uNeighborCount = 0;
		m_bDiscontinuousCrack = IFX_FALSE;
		m_pCrackVertex[0] = NULL;
		m_pCrackVertex[1] = NULL;
		m_crackedSide[0] = IFXTQTAddress::UnDefined;
		m_crackedSide[1] = IFXTQTAddress::UnDefined;
		m_action = IFXAdaptiveMetric::Sustain;

		// Release the Child triangles:
		pSubdivMgr->m_pTriangleAllocator->Deallocate(m_pChild[TQT_00]);
		pSubdivMgr->m_pTriangleAllocator->Deallocate(m_pChild[TQT_01]);
		pSubdivMgr->m_pTriangleAllocator->Deallocate(m_pChild[TQT_10]);
		pSubdivMgr->m_pTriangleAllocator->Deallocate(m_pChild[TQT_11]);

		m_pChild[TQT_00] = NULL;
		m_pChild[TQT_01] = NULL;
		m_pChild[TQT_10] = NULL;
		m_pChild[TQT_11] = NULL;
	}

	{
		// Deactivate this triangle:
		m_bChildrenActive = IFX_FALSE;
		m_uNeighborCount = 0;
		m_pCrackVertex[0] = NULL;
		m_pCrackVertex[1] = NULL;
		m_crackedSide[0] = IFXTQTAddress::UnDefined;
		m_crackedSide[1] = IFXTQTAddress::UnDefined;
		m_action = IFXAdaptiveMetric::Sustain;

		ReleaseVertices(pSubdivMgr);
	}


}




// Decend to all triangles the indicated level and forceably
// consolidate them out.  This is needed after the maxComputeDepth
// has been changed:
void IFXTQTTriangle::ConsolidateLevel(U32 uLevel)
{
	if (m_bChildrenActive)
	{
		if (m_address.Length() < (uLevel-1) )
		{
			// Recurse:
			m_pChild[TQT_00]->ConsolidateLevel(uLevel);
			m_pChild[TQT_01]->ConsolidateLevel(uLevel);
			m_pChild[TQT_10]->ConsolidateLevel(uLevel);
			m_pChild[TQT_11]->ConsolidateLevel(uLevel);
		}
		else
		{
			if (m_address.Length() == (uLevel-1))
			{
				// Force consolidation:
				// Deactivate children:

				m_pChild[TQT_00]->ResetRenderIndices();
				m_pChild[TQT_01]->ResetRenderIndices();
				m_pChild[TQT_10]->ResetRenderIndices();
				m_pChild[TQT_11]->ResetRenderIndices();

				m_bChildrenActive = IFX_FALSE;
				m_uNeighborCount = 0;
				m_action = IFXAdaptiveMetric::Sustain;

				m_pChild[TQT_00]->m_action = IFXAdaptiveMetric::Sustain;
				m_pChild[TQT_01]->m_action = IFXAdaptiveMetric::Sustain;
				m_pChild[TQT_10]->m_action = IFXAdaptiveMetric::Sustain;
				m_pChild[TQT_11]->m_action = IFXAdaptiveMetric::Sustain;
			}
		}
	}
	// Handle the base mesh case:
	else if (uLevel == 1)
	{
		// Force consolidation:
		// Deactivate children:
		m_bChildrenActive = IFX_FALSE;
		m_uNeighborCount = 0;
		m_bDiscontinuousCrack = IFX_FALSE;
		m_action = IFXAdaptiveMetric::Sustain;
	}
}





void IFXTQTTriangle::ReleaseVertices(IFXSubdivisionManager *pSubdivMgr)
{
	if (m_address.Length() != 0)
	{
		if (m_address.GetIthPair(0) == TQT_10)
		{
			if (m_pVertex[IFXTQTAddress::Left])
				pSubdivMgr->m_pVertexAllocator->Deallocate(m_pVertex[IFXTQTAddress::Left]);
			if (m_pVertex[IFXTQTAddress::Base])
				pSubdivMgr->m_pVertexAllocator->Deallocate(m_pVertex[IFXTQTAddress::Base]);
			if (m_pVertex[IFXTQTAddress::Right])
				pSubdivMgr->m_pVertexAllocator->Deallocate(m_pVertex[IFXTQTAddress::Right]);
		}

		m_pVertex[IFXTQTAddress::Left] = NULL;
		m_pVertex[IFXTQTAddress::Base] = NULL;
		m_pVertex[IFXTQTAddress::Right] = NULL;
	}

	m_action = IFXAdaptiveMetric::Sustain;
}



BOOL IFXTQTTriangle::Consolidate(IFXSubdivisionManager *pSubdivMgr)
{
	/** todo:
	Return the children triangles to the free list:
	int i;
	for ( i = 0; i < 4; i++)
	pSubdivMgr->m_pFreeTriangles->Return(m_pChild[i]);
	*/


	if (m_bChildrenActive)
	{
		// Recurse:
		BOOL b00 = m_pChild[TQT_00]->Consolidate(pSubdivMgr);
		BOOL b01 = m_pChild[TQT_01]->Consolidate(pSubdivMgr);
		BOOL b10 = m_pChild[TQT_10]->Consolidate(pSubdivMgr);
		BOOL b11 = m_pChild[TQT_11]->Consolidate(pSubdivMgr);

		if (b00 && b01 && b10 & b11)
		{
			// Deactivate children:
			m_bChildrenActive = IFX_FALSE;
			m_action = IFXAdaptiveMetric::Sustain;

			// Time to release midpoints & the triangles:
			// Release the midpoints that form the center(TQT_10) triangle:
			m_pChild[TQT_00]->ReleaseVertices(pSubdivMgr);
			m_pChild[TQT_01]->ReleaseVertices(pSubdivMgr);
			m_pChild[TQT_10]->ReleaseVertices(pSubdivMgr);
			m_pChild[TQT_11]->ReleaseVertices(pSubdivMgr);

			pSubdivMgr->m_pTriangleAllocator->Deallocate(m_pChild[TQT_00]);
			pSubdivMgr->m_pTriangleAllocator->Deallocate(m_pChild[TQT_01]);
			pSubdivMgr->m_pTriangleAllocator->Deallocate(m_pChild[TQT_10]);
			pSubdivMgr->m_pTriangleAllocator->Deallocate(m_pChild[TQT_11]);

			m_pChild[TQT_00] = NULL;
			m_pChild[TQT_01] = NULL;
			m_pChild[TQT_10] = NULL;
			m_pChild[TQT_11] = NULL;
		}

		return IFX_FALSE;
	}
	else
	{
		if (m_action == IFXAdaptiveMetric::Consolidate)
		{
			// We may have to cancel consolidation if neighbors differ by more
			// than 1 level of subdiv:
			IFXTQTAddress      pAddress[3];
			IFXTQTTriangle     *pTriangle[3];
			IFXTQTAddress::Direction    reOrientation[3];
			/* IFXTQTLocality    pLocality[3]; */

			// Gather 3 subdiv neighbors:
			/* pLocality[IFXTQTAddress::Left] = */ LocateNeighborTriangle (IFXTQTAddress::Left,
				&pAddress[IFXTQTAddress::Left], &pTriangle[IFXTQTAddress::Left],
				&reOrientation[IFXTQTAddress::Left]);
			/* pLocality[IFXTQTAddress::Base] = */ LocateNeighborTriangle (IFXTQTAddress::Base,
				&pAddress[IFXTQTAddress::Base], &pTriangle[IFXTQTAddress::Base],
				&reOrientation[IFXTQTAddress::Base]);
			/* pLocality[IFXTQTAddress::Right] = */ LocateNeighborTriangle (IFXTQTAddress::Right,
				&pAddress[IFXTQTAddress::Right], &pTriangle[IFXTQTAddress::Right],
				&reOrientation[IFXTQTAddress::Right]);

			// Check subdivision level of 3 neighbors...If anyone has children, then
			// we are off by more than 1 level of subdivision, thus violating our
			// "off by only 1" restriction for adaptive subdivision.  In such case,
			// we'll return IFX_FALSE, to cancel the subdivision:
			if (pTriangle[IFXTQTAddress::Left] && pTriangle[IFXTQTAddress::Left]->m_bChildrenActive)
				return IFX_FALSE;
			else if (pTriangle[IFXTQTAddress::Base] && pTriangle[IFXTQTAddress::Base]->m_bChildrenActive)
				return IFX_FALSE;
			else if (pTriangle[IFXTQTAddress::Right] && pTriangle[IFXTQTAddress::Right]->m_bChildrenActive)
				return IFX_FALSE;

			/***
			// Also check for discontinuity boundaries.  If we are up against an
			// attribute discontinuity, then we can only allow consolidation if
			// the neighbor triangle is also marked for consolidation.
			U32 direction;
			for (direction = IFXTQTAddress::Left; direction <= IFXTQTAddress::Right; direction++)
			{
			if (pTriangle[direction] && (pLocality[direction] == DistalTriangle))
			{
			IFXTQTBaseTriangle*             pBaseTriangle   = GetBaseTriangle();
			IFXEdgeContinuityDescriptor*    pEdgeDescr      =
			pBaseTriangle->GetEdgeDescriptor((IFXTQTAddress::Direction) direction);

			// Are there any discontinuities?, If so, is my neighbor marked for consolidation?:
			if (pEdgeDescr->AllAttributes )//&& (pTriangle[direction]->m_action != IFXAdaptiveMetric::Consolidate))
			{
			return IFX_FALSE;
			}
			}
			}
			***/

			// Adaptive "off by 1" condition satisfied and no discontinuity boundaries:
			return IFX_TRUE;
		}
		else
			return IFX_FALSE;
	}
}





U32 IFXTQTTriangle::CaptureCrackFillTriangles(IFXSubdivisionManager *pSubdivMgr)
{
	IFXTQTVertex *pFillVertex[3];

	// If 1 crack, then fill with 2 triangles:
	if (m_uNeighborCount == 2)
	{
		IFXASSERT (m_pCrackVertex[0]);

		// Crackfill triangle #1:
		pFillVertex[0] = m_pVertex[ IFXTQTAddress::m_usReOrient[ m_crackedSide[0]+2 ] ];
		pFillVertex[2] = m_pCrackVertex[0];
		pFillVertex[1] = m_pVertex[ IFXTQTAddress::m_usReOrient[ m_crackedSide[0] ] ];
		pSubdivMgr->AddRenderCrackFillTriangle(this, pFillVertex, m_bSametOrientation);

		// Crackfill triangle #2:
		pFillVertex[0] = pFillVertex[2];
		//pFillVertex[1] = pFillVertex[1];
		pFillVertex[2] = m_pVertex[ IFXTQTAddress::m_usReOrient[ m_crackedSide[0] + 1 ] ];
		pSubdivMgr->AddRenderCrackFillTriangle(this, pFillVertex, m_bSametOrientation);

		return 2;
	}
	// If we have 2 cracks, then fill with 3 triangles:
	else if (m_uNeighborCount == 4)
	{
		IFXASSERT (m_pCrackVertex[0] && m_pCrackVertex[1]);

		// Crackfill triangle #1:
		pFillVertex[0] = m_pVertex[ IFXTQTAddress::m_usReOrient[ m_crackedSide[0]+2 ] ];
		pFillVertex[2] = m_pCrackVertex[0];
		pFillVertex[1] = m_pVertex[ IFXTQTAddress::m_usReOrient[ m_crackedSide[0] ] ];
		pSubdivMgr->AddRenderCrackFillTriangle(this, pFillVertex, m_bSametOrientation);

		// Crackfill triangle #2:
		pFillVertex[0] = pFillVertex[2];
		//pFillVertex[1] = pFillVertex[1];
		pFillVertex[2] = m_pCrackVertex[1];
		pSubdivMgr->AddRenderCrackFillTriangle(this, pFillVertex, m_bSametOrientation);

		// Crackfill triangle #3:
		//pFillVertex[0] = pFillVertex[0];
		pFillVertex[1] = pFillVertex[2];
		pFillVertex[2] = m_pVertex[ IFXTQTAddress::m_usReOrient[ m_crackedSide[0] + 1 ] ];
		pSubdivMgr->AddRenderCrackFillTriangle(this, pFillVertex, m_bSametOrientation);
		return 3;
	}
	// Otherwise We've got a weird crack fill situation:
	else
	{
		IFXASSERT (IFX_FALSE);
		return 0;
	}
}


/**

Class:      IFXTQTTriangle
Method:     CaptureCrackFillTrianglesOnDiscontinuity
Return Value:

Description:


**/
U32 IFXTQTTriangle::CaptureCrackFillTrianglesOnDiscontinuity(IFXSubdivisionManager *pSubdivMgr)
{
	IFXTQTVertex *pFillVertex[3];

	IFXTQTVertex pDisconVertex[2];

	// If 1 crack, then fill with 2 triangles:
	if (m_uNeighborCount == 2)
	{
		if (!m_pCrackVertex[0])
			return 0;

		// Create temporary vertices for crack edges against discontinuities:
		// For these vertices, we'll copy the vertex position computed in the
		// neighbor, but we'll average the attributes from this triangle's principal
		// vertices.
		pDisconVertex[0].m_uRenderIndex = IFXTQTVertex::NotLabeled;
		pDisconVertex[0].m_position = m_pCrackVertex[0]->m_position;
		pDisconVertex[0].m_normal.Add(
			m_pVertex[ IFXTQTAddress::m_usReOrient[ m_crackedSide[0]+2 ] ]->m_normal,
			m_pVertex[ IFXTQTAddress::m_usReOrient[ m_crackedSide[0]+1 ] ]->m_normal);
		pDisconVertex[0].m_normal.Scale(0.5f);
		pDisconVertex[0].m_texCoord.Add(
			m_pVertex[ IFXTQTAddress::m_usReOrient[ m_crackedSide[0]+2 ] ]->m_texCoord,
			m_pVertex[ IFXTQTAddress::m_usReOrient[ m_crackedSide[0]+1 ] ]->m_texCoord);
		pDisconVertex[0].m_texCoord.Scale(0.5f);

		// Crackfill triangle #1:
		pFillVertex[0] = m_pVertex[ IFXTQTAddress::m_usReOrient[ m_crackedSide[0]+2 ] ];
		pFillVertex[2] = &pDisconVertex[0];
		pFillVertex[1] = m_pVertex[ IFXTQTAddress::m_usReOrient[ m_crackedSide[0] ] ];
		pSubdivMgr->AddRenderCrackFillTriangle(this, pFillVertex, m_bSametOrientation);

		// Crackfill triangle #2:
		pFillVertex[0] = pFillVertex[2];
		//pFillVertex[1] = pFillVertex[1];
		pFillVertex[2] = m_pVertex[ IFXTQTAddress::m_usReOrient[ m_crackedSide[0] + 1 ] ];
		pSubdivMgr->AddRenderCrackFillTriangle(this, pFillVertex, m_bSametOrientation);

		m_pCrackVertex[0] = NULL;
		m_pCrackVertex[1] = NULL;
		return 2;
	}
	// If we have 2 cracks, then fill with 3 triangles:
	else if (m_uNeighborCount == 4)
	{
		if (!m_pCrackVertex[0] || !m_pCrackVertex[1])
			return 0;

		// Create temporary vertices for crack edges against discontinuities:
		// For these vertices, we'll copy the vertex position computed in the
		// neighbor, but we'll average the attributes from this triangle's principal
		// vertices...crude but looks fine on crack fill.
		pDisconVertex[0].m_uRenderIndex = IFXTQTVertex::NotLabeled;
		pDisconVertex[0].m_position = m_pCrackVertex[0]->m_position;
		pDisconVertex[0].m_normal.Add(
			m_pVertex[ IFXTQTAddress::m_usReOrient[ m_crackedSide[0]+2 ] ]->m_normal,
			m_pVertex[ IFXTQTAddress::m_usReOrient[ m_crackedSide[0]+1 ] ]->m_normal);
		pDisconVertex[0].m_normal.Scale(0.5f);
		pDisconVertex[0].m_texCoord.Add(
			m_pVertex[ IFXTQTAddress::m_usReOrient[ m_crackedSide[0]+2 ] ]->m_texCoord,
			m_pVertex[ IFXTQTAddress::m_usReOrient[ m_crackedSide[0]+1 ] ]->m_texCoord);
		pDisconVertex[0].m_texCoord.Scale(0.5f);

		pDisconVertex[1].m_uRenderIndex = IFXTQTVertex::NotLabeled;
		pDisconVertex[1].m_position = m_pCrackVertex[1]->m_position;
		pDisconVertex[1].m_normal = m_pCrackVertex[1]->m_normal;
		pDisconVertex[1].m_normal.Add(
			m_pVertex[ IFXTQTAddress::m_usReOrient[ m_crackedSide[1]+2 ] ]->m_normal,
			m_pVertex[ IFXTQTAddress::m_usReOrient[ m_crackedSide[1]+1 ] ]->m_normal);
		pDisconVertex[1].m_normal.Scale(0.5f);
		pDisconVertex[1].m_texCoord.Add(
			m_pVertex[ IFXTQTAddress::m_usReOrient[ m_crackedSide[1]+2 ] ]->m_texCoord,
			m_pVertex[ IFXTQTAddress::m_usReOrient[ m_crackedSide[1]+1 ] ]->m_texCoord);
		pDisconVertex[1].m_texCoord.Scale(0.5f);


		// Crackfill triangle #1:
		pFillVertex[0] = m_pVertex[ IFXTQTAddress::m_usReOrient[ m_crackedSide[0]+2 ] ];
		pFillVertex[2] = &pDisconVertex[0];
		pFillVertex[1] = m_pVertex[ IFXTQTAddress::m_usReOrient[ m_crackedSide[0] ] ];
		pSubdivMgr->AddRenderCrackFillTriangle(this, pFillVertex, m_bSametOrientation);

		// Crackfill triangle #2:
		pFillVertex[0] = pFillVertex[2];
		//pFillVertex[1] = pFillVertex[1];
		pFillVertex[2] = &pDisconVertex[1];
		pSubdivMgr->AddRenderCrackFillTriangle(this, pFillVertex, m_bSametOrientation);

		// Crackfill triangle #3:
		//pFillVertex[0] = pFillVertex[0];
		pFillVertex[1] = pFillVertex[2];
		pFillVertex[2] = m_pVertex[ IFXTQTAddress::m_usReOrient[ m_crackedSide[0] + 1 ] ];
		pSubdivMgr->AddRenderCrackFillTriangle(this, pFillVertex, m_bSametOrientation);

		m_pCrackVertex[0] = NULL;
		m_pCrackVertex[1] = NULL;
		return 3;
	}
	// Otherwise We've got a weird crack fill situation:
	else
	{
		IFXASSERT (IFX_FALSE);
		return 0;
	}
}






void IFXTQTTriangle::GatherRenderData(IFXSubdivisionManager *pSubdivMgr, U16 usDepth)
{
	if (m_bChildrenActive && usDepth)
	{
		//U32 uNumTriangles = 0;
		// Recurse down the triangular quad tree:
		m_pChild[TQT_00]->GatherRenderData (pSubdivMgr, usDepth-1);
		m_pChild[TQT_01]->GatherRenderData (pSubdivMgr, usDepth-1);
		m_pChild[TQT_10]->GatherRenderData (pSubdivMgr, usDepth-1);
		m_pChild[TQT_11]->GatherRenderData (pSubdivMgr, usDepth-1);
	}
	else
	{
		// If crackfill data then get that, otherwise just get the triangle:
		if (m_uNeighborCount)
		{
			U32 uMaxRenderDepth;
			pSubdivMgr->GetInteger(IFXSubdivisionManager::MaxRenderDepth, &uMaxRenderDepth);

			if (usDepth <= uMaxRenderDepth)
			{
				if (m_bDiscontinuousCrack)
					CaptureCrackFillTrianglesOnDiscontinuity(pSubdivMgr);
				else
					CaptureCrackFillTriangles(pSubdivMgr);
			}

			else
			{
				pSubdivMgr->AddRenderTriangle(this);
			}
		}
		else
		{
			pSubdivMgr->AddRenderTriangle(this);
		}
	}
}


void IFXTQTTriangle::Debug(IFXSubdivisionManager *pSubdivMgr, U32 usDepth)
{
	if (m_bChildrenActive && usDepth)
	{
		//U32 uNumTriangles = 0;
		// Recurse down the triangular quad tree:
		m_pChild[TQT_00]->Debug (pSubdivMgr, usDepth-1);
		m_pChild[TQT_01]->Debug (pSubdivMgr, usDepth-1);
		m_pChild[TQT_10]->Debug (pSubdivMgr, usDepth-1);
		m_pChild[TQT_11]->Debug (pSubdivMgr, usDepth-1);
	}
	else
	{

		// Compute center of triangle:
		IFXVector3 center;
		center = m_pVertex[IFXTQTAddress::Left]->m_position;
		center.Add(m_pVertex[IFXTQTAddress::Base]->m_position);
		center.Add(m_pVertex[IFXTQTAddress::Right]->m_position);

		center.Scale (0.333333333f);

		IFXVector3 color;
		color.Set(1.0f, 1.0f, 1.0f);

		char pString[256];
		sprintf (pString, "%d, %d, %d", m_uNeighborCount, m_pBaseTriangle->m_usId, m_address.Address());

	}
}



#define INFINITE_RECURSION_PROBABLE 10

void IFXTQTTriangle::ResetMarkers(U32 depth)
{
	IFXASSERT (depth < INFINITE_RECURSION_PROBABLE);

	if (depth >= INFINITE_RECURSION_PROBABLE)
		return;

	if (m_bChildrenActive)
	{
		IFXASSERT (m_pChild[TQT_00]);
		IFXASSERT (m_pChild[TQT_01]);
		IFXASSERT (m_pChild[TQT_10]);
		IFXASSERT (m_pChild[TQT_11]);

		// Recurse down the triangular quad tree:
		m_pChild[TQT_00]->ResetMarkers(depth + 1);
		m_pChild[TQT_01]->ResetMarkers(depth + 1);
		m_pChild[TQT_10]->ResetMarkers(depth + 1);
		m_pChild[TQT_11]->ResetMarkers(depth + 1);
	}
	// Perform necessary per triangle data reset:
	m_uNeighborCount = 0;
	m_bDiscontinuousCrack = IFX_FALSE;

	IFXASSERT(m_pVertex[IFXTQTAddress::Left] && m_pVertex[IFXTQTAddress::Right] &&
		m_pVertex[IFXTQTAddress::Base]);

	m_pVertex[IFXTQTAddress::Left]->m_uRenderIndex = IFXTQTVertex::NotLabeled;
	m_pVertex[IFXTQTAddress::Base]->m_uRenderIndex = IFXTQTVertex::NotLabeled;
	m_pVertex[IFXTQTAddress::Right]->m_uRenderIndex = IFXTQTVertex::NotLabeled;
}
