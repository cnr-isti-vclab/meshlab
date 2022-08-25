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


#include "rgbPrimitives.h"
#include <algorithm>
#include <topologicalOp.h>

namespace rgbt
{

vector<FaceInfo::FaceColor>* RgbPrimitives::r4p = 0;
vector<FaceInfo::FaceColor>* RgbPrimitives::r2gb1p = 0;
vector<FaceInfo::FaceColor>* RgbPrimitives::r2gb2p = 0;
vector<FaceInfo::FaceColor>* RgbPrimitives::gbgb1p = 0;
vector<FaceInfo::FaceColor>* RgbPrimitives::gbgb2p = 0;
vector<FaceInfo::FaceColor>* RgbPrimitives::g2b21p = 0;
vector<FaceInfo::FaceColor>* RgbPrimitives::g2b22p = 0;

vector<FaceInfo::FaceColor>* RgbPrimitives::s6gp = 0;
vector<FaceInfo::FaceColor>* RgbPrimitives::s4g1bggr = 0;
vector<FaceInfo::FaceColor>* RgbPrimitives::s4g1brgg = 0;
vector<FaceInfo::FaceColor>* RgbPrimitives::s3g2rp = 0;

RgbPrimitives::subtype RgbPrimitives::stype = MODBUTFLY;

bool RgbPrimitives::triangleVertexCorrectness(RgbTriangleC& t)
{
    int vl[3];
    vl[0] = t.getVl(0);
    vl[1] = t.getVl(1);
    vl[2] = t.getVl(2);
    int l = t.getFaceLevel();
    std::sort(vl,vl+3);

    switch (t.getFaceColor())
    {
    case FaceInfo::FACE_BLUE_GGR:
    case FaceInfo::FACE_BLUE_RGG:
        return ((vl[0] <= l) && (vl[1] == l+1) && (vl[2] == l+1));
        break;
    case FaceInfo::FACE_GREEN:
        return ((vl[0] <= l) && (vl[1] <= l) && (vl[2] <= l));
        break;
    case FaceInfo::FACE_RED_GGR:
    case FaceInfo::FACE_RED_RGG:
        return ((vl[0] <= l) && (vl[1] <= l) && (vl[2] == l+1));
        break;
    }
    return false;
}

bool RgbPrimitives::triangleAdjCorrectness(RgbTriangleC& t)
{
    for (int i = 0; i <= 2; ++i)
    {
        if (t.getEdgeColor(i) != t.FF(i).getEdgeColor(t.FFi(i)) ||t.getEdgeLevel(i) != t.FF(i).getEdgeLevel(t.FFi(i)))
            return false;
    }
    return true;
}

bool RgbPrimitives::triangleVertexAngleCorrectness(RgbTriangleC& t)
{
	bool res = true;
	for (int i=0; i<3;i++)
	{
		RgbVertexC& v = t.V(i);
		if (!v.getIsBorder())
		{
			int rank = ModButterfly::baseArity(v);
			Pos p = Pos(t.face(),i);
			ModButterfly::rotate(v,p,2*rank);
			
			assert(p.v == v.vp());
			assert(p.f == t.face());
			assert(p.z == i);
			res == res && (p.v == v.vp()) && (p.f == t.face()) && (p.z == i);
		}
	}
	return res;
}

bool RgbPrimitives::triangleCorrectness(RgbTriangleC& t)
{
    bool a = triangleAdjCorrectness(t);
    bool v = triangleVertexCorrectness(t);
    bool angle = triangleVertexAngleCorrectness(t);
    return a && v && angle;
}

bool RgbPrimitives::gg_Split_Possible(RgbTriangleC& t, int EdgeIndex)
{
    if (t.getEdgeIsBorder(EdgeIndex))
        return false;   // edge is on the border
    
    assert(triangleCorrectness(t));
    RgbTriangleC t2 = t.FF(EdgeIndex);
    assert(triangleCorrectness(t2));

    return
    (
            
            (t.getFaceColor() == FaceInfo::FACE_GREEN) && // t is green
            (t2.getFaceColor() == FaceInfo::FACE_GREEN) && // t2 is green
            (t.getFaceLevel() == t2.getFaceLevel()) // t is at the same level of t2
    );

}
bool RgbPrimitives::rg_Split_Possible(RgbTriangleC& t, int EdgeIndex)
{
    if (t.getEdgeIsBorder(EdgeIndex))
        return false;   // edge is on the border
    assert(triangleCorrectness(t));
    RgbTriangleC t2 = t.FF(EdgeIndex);
    assert(triangleCorrectness(t2));
    
    return
    (
            (
                    (
                            (t.getFaceColor() == FaceInfo::FACE_GREEN) &&
                            (
                                    (t2.getFaceColor() == FaceInfo::FACE_RED_GGR) ||
                                    (t2.getFaceColor() == FaceInfo::FACE_RED_RGG)
                            )
                    )// t is green and t2 is red
                    ||
                    (
                            (
                                    (t.getFaceColor() == FaceInfo::FACE_RED_GGR) ||
                                    (t.getFaceColor() == FaceInfo::FACE_RED_RGG)
                            ) 
                            &&
                            (t2.getFaceColor() == FaceInfo::FACE_GREEN)
                    )// t2 is green and t is red
            )
            &&
            (t.getFaceLevel() == t2.getFaceLevel()) // t and t2 are at the same level
            &&
            (t.getEdgeColor(EdgeIndex) == FaceInfo::EDGE_GREEN)
    );

}

bool RgbPrimitives::rr_Split_Possible(RgbTriangleC& t, int EdgeIndex)
{
    if (t.getEdgeIsBorder(EdgeIndex))
        return false;   // edge is on the border
    assert(triangleCorrectness(t));
    RgbTriangleC t2 = t.FF(EdgeIndex);
    assert(triangleCorrectness(t2));
    
    return
    (
            (
                    (t.getFaceColor() == FaceInfo::FACE_RED_GGR) 
                    ||
                    (t.getFaceColor() == FaceInfo::FACE_RED_RGG)
            ) // t is red
            &&
            (

                    (t2.getFaceColor() == FaceInfo::FACE_RED_GGR) 
                    ||
                    (t2.getFaceColor() == FaceInfo::FACE_RED_RGG)
            ) // t2 is red
            &&
            (t.getFaceLevel() == t2.getFaceLevel()) // t and t2 are at the same level 
            &&
            (t.getEdgeColor(EdgeIndex) == FaceInfo::EDGE_GREEN) // edge is green
            &&
            (t.getEdgeLevel(EdgeIndex) == t.getFaceLevel()) // edge is at correct level
    );

}

bool RgbPrimitives::edgeSplit_Possible(RgbTriangleC& t, int EdgeIndex)
{
	if (!t.getEdgeIsBorder(EdgeIndex))
    return 
    (
            gg_Split_Possible(t,EdgeIndex) ||
            rg_Split_Possible(t,EdgeIndex) ||
            rr_Split_Possible(t,EdgeIndex)
    );
	else
	return
	(
	        b_g_Bisection_Possible(t,EdgeIndex) ||
	        b_r_Bisection_Possible(t,EdgeIndex)
	);
}

bool RgbPrimitives::doSplit(RgbTriangleC& fp, int EdgeIndex, int level, TopologicalOpC& to, vector<FacePointer> *vfp, RgbVertexC* vNewInserted, vector<RgbVertexC>* vcont, vector<RgbVertexC>* vupd)
{
	switch (stype)
	{
	case LOOP:
		return ControlPoint::doSplit(fp, EdgeIndex, level, to, vfp, vNewInserted, vcont, vupd);
	case MODBUTFLY:
		return ModButterfly::doSplit(fp, EdgeIndex, level, to, vfp);
	default:
		return false;		
	}
}

void RgbPrimitives::doCollapse(RgbTriangleC& fp, int EdgeIndex, TopologicalOpC& to, Point3<ScalarType> *p, vector<FacePointer> *vfp)
{
	switch (stype)
	{
	case LOOP:
		ControlPoint::doCollapse(fp, EdgeIndex, to, p, vfp);
		break;
	case MODBUTFLY:
		ModButterfly::doCollapse(fp, EdgeIndex, to, p, vfp);
		break;
	}
}

void RgbPrimitives::distributeContribute(vector<RgbVertexC>& vCont,RgbVertexC& vNew,vector<RgbVertexC>& vUpd)
{
    for (unsigned int i = 0; i < vCont.size(); ++i) 
    {
    	ControlPoint::addContributeIfPossible(vNew,vCont[i]);
	}
    for (unsigned int i = 0; i < vCont.size(); ++i) 
    {
    	ControlPoint::addContributeIfPossible(vCont[i],vNew);
	}
    for (unsigned int i = 0; i < vUpd.size(); ++i) 
    {
    	ControlPoint::updateP(vUpd[i]);
	}
}

void RgbPrimitives::gg_Split(RgbTriangleC& t, int EdgeIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt)
{
    assert(gg_Split_Possible(t,EdgeIndex));
    int l = t.getFaceLevel();
    
    // Store the face obtained with the split
    vector<FacePointer> vfp;
    // Execute the split
    RgbVertexC vNew;
    vector<RgbVertexC> vCont;
    vector<RgbVertexC> vUpd;

    bool todo = RgbPrimitives::doSplit(t,EdgeIndex,l+1,to,&vfp,&vNew,&vCont,&vUpd);
    
    if (!todo)
    	return; // The update on rgb is already done by doSplit

    RgbTriangleC t0 = RgbTriangleC(t.m,t.rgbInfo,vfp[0]->Index());
    RgbTriangleC t1 = RgbTriangleC(t.m,t.rgbInfo,vfp[1]->Index());
    RgbTriangleC t2 = RgbTriangleC(t.m,t.rgbInfo,vfp[2]->Index());
    RgbTriangleC t3 = RgbTriangleC(t.m,t.rgbInfo,vfp[3]->Index());

    g_Bisection(l,t0,t2);
    g_Bisection(l,t3,t1);
    
    assert(triangleCorrectness(t0)); 
    assert(triangleCorrectness(t1));
    assert(triangleCorrectness(t2)); 
    assert(triangleCorrectness(t3));
    
    if (vt)
    {
    	vt->push_back(t0);
    	vt->push_back(t1);
    	vt->push_back(t2);
    	vt->push_back(t3);
    }

    if (stype == LOOP)
    	distributeContribute(vCont,vNew,vUpd);
}

void RgbPrimitives::rg_Split(RgbTriangleC& t, int EdgeIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt)
{
    assert(rg_Split_Possible(t,EdgeIndex));
    int l = t.getFaceLevel();

    // Search the green triangle
    RgbTriangleC* tp = &t;
    int ti = EdgeIndex;
    RgbTriangleC ot = t.FF(EdgeIndex);
    int oti = t.FFi(EdgeIndex);
    
    // The color of the red triangle
    FaceInfo::FaceColor redtype;
    // contain the 2 indexes of the vertexes of the red edge
    VertexPair vp;
    if (tp->getFaceColor() == FaceInfo::FACE_RED_GGR || tp->getFaceColor() == FaceInfo::FACE_RED_RGG)
    {
        redtype = tp->getFaceColor();
        vp = tp->getRedEdge();
        tp = &ot;
        ti = oti;
    }
    else
    {
        redtype = ot.getFaceColor();
        vp = ot.getRedEdge();
    }
    
    // Store the face obtained with the split
    vector<FacePointer> vfp;
    
    RgbVertexC vNew;
    vector<RgbVertexC> vCont;
    vector<RgbVertexC> vUpd;
    
    // Execute the split
    bool todo = RgbPrimitives::doSplit(*tp,ti,l+1,to,&vfp,&vNew,&vCont,&vUpd);
    if (!todo)
    	return; // The update on rgb is already done by doSplit
    
    RgbTriangleC t0 = RgbTriangleC(t.m,t.rgbInfo,vfp[0]->Index());
    
    RgbTriangleC t1 = RgbTriangleC(t.m,t.rgbInfo,vfp[1]->Index());
    RgbTriangleC t2 = RgbTriangleC(t.m,t.rgbInfo,vfp[2]->Index());
    RgbTriangleC t3 = RgbTriangleC(t.m,t.rgbInfo,vfp[3]->Index());
    
    g_Bisection(l,t0,t2);
    r_Bisection(l,redtype,t1,t3,vp);
    
    assert(triangleCorrectness(t0));
    assert(triangleCorrectness(t1));
    assert(triangleCorrectness(t2)); 
    assert(triangleCorrectness(t3));

    if (vt)
    {
    	vt->push_back(t0);
    	vt->push_back(t1);
    	vt->push_back(t2);
    	vt->push_back(t3);
    }

    
    if (t1.isBlue())
    {
        assert(!t3.isBlue());
        bb_Swap_If_Needed(t1,vt);
    }
    else
    {
        assert(!t1.isBlue());
        bb_Swap_If_Needed(t3,vt);
    }
    
    if (stype == LOOP)
    	distributeContribute(vCont,vNew,vUpd);
    return;
}

void RgbPrimitives::rr_Split(RgbTriangleC& t, int EdgeIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt)
{
    assert(rr_Split_Possible(t,EdgeIndex));
    int l = t.getFaceLevel();
    
    // get the other triangle
    RgbTriangleC ottemp = t.FF(EdgeIndex);
    int otitemp = t.FFi(EdgeIndex);
    
    RgbTriangleC* tp;
    int ti;
    RgbTriangleC* otp;
    int oti;
    
    tp = &t;
    ti = EdgeIndex;
    otp = &ottemp;
    oti = otitemp;

    VertexPair vp = tp->getRedEdge();
    VertexPair ovp = otp->getRedEdge();

    FaceInfo::FaceColor redtypeu = tp->getFaceColor();
    FaceInfo::FaceColor redtypel = otp->getFaceColor();
    
    // Store the face obtained with the split
    vector<FacePointer> vfp;
    // Execute the split
    RgbVertexC vNew;
    vector<RgbVertexC> vCont;
    vector<RgbVertexC> vUpd;
    
    bool todo = RgbPrimitives::doSplit(*tp,ti,l+1,to,&vfp,&vNew,&vCont,&vUpd);
    if (!todo)
    	return; // The update on rgb is already done by doSplit

    
    RgbTriangleC t0 = RgbTriangleC(t.m,t.rgbInfo,vfp[0]->Index());
    
    RgbTriangleC t1 = RgbTriangleC(t.m,t.rgbInfo,vfp[1]->Index());
    RgbTriangleC t2 = RgbTriangleC(t.m,t.rgbInfo,vfp[2]->Index());
    RgbTriangleC t3 = RgbTriangleC(t.m,t.rgbInfo,vfp[3]->Index());

    r_Bisection(l,redtypeu,t2,t0,vp);
    r_Bisection(l,redtypel,t1,t3,ovp);
    
    assert(triangleCorrectness(t0));
    assert(triangleCorrectness(t1));
    assert(triangleCorrectness(t2)); 
    assert(triangleCorrectness(t3));

    vector<RgbTriangleC*> vb;
    
    if (t0.isBlue()) 
        vb.push_back(&t0);
    if (t1.isBlue()) 
        vb.push_back(&t1);
    if (t2.isBlue()) 
        vb.push_back(&t2);
    if (t3.isBlue()) 
        vb.push_back(&t3);
    
    assert(vb.size() == 2);

    if (vt)
    {
    	vt->push_back(t0);
    	vt->push_back(t1);
    	vt->push_back(t2);
    	vt->push_back(t3);
    }
    
    
    bb_Swap_If_Needed(*vb[0],vt);
    bb_Swap_If_Needed(*vb[1],vt);

    if (stype == LOOP)
    	distributeContribute(vCont,vNew,vUpd);
    return;
}

bool RgbPrimitives::edgeSplit(RgbTriangleC& t, int EdgeIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt)
{
	RgbVertexC v1 = t.V(EdgeIndex);
	RgbVertexC v2 = t.V((EdgeIndex+1)%3);
	int l = t.getFaceLevel();
	
	RgbTriangleC t2;
	int ti2;
	
	if ((stype == LOOP) && !t.getEdgeIsBorder(EdgeIndex))
	    ControlPoint::findInitialStencil(t,EdgeIndex,l+1,to);
	
	if (!IsValidEdge(v1,v2,&t2,&ti2)) 
		return true; // The split is already done (by findInitialStencil)
	
	if (!t.getEdgeIsBorder(EdgeIndex))
    	{
        if (gg_Split_Possible(t2,ti2))
        {
            gg_Split(t2,ti2,to,vt);
        } 
        else
        if (rg_Split_Possible(t2,ti2))
        {
            rg_Split(t2,ti2,to,vt);
        }
        else
        if (rr_Split_Possible(t2,ti2))
        {
            rr_Split(t2,ti2,to,vt);
        }
	}
	else
	{
        if (b_g_Bisection_Possible(t2,ti2))
        {
            b_g_Bisection(t2,ti2,to,vt);
        } 
        else
        if (b_r_Bisection_Possible(t2,ti2))
        {
            b_r_Bisection(t2,ti2,to,vt);
        } 

	}
	
    if (!IsValidEdge(v1,v2,&t2,&ti2)) 
        return true; // The split is already done
    else
        return false;

}

bool RgbPrimitives::bb_Swap_Possible(RgbTriangleC& t, int EdgeIndex)
{
    if (t.getEdgeIsBorder(EdgeIndex))
        return false;   // edge is on the border

    RgbTriangleC ot = t.FF(EdgeIndex);
    assert(triangleCorrectness(t));
    assert(triangleCorrectness(ot));
    return
    (
            (t.getFaceLevel() == ot.getFaceLevel()) &&
            ((t.getFaceColor() == FaceInfo::FACE_BLUE_GGR) || (t.getFaceColor() == FaceInfo::FACE_BLUE_RGG)) &&
            ((ot.getFaceColor() == FaceInfo::FACE_BLUE_GGR) || (ot.getFaceColor() == FaceInfo::FACE_BLUE_RGG)) &&
            (t.getEdgeColor(EdgeIndex) == FaceInfo::EDGE_RED) &&
            rgbt::CheckFlipEdge(*(t.face()),EdgeIndex)
    );
    
}

void RgbPrimitives::bb_Swap(RgbTriangleC& t, int EdgeIndex, vector<RgbTriangleC>* vt)
{
    assert(bb_Swap_Possible(t,EdgeIndex));
    int l = t.getFaceLevel();
    RgbTriangleC ot = t.FF(EdgeIndex);
    
    rgbt::FlipEdge(*(t.face()),EdgeIndex);

    // t and ot are not consistent with the color of edge and level of edge but
    // at the first setFaceColor all the data is recalculated

    t.setFaceColor(FaceInfo::FACE_GREEN);
    ot.setFaceColor(FaceInfo::FACE_GREEN);
    t.setFaceLevel(l+1);
    ot.setFaceLevel(l+1);
    
    assert(triangleCorrectness(t));
    assert(triangleCorrectness(ot));

    if (vt)
    {
    	vt->push_back(t);
    	vt->push_back(ot);
    }

}

void RgbPrimitives::bb_Swap_If_Needed(RgbTriangleC& t, vector<RgbTriangleC>* vt)
{
    // Search for the red edge on the blue face
    for (int i = 0; i < 3; ++i) 
    {
        if (t.getEdgeColor(i) == FaceInfo::EDGE_RED)
        {
            if (bb_Swap_Possible(t,i))
                bb_Swap(t,i,vt);
        }
        
    }
}

void RgbPrimitives::g_Bisection(int level, RgbTriangleC& rgg, RgbTriangleC& ggr)
{
    // Set new colors
    rgg.setFaceColor(FaceInfo::FACE_RED_RGG);
    ggr.setFaceColor(FaceInfo::FACE_RED_GGR);

    // Set new levels
    rgg.setFaceLevel(level);
    ggr.setFaceLevel(level);
}

void RgbPrimitives::r_Bisection(int level,FaceInfo::FaceColor color , RgbTriangleC& t1, RgbTriangleC& t2, VertexPair vp)
{
    assert(color == FaceInfo::FACE_RED_GGR || color == FaceInfo::FACE_RED_RGG);
    assert(t1.containEdge(vp) || t2.containEdge(vp));
    RgbTriangleC* green;
    RgbTriangleC* blue;
    
    if (t1.containEdge(vp))
    {
        green = &t2;
        blue = &t1;
    }
    else
    {
        green = &t1;
        blue = &t2;
    }
    
    // Set new colors
    green->setFaceColor(FaceInfo::FACE_GREEN);
    if (color == FaceInfo::FACE_RED_RGG)
        blue->setFaceColor(FaceInfo::FACE_BLUE_GGR);
    else
        blue->setFaceColor(FaceInfo::FACE_BLUE_RGG);

    // Set new levels
    green->setFaceLevel(level+1);
    blue->setFaceLevel(level);
    
}

void RgbPrimitives::vf(RgbTriangleC& t, int VertexIndex, vectorRgbTriangle& fc)
{
    assert(VertexIndex>= 0 && VertexIndex<=2);
    assert(!t.face()->IsD());
    assert(!t.face()->V(VertexIndex)->IsD());
    
    bool isBorder = t.getVertexIsBorder(VertexIndex);
    fc.reserve(fc.size()+10);
    vcg::face::Pos<FaceType> pos(t.face(),t.face()->V(VertexIndex));

    if (t.getNumberOfBoundaryEdge(&(t.V(VertexIndex))) >= 2)
    {
    	fc.push_back(t);
    	return;
    }
    	
    if (isBorder)       // if is border move cw until the border is found
    {
        pos.FlipE();
        pos.FlipF();
        
        while (!pos.IsBorder())
        {
            pos.FlipE();
            pos.FlipF();
        }
        
        pos.FlipE();
    }
    
    CMeshO::FacePointer first = pos.F();
    
    fc.push_back(RgbTriangleC(t.m,t.rgbInfo,pos.F()->Index()));
    pos.FlipF();
    pos.FlipE();
 
    
    
    while(pos.F() != first)
    {
        fc.push_back(RgbTriangleC(t.m,t.rgbInfo,pos.F()->Index()));
        
        if (pos.IsBorder())
            break;
        
        pos.FlipF();
        pos.FlipE();
    }
    
    int indexV = t.getVIndex(VertexIndex);
    int res;
    for (unsigned int i = 0; i < fc.size(); ++i) 
    {
        assert(fc[i].containVertex(indexV,&res));
        if (!isBorder)
        {
            assert(fc[i].FF((res+2)%3).face() == fc[(i+1)%fc.size()].face());
        }
        assert(!fc[i].face()->IsD());
    }
    
}


bool RgbPrimitives::r4_Merge_Possible(RgbTriangleC& t, int VertexIndex)
{
    if (t.V(VertexIndex).getIsBorder())
        return false;   // Vertex is on the border
    
    assert(VertexIndex>=0 && VertexIndex <= 2);
    if (!r4p)
    {
        r4p = new vector<FaceInfo::FaceColor>(4);
        (*r4p)[0] = FaceInfo::FACE_RED_RGG;
        (*r4p)[1] = FaceInfo::FACE_RED_GGR;
        (*r4p)[2] = FaceInfo::FACE_RED_RGG;
        (*r4p)[3] = FaceInfo::FACE_RED_GGR;
    }
    vectorRgbTriangle fc;
    vf(t,VertexIndex,fc);
    vectorFaceColor vcolor;
    extractColor(fc,vcolor);
    return isMatch(vcolor,*r4p);
}
bool RgbPrimitives::r2gb_Merge_Possible(RgbTriangleC& t, int VertexIndex)
{
    if (t.V(VertexIndex).getIsBorder())
        return false;   // Vertex is on the border

    assert(VertexIndex>=0 && VertexIndex <= 2);
    
    if (!r2gb1p)
    {
        r2gb1p = new vector<FaceInfo::FaceColor>(4);
        (*r2gb1p)[0] = FaceInfo::FACE_RED_GGR;
        (*r2gb1p)[1] = FaceInfo::FACE_RED_RGG;
        (*r2gb1p)[2] = FaceInfo::FACE_GREEN;
        (*r2gb1p)[3] = FaceInfo::FACE_BLUE_GGR;
    }

    if (!r2gb2p)
    {
        r2gb2p = new vector<FaceInfo::FaceColor>(4);
        (*r2gb2p)[0] = FaceInfo::FACE_RED_GGR;
        (*r2gb2p)[1] = FaceInfo::FACE_RED_RGG;
        (*r2gb2p)[2] = FaceInfo::FACE_BLUE_RGG;
        (*r2gb2p)[3] = FaceInfo::FACE_GREEN;
    }
    vectorRgbTriangle fc;
    vf(t,VertexIndex,fc);
    vectorFaceColor vcolor;
    extractColor(fc,vcolor);
    
    return (isMatch(vcolor,*r2gb1p) || isMatch(vcolor,*r2gb2p));
}
bool RgbPrimitives::gbgb_Merge_Possible(RgbTriangleC& t, int VertexIndex)
{
    if (t.V(VertexIndex).getIsBorder())
        return false;   // Vertex is on the border

    assert(VertexIndex>=0 && VertexIndex <= 2);
    
    if (!gbgb1p)
    {
        gbgb1p = new vector<FaceInfo::FaceColor>(4);
        (*gbgb1p)[0] = FaceInfo::FACE_GREEN;
        (*gbgb1p)[1] = FaceInfo::FACE_BLUE_GGR;
        (*gbgb1p)[2] = FaceInfo::FACE_GREEN;
        (*gbgb1p)[3] = FaceInfo::FACE_BLUE_GGR;
    }
    
    if (!gbgb2p)
    {
        gbgb2p = new vector<FaceInfo::FaceColor>(4);
        (*gbgb2p)[0] = FaceInfo::FACE_GREEN;
        (*gbgb2p)[1] = FaceInfo::FACE_BLUE_RGG;
        (*gbgb2p)[2] = FaceInfo::FACE_GREEN;
        (*gbgb2p)[3] = FaceInfo::FACE_BLUE_RGG;
    }
    
    vectorRgbTriangle fc;
    vf(t,VertexIndex,fc);
    vectorFaceColor vcolor;
    extractColor(fc,vcolor);
    return (isMatch(vcolor,*gbgb1p) || isMatch(vcolor,*gbgb2p));
}
bool RgbPrimitives::g2b2_Merge_Possible(RgbTriangleC& t, int VertexIndex)
{
    if (t.V(VertexIndex).getIsBorder())
        return false;   // Vertex is on the border

    assert(VertexIndex>=0 && VertexIndex <= 2);
    
    if (!g2b21p)
    {
        g2b21p = new vector<FaceInfo::FaceColor>(4);
        (*g2b21p)[0] = FaceInfo::FACE_BLUE_GGR;
        (*g2b21p)[1] = FaceInfo::FACE_GREEN;
        (*g2b21p)[2] = FaceInfo::FACE_GREEN;
        (*g2b21p)[3] = FaceInfo::FACE_BLUE_RGG;
    }
    
    if (!g2b22p)
    {
        g2b22p = new vector<FaceInfo::FaceColor>(4);
        (*g2b22p)[0] = FaceInfo::FACE_BLUE_RGG;
        (*g2b22p)[1] = FaceInfo::FACE_GREEN;
        (*g2b22p)[2] = FaceInfo::FACE_GREEN;
        (*g2b22p)[3] = FaceInfo::FACE_BLUE_GGR;
    }
    vectorRgbTriangle fc;
    vf(t,VertexIndex,fc);
    vectorFaceColor vcolor;
    extractColor(fc,vcolor);
    return (isMatch(vcolor,*g2b21p) || isMatch(vcolor,*g2b22p));
}
bool RgbPrimitives::gg_Swap_Possible(RgbTriangleC& t, int VertexIndex)
{
    return 
    (
            gg_Swap_6g_Possible(t,VertexIndex) ||
            gg_Swap_4g1b_Possible(t,VertexIndex) ||
            gg_Swap_3g2r_Possible(t,VertexIndex)
    );
}

bool RgbPrimitives::vertexRemoval_Possible(RgbTriangleC& t, int VertexIndex)
{
	if (t.getVl(VertexIndex) <= 0)
		return false;
	
	if (!t.V(VertexIndex).getIsBorder())
        return 
        (
                r4_Merge_Possible(t,VertexIndex) ||
                r2gb_Merge_Possible(t,VertexIndex) ||
                gbgb_Merge_Possible(t,VertexIndex) ||
                g2b2_Merge_Possible(t,VertexIndex) ||
                gg_Swap_Possible(t,VertexIndex) ||
                brb2g_Swap_Possible(t,VertexIndex)
        );
	else
	    return
	    (
	            b_r2_Merge_Possible(t,VertexIndex) ||
	            b_gb_Merge_Possible(t,VertexIndex)
	    );
}

void RgbPrimitives::r4_Merge(RgbTriangleC& t, int VertexIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt)
{
    assert(VertexIndex>=0 && VertexIndex <= 2);
    assert(r4_Merge_Possible(t,VertexIndex));
    
    vectorRgbTriangle fc;
    vf(t,VertexIndex,fc);
    assert(fc.size() == 4);
    vectorFaceColor vcolor;
    extractColor(fc,vcolor);
    
    int k = findColorIndex(vcolor,FaceInfo::FACE_RED_GGR);
    
    RgbTriangleC* f0 = &fc[(k+0)%4];
    assert(f0->getFaceColor() == FaceInfo::FACE_RED_GGR);
    int l = f0->getFaceLevel();
    RgbTriangleC* f1 = &fc[(k+1)%4];
    RgbTriangleC* f2 = &fc[(k+2)%4];
    RgbTriangleC* f3 = &fc[(k+3)%4];
    
    int mi = f0->maxLevelEdge();
    RgbTriangleC rgbtemp = f0->FF(mi);
    int rgbtempi = f0->face()->FFi(mi);
    RgbPrimitives::doCollapse(rgbtemp,rgbtempi,to);
    
    f1->setFaceColor(FaceInfo::FACE_GREEN,false);
    f2->setFaceColor(FaceInfo::FACE_GREEN,false);
    f1->setFaceLevel(l);
    f2->setFaceLevel(l);
    
    assert(triangleCorrectness(*f1));
    assert(triangleCorrectness(*f2));
    
    if (vt)
    {
    	vt->push_back(*f1);
    	vt->push_back(*f2);
    }
    
    assert(f0->face()->IsD());
    assert(!f1->face()->IsD());
    assert(!f2->face()->IsD());
    assert(f3->face()->IsD());
}
void RgbPrimitives::r2gb_Merge(RgbTriangleC& t, int VertexIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt)
{
    assert(VertexIndex>=0 && VertexIndex <= 2);
    assert(r2gb_Merge_Possible(t,VertexIndex));
    
    vectorRgbTriangle fc;
    vf(t,VertexIndex,fc);
    assert(fc.size() == 4);
    vectorFaceColor vcolor;
    extractColor(fc,vcolor);
    
    int k = findColorIndex(vcolor,FaceInfo::FACE_RED_GGR);
    
    RgbTriangleC* f0 = &fc[(k+0)%4];
    assert(f0->getFaceColor() == FaceInfo::FACE_RED_GGR);
    int l = f0->getFaceLevel();
    RgbTriangleC* f1 = &fc[(k+1)%4];
    RgbTriangleC* f2 = &fc[(k+2)%4];
    RgbTriangleC* f3 = &fc[(k+3)%4];
     
    assert((f2->getFaceColor() == FaceInfo::FACE_GREEN && f3->getFaceColor() == FaceInfo::FACE_BLUE_GGR) || (f3->getFaceColor() == FaceInfo::FACE_GREEN && f2->getFaceColor() == FaceInfo::FACE_BLUE_RGG ) );
    
    bool isr2gb1 = f2->getFaceColor() == FaceInfo::FACE_GREEN;
    
    int mi = f0->maxLevelEdge();
    RgbTriangleC rgbtemp = f0->FF(mi);
    int rgbtempi = f0->face()->FFi(mi);
    RgbPrimitives::doCollapse(rgbtemp,rgbtempi,to);
    
    if (isr2gb1)
    {
        f1->setFaceColor(FaceInfo::FACE_GREEN,false);
        f2->setFaceColor(FaceInfo::FACE_RED_RGG,false);
    }
    else
    {
        f1->setFaceColor(FaceInfo::FACE_GREEN,false);
        f2->setFaceColor(FaceInfo::FACE_RED_GGR,false);
    }
    
    f1->setFaceLevel(l);
    f2->setFaceLevel(l);
    
    assert(triangleCorrectness(*f1));
    assert(triangleCorrectness(*f2));

    if (vt)
    {
    	vt->push_back(*f1);
    	vt->push_back(*f2);
    }
    
    assert(f0->face()->IsD());
    assert(!f1->face()->IsD());
    assert(!f2->face()->IsD());
    assert(f3->face()->IsD());
}
void RgbPrimitives::gbgb_Merge(RgbTriangleC& t, int VertexIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt)
{
    assert(VertexIndex>=0 && VertexIndex <= 2);
    assert(gbgb_Merge_Possible(t,VertexIndex));
    
    vectorRgbTriangle fc;
    vf(t,VertexIndex,fc);
    assert(fc.size() == 4);
    vectorFaceColor vcolor;
    extractColor(fc,vcolor);
    
    // level of one of the blue triangles
    int l;
    
    RgbTriangleC* f0;
    RgbTriangleC* f1;
    RgbTriangleC* f2;
    RgbTriangleC* f3;
    
    int k = findColorIndex(vcolor,FaceInfo::FACE_GREEN);
    FaceInfo::FaceColor bluetype = fc[(k+1)%4].getFaceColor();
    // If the blue faces are RGG we need to take the faces in ccw order else in cw order 
    if (bluetype == FaceInfo::FACE_BLUE_RGG) 
    {
        // gbgb-2 merge
        f0 = &fc[(k+0)%4];
        assert(f0->getFaceColor() == FaceInfo::FACE_GREEN);
        
        f1 = &fc[(k+1)%4];
        l = f1->getFaceLevel();
        f2 = &fc[(k+2)%4];
        f3 = &fc[(k+3)%4];
    }
    else
    {
        // gbgb-1 merge
        f0 = &fc[(k+4)%4]; // = k-0 % 4
        assert(f0->getFaceColor() == FaceInfo::FACE_GREEN);
        
        f1 = &fc[(k+3)%4]; // = k-1 % 4
        l = f1->getFaceLevel();
        f2 = &fc[(k+2)%4]; // = k-2 % 4
        f3 = &fc[(k+1)%4]; // = k-3 % 4
    }
    
    assert(f0->isGreen());
    assert(f1->isBlue());
    assert(f2->isGreen());
    assert(f3->isBlue());
    
    assert(f1->getFaceColor() == f3->getFaceColor());
        
    int mi = f3->minLevelVertex();
    
    
    if (bluetype == FaceInfo::FACE_BLUE_RGG)
    {
        // gbgb-2 merge
        mi = (mi+2) % 3;
    }
    else
    {
        // gbgb-1 merge
        mi = (mi) % 3;
    }
    
    
    // The collapse must be performed on the green triangle if gbgb-2 merge
    // and on blue if is gbgb-1 merge:
    // this delete the correct point, if the collapse is performed
    // on the blue the point deleted is not one at level l+1
    if (bluetype == FaceInfo::FACE_BLUE_RGG)
    {
        // gbgb-2 merge
        RgbTriangleC rgbtemp = f3->FF(mi);
        int rgbtempi = f3->face()->FFi(mi);
        RgbPrimitives::doCollapse(rgbtemp,rgbtempi,to);
    }
    else
    {
        // gbgb-1 merge
    	RgbPrimitives::doCollapse(*f3,mi,to);
    }

    gb_Merge(l,bluetype,*f0);
    gb_Merge(l,bluetype,*f1);
    
    assert(triangleCorrectness(*f0));
    assert(triangleCorrectness(*f1));

    if (vt)
    {
    	vt->push_back(*f0);
    	vt->push_back(*f1);
    }
    
    assert(!f0->face()->IsD());
    assert(!f1->face()->IsD());
    assert(f2->face()->IsD());
    assert(f3->face()->IsD());
    
}
void RgbPrimitives::g2b2_Merge(RgbTriangleC& t, int VertexIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt)
{
    assert(VertexIndex>=0 && VertexIndex <= 2);
    assert(g2b2_Merge_Possible(t,VertexIndex));
    
    vectorRgbTriangle fc;
    vf(t,VertexIndex,fc);
    assert(fc.size() == 4);
    vectorFaceColor vcolor;
    extractColor(fc,vcolor);
    
    // level of one of the blue triangles
    int l;
    
    RgbTriangleC* f0;
    RgbTriangleC* f1;
    RgbTriangleC* f2;
    RgbTriangleC* f3;
    
    int k = findColorIndex(vcolor,FaceInfo::FACE_GREEN);
    if (fc[(k+1)%4].isGreen())
        ++k;
    ++k;
    assert(fc[(k)%4].isBlue()); // Now k is the first blue
    
    f0 = &fc[(k)%4];
    FaceInfo::FaceColor bluetypeu = f0->getFaceColor();
    l = f0->getFaceLevel();
    f1 = &fc[(k+1)%4];
    FaceInfo::FaceColor bluetypel = f1->getFaceColor();
    
    f2 = &fc[(k+2)%4];
    f3 = &fc[(k+3)%4];
    
    assert(f0->isBlue());
    assert(f1->isBlue());
    assert(f2->isGreen());
    assert(f3->isGreen());
    
    assert(f0->getFaceColor() != f1->getFaceColor());
    
    int mi = f2->minLevelVertex();
    
    RgbPrimitives::doCollapse(*f2,mi,to);
    
    gb_Merge(l,bluetypeu,*f0);
    gb_Merge(l,bluetypel,*f1);
    
    assert(triangleCorrectness(*f0));
    assert(triangleCorrectness(*f1));
    
    if (vt)
    {
    	vt->push_back(*f0);
    	vt->push_back(*f1);
    }
    
    assert(!f0->face()->IsD());
    assert(!f1->face()->IsD());
    assert(f2->face()->IsD());
    assert(f3->face()->IsD());
}

void RgbPrimitives::gg_Swap(RgbTriangleC& t, int VertexIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt)
{
    assert(VertexIndex>=0 && VertexIndex <= 2);
    assert(gg_Swap_Possible(t,VertexIndex));

    if (gg_Swap_6g_Possible(t,VertexIndex))
        gg_Swap_6g(t,VertexIndex,to,vt);
    else if (gg_Swap_4g1b_Possible(t,VertexIndex))
        gg_Swap_4g1b(t,VertexIndex,to,vt);
    else if (gg_Swap_3g2r_Possible(t,VertexIndex))
        gg_Swap_3g2r(t,VertexIndex,to,vt);
    
}
void RgbPrimitives::vertexRemoval(RgbTriangleC& t, int VertexIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt)
{
	if (t.getVl(VertexIndex) <= 0)
		return;
	
	RgbVertexC v = t.V(VertexIndex);
	vector<RgbVertexC> vv;
	if (stype == LOOP)
	{
		vv.reserve(6);
		RgbPrimitives::VV(v,vv,false);
		ControlPoint::vertexRemovalUpdate(v);
	}
	
	bool modified = false;
	if (!t.V(VertexIndex).getIsBorder())
	{
	
        if (r4_Merge_Possible(t,VertexIndex))
        {
            r4_Merge(t,VertexIndex,to,vt);
            modified = true;
        }
        else if (r2gb_Merge_Possible(t,VertexIndex))
        {
            r2gb_Merge(t,VertexIndex,to,vt);
            modified = true;
        }
        else if (gbgb_Merge_Possible(t,VertexIndex))
        {
            gbgb_Merge(t,VertexIndex,to,vt);
            modified = true;
        }
        else if (g2b2_Merge_Possible(t,VertexIndex))
        {
            g2b2_Merge(t,VertexIndex,to,vt);
            modified = true;
        }
        else if (gg_Swap_Possible(t,VertexIndex))
        {
            gg_Swap(t,VertexIndex,to,vt);
            modified = true;
        }
        else if (brb2g_Swap_Possible(t,VertexIndex))
        {
            brb2g_Swap(t,VertexIndex,to,vt);
            modified = true;
        }
	}
	else
	{
        if (b_r2_Merge_Possible(t,VertexIndex))
        {
            b_r2_Merge(t,VertexIndex,to,vt);
            modified = true;
        }
        else if (b_gb_Merge_Possible(t,VertexIndex))
        {
            b_gb_Merge(t,VertexIndex,to,vt);
            modified = true;
        }
	}
        
    if ((stype == LOOP) && modified)
    {
    	for (unsigned int i = 0; i < vv.size(); ++i) 
    	{
    		ControlPoint::updateP(vv[i]);
		}
    }
}

void RgbPrimitives::extractColor(vectorRgbTriangle& f,vectorFaceColor& c)
{
    vector<RgbTriangleC>::iterator it;
    c.reserve(c.size() + f.size());
    for (it = f.begin(); it < f.end(); ++it) 
    {
        c.push_back(it->getFaceColor());
    }
}

int RgbPrimitives::findColorIndex(vectorFaceColor& vc,FaceInfo::FaceColor color)
{
    for (unsigned int i = 0; i < vc.size(); ++i) 
    {
        if (vc[i] == color)
            return i;
    }
    assert(0);
    return -1;
}

void RgbPrimitives::gb_Merge(int level, FaceInfo::FaceColor color , RgbTriangleC& t)
{
    assert(color == FaceInfo::FACE_BLUE_RGG || color == FaceInfo::FACE_BLUE_GGR);
    t.setFaceLevel(level);
    if (color == FaceInfo::FACE_BLUE_RGG)
        t.setFaceColor(FaceInfo::FACE_RED_GGR);
    else
        t.setFaceColor(FaceInfo::FACE_RED_RGG);
    
}

bool RgbPrimitives::gg_SwapAuxPossible(RgbTriangleC& t, int EdgeIndex)
{
    if (t.getEdgeIsBorder(EdgeIndex))
        return false;   // edge is on the border
    
    RgbTriangleC ot = t.FF(EdgeIndex);
    int oti = t.FFi(EdgeIndex);
    assert(triangleCorrectness(t));
    assert(triangleCorrectness(ot));
    int l = t.getFaceLevel();
    
    return
    (
            (t.getFaceLevel() == ot.getFaceLevel()) &&
            ((t.getFaceColor() == FaceInfo::FACE_GREEN) && (ot.getFaceColor() == FaceInfo::FACE_GREEN)) 
            &&
            rgbt::CheckFlipEdge(*(t.face()),EdgeIndex)
            && 
            (
                    (t.getVl((EdgeIndex+2)%3) <= l-1 && ot.getVl((oti+2)%3) == l)
                    ||
                    (t.getVl((EdgeIndex+2)%3) == l && ot.getVl((oti+2)%3) <= l-1)
            )
    );
}

void RgbPrimitives::gg_SwapAux(RgbTriangleC& t, int EdgeIndex, vector<RgbTriangleC>* vt)
{
    assert(gg_SwapAuxPossible(t,EdgeIndex));
    
    int l = t.getFaceLevel();
    RgbTriangleC ot = t.FF(EdgeIndex);
    
    bool upperIsAtLevelL = (t.getVl((EdgeIndex+2)%3) == l);
    
    // t and ot are not consistent with the color of edge and level of edge but
    // at the first setFaceColor all the data is recalculated
    rgbt::FlipEdge(*(t.face()),EdgeIndex);
    
    if (!upperIsAtLevelL)
    {
        t.setFaceColor(FaceInfo::FACE_BLUE_GGR);
        ot.setFaceColor(FaceInfo::FACE_BLUE_RGG);
    }
    else
    {
        t.setFaceColor(FaceInfo::FACE_BLUE_RGG);
        ot.setFaceColor(FaceInfo::FACE_BLUE_GGR);
    }   
    t.setFaceLevel(l-1);
    ot.setFaceLevel(l-1);
    
    assert(triangleCorrectness(t));
    assert(triangleCorrectness(ot));
    
    if (vt)
    {
    	vt->push_back(t);
    	vt->push_back(ot);
    }
}

void RgbPrimitives::gg_Swap_4g1b(RgbTriangleC& t, int VertexIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt)
{
    assert(VertexIndex>=0 && VertexIndex <= 2);
    assert(gg_Swap_4g1b_Possible(t,VertexIndex));
    
    int vertexAbsoluteIndex = t.getVIndex(VertexIndex);
    int l = t.getFaceLevel();
    if (t.isBlue())
    	++l;
    
    vectorRgbTriangle fc;
    vf(t,VertexIndex,fc);
    assert(fc.size() == 5);
    int k = -1;
    for (unsigned int i = 0; i < fc.size(); ++i) 
    {
    	if (fc[i].isBlue())
    		k = i;
	}
    assert(k >= 0 && k <= 4);
    vector<int> extVertex(fc.size());
    vector<int> sharedVertex(fc.size());
    
    int res = 0;
    int nVertexLowLevel = 0;
    for (unsigned int i = 0; i < fc.size(); ++i) 
    {
    	assert(fc[i].containVertex(vertexAbsoluteIndex));
        fc[i].containVertex(vertexAbsoluteIndex,&res);
        sharedVertex[i] = res;
        extVertex[i] = (res+1)%3;
        if (fc[i].getVl(extVertex[i]) <= l-1)
        {
            nVertexLowLevel++;
        }
    }
    assert(nVertexLowLevel == 2);
    
    bool isGGR = (fc[k].getFaceColor() == FaceInfo::FACE_BLUE_GGR);
    if (isGGR)
    {
    	assert(fc[k].getVl(extVertex[k]) == l);
    	assert(fc[(k+1)%5].getVl(extVertex[(k+1)%5]) <= l-1);
    	assert(fc[(k+4)%5].getVl(extVertex[(k+4)%5]) <= l-1);

    	assert(gg_SwapAuxPossible(fc[(k+3)%5],sharedVertex[(k+3)%5]));
	    gg_SwapAux(fc[(k+3)%5],sharedVertex[(k+3)%5],vt);

	    assert(vertexRemoval_Possible(fc[(k+4)%5],sharedVertex[(k+4)%5]));
        vertexRemoval(fc[(k+4)%5],sharedVertex[(k+4)%5],to,vt);
    }
    else
    {
    	assert(fc[k].getVl(extVertex[k]) <= l-1);
    	assert(fc[(k+1)%5].getVl(extVertex[(k+1)%5]) == l);
    	assert(fc[(k+2)%5].getVl(extVertex[(k+2)%5]) <= l-1);

    	
    	assert(gg_SwapAuxPossible(fc[(k+3)%5],sharedVertex[(k+3)%5]));
    	gg_SwapAux(fc[(k+3)%5],sharedVertex[(k+3)%5],vt);

    	assert(vertexRemoval_Possible(fc[(k+1)%5],sharedVertex[(k+1)%5]));
        vertexRemoval(fc[(k+1)%5],sharedVertex[(k+1)%5],to,vt);
    }
    
}


void RgbPrimitives::gg_Swap_3g2r(RgbTriangleC& t, int VertexIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt)
{
	assert(VertexIndex>=0 && VertexIndex <= 2);
	assert(gg_Swap_3g2r_Possible(t,VertexIndex));
	
	int vertexAbsoluteIndex = t.getVIndex(VertexIndex);
	int l = t.getFaceLevel();
	if (t.isRed())
		++l;
	
	vectorRgbTriangle fc;
	vf(t,VertexIndex,fc);
	assert(fc.size() == 5);
	int k = -1;
	for (unsigned int i = 0; i < fc.size(); ++i) 
	{
		if (fc[i].getFaceColor() == FaceInfo::FACE_RED_GGR)
			k = i;
	}
	assert(k >= 0 && k <= 4);
	assert(fc[k].getFaceColor() == FaceInfo::FACE_RED_GGR);
	assert(fc[(k+1)%5].getFaceColor() == FaceInfo::FACE_RED_RGG);
	
	
	vector<int> extVertex(fc.size());
	vector<int> sharedVertex(fc.size());
	
	int res = 0;
	int nVertexLowLevel = 0;
	for (unsigned int i = 0; i < fc.size(); ++i) 
	{
	    fc[i].containVertex(vertexAbsoluteIndex,&res);
	    sharedVertex[i] = res;
	    extVertex[i] = (res+1)%3;
	    if (fc[i].getVl(extVertex[i]) <= l-1)
	    {
	        nVertexLowLevel++;
	    }
	}
	
	assert(nVertexLowLevel == 3);
	
	assert(fc[k].getVl(extVertex[k]) <= l-1);
	assert(fc[(k+1)%5].getVl(extVertex[(k+1)%5]) <= l-1);
	assert(fc[(k+2)%5].getVl(extVertex[(k+2)%5]) <= l-1);
	
    assert(gg_SwapAuxPossible(fc[(k+4)%5],sharedVertex[(k+4)%5]));
    gg_SwapAux(fc[(k+4)%5],sharedVertex[(k+4)%5],vt);
		
    assert(vertexRemoval_Possible(fc[k],sharedVertex[k]));
    vertexRemoval(fc[k],sharedVertex[k],to,vt);
}

void RgbPrimitives::gg_Swap_6g(RgbTriangleC& t, int VertexIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt)
{
    assert(VertexIndex>=0 && VertexIndex <= 2);
    assert(gg_Swap_6g_Possible(t,VertexIndex));
    
    int vertexAbsoluteIndex = t.getVIndex(VertexIndex);
    int l = t.getFaceLevel();
    
    vectorRgbTriangle fc;
    vf(t,VertexIndex,fc);
    assert(fc.size() == 6);
    vector<int> extVertex(fc.size());
    vector<int> sharedVertex(fc.size());
    
    int k = 0;
    
    int res = 0;
    int nVertexLowLevel = 0;
    for (unsigned int i = 0; i < fc.size(); ++i) 
    {
        fc[i].containVertex(vertexAbsoluteIndex,&res);
        sharedVertex[i] = res;
        extVertex[i] = (res+1)%3;
        if (fc[i].getVl(extVertex[i]) <= l-1)
        {
            nVertexLowLevel++;
            k = i;
        }
    }
    
    assert(nVertexLowLevel == 2);
    
    RgbTriangleC* f0 = &fc[(k+0)%6];
    RgbTriangleC* f2 = &fc[(k+2)%6];
    RgbTriangleC* f3 = &fc[(k+3)%6];

    assert(gg_SwapAuxPossible(*f0,(sharedVertex[(k+0)%6]+2)%3));
	gg_SwapAux(*f0,(sharedVertex[(k+0)%6]+2)%3,vt);

	assert(gg_SwapAuxPossible(*f3,(sharedVertex[(k+3)%6]+2)%3));
	gg_SwapAux(*f3,(sharedVertex[(k+3)%6]+2)%3,vt);
    	
    assert(vertexRemoval_Possible(*f2,sharedVertex[(k+2)%6]));
    vertexRemoval(*f2,sharedVertex[(k+2)%6],to,vt);
}

bool RgbPrimitives::check_4g1b_LevelCorrectness(vectorRgbTriangle& fc, int l)
{
    // Check Levels
	for (unsigned int i = 0; i < fc.size(); ++i) 
	{
		if (fc[i].getFaceColor() == FaceInfo::FACE_GREEN)
		{
			if (fc[i].getFaceLevel() != l)
				return false;
		}
		else
		{
			if 	(
					!(fc[i].getFaceColor() == FaceInfo::FACE_BLUE_GGR || fc[i].getFaceColor() == FaceInfo::FACE_BLUE_RGG )
					||
					(fc[i].getFaceLevel() != l-1)
				)
				return false;
		}
	}
	return true;
}

bool RgbPrimitives::gg_Swap_4g1b_Possible(RgbTriangleC& t, int VertexIndex)
{
    assert(VertexIndex>=0 && VertexIndex <= 2);
    
    if (t.V(VertexIndex).getIsBorder())
        return false;   // Vertex is on the border

    if (!s4g1bggr)
    {
    	s4g1bggr = new vector<FaceInfo::FaceColor>(5);
        (*s4g1bggr)[0] = FaceInfo::FACE_GREEN;
        (*s4g1bggr)[1] = FaceInfo::FACE_GREEN;
        (*s4g1bggr)[2] = FaceInfo::FACE_GREEN;
        (*s4g1bggr)[3] = FaceInfo::FACE_GREEN;
        (*s4g1bggr)[4] = FaceInfo::FACE_BLUE_GGR;
    }

    if (!s4g1brgg)
    {
    	s4g1brgg = new vector<FaceInfo::FaceColor>(5);
        (*s4g1brgg)[0] = FaceInfo::FACE_GREEN;
        (*s4g1brgg)[1] = FaceInfo::FACE_GREEN;
        (*s4g1brgg)[2] = FaceInfo::FACE_GREEN;
        (*s4g1brgg)[3] = FaceInfo::FACE_GREEN;
        (*s4g1brgg)[4] = FaceInfo::FACE_BLUE_RGG;
    }
    
    vectorRgbTriangle fc;
    vf(t,VertexIndex,fc);
    vectorFaceColor vcolor;
    extractColor(fc,vcolor);
    
    
    if (!(isMatch(vcolor,*s4g1bggr) || isMatch(vcolor,*s4g1brgg)))
    	return false;

    return true;
    
    int l = 0;
    
    if (fc[0].getFaceColor() == FaceInfo::FACE_GREEN)
    	l = fc[0].getFaceLevel();
    else
    	l = fc[1].getFaceLevel();
    
    if (t.getVl(VertexIndex) != l)
    	return false;
    
    return check_4g1b_LevelCorrectness(fc,l);
}

bool RgbPrimitives::check_3g2r_LevelCorrectness(vectorRgbTriangle& fc, int l)
{
    // Check Levels
	for (unsigned int i = 0; i < fc.size(); ++i) 
	{
		if (fc[i].getFaceColor() == FaceInfo::FACE_GREEN)
		{
			if (fc[i].getFaceLevel() != l)
				return false;
		}
		else
		{
			if 	(
					!(fc[i].isRed())
					||
					(fc[i].getFaceLevel() != l-1)
				)
				return false;
		}
	}
	return true;
}

bool RgbPrimitives::gg_Swap_3g2r_Possible(RgbTriangleC& t, int VertexIndex)
{
    assert(VertexIndex>=0 && VertexIndex <= 2);
    
    if (t.V(VertexIndex).getIsBorder())
        return false;   // Vertex is on the border

    if (!s3g2rp)
    {
    	s3g2rp = new vector<FaceInfo::FaceColor>(5);
        (*s3g2rp)[0] = FaceInfo::FACE_GREEN;
        (*s3g2rp)[1] = FaceInfo::FACE_GREEN;
        (*s3g2rp)[2] = FaceInfo::FACE_GREEN;
        (*s3g2rp)[3] = FaceInfo::FACE_RED_GGR;
        (*s3g2rp)[4] = FaceInfo::FACE_RED_RGG;
    }
    vectorRgbTriangle fc;
    vf(t,VertexIndex,fc);
    vectorFaceColor vcolor;
    extractColor(fc,vcolor);
    if (!isMatch(vcolor,*s3g2rp))
    	return false;

    return true;
    
    int l;
    
    if (fc[0].isRed())
    	l = fc[0].getFaceLevel() + 1;
    else
    	l = fc[0].getFaceLevel();
    	
    if (t.getVl(VertexIndex) != l)
    	return false;
    
    return check_3g2r_LevelCorrectness(fc,l);
    
}

bool RgbPrimitives::gg_Swap_6g_Possible(RgbTriangleC& t, int VertexIndex)
{
    assert(VertexIndex>=0 && VertexIndex <= 2);
    
    if (t.V(VertexIndex).getIsBorder())
        return false;   // Vertex is on the border
    
    if (!s6gp)
    {
        s6gp = new vector<FaceInfo::FaceColor>(6);
        (*s6gp)[0] = FaceInfo::FACE_GREEN;
        (*s6gp)[1] = FaceInfo::FACE_GREEN;
        (*s6gp)[2] = FaceInfo::FACE_GREEN;
        (*s6gp)[3] = FaceInfo::FACE_GREEN;
        (*s6gp)[4] = FaceInfo::FACE_GREEN;
        (*s6gp)[5] = FaceInfo::FACE_GREEN;
    }
    vectorRgbTriangle fc;
    vf(t,VertexIndex,fc);
    vectorFaceColor vcolor;
    extractColor(fc,vcolor);
    
    int level = fc[0].getFaceLevel();
    
    if (!(
    	   isMatch(vcolor,*s6gp) &&
    	   (fc[0].getFaceLevel() == level) &&
    	   (fc[1].getFaceLevel() == level) &&
    	   (fc[2].getFaceLevel() == level) &&
    	   (fc[3].getFaceLevel() == level) &&
    	   (fc[4].getFaceLevel() == level) &&
    	   (fc[5].getFaceLevel() == level)
    	   ))
    	   return false;
    
    int nVertexLowLevel = 0;
    int vertexAbsoluteIndex = t.V(VertexIndex).index;
    for (unsigned int i = 0; i < fc.size(); ++i) 
    {
    	int res = 0;
        fc[i].containVertex(vertexAbsoluteIndex,&res);
        if (fc[i].getVl((res+1)%3) <= level-1)
        {
            nVertexLowLevel++;
        }
    }
    
    if (nVertexLowLevel != 2)
    	return false;
    
    return true;

}


bool RgbPrimitives::IsValidEdge(RgbVertexC& rgbv1,RgbVertexC& rgbv2, RgbTriangleC* t, int* ti)
{
	CMeshO* m = rgbv1.m;
	RgbInfo* info = rgbv1.rgbInfo;
	int v1 = rgbv1.index;
	int v2 = rgbv2.index;
	
	
	assert((unsigned int)v1 < m->vert.size());
	assert((unsigned int)v2 < m->vert.size());
	
	if (m->vert[v1].IsD() || m->vert[v2].IsD())
	{
		//std::cerr << "DELETED" << std::endl;
		return false;
	}
	
	VertexType& v = m->vert[v1];
	RgbTriangleC tf = RgbTriangleC(m,info,v.VFp()->Index());
	int tfi = v.VFi();
	assert(tf.V(tfi).index == v1);

	VertexType& va = m->vert[v2];
	RgbTriangleC tfa = RgbTriangleC(m,info,va.VFp()->Index());
	int tfia = va.VFi();
	assert(tfa.V(tfia).index == v2);
	
	vector<RgbTriangleC> vf;
	vf.reserve(6);
	RgbPrimitives::vf(tf,tfi,vf);
	
	for (unsigned int i = 0; i < vf.size(); ++i) 
	{
		RgbTriangleC& tt = vf[i];
		int k = 0;
		while(tt.V(k).index != v1)
		{
			assert(k <= 2);
			k++;
		}
		
		if (tt.V((k+1)%3).index == v2)
		{
			if (t)
				*t = tt;
			if (ti)
				*ti = k;
			return true;
		}
		
	}
	return false;
}

void RgbPrimitives::recursiveEdgeSplitAux(RgbVertexC& v1, RgbVertexC& v2, TopologicalOpC& to, vector<RgbTriangleC>* vt)
{
	RgbTriangleC t;
	int EdgeIndex;

	if (!IsValidEdge(v1,v2,&t,&EdgeIndex))
		return;
	
	RgbTriangleC* tp = &t;
	
	if (tp->isRed())
	{
		int index = -1;
		int l = tp->getFaceLevel();
		for (int i = 0; i < 3; ++i) 
		{
			if (tp->getEdgeLevel(i) == l && tp->getEdgeColor(i) == FaceInfo::EDGE_GREEN)
				index = i;
		}
		assert(index >= 0 && index <= 2);
		RgbVertexC v1t = tp->V(index);
		RgbVertexC v2t = tp->V((index+1)%3);
		recursiveEdgeSplitVV(v1t,v2t,to,vt);
	}
	else
	{
		
		assert(tp->isBlue());
		int l = tp->getFaceLevel();
		int redEdge = tp->minLevelEdge();
		assert(tp->getEdgeColor(redEdge) == FaceInfo::EDGE_RED);
		RgbTriangleC redTriangle = tp->FF(redEdge);
		assert(redTriangle.getFaceLevel() == l);
		assert(redTriangle.isRed());
		
		int index = -1;
 		for (int i = 0; i < 3; ++i) 
		{
			if (redTriangle.getEdgeLevel(i) == l && redTriangle.getEdgeColor(i) == FaceInfo::EDGE_GREEN)
				index = i;
		}
		assert(index >= 0 && index <= 2);
		
		RgbVertexC v1t = redTriangle.V(index);
		RgbVertexC v2t = redTriangle.V((index+1)%3);

		recursiveEdgeSplitVV(v1t,v2t,to,vt);
	}
}


bool RgbPrimitives::recursiveEdgeSplit(RgbTriangleC& t, int EdgeIndex, TopologicalOpC& to, vector<RgbTriangleC>* vtr)
{
	RgbVertexC v1 = t.V(EdgeIndex);
	RgbVertexC v2 = t.V((EdgeIndex+1)%3);
	return recursiveEdgeSplitVV(v1,v2,to,vtr);
}

bool RgbPrimitives::recursiveEdgeSplitVV(RgbVertexC& v1,RgbVertexC& v2, TopologicalOpC& to, vector<RgbTriangleC>* vt)
{
	RgbTriangleC t;
	int EdgeIndex;
	
	if (!IsValidEdge(v1,v2,&t,&EdgeIndex))
	{
		return false;
	}
		
	
	if (t.getEdgeColor(EdgeIndex) == FaceInfo::EDGE_RED)
	{
		return false;
	}
	
	if (edgeSplit_Possible(t,EdgeIndex))
	{
		return edgeSplit(t,EdgeIndex,to,vt);
	}
	
	int l = t.getEdgeLevel(EdgeIndex);
	
	RgbTriangleC ot = t.FF(EdgeIndex);
	
	assert(t.getFaceLevel() == l || t.getFaceLevel() == l-1);
	assert(ot.getFaceLevel() == l || ot.getFaceLevel() == l-1);
	
	if (t.getFaceLevel() < l)
	{
		recursiveEdgeSplitAux(v1,v2,to,vt);
	}
	if (ot.getFaceLevel() < l)
	{
		recursiveEdgeSplitAux(v2,v1,to,vt);
	}
	
	if (!IsValidEdge(v1,v2,&t,&EdgeIndex)) // the edge can be already splitted
		return true;
	
	if (edgeSplit_Possible(t,EdgeIndex))
	{
		// std::cerr << "POSSIBLE2" << std::endl;
		return edgeSplit(t,EdgeIndex,to,vt);
	}
	
	return false;
}

bool RgbPrimitives::isVertexInternal(RgbVertexC& v)
{
    vectorRgbTriangle fc;
    assert(!v.vert().IsD());
    FacePointer fp = v.vert().VFp();
    int fi = v.vert().VFi();
    if (!fp)
        return false;
    vcg::face::Pos<FaceType> pos(fp,fp->V(fi));
    CMeshO::FacePointer first = pos.F();
    if (pos.IsBorder())
        return false;
    pos.FlipF();
    pos.FlipE();
    
    while(pos.F() != first)
    {
        if (pos.IsBorder())
        {
            return false;
        }
        
        pos.FlipF();
        pos.FlipE();
    }
    
    return true;
}


bool RgbPrimitives::isVertexInternal(RgbTriangleC& t, int VertexIndex)
{
	assert(VertexIndex>= 0 && VertexIndex<=2);
    vectorRgbTriangle fc;
    assert(!t.face()->IsD());
    assert(!t.face()->V(VertexIndex)->IsD());

    vcg::face::Pos<FaceType> pos(t.face(),t.face()->V(VertexIndex));
    CMeshO::FacePointer first = pos.F(); 
    pos.FlipF();
    pos.FlipE();
    
    while(pos.F() && (pos.F() != first))
    {
    	if (vcg::face::BorderCount(*pos.F()))
    	{
    		return false;
    	}
        
        pos.FlipF();
        pos.FlipE();
    }
    
    return true;
}

bool RgbPrimitives::brb2g_Swap_Possible(RgbTriangleC& t, int VertexIndex)
{
    assert(VertexIndex>=0 && VertexIndex <= 2);

    if (t.V(VertexIndex).getIsBorder())
        return false;   // Vertex is on the border

    vectorRgbTriangle fc;
    fc.reserve(5);
    vf(t,VertexIndex,fc);

    if (fc.size() != 5)
    	return false;
    
    int ri = -1;
    for (unsigned int i = 0; i < fc.size(); ++i) 
    {
		if (fc[i].isRed())
		{
			ri = i;
			break;
		}
	}
    
    assert(ri <= 5);
    if (ri < 0)
    	return false;
    
    int l = fc[ri].getFaceLevel();
    return 
    (
    		fc[(ri+0)%5].isRed() && (fc[(ri+0)%5].getFaceLevel()) == l &&
    		fc[(ri+1)%5].isBlue() && (fc[(ri+1)%5].getFaceLevel()) == l &&
    		fc[(ri+2)%5].isGreen() && (fc[(ri+2)%5].getFaceLevel()) == l+1 &&
    		fc[(ri+3)%5].isGreen() && (fc[(ri+3)%5].getFaceLevel()) == l+1 &&
    		fc[(ri+4)%5].isBlue() && (fc[(ri+4)%5].getFaceLevel()) == l 
    		
    );
    
}

void RgbPrimitives::brb2g_Swap(RgbTriangleC& t, int VertexIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt)
{
	RgbVertexC v = t.V(VertexIndex);
	
    assert(VertexIndex>=0 && VertexIndex <= 2);
    
    vectorRgbTriangle fc;
    fc.reserve(5);
    vf(t,VertexIndex,fc);

    assert(fc.size() == 5);

    int ri = -1;
    for (unsigned int i = 0; i < fc.size(); ++i) 
    {
		if (fc[i].isRed())
		{
			ri = i;
			break;
		}
	}
    
    assert(ri >= 0 && ri <= 4);
    
    int rei = -1;
    for (int i = 0; i < 3; ++i) 
    {
    	if (fc[ri].getEdgeColor(i) == FaceInfo::EDGE_RED)
    	{
			rei = i;
			break;
    	}
		
	}
    
    assert(rei >= 0 && rei <= 2);
    
    RgbTriangleC& t1 = fc[ri];
    RgbTriangleC t2 = fc[ri].FF(rei);
    
    int l = t1.getFaceLevel();
    
    assert(t1.isRed());
    assert(t2.isBlue());
    assert(t2.index == fc[(ri+1)%5].index || t2.index == fc[(ri+4)%5].index);
    
    FaceInfo::FaceColor redt = t1.getFaceColor();
    FaceInfo::FaceColor bluet = t2.getFaceColor();
    
    rgbt::FlipEdge(*(t1.face()),rei);

    t1.updateInfo();
    t2.updateInfo();
    
    RgbTriangleC* pred;
    RgbTriangleC* pblue;
    
    if (t1.countVertexAtLevel(l+1) == 2)
    {
    	// t1 is the blue triangle
    	pblue = &t1;
    	pred = &t2;
    }
    else
    {
    	// t2 is the blue triangle
    	pblue = &t2;
    	pred = &t1;
    }

	assert(pred->countVertexAtLevel(l+1) == 1);
	assert(pblue->countVertexAtLevel(l+1) == 2);
	
	if (bluet == FaceInfo::FACE_BLUE_GGR)
		pblue->setFaceColor(FaceInfo::FACE_BLUE_RGG);
	else
		pblue->setFaceColor(FaceInfo::FACE_BLUE_GGR);

	if (redt == FaceInfo::FACE_RED_GGR)
		pred->setFaceColor(FaceInfo::FACE_RED_RGG);
	else
		pred->setFaceColor(FaceInfo::FACE_RED_GGR);
    
    assert(triangleCorrectness(*pred));
    assert(triangleCorrectness(*pblue));
    
    RgbTriangleC& green = fc[(ri+2)%5];
    assert(green.isGreen());
    
    int greeni = 0;
    assert(green.containVertex(v.index));
    green.containVertex(v.index,&greeni);
    
    assert(g2b2_Merge_Possible(green,greeni));
    g2b2_Merge(green,greeni, to, vt);
    
}


bool RgbPrimitives::b_g_Bisection_Possible(RgbTriangleC& t, int EdgeIndex)
{
    assert(triangleCorrectness(t));

    return
    (
            (t.getEdgeIsBorder(EdgeIndex)) && // edge is on boundary
            (t.getFaceColor() == FaceInfo::FACE_GREEN) // t is green
    );
}

void RgbPrimitives::b_g_Bisection(RgbTriangleC& t, int EdgeIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt)
{
        assert(b_g_Bisection_Possible(t,EdgeIndex));
        int l = t.getFaceLevel();
        
        // Store the face obtained with the split
        vector<FacePointer> vfp;
        // Execute the split
        RgbVertexC vNew;
        vector<RgbVertexC> vCont;
        vector<RgbVertexC> vUpd;


        bool todo = RgbPrimitives::doSplit(t,EdgeIndex,l+1,to,&vfp,&vNew,&vCont,&vUpd);
        
        if (!todo)
            return; // The update on rgb is already done by doSplit
        
        RgbTriangleC t0 = RgbTriangleC(t.m,t.rgbInfo,vfp[0]->Index());
        RgbTriangleC t2 = RgbTriangleC(t.m,t.rgbInfo,vfp[1]->Index());

        g_Bisection(l,t0,t2);
        
        assert(triangleCorrectness(t0)); 
        assert(triangleCorrectness(t2)); 
        
        if (vt)
        {
            vt->push_back(t0);
            vt->push_back(t2);
        }

        if (stype == LOOP)
        	distributeContribute(vCont,vNew,vUpd);
}

bool RgbPrimitives::b_r_Bisection_Possible(RgbTriangleC& t, int EdgeIndex)
{
    assert(triangleCorrectness(t));

    return
    (
            (t.getEdgeIsBorder(EdgeIndex)) && // edge is on boundary
            (t.isRed()) && // t is red
            (t.getEdgeLevel(EdgeIndex) == t.getFaceLevel()) &&
            (t.getEdgeColor(EdgeIndex) == FaceInfo::EDGE_GREEN)
    );
}

void RgbPrimitives::b_r_Bisection(RgbTriangleC& t, int EdgeIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt)
{
    assert(b_r_Bisection_Possible(t,EdgeIndex));
    int l = t.getFaceLevel();
    
    RgbTriangleC* tp;
    int ti;
    
    tp = &t;
    ti = EdgeIndex;

    VertexPair vp = tp->getRedEdge();
    FaceInfo::FaceColor redtypeu = tp->getFaceColor();
    // Store the face obtained with the split
    vector<FacePointer> vfp;
    // Execute the split
    //to.doSplit(e,p,&vfp);
    RgbVertexC vNew;
    vector<RgbVertexC> vCont;
    vector<RgbVertexC> vUpd;
    
    bool todo = RgbPrimitives::doSplit(*tp,ti,l+1,to,&vfp,&vNew,&vCont,&vUpd);
    if (!todo)
        return; // The update on rgb is already done by doSplit

    
    RgbTriangleC t0 = RgbTriangleC(t.m,t.rgbInfo,vfp[0]->Index());
    RgbTriangleC t2 = RgbTriangleC(t.m,t.rgbInfo,vfp[1]->Index());

    
    r_Bisection(l,redtypeu,t2,t0,vp);
    
    assert(triangleCorrectness(t0));
    assert(triangleCorrectness(t2)); 

    vector<RgbTriangleC*> vb;
    if (t0.isBlue()) 
        vb.push_back(&t0);
    if (t2.isBlue()) 
        vb.push_back(&t2);
    
    assert(vb.size() == 1);

    if (vt)
    {
        vt->push_back(t0);
        vt->push_back(t2);
    }
    bb_Swap_If_Needed(*vb[0],vt);
    
    if (stype == LOOP)
    	distributeContribute(vCont,vNew,vUpd);
    return;
    
}


bool RgbPrimitives::b_r2_Merge_Possible(RgbTriangleC& t, int VertexIndex)
{
    if (!t.V(VertexIndex).getIsBorder())
        return false;   // Vertex is NOT on the border
    
    assert(VertexIndex>=0 && VertexIndex <= 2);

    vectorRgbTriangle fc;
    vf(t,VertexIndex,fc);
    vectorFaceColor vcolor;
    return 
    (
            (fc.size() == 2) &&
            (fc[0].getFaceColor() == FaceInfo::FACE_RED_GGR) &&
            (fc[1].getFaceColor() == FaceInfo::FACE_RED_RGG) &&
            (fc[0].getFaceLevel() == fc[1].getFaceLevel()) 
    );
}

void RgbPrimitives::b_r2_Merge(RgbTriangleC& t, int VertexIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt)
{
    assert(VertexIndex>=0 && VertexIndex <= 2);
    assert(b_r2_Merge_Possible(t,VertexIndex));
    
    vectorRgbTriangle fc;
    vf(t,VertexIndex,fc);
    assert(fc.size() == 2);
    
    
    RgbTriangleC* f0 = &fc[1];
    RgbTriangleC* f1 = &fc[0];

    // level of one of the red triangles
    int l = f0->getFaceLevel();
    
    assert(f0->getFaceColor() == FaceInfo::FACE_RED_RGG);
    assert(f1->getFaceColor() == FaceInfo::FACE_RED_GGR);
    
    int fi = (f0->maxLevelVertex()+2)%3;
    RgbPrimitives::doCollapse(*f0,fi,to);

    f1->setFaceColor(FaceInfo::FACE_GREEN,false);
    f1->setFaceLevel(l);

    assert(triangleCorrectness(*f1));

    if (vt)
    {
        vt->push_back(*f0);
        // Also the adjacent faces may contain vertexes that have to be removed
        vt->push_back(fc[0].FF(0));
        vt->push_back(fc[0].FF(1));
        vt->push_back(fc[0].FF(2));
    }
    
    assert(f0->face()->IsD());
    assert(!f1->face()->IsD());
}

bool RgbPrimitives::b_gb_Merge_Possible(RgbTriangleC& t, int VertexIndex)
{
    if (!t.V(VertexIndex).getIsBorder())
        return false;   // Vertex is NOT on the border
    
    assert(VertexIndex>=0 && VertexIndex <= 2);

    vectorRgbTriangle fc;
    vf(t,VertexIndex,fc);
    vectorFaceColor vcolor;
    
    RgbTriangleC* g;
    RgbTriangleC* b;
    
    if (fc.size() != 2)
        return false;
    
    if (fc[0].isGreen())
    {
        g = &fc[0];
        b = &fc[1];
        if (b->getFaceColor() != FaceInfo::FACE_BLUE_GGR)
            return false;
    }
    else
    {
        g = &fc[1];
        b = &fc[0];
        if (b->getFaceColor() != FaceInfo::FACE_BLUE_RGG)
            return false;
    }
    
    if (!g->isGreen())
        return false;
    
    return 
    (
            (b->getFaceLevel()+1 == g->getFaceLevel())
    );
}

void RgbPrimitives::b_gb_Merge(RgbTriangleC& t, int VertexIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt)
{
    assert(VertexIndex>=0 && VertexIndex <= 2);
    assert(b_gb_Merge_Possible(t,VertexIndex));
    
    vectorRgbTriangle fc;
    vf(t,VertexIndex,fc);
    assert(fc.size() == 2);
    RgbTriangleC* g;
    RgbTriangleC* b;
    
    if (fc[0].isGreen())
    {
        g = &fc[0];
        b = &fc[1];
    }
    else
    {
        g = &fc[1];
        b = &fc[0];
    }
    
    // level of the blue triangle
    int l = b->getFaceLevel();
    
    assert(g->isGreen());
    assert(b->isBlue());
    
    bool isBlueRGG = (b->getFaceColor() == FaceInfo::FACE_BLUE_RGG);
    int fi = -1;
    
    for (int i = 0; i < 3; i++)
        if (fc[1].getEdgeIsBorder(i))
            fi = i;
    
    // Special case if two edge are on the border
    if (fc[1].getEdgeIsBorder((fi+1)%3))
        fi = (fi+1)%3;
    
    assert(fi != -1);
    
    assert(!fc[1].face()->IsD());
    RgbPrimitives::doCollapse(fc[1],fi,to);

    if (isBlueRGG)
        fc[0].setFaceColor(FaceInfo::FACE_RED_GGR,false);
    else
        fc[0].setFaceColor(FaceInfo::FACE_RED_RGG,false);
    
    fc[0].setFaceLevel(l);
    
    assert(!fc[0].face()->IsD());
    assert(fc[1].face()->IsD());
    if (!triangleCorrectness(fc[0]))
    {
        bool r = triangleCorrectness(fc[0]);
        assert(r);
    }

    if (vt)
    {
        vt->push_back(fc[0]);
        // Also the adjacent faces may contain vertex that has to be removed
        vt->push_back(fc[0].FF(0));
        vt->push_back(fc[0].FF(1));
        vt->push_back(fc[0].FF(2));
    }
    
}

RgbPrimitives::RgbVertexC RgbPrimitives::findOppositeVertex(RgbTriangleC& tin, int EdgeIndex, vector<RgbVertexC>* firstVertexes)
{
	int count = 0;
	RgbTriangleC t = tin;
	int ti = EdgeIndex;
	while (true)
	{
		if (t.isGreen())
		{
			return t.V((ti+2)%3);
		}
		assert(t.isRed());
		{
			if ((count == 0) && firstVertexes)
			{
				firstVertexes->push_back(t.V((ti+2)%3));
			}
			int rei = -1;
	        for (int i = 0; i < 3; ++i) 
	        {
	            if (t.getEdgeColor(i) == FaceInfo::EDGE_RED)
	                rei = i;
	        }
	        assert(rei >= 0 && rei <= 2);
	        
	        RgbTriangleC t1 = t.FF(rei);
	        int t1i = t.FFi(rei);
	        
	        assert(t1.isRed() || t1.isBlue());
	        
	        if (t1.isRed())
	        {
	        	return t1.V((t1i+2)%3);
	        }
	        else
	        {
	        	assert(t1.isBlue());
	        	RgbTriangleC t2;
	        	int t2i;
	        	// isBlue
	        	if (t1.containVertex(t.V((ti+1)%3).index))
	        	{
	        		t2 = t1.FF((t1i+2)%3);
	        		t2i = t1.FFi((t1i+2)%3);
	    			if ((count == 0) && firstVertexes)
	    			{
	    				firstVertexes->push_back(t1.V((t1i+2)%3));
	    			}

	        	}
	        	else
	        	{
	        		assert(t1.containVertex(t.V(ti).index));
	        		t2 = t1.FF((t1i+1)%3);
	        		t2i = t1.FFi((t1i+1)%3);
	    			if ((count == 0) && firstVertexes)
	    			{
	    				firstVertexes->push_back(t1.V((t1i+2)%3));
	    			}
	        	}
	        	
	        	t = t2;
	        	ti = t2i;
	        	t.updateInfo();
	        	assert(t.isGreen() || t.isRed());
	        	
	        }
			
		}
		++count;
	}
}

void RgbPrimitives::splitGreenEdgeIfNeeded(RgbVertexC& v, int minLevel, TopologicalOpC& to)
{
	if (stype == LOOP)
	{
		if ((v.getLevel() == minLevel -1) || (v.getIsPinfReady()) || v.getIsMarked())
			return;
	}
	else 
	{
		if (v.getIsMarked())
			return;
	}
	
	v.setIsMarked(true);
	
	bool split = true;
	while(split)
	{
		split = false;
		int level;
	    int i = 0;

		FacePointer fp = v.vert().VFp();
		int fi = v.vert().VFi();
	    vcg::face::Pos<FaceType> pos(fp,fi);
	    
	    if (v.getIsBorder())       // if is border move cw until the border is found
	    {
	        pos.FlipE();
	        pos.FlipF();
	        
	        while (!pos.IsBorder())
	        {
	            pos.FlipE();
	            pos.FlipF();
	        }
	        
	        pos.FlipE();
	    }

	    CMeshO::FacePointer first = pos.F(); 

	    RgbTriangleC tmp = RgbTriangleC(v.m,v.rgbInfo,pos.F()->Index());
	    assert(tmp.containVertex(v.index));
	    tmp.containVertex(v.index,&i);
	    assert(i>=0 && i<= 2);
	    level = tmp.getEdgeLevel(i); 
	    
	    if (level < (minLevel - 1)  && tmp.getEdgeColor(i) == FaceInfo::EDGE_GREEN)
	    {
	    	split = RgbPrimitives::recursiveEdgeSplit(tmp,i,to);
	    	if (split)
	    	    continue;
	    }
	    
	    pos.FlipF();
	    pos.FlipE();
	    
	    while(pos.F() && (pos.F() != first))
	    {
	        RgbTriangleC tmp = RgbTriangleC(v.m,v.rgbInfo,pos.F()->Index());
	        assert(tmp.containVertex(v.index));
	        tmp.containVertex(v.index,&i);
	        assert(i>=0 && i<= 2);
	        
	        level = tmp.getEdgeLevel(i);
		    if (level < (minLevel -1 ) && tmp.getEdgeColor(i) == FaceInfo::EDGE_GREEN)
		    {
		    	split = RgbPrimitives::recursiveEdgeSplit(tmp,i,to);
		    	if (split)
		    	    break;
		    }

	        pos.FlipF();
	        pos.FlipE();
	        assert(pos.F()->V(0) == fp->V(fi) || pos.F()->V(1) == fp->V(fi) || pos.F()->V(2) == fp->V(fi));
	        assert(!fp->IsD());
	    }
	    
	    
	}
	
	v.setIsMarked(false);
	if (RgbPrimitives::stype == LOOP)
	{
		assert(v.getIsPinfReady());
	}
}

void RgbPrimitives::splitRedEdgeIfNeeded(RgbVertexC& v, int minLevel, TopologicalOpC& to)
{
	bool split = true;
	
	while(split)
	{
		split = false;
		int level;
	    int i = 0;

		FacePointer fp = v.vert().VFp();
		int fi = v.vert().VFi();
	    vcg::face::Pos<FaceType> pos(fp,fi);
	    
	    if (v.getIsBorder())       // if is border move cw until the border is found
	    {
	        pos.FlipE();
	        pos.FlipF();
	        
	        while (!pos.IsBorder())
	        {
	            pos.FlipE();
	            pos.FlipF();
	        }
	        
	        pos.FlipE();
	    }
	    
	    CMeshO::FacePointer first = pos.F(); 

	    RgbTriangleC tmp = RgbTriangleC(v.m,v.rgbInfo,pos.F()->Index());
	    assert(tmp.containVertex(v.index));
	    tmp.containVertex(v.index,&i);
	    assert(i>=0 && i<= 2);
        // ----------
        level = tmp.getEdgeLevel(i);
	    if (level < (minLevel - 1)  && tmp.getEdgeColor(i) == FaceInfo::EDGE_RED)
	    {
	    	if (tmp.isRed())
	    	{
    			assert(tmp.getEdgeColor((i+1)%3) == FaceInfo::EDGE_GREEN);
    			assert(tmp.getEdgeColor((i+2)%3) == FaceInfo::EDGE_GREEN);
    			assert(tmp.getEdgeColor((i)%3) == FaceInfo::EDGE_RED);

    			if (tmp.getEdgeLevel((i+1)%3) <= tmp.getEdgeLevel((i+2)%3))
	    		{
	    			split = RgbPrimitives::recursiveEdgeSplit(tmp,(i+1)%3,to);
	    		}
	    		else
	    		{
	    			split = RgbPrimitives::recursiveEdgeSplit(tmp,(i+2)%3,to);
	    		}
	    			
	    	}
	    	if (split)
	    	    continue;
	    	
	    	RgbTriangleC tmp2 = tmp.FF(i);
	    	int i2 = tmp.FFi(i);
	    	if (tmp2.isRed())
	    	{
    			assert(tmp2.getEdgeColor((i2+1)%3) == FaceInfo::EDGE_GREEN);
    			assert(tmp2.getEdgeColor((i2+2)%3) == FaceInfo::EDGE_GREEN);
    			assert(tmp2.getEdgeColor((i2)%3) == FaceInfo::EDGE_RED);
    			
	    		if (tmp2.getEdgeLevel((i2+1)%3) <= tmp2.getEdgeLevel((i2+2)%3))
	    		{
	    			split = RgbPrimitives::recursiveEdgeSplit(tmp2,(i2+1)%3,to);
	    		}
	    		else
	    		{
	    			split = RgbPrimitives::recursiveEdgeSplit(tmp2,(i2+2)%3,to);
	    		}
	    			
	    	}
	    	assert(split);
	    	if (split)
	    	    continue;
	    	
	    }
	    // ----------
	    
	    pos.FlipF();
	    pos.FlipE();
	    
	    while(pos.F() && (pos.F() != first))
	    {
	        RgbTriangleC tmp = RgbTriangleC(v.m,v.rgbInfo,pos.F()->Index());
	        assert(tmp.containVertex(v.index));
	        tmp.containVertex(v.index,&i);
	        assert(i>=0 && i<= 2);
	        // ----------
	        level = tmp.getEdgeLevel(i);
		    if (level < (minLevel - 1)  && tmp.getEdgeColor(i) == FaceInfo::EDGE_RED)
		    {
		    	if (tmp.isRed())
		    	{
		    		if (tmp.getEdgeLevel((i+1)%3) < tmp.getEdgeLevel((i+2)%3))
		    		{
		    			split = RgbPrimitives::recursiveEdgeSplit(tmp,(i+1)%3,to);
		    		}
		    		else
		    		{
		    			split = RgbPrimitives::recursiveEdgeSplit(tmp,(i+2)%3,to);
		    		}
		    			
		    	}
		    	if (split)
		    	    break;
		    	
		    	RgbTriangleC tmp2 = tmp.FF(i);
		    	int i2 = tmp.FFi(i);
		    	if (tmp2.isRed())
		    	{
		    		if (tmp2.getEdgeLevel((i2+1)%3) < tmp2.getEdgeLevel((i2+2)%3))
		    			split = RgbPrimitives::recursiveEdgeSplit(tmp2,(i2+1)%3,to);
		    		else
		    			split = RgbPrimitives::recursiveEdgeSplit(tmp2,(i2+2)%3,to);
		    			
		    	}
		    	if (split)
		    	    break;
		    	
		    }
	    	if (split)
	    	    continue;
		    // ----------
	        pos.FlipF();
	        pos.FlipE();
	        assert(pos.F()->V(0) == fp->V(fi) || pos.F()->V(1) == fp->V(fi) || pos.F()->V(2) == fp->V(fi));
	        assert(!fp->IsD());
	    }
	    
	    
	}
}

void RgbPrimitives::VF(RgbVertexC& v,vector<FacePointer>& vfp)
{
    assert(!v.vert().IsD());
	bool isBorder = v.getIsBorder();
    
    vcg::face::Pos<FaceType> pos(v.vert().VFp(),v.vert().VFi());
    
    RgbTriangleC t = RgbTriangleC(v.m,v.rgbInfo,pos.F()->Index());
    
    if (t.getNumberOfBoundaryEdge(&v) >= 2)
    {
    	vfp.push_back(pos.F());
    	return;
    }

    if (isBorder)       // if is border move cw until the border is found
    {
        pos.FlipE();
        pos.FlipF();
        
        while (!pos.IsBorder())
        {
            pos.FlipE();
            pos.FlipF();
        }
        
        pos.FlipE();
    }

    
    CMeshO::FacePointer first = pos.F(); 
    vfp.push_back(pos.F());
    pos.FlipF();
    pos.FlipE();
    
    while(pos.F() && (pos.F() != first))
    {
    	vfp.push_back(pos.F());
        
        pos.FlipF();
        pos.FlipE();
    }
    
}

void RgbPrimitives::updateNormal(RgbVertexC& v)
{
	vector<FacePointer> vfp;
	vfp.reserve(6);
	RgbPrimitives::VF(v,vfp);
	Point3f vnorm(0,0,0);
	int count = 0;
	for (unsigned int i = 0; i < vfp.size(); ++i) 
	{
		vcg::face::ComputeNormal(*(vfp[i]));
		vnorm += vfp[i]->cN();
		++count;
	}
	vnorm /= count; 
	v.vert().N() = vnorm;
}

void RgbPrimitives::VV(RgbVertexC& v, vector<RgbVertexC>& vv, bool onlyGreenEdge)
{
	// This VV is cw

	int i = 0;
	FacePointer fp = v.vert().VFp();
	int fi = v.vert().VFi(); 
	assert(fp->V(fi) == &(v.vert()));
	bool isBorder = v.getIsBorder();
	
    vcg::face::Pos<FaceType> pos(fp,fi);
    
    RgbTriangleC t = RgbTriangleC(v.m,v.rgbInfo,pos.F()->Index());
    
    if (t.getNumberOfBoundaryEdge(&v) >= 2)
    {
    	int index;
    	bool res = t.containVertex(v.index, &index);
    	assert(res);
    	
    	if (!onlyGreenEdge)
        {
        	vv.push_back(t.V((index+1)%3));
        	vv.push_back(t.V((index+2)%3));
        }
        else
        {
        	if ((t.getEdgeColor(index) == FaceInfo::EDGE_GREEN) && (t.getEdgeLevel(index) > t.getVl(index)))
        	{
        		vv.push_back(t.V((index+1)%3));
        	}
        	if ((t.getEdgeColor((index+2)%3) == FaceInfo::EDGE_GREEN) && (t.getEdgeLevel((index+2)%3) > t.getVl((index))))
        	{
        		vv.push_back(t.V((index+2)%3));
        	}
        }

    	return;
    }

    if (isBorder)       // if is border move ccw until the border is found
    {
        pos.FlipE();
        pos.FlipF();
        
        while (!pos.IsBorder())
        {
            pos.FlipE();
            pos.FlipF();
        }
        
        pos.FlipE();
    }

    CMeshO::FacePointer first = pos.F();     
    
    RgbTriangleC tmp = RgbTriangleC(v.m,v.rgbInfo,pos.F()->Index());

    assert(tmp.containVertex(v.index));
    tmp.containVertex(v.index,&i);
    assert(i>=0 && i<= 2);
    assert(tmp.V(i).index == v.index);

    if (!onlyGreenEdge)
    {
        if (isBorder)
        	vv.push_back(tmp.V((i+2)%3)); // we cannot scan all the triangle around the vertex, 
        								  // we have to save the last vertex
    	vv.push_back(tmp.V((i+1)%3));
    }
    else
    {
    	if (isBorder && (tmp.getEdgeColor((i+2)%3) == FaceInfo::EDGE_GREEN) && (tmp.getEdgeLevel((i+2)%3) > tmp.getVl(i)))
    		vv.push_back(tmp.V((i+2)%3));

    	if ((tmp.getEdgeColor(i) == FaceInfo::EDGE_GREEN) && (tmp.getEdgeLevel(i) > tmp.getVl(i)))
    		vv.push_back(tmp.V((i+1)%3));
    }
     
    
    pos.FlipF();
    pos.FlipE();
    
    while(pos.F() != first)
    {
        RgbTriangleC tmp = RgbTriangleC(v.m,v.rgbInfo,pos.F()->Index());
        assert(tmp.containVertex(v.index));
        tmp.containVertex(v.index,&i);
        assert(i>=0 && i<= 2);
        
        if (!onlyGreenEdge)
        {
        	vv.push_back(tmp.V((i+1)%3));
        }
        else
        {
        	if ((tmp.getEdgeColor(i) == FaceInfo::EDGE_GREEN) && (tmp.getEdgeLevel(i) > tmp.getVl(i)))
        		vv.push_back(tmp.V((i+1)%3));
        }
        
        if (pos.IsBorder())
        {
            break;
        }
        
        pos.FlipF();
        pos.FlipE();
        assert(pos.F()->V(0) == fp->V(fi) || pos.F()->V(1) == fp->V(fi) || pos.F()->V(2) == fp->V(fi));
        assert(!fp->IsD());
    }
}

unsigned int RgbPrimitives::baseIncidentEdges(RgbVertexC& v)
{
	int rank;
	if (v.getLevel() > 0)
	{
		rank = 6;
	}
	else
	{
		rank = v.getBaseArity();
	}
	return rank;
}

}
