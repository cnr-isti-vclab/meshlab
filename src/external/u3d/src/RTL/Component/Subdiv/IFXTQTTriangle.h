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

#ifndef IFXTQTTriangle_DOT_H
#define IFXTQTTriangle_DOT_H


#include "IFXTQTAddress.h"
#include "IFXAdaptiveMetric.h"
#include "IFXTQTVertex.h"
#include "IFXTQTAttribute.h"
#include "IFXAttributeNeighborhood.h"


// Forward Declarations:
class IFXTQTBaseTriangle;
class IFXSubdivisionManager;

class IFXTQTTriangle
{
public:

	// Enumerations:
	enum SametOrientation {TipUp = false, TipDown = true};

	IFXTQTTriangle ();
	~IFXTQTTriangle ();

	void Init (IFXTQTBaseTriangle *pBaseTriangle, IFXTQTTriangle *pParentTriangle,
		IFXTQTAddress address, SametOrientation sametOrient,
		IFXTQTVertex *pLeftVertex, IFXTQTVertex *pBaseVertex,
		IFXTQTVertex *pRightVertex);
	void SetVertices (IFXTQTVertex *pLeftVertex, IFXTQTVertex *pBaseVertex,
		IFXTQTVertex *pRightVertex);
	void SetBaseTriangle (IFXTQTBaseTriangle *pBaseTriangle);
	IFXRESULT SubdivideAdaptive  (IFXSubdivisionManager *pSubdivMgr);
	void ActivateParentTriangles ();
	void ResetMarkers(U32 depth);

	// Crack fill support:
	void AddCrackFillData (IFXSubdivisionManager *pSubdivMgr,
		IFXTQTAddress::Direction,
		IFXTQTVertex *pCrackVertex,
		BOOL bDiscontinuity);
	void DetectCrack (IFXSubdivisionManager *pSubdivMgr);
	void FindCracks(IFXSubdivisionManager *pSubdivMgr);


	// Traversals:
	/// @todo: Make a single traversal method, parametarized by operation:
	void SubdivideLeaves (IFXSubdivisionManager *pSubdivMgr);
	void Evaluate (IFXSubdivisionManager *pSubdivMgr);
	BOOL Consolidate (IFXSubdivisionManager *pSubdivMgr);
	void ConsolidateLevel(U32 uLevel);
	void ResetAndRelease(IFXSubdivisionManager *pSubdivMgr);
	void GatherRenderData(IFXSubdivisionManager *pSubdivMgr, U16 usDepth);

	// Accessors:
	IFXTQTLocality GetSubdivNeighbor (IFXSubdivisionManager *pSubdivMgr,
		IFXTQTAddress::Direction usDirection,
		IFXAttributeNeighborhood *pNeighHood, U32 uIndex);
	BOOL ChildrenActive();

	IFXTQTTriangle *        GetChildTriangle(U32 uIndex);
	void                    GetMidPoint (IFXTQTAddress::Direction usDirection,
		IFXTQTVertex **ppVertex);
	IFXTQTBaseTriangle*     GetBaseTriangle();
	IFXTQTTriangle*         GetParentTriangle();
	void                    GetAddress(IFXTQTAddress *pAddress);

	void                    GetVertices (IFXTQTVertex **pLeftVertex, IFXTQTVertex **pBaseVertex, IFXTQTVertex **pRightVertex);
	IFXTQTVertex **         GetVertices ();

	SametOrientation    GetSametOrientation();

	IFXAdaptiveMetric::Action   GetAction();
	void                        SetAction(IFXAdaptiveMetric::Action);
	F32                         GetErrorMeasure();
	void                        SetErrorMeasure(F32 errormeasure);

	void Debug(IFXSubdivisionManager *pSubdivMgr, U32 usDepth);

	static  IFXTQTAddress::Direction     m_usOpposite[3];


protected:

	IFXTQTAddress         m_address;
	SametOrientation      m_bSametOrientation;
	IFXTQTBaseTriangle        *m_pBaseTriangle;
	IFXTQTTriangle        *m_pParentTriangle;

	// Vertices are named by their "opposite" edge:
	IFXTQTVertex                *m_pVertex[3];

	// The Triangular Quad Tree data structure hangs here:
	IFXTQTTriangle          *m_pChild[4];

	// Flags
	BOOL                        m_bChildrenActive;
	IFXAdaptiveMetric::Action   m_action;

