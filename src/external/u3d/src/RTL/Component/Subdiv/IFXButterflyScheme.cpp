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
#include "IFXButterflyScheme.h"
#include "IFXButterflyMask.h"
#include "IFXBFMaskLayout.h"
#include "IFXAttributeNeighborhood.h"
#include "IFXSubdivisionManager.h"


IFXButterflyScheme::IFXButterflyScheme()
{
    m_pSubdivMgr = NULL;
	int i;
    for (i = 0; i < BUTTERFLY_NUM_MASK_LAYOUTS; i++)
        m_pMaskLayout[i] = NULL;

    for (i = 0; i < BUTTERFLY_NUM_MASKS; i++)
        m_pMask[i] = NULL;
   

    // Allocate layout conditions and masks to implement the butterfly scheme:
    m_pMaskLayout[BFCorner]         = new IFXBFCornerLayout;        if (!m_pMaskLayout[BFCorner]) return;
    m_pMaskLayout[BFBoundary]       = new IFXBFBoundaryLayout;      if (!m_pMaskLayout[BFBoundary]) return;
    m_pMaskLayout[BFNearBoundary]   = new IFXBFNearBoundaryLayout;  if (!m_pMaskLayout[BFNearBoundary]) return;
    m_pMaskLayout[BFNearBoundaryJustAverage]   = new IFXBFNearBoundaryJustAvgLayout;  if (!m_pMaskLayout[BFNearBoundaryJustAverage]) return;
    m_pMaskLayout[BFContinous]      = new IFXBFContinuousLayout;    if (!m_pMaskLayout[BFContinous]) return;
    
    m_pMask[BFMask2D]               = new IFXButterflyMask2D;       if (!m_pMask[BFMask2D]) return;
    m_pMask[BFMask3D]               = new IFXButterflyMask3D;       if (!m_pMask[BFMask3D]) return;
    m_pMask[BFMask3DNormal]         = new IFXButterflyMask3DNormal; if (!m_pMask[BFMask3DNormal]) return;
}



IFXButterflyScheme::~IFXButterflyScheme()
{
    
    // Cleanup butterfly scheme implementation allocations:
	int i;
    for (i = 0; i < BUTTERFLY_NUM_MASK_LAYOUTS; i++)
    {
       if (m_pMaskLayout[i])
       {
           delete m_pMaskLayout[i];
           m_pMaskLayout[i] = NULL;
       }
    }

    for (i = 0; i < BUTTERFLY_NUM_MASKS; i++)
    {
        if (m_pMask[i])
        {
            delete m_pMask[i];
            m_pMask[i] = NULL;
        }
    }
}





// Naming convention for a single edge's butterfly subdivision mask:
// The asterix indicates the desired subdivision target.  Naming is
// relative to that point.   tx indicates a triangle lableling; t4,
// t5, t3, t6, t7 are labled.
//
//                  5           9 
//                 / \         / 
//                /   \       /    
//               L     R     /      
//              /   t5  \   /        
//             /         \ /           
//            2-----B-----0-----------6
//           / \   cntr  / \         /  
//          /   \ trngle/   \  t6   /    
//         /     \     *     R     /      
//        L  t4  RL   RL  t3  \   /           
//       /         \ /         \ /          
//      4-----B-----1-----B-----3             
//                 / \         /   
//                /   \       /      
//               /     \  t7 /      
//              /       \   /        
//             /         \ /          
//            8           7            
// 
// If a mesh boundary is detected and a 4 point boundary mask is applied, then
// the additional 2 points requuired will be found in slots 8 and 9.


// GatherEdgeNeighborhood() will gather the neighbors needed to subdivide a
// specific edge.  The neighborhood will be stored as integer indexed pointers
// in the mask pictured above.  All of the neighborhood data will be stored in
// a IFXAttributeNeighborhood structure allocated on the runtime stack.
//  Note that the orientation of the mask depends
// on which edge (Left, Base, Right) is being subdivided.

