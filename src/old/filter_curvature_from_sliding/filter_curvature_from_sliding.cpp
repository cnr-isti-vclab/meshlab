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
/****************************************************************************
  History
$Log: samplefilter.cpp,v $
Revision 1.3  2006/11/29 00:59:20  cignoni
Cleaned plugins interface; changed useless help class into a plain string

Revision 1.2  2006/11/27 06:57:21  cignoni
Wrong way of using the __DATE__ preprocessor symbol

Revision 1.1  2006/09/25 09:24:39  e_cerisoli
add samplefilter

****************************************************************************/

#include <QtGui>

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/stat.h>
#include <vcg/math/histogram.h>

#include "filter_curvature_from_sliding.h"
#include "curvature_from_sliding.h"

// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

SlidingCurvaturePlugin::SlidingCurvaturePlugin() 
{ 
	typeList << FP_COMPUTE_PRINC_CURV_DIR_FROM_SLIDING;
  
  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
 QString SlidingCurvaturePlugin::filterName(FilterIDType filterId) 
{
  switch(filterId) {
		case FP_COMPUTE_PRINC_CURV_DIR_FROM_SLIDING :  return QString("Compute Curvature from Sliding"); 
		default : assert(0); 
	}
}

 int SlidingCurvaturePlugin::getRequirements(QAction *action)
{
 return	MeshModel::MM_VERTCURVDIR | MeshModel::MM_FACEMARK;
}


// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
 QString SlidingCurvaturePlugin::filterInfo(FilterIDType filterId)
{
  switch(filterId) {
		case FP_COMPUTE_PRINC_CURV_DIR_FROM_SLIDING :  return QString("Compute Curvature from Sliding"); 
		default : assert(0); 
	}
	return QString("Unknown Filter");
}

// The FilterClass describes in which generic class of filters it fits. 
// This choice affect the submenu in which each filter will be placed 
// More than a single class can be choosen.
 SlidingCurvaturePlugin::FilterClass SlidingCurvaturePlugin::getClass(QAction *a)
{
  switch(ID(a))
	{
		case FP_COMPUTE_PRINC_CURV_DIR_FROM_SLIDING :  return MeshFilterInterface::Normal; 
		default : assert(0); 
	}
	return MeshFilterInterface::Generic;
}

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parmeter you need to define, 
// - the name of the parameter, 
// - the string shown in the dialog 
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void SlidingCurvaturePlugin::initParameterSet(QAction *action,MeshModel &m, FilterParameterSet & parlst) 
{
	 switch(ID(action))	 {
		case FP_COMPUTE_PRINC_CURV_DIR_FROM_SLIDING :  
 		  parlst.addFloat ("radius",
											2.0,
											"Specify radius as multiplier of average edge lenght",
											"Specify radius as multiplier of average edge lenght"
											);
											break;
											
		default : assert(0); 
	}
}

// The Real Core Function doing the actual mesh processing.
// Move Vertex of a random quantity
bool SlidingCurvaturePlugin::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & par, vcg::CallBackPos *cb)
{
	if( ID(filter)== FP_COMPUTE_PRINC_CURV_DIR_FROM_SLIDING){
	vcg::Histogramf hi;
	vcg::CurvatureFromSliding<CMeshO> cfs;

	vcg::tri::UpdateBounding<CMeshO>::Box(m.cm);      // update bounding box
	vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFaceNormalized(m.cm); // update Normals
	vcg::tri::Stat<CMeshO>::ComputeEdgeHistogram(m.cm,hi);
	float radius = hi.Avg()*par.getFloat("radius");
	cfs.Compute(m.cm,radius, cb );}
	return true;
}

Q_EXPORT_PLUGIN(SlidingCurvaturePlugin)
