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
#ifndef IFXBUTTERFLYMASK_DOT_H
#define IFXBUTTERFLYMASK_DOT_H

#include "IFXVector3.h"

// Naming convention for a single butterfly subdivision mask:
//
//                  5           9 
//                 / \         / 
//                /   \       /    
//               /     \     /      
//              /  11   \   /        
//             /         \ /           
//            2-----------0-----------6
//           / \    B    / \    B    /  
//          /   \       /   \       /    
//         /     \     *     \     /      
//        /   6   \   /       \   /           
//       /         \ /         \ /          
//      4-----------1-----------3             
//                 / \         /   
//                /   \   1   /      
//               /     \     /      
//              /       \   /        
//             /         \ /          
//            8           7            
// 
// If a mesh boundary is detected and a 4 point boundary mask is applied, then
// the additional 2 points requuired will be found in slots 8 and 9.

class IFXButterflyMask
{
public:
    virtual ~IFXButterflyMask() {}

    virtual void IFXAPI  ApplyButterflyMask (F32 **ppMask, F32 *pMidPoint)                      = 0;
    virtual void IFXAPI  ReflectAttributeThroughEdge (F32 *pV0, F32 *pV1, F32 *pV2, F32 *pVr)   = 0;
    virtual void IFXAPI  Apply4PointMask (F32 *pV0, F32 *pV1, F32 *pV2, F32 *pV3, F32 *pMidpoint) = 0;
    virtual void IFXAPI  Apply2PointMask (F32 *pV0, F32 *pV1, F32 *pMidpoint) = 0;
    virtual void IFXAPI  ReflectAttributeThroughPointAlongEdge (F32 *pV1, F32 *pV2, F32 *pVr)   = 0;

    void SetSmoothingValue (F32 fValue);

protected:

    F32 m_fSmoothing;
    F32 m_fSmoothingTimes2;
};


IFXINLINE void IFXButterflyMask::SetSmoothingValue (F32 fSurfaceTension)
{
    // RE-map the UI's tension value to a butterfly smoothing w factor:
    F32 fValue = 1.0f - fSurfaceTension;
    m_fSmoothing = 0.2f * fValue;
    m_fSmoothingTimes2 = 2.0f * m_fSmoothing;
}







// 3D Butterfly Mask:
class IFXButterflyMask3D : public IFXButterflyMask
{
    virtual void IFXAPI  ApplyButterflyMask (F32 **ppMask, F32 *pMidpoint);
    virtual void IFXAPI  ReflectAttributeThroughEdge (F32 *pV0, F32 *pV1, F32 *pV2, F32 *pVr);
    virtual void IFXAPI  Apply4PointMask (F32 *pV0, F32 *pV1, F32 *pV2, F32 *pV3, F32 *pMidpoint);
    virtual void IFXAPI  Apply2PointMask (F32 *pV0, F32 *pV1, F32 *pMidpoint);
    virtual void IFXAPI  ReflectAttributeThroughPointAlongEdge (F32 *pV1, F32 *pV2, F32 *pVr);
};


IFXINLINE void IFXButterflyMask3D::ApplyButterflyMask (F32 **ppMask, F32 *pMidPoint)
{
    // Cast incoming float pointers to appropriate type:
    IFXVector3 **ppV    = (IFXVector3 **) ppMask;
    IFXVector3 *pResult = (IFXVector3 *) pMidPoint;

    pResult->X() = 0.5f * (ppV[0]->X() + ppV[1]->X()) 
						+ m_fSmoothingTimes2 * (ppV[2]->X() + ppV[3]->X())
						- m_fSmoothing * (ppV[4]->X() + ppV[5]->X() + ppV[6]->X() + ppV[7]->X());

	pResult->Y() = 0.5f * (ppV[0]->Y() + ppV[1]->Y()) 
						+ m_fSmoothingTimes2 * (ppV[2]->Y() + ppV[3]->Y())
						- m_fSmoothing * (ppV[4]->Y() + ppV[5]->Y() + ppV[6]->Y() + ppV[7]->Y());

	pResult->Z() = 0.5f * (ppV[0]->Z() + ppV[1]->Z()) 
						+ m_fSmoothingTimes2 * (ppV[2]->Z() + ppV[3]->Z())
						- m_fSmoothing * (ppV[4]->Z() + ppV[5]->Z() + ppV[6]->Z() + ppV[7]->Z());
}

