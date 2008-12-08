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

#include <QtGui>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <vcg/complex/trimesh/create/platonic.h>
#include <vcg/complex/trimesh/update/normal.h>
#include "filter_create.h"

// Constructor usually performs only two simple tasks of filling the two lists
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

FilterCreate::FilterCreate()
{
	typeList <<CR_BOX<< CR_SPHERE<< CR_ICOSAHEDRON<< CR_TETRAHEDRON<<CR_OCTAHEDRON;

  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action
// (this string is used also to define the menu entry)
const QString FilterCreate::filterName(FilterIDType filterId)
{
  switch(filterId) {
    case CR_BOX : return QString("Box");
    case CR_SPHERE: return QString("Sphere");
    case CR_ICOSAHEDRON: return QString("Icosahedron");
    case CR_OCTAHEDRON: return QString("Octahedron");
    case CR_TETRAHEDRON: return QString("Tetrahedron");
		default : assert(0);
	}
}

// Info() must return the longer string describing each filtering action
// (this string is used in the About plugin dialog)
const QString FilterCreate::filterInfo(FilterIDType filterId)
{
  switch(filterId) {
    case CR_BOX : return QString("Create a Box");
    case CR_SPHERE: return QString("Create a Sphere");
    case CR_ICOSAHEDRON: return QString("Create an Icosahedron");
    case CR_OCTAHEDRON: return QString("Create an Octahedron");
    case CR_TETRAHEDRON: return QString("Create a Tetrahedron");
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
void FilterCreate::initParameterSet(QAction *action,MeshModel &m, FilterParameterSet & parlst)
{
	 switch(ID(action))	 {

    case CR_BOX :
      parlst.addFloat("size",1,"Scale factor","Scales the new mesh");
      break;
		default : return;
	}
}

// The Real Core Function doing the actual mesh processing.
// Move Vertex of a random quantity
bool FilterCreate::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & par, vcg::CallBackPos *cb)
{

   switch(ID(filter))	 {
    case CR_TETRAHEDRON :
      vcg::tri::Tetrahedron<CMeshO>(m.cm);
      break;
    case CR_ICOSAHEDRON:
      vcg::tri::Icosahedron<CMeshO>(m.cm);
      break;
    case CR_OCTAHEDRON:
      vcg::tri::Octahedron<CMeshO>(m.cm);
      break;
    case CR_SPHERE:
      vcg::tri::Sphere<CMeshO>(m.cm);
      break;
    case CR_BOX:
      float sz=par.getFloat("size");
      vcg::Box3f b(vcg::Point3f(1,1,1)*(sz/2),vcg::Point3f(1,1,1)*(-sz/2));
      vcg::tri::Box<CMeshO>(m.cm,b);
      break;
   }
   vcg::tri::UpdateNormals<CMeshO>::PerFace(m.cm);
	return true;
}

const MeshFilterInterface::FilterClass FilterCreate::getClass(QAction *a)
{
  switch(ID(a))
  {

    case CR_BOX:
    case CR_TETRAHEDRON:
    case CR_ICOSAHEDRON:
    case CR_SPHERE:
    case CR_OCTAHEDRON:
      return MeshFilterInterface::MeshCreation;
      break;
    default: assert(0);
              return MeshFilterInterface::Generic;
  }
}
const int FilterCreate::getRequirements(QAction *action)
{
  switch(ID(action))
  {
    case CR_SPHERE:
      return MeshModel::MM_FACEFACETOPO;
  }
  return MeshModel::MM_NONE;
}
Q_EXPORT_PLUGIN(FilterCreate)