void IFXButterflyScheme::GatherEdgeNeighborhood(IFXTQTTriangle *pCntrTriangle, 
                                                IFXTQTAddress::Direction usDirection, 
                                                IFXAttributeNeighborhood *pNeighHood)
{
    // Clear the neighborhood:
    pNeighHood->Reset();

    // We'll re-orient depending on uDirection...this will allow us to pretend that
    // each neighborhood gather is a "Right edge neighborhood gather".
    IFXTQTAddress::Direction uLeft, uBase, uRight;
    uRight  = usDirection;
    uLeft   = IFXTQTAddress::m_usReOrient[usDirection + 1];
    uBase   = IFXTQTAddress::m_usReOrient[usDirection + 2];

    // Gather attribute data at the 3 local vertices of the center triangle:
    IFXTQTVertex **ppVertex;
    ppVertex = pCntrTriangle->GetVertices();

    // Point the neighborhood at these 3 vertices (trivial, so far)
    pNeighHood->PointAt(0, ppVertex[uLeft]);
    pNeighHood->PointAt(1, ppVertex[uBase]);
    pNeighHood->PointAt(2, ppVertex[uRight]);

    IFXTQTLocality locality;

    // 1) Gather ptrs to the 3 near neighbors directly adjacent to center triangle:
    pCntrTriangle->GetSubdivNeighbor (m_pSubdivMgr, uLeft,  pNeighHood, 4);
    pCntrTriangle->GetSubdivNeighbor (m_pSubdivMgr, uBase,  pNeighHood, 5);
    locality = pCntrTriangle->GetSubdivNeighbor (m_pSubdivMgr, uRight, pNeighHood, 3);

    IFXTQTAddress::Direction uDirectionA;
    IFXTQTAddress::Direction uDirectionB; 
    IFXTQTTriangle *pTriangle;

    // 2) If triangle t3 existed, then grab triangle t3's neighbor triangles:
    if (pNeighHood->VertexPresent(3))
    {
        pTriangle = pNeighHood->GetTriangle(3);
		uDirectionA = pNeighHood->ReOrient(3, IFXTQTAddress::Left);

		// Get edge continuity information:
        // Did we cross a base mesh triangle?, If so, then we need to look for discontinuities:
        if (locality == DistalTriangle) 
        {
            IFXTQTBaseTriangle*             pBaseTriangle   = pTriangle->GetBaseTriangle();
            IFXEdgeContinuityDescriptor*    pEdgeDescr      = pBaseTriangle->GetEdgeDescriptor(uDirectionA);

            // Are there any discontinuities?
            if (pEdgeDescr->AllAttributes)
			{
                pNeighHood->MarkDiscontinuous(true);
			}
        }

        // Check first to see if midpoint is already computed:
        IFXTQTVertex *pVertex;
        pTriangle->GetMidPoint(uDirectionA, &pVertex);
        
        // If midpoint existed, then we'll grab it and abandon this subdiv
        // computation, provided that the midpoint doesn't lie on an 
        // an edge with an attribute discontinuity:
        if (pVertex && !pNeighHood->Discontinuous())
        {
            pNeighHood->SetMidpoint(pVertex);
            return;
        }

        if (pTriangle->GetSametOrientation() == IFXTQTTriangle::TipUp)
        {
            uDirectionA = pNeighHood->ReOrient (3, IFXTQTAddress::Base);
            uDirectionB = pNeighHood->ReOrient (3, IFXTQTAddress::Right);
        }
        else
        {
            uDirectionA = pNeighHood->ReOrient (3, IFXTQTAddress::Base);
            uDirectionB = pNeighHood->ReOrient (3, IFXTQTAddress::Right);
        }
         
        pTriangle->GetSubdivNeighbor (m_pSubdivMgr, uDirectionA, pNeighHood, 7);
        pTriangle->GetSubdivNeighbor (m_pSubdivMgr, uDirectionB, pNeighHood, 6);
    }

    // Steps 3 and 4 provide additional support for boundary layouts

    // 3) If triangle t4 existed, then grab triangle t4's base neighbor triangle:
    if (pNeighHood->VertexPresent(4))
    {
        pTriangle = pNeighHood->GetTriangle(4);
        if (pTriangle->GetSametOrientation() == IFXTQTTriangle::TipUp)
            uDirectionA = pNeighHood->ReOrient (4, IFXTQTAddress::Base);
        else
            uDirectionA = pNeighHood->ReOrient (4, IFXTQTAddress::Right);

        pTriangle->GetSubdivNeighbor (m_pSubdivMgr, uDirectionA, pNeighHood, 8);
    }

    // 4) If triangle t5 existed, then grab triangle t5's right neighbor triangle:
    if (pNeighHood->VertexPresent(5))
    {
        pTriangle = pNeighHood->GetTriangle(5);
        if (pTriangle->GetSametOrientation() == IFXTQTTriangle::TipUp)
            uDirectionA = pNeighHood->ReOrient (5, IFXTQTAddress::Right);
        else
            uDirectionA = pNeighHood->ReOrient (5, IFXTQTAddress::Base);

        pTriangle->GetSubdivNeighbor (m_pSubdivMgr, uDirectionA, pNeighHood, 9);
    }
}


