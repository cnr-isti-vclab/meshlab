#include "controlPoint.h"
#include "vcg/simplex/face/component.h"

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

void ControlPoint::init(TriMeshType& m, RgbInfo& info)
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
			vr.setPl(vr.getCoord());

		}
	}

	for (unsigned int i = 0; i < m.vert.size(); ++i) 
	{
		VertexPointer v = & m.vert[i];
		if (!v->IsD() && v->VFp())
		{
			
			RgbTriangleC t = RgbTriangleC(m,info,v->VFp()->Index());
			int ti = v->VFi();
			if (!t.getVertexIsBorder(ti))
			{
				RgbVertexC vr = t.V(ti);
				assert(&vr.vert() == v);
				searchContribute(vr,false);
				assignPinf(vr,true);
			}
			else
			{
				RgbVertexC vr = t.V(ti);
				assert(&vr.vert() == v);
				searchContributeBoundary(vr,false);
				assignPinf(vr,true);
			}

		}
		
	}

}

void ControlPoint::findInitialStencil(RgbTriangleC& t, int EdgeIndex,int level, TopologicalOpC& to, vector<RgbVertexC>* indexes,vector<RgbVertexC>* firstVertex)
{
	bool isBorder = t.getEdgeIsBorder(EdgeIndex);
	
	RgbVertexC c1 = RgbPrimitives::findOppositeVertex(t,EdgeIndex,firstVertex);
	
	RgbTriangleC ot;
	int oi;
	RgbVertexC c2;

	if (!isBorder)
	{
		ot = t.FF(EdgeIndex);
		oi = t.FFi(EdgeIndex);
		c2 = RgbPrimitives::findOppositeVertex(ot,oi,firstVertex);
	}
	
	RgbPrimitives::splitGreenEdgeIfNeeded(t.V(EdgeIndex),level,to);
	RgbPrimitives::splitGreenEdgeIfNeeded(t.V((EdgeIndex+1)%3),level,to);
	RgbPrimitives::splitGreenEdgeIfNeeded(c1,level,to);
	if (!isBorder)
		RgbPrimitives::splitGreenEdgeIfNeeded(c2,level,to);
	
	if (indexes)
	{
		indexes->push_back(t.V(EdgeIndex));
		indexes->push_back(t.V((EdgeIndex+1)%3));
		indexes->push_back(c1);
		if (!isBorder)
			indexes->push_back(c2);
	}
	
}

void ControlPoint::assignPinf(RgbVertexC& v, bool initial)
{
	if (!v.getIsBorder())
	{
		assert(!v.getIsBorder());
	    assert(v.getLevel() == 0 || !v.getIsBorder());
		if (!initial)
		{
			assert(6 == v.getCount());
		}
		Point acc = v.getPinf();
		int rank = vertexRank(v);
		double an = alpha(rank);
		double c1 = (1.0 - (8.0*an)/(3.0+8.0*an));
		double c2 = ((8.0*an)/(rank*(3.0+8.0*an)));
	 	Point pinf = v.getPl();
		pinf *= c1;
		acc *= c2;
		pinf += acc; 
		v.setPinf(pinf);
		v.setIsPinfReady(true);
		updateP(v);
		
		addPinfContributeToVV(v);
		cleanTakenList(v);
	}
	else
	{
		assert(v.getIsBorder());
		Point acc = v.getPinf();
	 	Point pinf = v.getPl();
		pinf *= (2.0/3.0);
		acc *= (1.0/6.0);
		pinf += acc; 
		v.setPinf(pinf);
		v.setIsPinfReady(true);
		updateP(v);
		//addPinfContributeToVV(v);
	}
	
	
}

double ControlPoint::alpha(int n)
{
	return (5.0/8.0)-(pow((3.0+2.0*cos(2.0*M_PI/n)),2)/64.0);
}

double ControlPoint::gamma(int n,int k)
{
	return pow((5.0/8.0) - alpha(n),k);
}

