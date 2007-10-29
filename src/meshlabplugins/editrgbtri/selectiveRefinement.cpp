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

#include "selectiveRefinement.h"

namespace rgbt
{

SelectiveRefinement::SelectiveRefinement(CMeshO& m, RgbInfo& i, TopologicalOpC& to)
{
	this->m = &m;
	this->info = &i;
	this->to = &to;
	isRunning = false;
}

SelectiveRefinement::~SelectiveRefinement()
{
}

bool SelectiveRefinement::edgeNeedRefinement(RgbTriangleC& t, int EdgeIndex)
{
	if (isInBox(t,EdgeIndex))
		return ((edgeLenght(t,EdgeIndex) > maxEdgeLenghtInBox) && t.getEdgeLevel(EdgeIndex) < maxEdgeLevelInBox);
	else
		return ((edgeLenght(t,EdgeIndex) > maxEdgeLenght) && t.getEdgeLevel(EdgeIndex) < minEdgeLevel);
}
bool SelectiveRefinement::vertexCanBeRemoved(RgbTriangleC& t, int VertexIndex)
{
	RgbVertexC v = t.V(VertexIndex);
	if (!IsValidVertex(v.index,m,info))
		return false;
	
	if (isInBox(extractCoord(t,VertexIndex)))
		return false;
	else 
		return true;
}

double SelectiveRefinement::calculatePriorityEdge(RgbTriangleC& t, int EdgeIndex)
{
	double res = 0;
	res += edgeLenght(t,EdgeIndex);
	return res;
	
}
double SelectiveRefinement::calculatePriorityVertex(RgbTriangleC& t, int VertexIndex)
{
	RgbVertexC v = t.V(VertexIndex);
	return t.getVl(VertexIndex) + maxEdge(v);
}

double SelectiveRefinement::calculatePriorityEdgeComplex(RgbTriangleC& t, int EdgeIndex)
{
	double res = 0;
	if (isInBox(t,EdgeIndex))
		res += edgeLenght(t,EdgeIndex)-maxEdgeLenghtInBox;
	else
		res += edgeLenght(t,EdgeIndex)-maxEdgeLenght;
	return res;
	
}
double SelectiveRefinement::calculatePriorityVertexComplex(RgbTriangleC& t, int VertexIndex)
{
	RgbVertexC v = t.V(VertexIndex);
	double res = 0;
	if (isInBoxV(t,VertexIndex))
		res += maxEdge(v)-maxEdgeLenghtInBox;
	else
		res += maxEdge(v)-maxEdgeLenght;
	return res;
}

void SelectiveRefinement::start(bool interactive, bool simpleA)
{
	if (isRunning)
		return;
	isRunning = true;
	isCompletedCoarsening = false;
	simpleAlgorithm = simpleA;
	init();
	if (!interactive)
	{
		int i = 0;
		while (step())
		{
			i++;
		}
		stop();
	}
}

bool SelectiveRefinement::stepSimple()
{
	while (!coarseningQueue.empty())
	{
		RefOp ro = coarseningQueue.top();
		coarseningQueue.pop();
		if (coarseningQueue.empty())
			isCompletedCoarsening = true;
		
		VertexType& v = m->vert[ro.v1];

		RgbTriangleC t = RgbTriangleC(m,info,v.VFp()->Index());
		int index = v.VFi();
		
		if (IsValidVertex(ro.v1,m,info,&t,&index))
		{
			if (RgbPrimitives::vertexRemoval_Possible(t,index))
			{
				vector<RgbTriangleC> vt;
				vt.reserve(4);
				RgbPrimitives::vertexRemoval(t,index,*to,&vt);

				for (unsigned int i = 0; i < vt.size(); ++i) 
				{
					for (int j = 0; j < 3; ++j) 
					{
						int v = vt[i].V(j).index;
						assert((ro.v1 == v) || (!m->vert[v].IsD()));
						if (vertexCanBeRemoved(vt[i],j))
						{
							coarseningQueue.push(RefOp(v,calculatePriorityVertex(vt[i],j)));
						}
					}
				}
				return true;
			}
		}
	}
	
	if (m->fn > maxTriangles)
		return false;
	
	while (!refinementQueue.empty())
	{
		RefOp ro = refinementQueue.top();
		refinementQueue.pop();
		
		RgbTriangleC t;
		int index;
		
		if (IsValidEdge(ro.v1,ro.v2,m,info,&t,&index))
		{
			vector<RgbTriangleC> vt;
			RgbPrimitives::recursiveEdgeSplit(t,index,*to,&vt);
			
			// it's necessary to found the common vertex of every 4 consecutivetriangles in vt
			for (unsigned int i = 0; i < vt.size(); ++i) 
			{
				for (int j = 0; j < 3; ++j) 
				{
					int v1 = vt[i].V((j+0)%3).index;
					int v2 = vt[i].V((j+1)%3).index;
					if (vt[i].getEdgeColor(j) == FaceInfo::EDGE_GREEN)
					{
						if ((v1 > v2) || vt[i].getEdgeIsBorder(j))  // v1 v2 and v2 v1 are the same edge (not true on the border)
							if (edgeNeedRefinement(vt[i],j))
								refinementQueue.push(RefOp(v1,v2,calculatePriorityEdge(vt[i],j)));
					}
				}
			}
			return true;
		}
	}
	
	return false;
}

void SelectiveRefinement::stepComplex_aux(vector<RgbTriangleC>& vt)
{
	for (unsigned int i = 0; i < vt.size(); ++i) 
	{
		for (int j = 0; j < 3; ++j) 
		{
			int v = vt[i].V(j).index;
			
			if (IsValidVertex(v,m,info))
			{
				coarseningQueue.push(RefOp(v,calculatePriorityVertexComplex(vt[i],j)));
			}

		}
	}
	
	for (unsigned int i = 0; i < vt.size(); ++i) 
	{
		for (int j = 0; j < 3; ++j) 
		{
			int v1 = vt[i].V((j+0)%3).index;
			int v2 = vt[i].V((j+1)%3).index;
			if (vt[i].getEdgeColor(j) == FaceInfo::EDGE_GREEN)
			{
				if ((v1 > v2)  || vt[i].getEdgeIsBorder(j))  // v1 v2 and v2 v1 are the same edge
					if (IsValidEdge(vt[i].V(j).index,vt[i].V((j+1)%3).index,m,info))
						refinementQueue.push(RefOp(v1,v2,calculatePriorityEdgeComplex(vt[i],j)));
			}
		}
	}
}

bool SelectiveRefinement::stepComplex()
{
	while (true)
	{
		if (coarseningQueue.size() == 0)
		{
			if (m->fn > maxTriangles) // 2
			{
				return false;
			}
			break;
		}
		else
		{
			if (coarseningQueue.top().priority <= 0 || m->fn > maxTriangles) // 1
			{
				// Remove a node

				RefOp ro = coarseningQueue.top();
				coarseningQueue.pop();
				if (coarseningQueue.empty())
					isCompletedCoarsening = true;

				VertexType& v = m->vert[ro.v1];
				RgbTriangleC t;
				int index;

				if (IsValidVertex(ro.v1,m,info,&t,&index))
				{
					if (RgbPrimitives::vertexRemoval_Possible(t,index))
					{
						vector<RgbTriangleC> vt;
						vt.reserve(4);
						RgbPrimitives::vertexRemoval(t,index,*to,&vt);
						
						stepComplex_aux(vt); 
						
						return true;
					}
				}
			}
			else
				break;
		}
	}

	if (refinementQueue.top().priority <= 0)
	{
		return false;
	}

	while (true)
	{
		if (refinementQueue.empty())
		{
			break;
		}
		
		RefOp ro = refinementQueue.top();
		refinementQueue.pop();
		
		RgbTriangleC t;
		int index;
		
		if (IsValidEdge(ro.v1,ro.v2,m,info,&t,&index))
		{
			vector<RgbTriangleC> vt;
			RgbPrimitives::recursiveEdgeSplit(t,index,*to,&vt);
			stepComplex_aux(vt);
			return true;
		}
	}
	return false;
}

bool SelectiveRefinement::step()
{
	if (!isRunning)
		return false;
	
	if (simpleAlgorithm)
		return stepSimple();
	else
		return stepComplex();
	
}
void SelectiveRefinement::stop()
{
	isRunning = false;
}

bool SelectiveRefinement::IsValidEdge(int v1,int v2, CMeshO* m,RgbInfo* info, RgbTriangleC* t, int* ti)
{
	assert((unsigned int)v1 < m->vert.size());
	assert((unsigned int)v2 < m->vert.size());
	
	if (m->vert[v1].IsD() || m->vert[v2].IsD())
		return false;
	
	VertexType& v = m->vert[v1];
	RgbTriangleC tf = RgbTriangleC(m,info,v.VFp()->Index());
	int tfi = v.VFi();
	assert(tf.V(tfi).index == v1);

	VertexType& va = m->vert[v2];
	RgbTriangleC tfa = RgbTriangleC(m,info,va.VFp()->Index());
	int tfia = va.VFi();
	assert(tfa.V(tfia).index == v2);
	
	vector<RgbTriangleC> vf;
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

bool SelectiveRefinement::IsValidVertex(int vp, CMeshO* m,RgbInfo* info, RgbTriangleC* t, int* ti, bool ignoreNew)
{
	assert((unsigned int)vp < m->vert.size());
	if (m->vert[vp].IsD())
		return false;
	VertexType& v = m->vert[vp];
	if (!v.VFp())
		return false;

	RgbTriangleC tf = RgbTriangleC(m,info,v.VFp()->Index());
	assert(!tf.face()->IsD());
	int tfi = v.VFi();
	assert(tf.V(tfi).index == vp);

	if (tf.getVertexIsNew(tfi) && !ignoreNew)
		return false;

	if (t)
		*t = tf;
	if (ti)
		*ti = tfi;
	
	return true;
}


void SelectiveRefinement::init_queues()
{
	// Search for edge Split
	for (unsigned int i = 0; i < m->face.size(); ++i) 
	{
		FaceType& f = m->face[i];

		if (!f.IsD())
		{
			RgbTriangleC rt = RgbTriangleC(*m,*info,f.Index());
			for (int v = 0; v < 3; ++v) 
			{
				if (f.V(v) > f.V((v+1)%3) || rt.getEdgeIsBorder(v)) // is true once for each edge
				{
					if (rt.getEdgeColor(v) == FaceInfo::EDGE_GREEN && edgeNeedRefinement(rt,v))
					{
						refinementQueue.push(RefOp(rt.V(v).index,rt.V((v+1)%3).index,calculatePriorityEdge(rt,v)));
					}
				}
			}
		}
	}

	// Search for vertexRemoval
	int nvert = m->vert.size();
	vector<bool> vb(nvert,false);
	vector<RefOp> vr(nvert);
	
	for (unsigned int i = 0; i < m->face.size(); ++i) 
	{
		FaceType& f = m->face[i];
		
		if (!f.IsD())
		{
			RgbTriangleC rt = RgbTriangleC(*m,*info,f.Index());
			for (int v = 0; v < 3; ++v) 
			{
				RgbVertexC vr = rt.V(v);
				vr.setIsNew(false);
				if (vertexCanBeRemoved(rt,v) && RgbPrimitives::vertexRemoval_Possible(rt,v))
				{
					coarseningQueue.push(RefOp(rt.V(v).index,calculatePriorityVertex(rt,v)));
				}
			}
		}
	}
}

void SelectiveRefinement::init_queuesComplex()
{
	// Search for edge Split
	for (unsigned int i = 0; i < m->face.size(); ++i) 
	{
		FaceType& f = m->face[i];

		if (!f.IsD())
		{
			RgbTriangleC rt = RgbTriangleC(*m,*info,f.Index());
			for (int v = 0; v < 3; ++v) 
			{
				if (f.V(v) > f.V((v+1)%3) || rt.getEdgeIsBorder(v)) // is true once for each edge
				{
					if (rt.getEdgeColor(v) == FaceInfo::EDGE_GREEN && IsValidEdge(rt.V(v).index,rt.V((v+1)%3).index,m,info))
					{
						refinementQueue.push(RefOp(rt.V(v).index,rt.V((v+1)%3).index,calculatePriorityEdgeComplex(rt,v)));
					}
				}
			}
		}
	}

	// Search for vertexRemoval
	int nvert = m->vert.size();
	vector<bool> vb(nvert,false);
	vector<RefOp> vr(nvert);
	
	for (unsigned int i = 0; i < m->face.size(); ++i) 
	{
		FaceType& f = m->face[i];
		
		if (!f.IsD())
		{
			RgbTriangleC rt = RgbTriangleC(*m,*info,f.Index());
			for (int v = 0; v < 3; ++v) 
			{
				RgbVertexC vr = rt.V(v);
				vr.setIsNew(false);
				if (IsValidVertex(vr.index,m,info))
				{
					coarseningQueue.push(RefOp(rt.V(v).index,calculatePriorityVertexComplex(rt,v)));
				}
			}
		}
	}
}


void SelectiveRefinement::init()
{
	box.SetNull();
	while(!refinementQueue.empty())
		refinementQueue.pop();
	while(!coarseningQueue.empty())
		coarseningQueue.pop();
	
	// Calculate bounding box
	for (unsigned int i = 0; i < m->face.size(); ++i) 
	{
		FaceType& f = m->face[i];
		if (f.IsS() && !f.IsD())
		{
			for (int v = 0; v < 3; ++v) 
			{
				box.Add(f.V(v)->P());
			}
		}
	}

	if (simpleAlgorithm)
		init_queues();
	else
		init_queuesComplex();
	
}

double SelectiveRefinement::edgeLenght(RgbTriangleC& t, int index)
{
	Point v1 = t.getVertexCoord(index);
	Point v2 = t.getVertexCoord((index+1)%3);
	return (v2-v1).Norm();
}

bool SelectiveRefinement::isInBox(Point& p)
{
	return box.IsIn(p);
}

bool SelectiveRefinement::isInBox(RgbTriangleC& t, int EdgeIndex)
{
	Point v1 = extractCoord(t,EdgeIndex);
	Point v2 = extractCoord(t,(EdgeIndex+1)%3);
	return (isInBox(v1) && isInBox(v2));
}

bool SelectiveRefinement::isInBoxV(RgbTriangleC& t, int EdgeIndex)
{
    Point v1 = extractCoord(t,EdgeIndex);
    return isInBox(v1);
}

SelectiveRefinement::Point& SelectiveRefinement::extractCoord(RgbTriangleC& t, int VertexIndex)
{
	return t.getVertexCoord(VertexIndex);
}

double SelectiveRefinement::minEdge(RgbVertexC& v)
{
	vector<double> vv;
	vv.reserve(6);
	VE(v,vv);
	double value = vv[0];
	for (unsigned int i = 1; i < vv.size(); ++i) 
	{
		if (vv[i] < value)
			value = vv[i];
	}
	return value;
}

double SelectiveRefinement::maxEdge(RgbVertexC& v)
{
	vector<double> vv;
	vv.reserve(6);
	VE(v,vv);
	double value = vv[0];
	for (unsigned int i = 1; i < vv.size(); ++i) 
	{
		if (vv[i] > value)
			value = vv[i];
	}
	return value;
}

void SelectiveRefinement::VE(RgbVertexC& v, vector<double>& vv)
{
    int i;

	FacePointer fp = v.vert().VFp();
	int fi = v.vert().VFi();
    vcg::face::Pos<FaceType> pos(fp,fi);
    CMeshO::FacePointer first = pos.F(); 

    RgbTriangleC tmp = RgbTriangleC(v.m,v.rgbInfo,pos.F()->Index());
    assert(tmp.containVertex(v.index));
    tmp.containVertex(v.index,&i);
    assert(i>=0 && i<= 2);
    
    vv.push_back(edgeLenght(tmp,i));
    
    pos.FlipF();
    pos.FlipE();
    
    while(pos.F() && (pos.F() != first))
    {
        RgbTriangleC tmp = RgbTriangleC(v.m,v.rgbInfo,pos.F()->Index());
        assert(tmp.containVertex(v.index));
        tmp.containVertex(v.index,&i);
        assert(i>=0 && i<= 2);

        vv.push_back(edgeLenght(tmp,i));
        
        pos.FlipF();
        pos.FlipE();
        assert(pos.F()->V(0) == fp->V(fi) || pos.F()->V(1) == fp->V(fi) || pos.F()->V(2) == fp->V(fi));
        assert(!fp->IsD());
    }
}

}
