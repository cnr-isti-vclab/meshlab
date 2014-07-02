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
#include "filter_create.h"
#include <vcg/complex/algorithms/create/platonic.h>
#include <vcg/complex/algorithms/point_sampling.h>
#include <vcg/complex/algorithms/smooth.h>
#include <vcg/math/gen_normal.h>

using namespace vcg;
using namespace tri;

// Constructor usually performs only two simple tasks of filling the two lists
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

FilterCreate::FilterCreate()
{
    typeList << CR_BOX<< CR_ANNULUS << CR_SPHERE<< CR_SPHERE_CAP
             << CR_RANDOM_SPHERE<< CR_ICOSAHEDRON<< CR_DODECAHEDRON
             << CR_TETRAHEDRON<<CR_OCTAHEDRON<<CR_CONE<<CR_TORUS;

  foreach(FilterIDType tt , types())
      actionList << new QAction(filterName(tt), this);
}

QString FilterCreate::filterName(FilterIDType filterId) const
{
  switch(filterId) {
  case CR_BOX : return QString("Box");
  case CR_ANNULUS : return QString("Annulus");
  case CR_SPHERE: return QString("Sphere");
  case CR_SPHERE_CAP: return QString("Sphere Cap");
  case CR_RANDOM_SPHERE: return QString("Points on a Sphere");
  case CR_ICOSAHEDRON: return QString("Icosahedron");
  case CR_DODECAHEDRON: return QString("Dodecahedron");
  case CR_OCTAHEDRON: return QString("Octahedron");
  case CR_TETRAHEDRON: return QString("Tetrahedron");
  case CR_CONE: return QString("Cone");
  case CR_TORUS: return QString("Torus");
  default : assert(0);
  }
}

// Info() must return the longer string describing each filtering action
// (this string is used in the About plugin dialog)
QString FilterCreate::filterInfo(FilterIDType filterId) const
{
  switch(filterId) {
  case CR_BOX : return QString("Create a Box");
  case CR_ANNULUS : return QString("Create an Annulus, e.g. a flat region bounded by two concentric circles");
  case CR_SPHERE: return QString("Create a Sphere");
  case CR_SPHERE_CAP: return QString("Create a Sphere Cap subtended by a cone of given angle");
  case CR_RANDOM_SPHERE: return QString("Create a spherical point cloud, it can be random or regularly distributed.");
  case CR_ICOSAHEDRON: return QString("Create an Icosahedron");
  case CR_DODECAHEDRON: return QString("Create an Dodecahedron");
  case CR_OCTAHEDRON: return QString("Create an Octahedron");
  case CR_TETRAHEDRON: return QString("Create a Tetrahedron");
  case CR_CONE: return QString("Create a Cone");
  case CR_TORUS: return QString("Create a Torus");
  default : assert(0);
  }
}

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parmeter you need to define,
// - the name of the parameter,
// - the string shown in the dialog
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void FilterCreate::initParameterSet(QAction *action, MeshModel & /*m*/, RichParameterSet & parlst)
{
  switch(ID(action))	 {

  case CR_SPHERE :
    parlst.addParam(new RichFloat("radius",1,"Radius","Radius of the sphere"));
    parlst.addParam(new RichInt("subdiv",3,"Subdiv. Level","Number of the recursive subdivision of the surface. Default is 3 (a sphere approximation composed by 1280 faces).<br>"
                                "Admitted values are in the range 0 (an icosahedron) to 8 (a 1.3 MegaTris approximation of a sphere)"));
    break;

  case CR_SPHERE_CAP :
    parlst.addParam(new RichFloat("angle",60,"Angle","Angle of the cone subtending the cap. It must be < 180"));
    parlst.addParam(new RichInt("subdiv",3,"Subdiv. Level","Number of the recursive subdivision of the surface. Default is 3 (a sphere approximation composed by 1280 faces).<br>"
                                "Admitted values are in the range 0 (an icosahedron) to 8 (a 1.3 MegaTris approximation of a sphere)"));
    break;
  case CR_ANNULUS :
    parlst.addParam(new RichFloat("internalRadius",0.5f,"Internal Radius","Internal Radius of the annulus"));
    parlst.addParam(new RichFloat("externalRadius",1.0f,"External Radius","Externale Radius of the annulus"));
    parlst.addParam(new RichInt("sides",32,"Sides","Number of the sides of the poligonal approximation of the annulus "));
    break;
  case CR_RANDOM_SPHERE :
    parlst.addParam(new RichInt("pointNum",100,"Point Num","Number of points (approximate)."));
    parlst.addParam(new RichBool("randomFlag",true,"Random","If true the points are randomly generated using a Poisson Disk distribution. Otherwise the Dave Rusin's disco ball algorithm for the regular placement of points on a sphere is used."));
    break;
  case CR_BOX :
    parlst.addParam(new RichFloat("size",1,"Scale factor","Scales the new mesh"));
    break;
  case CR_CONE:
    parlst.addParam(new RichFloat("r0",1,"Radius 1","Radius of the bottom circumference"));
    parlst.addParam(new RichFloat("r1",2,"Radius 2","Radius of the top circumference"));
    parlst.addParam(new RichFloat("h",3,"Height","Height of the Cone"));
    parlst.addParam(new RichInt("subdiv",36,"Side","Number of sides of the polygonal approximation of the cone"));
    break;
  case CR_TORUS:
    parlst.addParam(new RichFloat("hRadius",3,"Horizontal Radius","Radius of the whole horizontal ring of the torus"));
    parlst.addParam(new RichFloat("vRadius",1,"Vertical Radius","Radius of the vertical section of the ring"));
    parlst.addParam(new RichInt("hSubdiv",24,"Horizontal Subdivision","Subdivision step of the ring"));
    parlst.addParam(new RichInt("vSubdiv",12,"Vertical Subdivision","Number of sides of the polygonal approximation of the torus section"));
    break;
  default : return;
  }
}

