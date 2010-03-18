/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004-2008                                           \/)\/    *
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
#include <common/interfaces.h>
#include <vcg/math/perlin_noise.h>
#include <vcg/complex/trimesh/clean.h>
#include <wrap/qt/col_qt_convert.h>


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
QString ExtraSampleDynPlugin::filterName(FilterIDType filterId) const
{
  switch(filterId) {
		case FP_VERTEX_COLOR_NOISE :  return QString("Vertex Color Noise"); 
		default : assert(0); 
	}
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
 QString ExtraSampleDynPlugin::filterInfo(FilterIDType filterId) const
{
  switch(filterId) {
		case FP_VERTEX_COLOR_NOISE :  return QString("Randomly add a small amount of a random base color to the mesh"); 
		default : assert(0); 
	}
}

// The FilterClass describes in which generic class of filters it fits. 
// This choice affect the submenu in which each filter will be placed 
// In this case this sample belong to the class of filters that change the vertex colors
 MeshFilterInterface::FilterClass ExtraSampleDynPlugin::getClass(QAction *) { return MeshFilterInterface::VertexColoring; }

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parmeter you need to define, 
// - the name of the parameter, 
// - the string shown in the dialog 
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
// 
// In this sample a couple of parameter are declared as dynamic. That means that the meshlab framework will automatically 
// manage the store and restore of the mesh state during the dynamic movement of the filter. 
// The pluging writer is no more burdened with the task of saving the state but has only to declare what the filter changes 
// (in this case just the vertex color). When a filter is dynamic (e.g. it has a dynamic float parameter) the meshlab 
// framework will automatically store that part of the state at the opening of the dialog. When the user drag the slider,
// the framework will restore the state and then simply call the apply callback of the filter. 
// So from the point of view of the filter every time you have the freshly new mesh. 
// 
// when the user press apply the current stored state is updated. 
// when the user press close the mesh state is restored to the one before the startup of the filter.

void ExtraSampleDynPlugin::initParameterSet(QAction *action,MeshModel &/*m*/, RichParameterSet & parlst)
{
	 switch(ID(action))	 {
		case FP_VERTEX_COLOR_NOISE :  
 		  parlst.addParam(new RichColor ("baseColor",
											 Color4b::Black,
											"BaseColor",
											"The base color that is added to the mesh."));
			
			parlst.addParam(new RichDynamicFloat("percentage",
												 0.5, 0, 1,
												"Alpha",
												"The random color is blended with the current one with the specified alpha"));
														
			parlst.addParam(new RichDynamicFloat("frequency",
														 20, 1, 200,
														 "Noisy Frequency",
														 "The frequency of the Noise on the mesh. Higher numbers means smaller spots."));
			break;
			
   default: break; // do not add any parameter for the other filters
  }
}

// The Real Core Function doing the actual mesh processing.
// It changes the color of the mesh according to a perlin noise function
bool ExtraSampleDynPlugin::applyFilter(QAction *, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *)
{
    MeshModel &m=*(md.mm());
    const Color4b baseColor = par.getColor4b("baseColor");
	const float percentage  = par.getDynamicFloat("percentage");
	const float frequency   = math::Clamp(par.getDynamicFloat("frequency"),1.f,1000.f);

	CMeshO::VertexIterator vi;
	
	float scale = frequency/m.cm.bbox.Diag() ;
	
  //qDebug("Dynamic Apply percentage %f frequency %f",percentage,frequency);
 	for(vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi)
		if(!(*vi).IsD()) 
		{
			float alpha = percentage *  (0.5f+math::Perlin::Noise((*vi).P()[0]*scale,(*vi).P()[1]*scale,(*vi).P()[2]*scale));
			alpha=math::Clamp(alpha,0.0f,1.0f);
			(*vi).C().lerp ( (*vi).C(), baseColor, alpha);
		}
			
	return true;
}

Q_EXPORT_PLUGIN(ExtraSampleDynPlugin)