ControlPoint::Point ControlPoint::computePkl(RgbVertexC& v, int kl)
{
	if (kl == v.getLevel())
		return v.getPl();
	assert(v.getIsPinfReady());
	if (!v.getIsBorder())
	{
		int n = vertexRank(v);
			
		int k = kl - v.getLevel();
		if (k < 0) 
			k = 0;
		
		double gnk = gamma(n,k);
		Point p1 = v.getPl();
		p1 *= gnk;
		Point p2 = v.getPinf();
		p2 *= (1.0 - gnk);
		return p1 + p2;
	}
	else
	{
		int k = kl - v.getLevel();
		if (k < 0) 
			k = 0;
		
		double cp = pow(1.0/4.0,k);
		
		Point p1 = v.getPl();
		p1 *= cp;
		Point p2 = v.getPinf();
		p2 *= (1.0 - cp);
		
		return p1 + p2;
	}
	
}
ControlPoint::Point ControlPoint::computePl(int l, vector<RgbVertexC>& stencil)
{
	assert(stencil.size() == 4);
	
	Point p0 = computePkl(stencil[0],l);
	p0 *= (3.0/8.0);
	Point p1 = computePkl(stencil[1],l);
	p1 *= (3.0/8.0);
	Point p2 = computePkl(stencil[2],l);
	p2 *= (1.0/8.0);
	Point p3 = computePkl(stencil[3],l);
	p3 *= (1.0/8.0);
	
	return p0 + p1 + p2 + p3;
}

ControlPoint::Point ControlPoint::computePlBoundary(int l,vector<RgbVertexC>& stencil)
{
	assert(stencil.size() >= 2);
	
	Point p0 = computePkl(stencil[0],l);
	p0 *= (1.0/2.0);
	Point p1 = computePkl(stencil[1],l);
	p1 *= (1.0/2.0);
	
	return p0 + p1;
}


void ControlPoint::updateP(RgbVertexC& v)
{
	if 	(v.getIsPinfReady())
	{
		 
		int minLevel = minimalEdgeLevel(v);
		Point tmp = computePkl(v,minLevel);
		v.setCoord(tmp);
	}
	RgbPrimitives::updateNormal(v);
}


void ControlPoint::addContribute(RgbVertexC& v,Point& p, bool update)
{
	//assert(v.getLevel() == 0 || v.getCount() < 6);
	//if (!(v.getLevel() == 0 || v.getCount() < 6))
	//	std::cerr << "cont: " << v.getCount() << "level" << v.getLevel() << std::endl;
	
	v.setCount(v.getCount() + 1);
	if (!v.getIsPinfReady())
	{
		Point temp = v.getPinf() + p; 
		v.setPinf(temp);
	}
	if (update)
	{
		if (v.getCount() == 6)
		{
			assignPinf(v);
		}
	}
}
void ControlPoint::removeContribute(RgbVertexC& v,Point& p)
{
	//if (v.getCount() <= 0)
	//	std::cerr << "conti: " << v.getCount() << std::endl;
	//assert(v.getCount() > 0);
	v.setCount(v.getCount() - 1);
	if (!v.getIsPinfReady())
	{
		Point temp = v.getPinf() - p; 
		v.setPinf(temp);
	}
	updateP(v);
}


bool ControlPoint::doSplit(RgbTriangleC& fp, int EdgeIndex, int level, TopologicalOpC& to , vector<FacePointer> *vfp, RgbVertexC* vNewInserted, vector<RgbVertexC>* vcont, vector<RgbVertexC>* vupd)
{
    assert(EdgeIndex >= 0 && EdgeIndex <= 2);

    vector<RgbVertexC> stencil;
	stencil.reserve(4);
	vector<RgbVertexC> firstVertexes;
	firstVertexes.reserve(4);

	RgbVertexC v1 = fp.V(EdgeIndex);
	RgbVertexC v2 = fp.V((EdgeIndex+1)%3);
	bool isBorder = fp.getEdgeIsBorder(EdgeIndex);

	findInitialStencil(fp,EdgeIndex,level,to,&stencil,&firstVertexes);

	fp.updateInfo();

	if (!RgbPrimitives::IsValidEdge(v1,v2))
	{
		//std::cerr << "different" << std::endl;
		return false; // The current split was already done by findInitialStencil
	}
	
	Point p;
    if (!isBorder)
    {
        p = computePl(level-1,stencil);
    }
    else
    {
    	p = computePlBoundary(level-1,stencil);
    }
	
	VertexPointer vp;
	vector<VertexPointer> vtemp;
	
	if (!isBorder)
	    to.doSplit(fp.face(),EdgeIndex, p, vfp, &vtemp);
	else
	    to.doSplitBoundary(fp.face(),EdgeIndex, p, vfp, &vtemp);
	
#ifndef NDEBUG
	std::cerr << fp.m->vn << std::endl;
#endif
	
	vp = vtemp[0];
	
	RgbVertexC vNew = RgbVertexC(*(fp.m),*(fp.rgbInfo),vp);
	vNew.resetInfo();
	vNew.setLevel(level);
	vNew.setPl(p);
	vNew.setIsNew(true);
	vNew.setIsBorder(isBorder);
	
	if (isBorder)
	{
		Point psum = computePkl(stencil[0],level);
		psum += computePkl(stencil[1],level);
		vNew.setPinf(psum);
		assignPinf(vNew,false);
	}
	
	if (vcont)
	{
		vcont->push_back(stencil[0]);
		vcont->push_back(stencil[1]);
		for (unsigned int i = 0; i < firstVertexes.size(); ++i)
			vcont->push_back(firstVertexes[i]);
	}
	
	if (vupd)
	{
 		vupd->push_back(stencil[0]);
		vupd->push_back(stencil[1]);
	}
	
	if (vNewInserted)
		*vNewInserted = vNew;

	ControlPoint::updateP(vNew);
	
	return true;
}

