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
#include <iostream>        // DEBUG

//#include <vcg/complex/trimesh/update/selection.h>     // DEBUG
#include <vcg/complex/trimesh/allocate.h>

#include "filter_aging.h"


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
            params.addFloat("AngleThreshold", 60.0, "Angle Threshold", "The minimum angle between two adjacent faces to consider the edge they are sharing.");
            break;
        default:
            assert(0);
    }
}


// The Real Core Function doing the actual mesh processing
bool GeometryAgingPlugin::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet &params, vcg::CallBackPos *cb)
{
    CMeshO::FaceIterator fi;
    int borderNum = 0;
    int chips = 0;
    float angleThreshold = params.getFloat("AngleThreshold");
    bool hasSel = hasSelected(m);
    typedef std::pair<CMeshO::FacePointer, int> sedge;      // selected edge
    map<sedge, vector<EroderPoint>* > edge2Eroder; 
    
    // first loop to clear V bits
    for(fi=m.cm.face.begin(); fi!=m.cm.face.end(); ++fi)
        (*fi).ClearV();
    //tri::UpdateSelection<CMeshO>::ClearFace(m.cm);        // DEBUG
    
    // I'm looking for edges whose incident faces normals make an angle of at least
    // 60 degrees (or other threshold value chosen by the user).
    // I'm also interested in border edges.
    for(fi=m.cm.face.begin(); fi!=m.cm.face.end(); ++fi) {
         // Typical usage of the callback for showing a nice progress bar in the bottom. 
         // First parameter is a 0..100 number indicating percentage of completion, the second is an info string.
        //cb(100*vcount/model.cm.vert.size(), "Exploring the mesh...");
        
        if(!(*fi).IsD()) {
            if(hasSel && !(*fi).IsS()) continue;    // some faces of the mesh are selected but not the current one
            for(int j=0; j<3; j++) {
                if((*fi).FFp(j)->IsV()) continue;   // face already visited
                if(hasSel && !(*fi).FFp(j)->IsS()) continue;    // some faces of the mesh are selected, the current one too, but not its j-th neighbour
                if(/*(*fi).FFp(j) == &(*fi)*/(*fi).IsB(j)) {         // found border edge
                    borderNum++;
                    edge2Eroder[sedge(&(*fi), j)] = generateEdgeEroder(true, vcg::Distance((*fi).P0(j), (*fi).P1(j)));
                //    (*fi).SetS();                 // DEBUG
                //    (*fi).FFp(j)->SetS();         // DEBUG
                }
                else {      // this is not a border edge
                    // the angle between the two face normals in degrees
                    // TODO: check non 2-manifold cases, it's all ok? or there are problems?
                    double ffangle = vcg::Angle((*fi).N(), (*fi).FFp(j)->N())*180/M_PI;
                    
                    // the 2 points not shared between the 2 faces
                    CVertexO *f1p = (*fi).V2(j);
                    CVertexO *f2p = (*fi).FFp(j)->V2((*fi).FFi(j));
                    
                    Point3<CVertexO::ScalarType> y, median;
                    
                    y = (*fi).N().Normalize() ^ (*fi).FFp(j)->N().Normalize();
                    median = y ^ (Point3<CVertexO::ScalarType>(f1p->P() - f2p->P()));
                    
                    /* There are always 2 cases wich produce the same angle value:
                             ___|_           ____
                            |               |  |
                           -|               |-
                        
                        In the first case the edge lies in a concave area of the mesh
                        while in the second case it lies in a convex area.
                        I need a way to know wich is the current case.
                        This is done comparing ffangle with the angle between the 
                        normal to the current face and the median vector. 
                    */
                    if(ffangle-angleThreshold >= -0.001  && vcg::Angle((*fi).N(), median)*180/M_PI < ffangle) {
                        chips++;
                    //    (*fi).SetS();                 // DEBUG
                    //    (*fi).FFp(j)->SetS();         // DEBUG
                    }
                }
            }
            (*fi).SetV();
        }
    }
    
    // erode border edges
    /*if(borderNum > 0) {
	    CMeshO::VertexIterator lastv = tri::Allocator<CMeshO>::AddVertices(m.cm, borderNum);
        CMeshO::FaceIterator lastf = tri::Allocator<CMeshO>::AddFaces(m.cm, borderNum);
        for(map<sedge, vector<EroderPoint>* >::iterator it = edge2Eroder.begin(); it != edge2Eroder.end(); ++it) {
            if((*it).first.first->IsB((*it).first.second)) {        // border face
                CMeshO::FacePointer f = (*it).first.first;
                int z = (*it).first.second;
                vector<EroderPoint>* e = (*it).second;
                vector<CMeshO::VertexPointer> vv;
                vector<CMeshO::FacePointer> fv;
                vector<CMeshO::FaceType::TexCoordType> wtt;
                
                for(unsigned int j=0; j<3; j++)
                    vv.push_back(f->V((z+j)%3));
                
                for(unsigned int i=0; i<e->size(); i++) {
                    (*lastv).P() = f->V(z)->P() * (1.0f - e->at(i).yrel) + f->V1(z)->P() * e->at(i).yrel;
                    if(CMeshO::HasPerVertexNormal())
                        (*lastv).N()= (f->V(z)->N() * (1.0f - e->at(i).yrel) + f->V1(z)->N() * e->at(i).yrel).Normalize();
                    if(CMeshO::HasPerVertexColor())
                        (*lastv).C().lerp(f->V(z)->C(), f->V1(z)->C(), e->at(i).yrel);
                    vv.push_back(&*lastv);
                    ++lastv;
                }
                
                fv.push_back(f);
                for(unsigned int i=0; i<vv.size()-3; i++) {
                    fv.push_back(&*lastf);
                    ++lastf;
                }
                
                if(tri::HasPerWedgeTexCoord(m.cm)) {
                    for(unsigned int i=0; i<3; i++)
				        wtt.push_back(f->WT(i));
                    CMeshO::FaceType::TexCoordType t0 = f->WT(z);
                    CMeshO::FaceType::TexCoordType t1 = f->WT((z+1)%3);
                    assert(t0.n() == t1.n());
				    for(unsigned int i=0; i<e->size(); i++) {
                        CMeshO::FaceType::TexCoordType tmp;
                        tmp.n() = t0.n();
                        tmp.t() = (t0.t() * (1.0f - e->at(i).yrel) + t1.t() * e->at(i).yrel);
                        wtt.push_back(tmp);
                    }
                }
                
                int orgflag = f->UberFlags();
                for(unsigned int i=0; i<fv.size(); i++) {
                    fv.at(i)->V(0) = vv.at((i>0?2+i:i));
                    fv.at(i)->V(1) = vv.at((i<fv.size()-1?3+i:1));
                    fv.at(i)->V(2) = vv.at(2);
                    if(tri::HasPerWedgeTexCoord(m.cm)) {
					    fv.at(i)->WT(0) = wtt.at((i>0?2+i:i));
                        fv.at(i)->WT(1) = wtt.at((i<fv.size()-1?3+i:1));
                        fv.at(i)->WT(2) = wtt.at(2);
                    }
                }
                
                for(unsigned int i=0; i<fv.size(); i++) {
                    if(orgflag & (CMeshO::FaceType::BORDER0 << z))
                        fv.at(i)->SetB(0);
                    else
                        fv.at(i)->ClearB(0);
                    fv.at(i)->ClearB(1);
                    fv.at(i)->ClearB(2);
                }
                if(orgflag & (CMeshO::FaceType::BORDER0 << z+1))
                    fv.at(fv.size()-1)->SetB(1);
                else
                    fv.at(fv.size()-1)->ClearB(1);
                if(orgflag & (CMeshO::FaceType::BORDER0 << z+2))
                    fv.at(0)->SetB(2);
                else
                    fv.at(0)->ClearB(2);
            }
        }
    }
    vcg::tri::UpdateTopology<CMeshO>::FaceFace(m.cm);*/
    //vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromFF(m.cm);
    
    // clear V bits again
    for(fi=m.cm.face.begin(); fi!=m.cm.face.end(); ++fi)
        (*fi).ClearV();
    
    // Log function dump textual info in the lower part of the MeshLab screen
    Log(0,"Found %i border edges and %i internal edges to erode.", borderNum, chips);       // DEBUG
    
    for(map<sedge, vector<EroderPoint>* >::iterator it = edge2Eroder.begin(); it != edge2Eroder.end(); ++it)
        delete it->second;
    
    return true;
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
vector<GeometryAgingPlugin::EroderPoint>* GeometryAgingPlugin::generateEdgeEroder(bool border, float len)
{
    vector<EroderPoint>* e = new vector<EroderPoint>();
    
    srand(time(NULL));
    
    //std::cout << "len: " << len << std::endl;           // DEBUG
    if(border)
        e->push_back(GeometryAgingPlugin::EroderPoint(0.5, 0.0));
    /*else
        e->push_back(GeometryAgingPlugin::EroderPoint(0.5, 0.0, 0.0));*/
    
    return e;
}


Q_EXPORT_PLUGIN(GeometryAgingPlugin)