// The Real Core Function doing the actual mesh processing.
bool FilterCreate::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, CallBackPos * /*cb*/)
{
    MeshModel* m=md.addNewMesh("",this->filterName(ID(filter)));
  switch(ID(filter))	 {
    case CR_TETRAHEDRON :
      tri::Tetrahedron<CMeshO>(m->cm);
      break;
    case CR_ICOSAHEDRON:
      tri::Icosahedron<CMeshO>(m->cm);
      break;
    case CR_DODECAHEDRON:
      tri::Dodecahedron<CMeshO>(m->cm);
      m->updateDataMask(MeshModel::MM_POLYGONAL);
      break;
    case CR_OCTAHEDRON:
      tri::Octahedron<CMeshO>(m->cm);
      break;
    case CR_ANNULUS:
      tri::Annulus<CMeshO>(m->cm,par.getFloat("internalRadius"),
                                par.getFloat("externalRadius"),par.getInt("sides"));
      break;
    case CR_TORUS:
    {
      float hRadius=par.getFloat("hRadius");
      float vRadius=par.getFloat("vRadius");
      int hSubdiv=par.getInt("hSubdiv");
      int vSubdiv=par.getInt("vSubdiv");
      tri::Torus(m->cm,hRadius,vRadius,hSubdiv,vSubdiv);
        break;
    }
  case CR_RANDOM_SPHERE:
  {
    CMeshO tt;

    int pointNum = par.getInt("pointNum");
    bool randomFlag=par.getBool("randomFlag");

    if(randomFlag)
    {
      int oversamplingFactor =100;
      if(pointNum <= 100) oversamplingFactor = 1000;
      if(pointNum >= 10000) oversamplingFactor = 50;
      if(pointNum >= 100000) oversamplingFactor = 20;

      math::MarsenneTwisterRNG rng;
      tri::Allocator<CMeshO>::AddVertices(tt,pointNum*50);
      for(CMeshO::VertexIterator vi=tt.vert.begin();vi!=tt.vert.end();++vi)
        vi->P()=math::GeneratePointOnUnitSphereUniform<CMeshO::ScalarType>(rng);
      tri::UpdateBounding<CMeshO>::Box(tt);

      const float SphereArea = 4*M_PI;
      float poissonRadius = 2.0*sqrt((SphereArea / float(pointNum*2))/M_PI);

      std::vector<Point3m> poissonSamples;
      tri::TrivialSampler<CMeshO> pdSampler(poissonSamples);
      tri::SurfaceSampling<CMeshO, tri::TrivialSampler<CMeshO> >::PoissonDiskParam pp;

      tri::SurfaceSampling<CMeshO,tri::TrivialSampler<CMeshO> >::PoissonDiskPruning(pdSampler, tt, poissonRadius, pp);
      m->cm.Clear();
      for(size_t i=0;i<poissonSamples.size();++i)
        tri::Allocator<CMeshO>::AddVertex(m->cm,poissonSamples[i],poissonSamples[i]);
    }
    else
    {
      std::vector<Point3m> regularSamples;
      GenNormal<CMeshO::ScalarType>::Regular(pointNum,regularSamples);
      m->cm.Clear();
      for(size_t i=0;i<regularSamples.size();++i)
        tri::Allocator<CMeshO>::AddVertex(m->cm,regularSamples[i],regularSamples[i]);
    }
  } break;

  case CR_SPHERE_CAP:
  {
    int rec = par.getInt("subdiv");
    const float angleDeg = par.getFloat("angle");
    m->updateDataMask(MeshModel::MM_FACEFACETOPO);
    tri::UpdateTopology<CMeshO>::FaceFace(m->cm);
    tri::SphericalCap(m->cm,math::ToRad(angleDeg),rec);
  } break;

  case CR_SPHERE:
    {
        int rec = par.getInt("subdiv");
        float radius = par.getFloat("radius");
        m->cm.face.EnableFFAdjacency();
        m->updateDataMask(MeshModel::MM_FACEFACETOPO);
        assert(tri::HasPerVertexTexCoord(m->cm) == false);
        tri::Sphere<CMeshO>(m->cm,rec);
        tri::UpdatePosition<CMeshO>::Scale(m->cm,radius);
        break;
    }
    case CR_BOX:
    {
      float sz=par.getFloat("size");
      Box3m b(Point3m(1,1,1)*(-sz/2),Point3m(1,1,1)*(sz/2));
      tri::Box<CMeshO>(m->cm,b);
            m->updateDataMask(MeshModel::MM_POLYGONAL);

      break;
    }
    case CR_CONE:
      float r0=par.getFloat("r0");
      float r1=par.getFloat("r1");
      float h=par.getFloat("h");
      int subdiv=par.getInt("subdiv");
      tri::Cone<CMeshO>(m->cm,r0,r1,h,subdiv);
      break;
   }
     tri::UpdateBounding<CMeshO>::Box(m->cm);
   tri::UpdateNormal<CMeshO>::PerVertexNormalizedPerFaceNormalized(m->cm);
    return true;
}

 MeshFilterInterface::FilterClass FilterCreate::getClass(QAction *a)
{
  switch(ID(a))
  {
    case CR_BOX:
    case CR_TETRAHEDRON:
    case CR_ICOSAHEDRON:
    case CR_DODECAHEDRON:
    case CR_SPHERE:
    case CR_SPHERE_CAP:
    case CR_ANNULUS:
    case CR_RANDOM_SPHERE:
    case CR_OCTAHEDRON:
    case CR_CONE:
    case CR_TORUS:
      return MeshFilterInterface::MeshCreation;
      break;
    default: assert(0);
              return MeshFilterInterface::Generic;
  }
}

QString FilterCreate::filterScriptFunctionName( FilterIDType filterID )
 {
    switch(filterID)
    {
        case CR_BOX : return QString("box");
        case CR_ANNULUS : return QString("annulus");
        case CR_SPHERE: return QString("sphere");
        case CR_SPHERE_CAP: return QString("spherecap");
        case CR_RANDOM_SPHERE: return QString("randomsphere");
        case CR_ICOSAHEDRON: return QString("icosahedron");
        case CR_DODECAHEDRON: return QString("dodecahedron");
        case CR_OCTAHEDRON: return QString("octahedron");
        case CR_TETRAHEDRON: return QString("tetrahedron");
        case CR_CONE: return QString("cone");
        case CR_TORUS: return QString("torus");
        default : assert(0);
    }
 }


MESHLAB_PLUGIN_NAME_EXPORTER(FilterCreate)
