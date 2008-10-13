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
#include <sys/stat.h>
#include <sys/types.h>
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


using namespace std;
using namespace vcg;

// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

ExtraFilter_SlicePlugin::ExtraFilter_SlicePlugin () 
{ 
	typeList << FP_PLANE;
  
  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
const QString ExtraFilter_SlicePlugin::filterName(FilterIDType filterId) 
{
  switch(filterId) {
		case FP_PLANE :  return QString("Cross section plane"); 
		default : assert(0); 
	}
  return QString("error!");
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
const QString ExtraFilter_SlicePlugin::filterInfo(FilterIDType filterId)
{
 switch(filterId) {
		case FP_PLANE :  return QString("Export a cross section of the object and of the associated bounding box relative to one of the XY, YZ or ZX axes in svg format. By default, the cross-section goes through the middle of the object (Cross plane offset == 0)."); 
	
		default : assert(0); 
	}
  return QString("error!");
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
	
		  case FP_PLANE :  
		  {
 		   	QStringList metrics;
				metrics.push_back("XY Axis");
				metrics.push_back("YZ Axis");
				metrics.push_back("ZX Axis");
				parlst.addEnum("plane", 0, metrics, tr("Axis:"), tr("Choose a cross section axis."));
				parlst.addFloat("CrossplaneVal", 0.0, "Cross plane offset", "Specify an offset of the cross-plane as a float. The offset corresponds to the distance between the center of the object and the point where the plan crosses it. By default (Cross plane offset == 0), the plane crosses the center of the object, so the offset can be positive or negetive");
				parlst.addString("filename", "Slice", "Name of the svg files and of the folder contaning them, it is automatically created in the Sample folder of the Meshlab tree");
				parlst.addBool ("SelAlone",m.cm.sfn>0,"Automatically generate a series of cross-sections along the whole length of the object and store each plane in a separate SVG file. The distance between each plane is given by the step value below");
				parlst.addBool ("SelAll",m.cm.sfn>0,"Automatically generate a series of cross-sections along the whole length of the object and store each plane in a same SVG file. All the planes are surimposed. The distance between each plane is given by the step value below");
				parlst.addFloat("CrossStepVal", 0.0, "Step value between each plane for automatically generating cross-sections. Should be used with the bool selection above.");			 		  
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
		case FP_PLANE :
		{
			int plane = parlst.getEnum("plane");	
			int i=0;
			bool selAlone = parlst.getBool("SelAlone");
			bool selAll = parlst.getBool("SelAll");
			float StepVal = parlst.getFloat("CrossStepVal");
			float planeVal = parlst.getFloat("CrossplaneVal");
			Number = QString::number(planeVal);
			number = QString::number(StepVal);
			fileName = parlst.getString("filename");
			folderN = fileName;
			const char *folderName = fileName.toLatin1();
			mkdir(folderName, 0777);
			if (fileName==0) return false;
    			if (!(fileName.endsWith("svg")))
			fileName= fileName+".svg";
			double avg_length; 
			b=m.cm.bbox; //Boundig Box
			Point3f mi=b.min;
			Point3f ma=b.max;
			Point3f center=b.Center() ;
			edgeMax= max((ma[0]-mi[0]), (ma[1]-mi[1]));
			edgeMax=max(edgeMax, (ma[2]-mi[2])); //edgeMax: the longest side of BBox
			pr.numCol=1;
			pr.numRow=1;
			mesh_grid = new TriMeshGrid();
			mesh_grid->Set(m.cm.face.begin() ,m.cm.face.end());
			
			switch(plane)
			{ 
				case 0: 
				{
					Point3f* dir=new Point3f(0,0,1);   //the planes' normal vector init 
					Point3d d((*dir).X(),(*dir).Y(), (*dir).Z());
					pr.setPlane(0, d);
						
					float scale = (pr.getViewBox().V(0))/(edgeMax*2);
					pr.setScale(scale);
					Plane3f pl;
					pl.SetDirection(*dir);

				       		if(!selAll && !selAlone)
						{
							edge_mesh = new n_EdgeMesh();
							pl.SetOffset( (center.X()*dir->X() )+ (center.Y()*dir->Y()) +(center.Z()*dir->Z())+ planeVal);
							vcg::Intersection<n_Mesh, n_EdgeMesh, n_Mesh::ScalarType, TriMeshGrid>(pl , *edge_mesh, avg_length, mesh_grid, intersected_cells);
							vcg::edge::UpdateBounding<n_EdgeMesh>::Box(*edge_mesh);
							fileN="./"+folderN+"/"+fileName.left( fileName.length ()- 4 )+"_XY_"+Number+".svg";
							vcg::edge::io::ExporterSVG<n_EdgeMesh>::Save(edge_mesh, fileN.toLatin1().data(), pr, mi[0], mi[1], ma[0], ma[1], center[1], center[0]);				
						}

						if(selAlone && !selAll && StepVal != 0.0)
						{							
							while((mi[2]+i*StepVal) <= ma[2])
							{
								edge_mesh = new n_EdgeMesh();
								pl.SetOffset( (center.X()*dir->X() )+ (center.Y()*dir->Y()) +(center.Z()*dir->Z()) + mi[2] + (i*StepVal));
								vcg::Intersection<n_Mesh, n_EdgeMesh, n_Mesh::ScalarType, TriMeshGrid>(pl , *edge_mesh, avg_length, mesh_grid, intersected_cells);
								vcg::edge::UpdateBounding<n_EdgeMesh>::Box(*edge_mesh);
								StepNumber = QString::number(mi[2] + (i*StepVal));
								fileN="./"+folderN+"/" + fileName.left( fileName.length ()- 4 ) + "_XY_" + StepNumber + ".svg";
								vcg::edge::io::ExporterSVG<n_EdgeMesh>::Save(edge_mesh, fileN.toLatin1().data(), pr, mi[0], mi[1], ma[0], ma[1], center[1], center[0]);
								i++;
							}	
						}

						if(selAll && !selAlone && StepVal != 0.0)
						{
							edge_mesh = new n_EdgeMesh();
							fileN="./"+folderN+"/" + fileName.left( fileName.length ()- 4 ) + "_XY_" + number + ".svg";

							while((mi[2]+i*StepVal) <= ma[2])
							{	
								pl.SetOffset( (center.X()*dir->X() )+ (center.Y()*dir->Y()) +(center.Z()*dir->Z()) + mi[2] + (i*StepVal));
								vcg::Intersection<n_Mesh, n_EdgeMesh, n_Mesh::ScalarType, TriMeshGrid>(pl , *edge_mesh, avg_length, mesh_grid, intersected_cells);
								vcg::edge::UpdateBounding<n_EdgeMesh>::Box(*edge_mesh);
								i++;	
							}
							vcg::edge::io::ExporterSVG<n_EdgeMesh>::Save(edge_mesh, fileN.toLatin1().data(), pr, mi[0], mi[1], ma[0], ma[1], center[1], center[0]);		
						}
						
						if( (selAll && selAlone)  || ((selAll || selAlone) && StepVal == 0.0) )
							return false;	
				} 
				break;

				case 1: 
				{
					Point3f* dir=new Point3f(1,0,0);   //the planes' normal vector init 
					Point3d d((*dir).X(),(*dir).Y(), (*dir).Z());
					pr.setPlane(0, d);
						
					float scale = (pr.getViewBox().V(0))/(edgeMax*2) ;
					pr.setScale(scale);
					Plane3f pl;
					pl.SetDirection(*dir);

				       		if(!selAll && !selAlone)
						{
							edge_mesh = new n_EdgeMesh();
							pl.SetOffset( (center.X()*dir->X() )+ (center.Y()*dir->Y()) +(center.Z()*dir->Z())+ planeVal);
							vcg::Intersection<n_Mesh, n_EdgeMesh, n_Mesh::ScalarType, TriMeshGrid>(pl , *edge_mesh, avg_length, mesh_grid, intersected_cells);
							vcg::edge::UpdateBounding<n_EdgeMesh>::Box(*edge_mesh);
							fileN="./"+folderN+"/"+fileName.left( fileName.length ()- 4 )+"_YZ_"+Number+".svg";
							vcg::edge::io::ExporterSVG<n_EdgeMesh>::Save(edge_mesh, fileN.toLatin1().data(), pr, mi[2], mi[1], ma[2], ma[1], center[2], center[1]);		
						}

						if(selAlone && !selAll && StepVal != 0.0)
						{							
							while((mi[0]+i*StepVal) <= ma[0])
							{
								edge_mesh = new n_EdgeMesh();
								pl.SetOffset( (center.X()*dir->X() )+ (center.Y()*dir->Y()) +(center.Z()*dir->Z()) + mi[0] + (i*StepVal));
								vcg::Intersection<n_Mesh, n_EdgeMesh, n_Mesh::ScalarType, TriMeshGrid>(pl , *edge_mesh, avg_length, mesh_grid, intersected_cells);
								vcg::edge::UpdateBounding<n_EdgeMesh>::Box(*edge_mesh);
								StepNumber = QString::number(mi[2] + (i*StepVal));
								fileN="./"+folderN+"/" + fileName.left( fileName.length ()- 4 ) + "_YZ_" + StepNumber + ".svg";
								vcg::edge::io::ExporterSVG<n_EdgeMesh>::Save(edge_mesh, fileN.toLatin1().data(), pr, mi[2], mi[1], ma[2], ma[1], center[2], center[1]);
								i++;
							}	
						}

						if(selAll && !selAlone && StepVal != 0.0)
						{
							edge_mesh = new n_EdgeMesh();
							fileN="./"+folderN+"/" + fileName.left( fileName.length ()- 4 ) + "_YZ_" + number + ".svg";

							while((mi[0]+i*StepVal) <= ma[0])
							{
									
								pl.SetOffset( (center.X()*dir->X() )+ (center.Y()*dir->Y()) +(center.Z()*dir->Z()) + mi[0] + (i*StepVal));
								vcg::Intersection<n_Mesh, n_EdgeMesh, n_Mesh::ScalarType, TriMeshGrid>(pl , *edge_mesh, avg_length, mesh_grid, intersected_cells);
								vcg::edge::UpdateBounding<n_EdgeMesh>::Box(*edge_mesh);
								i++;	
							}
							vcg::edge::io::ExporterSVG<n_EdgeMesh>::Save(edge_mesh, fileN.toLatin1().data(), pr, mi[2], mi[1], ma[2], ma[1], center[2], center[1]);	
						}
						
						if( (selAll && selAlone)  || ((selAll || selAlone) && StepVal == 0.0) )
							return false;	
				} 
				break;

				case 2: 
				{ 	
					Point3f* dir=new Point3f(0,1,0);   //the planes' normal vector init 
					Point3d d((*dir).X(),(*dir).Y(), (*dir).Z());
					pr.setPlane(0, d);
						
					float scale = (pr.getViewBox().V(0))/(edgeMax*2);
					pr.setScale(scale);
					Plane3f pl;
					pl.SetDirection(*dir);

				       		if(!selAll && !selAlone)
						{
							edge_mesh = new n_EdgeMesh();
							pl.SetOffset( (center.X()*dir->X() )+ (center.Y()*dir->Y()) +(center.Z()*dir->Z()) + planeVal);
							vcg::Intersection<n_Mesh, n_EdgeMesh, n_Mesh::ScalarType, TriMeshGrid>(pl , *edge_mesh, avg_length, mesh_grid, intersected_cells);
							vcg::edge::UpdateBounding<n_EdgeMesh>::Box(*edge_mesh);
							fileN="./"+folderN+"/"+fileName.left( fileName.length ()- 4 )+"_ZX_"+Number+".svg";
							vcg::edge::io::ExporterSVG<n_EdgeMesh>::Save(edge_mesh, fileN.toLatin1().data(), pr, mi[0], mi[2], ma[0], ma[2], center[0], center[2]);		
						}

						if(selAlone && !selAll && StepVal != 0.0)
						{							
							while((mi[1]+i*StepVal) <= ma[1])
							{
								edge_mesh = new n_EdgeMesh();
								pl.SetOffset( (center.X()*dir->X() )+ (center.Y()*dir->Y()) +(center.Z()*dir->Z()) + mi[1] + (i*StepVal));
								vcg::Intersection<n_Mesh, n_EdgeMesh, n_Mesh::ScalarType, TriMeshGrid>(pl , *edge_mesh, avg_length, mesh_grid, intersected_cells);
								vcg::edge::UpdateBounding<n_EdgeMesh>::Box(*edge_mesh);
								StepNumber = QString::number(mi[1] + (i*StepVal));
								fileN="./"+folderN+"/"+fileName.left(fileName.length()-4)+"_ZX_"+StepNumber+".svg";
								vcg::edge::io::ExporterSVG<n_EdgeMesh>::Save(edge_mesh, fileN.toLatin1().data(), pr, mi[0], mi[2], ma[0], ma[2], center[0], center[2]);
								i++;
							}	
						}

						if(selAll && !selAlone && StepVal != 0.0)
						{
							edge_mesh = new n_EdgeMesh();
							fileN="./"+folderN+"/"+fileName.left(fileName.length()-4)+"_ZX_"+number+".svg";

							while((mi[1]+i*StepVal) <= ma[1])
							{
									
								pl.SetOffset((center.X()*dir->X() )+(center.Y()*dir->Y())+(center.Z()*dir->Z())+ mi[1] + (i*StepVal));
								vcg::Intersection<n_Mesh, n_EdgeMesh, n_Mesh::ScalarType, TriMeshGrid>(pl, *edge_mesh, avg_length, mesh_grid, intersected_cells);
								vcg::edge::UpdateBounding<n_EdgeMesh>::Box(*edge_mesh);
								i++;	
							}
							vcg::edge::io::ExporterSVG<n_EdgeMesh>::Save(edge_mesh, fileN.toLatin1().data(), pr, mi[0], mi[2], ma[0], ma[2], center[0], center[2]);	
						}
						
						if( (selAll && selAlone) || ((selAll || selAlone) && StepVal == 0.0) )
							return false;	
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
		case FP_PLANE :
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
  case  FP_PLANE: return true;
	default: return false;
  }
}

Q_EXPORT_PLUGIN(ExtraFilter_SlicePlugin)