void ControlPoint::addToLists(RgbVertexC& dest, RgbVertexC& orig)
{
	// Optimization: no need to track anything if one of the vertex is a vertex of the original mesh
	if (dest.getLevel() == 0 || orig.getLevel() == 0)
		return;
	
	dest.taken().push_back(orig.index);
	orig.given().push_back(dest.index);
}

bool contain(std::list<int>& l, int e)
{
	for (std::list<int>::iterator i = l.begin(); i != l.end(); ++i) 
	{
		if (*i == e)
			return true;
	}
	return false;
}

void ControlPoint::removeFromLists(RgbVertexC& dest, RgbVertexC& orig)
{
	assert(contain(dest.taken(),orig.index));
	assert(contain(orig.given(),dest.index));
	dest.taken().remove(orig.index);
	orig.given().remove(dest.index);
}

void ControlPoint::listUpdateVertexRemoval(RgbVertexC& v, list<RgbVertexC>& l)
{
	cleanTakenList(v);
	
	list<int> templ = list<int>(v.given());
	
	for (std::list<int>::iterator i = templ.begin(); i != templ.end(); ++i) 
	{
		RgbVertexC orig = RgbVertexC(*v.m,*v.rgbInfo,*i);
		removeFromLists(orig,v);
		l.push_back(orig);
	}
}

bool ControlPoint::addContributeIfPossible(RgbVertexC& dest, RgbVertexC& orig, bool execute)
{
	if (dest.getLevel() == 0)
		return false;
	
	if (dest.getIsBorder())
	    return false;
	
	if ((dest.getLevel() == orig.getLevel()) && !dest.getIsPinfReady())
	{
		if (execute)
		{
			Point p = orig.getPl();
			addToLists(dest,orig);
			addContribute(dest,p);
		}
		return true; 
	}
	else
	{
		if (dest.getLevel() > orig.getLevel() && orig.getIsPinfReady() && !dest.getIsPinfReady()) // && (orig.getLevel() == 0))
		{
			if (execute)
			{
				Point p = computePkl(orig,dest.getLevel());
				addToLists(dest,orig);
				addContribute(dest,p);
			}
			return true;
		}
	}
	return false;
	
}

void ControlPoint::searchContribute(RgbVertexC& v,bool update)
{
	vector<RgbVertexC> vv;
	vv.reserve(6);
	
	RgbPrimitives::VV(v,vv);
	
	for (unsigned int i = 0; i < vv.size(); ++i) 
	{
		if (vv[i].getLevel() == v.getLevel())
		{
			Point p = vv[i].getPl();
			addContribute(v,p,update);
		}
	}
}

void ControlPoint::searchContributeBoundary(RgbVertexC& v,bool update)
{
	assert(v.getIsBorder());
	vector<RgbVertexC> vv;
	vv.reserve(6);
	
	RgbPrimitives::VV(v,vv,false);

	int last = vv.size() -1;
	
	assert(vv.size() >= 2);
	assert(vv[0].getIsBorder());
	assert(vv[last].getIsBorder());
	vector<RgbVertexC> vv2(2);
	vv2[0] = vv[0];
	vv2[1] = vv[last];
	
	for (int i = 0; i < 2; ++i)
	{
		if (vv2[i].getLevel() == v.getLevel())
		{
			Point p = vv2[i].getPl();
			addContribute(v,p,update);
		}
		else 
		{
			// pinf is computed during the vertex creation so it is available for all boundary points
			assert(vv2[i].getIsPinfReady());
			Point p = computePkl(vv2[i],v.getLevel());
			addContribute(v,p,update);
		}
	}
}