	// Crack fill data:
	U8                          m_uNeighborCount;
	BOOL                        m_bDiscontinuousCrack;
	IFXTQTVertex                *m_pCrackVertex[2];
	IFXTQTAddress::Direction    m_crackedSide[2];

	// amount of error associated with this triangle, used for adaptive metric
	F32                         m_fErrorMeasure;

	// Implementation methods:
	void Reset ();
	IFXTQTLocality LocateNeighborTriangle(IFXTQTAddress::Direction usDirection,
		IFXTQTAddress *pNeighAddr, IFXTQTTriangle **ppNeighbor,
		IFXTQTAddress::Direction *pOrientation);
	void GetSubdivNeighbors(IFXSubdivisionManager *pSubdivMgr, IFXTQTAddress pAddress[12], IFXTQTTriangle *pTriangle[12], IFXTQTVertex *pVertex[12], IFXTQTVertex *ppMidPoint[3]);
	void FindTriangle (IFXTQTAddress *pAddress, IFXTQTTriangle **ppTriangle);
	IFXRESULT CreateSubdivisionChildren(IFXSubdivisionManager *pSubdivMgr, IFXAttributeNeighborhood pNeighHood[3]);

	IFXTQTVertex* DetermineVertexOnCrackedSide(IFXTQTAddress::Direction crackDirection);
	BOOL IsCrackAcrossDiscontinuity(IFXTQTTriangle* pNeighborTriangle);
	U32  CaptureCrackFillTriangles(IFXSubdivisionManager *pSubdivMgr);
	U32  CaptureCrackFillTrianglesOnDiscontinuity(IFXSubdivisionManager *pSubdivMgr);

	void ResetRenderIndices();
	void ReleaseVertices(IFXSubdivisionManager *pSubdivMgr);
};

IFXINLINE F32 IFXTQTTriangle::GetErrorMeasure()
{
	return m_fErrorMeasure;
}

IFXINLINE void IFXTQTTriangle::SetErrorMeasure(F32 errormeasure)
{
	m_fErrorMeasure = errormeasure;
}

IFXINLINE IFXTQTTriangle* IFXTQTTriangle::GetParentTriangle()
{
	return m_pParentTriangle;
}

IFXINLINE BOOL IFXTQTTriangle::ChildrenActive()
{
	return this->m_bChildrenActive;
}

IFXINLINE IFXTQTTriangle * IFXTQTTriangle::GetChildTriangle(U32 uIndex)
{
	return this->m_pChild[uIndex];
}

IFXINLINE IFXTQTBaseTriangle *IFXTQTTriangle::GetBaseTriangle()
{
	return m_pBaseTriangle;
}

IFXINLINE void IFXTQTTriangle::GetAddress(IFXTQTAddress *pAddress)
{
	pAddress->Assign (&m_address);
}


IFXINLINE void IFXTQTTriangle::GetVertices (IFXTQTVertex **pLeftVertex, IFXTQTVertex **pBaseVertex, IFXTQTVertex **pRightVertex)
{
	*pLeftVertex = m_pVertex[IFXTQTAddress::Left];
	*pBaseVertex = m_pVertex[IFXTQTAddress::Base];
	*pRightVertex = m_pVertex[IFXTQTAddress::Right];
}

IFXINLINE IFXTQTVertex **IFXTQTTriangle::GetVertices ()
{
	return this->m_pVertex;
}

IFXINLINE IFXTQTTriangle::SametOrientation IFXTQTTriangle::GetSametOrientation()
{
	return this->m_bSametOrientation;
}

IFXINLINE void IFXTQTTriangle::SetVertices (IFXTQTVertex *pLeftVertex, IFXTQTVertex *pBaseVertex, IFXTQTVertex *pRightVertex)
{
	m_pVertex[IFXTQTAddress::Left]    = pLeftVertex;
	m_pVertex[IFXTQTAddress::Base]    = pBaseVertex;
	m_pVertex[IFXTQTAddress::Right] = pRightVertex;
}

IFXINLINE void IFXTQTTriangle::SetBaseTriangle (IFXTQTBaseTriangle *pBaseTriangle)
{
	m_pBaseTriangle = pBaseTriangle;
}

