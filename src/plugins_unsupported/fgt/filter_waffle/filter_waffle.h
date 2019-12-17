/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2008                                                \/)\/    *
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


#ifndef FILTER_SLICE_H
#define FILTER_SLICE_H

#include <QObject>
#include <common/interfaces.h>
#include <vcg/complex/algorithms/update/bounding.h>
#include <vcg/complex/algorithms/update/flag.h>
#include <vcg/complex/algorithms/refine.h>
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/append.h>
#include <vcg/complex/algorithms/update/selection.h>
//#include <wrap/io_edgemesh/export_svg.h>

#include <vcg/space/plane3.h>

#include "filter_waffle_functors.h"
#include "export_svg.h"

typedef vcg::tri::io::SVGProperties SVGProperties;


typedef enum{X = 0, Y = 1, Z = 2} Axis;

template <int A>
struct Succ
{
    enum { value = A+1 };
};

template <>
struct Succ<Z>
{
    enum { value = X };
};

template <int A>
struct Min
{
    enum { value = A+1 };
};


//L1: lato + a destra, L2: lato + a sinistra, l1: lato minore che passa per il centro
typedef enum{L1=0,L2=1,l1=2,NO_LATO=3} latoRect;

//come vengono incastrati 2 piani
typedef enum{NOT_PLANE,NOT_SAGOME, NILL} Incastri;

class JointPoint{
public:
    Point2f p;
    int e;
    int indV;  //indice del vertice da spostare nel nuovo edge oppure del vertice collineare
    bool collinear; //indica se è collineare un vertice

    JointPoint(){}

    JointPoint(const Point2f &p1, const int &e1, const int &indV1, const bool &collinear1)
    {
        p = p1; e = e1; indV = indV1; collinear = collinear1;
    }
};



class ExtraFilter_SlicePlugin : public QObject, public MeshFilterInterface
{
        Q_OBJECT
        Q_INTERFACES(MeshFilterInterface)

public:
//        enum{X,Y,Z};
        enum { FP_WAFFLE_SLICE };
//        enum { CAP_CW, CAP_CCW };
        enum RefPlane { REF_CENTER,REF_MIN,REF_ORIG};
        ExtraFilter_SlicePlugin();
        ~ExtraFilter_SlicePlugin(){};

        virtual QString filterName(FilterIDType filter) const;
        virtual QString filterInfo(FilterIDType filter) const;
//        virtual bool autoDialog(QAction *);
        virtual FilterClass getClass(QAction *);
        virtual void initParameterSet(QAction *,MeshModel &/*m*/, RichParameterSet & /*parent*/);
        virtual bool applyFilter(QAction *filter, MeshDocument &m, RichParameterSet & /*parent*/, vcg::CallBackPos * cb) ;
        virtual int getRequirements(QAction *){return MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER | MeshModel::MM_VERTFLAG | MeshModel::MM_VERTMARK | MeshModel::MM_VERTCOORD;}

//        static void extrude(MeshDocument* doc,MeshModel* orig, MeshModel* dest, float eps, vcg::Point3f planeAxis);

    private:
        SVGProperties pr;
//        void createSlice(MeshModel* orig,MeshModel* dest);

        // nuove funzioni
        void generateRectSeg(const float &dist, const float &epsTmp, const float &lengthDiag, Segment2f lati[]);
        Incastri subtraction(CMeshO &em, Segment2f lati[], const Axis &axis, const Axis &axisOrthog, const Axis &axisJoint, const float height);
        void addAndBreak(CMeshO &em, Point3f & pJoint, const Axis &axisOrthog, const Axis &axisJoint, const JointPoint & jp, const CMeshO::VertexIterator vi, const CMeshO::EdgeIterator ei);
};

