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
$Log$
Revision 1.1  2008/02/18 18:05:20  amaione
UPDATED PERSONAL PROJECTS FILES
AND
ADDED PROJECT AND SRC FILES FOR FILTER QUALITY

Revision 1.1  2008/02/18 17:14:40  amaione
ADDED SKELETON FILES FOR QUALITY MAPPER FILTER

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

#include "filterqualitymapper.h"

// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

QualityMapperFilter::QualityMapperFilter() 
{ 
	typeList << FP_MOVE_VERTEX;
  
  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
const QString QualityMapperFilter::filterName(FilterIDType filterId) 
{
  switch(filterId) {
		case FP_MOVE_VERTEX :  return QString("Quality Mapper plugin applier"); 
		default : assert(0); 
	}
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
const QString QualityMapperFilter::filterInfo(FilterIDType filterId)
{
  switch(filterId) {
		case FP_MOVE_VERTEX :  return QString("Edits color of mesh vertexes using Quality Mapper edit functionalities"); 
		default : assert(0); 
	}
}

const PluginInfo &QualityMapperFilter::pluginInfo()
{
   static PluginInfo ai;
   ai.Date=tr(__DATE__);
	 ai.Version = tr("1.0");
	 ai.Author = ("Federico Bellucci, Alessandro Maione");
   return ai;
 }

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parameter you need to define, 
// - the name of the parameter, 
// - the string shown in the dialog 
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void QualityMapperFilter::initParameterSet(QAction *action,MeshModel &m, FilterParameterSet & parlst) 
//void QualityMapperFilter::initParList(QAction *action, MeshModel &m, FilterParameterSet &parlst)
{
	 switch(ID(action))	 {
		case FP_MOVE_VERTEX : 
			parlst.addString("nome", "valore di default", "descrizione", "tooltip");
//  		  parlst.addBool ("UpdateNormals",
// 											true,
// 											"Recompute normals",
// 											"Toggle the recomputation of the normals after the random displacement.\n\n"
// 											"If disabled the face normals will remains unchanged resulting in a visually pleasant effect.");
// 			parlst.addAbsPerc("Displacement",
// 												m.cm.bbox.Diag()/100.0,0,m.cm.bbox.Diag(),
// 												"Max displacement",
// 												"The vertex are displaced of a vector whose norm is bounded by this value");
			break;
											
		default : assert(0); 
	}
}

// The Real Core Function doing the actual mesh processing.
// Move Vertex of a random quantity
bool QualityMapperFilter::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & par, vcg::CallBackPos *cb)
{
	//MeshModel &m=*md->mm();
	srand(time(NULL)); 
	const float max_displacement =par.getAbsPerc("Displacement");

 	for(unsigned int i = 0; i< m.cm.vert.size(); i++){
		 // Typical usage of the callback for showing a nice progress bar in the bottom. 
		 // First parameter is a 0..100 number indicating percentage of completion, the second is an info string.
		  cb(100*i/m.cm.vert.size(), "Randomly Displacing...");

		float rndax = (float(2.0f*rand())/RAND_MAX - 1.0f ) *max_displacement;
		float rnday = (float(2.0f*rand())/RAND_MAX - 1.0f ) *max_displacement;
		float rndaz = (float(2.0f*rand())/RAND_MAX - 1.0f ) *max_displacement;
		m.cm.vert[i].P() += vcg::Point3f(rndax,rnday,rndaz);		
	}
	
	// Log function dump textual info in the lower part of the MeshLab screen. 
	Log(0,"Successfully displaced %i vertices",m.cm.vn);
	
	// to access to the parameters of the filter dialog simply use the getXXXX function of the FilterParameter Class
	if(par.getBool("UpdateNormals"))	
			vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
	
	vcg::tri::UpdateBounding<CMeshO>::Box(m.cm);
  
	return true;
}

Q_EXPORT_PLUGIN(QualityMapperFilter)