void ControlPoint::addPinfContributeToVV(RgbVertexC& v)
{
	assert(v.getIsPinfReady());
	vector<RgbVertexC> vv;
	vv.reserve(6);
	
	RgbPrimitives::VV(v,vv,true);
	
	for (unsigned int i = 0; i < vv.size(); ++i) 
	{
		if (vv[i].getLevel() > v.getLevel())
		{
			addContributeIfPossible(vv[i],v,true);
		}
	}
	
}

void ControlPoint::doCollapse(RgbTriangleC& fp, int EdgeIndex, TopologicalOpC& to, Point3<ScalarType> *p, vector<FacePointer> *vfp)
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

int ControlPoint::minimalEdgeLevel(RgbVertexC& v)
{
	int level;
    int i = 0;

    bool isBorder = v.getIsBorder();
	FacePointer fp = v.vert().VFp();
	int fi = v.vert().VFi();
    vcg::face::Pos<FaceType> pos(fp,fi);
    CMeshO::FacePointer first = pos.F();
    
    RgbTriangleC t = RgbTriangleC(v.m,v.rgbInfo,pos.F()->Index());
    if (t.getNumberOfBoundaryEdge(&v) >= 2)
    {
    	
    	int index;
    	bool res = t.containVertex(v.index, &index);
    	assert(res);
    	
    	int l1 = t.getEdgeLevel(index);
    	int l2 = t.getEdgeLevel((index+2)%3);
    	
    	if (l1 > l2)
    		return l2;
    	else
    		return l1;
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

    RgbTriangleC tmp = RgbTriangleC(v.m,v.rgbInfo,pos.F()->Index());
    assert(tmp.containVertex(v.index));
    tmp.containVertex(v.index,&i);
    assert(i>=0 && i<= 2);
    level = tmp.getEdgeLevel(i); 
    
    pos.FlipF();
    pos.FlipE();
    
    while(pos.F() != first)
    {
        RgbTriangleC tmp = RgbTriangleC(v.m,v.rgbInfo,pos.F()->Index());
        assert(tmp.containVertex(v.index));
        tmp.containVertex(v.index,&i);
        assert(i>=0 && i<= 2);
        if (tmp.getEdgeLevel(i) < level)
        	level = tmp.getEdgeLevel(i);

        if (pos.IsBorder())
        	break;
        
        pos.FlipF();
        pos.FlipE();
        assert(pos.F()->V(0) == fp->V(fi) || pos.F()->V(1) == fp->V(fi) || pos.F()->V(2) == fp->V(fi));
        assert(!fp->IsD());
    }
    return level;
}

void ControlPoint::vertexRemovalUpdate(RgbVertexC& v)
{
	list<RgbVertexC> l;
	listUpdateVertexRemoval(v,l);
	
	for (list<RgbVertexC>::iterator i = l.begin(); i != l.end(); ++i) 
	{
		RgbVertexC& vv = *i;
		if (!vv.getIsPinfReady())
		{
			if (vv.getLevel() > v.getLevel() && v.getIsPinfReady())  
			{
				Point p = computePkl(v,vv.getLevel());
				removeContribute(vv,p);
			}
			if (vv.getLevel() == v.getLevel())
			{
				Point p = v.getPl();
				removeContribute(vv,p);
			}
		}	
	}
	
}

void ControlPoint::cleanTakenList(RgbVertexC& v)
{
	list<int> templ = list<int>(v.taken());
	// If depends on other control point remove all dependencies
	for (std::list<int>::iterator i = templ.begin(); i != templ.end(); ++i) 
	{
		RgbVertexC orig = RgbVertexC(*v.m,*v.rgbInfo,*i);
		removeFromLists(v,orig);
	}
}

unsigned int ControlPoint::vertexRank(RgbVertexC& v)
{
	int rank;
	if (v.getLevel() > 0)
	{
		rank = 6;
	}
	else
	{
		rank = v.getCount();
	}
	return rank;
}


}
