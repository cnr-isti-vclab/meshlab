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


#include "samplefilter.h"

// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

ExtraSamplePlugin::ExtraSamplePlugin() 
{ 
	typeList << FP_MOVE_VERTEX;
  
  foreach(FilterType tt , types())
	  actionList << new QAction(ST(tt), this);
}

// The very short string describing each filtering action (this string is used also to define the menu entry)
const QString ExtraSamplePlugin::ST(FilterType filter) 
{
  return QString("Random Vertex Displacement");
}

// The longer string describing each filtering action 
// (this string is used in the About plugin dialog)
const QString ExtraSamplePlugin::Info(FilterType filter)
{
   return tr("Apply Filter Move Vertex");
}

const PluginInfo &ExtraSamplePlugin::Info()
{
   static PluginInfo ai;
   ai.Date=tr(__DATE__);
	 ai.Version = tr("0.8");
	 ai.Author = ("Elisa Cerisoli, Paolo Cignoni");
   return ai;
 }

//Move Vertex of random quantity
bool ExtraSamplePlugin::applyFilter(QAction *filter, MeshModel &m, FilterParameter & par, vcg::CallBackPos *cb)
{
	srand(time(NULL)); 
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