void IFXButterflyScheme::SubdivideEdge (IFXAttributeNeighborhood *pNeighHood)
{
    // Use subdivision to compute the value for each vertex attribute:
    IdentifyLayoutAndApplyMask (pNeighHood->GetPositionMask(),  m_pMask[BFMask3D],       pNeighHood->GetMidpointPosition(), true);
    IdentifyLayoutAndApplyMask (pNeighHood->GetNormalMask(),    m_pMask[BFMask3DNormal], pNeighHood->GetMidpointNormal(),   false);
   
	//IdentifyLayoutAndApplyMask (pNeighHood->GetTexCoordMask(),  m_pMask[BFMask2D],       pNeighHood->GetMidpointTexCoord(), false);
	// We'll simplify texture coordinate subdivision by forcing 
	// use of the Just Average mask:
	
	 m_pMaskLayout[BFNearBoundaryJustAverage]->SelectMask(
		 pNeighHood->GetTexCoordMask(), 
		 m_pMask[BFMask2D], 
		 pNeighHood->GetMidpointTexCoord());
	
}





void IFXButterflyScheme::IdentifyLayoutAndApplyMask(F32 **ppMaskValues, IFXButterflyMask *pMask, F32 *pMidpoint, bool bPosition)
{
    // Logic for mask discontinuities:
    // We select a subdivision mask based on how much of the mask data 
    // existed.  Usually the mask will be completely filled in and the
    // continuous subdivsion mask will be employed.  However if some of
    // the mask data is missing, then we are on a mesh or attribute boundary
    // and we must employ the appropriate special case mask:

    // For butterfly there are 4 cases:
    // 1) On a boundary, 2) A Boundary at a corner, 3) In the 1-neighborhood of a Boundary, 
    // 4) No boundary, the mesh is continous.


    // 1) Are we on a boundary?...if so apply special 4 point boundary mask:
    if (ppMaskValues[3] == NULL)
    {
        // 2) Is the boundary on a corner...worst case!:
        if (!ppMaskValues[8] || !ppMaskValues[9])
        {
            //m_pMaskLayout[BFCorner]->SelectMask(ppMaskValues, pMask, pMidpoint);
            m_pMaskLayout[BFNearBoundaryJustAverage]->SelectMask(ppMaskValues, pMask, pMidpoint);
        }
        else
        {
            m_pMaskLayout[BFBoundary]->SelectMask(ppMaskValues, pMask, pMidpoint);
        }
    }
    // 3) Are we adjacent to a boundary?...if so the near boundary mask layout will
    // fake the cross-boundary points via projection, then it invokes the usual
    // continous mask:
    else if (!(ppMaskValues[4] && ppMaskValues[5] && ppMaskValues[6] && ppMaskValues[7]))
    {
        if (bPosition)
            //m_pMaskLayout[BFNearBoundary]->SelectMask(ppMaskValues, pMask, pMidpoint);
            m_pMaskLayout[BFNearBoundaryJustAverage]->SelectMask(ppMaskValues, pMask, pMidpoint);
        else
            m_pMaskLayout[BFNearBoundaryJustAverage]->SelectMask(ppMaskValues, pMask, pMidpoint);
    }
    else 
    {
        //4) No boundary, use the general case continuous butterfly mask:
        m_pMaskLayout[BFContinous]->SelectMask(ppMaskValues, pMask, pMidpoint);
    }
}


