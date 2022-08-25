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
#ifndef IFXBFMASKLAYOUT_DOT_H
#define IFXBFMASKLAYOUT_DOT_H

#include "IFXButterflyMask.h"

/** 
	Class IFXBFMaskLayout abstracts the possible layout configurations that
	maybe encountered. The method SelectMask is polymorphically defined
	depending on the layout condition identified.  SelectMask will then
	perform any mask fixup that maybe necessary and then make the call to 
	the appropriate subdivision mask.
*/
class IFXBFMaskLayout
{
public:
   virtual ~IFXBFMaskLayout() {}

   virtual void IFXAPI  SelectMask (F32 **ppMaskValues, IFXButterflyMask *pBFMask, F32 *pMidPoint) = 0;

};


// Attribute or mesh corner:

// Naming convention for a single subdivision mask with vertices 8 and 9 as
// support for boundary computations.   This mask further demonstates the
// special case of boundaries adjacent to corners.  An example mesh boundary 
// near a mesh corner is indicated with the "#" symbol:"C" indicates the
// troublesome mesh corner.  All vertices to the right of the mesh boundary
// will be null :
//
//                #
//                 #
//                  #
//                   #
//                  5 #          9 
//    MESH         / \ #       /  
//                /   \ #     /    
//               /     \ #   /      
//              /       \ # /        
//             /         \ #                NON-MESH...outer space!
//            2-----------0-#C--------6
//           / \         / \#        /  
//          /   \       /  #\       /    
//         /     \     *  #  \     /      
//        /       \   /  #    \   /           
//       /         \ /  #      \ /          
//      4-----------1--#--------3             
//                 / \#        /   
//                /  #\       /      
//               /  #  \     /      
//              /  #    \   /        
//             /  #      \ /          
//            8  #        7     
//              #
//             #
//            #
//           #
//
class IFXBFCornerLayout : public IFXBFMaskLayout
{
public: 
    virtual void IFXAPI  SelectMask (F32 **ppMaskValues, IFXButterflyMask *pBFMask, F32 *pMidPoint);

private:
    
    IFXVector3  m_pTempVertex[2];
};


void IFXBFCornerLayout::SelectMask(F32 **ppMaskValues, IFXButterflyMask *pBFMask, F32 *pMidpoint)
{
    IFXASSERT (ppMaskValues[0] && ppMaskValues[1] && ppMaskValues[2]);
   
    // Heads up for a weird (hopefully rare!) corner/boundary condidtion...
    // only one of the support vertices should be NULL on a mesh corner.
    // If both supports for 4 point boundary subdivison are NULL, then we
    // don't have enough mesh data to compute the mask.  This might happen
    // with a 2 triangle rectangular mesh for instance...In other words, we are
    // on a boundary on a triangle that touches two mesh corners.
    
    // Fake the missing vertex values via reflection:
    if (!ppMaskValues[8])
    {      
        ppMaskValues[8] = (float *) &m_pTempVertex[0];

        pBFMask->ReflectAttributeThroughPointAlongEdge (ppMaskValues[0], ppMaskValues[1], ppMaskValues[8]);
     }
    
    if (!ppMaskValues[9])
    { 
        ppMaskValues[9] = (float *) &m_pTempVertex[1];
              
        pBFMask->ReflectAttributeThroughPointAlongEdge (ppMaskValues[1], ppMaskValues[0], ppMaskValues[9]);
    }

    // Now that the fix up is complete, we subdivide using the 4 point boundary mask:
    pBFMask->Apply4PointMask(ppMaskValues[0], ppMaskValues[1], ppMaskValues[8], ppMaskValues[9], pMidpoint); 
}


// Attribute or mesh boundary:
class IFXBFBoundaryLayout : public IFXBFMaskLayout
{
public:
    virtual void IFXAPI  SelectMask (F32 **ppMaskValues, IFXButterflyMask *pBFMask, F32 *pMidpoint);
};

void IFXBFBoundaryLayout::SelectMask(F32 **ppMaskValues, IFXButterflyMask *pBFMask, F32 *pMidpoint)
{
    pBFMask->Apply4PointMask(ppMaskValues[0], ppMaskValues[1], ppMaskValues[8], ppMaskValues[9], pMidpoint); 
}


// Near attribute or mesh boundary:
// We are within the 1-neighborhood of a mesh or attribute boundary.

