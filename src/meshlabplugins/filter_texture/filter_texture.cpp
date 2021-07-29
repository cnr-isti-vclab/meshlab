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
#include <vcg/math/base.h>

#include <Eigen/Sparse>

#include <float.h>
#include <stdlib.h>
#include "filter_texture.h"
#include "pushpull.h"
#include "rastering.h"
#include <vcg/complex/algorithms/update/texture.h>
#include<wrap/io_trimesh/export_ply.h>
#include <vcg/complex/algorithms/parametrization/voronoi_atlas.h>
#include <common/utilities/load_save.h>
#include <QStandardPaths>

using namespace vcg;

// ERROR CHECKING UTILITY
#define CheckError(x,y); if ((x)) {throw MLException((y));}
///////////////////////////////////////////////////////

FilterTexturePlugin::FilterTexturePlugin()
{
	typeList = {
		FP_VORONOI_ATLAS,
		FP_UV_WEDGE_TO_VERTEX,
		FP_UV_VERTEX_TO_WEDGE,
		FP_BASIC_TRIANGLE_MAPPING,
		FP_SET_TEXTURE,
		FP_PLANAR_MAPPING,
		FP_COLOR_TO_TEXTURE,
		FP_TRANSFER_TO_TEXTURE,
		FP_TEX_TO_VCOLOR_TRANSFER
	};
	
	for(ActionIDType tt: types())
		actionList.push_back(new QAction(filterName(tt), this));
}

QString FilterTexturePlugin::pluginName() const
{
	return "FilterTexture";
}

QString FilterTexturePlugin::filterName(ActionIDType filterId) const
{
	switch(filterId)
	{
	case FP_VORONOI_ATLAS : return QString("Parametrization: Voronoi Atlas");
	case FP_UV_WEDGE_TO_VERTEX : return QString("Convert PerWedge UV into PerVertex UV");
	case FP_UV_VERTEX_TO_WEDGE : return QString("Convert PerVertex UV into PerWedge UV");
	case FP_BASIC_TRIANGLE_MAPPING : return QString("Parametrization: Trivial Per-Triangle");
	case FP_PLANAR_MAPPING : return QString("Parametrization: Flat Plane");
	case FP_SET_TEXTURE : return QString("Set Texture");
	case FP_COLOR_TO_TEXTURE : return QString("Transfer: Vertex Color to Texture");
	case FP_TRANSFER_TO_TEXTURE : return QString("Transfer: Vertex Attributes to Texture (1 or 2 meshes)");
	case FP_TEX_TO_VCOLOR_TRANSFER : return QString("Transfer: Texture to Vertex Color (1 or 2 meshes)");
	default : assert(0);
	}
	return {};
}

// Info() must return the longer string describing each filtering action
// (this string is used in the About plugin dialog)
QString FilterTexturePlugin::filterInfo(ActionIDType filterId) const
{
	switch(filterId)
	{
	case FP_VORONOI_ATLAS :  return QString("Build an atlased parametrization based on a geodesic voronoi partitioning of the surface and parametrizing each region using Harmonic Mapping. For the  parametrization of the disk like voronoi regions the used method is: <br><b>Ulrich Pinkall, Konrad Polthier</b><br>\
											<i>Computing Discrete Minimal Surfaces and Their Conjugates</i> <br>\
											Experimental Mathematics, Vol 2 (1), 1993<br> .");
	case FP_UV_WEDGE_TO_VERTEX : return QString("Converts per Wedge Texture Coordinates to per Vertex Texture Coordinates splitting vertices with not coherent Wedge coordinates.");
	case FP_UV_VERTEX_TO_WEDGE : return QString("Converts per Vertex Texture Coordinates to per Wedge Texture Coordinates. It does not merge superfluous vertices...");
	case FP_BASIC_TRIANGLE_MAPPING : return QString("Builds a trivial triangle-by-triangle parametrization. <br> Two methods are provided, the first maps all triangles into equal sized triangles, while the second one adapt the size of the triangles in texture space to their original size.");
	case FP_PLANAR_MAPPING : return QString("Builds a trivial flat-plane parametrization.");
	case FP_SET_TEXTURE : return QString("Set a texture associated with current mesh parametrization.<br>" "If the texture provided exists, then it will be simply associated to the current mesh; else the filter will fail with no further actions.");
	case FP_COLOR_TO_TEXTURE : return QString("Fills the specified texture using per-vertex color data of the mesh.");
	case FP_TRANSFER_TO_TEXTURE : return QString("Transfer texture color, vertex color or normal from one mesh the texture of another mesh. This may be useful to restore detail lost in simplification, or resample a texture in a different parametrization.");
	case FP_TEX_TO_VCOLOR_TRANSFER : return QString("Generates Vertex Color values picking color from a texture (same mesh or another mesh).");
	default : assert(0);
	}
	return QString("Unknown Filter");
}
	
int FilterTexturePlugin::getPreConditions(const QAction *a) const
{
	switch (ID(a))
	{
	case FP_UV_WEDGE_TO_VERTEX : return MeshModel::MM_WEDGTEXCOORD;
	case FP_UV_VERTEX_TO_WEDGE : return MeshModel::MM_VERTTEXCOORD;
	case FP_BASIC_TRIANGLE_MAPPING :
	case FP_VORONOI_ATLAS :
	case FP_PLANAR_MAPPING : return MeshModel::MM_FACENUMBER;
	case FP_SET_TEXTURE : return MeshModel::MM_WEDGTEXCOORD;
	case FP_COLOR_TO_TEXTURE : return MeshModel::MM_VERTCOLOR | MeshModel::MM_WEDGTEXCOORD;
	case FP_TRANSFER_TO_TEXTURE : return MeshModel::MM_NONE;
	case FP_TEX_TO_VCOLOR_TRANSFER : return MeshModel::MM_NONE;
	default: assert(0);
	}
	return MeshModel::MM_NONE;
}
	