namespace vcg {
  namespace tri {

template <class MeshType>
    bool ExtrudeBoundary(MeshType &orig, MeshType &dest, float eps, Point3f planeAxis)
{
  tri::Append<MeshType,MeshType>::Mesh(dest, orig);
  tri::UpdateTopology<MeshType>::FaceFace(dest);
  //create the clone, move it eps/2 on the left and invert its normals
  for (size_t i=0;i<dest.vert.size();i++)
    dest.vert[i].P()-=planeAxis*eps/2;
  tri::Clean<MeshType>::FlipMesh(dest);
  vcg::tri::UpdateTopology<MeshType>::FaceFace(dest);
  vcg::tri::UpdateNormals<MeshType>::PerVertexPerFace(dest);
  //find the border outlines
  std::vector< std::vector<Point3f> > outlines;
  std::vector<Point3f> outline;
  vcg::tri::UpdateFlags<MeshType>::VertexClearV(dest);
  vcg::tri::UpdateFlags<MeshType>::FaceBorderFromFF(dest);
  int nv=0;
  for(size_t i=0;i<dest.face.size();i++)
  {
    for (int j=0;j<3;j++)
    if (!dest.face[i].IsV() && dest.face[i].IsB(j))
    {
      CFaceO* startB=&(dest.face[i]);
      vcg::face::Pos<CFaceO> p(startB,j);
      do
      {
        p.V()->SetV();
        outline.push_back(p.V()->P());
        p.NextB();
        nv++;
      }
      while(!p.V()->IsV());
      outlines.push_back(outline);
      outline.clear();
    }
  }
  if (nv<2) return false;
  //I have at least 3 vertexes
  MeshType tempMesh;
  tri::Append<MeshType,MeshType>::Mesh(tempMesh, orig);
  for (size_t i=0;i<tempMesh.vert.size();i++)
    tempMesh.vert[i].P()+=planeAxis*eps/2;
  //create the clone and move it eps/2 on the right
  tri::Append<MeshType,MeshType>::Mesh(dest, tempMesh);
  //delete tempMesh;
  tempMesh.Clear();
  typename MeshType::VertexIterator vi=vcg::tri::Allocator<MeshType>::AddVertices(tempMesh,2*nv);

  //I have at least 6 vertexes
  for (size_t i=0;i<outlines.size();i++)
  {
    (&*vi)->P()=outlines[i][0];
    CVertexO* v0=(&*vi); ++vi;
    (&*vi)->P()=outlines[i][0]+planeAxis*eps;
    CVertexO* v1=(&*vi); ++vi;
    CVertexO* vs0=v0;	//we need the start point
    CVertexO* vs1=v1; //to close the loop
//		Log(0,"%d",outlines[i].size());
    for(size_t j=1;j<outlines[i].size();++j)
    {
      (&*vi)->P()=outlines[i][j];
      CVertexO* v2=(&*vi); ++vi;
      (&*vi)->P()=outlines[i][j]+planeAxis*eps;
      CVertexO* v3=(&*vi); ++vi;
      typename MeshType::FaceIterator fi=vcg::tri::Allocator<MeshType>::AddFaces(tempMesh,2);

      (&*fi)->V(2)=v0;
      (&*fi)->V(1)=v1;
      (&*fi)->V(0)=v2;
      ++fi;
      (&*fi)->V(0)=v1;
      (&*fi)->V(1)=v2;
      (&*fi)->V(2)=v3;

      v0=v2;
      v1=v3;
      if (j==outlines[i].size()-1)
      {
        typename MeshType::FaceIterator fi=vcg::tri::Allocator<MeshType>::AddFaces(tempMesh,2);
        (&*fi)->V(2)=v0;
        (&*fi)->V(1)=v1;
        (&*fi)->V(0)=vs0;
        ++fi;
        (&*fi)->V(0)=v1;
        (&*fi)->V(1)=vs0;
        (&*fi)->V(2)=vs1;
      }
    }
  }

  tri::Append<MeshType,MeshType>::Mesh(dest, tempMesh);
  tri::Clean<MeshType>::RemoveDuplicateVertex(dest);
  vcg::tri::UpdateTopology<MeshType>::FaceFace(dest);
  vcg::tri::UpdateNormals<MeshType>::PerVertexPerFace(dest);
  vcg::tri::UpdateBounding<MeshType>::Box(dest);
  return true;
  }


} // end namespace tri
} // end namespace vcg

#endif
