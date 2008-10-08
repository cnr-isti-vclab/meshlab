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

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/update/position.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/allocate.h>

#include <vcg/complex/trimesh/update/selection.h>
#include <vcg/complex/trimesh/append.h>

#include "filter_rangemap.h"

using namespace std;
using namespace vcg;

// Constructor 
RangeMapPlugin::RangeMapPlugin() 
{ 
	typeList << 
	FP_SELECTBYANGLE;
  
  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

// ST() return the very short string describing each filtering action 
const QString RangeMapPlugin::filterName(FilterIDType filterId) 
{
  switch(filterId) {
		case FP_SELECTBYANGLE :  return QString("Select Faces by view angle"); 
		default : assert(0); 
	}
}

// Info() return the longer string describing each filtering action 
const QString RangeMapPlugin::filterInfo(FilterIDType filterId)
{
  switch(filterId) {
		case FP_SELECTBYANGLE :  return QString("Select faces according to the angle between their normal and the view direction. It is used in range map processing to select and delete steep faces parallel to viewdirection"); 
		default : assert(0); 
	}
}

const PluginInfo &RangeMapPlugin::pluginInfo()
{
   static PluginInfo ai;
   ai.Date=tr(__DATE__);
	 ai.Version = tr("1.2");
	 ai.Author = ("Marco Callieri");
   return ai;
 }

// Return true if the specified action has an automatic dialog.
// return false if the action has no parameters or has an self generated dialog.
bool RangeMapPlugin::autoDialog(QAction *action)
{
	 switch(ID(action))
	 {
		 case FP_SELECTBYANGLE :
			 return true;
	 }
  return false;
}

// This function define the needed parameters for each filter. 
void RangeMapPlugin::initParameterSet(QAction *action, MeshDocument &m, FilterParameterSet & parlst) 
{
	 switch(ID(action))	 
	 {
		case FP_SELECTBYANGLE :
			{
				parlst.addFloat("anglelimit",
												75.0f,
												"angle threshold",
												"faces with normal at higher angle w.r.t. the view direction are selected");
	 		  parlst.addBool ("usecamera",
												false,
												"Use ViewPoint from Mesh Camera",
												"Uses the ViewPoint from the camera associated to the current mesh\n if there is no camera, an error occurs");
				parlst.addPoint3f("viewpoint",
												Point3f(0.0f, 0.0f, 0.0f),
												"ViewPoint",
												"if UseCamera is true, this value is ignored");
			}
			break;
											
		default : assert(0); 
	}
}

// Core Function doing the actual mesh processing.
bool RangeMapPlugin::applyFilter(QAction *filter, MeshDocument &m, FilterParameterSet & par, vcg::CallBackPos *cb)
{
	CMeshO::FaceIterator   fi;

	switch(ID(filter))
  {
		case FP_SELECTBYANGLE : 
		{
			bool usecam = par.getBool("usecamera");
			Point3f viewpoint = par.getPoint3f("viewpoint");	

			// if usecamera but mesh does not have one
			if((usecam)&&((m.mm()->ioMask & MeshModel::IOM_CAMERA)==0)) 
				return false;

			if(usecam)
			{
				viewpoint = m.mm()->cm.shot.GetViewPoint();
			}

			// angle threshold in radians
			float limit = cos( par.getFloat("anglelimit")*3.14159265358979323846/180.0 );
			Point3f viewray;

			for(fi=m.mm()->cm.face.begin();fi!=m.mm()->cm.face.end();++fi)
				if(!(*fi).IsD())
				{
					viewray = viewpoint - (((*fi).V(0)->P() + (*fi).V(1)->P() + (*fi).V(2)->P()) / 3.0);
					viewray.Normalize();

					if((viewray * (*fi).N().Normalize()) < limit)
						fi->SetS();
				}

		}
		break;

	}

	return true;
}

const RangeMapPlugin::FilterClass RangeMapPlugin::getClass(QAction *a)
{
  switch(ID(a))
  {
    	case FP_SELECTBYANGLE :
				return MeshFilterInterface::FilterClass(MeshFilterInterface::RangeMap + MeshFilterInterface::Selection);     
	default : 
		return MeshFilterInterface::Generic;
  }
}

Q_EXPORT_PLUGIN(RangeMapPlugin)
