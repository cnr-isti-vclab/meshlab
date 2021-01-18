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

#include "IFXTQTBaseTriangle.h"
#include "IFXSubdivisionManager.h"



void IFXTQTBaseTriangle::BreadthFirstEvaluate (IFXSubdivisionManager *pSubdivMgr)
{
	U32 uCount = 0;
	U32 uLimit = 1;
	BOOL  bLazyEvalEnabled;

	pSubdivMgr->GetInteger(IFXSubdivisionManager::LazyEvaluationLimit, &uLimit);
	pSubdivMgr->GetBool(IFXSubdivisionManager::LazyEvaluation, &bLazyEvalEnabled);

	IFXTQTTriangle *pCurrTriangle;

	// Assumming we start with the root, enqueue the tree root:
	if (m_BftDeque.GetNumberQueued() == 0)
		m_BftDeque.PushTail() = this;

	// While deque has nodes left to do:
	/// @todo: while some threshold # of triangle processed
	while (m_BftDeque.GetNumberQueued() && (uCount < uLimit))
	{
		// Pull a triangle from the front of the deque:
		pCurrTriangle = m_BftDeque.ElementFromHead(0);
		m_BftDeque.PopHead();
		//int size = m_BftDeque.GetNumberQueued();

		// if pCurrTriangles's children are active, add them to the deque for later processing:
		if (pCurrTriangle->ChildrenActive())
		{
			// Add the 4 children to end of the queue:
			m_BftDeque.PushTail() = pCurrTriangle->GetChildTriangle(TQT_00);
			m_BftDeque.PushTail() = pCurrTriangle->GetChildTriangle(TQT_01);
			m_BftDeque.PushTail() = pCurrTriangle->GetChildTriangle(TQT_10);
			m_BftDeque.PushTail() = pCurrTriangle->GetChildTriangle(TQT_11);
		}
		else
		{
			IFXTQTVertex **pVertex = NULL;
			pVertex = pCurrTriangle->GetVertices();

			if (pVertex[0] &&
				pVertex[1] &&
				pVertex[2])
			{
				// This is a leaf triangle, let's evaluate it:
				// Only perform evaluation on the leaf triangles:
				IFXAdaptiveMetric *pMetric = 0;
				pSubdivMgr->GetAdaptiveMetric(&pMetric);

				IFXAdaptiveMetric::Action action;
				pMetric->EvaluateTriangle (pCurrTriangle, &action);
				pCurrTriangle->SetAction (action);

				if (action == IFXAdaptiveMetric::Subdivide)
				{
					pCurrTriangle->SubdivideAdaptive(pSubdivMgr);
					uCount++;
				}
			}
		}
	}
}


void IFXTQTBaseTriangle::ResetAll(IFXSubdivisionManager *pSubdivMgr)
{
	// First release any deque list data:
	U32 uSize = m_BftDeque.GetNumberQueued();

	if (uSize)
	{
		U32 t;
		for ( t = 0; t < uSize; t++)
			m_BftDeque.PopHead();
	}

	// Now recurse down each triangular quadtree and release data:
	this->ResetAndRelease(pSubdivMgr);
}
