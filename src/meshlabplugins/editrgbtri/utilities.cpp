/****************************************************************************
* Rgb Triangulations Plugin                                                 *
*                                                                           *
* Author: Daniele Panozzo (daniele.panozzo@gmail.com)                       *
* Copyright(C) 2007                                                         *
* DISI - Department of Computer Science                                     *
* University of Genova                                                      *
*                                                                           *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *   
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
****************************************************************************/

#include "utilities.h"
#include <vcg/simplex/face/pos.h>
#include <assert.h>
#include <vcg/complex/trimesh/edge_collapse.h>


namespace rgbt
{

Utilities::Utilities()
{
}

Utilities::~Utilities()
{
}

void Utilities::HighlightFF(CMeshO::FacePointer fp)
{
    if (!fp) return;
    fp->FFp(0)->SetS();
    fp->FFp(1)->SetS();
    fp->FFp(2)->SetS();
}

void Utilities::HighlightVF(CMeshO::VertexPointer vp)
{
    if (!vp) return;
    vcg::face::VFIterator<CMeshO::FaceType> vf(vp);
    CMeshO::FacePointer first = vf.F();
    first->SetS();
    ++vf;
    while(vf.F() && (vf.F() != first))
    {
        vf.F()->SetS();
        ++vf;
    }
    
}

void Utilities::HighlightVFPos(CMeshO::FacePointer fp, int i)
{
    if (!fp) return;
    assert(i>= 0 && i<=2);
    vcg::face::Pos<CMeshO::FaceType> pos(fp,fp->V(i));
    CMeshO::FacePointer first = pos.F(); 
    pos.F()->SetS();
    pos.FlipE();
    pos.FlipF();
    while(pos.F() && (pos.F() != first))
    {
        pos.F()->SetS();
        pos.FlipE();
        pos.FlipF();
    }
    

}

void Utilities::doCollapse(CMeshO &m, EdgeFIType & c, const PointType &p)
{
    TopologicalOp<CMeshO> top(m);
    top.doCollapse<false>(c,&p);
    
}

void Utilities::doSplit(CMeshO &m, EdgeFIType & c, const PointType &p)
{
    TopologicalOp<CMeshO> top(m);
    top.doSplit<false>(c,p);
    
}

bool Utilities::commonEdge(CMeshO::FacePointer fp1, CMeshO::FacePointer fp2, Utilities::EdgeFIType* edge)
{
    assert(fp1);
    assert(fp2);
    
    for (int i = 0; i < 3; ++i) {
        if (fp1->FFp(i) == fp2)
        {
            if (edge)
                *edge = EdgeFIType(fp1,i);
            return true;
        }
    }
    return false;
}

bool Utilities::commonVertex(vector<FacePointer> fc, Utilities::EdgeFIType* vert)
{
    if (fc.size() < 2)
        return false;
    
    FacePointer f = fc[0];
    
    for (int i = 0; i < 3; ++i) 
    {
        VertexPointer v = f->V(i);
        vector<FacePointer>::iterator it = fc.begin();
        vector<FacePointer>::iterator end = fc.end();
        ++it; // skip the first
        bool isCommon = true;
        for (; it != end; ++it) 
        {
            bool isInTriangle = false;
            for (int j = 0; j < 3; ++j) 
            {
                
                if ((*it)->V(j) == v)
                    isInTriangle = true;
            }
            if (!isInTriangle)
                isCommon = false;
            
        }
        
        if (isCommon)
        {
            if(vert)
            {
                vert->fp = f;
                vert->i = i;
            }
            return true;
        }
    }
    
    return false;
}

}
