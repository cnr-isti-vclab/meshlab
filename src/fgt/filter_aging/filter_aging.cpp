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


#include <math.h>

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
void GeometryAgingPlugin::initParameterSet(QAction *action, MeshModel &model, FilterParameterSet &params)
{
    switch(ID(action)) {
        case FP_ERODE:
            
            break;
        default:
            assert(0);
    }
}


// The Real Core Function doing the actual mesh processing
bool GeometryAgingPlugin::applyFilter(QAction *filter, MeshModel &model, FilterParameterSet &params, vcg::CallBackPos *cb)
{
    CMeshO::FaceIterator fi;
    int fcount;
    int border = 0;
    int chips = 0;
    
    //srand(time(NULL)); 
    
    // first tour to clear V bits
    for(fi=model.cm.face.begin(); fi!=model.cm.face.end(); ++fi)
        (*fi).ClearV();
    
    // I'm looking for edges whose incident faces normals make an angle of at least
    // 60 degrees (or other threshold value chosen by the user).
    // I'm also interested in border edges.
    for(fi=model.cm.face.begin(), fcount=0; fi!=model.cm.face.end(); ++fi, ++fcount) {
         // Typical usage of the callback for showing a nice progress bar in the bottom. 
         // First parameter is a 0..100 number indicating percentage of completion, the second is an info string.
        //cb(100*vcount/model.cm.vert.size(), "Exploring the mesh...");
        
        if(!(*fi).IsD()) {
            for(int j=0; j<3; j++) {
                if((*fi).FFp(j)->IsV()) continue;   // face already visited
                if((*fi).FFp(j) == &(*fi))           // found border edge
                    border++;
                else {      // this is not a border edge
                    // the angle between the two face normals in degrees
                    // TODO: check non 2-manifold cases, it's all ok? or there are problems?
                    float ffangle = vcg::Angle((*fi).N(), (*fi).FFp(j)->N())*180/M_PI;
                    Point3<CVertexO::ScalarType> y, median;
                    CVertexO *f1p, *f2p;        // the 2 points not shared by the 2 faces
                    
                    if(j == 0)
                        f1p = (*fi).V(2);
                    else if(j == 1)
                        f1p = (*fi).V(0);
                    else
                        f1p = (*fi).V(1);
                    if((*fi).FFi(j) == 0)
                        f2p = (*fi).FFp(j)->V(2);
                    else if((*fi).FFi(j) == 1)
                        f2p = (*fi).FFp(j)->V(0);
                    else
                        f2p = (*fi).FFp(j)->V(1);
                    
                    y = (*fi).N() ^ (*fi).FFp(j)->N();
                    median = y ^ (Point3<CVertexO::ScalarType>(f1p->P() - f2p->P()));
                    
                    /* There are always 2 cases wich produce the same angle value:
                             ___|_           ____
                            |               |  |
                           -|               |-
                        
                        In the first case the edge lies in a concave area of the mesh
                        while in the second case it lies in a convex area.
                        I need a way to know wich is the current case.
                        This is done comparing ffangle with the angle between the 
                        normal to the current face and the median vector. */
                    if(ffangle > 60 && vcg::AngleN((*fi).N(), median)*180/M_PI < ffangle)
                        chips++;
                }
            }
            (*fi).SetV();
        }
    }
    
    // clear V bits again
    for(fi=model.cm.face.begin(); fi!=model.cm.face.end(); ++fi)
        (*fi).ClearV();
    
    // Log function dump textual info in the lower part of the MeshLab screen. 
    Log(0,"Successfully visited %i faces. Found %i border edges and %i internal edges to erode.", fcount, border, chips);       // DEBUG
    
    return true;
}

Q_EXPORT_PLUGIN(GeometryAgingPlugin)
