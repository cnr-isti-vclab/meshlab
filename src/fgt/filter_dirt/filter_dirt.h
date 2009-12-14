/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2007                                                \/)\/    *
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

#ifndef FILTERDIRTPLUGIN_H
#define FILTERDIRTPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QString>

#include <common/meshmodel.h>
#include <common/interfaces.h>
#include<vector>
#include<vcg/simplex/vertex/base.h>
#include<vcg/simplex/face/base.h>
#include<vcg/complex/trimesh/base.h>
#include "muParser.h"

using namespace vcg;
using namespace mu;

class DustEdge;//Never used
class DustVertex;//Never used
class DustFace;

class DustVertex : public VertexSimp2<DustVertex, DustEdge, DustFace> {};
class DustFace:public FaceSimp2< DustVertex, DustEdge, DustFace,face::Normal3f>{};
class DustMesh:public tri::TriMesh< std::vector<DustVertex>,std::vector<DustFace> >{};



class FilterDirt : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)
    protected:
        double x,y,z,nx,ny,nz,r,g,b,q,rad;
        //double x0,y0,z0,x1,y1,z1,x2,y2,z2,nx0,ny0,nz0,nx1,ny1,nz1,nx2,ny2,nz2,r0,g0,b0,r1,g1,b1,r2,g2,b2,q0,q1,q2;
        double v,f,v0i,v1i,v2i;
        std::vector<std::string> v_attrNames;
        std::vector<double> v_attrValue;
        //std::vector<std::string> f_attrNames;
        //std::vector<double> f_attrValue;
        std::vector<CMeshO::PerVertexAttributeHandle<float> > vhandlers;
        //std::vector<CMeshO::PerFaceAttributeHandle<float> > fhandlers;

    public:
        enum {FP_DIRT} ;
		
        FilterDirt();
        ~FilterDirt(){};

        virtual QString filterName(FilterIDType filter) const;
        virtual QString filterInfo(FilterIDType filter) const;
        virtual int getRequirements(QAction *);
        virtual bool autoDialog(QAction *) {return true;}
        virtual void initParameterSet(QAction* filter,MeshModel &,RichParameterSet &){};
        virtual void initParameterSet(QAction *,MeshDocument &/*m*/, RichParameterSet & /*parent*/);
        virtual bool applyFilter(QAction*  filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb);
        virtual bool applyFilter(QAction * /*filter */, MeshModel &, RichParameterSet & /*parent*/, vcg::CallBackPos *) { assert(0); return false;} ;
        virtual FilterClass getClass(QAction *);
        void setPerVertexVariables(Parser &p);
        void setAttributes(CMeshO::VertexIterator &vi, CMeshO &m);

};


#endif
