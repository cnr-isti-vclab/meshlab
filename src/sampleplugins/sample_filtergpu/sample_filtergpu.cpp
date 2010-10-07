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

#include "samplefiltergpu.h"
#include <QtScript>
#include <QGLFramebufferObject>
#include <QImage>

// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

ExtraSampleGPUPlugin::ExtraSampleGPUPlugin() 
{ 
	typeList << FP_GPU_EXAMPLE;
  
  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
QString ExtraSampleGPUPlugin::filterName(FilterIDType filterId) const
{
  switch(filterId) {
		case FP_GPU_EXAMPLE :  return QString("GPU Filter Example "); 
		default : assert(0); 
	}
  return QString();
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
 QString ExtraSampleGPUPlugin::filterInfo(FilterIDType filterId) const
{
  switch(filterId) {
		case FP_GPU_EXAMPLE :  return QString("Small useless filter added only to show how to work with a gl render context inside a filter."); 
		default : assert(0); 
	}
	return QString("Unknown Filter");
}

// The FilterClass describes in which generic class of filters it fits. 
// This choice affect the submenu in which each filter will be placed 
// More than a single class can be choosen.
ExtraSampleGPUPlugin::FilterClass ExtraSampleGPUPlugin::getClass(QAction *a)
{
  switch(ID(a))
	{
		case FP_GPU_EXAMPLE :  return MeshFilterInterface::Generic; 
		default : assert(0); 
	}
	return MeshFilterInterface::Generic;
}

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parameter you need to define, 
// - the name of the parameter, 
// - the string shown in the dialog 
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void ExtraSampleGPUPlugin::initParameterSet(QAction *action,MeshModel &m, RichParameterSet & parlst) 
{
	switch(ID(action))	 
	{
		case FP_GPU_EXAMPLE :
		{
			QColor col(100,50,240);
			parlst.addParam(new RichColor ("ClearColor",col,"Clear Color","Color used to clear the gl color buffer."));
			parlst.addParam(new RichBool ("LogFBOBuffer",false,"Log FBOBuffer","Produce a log of the current FBO status in the Layer Log window."));
			break;
		}
		default : assert(0); 
	}
}

// The Real Core Function doing the actual mesh processing.
// Move Vertex of a random quantity
bool ExtraSampleGPUPlugin::applyFilter(QAction * a, MeshDocument &/*md*/, RichParameterSet & par, vcg::CallBackPos */*cb*/)
{
	switch(ID(a))
	{
		case FP_GPU_EXAMPLE :
		{
			glContext->makeCurrent();
			QSize fbosize(4,4);
			QGLFramebufferObject fbo(fbosize);
			QColor clcolor = par.getColor("ClearColor");
			fbo.bind();
			glClearColor(clcolor.redF(),clcolor.greenF(),clcolor.blueF(),1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			fbo.release();
			if (par.getBool("LogFBOBuffer"))
			{
				QImage img(fbo.toImage());

				for(int ii = 0; ii < fbosize.height() * fbosize.width();++ii)
					this->Log("Pixel[%d][%d] = [%f,%f,%f]",ii / fbosize.height(),ii % fbosize.height(),clcolor.redF(),clcolor.greenF(),clcolor.blueF());
					
			}
			glContext->doneCurrent();
			break;
		}
	}
	return true;
}

Q_EXPORT_PLUGIN(ExtraSampleGPUPlugin)
