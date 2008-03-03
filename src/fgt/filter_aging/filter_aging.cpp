/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
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
*                                                                           *
****************************************************************************/


#include <map>
#include <vector>
//#include <iostream>        // DEBUG

#include "filter_aging.h"

#include <vcg/complex/trimesh/allocate.h>
#include <vcg/complex/trimesh/update/normal.h>


// Constructor 
GeometryAgingPlugin::GeometryAgingPlugin() 
{ 
    typeList << FP_ERODE;
    
    foreach(FilterIDType tt, types())
        actionList << new QAction(filterName(tt), this);
}


// Destructor 
GeometryAgingPlugin::~GeometryAgingPlugin() 
{ 
}


// Returns the very short string describing each filtering action
const QString GeometryAgingPlugin::filterName(FilterIDType filterId) 
{
    switch(filterId) {
        case FP_ERODE:
            return QString("Aging Simulation");
        default:
            assert(0);
    }
}


// Returns the longer string describing each filtering action
const QString GeometryAgingPlugin::filterInfo(FilterIDType filterId) 
{
    switch(filterId) {
        case FP_ERODE: 
            return QString("Simulate the aging effects due to small collisions or various chipping events");
        default: 
            assert(0);
    }
}


// Returns plugin info
const PluginInfo &GeometryAgingPlugin::pluginInfo() 
{
    static PluginInfo ai;
    ai.Date = tr(__DATE__);
    ai.Version = tr("0.1");
    ai.Author = tr("Marco Fais");
    return ai;
 }


// Initializes the list of parameters (called by the auto dialog framework)
void GeometryAgingPlugin::initParameterSet(QAction *action, MeshModel &m, FilterParameterSet &params)
{
    switch(ID(action)) {
        case FP_ERODE:
            params.addAbsPerc("AngleThreshold", 60.0, 10.0, 170.0,  "Angle Threshold", "The minimum angle between two adjacent faces to consider the edge they are sharing.");
            params.addAbsPerc("EdgeLenThreshold", m.cm.bbox.Diag()*0.01, 0.0, m.cm.bbox.Diag(),"Edge Len Threshold", "The minimum length of an edge. Usefull to avoid the creation of too many small faces.");
            break;
        default:
            assert(0);
    }
}


// The Real Core Function doing the actual mesh processing
bool GeometryAgingPlugin::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet &params, vcg::CallBackPos *cb)
{
    float angleThreshold;
    float edgeLenTreshold;
    
    switch(ID(filter)) {
        case FP_ERODE:
            angleThreshold = params.getAbsPerc("AngleThreshold");
            edgeLenTreshold = params.getAbsPerc("EdgeLenThreshold");
            return refineNeededEdges(m.cm, FractPoint(), EdgePred(angleThreshold, edgeLenTreshold), hasSelected(m), cb);
        default:
            assert(0);
    }
}


// checks the mesh looking for selected faces
bool GeometryAgingPlugin::hasSelected(MeshModel &m)
{
    CMeshO::FaceIterator fi;
    for(fi=m.cm.face.begin(); fi!=m.cm.face.end(); ++fi) {
		if(!(*fi).IsD() && (*fi).IsS()) return true;
	}
	return false;
}


// randomly creates a vector of eroderPoints and returns a pointer to it
vector<GeometryAgingPlugin::EroderPoint>* GeometryAgingPlugin::generateEdgeEroder(CMeshO::FacePointer f, int z)
{
    vector<EroderPoint>* e = new vector<EroderPoint>();
    
    srand(time(NULL));
    
    //std::cout << "len: " << len << std::endl;           // DEBUG
    if(f->IsB(z))
        e->push_back(GeometryAgingPlugin::EroderPoint(0.5, 0.0));
    /*else
        e->push_back(GeometryAgingPlugin::EroderPoint(0.5, 0.0, 0.0));*/
    
    return e;
}


