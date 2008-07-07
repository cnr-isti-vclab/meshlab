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

#include <vcg/math/perlin_noise.h>

#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>

#include "samplefilterdyn.h"

using namespace vcg;

// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

ExtraSampleDynPlugin::ExtraSampleDynPlugin() 
{ 
	typeList << FP_VERTEX_COLOR_NOISE;
  
  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
const QString ExtraSampleDynPlugin::filterName(FilterIDType filterId) 
{
  switch(filterId) {
		case FP_VERTEX_COLOR_NOISE :  return QString("Vertex Color Noise"); 
		default : assert(0); 
	}
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
const QString ExtraSampleDynPlugin::filterInfo(FilterIDType filterId)
{
  switch(filterId) {
		case FP_VERTEX_COLOR_NOISE :  return QString("Randomly add a small amount of a random base color to the mesh"); 
		default : assert(0); 
	}
}

const PluginInfo &ExtraSampleDynPlugin::pluginInfo()
{
   static PluginInfo ai;
   ai.Date=tr(__DATE__);
	 ai.Version = tr("1.0");
	 ai.Author = ("Paolo Cignoni");
   return ai;
 }

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parmeter you need to define, 
// - the name of the parameter, 
// - the string shown in the dialog 
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void ExtraSampleDynPlugin::initParameterSet(QAction *action,MeshModel &m, FilterParameterSet & parlst) 
//void ExtraSamplePlugin::initParList(QAction *action, MeshModel &m, FilterParameterSet &parlst)
{
	 switch(ID(action))	 {
		case FP_VERTEX_COLOR_NOISE :  
 		  parlst.addColor ("baseColor",
											 Color4b::Black,
											"BaseColor",
											"The base color that is added to the mesh.");
			parlst.addDynamicFloat("percentage",
												 0.5, 0, 1,
												MeshModel::MM_VERTCOLOR,
												"Alpha",
												"The random color is blended with the current one with the specified alpha");
														
			parlst.addDynamicFloat("frequency",
														 20, 1, 100,
														 MeshModel::MM_VERTCOLOR,
														 "Noisy Frequency",
														 "The random color is blended with the current one with the specified alpha");
			break;
			
		default : assert(0); 
	}
}

// The Real Core Function doing the actual mesh processing.
// Move Vertex of a random quantity
bool ExtraSampleDynPlugin::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & par, vcg::CallBackPos *cb)
{
	srand(time(NULL)); 
	const Color4b baseColor = par.getColor4b("baseColor");
	const float percentage  = par.getDynamicFloat("percentage");
	const float frequency   = par.getDynamicFloat("frequency");

	CMeshO::VertexIterator vi;
  qDebug("Dynamic Apply percentage %f frequency %f",percentage,frequency);
 	for(vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi)
		if(!(*vi).IsD()) 
		{
			float alpha = percentage *  math::Abs(math::Perlin::Noise((*vi).P()[0]/frequency,(*vi).P()[1]/frequency,(*vi).P()[2]/frequency));
			(*vi).C().lerp ( (*vi).C(), baseColor, alpha);
		}
	
	// Log function dump textual info in the lower part of the MeshLab screen. 
	Log(0,"Successfully displaced %i vertices",m.cm.vn);
	
	return true;
}

Q_EXPORT_PLUGIN(ExtraSampleDynPlugin)
