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
#include <map>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

#include <vcg/complex/trimesh/clean.h>

#include "filter_texture.h"

FilterTexturePlugin::FilterTexturePlugin() 
{ 
	typeList << FP_UVTOCOLOR;
  
	foreach(FilterIDType tt , types())
		actionList << new QAction(filterName(tt), this);
}

const QString FilterTexturePlugin::filterName(FilterIDType filterId) const 
{
	switch(filterId) {
		case FP_UVTOCOLOR : return QString("UV to Color"); 
		default : assert(0); 
	}
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
const QString FilterTexturePlugin::filterInfo(FilterIDType filterId) const
{
	switch(filterId)
	{
		case FP_UVTOCOLOR :  return QString("Maps the UV Space into a color space (Red Green), thus colorizing mesh vertices according to UV coords."); 
		default : assert(0); 
	}
	return QString("Unknown Filter");
}

int FilterTexturePlugin::getPreConditions(QAction *a) const
{
	switch (ID(a))
	{
		case FP_UVTOCOLOR: return MeshFilterInterface::FP_WedgeTexCoord;
		default: assert(0);
	}
	return MeshFilterInterface::FP_Generic;
}

const int FilterTexturePlugin::getRequirements(QAction *a)
{
	switch (ID(a))
	{
		case FP_UVTOCOLOR: return MeshModel::MM_VERTMARK;
		default: assert(0);	
	}
	return MeshModel::MM_NONE;
}

int FilterTexturePlugin::postCondition( QAction *a) const
{
	switch (ID(a))
	{
		case FP_UVTOCOLOR: return MeshModel::MM_VERTCOLOR;
		default: assert(0);	
	}
	return MeshModel::MM_NONE;
}

// The FilterClass describes in which generic class of filters it fits. 
// This choice affect the submenu in which each filter will be placed 
// More than a single class can be choosen.
const FilterTexturePlugin::FilterClass FilterTexturePlugin::getClass(QAction *a)
{
  switch(ID(a))
	{
		case FP_UVTOCOLOR :  return MeshFilterInterface::VertexColoring; 
		default : assert(0); 
	}
	return MeshFilterInterface::Generic;
}

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parmeter you need to define, 
// - the name of the parameter, 
// - the string shown in the dialog 
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void FilterTexturePlugin::initParameterSet(QAction *action, MeshModel &m, RichParameterSet & parlst) 
{
	 switch(ID(action))	 {
		 case FP_UVTOCOLOR : break; 
		 default : assert(0); 
	}
}

// The Real Core Function doing the actual mesh processing.
bool FilterTexturePlugin::applyFilter(QAction *filter, MeshModel &m, RichParameterSet &par, vcg::CallBackPos *cb)
{
	switch(ID(filter))	 {
		case FP_UVTOCOLOR : {
			assert(m.hasDataMask(MeshModel::MM_WEDGTEXCOORD));
			assert(m.hasDataMask(MeshModel::MM_VERTMARK));
			m.cm.UnMarkAll();
			
			//Holds per vertex texture coordinates
			std::map<CVertexO*, CFaceO::TexCoordType*> vertcoord;
			
			CVertexO *v;
			CFaceO::TexCoordType *wtc;
			
			//Iterates through faces updating vertex coords map (vertcoord) using per wedge texture coords
			//If for a vertex two different Wedge texture coordinates are found then the vertex is marked (as seam).
			CFaceO *fi;
			uint size = m.cm.face.size();
			for (uint i=0; i < size; ++i)
			{
				fi = &m.cm.face[i];
				if (!fi->IsD())
					for (int i=0; i<3; ++i)
					{
						v = fi->V(i);
						wtc = vertcoord[v]; 
						if (!wtc)
							vertcoord[v] = &fi->WT(i);
						else if (!m.cm.IsMarked(v) && *wtc != fi->WT(i))
							m.cm.Mark(v);
					}
				cb(i*70/size, "Finding seams...");
			}
			//Iterates trough vertices coloring them with:
			// blue for seams (marked ones)
			// Red-Green space corresponding color otherwise
			CVertexO *vi;
			size = m.cm.vert.size();
			for (uint i=0; i < size; ++i)
			{
				vi = &m.cm.vert[i];
				if (m.cm.IsMarked(vi))
					vi->C() = vcg::Color4b(0, 0, 255, 0);
				else if ((wtc = vertcoord[vi]))
					vi->C() = vcg::Color4b(wtc->U()*255, wtc->V()*255, 0, 0);
				cb(i*30/size+70, "Colorizing mesh...");
			}
			m.cm.UnMarkAll();
			break;
		}
		default: assert(0);
	}
	
	return true;
}

Q_EXPORT_PLUGIN(FilterTexturePlugin)