int FilterTexturePlugin::getRequirements(const QAction *a)
{
	switch (ID(a))
	{
	case FP_VORONOI_ATLAS :
	case FP_UV_WEDGE_TO_VERTEX :
	case FP_UV_VERTEX_TO_WEDGE :
	case FP_BASIC_TRIANGLE_MAPPING :
	case FP_PLANAR_MAPPING :
	case FP_SET_TEXTURE : return MeshModel::MM_NONE;
	case FP_COLOR_TO_TEXTURE : return MeshModel::MM_FACEFACETOPO;
	case FP_TRANSFER_TO_TEXTURE : return MeshModel::MM_NONE;
	case FP_TEX_TO_VCOLOR_TRANSFER : return MeshModel::MM_NONE;
	default: assert(0);
	}
	return MeshModel::MM_NONE;
}
	
int FilterTexturePlugin::postCondition(const QAction *a) const
{
	switch (ID(a))
	{
	case FP_VORONOI_ATLAS: return MeshModel::MM_WEDGTEXCOORD;
	case FP_UV_WEDGE_TO_VERTEX: return MeshModel::MM_VERTTEXCOORD;
	case FP_UV_VERTEX_TO_WEDGE : return MeshModel::MM_WEDGTEXCOORD;
	case FP_PLANAR_MAPPING : return MeshModel::MM_WEDGTEXCOORD;
	case FP_BASIC_TRIANGLE_MAPPING : return MeshModel::MM_WEDGTEXCOORD;
	case FP_SET_TEXTURE : return MeshModel::MM_NONE;
	case FP_COLOR_TO_TEXTURE : return MeshModel::MM_NONE;
	case FP_TRANSFER_TO_TEXTURE : return MeshModel::MM_NONE;
	case FP_TEX_TO_VCOLOR_TRANSFER: return MeshModel::MM_VERTCOLOR;
	default: assert(0);
	}
	return MeshModel::MM_NONE;
}
	
// The FilterClass describes in which generic class of filters it fits.
// This choice affect the submenu in which each filter will be placed
// More than a single class can be chosen.
FilterTexturePlugin::FilterClass FilterTexturePlugin::getClass(const QAction *a) const
{
	switch(ID(a))
	{
	case FP_VORONOI_ATLAS :
	case FP_UV_WEDGE_TO_VERTEX :
	case FP_UV_VERTEX_TO_WEDGE :
	case FP_BASIC_TRIANGLE_MAPPING :
	case FP_PLANAR_MAPPING :
	case FP_SET_TEXTURE :
	case FP_COLOR_TO_TEXTURE :
	case FP_TRANSFER_TO_TEXTURE : return FilterPlugin::Texture;
	case FP_TEX_TO_VCOLOR_TRANSFER : return FilterClass(FilterPlugin::VertexColoring + FilterPlugin::Texture);
	default : assert(0);
	}
	return FilterPlugin::Generic;
}
	
static QString extractFilenameTexture(const MeshModel* mm)
{
	if (mm->fullName().isEmpty()){
		return "texture.png";
	}
	QFileInfo fi(mm->fullName());
	QString fileName = fi.baseName().append("_tex.png");
	//fileName = fi.absolutePath() + "/" + fileName;
	return fileName;
}
	
// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parameter you need to define,
// - the name of the parameter,
// - the string shown in the dialog
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
RichParameterList FilterTexturePlugin::initParameterList(const QAction *action, const MeshDocument &md)
{
	RichParameterList parlst;
	const MeshModel* trg = md.mm();
	for (const MeshModel& tmp : md.meshIterator()){
		if (&tmp != trg){
			trg = &tmp;
			break;
		}
	}
	QString fileName = extractFilenameTexture(md.mm());
	QString trgFileName = extractFilenameTexture(trg);
	switch(ID(action)) {
	case FP_VORONOI_ATLAS :
		parlst.addParam(RichInt("regionNum", 10, "Approx. Region Num", "An estimation of the number of regions that must be generated. Smaller regions could lead to parametrizations with smaller distortion."));
		parlst.addParam(RichBool("overlapFlag", false, "Overlap", "If checked the resulting parametrization will be composed by <i>overlapping</i> regions, e.g. the resulting mesh will have duplicated faces: each region will have a ring of ovelapping duplicate faces that will ensure that border regions will be parametrized in the atlas twice. This is quite useful for building mipmap robust atlases"));
		break;
	case FP_UV_WEDGE_TO_VERTEX :
		break;
	case FP_PLANAR_MAPPING :
		parlst.addParam(RichEnum("projectionPlane", 0, QStringList("XY") << "XZ"<<"YZ","Projection plane","Choose the projection plane"));
		parlst.addParam(RichBool("aspectRatio", false, "Preserve Ratio", "If checked the resulting parametrization will preserve the original apsect ratio of the model otherwise it will fill up the whole 0..1 uv space"));
		parlst.addParam(RichFloat("sideGutter", 0.0, "Side Gutter", "Leave an empty space around the parametrization area of the specified size (in texture space); accepted range [0.0 - 0.5]."));
		break;
	case FP_BASIC_TRIANGLE_MAPPING :
		parlst.addParam(RichInt("sidedim", 0, "Quads per line", "Indicates how many triangles have to be put on each line (every quad contains two triangles)\nLeave 0 for automatic calculation"));
		parlst.addParam(RichInt("textdim", 1024, "Texture Dimension (px)", "Gives an indication on how big the texture is"));
		parlst.addParam(RichInt("border", 2, "Inter-Triangle border (px)", "Specifies how many pixels to be left between triangles in parametrization domain"));
		parlst.addParam(RichEnum("method", 1, QStringList("Basic") << "Space-optimizing", "Method", "Choose space optimizing to map smaller faces into smaller triangles in parametrizazion domain"));
		break;
	case FP_SET_TEXTURE :
		parlst.addParam(RichOpenFile("textName", fileName, QStringList{"*.png", "*.jpg", "*.jpeg", "*.dds"},"Texture file", "If the file exists it will be associated to the mesh else a dummy one will be created"));
		break;
	case FP_COLOR_TO_TEXTURE : {
		parlst.addParam(RichString("textName", fileName, "Texture file", "The texture file to be created"));
		parlst.addParam(RichInt("textW", 1024, "Texture width (px)", "The texture width"));
		parlst.addParam(RichInt("textH", 1024, "Texture height (px)", "The texture height"));
		parlst.addParam(RichBool("overwrite", false, "Overwrite texture", "if current mesh has a texture will be overwritten (with provided texture dimension)"));
		parlst.addParam(RichBool("pullpush", true, "Fill texture", "if enabled the unmapped texture space is colored using a pull push filling algorithm, if false is set to black"));
	}
		break;
	case FP_TRANSFER_TO_TEXTURE : {
		parlst.addParam(RichMesh ("sourceMesh",md.mm()->id(),&md, "Source Mesh",
								  "The mesh that contains the source data that we want to transfer"));
		parlst.addParam(RichMesh ("targetMesh",trg->id(),&md, "Target Mesh",
								  "The mesh whose texture will be filled according to source mesh data"));
		parlst.addParam(RichEnum("AttributeEnum", 0, QStringList("Vertex Color")  << "Vertex Normal" << "Vertex Quality"<< "Texture Color", "Color Data Source",
								 "Choose what attribute has to be transferred onto the target texture. You can choose between Per vertex attributes (color,normal,quality) or to transfer color information from source mesh texture"));
		parlst.addParam(RichAbsPerc("upperBound", md.mm()->cm.bbox.Diag()/50.0, 0.0f, md.mm()->cm.bbox.Diag(),
									tr("Max Dist Search"), tr("Sample points for which we do not find anything within this distance are rejected and not considered for recovering data")));
		parlst.addParam(RichString("textName", trgFileName, "Texture file", "The texture file to be created"));
		parlst.addParam(RichInt("textW", 1024, "Texture width (px)", "The texture width"));
		parlst.addParam(RichInt("textH", 1024, "Texture height (px)", "The texture height"));
		parlst.addParam(RichBool("overwrite", false, "Overwrite Target Mesh Texture", "if target mesh has a texture will be overwritten (with provided texture dimension)"));
		parlst.addParam(RichBool("pullpush", true, "Fill texture", "if enabled the unmapped texture space is colored using a pull push filling algorithm, if false is set to black"));
	}
		break;
	case FP_TEX_TO_VCOLOR_TRANSFER : {
		parlst.addParam(RichMesh ("sourceMesh",md.mm()->id(),&md, "Source Mesh",
								  "The mesh with associated texture that we want to sample from"));
		parlst.addParam(RichMesh ("targetMesh",trg->id(),&md, "Target Mesh",
								  "The mesh whose vertex color will be filled according to source mesh texture"));
		parlst.addParam(RichAbsPerc("upperBound", md.mm()->cm.bbox.Diag()/50.0, 0.0f, md.mm()->cm.bbox.Diag(),
									tr("Max Dist Search"), tr("Sample points for which we do not find anything within this distance are rejected and not considered for recovering color")));
	}
		break;
	default: break; // do not add any parameter for the other filters
	}
	return parlst;
}
	
	
/////// FUNCTIONS NEEDED BY "UV WEDGE TO VERTEX" FILTER
inline void ExtractVertex(const CMeshO & srcMesh, const CMeshO::FaceType & f, int whichWedge, const CMeshO & dstMesh, CMeshO::VertexType & v)
{
	(void)srcMesh;
	(void)dstMesh;
	// This is done to preserve every single perVertex property
	// perVextex Texture Coordinate is instead obtained from perWedge one.
	v.ImportData(*f.cV(whichWedge));
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
	
typedef Triangle2<CMeshO::FaceType::TexCoordType::ScalarType> Tri2;
	
inline void buildTrianglesCache(std::vector<Tri2> &arr, int maxLevels, float border, float quadSize, int idx=-1)
{
	assert(idx >= -1);
	Tri2 &t0 = arr[2*idx+2];
	Tri2 &t1 = arr[2*idx+3];
	if (idx == -1)
	{
		// build triangle 0
		t0.P(1).X() = quadSize - (0.5 + M_SQRT1_2)*border;
		t0.P(0).X() = 0.5 * border;
		t0.P(1).Y() = 1.0 - t0.P(0).X();
		t0.P(0).Y() = 1.0 - t0.P(1).X();
		t0.P(2).X() = t0.P(0).X();
		t0.P(2).Y() = t0.P(1).Y();
		// build triangle 1
		t1.P(1).X() = (0.5 + M_SQRT1_2)*border;
		t1.P(0).X() = quadSize - 0.5 * border;
		t1.P(1).Y() = 1.0 - t1.P(0).X();
		t1.P(0).Y() = 1.0 - t1.P(1).X();
		t1.P(2).X() = t1.P(0).X();
		t1.P(2).Y() = t1.P(1).Y();
	}
	else {
		// split triangle idx in t0 t1
		Tri2 &t = arr[idx];
		Tri2::CoordType midPoint = (t.P(0) + t.P(1)) / 2;
		Tri2::CoordType vec10 = (t.P(0) - t.P(1)).Normalize() * (border/2.0);
		t0.P(1) = t.P(0);
		t1.P(0) = t.P(1);
		t0.P(2) = midPoint + vec10;
		t1.P(2) = midPoint - vec10;
		t0.P(0) = t.P(2) + ( (t.P(0)-t.P(2)).Normalize() * border / M_SQRT2 );
		t1.P(1) = t.P(2) + ( (t.P(1)-t.P(2)).Normalize() * border / M_SQRT2 );
	}
	if (--maxLevels <= 0) return;
	buildTrianglesCache (arr, maxLevels, border, quadSize, 2*idx+2);
	buildTrianglesCache (arr, maxLevels, border, quadSize, 2*idx+3);
}

template<typename T>
T log_2(const T num)
{
	return T(log(num) / log(T(2)));
}

// The Real Core Function doing the actual mesh processing.
std::map<std::string, QVariant> FilterTexturePlugin::applyFilter(
			const QAction *filter,
			const RichParameterList &par,
			MeshDocument &md,
			unsigned int& /*postConditionMask*/,
			CallBackPos *cb)
{
	MeshModel &m=*(md.mm());
	switch(ID(filter))     {
	case FP_VORONOI_ATLAS : {
		MeshModel *baseModel=md.mm();
		baseModel->updateDataMask(MeshModel::MM_FACEFACETOPO);
		tri::UpdateTopology<CMeshO>::FaceFace(baseModel->cm);
		int nonManifVertNum=tri::Clean<CMeshO>::CountNonManifoldVertexFF(baseModel->cm,false);
		int nonManifEdgeNum=tri::Clean<CMeshO>::CountNonManifoldEdgeFF(baseModel->cm,false);
		
		if(nonManifVertNum>0 || nonManifEdgeNum>0)
		{
			log("Mesh is not manifold\n:%i non manifold Vertices\n%i nonmanifold Edges\n",nonManifVertNum,nonManifEdgeNum);
			throw MLException("Mesh is not manifold. See Log for details");
		}
		
		MeshModel *paraModel=md.addNewMesh("","VoroAtlas",false);
		tri::VoronoiAtlas<CMeshO>::VoronoiAtlasParam pp;
		pp.sampleNum =par.getInt("regionNum");
		pp.overlap=par.getBool("overlapFlag");
		
		paraModel->updateDataMask(MeshModel::MM_WEDGTEXCOORD);
		// Note that CMesh has ocf texcoord and the append inside VoronoiAtlas class need that.
		// This is a design bug of the VCGLib...
		int bitToBeCleared =0;
		if(!baseModel->hasDataMask(MeshModel::MM_WEDGTEXCOORD)) bitToBeCleared |=MeshModel::MM_WEDGTEXCOORD;
		if(!baseModel->hasDataMask(MeshModel::MM_VERTTEXCOORD)) bitToBeCleared |=MeshModel::MM_VERTTEXCOORD;
		baseModel->updateDataMask(MeshModel::MM_WEDGTEXCOORD | MeshModel::MM_VERTTEXCOORD);
		tri::VoronoiAtlas<CMeshO>::Build(baseModel->cm,paraModel->cm, pp);
		if(pp.overlap==false)
			tri::Clean<CMeshO>::RemoveDuplicateVertex(paraModel->cm);
		
		paraModel->updateBoxAndNormals();
		baseModel->clearDataMask(bitToBeCleared);
		log("Voronoi Atlas: Completed Processing in %i iterations",pp.vas.iterNum);
		log("Asked %i generated %i regions",pp.sampleNum,pp.vas.regionNum);
		log("Unwrap Time   %6.3f s", float(pp.vas.unwrapTime) / CLOCKS_PER_SEC);
		log("Voronoi Time  %6.3f s", float(pp.vas.voronoiTime) / CLOCKS_PER_SEC);
		log("Sampling Time %6.3f s", float(pp.vas.samplingTime) / CLOCKS_PER_SEC);
	}
		break;
		
	case FP_UV_WEDGE_TO_VERTEX : {
		int vn = m.cm.vn;
		m.updateDataMask(MeshModel::MM_VERTTEXCOORD);
		tri::AttributeSeam::SplitVertex(m.cm, ExtractVertex, CompareVertex);
		if (m.cm.vn != vn)
		{
			m.clearDataMask(MeshModel::MM_FACEFACETOPO);
			m.clearDataMask(MeshModel::MM_VERTFACETOPO);
		}
	}
		break;
		
	case FP_UV_VERTEX_TO_WEDGE : {
		m.updateDataMask(MeshModel::MM_WEDGTEXCOORD);
		tri::UpdateTexture<CMeshO>::WedgeTexFromVertexTex(m.cm);
	}
		break;
		
	case FP_PLANAR_MAPPING : 
	{
		m.updateDataMask(MeshModel::MM_WEDGTEXCOORD);
		
		// Get Parameters
		Point3m planeVec[3][2] = {
			{Point3m(1,0,0),Point3m(0,1,0)},  // XY
			{Point3m(0,0,1),Point3m(1,0,0)},  // XZ
			{Point3m(0,1,0),Point3m(0,0,1)}   // YZ
		};
		
		int sideDim = par.getEnum("projectionPlane");
		bool aspectRatio = par.getBool("aspectRatio");
		Scalarm sideGutter = par.getFloat("sideGutter");
		
		// the mesh has to be correctly transformed
		if (m.cm.Tr != Matrix44m::Identity())
			tri::UpdatePosition<CMeshO>::Matrix(m.cm, m.cm.Tr, true);
		
		tri::UpdateTexture<CMeshO>::WedgeTexFromPlane(m.cm, planeVec[sideDim][0], planeVec[sideDim][1], aspectRatio, sideGutter);
		
		// the mesh has to return to its original position
		if (m.cm.Tr != Matrix44m::Identity())
			tri::UpdatePosition<CMeshO>::Matrix(m.cm, Inverse(m.cm.Tr), true);
	}
		break;
		
	case FP_BASIC_TRIANGLE_MAPPING :
	{
		m.updateDataMask(MeshModel::MM_WEDGTEXCOORD);
		
		// Get Parameters
		int sideDim = par.getInt("sidedim");
		int textDim = par.getInt("textdim");
		int pxBorder = par.getInt("border");
		bool adv = false;
		switch(par.getEnum("method")) {
		case 0 : adv = false; break; // Basic
		case 1 : adv = true; break;  // Advanced
		default : assert(0);
		};
		
		// Pre checks
		CheckError(textDim <= 0, "Texture Dimension has an incorrect value");
		CheckError(pxBorder < 0,   "Inter-Triangle border has an incorrect value");
		CheckError(sideDim < 0,  "Quads per line border has an incorrect value");
		
		if (adv) //ADVANCED SPACE-OPTIMIZING
		{
			float border = ((float)pxBorder) / textDim;
			
			// Creates a vector of double areas
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
			int    buckSize = (int)ceil(log_2(maxArea/minArea) + DBL_EPSILON);
			std::vector<std::vector<uint> > buckets(buckSize);
			for (uint i=0; i<areas.size(); ++i)
				if (areas[i]>=0)
				{
					int slot = (int)ceil(log_2(maxArea/areas[i]) + DBL_EPSILON) - 1;
					assert(slot < buckSize && slot >= 0);
					buckets[slot].push_back(i);
				}
			
			// Determines correct dimension and accordingly max halfening levels
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
						tmp = t.P(0) + origin;
						m.cm.face[fidx].WT(lEdge) = CFaceO::TexCoordType(tmp.X(), tmp.Y());
						m.cm.face[fidx].WT(lEdge).N() = 0;
						lEdge = (lEdge+1)%3;
						tmp = t.P(1) + origin;
						m.cm.face[fidx].WT(lEdge) = CFaceO::TexCoordType(tmp.X(), tmp.Y());
						m.cm.face[fidx].WT(lEdge).N() = 0;
						lEdge = (lEdge+1)%3;
						tmp = t.P(2) + origin;
						m.cm.face[fidx].WT(lEdge) = CFaceO::TexCoordType(tmp.X(), tmp.Y());
						m.cm.face[fidx].WT(lEdge).N() = 0;
						++it;
						cb(face*100/faceNo, "Generating parametrization...");
					}
				}
			}
			assert(face == faceNo);
			assert(it == buckets[buckSize-1].end());
			log( "Biggest triangle's catheti are %.2f px long", (cache[0].P(0)-cache[0].P(2)).Norm() * textDim);
			log( "Smallest triangle's catheti are %.2f px long", (cache[cache.size()-1].P(0)-cache[cache.size()-1].P(2)).Norm() * textDim);
			
		}
		else //BASIC
		{
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
			log( "Triangles' catheti are %.2f px long", (1.0/sideDim-border-bordersq2)*textDim);
		}
	}
		break;
		
	case FP_SET_TEXTURE : {
		// Get parameters
		QString textName = par.getOpenFileName("textName");
		
		CheckError(textName.length() == 0, "Texture file not specified");
		
		QImage textFile = meshlab::loadImage(textName, &md.Log, cb);
		QFileInfo finfo(textName);
		//Assign texture
		m.clearTextures();
		m.addTexture(finfo.fileName().toStdString(), textFile);
	}
		break;
		
	case FP_COLOR_TO_TEXTURE : 
	{
		QString textName = par.getString("textName");
		int textW = par.getInt("textW");
		int textH = par.getInt("textH");
		bool overwrite = par.getBool("overwrite");
		bool pp = par.getBool("pullpush");

		CheckError(textW <= 0, "Texture Width has an incorrect value");
		CheckError(textH <= 0, "Texture Height has an incorrect value");
		if (overwrite)
		{
			CheckError(m.cm.textures.empty(), "Mesh has no associated texture to overwrite");
		}
		else
		{
			CheckError(textName.length() == 0, "Texture file not specified");
			//CheckError(std::max<int>(textName.lastIndexOf("\\"), textName.lastIndexOf("/")) != -1, "Path in Texture file not allowed");
		}

		if (m.cm.textures.empty())
		{
			QImage img(textW, textH, QImage::Format_RGB32);
			img.fill(qRgb(255, 255, 255)); // white
			m.addTexture(textName.toStdString(), img);

			//Assign texture
			for(auto fi=m.cm.face.begin();fi!=m.cm.face.end();++fi){
				if(!(*fi).IsD()) if((*fi).WT(0).N()==-1) {
					(*fi).WT(0).N() = 0;
					(*fi).WT(1).N() = 0;
					(*fi).WT(2).N() = 0;
				}
			}
		}

		QString baseName(textName);
		if (baseName.lastIndexOf(".") != -1)
			if (baseName.endsWith("bmp", Qt::CaseInsensitive) || baseName.endsWith("jpg", Qt::CaseInsensitive) || baseName.endsWith("png", Qt::CaseInsensitive)
					|| baseName.endsWith("jpeg", Qt::CaseInsensitive) || baseName.endsWith("tif", Qt::CaseInsensitive) || baseName.endsWith("tiff", Qt::CaseInsensitive))
				baseName.truncate(baseName.lastIndexOf("."));

		unsigned int texNum = m.cm.textures.size();
		vector <QString> texFileNames;
		texFileNames.resize(texNum);
		vector <QImage> trgImgs;
		trgImgs.reserve(texNum);

		// Image texture creation
		for (unsigned int texInd = 0; texInd < texNum; texInd++)
		{
			if (overwrite)
			{
				texFileNames[texInd] = QString(m.cm.textures[texInd].c_str());
			}
			else
			{
				if (texNum==1)
					texFileNames[texInd] = baseName + ".png";
				else
					texFileNames[texInd] = baseName + "_" + QString::number(texInd) + ".png";
			}

			trgImgs.push_back(QImage(QSize(textW, textH), QImage::Format_ARGB32));
			trgImgs[texInd].fill(qRgba(0, 0, 0, 0)); // transparent black
		}

		// Compute (texture-space) border edges
		tri::UpdateTopology<CMeshO>::FaceFaceFromTexCoord(m.cm);
		tri::UpdateFlags<CMeshO>::FaceBorderFromFF(m.cm);

		// Rasterizing triangles
		RasterSampler rs(trgImgs);
		rs.InitCallback(cb, m.cm.fn, 0, 80);
		tri::SurfaceSampling<CMeshO,RasterSampler>::Texture(m.cm,rs,textW,textH,true);

		// Undo topology changes
		tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
		tri::UpdateFlags<CMeshO>::FaceBorderFromFF(m.cm);

		for (unsigned int texInd = 0; texInd < texNum; texInd++)
		{
			// Revert alpha values for border edge pixels to 255
			cb(81, "Cleaning up texture ...");
			for (int y = 0; y<textH; ++y)
				for (int x = 0; x<textW; ++x)
				{
					QRgb px = trgImgs[texInd].pixel(x, y);
					if (qAlpha(px) < 255 && (!pp || qAlpha(px) > 0))
						trgImgs[texInd].setPixel(x, y, px | 0xff000000);
				}

			// PullPush
			if (pp)
			{
				cb(85, "Filling texture holes...");
				PullPush(trgImgs[texInd], qRgba(0, 0, 0, 0));
			}
		}

		if (overwrite) {
			for (unsigned int texInd = 0; texInd < texNum; texInd++)
				m.setTexture(texFileNames[texInd].toStdString(), trgImgs[texInd]);
		}
		else  {
			m.clearTextures();
			for (unsigned int texInd = 0; texInd < texNum; texInd++)
				m.addTexture(texFileNames[texInd].toStdString(), trgImgs[texInd]);
		}

		cb(100, "Done");
	}
	break;

	case FP_TRANSFER_TO_TEXTURE : 
		transferToTexture(md, par, cb);
		break;

	case FP_TEX_TO_VCOLOR_TRANSFER :
	{
		MeshModel *srcMesh = md.getMesh(par.getMeshId("sourceMesh"));
		MeshModel *trgMesh = md.getMesh(par.getMeshId("targetMesh"));
		Scalarm upperbound = par.getAbsPerc("upperBound"); // maximum distance to stop search
		
		assert(srcMesh!=NULL);
		assert(trgMesh!=NULL);
		
		CheckError(srcMesh->cm.fn == 0, "Source mesh requires to have faces");
		
		// Check whether is possible to access source mesh texture
		CheckError(!srcMesh->hasDataMask(MeshModel::MM_WEDGTEXCOORD), "Source mesh doesn't have Per Wedge Texture Coordinates");
		CheckError(srcMesh->cm.textures.empty(), "Source mesh doesn't have any associated texture");
		
		vector <QImage> srcImgs;
		srcImgs.resize(srcMesh->cm.textures.size());
		
		for (size_t textInd = 0; textInd < srcMesh->cm.textures.size(); textInd++)
		{
			srcImgs[textInd] = srcMesh->getTexture(srcMesh->cm.textures[textInd]);
		}
		
		trgMesh->updateDataMask(MeshModel::MM_VERTCOLOR);
		srcMesh->updateDataMask(MeshModel::MM_FACEMARK);
		
		// the meshes have to be transformed
		// only if source different from target (if single mesh, it does not matter)
		if (srcMesh != trgMesh)
		{
			if (srcMesh->cm.Tr != Matrix44m::Identity())
				tri::UpdatePosition<CMeshO>::Matrix(srcMesh->cm, srcMesh->cm.Tr, true);
			if (trgMesh->cm.Tr != Matrix44m::Identity())
				tri::UpdatePosition<CMeshO>::Matrix(trgMesh->cm, trgMesh->cm.Tr, true);
		}
		
		tri::UpdateNormal<CMeshO>::PerFaceNormalized(srcMesh->cm);
		// Colorizing vertices
		VertexSampler vs(srcMesh->cm, srcImgs, upperbound);
		vs.InitCallback(cb, trgMesh->cm.vn);
		tri::SurfaceSampling<CMeshO,VertexSampler>::VertexUniform(trgMesh->cm,vs,trgMesh->cm.vn);
		
		// the meshes have to return to their original position
		// only if source different from target (if single mesh, it does not matter)
		if (srcMesh != trgMesh)
		{
			if (srcMesh->cm.Tr != Matrix44m::Identity())
				tri::UpdatePosition<CMeshO>::Matrix(srcMesh->cm, Inverse(srcMesh->cm.Tr), true);
			if (trgMesh->cm.Tr != Matrix44m::Identity())
				tri::UpdatePosition<CMeshO>::Matrix(trgMesh->cm, Inverse(trgMesh->cm.Tr), true);
		}
		
		srcMesh->clearDataMask(MeshModel::MM_FACEMARK);
	}
		break;
		
	default:
		wrongActionCalled(filter);
	}
	
	return std::map<std::string, QVariant>();
}
	
