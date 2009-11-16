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
#include <float.h>
#include <stdlib.h>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/attribute_seam.h>
#include <vcg/space/triangle2.h>

#include "filter_texture.h"

FilterTexturePlugin::FilterTexturePlugin() 
{ 
	typeList << FP_UV_TO_COLOR
			<< FP_UV_WEDGE_TO_VERTEX
			<< FP_BASIC_TRIANGLE_MAPPING
			<< FP_SET_TEXTURE;
  
	foreach(FilterIDType tt , types())
		actionList << new QAction(filterName(tt), this);
}

const QString FilterTexturePlugin::filterName(FilterIDType filterId) const 
{
	switch(filterId) {
		case FP_UV_TO_COLOR : return QString("UV to Color"); 
		case FP_UV_WEDGE_TO_VERTEX : return QString("Convert PerWedge UV into PerVertex UV");
		case FP_BASIC_TRIANGLE_MAPPING : return QString("Basic Triangle Mapping");
		case FP_SET_TEXTURE : return QString("Set Texture");
		default : assert(0); 
	}
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
const QString FilterTexturePlugin::filterInfo(FilterIDType filterId) const
{
	switch(filterId)
	{
		case FP_UV_TO_COLOR :  return QString("Maps the UV Space into a color space, thus colorizing mesh vertices according to UV coords.");
		case FP_UV_WEDGE_TO_VERTEX : return QString("Converts per Wedge Texture Coordinates to per Vertex Texture Coordinates splitting vertices with not coherent Wedge coordinates.");
		case FP_BASIC_TRIANGLE_MAPPING : return QString("Builds a basic parametrization");
		case FP_SET_TEXTURE : return QString("Set a texture associated with current mesh parametrization.<br>"
											 "If the texture provided exists it will be simply set else a dummy one will be created in the same directory");
		default : assert(0); 
	}
	return QString("Unknown Filter");
}

int FilterTexturePlugin::getPreConditions(QAction *a) const
{
	switch (ID(a))
	{
		case FP_UV_TO_COLOR : return MeshFilterInterface::FP_VertexTexCoord;
		case FP_UV_WEDGE_TO_VERTEX : return MeshFilterInterface::FP_WedgeTexCoord;
		case FP_BASIC_TRIANGLE_MAPPING : return MeshFilterInterface::FP_Face;
		case FP_SET_TEXTURE : return MeshFilterInterface::FP_WedgeTexCoord;
		default: assert(0);
	}
	return MeshFilterInterface::FP_Generic;
}

const int FilterTexturePlugin::getRequirements(QAction *a)
{
	switch (ID(a))
	{
		case FP_UV_TO_COLOR :
		case FP_UV_WEDGE_TO_VERTEX :
		case FP_BASIC_TRIANGLE_MAPPING :
		case FP_SET_TEXTURE :
			return MeshModel::MM_NONE;
		default: assert(0);	
	}
	return MeshModel::MM_NONE;
}

int FilterTexturePlugin::postCondition( QAction *a) const
{
	switch (ID(a))
	{
		case FP_UV_TO_COLOR : return MeshModel::MM_VERTCOLOR;
		case FP_UV_WEDGE_TO_VERTEX : return MeshModel::MM_UNKNOWN;
		case FP_BASIC_TRIANGLE_MAPPING : return MeshModel::MM_WEDGTEXCOORD;
		case FP_SET_TEXTURE : return MeshModel::MM_UNKNOWN;
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
		case FP_UV_TO_COLOR : return FilterClass(MeshFilterInterface::VertexColoring + MeshFilterInterface::Texture);
		case FP_UV_WEDGE_TO_VERTEX : 
		case FP_BASIC_TRIANGLE_MAPPING :
		case FP_SET_TEXTURE : return MeshFilterInterface::Texture;
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
		case FP_UV_TO_COLOR :
			parlst.addParam(new RichEnum("colorspace", 0, QStringList("Red-Green") << "Hue-Saturation", "Color Space", "The color space used to mapping UV to"));
			break;
		case FP_UV_WEDGE_TO_VERTEX : 
			break;
		case FP_BASIC_TRIANGLE_MAPPING :
			parlst.addParam(new RichInt("sidedim", 0, "Quads per line", "Indicates how many triangles have to be put on each line (every quad contains two triangles)\nLeave 0 for automatic calculation"));
			parlst.addParam(new RichInt("textdim", 1024, "Texture Dimension (px)", "Gives an indication on how big the texture is"));
			parlst.addParam(new RichInt("border", 1, "Inter-Triangle border (px)", "Specifies how many pixels to be left between triangles in parametrization domain"));
			parlst.addParam(new RichEnum("method", 0, QStringList("Basic") << "Space-optimizing", "Method", "Choose space optimizing to map smaller faces into smaller triangles in parametrizazion domain"));
			break;
		case FP_SET_TEXTURE : {
			QString fileName(m.fileName.c_str());
			int lastPoint = fileName.lastIndexOf(".");
			if (lastPoint <= 0)
				fileName = QString("");
			else {
				fileName = fileName.left(lastPoint);
				lastPoint = std::max<int>(fileName.lastIndexOf('\\'),fileName.lastIndexOf('/'));
				if (lastPoint > 0)
					fileName = fileName.right(fileName.size() - 1 - lastPoint);
			}
			fileName = fileName.append(".png");
			parlst.addParam(new RichString("textName", fileName, "Texture file", "If the file exists it will be associated to the mesh else a dummy one will be created"));
			parlst.addParam(new RichInt("textDim", 1024, "Texture Dimension (px)", "If the named texture doesn't exists the dummy one will be squared with this side"));
			}
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

typedef vcg::Triangle2<CMeshO::FaceType::TexCoordType::ScalarType> Tri2;

inline void buildTrianglesCache(std::vector<Tri2> &arr, int maxLevels, float border, float quadSize, int idx=-1)
{
	assert(idx >= -1);
	Tri2 &t0 = arr[2*idx+2];
	Tri2 &t1 = arr[2*idx+3];
	if (idx == -1)
	{
		// build triangle 0
		t0.P(0).X() = quadSize - (0.5 + M_SQRT1_2)*border;
		t0.P(1).X() = 0.5 * border;
		t0.P(0).Y() = 1.0 - t0.P(1).X();
		t0.P(1).Y() = 1.0 - t0.P(0).X();
		t0.P(2).X() = t0.P(1).X();
		t0.P(2).Y() = t0.P(0).Y();
		// build triangle 1
		t1.P(0).X() = (0.5 + M_SQRT1_2)*border;
		t1.P(1).X() = quadSize - 0.5 * border;
		t1.P(0).Y() = 1.0 - t1.P(1).X();
		t1.P(1).Y() = 1.0 - t1.P(0).X();
		t1.P(2).X() = t1.P(1).X();
		t1.P(2).Y() = t1.P(0).Y();
	}
	else {
		// split triangle idx in t0 t1
		Tri2 &t = arr[idx];
		Tri2::CoordType midPoint = (t.P(0) + t.P(1)) / 2;
		Tri2::CoordType vec01 = (t.P(1) - t.P(0)).Normalize() * (border/2.0);
		t0.P(0) = t.P(1);
		t1.P(1) = t.P(0);
		t0.P(2) = midPoint + vec01;
		t1.P(2) = midPoint - vec01;
		t0.P(1) = t.P(2) + ( (t.P(1)-t.P(2)).Normalize() * border / M_SQRT2 );
		t1.P(0) = t.P(2) + ( (t.P(0)-t.P(2)).Normalize() * border / M_SQRT2 );
	}
	if (--maxLevels <= 0) return;
	buildTrianglesCache (arr, maxLevels, border, quadSize, 2*idx+2);
	buildTrianglesCache (arr, maxLevels, border, quadSize, 2*idx+3);
	
}

#define CheckError(x,y); if ((x)) {this->errorMessage = (y); return false;}
///////////////////////////////////////////////////////

template<typename T>
T log_2(const T num)
{
	return T(log(num) / log(T(2)));
}

// The Real Core Function doing the actual mesh processing.
bool FilterTexturePlugin::applyFilter(QAction *filter, MeshModel &m, RichParameterSet &par, vcg::CallBackPos *cb)
{
	switch(ID(filter))	 {
		case FP_UV_TO_COLOR : {
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
			
			// Get Parameters
			int sideDim = par.getInt("sidedim");
			int textDim = par.getInt("textdim");
			int pxBorder = par.getInt("border");
			bool adv;
			switch(par.getEnum("method")) {
				case 0 : adv = false; break; // Basic
				case 1 : adv = true; break;  // Advanced
				default : assert(0);
			};
			
			// Pre checks
			CheckError(textDim <= 0, "Texture Dimension has an incorrect value");
			CheckError(pxBorder < 0,   "Inter-Triangle border has an incorrect value");
			CheckError(sideDim < 0,  "Quads per line border has an incorrect value");
			
			if (adv)
			{ //ADVANCED SPACE-OPTIMIZING
				
				float border = ((float)pxBorder) / textDim;
				
				// Creates a vector of pair <face index, double area>
				double maxArea = -1, minArea=DBL_MAX;
				std::vector<double> areas;
				int faceNo = 0;
				for (uint i=0; i<m.cm.face.size(); ++i)
				{
					if (!m.cm.face[i].IsD())
					{
						double area = DoubleArea(m.cm.face[i]);
						if (area == 0) area = DBL_MIN;
						if (area > maxArea) maxArea = area;
						if (area < minArea) minArea = area;
						areas.push_back(area);
						++faceNo;
					} else {
						areas.push_back(-1.0);
					}
				}
				
				// Creates buckets containing each halfening level triangles (a histogram)
				int	buckSize = (int)ceil(log_2(maxArea/minArea) + DBL_EPSILON);
				std::vector<std::vector<uint> > buckets(buckSize);
				for (uint i=0; i<areas.size(); ++i)
					if (areas[i]>=0)
					{
						int slot = (int)ceil(log_2(maxArea/areas[i]) + DBL_EPSILON) - 1;
						assert(slot < buckSize && slot >= 0);
						buckets[slot].push_back(i);
					}
				
				// Determinates correct dimension and accordingly max halfening levels
				int dim = 0;
				int halfeningLevels = 0;
				
				double qn = 0., divisor = 2.0;
				int rest = faceNo, oneFact = 1, sqrt2Fact = 1;
				bool enough = false;
				while (halfeningLevels < buckSize)
				{
					int tmp =(int)ceil(sqrt(qn + rest/divisor));
					bool newenough = true;
					if (sideDim != 0)
					{
						newenough = sideDim>=tmp;
						tmp = sideDim;
					}
						
					// this check triangles dimension limit too
					if (newenough && 1.0/tmp < (sqrt2Fact/M_SQRT2 + oneFact)*border +
						(oneFact != sqrt2Fact ? oneFact*M_SQRT2*2.0/textDim : oneFact*2.0/textDim)) break;
					
					enough = newenough;
					rest -= buckets[halfeningLevels].size();
					qn += buckets[halfeningLevels].size() / divisor;
					divisor *= 2.0;
					
					if (halfeningLevels%2)
						oneFact *= 2;
					else
						sqrt2Fact *= 2;
					
					dim = tmp;
					halfeningLevels++;
				}
				
				// Post checks
				CheckError(!enough && halfeningLevels==buckSize, QString("Quads per line aren't enough to obtain a correct parametrization\nTry setting at least ") + QString::number((int)ceil(sqrt(qn))));
				CheckError(halfeningLevels==0  || !enough, "Inter-Triangle border is too much");
								
				//Create cache of possible triangles (need only translation in correct position)
				std::vector<Tri2> cache((1 << (halfeningLevels+1))-2);
				buildTrianglesCache(cache, halfeningLevels, border, 1.0/dim);
				
				// Setting texture coordinates (finally)
				Tri2::CoordType origin;
				Tri2::CoordType tmp;
				int buckIdx=0, face=0;
				std::vector<uint>::iterator it = buckets[buckIdx].begin();
				int currLevel = 1;
				for (int i=0; i<dim && face<faceNo; ++i)
				{
					origin.Y() = -((float)i)/dim;
					for (int j=0; j<dim && face<faceNo; j++)
					{
						origin.X() = ((float)j)/dim;
						for (int pos=(1<<currLevel)-2; pos<(1<<(currLevel+1))-2 && face<faceNo; ++pos, ++face)
						{
							while (it == buckets[buckIdx].end()) {
								if (++buckIdx < halfeningLevels)
								{
									++currLevel;
									pos = 2*pos+2;
								}
								it = buckets[buckIdx].begin();
							}
							int fidx = *it;
							int lEdge = getLongestEdge(m.cm.face[fidx]);
							Tri2 &t = cache[pos];
							tmp = t.P0(lEdge) + origin;
							m.cm.face[fidx].WT(0) = CFaceO::TexCoordType(tmp.X(), tmp.Y());
							m.cm.face[fidx].WT(0).N() = 0;
							tmp = t.P1(lEdge) + origin;
							m.cm.face[fidx].WT(1) = CFaceO::TexCoordType(tmp.X(), tmp.Y());
							m.cm.face[fidx].WT(1).N() = 0;
							tmp = t.P2(lEdge) + origin;
							m.cm.face[fidx].WT(2) = CFaceO::TexCoordType(tmp.X(), tmp.Y());
							m.cm.face[fidx].WT(2).N() = 0;
							++it;
							cb(face*100/faceNo, "Generating parametrization...");
						}
					}
				}
				assert(face == faceNo);
				assert(it == buckets[buckSize-1].end());
				Log(GLLogStream::FILTER, "Biggest triangle's catheti are %.2f px long", (cache[0].P(0)-cache[0].P(2)).Norm() * textDim);
				Log(GLLogStream::FILTER, "Smallest triangle's catheti are %.2f px long", (cache[cache.size()-1].P(0)-cache[cache.size()-1].P(2)).Norm() * textDim);
				
			}
			else
			{ //BASIC
				
				//Get total faces and total undeleted face
				int faceNo = m.cm.face.size();
				int faceNotD = 0;
				for (CMeshO::FaceIterator fi=m.cm.face.begin(); fi!=m.cm.face.end(); ++fi)
					if (!fi->IsD()) ++faceNotD;
				
				// Minimum side dimension to get correct halfsquared triangles
				int optimalDim = ceilf(sqrtf(faceNotD/2.));
				if (sideDim == 0) sideDim = optimalDim;
				else {
					CheckError(optimalDim > sideDim, QString("Quads per line aren't enough to obtain a correct parametrization\nTry setting at least ") + QString::number(optimalDim));
				}
				
				//Calculating border size in UV space
				float border = ((float)pxBorder) / textDim;
				CheckError(border*(1.0+M_SQRT2)+2.0/textDim > 1.0/sideDim, "Inter-Triangle border is too much");
				
				float bordersq2 = border / M_SQRT2;
				float halfborder = border / 2;
				
				bool odd = true;
				CFaceO::TexCoordType botl, topr;
				int face=0;
				botl.V() = 1.;
				for (int i=0; i<sideDim && face<faceNo; ++i)
				{
					topr.V() = botl.V();
					topr.U() = 0.;
					botl.V() = 1.0 - 1.0/sideDim*(i+1);
					for (int j=0; j<2*sideDim && face<faceNo; ++face)
					{
						if (!m.cm.face[face].IsD())
						{
							int lEdge = getLongestEdge(m.cm.face[face]);
							if (odd) {
								botl.U() = topr.U();
								topr.U() = 1.0/sideDim*(j/2+1);
								CFaceO::TexCoordType bl(botl.U()+halfborder, botl.V()+halfborder+bordersq2);
								CFaceO::TexCoordType tr(topr.U()-(halfborder+bordersq2), topr.V()-halfborder);
								bl.N() = 0;
								tr.N() = 0;
								m.cm.face[face].WT(lEdge) = bl;
								m.cm.face[face].WT((++lEdge)%3) = tr;
								m.cm.face[face].WT((++lEdge)%3) = CFaceO::TexCoordType(bl.U(), tr.V());
								m.cm.face[face].WT(lEdge%3).N() = 0;
							} else {
								CFaceO::TexCoordType bl(botl.U()+(halfborder+bordersq2), botl.V()+halfborder);
								CFaceO::TexCoordType tr(topr.U()-halfborder, topr.V()-(halfborder+bordersq2));
								bl.N() = 0;
								tr.N() = 0;
								m.cm.face[face].WT(lEdge) = tr;
								m.cm.face[face].WT((++lEdge)%3) = bl;
								m.cm.face[face].WT((++lEdge)%3) = CFaceO::TexCoordType(tr.U(), bl.V());
								m.cm.face[face].WT(lEdge%3).N() = 0;
							}
							cb(face*100/faceNo, "Generating parametrization...");
							odd=!odd; ++j;
						}
					}
				}
				Log(GLLogStream::FILTER, "Triangles catheti are %.2f px long", (1.0/sideDim-border-bordersq2)*textDim);
			}
		}
			break;
		
		case FP_SET_TEXTURE : {
			
#define CHECKERDIM 64
			
			// Get parameters
			QString textName = par.getString("textName");
			int textDim = par.getInt("textDim");
			
			CheckError(!QFile(m.fileName.c_str()).exists(), "Save the file before setting a texture");
			CheckError(textDim <= 0, "Texture Dimension has an incorrect value");
			CheckError(textName.length() == 0, "Texture file not specified");
			
			// Check textName and eventually add .png ext
			CheckError(std::max<int>(textName.lastIndexOf("\\"),textName.lastIndexOf("/")) != -1, "Path in Texture file not allowed");
			if (!textName.endsWith(".png", Qt::CaseInsensitive))
				textName.append(".png");
			
			// Creates path to texture file;
			QString fileName(m.fileName.c_str());
			int lastPoint = std::max<int>(fileName.lastIndexOf('\\'),fileName.lastIndexOf('/'));
			if (lastPoint < 0)
				fileName = textName;
			else
				fileName = fileName.left(lastPoint).append(QDir::separator()).append(textName);
			
			QFile textFile(fileName);
			if (!textFile.exists())
			{
				//create dummy checkers texture image
				QImage img(textDim, textDim, QImage::Format_RGB32);
				img.fill(0xffffffff);
				QPainter p(&img);
				QBrush gray(Qt::gray);
				QRect rect(0,0,CHECKERDIM,CHECKERDIM);
				bool odd = true;
				for (int y=0; y<textDim; y+=CHECKERDIM, odd=!odd)
				{
					rect.moveTop(y);
					for (int x=odd?0:CHECKERDIM; x<textDim; x+=2*CHECKERDIM)
					{
						rect.moveLeft(x);
						p.fillRect(rect, gray);
					}
				}
				//save
				CheckError(!img.save(fileName, "PNG"), "Specified file cannot be saved");
				Log(GLLogStream::FILTER, "Dummy Texture \"%s\" Created ", fileName.toStdString().c_str());
				assert(textFile.exists());
			}
			//set
			m.cm.textures.clear();
			m.cm.textures.push_back(textName.toStdString());
		}
		break;
			
		default: assert(0);
	}
	
	return true;
}

Q_EXPORT_PLUGIN(FilterTexturePlugin)