// There are 4 special cases where
// one of the vertices in the mask is unavailable due to the subdivision
// triangle's proxemity to a boundary. It's probably 1 triangle away from
// the boundary.  The following diagram depicts such a nearby boundary (#'s) in
// which case mask vertex 6 is unavaible and must be "synthesized".  Mask
// vertex 9 is also unavaible, but it is only used when to support the 4 pt
// mask when the subdivision triangle lies right on the boundary...this 
// case is handled else where.   
//    The other possible missing vertices include vertices 5, 4, and 7.
// In each case the missing vertex is "synthesized" via reflection.
//  
//                #
//                 #
//                  #
//                   #
//                  5 #          9 
//     MESH        / \ #       /            NON-MESH...outer space!
//                /   \ #     /    
//               /     \ #   /      
//              /       \ # /        
//             /         \ #           
//            2-----------0-#---------6
//           / \         / \ #  B    /  
//          /   \       /   \ #     /    
//         /     \     *     \ #   /      
//        /       \   /       \ # /           
//       /         \ /         \ #          
//      4-----------1-----------3 #           
//                 / \         /   #
//                /   \       /     #
//               /     \     /       #
//              /       \   /         #
//             /         \ /          
//            8           7     
//               
//   
class IFXBFNearBoundaryLayout : public IFXBFMaskLayout
{
public:
    virtual void IFXAPI  SelectMask (F32 **ppMaskValues, IFXButterflyMask *pBFMask, F32 *pMidPoint);

private:
    // Temporaries:
    IFXVector3 m_tempVector3[4];

    // Implementation methods:

};

void IFXBFNearBoundaryLayout::SelectMask(F32 **ppMaskValues, IFXButterflyMask *pBFMask, F32 *pMidPoint)
{
    // We need to fake the missing values via reflection:
    if (!ppMaskValues[4])
    {
        ppMaskValues[4] = (float *) &m_tempVector3[0];
        pBFMask->ReflectAttributeThroughEdge(ppMaskValues[0], ppMaskValues[1], ppMaskValues[2], ppMaskValues[4]);
    }
    if (!ppMaskValues[5])
    {
        ppMaskValues[5] = (float *) &m_tempVector3[1];
        pBFMask->ReflectAttributeThroughEdge(ppMaskValues[1], ppMaskValues[0], ppMaskValues[2], ppMaskValues[5]);
    }

    if (!ppMaskValues[6])
    {
        IFXASSERT (ppMaskValues[3]);
                
        ppMaskValues[6] = (float *) &m_tempVector3[2];
        pBFMask->ReflectAttributeThroughEdge(ppMaskValues[1], ppMaskValues[0], ppMaskValues[3], ppMaskValues[6]);
    }
    
    if (!ppMaskValues[7])
    {
        IFXASSERT (ppMaskValues[3]);

        ppMaskValues[7] = (float *) &m_tempVector3[2];
        pBFMask->ReflectAttributeThroughEdge(ppMaskValues[0], ppMaskValues[1], ppMaskValues[3], ppMaskValues[7]);
    }

    // Now invoke the continous butterfly mask:
    pBFMask->ApplyButterflyMask(ppMaskValues, pMidPoint);
}


class IFXBFNearBoundaryJustAvgLayout : public IFXBFMaskLayout
{
public:
    virtual void IFXAPI  SelectMask (F32 **ppMaskValues, IFXButterflyMask *pBFMask, F32 *pMidPoint);

private:
    // Temporaries:
    IFXVector3 m_tempVector3[4];

    // Implementation methods:

};

void IFXBFNearBoundaryJustAvgLayout::SelectMask(F32 **ppMaskValues, IFXButterflyMask *pBFMask, F32 *pMidPoint)
{
    // We won't fake any points via projection.  Rather, we'll just do a simple
    // average.  This is usefull for some surface attributes such as texture coordinates
    // and normals.

    pBFMask->Apply2PointMask(ppMaskValues[0], ppMaskValues[1], pMidPoint);
}

// This is the *general case* for butterfly scheme: all attributes in the mask
// are present and continuous.
class IFXBFContinuousLayout : public IFXBFMaskLayout
{
public:
    virtual void IFXAPI  SelectMask (F32 **ppMaskValues, IFXButterflyMask *pBFMask, F32 *pMidPoint);
};

void IFXBFContinuousLayout::SelectMask(F32 **ppMaskValues, IFXButterflyMask *pBFMask, F32 *pMidPoint)
{
    pBFMask->ApplyButterflyMask(ppMaskValues, pMidPoint);
}

#endif