FilterPlugin::FilterArity FilterTexturePlugin::filterArity(const QAction * filter ) const
{
	switch(ID(filter))
	{
	case FP_VORONOI_ATLAS : 
	case FP_UV_WEDGE_TO_VERTEX : 
	case FP_UV_VERTEX_TO_WEDGE : 
	case FP_BASIC_TRIANGLE_MAPPING : 
	case FP_PLANAR_MAPPING : 
	case FP_SET_TEXTURE : 
	case FP_COLOR_TO_TEXTURE : 
		return FilterPlugin::SINGLE_MESH;
	case FP_TRANSFER_TO_TEXTURE : 
	case FP_TEX_TO_VCOLOR_TRANSFER : 
		return FilterPlugin::FIXED;
	}
	return FilterPlugin::NONE;
}

void FilterTexturePlugin::transferToTexture(
			MeshDocument &md,
			const RichParameterList & par,
			vcg::CallBackPos * cb)
{
	MeshModel *srcMesh = md.getMesh(par.getMeshId("sourceMesh"));
	MeshModel *trgMesh = md.getMesh(par.getMeshId("targetMesh"));
	bool vertexSampling=false;
	bool textureSampling=false;
	int  vertexMode= -1;
	switch (par.getEnum("AttributeEnum"))
	{
		case 0: vertexSampling= true; vertexMode=0; break;  // Color
		case 1: vertexSampling= true; vertexMode=1; break;  // Normal
		case 2: vertexSampling= true; vertexMode=2; break;  // Quality
		case 3: textureSampling = true; break;
		default: assert(0);
	}
	Scalarm upperbound = par.getAbsPerc("upperBound"); // maximum distance to stop search
	QString textName = par.getString("textName");
	int textW = par.getInt("textW");
	int textH = par.getInt("textH");
	bool overwrite = par.getBool("overwrite");
	bool pp = par.getBool("pullpush");

	assert (srcMesh != NULL);
	assert (trgMesh != NULL);
	CheckError(trgMesh->cm.fn == 0, "Target mesh needs to have faces");
	CheckError(!trgMesh->hasDataMask(MeshModel::MM_WEDGTEXCOORD), "Target mesh does not have Per-Wedge Texture Coordinates");
	CheckError(textW <= 0, "Texture Width has an incorrect value");
	CheckError(textH <= 0, "Texture Height has an incorrect value");

	if (vertexSampling) {
		if (vertexMode == 0) { CheckError(!srcMesh->hasDataMask(MeshModel::MM_VERTCOLOR), "Source mesh doesn't have Per-Vertex Color"); }
		if (vertexMode == 1) { CheckError(!srcMesh->hasDataMask(MeshModel::MM_VERTNORMAL), "Source mesh doesn't have Per-Vertex Normal"); }
		if (vertexMode == 2) { CheckError(!srcMesh->hasDataMask(MeshModel::MM_VERTQUALITY), "Source mesh doesn't have Per-Vertex Quality"); }
	}
	else {
		CheckError(srcMesh->cm.fn == 0, "Source mesh needs to have faces");
		CheckError(!srcMesh->hasDataMask(MeshModel::MM_WEDGTEXCOORD), "Source mesh does not have Per-Wedge Texture Coordinates");
		CheckError(srcMesh->cm.textures.empty(), "Source mesh does not have any associated texture");
	}

	if (overwrite) {
		CheckError(trgMesh->cm.textures.empty(), "Mesh has no associated texture to overwrite");
	}
	else {
		CheckError(textName.length() == 0, "Texture file not specified");
	}

	// Source images (for texture to texture transfer)
	int numSrcTex = srcMesh->cm.textures.size();
	vector <QImage> srcImgs;
	vector <QString> srcTextureFileNames;
	srcImgs.resize(numSrcTex);
	srcTextureFileNames.resize(numSrcTex);
	int srcTexInd = 0;
	// Target images
	int numTrgTex = 1;
	if (trgMesh->cm.textures.size()> 0)
		numTrgTex = trgMesh->cm.textures.size();
	else { //I am creating the texture, need to set N() to WT
		for (auto& f : trgMesh->cm.face) {
			if (!f.IsD()) {
				f.WT(0).N() = 0;
				f.WT(1).N() = 0;
				f.WT(2).N() = 0;
			}
		}
	}

	QString baseName(textName);
	if (baseName.lastIndexOf(".") != -1){
		if (baseName.endsWith("bmp", Qt::CaseInsensitive) || baseName.endsWith("jpg", Qt::CaseInsensitive) || baseName.endsWith("png", Qt::CaseInsensitive)
				|| baseName.endsWith("jpeg", Qt::CaseInsensitive) || baseName.endsWith("tif", Qt::CaseInsensitive) || baseName.endsWith("tiff", Qt::CaseInsensitive))
			baseName.truncate(baseName.lastIndexOf("."));
	}
	vector <QImage> trgImgs;
	vector <QString> trgTextureFileNames;
	trgImgs.reserve(numTrgTex);
	trgTextureFileNames.resize(numTrgTex);
	int trgTexInd = 0;

	if (textureSampling)
	{
		for (srcTexInd = 0; srcTexInd < numSrcTex; srcTexInd++)
		{
			srcTextureFileNames[srcTexInd] = srcMesh->cm.textures[srcTexInd].c_str();
		}
	}

	// target textures naming and creation
	for (trgTexInd = 0; trgTexInd < numTrgTex; trgTexInd++)
	{
		if (overwrite)
		{
			trgTextureFileNames[trgTexInd] = QString(trgMesh->cm.textures[trgTexInd].c_str());
		}
		else
		{
			if (numTrgTex == 1)
				trgTextureFileNames[trgTexInd] = baseName + ".png";
			else
				trgTextureFileNames[trgTexInd] = baseName + "_" + QString::number(trgTexInd) + ".png";
		}

		trgImgs.push_back(QImage(QSize(textW, textH), QImage::Format_ARGB32));
		trgImgs[trgTexInd].fill(qRgba(0, 0, 0, 0)); // transparent black
	}

	// Compute (texture-space) border edges
	trgMesh->updateDataMask(MeshModel::MM_FACEFACETOPO);
	tri::UpdateTopology<CMeshO>::FaceFaceFromTexCoord(trgMesh->cm);
	tri::UpdateFlags<CMeshO>::FaceBorderFromFF(trgMesh->cm);

	// the meshes have to be transformed
	// only if source different from target (if single mesh, it does not matter)
	if (srcMesh != trgMesh)
	{
		if (srcMesh->cm.Tr != Matrix44m::Identity())
			tri::UpdatePosition<CMeshO>::Matrix(srcMesh->cm, srcMesh->cm.Tr, true);
		if (trgMesh->cm.Tr != Matrix44m::Identity())
			tri::UpdatePosition<CMeshO>::Matrix(trgMesh->cm, trgMesh->cm.Tr, true);
	}

	// Rasterizing faces
	srcMesh->updateDataMask(MeshModel::MM_FACEMARK);
	tri::UpdateNormal<CMeshO>::PerFaceNormalized(srcMesh->cm);
	if (vertexSampling)
	{
		TransferColorSampler sampler(srcMesh->cm, trgImgs, upperbound, vertexMode); // color sampling
		sampler.InitCallback(cb, trgMesh->cm.fn, 0, 80);
		tri::SurfaceSampling<CMeshO, TransferColorSampler>::Texture(trgMesh->cm, sampler, textW, textH, false);
	}
	else
	{
		TransferColorSampler sampler(srcMesh->cm, trgImgs, &srcImgs, upperbound); // texture sampling
		sampler.InitCallback(cb, trgMesh->cm.fn, 0, 80);
		tri::SurfaceSampling<CMeshO, TransferColorSampler>::Texture(trgMesh->cm, sampler, textW, textH, false);
	}

	// the meshes have to return to their original position
	// only if source different from target (if single mesh, it does not matter)
	if (srcMesh != trgMesh)
	{
		if (srcMesh->cm.Tr != Matrix44m::Identity())
			tri::UpdatePosition<CMeshO>::Matrix(srcMesh->cm, Inverse(srcMesh->cm.Tr), true);
		if (trgMesh->cm.Tr != Matrix44m::Identity())
			tri::UpdatePosition<CMeshO>::Matrix(trgMesh->cm, Inverse(trgMesh->cm.Tr), true);
	}

	// Undo topology changes
	tri::UpdateTopology<CMeshO>::FaceFace(trgMesh->cm);
	tri::UpdateFlags<CMeshO>::FaceBorderFromFF(trgMesh->cm);

	for (trgTexInd = 0; trgTexInd < numTrgTex; trgTexInd++)
	{
		// Revert alpha values for border edge pixels to 255
		cb(81, "Cleaning up texture ...");
		for (int y = 0; y<textH; ++y)
			for (int x = 0; x<textW; ++x)
			{
				QRgb px = trgImgs[trgTexInd].pixel(x, y);
				if (qAlpha(px) < 255 && (!pp || qAlpha(px) > 0))
					trgImgs[trgTexInd].setPixel(x, y, px | 0xff000000);
			}

		// PullPush
		if (pp)
		{
			cb(85, "Filling texture holes...");
			PullPush(trgImgs[trgTexInd], qRgba(0, 0, 0, 0));
		}
	}

	if (overwrite){
		for (trgTexInd = 0; trgTexInd < numTrgTex; trgTexInd++)
			trgMesh->setTexture(trgTextureFileNames[trgTexInd].toStdString(), trgImgs[trgTexInd]);
	}
	else{
		trgMesh->clearTextures();
		for (trgTexInd = 0; trgTexInd < numTrgTex; trgTexInd++)
			trgMesh->addTexture(trgTextureFileNames[trgTexInd].toStdString(), trgImgs[trgTexInd]);
	}

	cb(100, "Done");
}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterTexturePlugin)

