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
#include <vcg/complex/trimesh/attribute_seam.h>

#include "filter_texture.h"

FilterTexturePlugin::FilterTexturePlugin() 
{ 
	typeList << FP_UVTOCOLOR
			<< FP_UV_WEDGE_TO_VERTEX;
  
	foreach(FilterIDType tt , types())
		actionList << new QAction(filterName(tt), this);
}

const QString FilterTexturePlugin::filterName(FilterIDType filterId) const 
{
	switch(filterId) {
		case FP_UVTOCOLOR : return QString("UV to Color"); 
		// TODO Change name
		case FP_UV_WEDGE_TO_VERTEX : return QString("Convert PerWedge UV into PerVertex UV");
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
		case FP_UV_WEDGE_TO_VERTEX : return QString("Converts per Wedge Texture Coordinates to per Vertex Texture Coordinates splitting vertices with Wedge coordinates not coherent.");
		default : assert(0); 
	}
	return QString("Unknown Filter");
}

int FilterTexturePlugin::getPreConditions(QAction *a) const
{
	switch (ID(a))
	{
		case FP_UVTOCOLOR : return MeshFilterInterface::FP_VertexTexCoord;
		case FP_UV_WEDGE_TO_VERTEX : return MeshFilterInterface::FP_WedgeTexCoord;
		default: assert(0);
	}
	return MeshFilterInterface::FP_Generic;
}

const int FilterTexturePlugin::getRequirements(QAction *a)
{
	switch (ID(a))
	{
		case FP_UVTOCOLOR :
		case FP_UV_WEDGE_TO_VERTEX :
			return MeshModel::MM_NONE;
		default: assert(0);	
	}
	return MeshModel::MM_NONE;
}

int FilterTexturePlugin::postCondition( QAction *a) const
{
	switch (ID(a))
	{
		case FP_UVTOCOLOR : return MeshModel::MM_VERTCOLOR;
		case FP_UV_WEDGE_TO_VERTEX : return MeshModel::MM_UNKNOWN;
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
		case FP_UVTOCOLOR : return MeshFilterInterface::VertexColoring;
		case FP_UV_WEDGE_TO_VERTEX : return MeshFilterInterface::Texture;
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
	switch(ID(action))	{
		case FP_UVTOCOLOR :
			parlst.addParam(new RichEnum("colorspace", 0, QStringList() << "Red-Green" << "Hue-Saturation", "Color Space", "The color space used to mapping UV to."));
		case FP_UV_WEDGE_TO_VERTEX : 
			break;
		default : assert(0); 
	}
}


/////// FUNCTIONS NEEDED BY "UV WEDGE TO VERTEX" FILTER
inline void ExtractVertex(const CMeshO & srcMesh, const CMeshO::FaceType & f, int whichWedge, const CMeshO & dstMesh, CMeshO::VertexType & v)
{
	(void)srcMesh;
	(void)dstMesh;
	// This is done to preserve every single perVertex property
	// perVextex Texture Coordinate is instead obtained from perWedge one.
	v.ImportLocal(*f.cV(whichWedge));
	v.T() = f.cWT(whichWedge);
}

inline bool CompareVertex(const CMeshO & m, const CMeshO::VertexType & vA, const CMeshO::VertexType & vB)
{
	(void)m;
	return (vA.cT() == vB.cT());
}
/////////////////////////////////////////////////////


// The Real Core Function doing the actual mesh processing.
bool FilterTexturePlugin::applyFilter(QAction *filter, MeshModel &m, RichParameterSet &par, vcg::CallBackPos *cb)
{
	switch(ID(filter))	 {
		case FP_UVTOCOLOR : {
			int vcount = m.cm.vert.size();
			int colsp = par.getEnum("colorspace");
			if (!m.hasDataMask(MeshModel::MM_VERTCOLOR))
				m.updateDataMask(MeshModel::MM_VERTCOLOR);
			for (int i=0; i<vcount; ++i)
			{
				CMeshO::VertexType &v = m.cm.vert[i];
				if (!v.IsD())
				{
					//Normalized 0..1 
					float normU, normV;
					normU = v.T().U() - (int)v.T().U();
					if (normU < 0.) normU += 1.;
					normV = v.T().V() - (int)v.T().V();
					if (normV < 0.) normV += 1.;
					
					switch(colsp) {
						// Red-Green color space
						case 0 : v.C() = vcg::Color4b((int)round(normU*255), (int)round(normV*255), 0, 255); break;
						// Hue-Saturation color space
						case 1 : {
							vcg::Color4f c;
							c.SetHSVColor(normU, normV, 1.0);
							v.C().Import(c);
						}
							break;
						default : assert(0);
					};
				}
				cb(i*100/vcount, "Colorizing vertices from UV coordinates ...");
			}
		}
		break;
		case FP_UV_WEDGE_TO_VERTEX : {
			int vn = m.cm.vn;
			if (!m.hasDataMask(MeshModel::MM_VERTTEXCOORD))
				m.updateDataMask(MeshModel::MM_VERTTEXCOORD);
			vcg::tri::AttributeSeam::SplitVertex(m.cm, ExtractVertex, CompareVertex);
			if (m.cm.vn != vn)
			{
				if (m.hasDataMask(MeshModel::MM_FACEFACETOPO))
					m.clearDataMask(MeshModel::MM_FACEFACETOPO);
				if (m.hasDataMask(MeshModel::MM_VERTFACETOPO))
					m.clearDataMask(MeshModel::MM_VERTFACETOPO);
			}
		}
		break;
		default: assert(0);
	}
	
	return true;
}

Q_EXPORT_PLUGIN(FilterTexturePlugin)