// refines the edges which will be eroded in the next phase
bool GeometryAgingPlugin::refineNeededEdges(CMeshO &m, FractPoint frpoi, EdgePred ep, bool hasSelected, CallBackPos *cb)
{
    int j, newVertNum = 0, newFaceNum = 0;
    typedef std::pair<CMeshO::VertexPointer, CMeshO::VertexPointer> vvpair;
    typedef std::pair<vvpair, int> vvi;
    map<vvi, CMeshO::VertexPointer> edge2Vert;
    
    // First Loop: we analyze the mesh to compute the number of the new faces and new vertices 
    CMeshO::FaceIterator fi;
    int step = 0, percStep = m.fn / 33;
    if(percStep == 0) percStep = 1;
    for(fi=m.face.begin(),j=0; fi!=m.face.end(); ++fi) 
        if(!(*fi).IsD())
        {
            if(cb && (++step%percStep) == 0)
                (*cb)(step/percStep, "Eroding...");
            for(j=0; j<3; j++) {
                if(ep(&(*fi), j) && (!hasSelected || ((*fi).IsS() && (*fi).FFp(j)->IsS()))) {
                    int numf = ep.numFacesToAdd(&(*fi), j);
                    if(numf <= 0) continue;
                    newFaceNum += numf;
                    if(((*fi).V(j) < (*fi).V1(j)) || (*fi).IsB(j))
                        newVertNum += numf;
                }
            }
        } // end face loop
    
    if(newVertNum==0) {	// temporary error message
        errorMessage = QString("Found 0 edges to erode. Change filter parameters and try again.");
        return false;
    }
    
    CMeshO::VertexIterator lastv = tri::Allocator<CMeshO>::AddVertices(m, newVertNum);
	
	// Second Loop: initialization of the edge-vertexes map
	// and the position of new vertexes
    for(fi=m.face.begin(); fi!=m.face.end(); ++fi) 
        if(!(*fi).IsD())
        {
            if(cb && (++step%percStep) == 0)
                (*cb)(step/percStep, "Eroding...");
            for(j=0; j<3; j++)
			    if(ep(&(*fi), j)  && (!hasSelected || ((*fi).IsS() && (*fi).FFp(j)->IsS())))
                    if((*fi).V(j) < (*fi).V1(j) || (*fi).IsB(j)) {
                        int numf = ep.numFacesToAdd(&(*fi), j) + 1;
                        for(int p=1; p<numf; p++) {
                            frpoi((*lastv), face::Pos<CMeshO::FaceType>(&*fi,j), (float)p/numf);
                            edge2Vert[vvi(vvpair((*fi).V(j), (*fi).V1(j)), p)] = &*lastv;
                            ++lastv;
                        }
                    }
        }
	assert(lastv == m.vert.end());

    CMeshO::FaceIterator lastf = tri::Allocator<CMeshO>::AddFaces(m, newFaceNum);
    CMeshO::FaceIterator oldendf = lastf; 
	
/*
          v0

   
          f0

    mp01      mp02

          f3
      f1       f2

 v1      mp12       v2

*/

    vector<CMeshO::VertexPointer> vv;        // total face vertexes
                                        //   0..2 original face vertexes 
                                        //   3... newly created vertexes
    vector<CMeshO::FacePointer> nf;          // total faces (the original one and the newly created)
    vector<CMeshO::FaceType::TexCoordType> wtt;  // per wedge texture coordinates 
    
    for(fi=m.face.begin(); fi!=oldendf; ++fi) 
        if(!(*fi).IsD())
        {
            if(cb && (++step%percStep) == 0)
                (*cb)(step/percStep, "Eroding...");
            vv.clear();
            nf.clear();
            wtt.clear();
            vv.push_back((*fi).V(0));
            vv.push_back((*fi).V(1));
            vv.push_back((*fi).V(2));
            int n[3];
            for(j=0; j<3; j++) {
                if(ep(&(*fi), j)  && (!hasSelected || ((*fi).IsS() && (*fi).FFp(j)->IsS()))) {
                	n[j] = ep.numFacesToAdd(&(*fi), j);
                	if((*fi).V(j) < (*fi).V1(j) || (*fi).IsB(j)) 
                		for(int p=1; p<n[j]+1; p++)
                			vv.push_back(edge2Vert[vvi(vvpair((*fi).V(j), (*fi).V1(j)), p)]);
                	else 
                		for(int p=1; p<n[j]+1; p++)
                			vv.push_back(edge2Vert[vvi(vvpair((*fi).V1(j), (*fi).V(j)), p)]);
                }
                else
                	n[j] = 0;
            }
            
            nf.push_back(&*fi);
            int i;
            for(i=1; i<=n[0]+n[1]+n[2]; i++) {
                nf.push_back(&*lastf);
                lastf++;
                if(hasSelected || (*fi).IsS()) 
                    (*nf[i]).SetS();
            }
            
            if(tri::HasPerWedgeTexCoord(m)) {
                for(i=0; i<3; ++i)
                    wtt.push_back((*fi).WT(i));
                for(j=1; j<=n[0]; j++)
                    wtt.push_back(frpoi.WedgeInterp((*fi).WT(0), (*fi).WT(1), j/(n[0]+1)));
                for(j=1; j<=n[1]; j++)
                    wtt.push_back(frpoi.WedgeInterp((*fi).WT(1), (*fi).WT(2), j/(n[1]+1)));
                for(j=1; j<=n[2]; j++)
                    wtt.push_back(frpoi.WedgeInterp((*fi).WT(2), (*fi).WT(0), j/(n[2]+1)));
            }
            
            int orgflag = (*fi).UberFlags();
            for(j=0; j<3; j++)
            	splitFaceEdge(m, nf, vv, wtt, n, j, orgflag);
        }

    assert(lastf == m.face.end());	
    assert(!m.vert.empty());
    for(fi=m.face.begin(); fi!=m.face.end(); ++fi) 
        if(!(*fi).IsD()){
            assert((*fi).V(0) >= &*m.vert.begin() && (*fi).V(0) <= &m.vert.back());
            assert((*fi).V(1) >= &*m.vert.begin() && (*fi).V(1) <= &m.vert.back());
            assert((*fi).V(2) >= &*m.vert.begin() && (*fi).V(2) <= &m.vert.back());
    }
    vcg::tri::UpdateTopology<CMeshO>::FaceFace(m);
    vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m);
    
    Log(0,"Created %i new vertexes and %i new faces.", newVertNum, newFaceNum);       // DEBUG
    
    return true;
}


