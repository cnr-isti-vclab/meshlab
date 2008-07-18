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
#include <qfiledialog.h>
#include <limits>
#include <math.h>
#include <stdlib.h>
#include <qstring.h>

#include "filter_slice.h"

#include <vcg/simplex/vertexplus/base.h>
#include <vcg/simplex/vertexplus/component_ocf.h>
#include <vcg/space/point3.h>
#include <vcg/space/box3.h>
#include <vcg/space/index/grid_closest.h>
#include <vcg/complex/intersection.h>
#include <vcg/complex/edgemesh/update/bounding.h>

#include <wrap/gui/trackball.h>


using namespace std;
using namespace vcg;

// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

ExtraFilter_SlicePlugin::ExtraFilter_SlicePlugin () 
{ 
	typeList << FP_PLAN;
  
  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
const QString ExtraFilter_SlicePlugin::filterName(FilterIDType filterId) 
{
  switch(filterId) {
		case FP_PLAN :  return QString("Cross section plan"); 
		default : assert(0); 
	}
  return QString("error!");
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
const QString ExtraFilter_SlicePlugin::filterInfo(FilterIDType filterId)
{
 switch(filterId) {
		case FP_PLAN :  return QString("Provide the cross section of the chosen plan"); 
		
		default : assert(0); 
	}
  return QString("error!");
}

const PluginInfo &ExtraFilter_SlicePlugin::pluginInfo()
{
   static PluginInfo ai;
   ai.Date=tr("Jul 2008");
	 ai.Version = tr("1.0");
	 ai.Author = ("Couet Julien, with the aid of Nicola Andrenucci editslice's code");
   return ai;
 }

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parmeter you need to define, 
// - the name of the parameter, 
// - the string shown in the dialog 
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
//void ExtraSamplePlugin::initParameterSet(QAction *action,MeshModel &m, FilterParameterSet & parlst) 
void ExtraFilter_SlicePlugin::initParameterSet(QAction *filter, MeshModel &m, FilterParameterSet &parlst)
{
	switch(ID(filter))
		 {	
	
		  case FP_PLAN :  
		  {
 		   	QStringList metrics;
				metrics.push_back("XY Cross Section");
				metrics.push_back("YZ Cross Section");
				metrics.push_back("ZX Cross Section");
				parlst.addEnum("Plan", 0, metrics, tr("Plan:"), tr("Choose a cross section plan."));
				parlst.addFloat("CrossPlanVal", 0.0, "Crossing plan value", "Specify a value of the crossing plan as a float.");
				parlst.addString("filename", "Slice", "Name of the svg file", "All the svg files are registered in the sample/SvgSample folder of the MEshlab tree."); 		  
		  }
		  break;
											
		  default : assert(0); 
	         }
}

// The Real Core Function doing the actual mesh processing.
// Move Vertex of a random quantity
bool ExtraFilter_SlicePlugin::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet &parlst, vcg::CallBackPos *cb)
{
	switch(ID(filter))
	{
		case FP_PLAN :
		{
			int plan = parlst.getEnum("Plan");
			float PlanVal = parlst.getFloat("CrossPlanVal");
			fileName = parlst.getString("filename");
			if (fileName==0) return false;
    			if (!(fileName.endsWith("svg")))
			fileName= fileName+".svg";
			
			double avg_length; 

			b=m.cm.bbox; //Boundig Box
			Point3f mi=b.min;
			Point3f ma=b.max;
			Point3f centre=b.Center() ;
			edgeMax=0;
    			float LX= ma[0]-mi[0];
			float LY= ma[1]-mi[1];
			float LZ= ma[2]-mi[2];
			edgeMax= max(LX, LY);
			edgeMax=max(edgeMax, LZ); //edgeMax: the longest side of BBox

			pr.setPosition(Point2d(0,0));
			pr.numCol=1;
			pr.numRow=1;

			edge_mesh = new n_EdgeMesh();
			
			switch(plan)
			{ 
				case 0: 
				{
					Point3f* dir=new Point3f(0,1,0);   //the plans' normal vector init 
					Point3f translation_plans=trackball_slice.track.tra;  //vettore di translazione dei piani
					
					Point3d d((*dir).X(),(*dir).Y(), (*dir).Z());
					pr.setPlane(0, d); 

					mesh_grid = new TriMeshGrid();
					mesh_grid->Set(m.cm.face.begin() ,m.cm.face.end());

					float scale = (pr.getViewBox().V(0)*2)/(edgeMax*1.4142) ;
					pr.setScale(scale);

					Point3f rotationCenter=m.cm.bbox.Center(); //the point where the plans rotate
					Plane3f pl;
					pl.SetDirection(*dir);
				
					Point3f off= translation_plans; //translation vector
					pl.SetOffset( (rotationCenter.X()*dir->X() )+ (rotationCenter.Y()*dir->Y()) +(rotationCenter.Z()*dir->Z())+ (off*(*dir)) + PlanVal);
			
					vcg::Intersection<n_Mesh, n_EdgeMesh, n_Mesh::ScalarType, TriMeshGrid>(pl , *edge_mesh, avg_length, mesh_grid, intersected_cells);
					vcg::edge::UpdateBounding<n_EdgeMesh>::Box(*edge_mesh);

					fileN="./SvgSample/"+fileName.left( fileName.length ()- 4 )+"XY"+".svg";
					vcg::edge::io::ExporterSVG<n_EdgeMesh>::Save(edge_mesh, fileN.toLatin1().data(),pr);			

			
				} 
				break;

				case 1: 
				{ 
					Point3f* dir=new Point3f(0,0,1);   //the plans' normal vector init 
					Point3f translation_plans=trackball_slice.track.tra;  //vettore di translazione dei piani
				
					Point3d d((*dir).X(),(*dir).Y(), (*dir).Z());
					pr.setPlane(0, d); 

					mesh_grid = new TriMeshGrid();
					mesh_grid->Set(m.cm.face.begin() ,m.cm.face.end());
					
					float scale = (pr.getViewBox().V(0)*2)/(edgeMax*1.4142) ;
					pr.setScale(scale);

					Point3f rotationCenter=m.cm.bbox.Center(); //the point where the plans rotate
					Plane3f pl;
					pl.SetDirection(*dir);
				
					Point3f off= translation_plans; //translation vector
					pl.SetOffset( (rotationCenter.X()*dir->X() )+ (rotationCenter.Y()*dir->Y()) +(rotationCenter.Z()*dir->Z())+ (off*(*dir)) + PlanVal);
			
					vcg::Intersection<n_Mesh, n_EdgeMesh, n_Mesh::ScalarType, TriMeshGrid>(pl , *edge_mesh, avg_length, mesh_grid, intersected_cells);
					vcg::edge::UpdateBounding<n_EdgeMesh>::Box(*edge_mesh);

					fileN="./SvgSample/"+fileName.left( fileName.length ()- 4 )+"YZ"+".svg";
					vcg::edge::io::ExporterSVG<n_EdgeMesh>::Save(edge_mesh, fileN.toLatin1().data(),pr);
        					
				} 
				break;

				case 2: 
				{ 	
					Point3f* dir=new Point3f(1,0,0);   //the plans' normal vector init 
					Point3f translation_plans=trackball_slice.track.tra;  //vettore di translazione dei piani
					
					Point3d d((*dir).X(),(*dir).Y(), (*dir).Z());
					pr.setPlane(0, d); 

					mesh_grid = new TriMeshGrid();
					mesh_grid->Set(m.cm.face.begin() ,m.cm.face.end());
					
					float scale = (pr.getViewBox().V(0)*2)/(edgeMax*1.4142) ;
					pr.setScale(scale);

					Point3f rotationCenter=m.cm.bbox.Center(); //the point where the plans rotate
					Plane3f pl;
					pl.SetDirection(*dir);
				
					Point3f off= translation_plans; //translation vector
					pl.SetOffset( (rotationCenter.X()*dir->X() )+ (rotationCenter.Y()*dir->Y()) +(rotationCenter.Z()*dir->Z())+ (off*(*dir)) + PlanVal);
			
					vcg::Intersection<n_Mesh, n_EdgeMesh, n_Mesh::ScalarType, TriMeshGrid>(pl , *edge_mesh, avg_length, mesh_grid, intersected_cells);
					vcg::edge::UpdateBounding<n_EdgeMesh>::Box(*edge_mesh);

					fileN="./SvgSample/"+fileName.left( fileName.length ()- 4 )+"ZX"+".svg";
					vcg::edge::io::ExporterSVG<n_EdgeMesh>::Save(edge_mesh, fileN.toLatin1().data(),pr);
				}
				break;
					
				default: assert(0);
			} 
		}
		break;
	}
	return true;
}

const MeshFilterInterface::FilterClass ExtraFilter_SlicePlugin::getClass(QAction *filter)
{
	switch(ID(filter))
	{
		case FP_PLAN :
		{
			return MeshFilterInterface::Generic; 
		}
		break;
		
		default: assert(0);
              	return MeshFilterInterface::Generic;
	}
}

bool ExtraFilter_SlicePlugin::autoDialog(QAction *action)
{
  switch(ID(action)) 
  {
  case  FP_PLAN: return true;
	default: return false;
  }
}

Q_EXPORT_PLUGIN(ExtraFilter_SlicePlugin)
