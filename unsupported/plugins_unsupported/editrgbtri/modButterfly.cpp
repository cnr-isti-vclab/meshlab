#include "modButterfly.h"
#include "vcg/simplex/face/component.h"
#include <math.h>

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

namespace rgbt
{


void ModButterfly::init(TriMeshType& m, RgbInfo& info)
{
	for (unsigned int i = 0; i < m.vert.size(); ++i) 
	{
		//std::cerr << "index:" << i << std::endl;
		VertexPointer v = & m.vert[i];
		if (!v->IsD() && v->VFp())
		{
			
			RgbTriangleC t = RgbTriangleC(m,info,v->VFp()->Index());
			int ti = v->VFi();
			RgbVertexC vr = t.V(ti);
			assert(&vr.vert() == v);
			
			vector<RgbVertexC> vv;
			vv.reserve(10);
			assert(vv.size() == 0);
			RgbPrimitives::VV(vr,vv);
			assert(vv.size() >= 2);
			vr.setBaseArity(vv.size());

		}
	}

}

void ModButterfly::findHalfStencil(RgbVertexC& v, Pos& pos, vector<RgbVertexC>& stencil)
{
	assert(v.vp() == pos.v);
	
	Pos p;
	p = pos;
	p.FlipV();
	RgbVertexC v2 = RgbVertexC(*v.m,*v.rgbInfo,p.v);
	
	int maxlevel = v.getLevel();
	if (maxlevel < v2.getLevel()) 
		maxlevel = v2.getLevel();
	
	p = pos;
	rotate(v,p,2);
	RgbVertexC vtemp = move(v,p,maxlevel); 
	stencil.push_back(vtemp);

#ifndef NDEBUG
	Pos ptemp;
	ptemp = pos;
	ptemp.FlipV();
	rotate(v2,ptemp,2);
	RgbVertexC vtemp2 = move(v2,ptemp,maxlevel);
	assert(vtemp.index == vtemp2.index);
#endif

	p = pos;
	rotate(v,p,4);
	stencil.push_back(move(v,p,maxlevel));

	p = pos;
	p.FlipV();
	rotate(v2,p,4);
	stencil.push_back(move(v2,p,maxlevel));
}

bool ModButterfly::doSplit(RgbTriangleC& fp, int EdgeIndex, int level, TopologicalOpC& to , vector<FacePointer> *vfp)
{
	RgbVertexC v1ext;
	RgbVertexC v2ext;
	
    assert(EdgeIndex >= 0 && EdgeIndex <= 2);

	RgbVertexC v1 = fp.V(EdgeIndex);
	RgbVertexC v2 = fp.V((EdgeIndex+1)%3);
	bool isBorder = fp.getEdgeIsBorder(EdgeIndex);

	vector<RgbVertexC> stencil1;
	stencil1.reserve(3);
    vector<RgbVertexC> stencil2;
	stencil2.reserve(3);

	Point p;

    if (!isBorder)
    {
    	bool v1r = (baseArity(v1) != 6) || (v1.getIsBorder());
    	bool v2r = (baseArity(v2) != 6) || (v2.getIsBorder());

    	int maxlevel = v1.getLevel();
    	if (maxlevel < v2.getLevel()) 
    		maxlevel = v2.getLevel();
    	
    	if (v1r && v2r) // both the vertices are extraordinary
    	{
    		RgbPrimitives::splitGreenEdgeIfNeeded(v1,maxlevel + 1,to);
    		RgbPrimitives::splitRedEdgeIfNeeded(v1,maxlevel + 1,to);
    		RgbPrimitives::splitGreenEdgeIfNeeded(v2,maxlevel + 1,to);
    		RgbPrimitives::splitRedEdgeIfNeeded(v2,maxlevel + 1,to);
    		
    		assert(RgbPrimitives::IsValidEdge(v1,v2,&fp,&EdgeIndex));
    		RgbPrimitives::IsValidEdge(v1,v2,&fp,&EdgeIndex);
    		
	    	Pos pos1 = Pos(fp.face(),EdgeIndex);
	    	Pos pos2 = pos1;
	    	pos2.FlipF();
	    	pos2.FlipV();

    		p = computeExtraordinary(v1,pos1);
    		p += computeExtraordinary(v2,pos2);
    		p /= 2.0;
    	}
    	else if (v1r)
    	{
    		RgbPrimitives::splitGreenEdgeIfNeeded(v1,maxlevel + 1,to);
    		RgbPrimitives::splitRedEdgeIfNeeded(v1,maxlevel + 1,to);

    		assert(RgbPrimitives::IsValidEdge(v1,v2,&fp,&EdgeIndex));
    		RgbPrimitives::IsValidEdge(v1,v2,&fp,&EdgeIndex);

    		Pos pos1 = Pos(fp.face(),EdgeIndex);
    		p = computeExtraordinary(v1,pos1);
    	}
    	else if (v2r)
    	{
    		RgbPrimitives::splitGreenEdgeIfNeeded(v2,maxlevel + 1,to);
    		RgbPrimitives::splitRedEdgeIfNeeded(v2,maxlevel + 1,to);

    		assert(RgbPrimitives::IsValidEdge(v1,v2,&fp,&EdgeIndex));
    		RgbPrimitives::IsValidEdge(v1,v2,&fp,&EdgeIndex);

    		Pos pos2 = Pos(fp.face(),EdgeIndex);

    		pos2.FlipV();
    		p = computeExtraordinary(v2,pos2);
    	}
    	else
    	{
	    	// Additional split

	    	RgbPrimitives::splitGreenEdgeIfNeeded(v1,maxlevel+1,to);
	    	RgbPrimitives::splitGreenEdgeIfNeeded(v2,maxlevel+1,to);
	    	RgbPrimitives::splitRedEdgeIfNeeded(v1,maxlevel+1,to);
	    	RgbPrimitives::splitRedEdgeIfNeeded(v2,maxlevel+1,to);
	    	RgbPrimitives::splitGreenEdgeIfNeeded(v1,maxlevel+1,to);
	    	RgbPrimitives::splitGreenEdgeIfNeeded(v2,maxlevel+1,to);
	    	RgbPrimitives::splitRedEdgeIfNeeded(v1,maxlevel+1,to);
	    	RgbPrimitives::splitRedEdgeIfNeeded(v2,maxlevel+1,to);
	    	
	    	
	    	if (!RgbPrimitives::IsValidEdge(v1,v2,&fp,&EdgeIndex))
	    	{
	    		return false; // split already done
	    	}
	    	// --------
	    	
	    	Pos pos1 = Pos(fp.face(),EdgeIndex);
	    	Pos pos2 = pos1;
	    	pos2.FlipF();
	    	pos2.FlipV();
	    	
	    	findHalfStencil(v1,pos1,stencil1);
	    	findHalfStencil(v2,pos2,stencil2);
	    	
	    	assert(stencil1.size() == 3);
	    	assert(stencil2.size() == 3);
	    	
	    	Point acc;
	    	Point pt;
	    	
	    	pt = v1.getCoord(); 
	    	pt *= 1.0/2.0;
	    	acc = pt;
	    	
	    	pt = v2.getCoord(); 
	    	pt *= 1.0/2.0;
	    	acc += pt;
	
	    	pt = stencil1[0].getCoord(); 
	    	pt *= 1.0/8.0;
	    	acc += pt;
	    	
	    	pt = stencil2[0].getCoord(); 
	    	pt *= 1.0/8.0;
	    	acc += pt;
	    	
	    	pt = stencil1[1].getCoord(); 
	    	pt *= 1.0/16.0;
	    	acc -= pt;
	
	    	pt = stencil1[2].getCoord(); 
	    	pt *= 1.0/16.0;
	    	acc -= pt;
	
	    	pt = stencil2[1].getCoord(); 
	    	pt *= 1.0/16.0;
	    	acc -= pt;
	
	    	pt = stencil2[2].getCoord(); 
	    	pt *= 1.0/16.0;
	    	acc -= pt;
	    	
	    	p = acc;
    	}
    }
    else
    {
    	// Boundary
    	// Additional split
    	int maxlevel = v1.getLevel();
    	if (maxlevel < v2.getLevel()) 
    		maxlevel = v2.getLevel();

    	RgbPrimitives::splitGreenEdgeIfNeeded(v1,maxlevel+1,to);
    	RgbPrimitives::splitGreenEdgeIfNeeded(v2,maxlevel+1,to);
    	
    	if (!RgbPrimitives::IsValidEdge(v1,v2,&fp,&EdgeIndex))
    	{
    		return false; // split already done
    	}
    	// --------
    	
    	Pos pos1 = Pos(fp.face(),EdgeIndex);
    	Pos pos2 = pos1;
    	pos2.FlipV();
    	
    	rotateUntilBorder(v1,pos1);
    	rotateUntilBorder(v2,pos2);
    	
    	v1ext = move(v1,pos1,maxlevel);
    	v2ext = move(v2,pos2,maxlevel);
    	
    	assert(v1ext.getIsBorder());
    	assert(v2ext.getIsBorder());
    	
    	Point acc;
    	Point pt;
    	
    	pt = v1.getCoord(); 
    	pt *= 9.0/16.0;
    	acc = pt;
    	
    	pt = v2.getCoord(); 
    	pt *= 9.0/16.0;
    	acc += pt;

    	pt = v1ext.getCoord(); 
    	pt *= 1.0/16.0;
    	acc -= pt;
    	
    	pt = v2ext.getCoord(); 
    	pt *= 1.0/16.0;
    	acc -= pt;
    	
    	p = acc;
    }
	
	VertexPointer vp;
	vector<VertexPointer> vtemp;
	
	if (!isBorder)
	    to.doSplit(fp.face(),EdgeIndex, p, vfp, &vtemp);
	else
	    to.doSplitBoundary(fp.face(),EdgeIndex, p, vfp, &vtemp);
	
	vp = vtemp[0];
	
	RgbVertexC vNew = RgbVertexC(*(fp.m),*(fp.rgbInfo),vp);
	vNew.resetInfo();
	vNew.setLevel(level);
	vNew.setCoord(p);
	vNew.setIsNew(true);
	vNew.setIsBorder(isBorder);
	
	vector<RgbVertexC> vv;
	RgbPrimitives::VV(vNew,vv,false);
	
	RgbPrimitives::updateNormal(vNew);
	for(vector<RgbVertexC>::iterator i = vv.begin(); i != vv.end(); i++)
		RgbPrimitives::updateNormal(*i);
	
	return true;
}

void ModButterfly::doCollapse(RgbTriangleC& fp, int EdgeIndex, TopologicalOpC& to, Point3<ScalarType> *p, vector<FacePointer> *vfp)
{
    //assert(!fp.V(EdgeIndex).getIsNew());
    if (!fp.getEdgeIsBorder(EdgeIndex))
        to.doCollapse(fp.face(),EdgeIndex, p, vfp);
    else
        to.doCollapseBoundary(fp.face(),EdgeIndex, p, vfp);
    
#ifndef NDEBUG
    std::cerr << fp.m->vn << std::endl;
#endif

}

void ModButterfly::rotate(RgbVertexC& v, Pos& pos,int angle)
{
	//std::cerr << v.index << " requested: " << angle << " ----- ";
	assert(v.vp() == pos.v);
	while (angle > 0)
	{
		RgbTriangleC t = RgbTriangleC(v.m,v.rgbInfo,pos.F()->Index());
		int realz = -1;
		for (int i = 0; i<3; i++)
			if (t.V(i).vp() == pos.v)
					realz = i;
		assert(realz != -1);
		
		angle -= t.getAngle(realz);
		//std::cerr << "| " << t.getAngle(realz);
		pos.FlipE();
		pos.FlipF();
	}
	//std::cerr << " ----- result : " << angle << std::endl;
	//assert(angle == 0);
}

ModButterfly::RgbVertexC ModButterfly::move(RgbVertexC& v, Pos& pos, int level)
{
	assert(v.vp() == pos.v);
	assert(level >= v.getLevel());
	int vl = level;
	RgbVertexC lastv;
	do 
	{
		pos.FlipV();
		lastv = RgbVertexC(*v.m,*v.rgbInfo,pos.v);
		if (lastv.getLevel() <= vl)
			break; // no need to continue, in fact a rotation of exactly 6 can be impossible in some cases
		rotate(lastv,pos,6);
		pos.FlipF();
		
		assert(lastv.getLevel() > vl);
	} while (lastv.getLevel() > vl);
	
	return RgbVertexC(*v.m,*v.rgbInfo,pos.v);
}

void ModButterfly::rotateUntilBorder(RgbVertexC& v, Pos& pos)
{
	assert(v.vp() == pos.v);
	do 
	{
		pos.FlipE();
		pos.FlipF();
	} while (!pos.IsBorder());
	assert(v.vp() == pos.v);
}

ModButterfly::Point ModButterfly::computeExtraordinary(RgbVertexC& v, Pos& pos)
{
	int rank;
	if (v.getLevel() == 0) 
		rank = baseArity(v);
	else
		rank = 4;
	
	vector<Point> points;
	vector<double> coeff;
	computeExtraordinaryPattern(coeff,rank);
	points.reserve(rank+1);
	points.push_back(v.getCoord());
	
	if (!v.getIsBorder())
	{
		int angle = rank * 2;
		do
		{
			pos.FlipV();
			points.push_back(pos.v->P());
			assert(pos.v != v.vp());
			pos.FlipV();
			rotate(v,pos,2);
			angle -= 2;
			assert (angle >= 0);
		} 
		while (angle != 0);
	}
	else
	{
		Pos pos2 = pos;
		// Border
		int angle = (rank-1) * 2;
		do
		{
			pos.FlipV();
			points.push_back(pos.v->P());
			assert(pos.v != v.vp());
			pos.FlipV();
			assert(pos.v == v.vp());
			rotate(v,pos,2);
			angle -= 2;
			assert (angle >= 0);
		} 
		while ((!pos.IsBorder()) && (angle != 0));
		
		pos.FlipV();
		points.push_back(pos.v->P());
		assert(pos.v != v.vp());
		pos.FlipV();
		
		assert(angle >= 0);
		vector<Point> pointsreverse;

		pos2.FlipF();
		rotate(v,pos2,2);
		angle -= 2;
		if ((!pos2.IsBorder()) && (angle != 0))
		{
			do
			{
				pos2.FlipV();
				pointsreverse.push_back(pos2.v->P());
				assert(pos2.v != v.vp());
				pos2.FlipV();
				assert(pos2.v == v.vp());
				rotate(v,pos2,2);
				angle -= 2;
				assert (angle >= 0);
			} 
			while ((!pos2.IsBorder()) && (angle != 0));
		}
		assert(angle == 0);

		pos.FlipV();
		points.push_back(pos.v->P());
		assert(pos.v != v.vp());
		pos.FlipV();
		
		for(int i=pointsreverse.size()-1;i>= 0; i--)
			points.push_back(pointsreverse[i]);
		
	}

	assert(points.size() == (unsigned int)(rank + 1));
	assert(points.size() == coeff.size());
	
	Point acc = Point(0.0,0.0,0.0);
	
	for (int i=0; i<rank+1; i++)
	{
		Point tmp = points[i] * coeff[i];
		acc += tmp;
	}
	
	return acc;
}

void ModButterfly::computeExtraordinaryPattern(vector<double>& pattern, int k)
{
	assert(k >= 3);
	pattern.clear();
	pattern.reserve(k+1);
	pattern.push_back(3.0/4.0);
	
	switch (k) {
		case 3:
			pattern.push_back(5.0/12.0);
			pattern.push_back(-(1.0/12.0));
			pattern.push_back(-(1.0/12.0));
			break;
		case 4:
			pattern.push_back(3.0/8.0);
			pattern.push_back(0);
			pattern.push_back(-(1.0/8.0));
			pattern.push_back(0);
			break;
		default:
			for (int i = 0; i < k; i++)
			{
				double kd = k;
				double id = i;
				double v = (1.0/kd)*((1.0/4.0) + cos((2.0*id*M_PI)/kd) + ((1.0/2.0)*cos((4.0*id*M_PI)/kd)));
				pattern.push_back(v);
			}
			break;
	}
}

int ModButterfly::baseArity(RgbVertexC& v)
{
	int arity;
	if (v.getLevel() > 0)
	{
		arity = 6;
	}
	else
	{
		arity = v.getBaseArity();
	}
	return arity;
}


}