// splits the current edge (z) of the current face creating numf[z] new faces.
// actually the face edges are already splitted: the new vertexes (contained in vector vv) are 
// already in their final position but the area need to be retriangulated 
// (vector f  contains the pointers to all the faces needed to retriangulate the old face)
void GeometryAgingPlugin::splitFaceEdge(CMeshO &m, vector<CMeshO::FacePointer> &f, vector<CMeshO::VertexPointer> &vv, vector<CMeshO::FaceType::TexCoordType> &wtt, int numf[3], int z, int orgflag) 
{
	if(numf[z] <= 0) return;
	
	int idx = 3;		// index of the first vertex of the curr face (the second will be idx+1)
	int idx3;			// index of the third vertex of the curr face
	int oldf = 0;		// face to split index
	int newf = 1;		// first new face index
	int vf0 = (z+1)%3;	// index of the vertex to move in face 0 (original face to split)
        	        	
	if(z == 0)
		idx3 = 2;
	else if(z == 1) {
		if(numf[0] > 0) {
			idx3 = 2 + numf[0];
			oldf += numf[0];
			newf += numf[0];
		}
		else
			idx3 = 0;
	}
	else {
		if(numf[0] > 0) {
			idx3 = 3;
			newf += numf[0] + numf[1];
		}
		else if(numf[1] > 0) {
			idx3 = 2 + numf[1];
			oldf += numf[1];
			newf += numf[1];
			vf0 = z;
		}
		else
			idx3 = 1;
	}
        	    	
	int i;
	for(i=0; i<z; i++) idx += numf[i];
	
	f.at(oldf)->V(vf0) = vv.at(idx);
	if(tri::HasPerWedgeTexCoord(m))
		f.at(oldf)->WT((z+1)%3) = wtt.at(idx);
	if(idx > 2)
		f.at(oldf)->ClearB((z+1)%3);
	
	for(i=0; i<numf[z]; i++, idx++, newf++) {
		f.at(newf)->V(0) = vv.at(idx);
		f.at(newf)->V(1) = vv.at((i==(numf[z]-1)?(z+1)%3:idx+1));
		f.at(newf)->V(2) = vv.at(idx3);
		if(tri::HasPerWedgeTexCoord(m)) {
			f.at(newf)->WT(0) = wtt.at(idx);
			f.at(newf)->WT(1) = wtt.at((i==(numf[z]-1)?(z+1)%3:idx+1));
			f.at(newf)->WT(2) = wtt.at(idx3);
		}
		for(int j=0; j<3; j++) assert(f.at(newf)->V(j) != 0);
		if(orgflag & (CMeshO::FaceType::BORDER0 << z))
			f.at(newf)->SetB(0);
		else
			f.at(newf)->ClearB(0);
		f.at(newf)->ClearB(1);
		f.at(newf)->ClearB(2);
	}
}


Q_EXPORT_PLUGIN(GeometryAgingPlugin)
