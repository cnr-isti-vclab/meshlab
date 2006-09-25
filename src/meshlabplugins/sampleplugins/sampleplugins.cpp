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
Revision 1.1  2006/09/25 09:24:39  e_cerisoli
add sampleplugins

****************************************************************************/

#include <QtGui>

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "sampleplugins.h"

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>

//Constructor
ExtraSamplePlugin::ExtraSamplePlugin() 
{ 
	typeList << FP_MOVE_VERTEX;
  
  FilterType tt;
  
  foreach(tt , types())
	  actionList << new QAction(ST(tt), this);
  init_randnumber = false;
}


const QString ExtraSamplePlugin::ST(FilterType filter) 
{
  return QString("Move Vertex");
}

//Destructor
ExtraSamplePlugin::~ExtraSamplePlugin()
{}

const ActionInfo &ExtraSamplePlugin::Info(QAction *action)
{
   static ActionInfo ai;
   ai.Help = tr("Apply Filter Move Vertex");
   return ai;
}


const PluginInfo &ExtraSamplePlugin::Info()
{
   static PluginInfo ai;
   ai.Date=tr("__DATE__");
	 ai.Version = tr("0.5");
	 ai.Author = ("Elisa Cerisoli");
   return ai;
 }

//Move Vertex of random quantity
bool ExtraSamplePlugin::applyFilter(QAction *filter, MeshModel &m, FilterParameter & par, vcg::CallBackPos *cb)
{
	if(!init_randnumber)
	{
		srand(time(NULL)); 
		init_randnumber = true;
	}
	const float max_displacement = m.cm.bbox.Diag()/100;
 	for(int i = 0; i< m.cm.vert.size(); i++){
				
		float rndax = (float(rand())/RAND_MAX)*max_displacement;
		float rnday = (float(rand())/RAND_MAX)*max_displacement;
		float rndaz = (float(rand())/RAND_MAX)*max_displacement;
		m.cm.vert[i].P() += vcg::Point3f(rndax,rnday,rndaz);
		
	}
	vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
	vcg::tri::UpdateBounding<CMeshO>::Box(m.cm);
  
	return true;
}

Q_EXPORT_PLUGIN(ExtraSamplePlugin)
