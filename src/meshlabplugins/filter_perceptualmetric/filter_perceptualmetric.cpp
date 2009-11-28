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
$Log: filter_perceptualmetric.cpp,v $
****************************************************************************/

#include <QtGui>

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/update/position.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/flag.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/quality.h>
#include <vcg/complex/trimesh/point_sampling.h>
#include <vcg/complex/trimesh/create/resampler.h>
#include <vcg/simplex/face/distance.h>
#include <vcg/complex/trimesh/update/color.h>
#include <vcg/complex/trimesh/geodesic.h>
#include <vcg/space/index/grid_static_ptr.h>

// filter interface
#include "filter_perceptualmetric.h"

// perceptual metrics core
#include "perceptualmetrics.h"

using namespace vcg;
using namespace std;


// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

FilterPerceptualMetric::FilterPerceptualMetric() 
{ 
	typeList 
			<< FP_ROUGHNESS_MULTISCALE 
			<< FP_ROUGHNESS_SMOOTHING
			<< FP_STRAIN_ENERGY
	;
  
  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
 QString FilterPerceptualMetric::filterName(FilterIDType filterId) 
{
	switch(filterId) 
	{
		case FP_ROUGHNESS_MULTISCALE :  return QString("Roughness-multiscale perceptual metric"); 
		case FP_ROUGHNESS_SMOOTHING  :  return QString("Roughness-smoothing perceptual metric"); 
		case FP_STRAIN_ENERGY        :  return QString("Energy Strain-based perceptual metric");  
			
		default : assert(0); return QString("unknown filter!!!!");
	}
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
 QString FilterPerceptualMetric::filterInfo(FilterIDType filterId)
{
	switch(filterId) 
	{
		case FP_ROUGHNESS_MULTISCALE :  return QString("Evaluate the perceptual difference between two given meshes; this perceptual metric is based on roughness measure, the roughness is evaluated at multiple-scale using dihedral angles."); 
		case FP_ROUGHNESS_SMOOTHING  :  return QString("Evaluate the perceptual difference between two given meshes; this perceptual metric is based on roughness measure, the roughness is evaluated using a smoothed version of the 3D models."); 
		case FP_STRAIN_ENERGY        :  return QString("Evaluate the perceptual difference between two given meshes; this perceptual metric is based on strain energy.");

		default : assert(0); return QString("unknown filter!!!!");
	}
}
 int FilterPerceptualMetric::getRequirements(QAction *action)
{
	switch(ID(action))
	{
		case FP_ROUGHNESS_MULTISCALE : return MeshModel::MM_VERTFACETOPO | MeshModel::MM_FACENORMAL;
		case FP_ROUGHNESS_SMOOTHING  : return MeshModel::MM_VERTFACETOPO;
		case FP_STRAIN_ENERGY        : return MeshModel::MM_VERTFACETOPO;

		default: assert(0);
	}

	return 0;
}

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parameter you need to define, 
// - the name of the parameter, 
// - the string shown in the dialog 
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void FilterPerceptualMetric::initParameterSet(QAction *action, MeshDocument & md, FilterParameterSet & parlst) 
{
	switch(ID(action))	 
	{
		case FP_ROUGHNESS_MULTISCALE :
		{
			MeshModel *refmesh = md.mm();
			foreach (refmesh, md.meshList) 
				if (refmesh != md.mm())  break;

			parlst.addMesh("ReferenceMesh", refmesh, "Reference Mesh",
				"The original mesh.");
			parlst.addMesh("InputMesh", md.mm(), "Mesh",
				"The mesh where the perceptual impairment of the processing is evaluated.");
		} break;
		
		case FP_ROUGHNESS_SMOOTHING :
		{
			MeshModel *refmesh = md.mm();
			foreach (refmesh, md.meshList) 
				if (refmesh != md.mm())  break;

			parlst.addMesh("ReferenceMesh", refmesh, "Reference Mesh",
				"The original mesh.");
			parlst.addMesh("InputMesh", md.mm(), "Mesh",
				"The mesh where the perceptual impairment of the processing is evaluated.");
		} break;

		case FP_STRAIN_ENERGY :  
		{
			MeshModel *refmesh = md.mm();
			foreach (refmesh, md.meshList) 
				if (refmesh != md.mm())  break;
		
			parlst.addMesh("ReferenceMesh", refmesh, "Reference Mesh",
				"The original mesh.");
			parlst.addMesh("InputMesh", md.mm(), "Mesh",
				"The mesh where the perceptual impairment of the processing is evaluated.");
		} break;

		default : assert(0); 
	}
}

bool FilterPerceptualMetric::applyFilter(QAction *action, MeshDocument &md, FilterParameterSet & par, vcg::CallBackPos *cb)
{
	switch(ID(action))
	{

		case FP_ROUGHNESS_MULTISCALE :  
		{
			MeshModel* mesh0 = par.getMesh("ReferenceMesh");  // reference mesh 
			MeshModel* mesh1 = par.getMesh("InputMesh");      // the processed mesh
			
			double globalimpact = PerceptualMetrics<CMeshO>::roughnessMultiscale(mesh0->cm, mesh1->cm);

			Log(0,"This metric is not implemented yet!!");
		}
		break;

		case FP_ROUGHNESS_SMOOTHING :  
		{
			MeshModel* mesh0 = par.getMesh("ReferenceMesh");  // reference mesh 
			MeshModel* mesh1 = par.getMesh("InputMesh");      // the processed mesh

			double globalimpact = PerceptualMetrics<CMeshO>::roughnessSmoothing(mesh0->cm, mesh1->cm);
			
			Log(0,"This metric is not implemented yet!!");
		}
		break;

		case FP_STRAIN_ENERGY : 
		{
			MeshModel* mesh0 = par.getMesh("ReferenceMesh");  // reference mesh 
			MeshModel* mesh1 = par.getMesh("InputMesh");      // the processed mesh

			double globalimpact = PerceptualMetrics<CMeshO>::strainEnergy(mesh0->cm, mesh1->cm);
			
			Log(0,"Perceptual Distance: %f",globalimpact);
		}
		break;

		default : assert(0);
	}
	return true;
}

 MeshFilterInterface::FilterClass FilterPerceptualMetric::getClass(QAction *action)
{
	switch(ID(action))
	{
		case FP_ROUGHNESS_MULTISCALE : return Quality;
		case FP_ROUGHNESS_SMOOTHING : return Quality;
		case FP_STRAIN_ENERGY : return Quality;
		default: assert(0);
	}
	return FilterClass(0);
}


Q_EXPORT_PLUGIN(FilterPerceptualMetric)
