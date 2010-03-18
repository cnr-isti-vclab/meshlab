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
#include <vcg/complex/trimesh/create/platonic.h>

// Constructor usually performs only two simple tasks of filling the two lists
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

FilterCreate::FilterCreate()
{
	typeList <<CR_BOX<< CR_SPHERE<< CR_ICOSAHEDRON<< CR_DODECAHEDRON<< CR_TETRAHEDRON<<CR_OCTAHEDRON<<CR_CONE;

  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

QString FilterCreate::filterName(FilterIDType filterId) const
{
  switch(filterId) {
    case CR_BOX : return QString("Box");
    case CR_SPHERE: return QString("Sphere");
    case CR_ICOSAHEDRON: return QString("Icosahedron");
    case CR_DODECAHEDRON: return QString("Dodecahedron");
    case CR_OCTAHEDRON: return QString("Octahedron");
    case CR_TETRAHEDRON: return QString("Tetrahedron");
    case CR_CONE: return QString("Cone");
		default : assert(0);
	}
}

// Info() must return the longer string describing each filtering action
// (this string is used in the About plugin dialog)
 QString FilterCreate::filterInfo(FilterIDType filterId) const
{
  switch(filterId) {
    case CR_BOX : return QString("Create a Box");
    case CR_SPHERE: return QString("Create a Sphere");
    case CR_ICOSAHEDRON: return QString("Create an Icosahedron");
    case CR_DODECAHEDRON: return QString("Create an Dodecahedron");
    case CR_OCTAHEDRON: return QString("Create an Octahedron");
    case CR_TETRAHEDRON: return QString("Create a Tetrahedron");
    case CR_CONE: return QString("Create a Cone");
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

    case CR_BOX :
      parlst.addParam(new RichFloat("size",1,"Scale factor","Scales the new mesh"));
      break;
    case CR_CONE:
      parlst.addParam(new RichFloat("r0",1,"Radius 1","Radius of the bottom circumference"));
      parlst.addParam(new RichFloat("r1",2,"Radius 2","Radius of the top circumference"));
      parlst.addParam(new RichFloat("h",3,"Height","Height of the Cone"));
      parlst.addParam(new RichInt("subdiv",36,"Side","Number of sides of the polygonal approximation of the cone"));
      break;
		default : return;
	}
}

// The Real Core Function doing the actual mesh processing.
bool FilterCreate::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos * /*cb*/)
{
    MeshModel &m=(*md.mm());
  switch(ID(filter))	 {
    case CR_TETRAHEDRON :
      vcg::tri::Tetrahedron<CMeshO>(m.cm);
      break;
    case CR_ICOSAHEDRON:
      vcg::tri::Icosahedron<CMeshO>(m.cm);
      break;
    case CR_DODECAHEDRON:
      vcg::tri::Dodecahedron<CMeshO>(m.cm);
			m.updateDataMask(MeshModel::MM_POLYGONAL);
      break;
    case CR_OCTAHEDRON:
      vcg::tri::Octahedron<CMeshO>(m.cm);
      break;
    case CR_SPHERE:
      vcg::tri::Sphere<CMeshO>(m.cm);
      break;
    case CR_BOX:
    {
      float sz=par.getFloat("size");
      vcg::Box3f b(vcg::Point3f(1,1,1)*(sz/2),vcg::Point3f(1,1,1)*(-sz/2));
      vcg::tri::Box<CMeshO>(m.cm,b);
			m.updateDataMask(MeshModel::MM_POLYGONAL);

      break;
    }
    case CR_CONE:
      float r0=par.getFloat("r0");
      float r1=par.getFloat("r1");
      float h=par.getFloat("h");
      int subdiv=par.getInt("subdiv");
      vcg::tri::Cone<CMeshO>(m.cm,r0,r1,h,subdiv);
      break;
   }
 	 vcg::tri::UpdateBounding<CMeshO>::Box(m.cm);
   vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFaceNormalized(m.cm);
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
    case CR_OCTAHEDRON:
    case CR_CONE:
      return MeshFilterInterface::MeshCreation;
      break;
    default: assert(0);
              return MeshFilterInterface::Generic;
  }
}
 int FilterCreate::getRequirements(QAction *action)
{
  switch(ID(action))
  {
    case CR_SPHERE:
      return MeshModel::MM_FACEFACETOPO;
  }
  return MeshModel::MM_NONE;
}


Q_EXPORT_PLUGIN(FilterCreate)