IFXINLINE void IFXTQTTriangle::Init (IFXTQTBaseTriangle *pBaseTriangle, IFXTQTTriangle *pParentTriangle,
									 IFXTQTAddress address, SametOrientation sametOrient,
									 IFXTQTVertex *pLeftVertex, IFXTQTVertex *pBaseVertex, IFXTQTVertex *pRightVertex)
{
	// Copy Base Triangle ptr:
	m_pBaseTriangle    = pBaseTriangle;
	m_pParentTriangle  = pParentTriangle;

	// Copy vertex ptrs:
	m_pVertex[IFXTQTAddress::Left]    = pLeftVertex;
	m_pVertex[IFXTQTAddress::Base]    = pBaseVertex;
	m_pVertex[IFXTQTAddress::Right] = pRightVertex;

	// Set uncomputed children to NULL:
	m_pChild[TQT_00] = NULL;
	m_pChild[TQT_01] = NULL;
	m_pChild[TQT_10] = NULL;
	m_pChild[TQT_11] = NULL;

	m_address = address;

	// Copy the samet orientation:
	m_bSametOrientation = sametOrient;

	// Flags:
	m_bChildrenActive = false;
	m_action = IFXAdaptiveMetric::Sustain;

	m_uNeighborCount = 0;
	m_bDiscontinuousCrack = false;
	m_pCrackVertex[0] = NULL;
	m_pCrackVertex[1] = NULL;

	m_fErrorMeasure = -1.0f; // an initial error term, better than perfect ( < 0 )
}

// Starts at the base triangle for a Triangluar Quad Tree and traverses down
// the tree to locate a child triangle with a particular address.  At each level,
// a pair in the address indicates who is the appropriate next child.
/** 
@todo: This search technique can be replaced with a tunable hashing scheme that
will not require decending the tree to find a triangle.
For the moment, It's unrolled the recursive traversal and written it as a for loop.
*/

IFXINLINE void IFXTQTTriangle::FindTriangle (IFXTQTAddress *pAddress, IFXTQTTriangle **ppTriangle)
{
	IFXASSERT (pAddress);

	U32 uDepth = pAddress->Length() - m_address.Length() - 1;
	IFXTQTTriangle *pParent = this;

	// While bits left to process the parent has active children:
	for (; ((uDepth > 0) && pParent->m_bChildrenActive); uDepth--)
	{
		U32 whichChild = pAddress->GetIthPair(uDepth);
		pParent        = pParent->m_pChild[whichChild];
	}

	if (pParent->m_bChildrenActive) {
		*ppTriangle = pParent->m_pChild[pAddress->GetIthPair(0)];
		IFXASSERT(*ppTriangle);
		return;
	}
	// in adaptive mode, it is possible for the neighbor to be a subdiv level less
	*ppTriangle = pParent;

	///@todo Raise an exception here if (ppTriangle->m_address != address!) (special case for when only parent is found.,..)
}

IFXINLINE void IFXTQTTriangle::GetMidPoint (IFXTQTAddress::Direction usDirection,
											IFXTQTVertex **ppVertex)
{
	*ppVertex = NULL;

	// If no children, return with ppVertex == NULL;
	if (!m_pChild[TQT_00])
		return;

	// Otherwise, the desired midpoint will be "labeled with the opposite direction"
	// in the center (TQT_10) child triangle:
	IFXTQTAddress::Direction usOppDir = m_usOpposite [usDirection];
	*ppVertex = m_pChild[TQT_10]->m_pVertex[usOppDir];
}

IFXINLINE void IFXTQTTriangle::ResetRenderIndices()
{
	m_pVertex[IFXTQTAddress::Left]->m_uRenderIndex = IFXTQTVertex::NotLabeled;
	m_pVertex[IFXTQTAddress::Base]->m_uRenderIndex = IFXTQTVertex::NotLabeled;
	m_pVertex[IFXTQTAddress::Right]->m_uRenderIndex = IFXTQTVertex::NotLabeled;
}

IFXINLINE void IFXTQTTriangle::Reset()
{
	// Initial state:
	m_bChildrenActive = false;
	m_uNeighborCount = 0;
	m_bDiscontinuousCrack = false;
	m_action = IFXAdaptiveMetric::Sustain;
	m_pCrackVertex[0] = NULL;
	m_pCrackVertex[1] = NULL;
}


IFXINLINE IFXAdaptiveMetric::Action IFXTQTTriangle::GetAction()
{
	return m_action;
}


IFXINLINE void IFXTQTTriangle::SetAction(IFXAdaptiveMetric::Action action)
{
	m_action = action;
}

#endif