// If we are near by a boundary and one of the vertices in the secondary
// support of the subdivision mask is set to null, then we'll fake the
// vertex by reflecting it's neighbor point.
// pV0 is the point to reflect, pV1 and pV2 define the edge across which
// we will reflect.  pVr will contain the reflected result.
// The BFMaskLayout objects will detect this situation and invoke this
// method when it occurs.
IFXINLINE void IFXButterflyMask3D::ReflectAttributeThroughEdge (F32 *pV0, F32 *pV1, F32 *pV2, F32 *pVr)
{
    // Cast incoming float pointers to appropriate IFXVector3 type:
    IFXVector3 *p3D_V0 = (IFXVector3 *)  pV0;
    IFXVector3 *p3D_V1 = (IFXVector3 *)  pV1;
    IFXVector3 *p3D_V2 = (IFXVector3 *)  pV2;
    IFXVector3 *p3D_Vr = (IFXVector3 *)  pVr;

    IFXVector3 u, v;
    u.Subtract(*p3D_V0, *p3D_V1);
    v.Subtract(*p3D_V2, *p3D_V1);
    F32 fDot = u.DotProduct(v);
    
    if (fDot != 0.0f)
    {
        fDot /= v.DotProduct(v);    // Scale it
        u.Scale(fDot);
    }

    u.Add (*p3D_V2);

    // u now represents the perpendicular intersect point.

    v.Subtract (*p3D_V0, u);
    v.Scale (-1.0f);
    p3D_Vr->Add(u, v);
    // pVr now represents the reflected point.
}


IFXINLINE void IFXButterflyMask3D::ReflectAttributeThroughPointAlongEdge (F32 *pV1, F32 *pV2, F32 *pVr)
{
    // Cast the incoming floats to vector 3:
    IFXVector3 *p3D_pV1 = (IFXVector3 *) pV1;
    IFXVector3 *p3D_pV2 = (IFXVector3 *) pV2;
    IFXVector3 *p3D_pVr = (IFXVector3 *) pVr;

    IFXVector3 u;
    u.Subtract(*p3D_pV2, *p3D_pV1);
    p3D_pVr->Add(*p3D_pV2, u);
    
    // p3D_Vr now represents the reflected point.
}


 


// The Boundary Mask, 4 point mask:
//     V2-----------V0-----*-----V1-----------V3
//    -1/16        9/16         9/16         -1/16
IFXINLINE void IFXButterflyMask3D::Apply4PointMask (F32 *pV0, F32 *pV1, F32 *pV2, F32 *pV3, F32 *pMidpoint)
{
    // Cast incoming float pointers to appropriate type:
    IFXVector3 *p3D_V0 = (IFXVector3 *) pV0;
    IFXVector3 *p3D_V1 = (IFXVector3 *) pV1;
    IFXVector3 *p3D_V2 = (IFXVector3 *) pV2;
    IFXVector3 *p3D_V3 = (IFXVector3 *) pV3;
    IFXVector3 *p3D_Vr = (IFXVector3 *) pMidpoint;

    p3D_Vr->X() = 0.5625f * (p3D_V0->X() + p3D_V1->X()) - 0.0625f * (p3D_V2->X() + p3D_V3->X());
    p3D_Vr->Y() = 0.5625f * (p3D_V0->Y() + p3D_V1->Y()) - 0.0625f * (p3D_V2->Y() + p3D_V3->Y());
    p3D_Vr->Z() = 0.5625f * (p3D_V0->Z() + p3D_V1->Z()) - 0.0625f * (p3D_V2->Z() + p3D_V3->Z());
}

// A simple average used to handle some anonmalous cases:
//     V0-----*-----V1
//    1/2          1/2

IFXINLINE void IFXButterflyMask3D::Apply2PointMask (F32 *pV0, F32 *pV1, F32 *pMidpoint)
{
    // Cast incoming float pointers to appropriate type:
    IFXVector3 *p3D_V0 = (IFXVector3 *) pV0;
    IFXVector3 *p3D_V1 = (IFXVector3 *) pV1;
    IFXVector3 *p3D_Vr = (IFXVector3 *) pMidpoint;

    p3D_Vr->Add(*p3D_V0, *p3D_V1);
    p3D_Vr->Scale(0.5f);
}




// 3D Butterfly Mask with over-ridden fixup methods for Normals:
class IFXButterflyMask3DNormal : public IFXButterflyMask3D
{
};




// 2D Butterfly Mask:
class IFXButterflyMask2D : public IFXButterflyMask
{
    virtual void IFXAPI  ApplyButterflyMask (F32 **ppMask, F32 *pMidpoint);
    virtual void IFXAPI  ReflectAttributeThroughEdge (F32 *pV0, F32 *pV1, F32 *pV2, F32 *pVr);
    virtual void IFXAPI  Apply4PointMask (F32 *pV0, F32 *pV1, F32 *pV2, F32 *pV3, F32 *pMidpoint);
    virtual void IFXAPI  Apply2PointMask (F32 *pV0, F32 *pV1, F32 *pMidpoint);
    virtual void IFXAPI  ReflectAttributeThroughPointAlongEdge (F32 *pV1, F32 *pV2, F32 *pVr);
};

