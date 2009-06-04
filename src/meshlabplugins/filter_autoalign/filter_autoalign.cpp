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
#include <vcg/complex/trimesh/append.h>
#include <vcg/complex/trimesh/autoalign_4pcs.h>

#include "filter_autoalign.h"

using namespace vcg;
using namespace std;
// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

FilterAutoalign::FilterAutoalign() 
{ 
	typeList << FP_AUTOALIGN;
  
  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
const QString FilterAutoalign::filterName(FilterIDType filterId) const
{
  switch(filterId) {
		case FP_AUTOALIGN :  return QString("Automatic pair Alignement"); 
		default : assert(0); 
	}
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
const QString FilterAutoalign::filterInfo(FilterIDType filterId) const
{
  switch(filterId) {
		case FP_AUTOALIGN :  return QString(" Automatic Rough Alignment of two meshes. Based on the paper <b> 4-Points Congruent Sets for Robust Pairwise Surface Registration</b>, by Aiger,Mitra, Cohen-Or. Siggraph 2008  "); 
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
void FilterAutoalign::initParameterSet(QAction *action,MeshDocument & md/*m*/, FilterParameterSet & parlst) 
//void ExtraSamplePlugin::initParList(QAction *action, MeshModel &m, FilterParameterSet &parlst)
{
	 MeshModel *target;
	 switch(ID(action))	 {
		case FP_AUTOALIGN :  
 				target= md.mm();
				foreach (target, md.meshList) 
						if (target != md.mm())  break;
		    
				parlst.addMesh ("FirstMesh", md.mm(), "First Mesh",
												"The mesh were the coplanar bases are sampled (it will contain the trasformation)");
				parlst.addMesh ("SecondMesh", target, "Second Mesh",
												"The mesh were similar coplanar based are searched.");
				parlst.addFloat("overlapping",0.5f,"Estimated fraction of the\n first mesh overlapped by the second");
				parlst.addFloat("tolerance [0.0,1.0]",0.3f,"Error tolerance");
		 break;		
		default : assert(0); 
	}
}

// The Real Core Function doing the actual mesh processing.
bool FilterAutoalign::applyFilter(QAction *filter, MeshDocument &md, FilterParameterSet & par, vcg::CallBackPos *cb)
{
	vcg::tri::FourPCS<CMeshO> *fpcs ;
	bool res;
	switch(ID(filter)) {
		case FP_AUTOALIGN :
		{
			MeshModel *firstMesh= par.getMesh("FirstMesh");
			MeshModel *secondMesh= par.getMesh("SecondMesh");
			fpcs = new vcg::tri::FourPCS<CMeshO>();
			fpcs->prs.Default();
			fpcs->prs.f =  par.getFloat("overlapping");
			firstMesh->cm.vert.EnableMark();
			secondMesh->cm.vert.EnableMark();
			fpcs->Init(firstMesh->cm,secondMesh->cm);
			res = fpcs->Align(0,firstMesh->cm.Tr,cb);
			firstMesh->cm.vert.DisableMark();
			secondMesh->cm.vert.DisableMark();

			// Log function dump textual info in the lower part of the MeshLab screen. 
			Log(0,(res)?" Automatic Rough Alignment Done":"Automatic Rough Alignment Failed");
				delete fpcs;
		} break;
		default: assert (0);
	}
	return true;
}

const FilterAutoalign::FilterClass FilterAutoalign::getClass(QAction *a)
{
  switch(ID(a))
  {
    case FP_AUTOALIGN :
      return MeshFilterInterface::Layer;     
    default : 
			return MeshFilterInterface::Generic;
  }
}

Q_EXPORT_PLUGIN(FilterAutoalign)
