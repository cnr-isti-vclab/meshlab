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
#ifndef IFXSUBDIVISIONSCHEME_H
#define IFXSUBDIVISIONSCHEME_H

#include "IFXTQTTriangle.h"
#include "IFXButterflyMask.h"


class IFXBFMaskLayout;
class IFXAttributeNeighborhood;


#define BUTTERFLY_NUM_MASK_LAYOUTS  5   ///< Number of detectable mask layout variations.
#define BUTTERFLY_NUM_MASKS         3   ///< Number of mask implementations (optimized for:scalar, 2D, 3D)


/// IFXButterflyScheme implements the butterfly subdivision scheme  
class IFXButterflyScheme
{
public:

    IFXButterflyScheme();
    ~IFXButterflyScheme();

    void GatherEdgeNeighborhood (IFXTQTTriangle *pCntrTriangle, 
		IFXTQTAddress::Direction uDirection, IFXAttributeNeighborhood *pNeighHood);
    void SubdivideEdge  (IFXAttributeNeighborhood *pNeighHood);

    void SetSubdivMgr       (IFXSubdivisionManager *pSubdivMgr); 
    void SetSmoothingFactor      (F32 fSurfaceTension);

private:

    enum {BFCorner, BFBoundary, BFNearBoundary, BFNearBoundaryJustAverage, BFContinous};
    enum {BFMask2D, BFMask3D, BFMask3DNormal};

    IFXBFMaskLayout         *m_pMaskLayout  [BUTTERFLY_NUM_MASK_LAYOUTS];
    IFXButterflyMask        *m_pMask        [BUTTERFLY_NUM_MASKS];
    IFXSubdivisionManager   *m_pSubdivMgr;

    // Implementation methods:
    void IdentifyLayoutAndApplyMask(F32 **ppMaskValues, IFXButterflyMask *pMask, F32 *pMidpoint, bool bPosition);
};


IFXINLINE void IFXButterflyScheme::SetSubdivMgr (IFXSubdivisionManager *pSubdivMgr)
{
    m_pSubdivMgr = pSubdivMgr;
}

IFXINLINE void IFXButterflyScheme::SetSmoothingFactor (F32 fSurfaceTension)  
{ 
    // Set the smoothing factor in each butterfly mask implementation
	U32 i;
    for (i = 0; i < BUTTERFLY_NUM_MASKS; i++)
    {
        if (m_pMask[i])
            m_pMask[i]->SetSmoothingValue(fSurfaceTension);
    }
}

#endif
