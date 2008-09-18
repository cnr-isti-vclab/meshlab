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
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>

#include <string>

#include "filter_func.h"
#include "muParser.h"

using namespace mu;
using namespace vcg;

// Constructor
FilterFunctionPlugin::FilterFunctionPlugin() 
{ 
	typeList << FF_SELECTION;
  
	foreach(FilterIDType tt , types())
		actionList << new QAction(filterName(tt), this);
}

// short string describing each filtering action 
const QString FilterFunctionPlugin::filterName(FilterIDType filterId) 
{
	switch(filterId) {
		case FF_SELECTION :  return QString("Conditional Selection"); 
		default : assert(0); 
	}
	return QString("error!");
}

// long string describing each filtering action 
const QString FilterFunctionPlugin::filterInfo(FilterIDType filterId)
{
	switch(filterId) {
		case FF_SELECTION :  return QString("Boolean function using muparser lib to perform vertex selection"); 
		default : assert(0); 
	}
	return QString("error!");
}

const PluginInfo &FilterFunctionPlugin::pluginInfo()
{
   static PluginInfo ai;
   ai.Date=tr(__DATE__);
	 ai.Version = tr("0.1");
	 ai.Author = ("Giuseppe Alemanno");
   return ai;
 }

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parmeter you need to define, 
// - the name of the parameter, 
// - the string shown in the dialog 
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void FilterFunctionPlugin::initParameterSet(QAction *action,MeshModel &m, FilterParameterSet & parlst) 
{
	switch(ID(action))	 {
		case FF_SELECTION :
			parlst.addString("condSelect"," ", "boolean function",
							 "you can use: ( ) and or < > = \
							 x,y,z for vertex coord, nx, ny, nz for normal coord, r, g, b for color and q for quality");
			break;
		default : assert(0); 
	}
}

// The Real Core Function doing the actual mesh processing.
bool FilterFunctionPlugin::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & par, vcg::CallBackPos *cb)
{
	QString select = par.getString("condSelect");

	Parser p;

	double x,y,z,nx,ny,nz,r,g,b,q;
	
	p.DefineVar("x", &x);
	p.DefineVar("y", &y);
	p.DefineVar("z", &z);
	p.DefineVar("nx", &nx);
	p.DefineVar("ny", &ny);
	p.DefineVar("nz", &nz);
	p.DefineVar("r", &r);
	p.DefineVar("g", &g);
	p.DefineVar("b", &b);
	p.DefineVar("q", &q);
	
	std::string expr = select.toStdString();
	p.SetExpr(expr);

	int numvert = 0;
	time_t	start = clock();

	CMeshO::VertexIterator vi;
	for(vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi)
	{
		x = (*vi).P()[0]; // coord x
		y = (*vi).P()[1]; // coord y
		z = (*vi).P()[2]; // coord z

		nx = (*vi).N()[0]; // normal coord x
		ny = (*vi).N()[1]; // normal coord y
		nz = (*vi).N()[2]; // normal coord z

		r = (*vi).C()[0];  // color R
		g = (*vi).C()[1];  // color G
		b = (*vi).C()[2];  // color B

		q = (*vi).Q();     // quality

		bool selected = false;

		try {
			selected = p.Eval();
		} catch(mu::Parser::exception_type &e) {
			Log(GLLogStream::Info, "parsing error. check syntax");
			return false;
		}

		if(selected) { 
			(*vi).SetS();
			numvert++;
		} else (*vi).ClearS();
	}

	Log(GLLogStream::Info, "selected %d vertices in %.2f sec.", numvert, (clock() - start) / (float) CLOCKS_PER_SEC);

	return true;
}

Q_EXPORT_PLUGIN(FilterFunctionPlugin)
