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
#include <vcg/space/triangle3.h>

#include "filter_texture.h"

FilterTexturePlugin::FilterTexturePlugin() 
{ 
	typeList << FP_UVTOCOLOR
			<< FP_UV_WEDGE_TO_VERTEX
			<< FP_BASIC_TRIANGLE_MAPPING;
  
	foreach(FilterIDType tt , types())
		actionList << new QAction(filterName(tt), this);
}

const QString FilterTexturePlugin::filterName(FilterIDType filterId) const 
{
	switch(filterId) {
		case FP_UVTOCOLOR : return QString("UV to Color"); 
		case FP_UV_WEDGE_TO_VERTEX : return QString("Convert PerWedge UV into PerVertex UV");
		case FP_BASIC_TRIANGLE_MAPPING : return QString("Basic Tringle Mapping");
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
		case FP_UV_WEDGE_TO_VERTEX : return QString("Converts per Wedge Texture Coordinates to per Vertex Texture Coordinates splitting vertices with not coherent Wedge coordinates.");
		case FP_BASIC_TRIANGLE_MAPPING : return QString("Builds a basic parametrization");
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
		case FP_BASIC_TRIANGLE_MAPPING : return MeshFilterInterface::FP_Face;
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
		case FP_BASIC_TRIANGLE_MAPPING :
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
		case FP_BASIC_TRIANGLE_MAPPING : return MeshModel::MM_WEDGTEXCOORD;
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
		case FP_UVTOCOLOR : return FilterClass(MeshFilterInterface::VertexColoring + MeshFilterInterface::Texture);
		case FP_UV_WEDGE_TO_VERTEX : 
		case FP_BASIC_TRIANGLE_MAPPING : return MeshFilterInterface::Texture;
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
			parlst.addParam(new RichEnum("colorspace", 0, QStringList("Red-Green") << "Hue-Saturation", "Color Space", "The color space used to mapping UV to"));
			break;
		case FP_UV_WEDGE_TO_VERTEX : 
			break;
		case FP_BASIC_TRIANGLE_MAPPING :
			parlst.addParam(new RichInt("sidedim", 0, "Quads per line", "Indicates how many triangles have to be put on each line (every quad contains two triangles)"));
			parlst.addParam(new RichInt("textdim", 1024, "Texture Dimension (px)", "Gives an indication on how big the texture is"));
			parlst.addParam(new RichInt("border", 1, "Inter-Triangle border (px)", "Specifies how many pixels to be left between triangles in parametrization domain"));
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

/////// FUNCTIONS NEEDED BY "BASIC PARAMETRIZATION" FILTER
inline int getLongestEdge(const CMeshO::FaceType & f)
{
	int res=0;
	const CMeshO::CoordType &p0=f.cP(0), &p1=f.cP(1), p2=f.cP(2);
	double  maxd01 = SquaredDistance(p0,p1);
    double  maxd12 = SquaredDistance(p1,p2);
    double  maxd20 = SquaredDistance(p2,p0);
    if(maxd01 > maxd12)
        if(maxd01 > maxd20)     res = 0;
        else                    res = 2;
		else
			if(maxd12 > maxd20)     res = 1;
			else                    res = 2;
	return res;
}
///////////////////////////////////////////////////////


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
					// 'Normalized' 0..1 
					float normU, normV;
					normU = v.T().U() - (int)v.T().U();
					if (normU < 0.) normU += 1.;
					normV = v.T().V() - (int)v.T().V();
					if (normV < 0.) normV += 1.;
					
					switch(colsp) {
						// Red-Green color space
						case 0 : v.C() = vcg::Color4b((int)floor((normU*255)+0.5), (int)floor((normV*255)+0.5), 0, 255); break;
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
		
		case FP_BASIC_TRIANGLE_MAPPING : {
			if (!m.hasDataMask(MeshModel::MM_WEDGTEXCOORD))
				m.updateDataMask(MeshModel::MM_WEDGTEXCOORD);
			
			static const float sqrt2 = sqrt(2.0);
			
			// Get Parameters
			int sideDim = par.getInt("sidedim");
			int textDim = par.getInt("textdim");
			int pxBorder = par.getInt("border");
			
			//Get total faces and total undeleted face
			int faceNo = m.cm.face.size();
			int faceNotD = 0;
			for (CMeshO::FaceIterator fi=m.cm.face.begin(); fi!=m.cm.face.end(); ++fi)
				if (!fi->IsD()) ++faceNotD;
			
			// Minimum side dimension to get correct halfsquared triangles
			int optimalDim = ceilf(sqrtf(faceNotD/2.));
			if (sideDim == 0) sideDim = optimalDim;
			else if (optimalDim > sideDim)
			{
				this->errorMessage = "Triangles per lines aren't enough to obtain a correct parametrization";
				return false;
			}
			
			//Calculating border size in UV space
			float border = ((float)pxBorder) / textDim;
			if (border < 0)
			{
				this->errorMessage = "Inter-Triangle border has an incorrect value";
				return false;
			} else if (border*(1.0+sqrt2)*sideDim > 1.0)
			{
				this->errorMessage = "Inter-Triangle border is too much";
				return false;
			}
			float bordersq2 = border / sqrt2;
			float halfborder = border / 2;
			
			bool odd = true;
			CFaceO::TexCoordType botl, topr;
			int idx=0;
			botl.V() = 1.;
			for (int i=0; i<sideDim && idx<faceNo; ++i)
			{
				topr.V() = botl.V();
				topr.U() = 0.;
				botl.V() = 1.0 - 1.0/sideDim*(i+1);
				for (int j=0; j<2*sideDim && idx<faceNo; ++idx)
				{
					if (!m.cm.face[idx].IsD())
					{
						int lEdge = getLongestEdge(m.cm.face[idx]);
						if (odd) {
							botl.U() = topr.U();
							topr.U() = 1.0/sideDim*(j/2+1);
							CFaceO::TexCoordType bl(botl.U()+halfborder, botl.V()+halfborder+bordersq2);
							CFaceO::TexCoordType tr(topr.U()-(halfborder+bordersq2), topr.V()-halfborder);
							m.cm.face[idx].WT(lEdge) = bl;
							m.cm.face[idx].WT((++lEdge)%3) = tr;
							m.cm.face[idx].WT((++lEdge)%3) = CFaceO::TexCoordType(bl.U(), tr.V());
						} else {
							CFaceO::TexCoordType bl(botl.U()+(halfborder+bordersq2), botl.V()+halfborder);
							CFaceO::TexCoordType tr(topr.U()-halfborder, topr.V()-(halfborder+bordersq2));
							m.cm.face[idx].WT(lEdge) = tr;
							m.cm.face[idx].WT((++lEdge)%3) = bl;
							m.cm.face[idx].WT((++lEdge)%3) = CFaceO::TexCoordType(tr.U(), bl.V());
						}
						cb(idx*100/faceNo, "Generating parametrization...");
						odd=!odd; ++j;
					}
				}
			}
		}
		break;
			
		default: assert(0);
	}
	
	return true;
}

Q_EXPORT_PLUGIN(FilterTexturePlugin)
