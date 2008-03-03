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


#ifndef GEOMETRYAGINGPLUGIN_H
#define GEOMETRYAGINGPLUGIN_H


#include <QObject>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
//#include <vcg/complex/trimesh/refine.h>


struct FractPoint : public std::unary_function<face::Pos<CMeshO::FaceType>, CMeshO::CoordType>
{
    void operator()(CMeshO::VertexType &nv, face::Pos<CMeshO::FaceType> ep, float relpos = 0.5f) {
        nv.P() = ep.f->V(ep.z)->P() * (1.0f - relpos) + ep.f->V1(ep.z)->P() * relpos;
        if(CMeshO::HasPerVertexNormal())
            nv.N() = (ep.f->V(ep.z)->N() * (1.0f - relpos) + ep.f->V1(ep.z)->N() * relpos).Normalize();
        if(CMeshO::HasPerVertexColor())
            nv.C().lerp(ep.f->V(ep.z)->C(), ep.f->V1(ep.z)->C(), relpos);
	}
    
	Color4<CMeshO::ScalarType> WedgeInterp(Color4<CMeshO::ScalarType> &c0, Color4<CMeshO::ScalarType> &c1, float relpos = 0.5f)
    {
        Color4<CMeshO::ScalarType> cc;
        cc.lerp(c0, c1, relpos);
        return cc;
	}

    template<class FL_TYPE>
    TexCoord2<FL_TYPE, 1> WedgeInterp(TexCoord2<FL_TYPE, 1> &t0, TexCoord2<FL_TYPE, 1> &t1, float relpos = 0.5f)
    {
        TexCoord2<FL_TYPE, 1> tmp;
        assert(t0.n() == t1.n());
        tmp.n() = t0.n();
        tmp.t() = t0.t() * (1.0f - relpos) + t1.t() * relpos;
        return tmp;
    }
};


class EdgePred
{
    public:
        EdgePred(float angleTh, float edgeLenTh) {
            this->angleTh = angleTh;
            this->edgeLenTh = edgeLenTh;
        }
    
        bool operator()(CMeshO::FacePointer f, int idx) {
            if(f->IsB(idx)) return true;
            
            // the angle between the two face normals in degrees
            // TODO: check non 2-manifold cases, it's all ok? or there are problems?
            /*double ffangle = vcg::Angle(f->N(), f->FFp(idx)->N()) * 180 / M_PI;
            
            // the 2 points not shared between the 2 faces
            CVertexO *f1p = f->V2(idx);
            CVertexO *f2p = f->FFp(idx)->V2(f->FFi(idx));
            
            Point3<CVertexO::ScalarType> y, median;
            
            y = f->N().Normalize() ^ f->FFp(idx)->N().Normalize();
            median = y ^ (Point3<CVertexO::ScalarType>(f1p->P() - f2p->P()));
            */
            /* There are always 2 cases wich produce the same angle value:
                         ___|_           ____
                        |               |  |
                       -|               |-
                
               In the first case the edge lies in a concave area of the mesh
               while in the second case it lies in a convex area.
               We need a way to know wich is the current case.
               This is done comparing ffangle with the angle between the 
               normal to the current face and the median vector. 
            */
            //return (ffangle-angleThreshold >= -0.001  && vcg::Angle(f->N(), median) * 180 / M_PI < ffangle);
            return false;
        }
        
        int numFacesToAdd(CMeshO::FacePointer f, int idx) {
            return (int)(Distance(f->V(idx)->P(), f->V1(idx)->P()) / edgeLenTh) - 1;
        }
        
    protected:
        float angleTh;
        float edgeLenTh;
};


class GeometryAgingPlugin : public QObject, public MeshFilterInterface
{
    Q_OBJECT
    Q_INTERFACES(MeshFilterInterface)

    public:
        enum {FP_ERODE};
        typedef struct EP{
            float lx;       // x offset along the surface of the face on the left side of the edge
            float rx;       // x offset along the surface of the face on the right side of the edge
            float yrel;     // y relative position between the two edge vertexes
            EP(float yrel, float lx, float rx=0.0) {
                this->yrel = yrel;
                this->lx = lx;
                this->rx = rx;
            }
        } EroderPoint;

        GeometryAgingPlugin();
        virtual ~GeometryAgingPlugin();
        
        virtual const QString filterInfo(FilterIDType filter);
        virtual const QString filterName(FilterIDType filter);
        virtual const PluginInfo &pluginInfo();
        virtual const int getRequirements(QAction *) {return (MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG);}
        virtual bool autoDialog(QAction *) {return true;}
        virtual void initParameterSet(QAction *action, MeshModel &m, FilterParameterSet &params);
        virtual bool applyFilter(QAction *filter, MeshModel &m, FilterParameterSet &params, vcg::CallBackPos *cb);
        
    protected:
        bool hasSelected(MeshModel &m);
        vector<EroderPoint>* generateEdgeEroder(CMeshO::FacePointer f, int z);
        bool refineNeededEdges(CMeshO &m, FractPoint mid, EdgePred ep, bool hasSelected, CallBackPos *cb);
        void splitFaceEdge(CMeshO &m, vector<CMeshO::FacePointer> &f, vector<CMeshO::VertexPointer> &vv, vector<CMeshO::FaceType::TexCoordType> &wtt, int numf[3], int z, int orgflag);
};


#endif