IFXINLINE void IFXButterflyMask2D::ApplyButterflyMask (F32 **ppMask, F32 *pMidPoint)
{
    // Cast incoming float pointers to appropriate type:
    IFXVector2 **ppV    = (IFXVector2 **) ppMask;
    IFXVector2 *pResult = (IFXVector2 *) pMidPoint;

    pResult->X() = 0.5f * (ppV[0]->X() + ppV[1]->X()) 
						+ m_fSmoothingTimes2 * (ppV[2]->X() + ppV[3]->X())
						- m_fSmoothing * (ppV[4]->X() + ppV[5]->X() + ppV[6]->X() + ppV[7]->X());

	pResult->Y() = 0.5f * (ppV[0]->Y() + ppV[1]->Y()) 
						+ m_fSmoothingTimes2 * (ppV[2]->Y() + ppV[3]->Y())
						- m_fSmoothing * (ppV[4]->Y() + ppV[5]->Y() + ppV[6]->Y() + ppV[7]->Y());
}

// If we are near by a boundary and one of the vertices in the secondary
// support of the subdivision mask is set to null, then we'll fake the
// vertex by reflecting it's neighbor point.
// pV0 is the point to reflect, pV1 and pV2 define the edge across which
// we will reflect.  pVr will contain the reflected result.
// The BFMaskLayout objects will detect this situation and invoke this
// method when it occurs.
IFXINLINE void IFXButterflyMask2D::ReflectAttributeThroughEdge (F32 *pV0, F32 *pV1, F32 *pV2, F32 *pVr)
{
    // Cast incoming float pointers to appropriate IFXVector2 type:
    IFXVector2 *p2D_V0 = (IFXVector2 *)  pV0;
    IFXVector2 *p2D_V1 = (IFXVector2 *)  pV1;
    IFXVector2 *p2D_V2 = (IFXVector2 *)  pV2;
    IFXVector2 *p2D_Vr = (IFXVector2 *)  pVr;

    IFXVector2 u, v;
    u.Subtract(*p2D_V0, *p2D_V1);
    v.Subtract(*p2D_V2, *p2D_V1);
    F32 fDot = u.DotProduct(v);
    
    if (fDot != 0.0f)
    {
        fDot /= v.DotProduct(v);    // Scale it
        u.Scale(fDot);
    }

    u.Add (*p2D_V2);

    // u now represents the perpendicular intersect point.

    v.Subtract (*p2D_V0, u);
    v.Scale (-1.0f);
    p2D_Vr->Add(u, v);
    // pVr now represents the reflected point.
}


IFXINLINE void IFXButterflyMask2D::ReflectAttributeThroughPointAlongEdge (F32 *pV1, F32 *pV2, F32 *pVr)
{
    // Cast the incoming floats to vector 3:
    IFXVector2 *p2D_pV1 = (IFXVector2 *) pV1;
    IFXVector2 *p2D_pV2 = (IFXVector2 *) pV2;
    IFXVector2 *p2D_pVr = (IFXVector2 *) pVr;

    IFXVector2 u;
    u.Subtract(*p2D_pV2, *p2D_pV1);
    p2D_pVr->Add(*p2D_pV2, u);
    
    // p2D_Vr now represents the reflected point.
}


 
// See description for IFXButterflyMask3D::Apply4PointMask
IFXINLINE void IFXButterflyMask2D::Apply4PointMask (F32 *pV0, F32 *pV1, F32 *pV2, F32 *pV3, F32 *pMidpoint)
{
    // Cast incoming float pointers to appropriate type:
    IFXVector2 *p2D_V0 = (IFXVector2 *) pV0;
    IFXVector2 *p2D_V1 = (IFXVector2 *) pV1;
    IFXVector2 *p2D_V2 = (IFXVector2 *) pV2;
    IFXVector2 *p2D_V3 = (IFXVector2 *) pV3;
    IFXVector2 *p2D_Vr = (IFXVector2 *) pMidpoint;

    p2D_Vr->X() = 0.5625f * (p2D_V0->X() + p2D_V1->X()) - 0.0625f * (p2D_V2->X() + p2D_V3->X());
    p2D_Vr->Y() = 0.5625f * (p2D_V0->Y() + p2D_V1->Y()) - 0.0625f * (p2D_V2->Y() + p2D_V3->Y());
}

// A simple average used to handle some anonmalous cases:
//     V0-----*-----V1
//    1/2          1/2
IFXINLINE void IFXButterflyMask2D::Apply2PointMask (F32 *pV0, F32 *pV1, F32 *pMidpoint)
{
    // Cast incoming float pointers to appropriate type:
    IFXVector2 *p2D_V0 = (IFXVector2 *) pV0;
    IFXVector2 *p2D_V1 = (IFXVector2 *) pV1;
    IFXVector2 *p2D_Vr = (IFXVector2 *) pMidpoint;

    p2D_Vr->Add(*p2D_V0, *p2D_V1);
    p2D_Vr->Scale(0.5f);
}

#endif
