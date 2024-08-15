/*****************************************************************************
 * MeshLab                                                           o o     *
 * A versatile mesh processing toolbox                             o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2005-2021                                           \/)\/    *
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

#include "filter_func.h"
#include <random>
#include <vcg/complex/algorithms/create/platonic.h>
#include <vcg/complex/algorithms/create/marching_cubes.h>
#include <vcg/complex/algorithms/create/mc_trivial_walker.h>

#include "muParser.h"
#include "string_conversion.h"

#include <random>

using namespace mu;
using namespace vcg;

std::random_device randomDev;
std::default_random_engine rndEngine(randomDev());
//Function to generate a random double number in [0..1) interval
double ML_Rnd() { return std::generate_canonical<double, 24>(rndEngine); }
//Function to generate a random integer number in [0..a) interval
double ML_RandInt(const double a) { return std::floor(a*ML_Rnd()); }

//Add rnd() and randint() custom functions to a mu::Parser
void setCustomFunctions(mu::Parser& p)
{
	p.DefineFun("rnd", ML_Rnd);
	p.DefineFun("randInt", ML_RandInt);
}

// Constructor
FilterFunctionPlugin::FilterFunctionPlugin()
{
	typeList = {
		FF_VERT_SELECTION,
		FF_FACE_SELECTION,
		FF_GEOM_FUNC,
		FF_WEDGE_TEXTURE_FUNC,
		FF_VERT_TEXTURE_FUNC,
		FF_FACE_COLOR,
		FF_VERT_COLOR,
		FF_VERT_QUALITY,
		FF_VERT_NORMAL,
		FF_FACE_NORMAL,
		FF_FACE_QUALITY,
		FF_DEF_VERT_SCALAR_ATTRIB,
		FF_DEF_FACE_SCALAR_ATTRIB,
		FF_DEF_VERT_POINT_ATTRIB,
		FF_DEF_FACE_POINT_ATTRIB,
		FF_GRID,
		FF_ISOSURFACE,
		FF_REFINE};

	for (ActionIDType tt : types())
		actionList.push_back(new QAction(filterName(tt), this));
}

FilterFunctionPlugin::~FilterFunctionPlugin()
{
}

QString FilterFunctionPlugin::pluginName() const
{
	return "FilterFunc";
}

// short string describing each filtering action
QString FilterFunctionPlugin::filterName(ActionIDType filterId) const
{
	switch (filterId) {
	case FF_VERT_SELECTION: return QString("Conditional Vertex Selection");
	case FF_FACE_SELECTION: return QString("Conditional Face Selection");
	case FF_GEOM_FUNC: return QString("Per Vertex Geometric Function");
	case FF_FACE_COLOR: return QString("Per Face Color Function");
	case FF_FACE_QUALITY: return QString("Per Face Quality Function");
	case FF_VERT_COLOR: return QString("Per Vertex Color Function");
	case FF_VERT_QUALITY: return QString("Per Vertex Quality Function");
	case FF_VERT_TEXTURE_FUNC: return QString("Per Vertex Texture Function");
	case FF_WEDGE_TEXTURE_FUNC: return QString("Per Wedge Texture Function");
	case FF_VERT_NORMAL: return QString("Per Vertex Normal Function");
	case FF_FACE_NORMAL: return QString("Per Face Normal Function");
	case FF_DEF_VERT_SCALAR_ATTRIB: return QString("Define New Per Vertex Custom Scalar Attribute");
	case FF_DEF_FACE_SCALAR_ATTRIB: return QString("Define New Per Face Custom Scalar Attribute");
	case FF_DEF_VERT_POINT_ATTRIB: return QString("Define New Per Vertex Custom Point Attribute");
	case FF_DEF_FACE_POINT_ATTRIB: return QString("Define New Per Face Custom Point Attribute");
	case FF_GRID: return QString("Grid Generator");
	case FF_REFINE: return QString("Refine User-Defined");
	case FF_ISOSURFACE: return QString("Implicit Surface");

	default: assert(0); return QString();
	}
}

QString FilterFunctionPlugin::pythonFilterName(ActionIDType f) const
{
	switch (f) {
	case FF_VERT_SELECTION: return QString("compute_selection_by_condition_per_vertex");
	case FF_FACE_SELECTION: return QString("compute_selection_by_condition_per_face");
	case FF_GEOM_FUNC: return QString("compute_coord_by_function");
	case FF_FACE_COLOR: return QString("compute_color_by_function_per_face");
	case FF_FACE_QUALITY: return QString("compute_scalar_by_function_per_face");
	case FF_VERT_COLOR: return QString("compute_color_by_function_per_vertex");
	case FF_VERT_QUALITY: return QString("compute_scalar_by_function_per_vertex");
	case FF_VERT_TEXTURE_FUNC: return QString("compute_texcoord_by_function_per_vertex");
	case FF_WEDGE_TEXTURE_FUNC: return QString("compute_texcoord_by_function_per_wedge");
	case FF_VERT_NORMAL: return QString("compute_normal_by_function_per_vertex");
	case FF_FACE_NORMAL: return QString("compute_normal_by_function_per_face");
	case FF_DEF_VERT_SCALAR_ATTRIB: return QString("compute_new_custom_scalar_attribute_per_vertex");
	case FF_DEF_FACE_SCALAR_ATTRIB: return QString("compute_new_custom_scalar_attribute_per_face");
	case FF_DEF_VERT_POINT_ATTRIB: return QString("compute_new_custom_point_attribute_per_vertex");
	case FF_DEF_FACE_POINT_ATTRIB: return QString("compute_new_custom_point_attribute_per_face");
	case FF_GRID: return QString("create_grid");
	case FF_REFINE: return QString("meshing_refine_by_function");
	case FF_ISOSURFACE: return QString("create_implicit_surface");

	default: assert(0); return QString();
	}
}

const QString PossibleOperators(
	"<br>It's possible to use any of the predefined muparser built-in "
	"<a href='https://beltoforion.de/en/muparser/features.php#idDef2'>operators</a> and "
	"<a href='https://beltoforion.de/en/muparser/features.php#idDef1'>functions</a>, like: "
	"<b>&&</b> (logic and), <b>||</b> (logic or), <b>&lt;</b>, <b>&lt;=</b>, <b>></b>, <b>>=</b>, "
	"<b>!=</b> (not equal), <b>==</b> (equal), <b>_?_:_</b> (c/c++ ternary operator), and "
	"<b>rnd()</b> (random value in [0..1]), and these values:" );

const QString PerVertexAttributeString(
	"<ul><li>Per-vertex variables:<br>"
	"<b>x,y,z</b> (position), <b>nx,ny,nz</b> (normal), <b>r,g,b,a</b> (color), <b>q</b> "
	"(quality), <b>vi</b> (vertex index), <b>vtu,vtv,ti</b> (texture coords and texture "
	"index), <b>vsel</b> ( 1 if selected, 0 if not selected)."
	"</li><li>Bounding Box variables:<br>"
	"<b>xmin,ymin,zmin</b> (min coordinates), <b>xmax,ymax,zmax</b> (max coordinates), "
	"<b>xmid,ymid,zmid</b> (midpoint coordinates), <b>xdim,ydim,zdim</b> (dimensions), "
	"<b>bbdiag</b> (diagonal length)"
	"</li><li>User-defined attributes:<br>"
	"All user defined custom <i>vertex attributes</i> are available. "
	"Point3 attribute are available as 3 variables with _x, _y, _z appended to the attribute name."
	"</li></ul>");

const QString PerFaceAttributeString(
	"<ul><li>Per-face variables:<br>"
	"<b>fi</b> (face index), <b>fr,fg,fb,fa</b> (face color), <b>fq</b> (face quality), "
	"<b>fnx,fny,fnz</b> (face normal), <b>fsel</b> ( 1 if face is selected, 0 if not selected)."
	"</li><li>Per-vertex variables:<br>"
	"<b>x0,y0,z0</b> (first vertex position), <b>x1,y1,z1</b> (second vertex position),"
	"<b>x2,y2,z2</b> (third vertex position), "
	"<b>nx0,ny0,nz0 nx1,ny1,nz1 nx2,ny2,nz2</b> (vertex normals), <b>r0,g0,b0,a0 r1,g1,b1,a1 "
	"r2,g2,b2,a2</b> (vertex colors), <b>vi0, vi1, vi2</b> (vertex indices), "
	"<b>q0,q1,q2</b> (vertex quality), <b>wtu0,wtv0 wtu1,wtv1 wtu2,wtv2</b> (per-wedge texture coords), "
	"<b>ti</b> (face texture index), <b>vsel0,vsel1,vsel2</b> (1 if vertex is selected, 0 if not)."
	"</li><li>Bounding Box variables:<br>"
	"<b>xmin,ymin,zmin</b> (min coordinates), <b>xmax,ymax,zmax</b> (max coordinates), "
	"<b>xmid,ymid,zmid</b> (midpoint coordinates), <b>xdim,ydim,zdim</b> (dimensions), "
	"<b>bbdiag</b> (diagonal length)."
	"</li><li>User-defined attributes:<br>"
	"All user defined custom <i>face scalar attributes</i> are available. "
	"Point3 attribute are available as 3 variables with _x, _y, _z appended to the attribute name."
	"</li></ul>");


// long string describing each filtering action
QString FilterFunctionPlugin::filterInfo(ActionIDType filterId) const
{
	switch (filterId) {
	case FF_VERT_SELECTION:
		return tr("Boolean function using muparser lib to perform vertex selection over current "
				  "mesh.<br>") +
			   PossibleOperators + PerVertexAttributeString;

	case FF_FACE_SELECTION:
		return tr("Boolean function using muparser lib to perform faces selection over current "
				  "mesh.<br>") +
			   PossibleOperators + PerFaceAttributeString;

	case FF_GEOM_FUNC:
		return tr("Geometric function using muparser lib to generate new Coord<br>"
				  "You can change x,y,z for every vertex according to the function "
				  "specified.<br>") +
			   PerVertexAttributeString;

	case FF_FACE_COLOR:
		return tr("Color function using muparser lib to generate new RGBA color for every face<br>"
				  "Red, Green, Blue and Alpha channels may be defined specifying a function in "
				  "their respective fields.<br>") +
			   PerFaceAttributeString;

	case FF_VERT_COLOR:
		return tr("Color function using muparser lib to generate new RGBA color for every "
				  "vertex<br>"
				  "Red, Green, Blue and Alpha channels may be defined specifying a function in "
				  "their respective fields.<br>") +
			   PerVertexAttributeString;

	case FF_VERT_QUALITY:
		return tr("Quality function using muparser to generate new Quality for every vertex<br>") +
			   PerVertexAttributeString;

	case FF_VERT_TEXTURE_FUNC:
		return tr("Texture function using muparser to generate new texture coords for every "
				  "vertex<br>") +
			   PerVertexAttributeString;
		
	case FF_VERT_NORMAL:
		return tr("Normal function using muparser to generate new Normal for every vertex<br>") +
			   PerVertexAttributeString;
		
	case FF_FACE_NORMAL:
		return tr("Normal function using muparser to generate new Normal for every face<br>") +
			   PerFaceAttributeString;
		
	case FF_FACE_QUALITY:
		return tr("Quality function using muparser to generate new Quality for every face<br>"
				  "Insert three function each one for quality of the three vertex of a face<br>") +
			   PerFaceAttributeString;

	case FF_WEDGE_TEXTURE_FUNC:
		return tr("Texture function using muparser to generate new per wedge tex coords for every "
				  "face<br>"
				  "Insert six functions each u v for each one of the three vertex of a face<br>") +
			   PerFaceAttributeString;

	case FF_DEF_VERT_SCALAR_ATTRIB:
		return tr("Add a new Per-Vertex custom scalar attribute to current mesh and fill it with "
				  "the defined function.<br>"
				  "Attribute names must contain only letters, numbers and underscores.<br>"
				  "The name specified for the attribute can be used in other filter functions.<br>") +
			   PerVertexAttributeString;

	case FF_DEF_FACE_SCALAR_ATTRIB:
		return tr("Add a new Per-Face custom scalar attribute to current mesh and fill it with "
				  "the defined function.<br>"
				  "Attribute names must contain only letters, numbers and underscores.<br>"
			      "The name specified for the attribute can be used in other filter functions.<br>") +
			   PerFaceAttributeString;

	case FF_DEF_VERT_POINT_ATTRIB:
		return tr("Add a new Per-Vertex custom point attribute to current mesh and fill it with the "
				  "defined functions.<br>"
				  "Attribute names must contain only letters, numbers and underscores.<br>"
				  "The name specified for the attribute can be used in other filter functions.<br>") +
			   PerVertexAttributeString;

	case FF_DEF_FACE_POINT_ATTRIB:
		return tr("Add a new Per-Face custom point attribute to current mesh and fill it with the "
				  "defined functions.<br>"
				  "Attribute names must contain only letters, numbers and underscores.<br>"
				  "The name specified for the attribute can be used in other filter functions.<br>") +
			   PerFaceAttributeString;
	case FF_GRID:
		return tr(
			"Generate a new 2D Grid mesh with number of vertices on X and Y axis specified by user "
			"with absolute length/height.<br>"
			"It's possible to center Grid on origin.");

	case FF_ISOSURFACE:
		return tr(
			"Generate a new mesh that corresponds to the 0 valued isosurface defined by the scalar "
			"field generated by the given expression");

	case FF_REFINE:
		return tr("Refine current mesh with user defined parameters.<br>"
				  "Specify a Boolean Function needed to select which edges will be cut for "
				  "refinement purpose.<br>"
				  "Each edge is identified with first and second vertex.<br>"
				  "Arguments accepted are first and second vertex attributes:<br>") +
			   PossibleOperators + PerFaceAttributeString;

	default: assert(0);
	}
	return QString("filter not found!");
}

FilterFunctionPlugin::FilterClass FilterFunctionPlugin::getClass(const QAction* a) const
{
	switch (ID(a)) {
	case FF_FACE_SELECTION:
	case FF_VERT_SELECTION: return FilterPlugin::Selection;
	case FF_FACE_QUALITY: return FilterClass(Quality + FaceColoring);
	case FF_VERT_QUALITY: return FilterClass(Quality + VertexColoring);
	case FF_VERT_TEXTURE_FUNC: return FilterPlugin::Texture;
	case FF_VERT_COLOR: return FilterPlugin::VertexColoring;
	case FF_VERT_NORMAL: return FilterPlugin::Normal;
	case FF_FACE_NORMAL: return FilterPlugin::Normal;
	case FF_FACE_COLOR: return FilterPlugin::FaceColoring;
	case FF_WEDGE_TEXTURE_FUNC: return FilterPlugin::Texture;
	case FF_ISOSURFACE: return FilterPlugin::MeshCreation;
	case FF_GRID: return FilterPlugin::MeshCreation;
	case FF_REFINE: return FilterPlugin::Remeshing;
	case FF_GEOM_FUNC: return FilterPlugin::Smoothing;
	case FF_DEF_VERT_SCALAR_ATTRIB:
	case FF_DEF_FACE_SCALAR_ATTRIB:
	case FF_DEF_VERT_POINT_ATTRIB:
	case FF_DEF_FACE_POINT_ATTRIB:
		return FilterPlugin::Layer;

	default: return FilterPlugin::Generic;
	}
}

int FilterFunctionPlugin::postCondition(const QAction* action) const
{
	switch (ID(action)) {
	case FF_VERT_SELECTION:
	case FF_FACE_SELECTION: return MeshModel::MM_VERTFLAGSELECT | MeshModel::MM_FACEFLAGSELECT;
	case FF_FACE_COLOR: return MeshModel::MM_FACECOLOR;
	case FF_GEOM_FUNC:
		return MeshModel::MM_VERTCOORD + MeshModel::MM_VERTNORMAL + MeshModel::MM_FACENORMAL;
	case FF_VERT_COLOR: return MeshModel::MM_VERTCOLOR;
	case FF_VERT_NORMAL: return MeshModel::MM_VERTNORMAL;
	case FF_FACE_NORMAL: return MeshModel::MM_FACENORMAL;
	case FF_VERT_TEXTURE_FUNC: return MeshModel::MM_VERTTEXCOORD;
	case FF_WEDGE_TEXTURE_FUNC: return MeshModel::MM_WEDGTEXCOORD;
	case FF_VERT_QUALITY: return MeshModel::MM_VERTQUALITY + MeshModel::MM_VERTCOLOR;
	case FF_FACE_QUALITY: return MeshModel::MM_FACECOLOR + MeshModel::MM_FACEQUALITY;

	case FF_DEF_VERT_SCALAR_ATTRIB:
	case FF_DEF_FACE_SCALAR_ATTRIB:
	case FF_DEF_VERT_POINT_ATTRIB:
	case FF_DEF_FACE_POINT_ATTRIB:
		return MeshModel::MM_NONE; // none, because they do not change any existing data

	case FF_REFINE: return MeshModel::MM_ALL;

	case FF_GRID:
	case FF_ISOSURFACE:
		return MeshModel::MM_NONE; // none, because they create a new layer, without affecting old
								   // one
	}

	return MeshModel::MM_NONE;
}

int FilterFunctionPlugin::getRequirements(const QAction* action)
{
	switch (ID(action)) {
	case FF_VERT_SELECTION:
	case FF_GEOM_FUNC:
	case FF_VERT_COLOR:
	case FF_VERT_NORMAL:
	case FF_FACE_NORMAL:
	case FF_VERT_QUALITY:
	case FF_VERT_TEXTURE_FUNC:
	case FF_DEF_VERT_SCALAR_ATTRIB:
	case FF_DEF_FACE_SCALAR_ATTRIB:
	case FF_DEF_VERT_POINT_ATTRIB:
	case FF_DEF_FACE_POINT_ATTRIB:
	case FF_GRID:
	case FF_ISOSURFACE:
	case FF_FACE_SELECTION: return 0;
	case FF_WEDGE_TEXTURE_FUNC: return MeshModel::MM_WEDGTEXCOORD;
	case FF_FACE_QUALITY: return MeshModel::MM_FACECOLOR | MeshModel::MM_FACEQUALITY;
	case FF_FACE_COLOR: return MeshModel::MM_FACECOLOR;
	case FF_REFINE: return MeshModel::MM_FACEFACETOPO | MeshModel::MM_VERTMARK;
	default: assert(0);
	}
	return 0;
}

// This function define the needed parameters for each filter. Return true if the filter has some
// parameters it is called every time, so you can set the default value of parameters according to
// the mesh For each parameter you need to define,
// - the name of the parameter,
// - the string shown in the dialog
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the
// dialog)
RichParameterList FilterFunctionPlugin::initParameterList(const QAction* action, const MeshModel&)
{
	RichParameterList parlst;
	switch (ID(action)) {
	case FF_VERT_SELECTION:
		parlst.addParam(RichString(
			"condSelect",
			"(q < 0)",
			"boolean function",
			"type a boolean function that will be evaluated in order to select a subset of "
			"vertices<br>"
			"example: (y > 0) and (ny > 0)"));
		break;

	case FF_FACE_SELECTION:
		parlst.addParam(RichString(
			"condSelect",
			"(fi == 0)",
			"boolean function",
			"type a boolean function that will be evaluated in order to select a subset of "
			"faces<br>"));
		break;

	case FF_GEOM_FUNC:
		parlst.addParam(
			RichString("x", "x", "func x = ", "insert function to generate new coord for x"));
		parlst.addParam(
			RichString("y", "y", "func y = ", "insert function to generate new coord for y"));
		parlst.addParam(RichString(
			"z", "sin(x+y)", "func z = ", "insert function to generate new coord for z"));
		parlst.addParam(RichBool(
			"onselected",
			false,
			"only on selection",
			"if checked, only affects selected vertices"));
		break;
		
	case FF_VERT_NORMAL:
		parlst.addParam(RichString(
			"x", "-nx", "func nx = ", "insert function to generate new x for the normal"));
		parlst.addParam(RichString(
			"y", "-ny", "func ny = ", "insert function to generate new y for the normal"));
		parlst.addParam(RichString(
			"z", "-nz", "func nz = ", "insert function to generate new z for the normal"));
		parlst.addParam(RichBool(
			"onselected",
			false,
			"only on selection",
			"if checked, only affects selected vertices"));
		break;
		
	case FF_FACE_NORMAL:
		parlst.addParam(RichString(
			"x", "-fnx", "func nx = ", "insert function to generate new x for the normal"));
		parlst.addParam(RichString(
			"y", "-fny", "func ny = ", "insert function to generate new y for the normal"));
		parlst.addParam(RichString(
			"z", "-fnz", "func nz = ", "insert function to generate new z for the normal"));
		parlst.addParam(RichBool(
			"onselected",
			false,
			"only on selection",
			"if checked, only affects selected vertices"));
		break;
		
	case FF_VERT_COLOR:
		parlst.addParam(RichString(
			"x", "255", "func r = ", "function to generate Red component. Expected Range 0-255"));
		parlst.addParam(RichString(
			"y", "255", "func g = ", "function to generate Green component. Expected Range 0-255"));
		parlst.addParam(RichString(
			"z", "0", "func b = ", "function to generate Blue component. Expected Range 0-255"));
		parlst.addParam(RichString(
			"a",
			"255",
			"func alpha = ",
			"function to generate Alpha component. Expected Range 0-255"));
		parlst.addParam(RichBool(
			"onselected",
			false,
			"only on selection",
			"if checked, only affects selected vertices"));
		break;

	case FF_VERT_TEXTURE_FUNC:
		parlst.addParam(RichString(
			"u", "x", "func u = ", "function to generate u texture coord. Expected Range 0-1"));
		parlst.addParam(RichString(
			"v", "y", "func v = ", "function to generate v texture coord. Expected Range 0-1"));
		parlst.addParam(RichBool(
			"onselected",
			false,
			"only on selection",
			"if checked, only affects selected vertices"));
		break;

	case FF_VERT_QUALITY:
		parlst.addParam(RichString(
			"q", "vi", "func q = ", "function to generate new Quality for every vertex"));
		parlst.addParam(RichBool(
			"normalize",
			false,
			"normalize",
			"if checked normalize all quality values in range [0..1]"));
		parlst.addParam(RichBool(
			"map",
			false,
			"map into color",
			"if checked map quality generated values into per-vertex color"));
		parlst.addParam(RichBool(
			"onselected",
			false,
			"only on selection",
			"if checked, only affects selected vertices"));
		break;

	case FF_FACE_COLOR:
		parlst.addParam(RichString(
			"r", "255", "func r = ", "function to generate Red component. Expected Range 0-255"));
		parlst.addParam(RichString(
			"g", "0", "func g = ", "function to generate Green component. Expected Range 0-255"));
		parlst.addParam(RichString(
			"b", "255", "func b = ", "function to generate Blue component. Expected Range 0-255"));
		parlst.addParam(RichString(
			"a",
			"255",
			"func alpha = ",
			"function to generate Alpha component. Expected Range 0-255"));
		parlst.addParam(RichBool(
			"onselected", false, "only on selection", "if checked, only affects selected faces"));
		break;

	case FF_FACE_QUALITY:
		parlst.addParam(RichString(
			"q", "x0+y0+z0", "func q0 = ", "function to generate new Quality foreach face"));
		parlst.addParam(RichBool(
			"normalize",
			false,
			"normalize",
			"if checked normalize all quality values in range [0..1]"));
		parlst.addParam(RichBool(
			"map",
			false,
			"map into color",
			"if checked map quality generated values into per-vertex color"));
		parlst.addParam(RichBool(
			"onselected", false, "only on selection", "if checked, only affects selected faces"));
		break;

	case FF_WEDGE_TEXTURE_FUNC:
		parlst.addParam(RichString(
			"u0",
			"x0",
			"func u0 = ",
			"function to generate u texture coord. of wedge 0. Expected Range 0-1"));
		parlst.addParam(RichString(
			"v0",
			"y0",
			"func v0 = ",
			"function to generate v texture coord. of wedge 0. Expected Range 0-1"));
		parlst.addParam(RichString(
			"u1",
			"x1",
			"func u1 = ",
			"function to generate u texture coord. of wedge 1. Expected Range 0-1"));
		parlst.addParam(RichString(
			"v1",
			"y1",
			"func v1 = ",
			"function to generate v texture coord. of wedge 1. Expected Range 0-1"));
		parlst.addParam(RichString(
			"u2",
			"x2",
			"func u2 = ",
			"function to generate u texture coord. of wedge 2. Expected Range 0-1"));
		parlst.addParam(RichString(
			"v2",
			"y2",
			"func v2 = ",
			"function to generate v texture coord. of wedge 2. Expected Range 0-1"));
		parlst.addParam(RichBool(
			"onselected", false, "only on selection", "if checked, only affects selected faces"));
		break;

	case FF_DEF_VERT_SCALAR_ATTRIB:
		parlst.addParam(RichString(
			"name",
			"CustomAttrName",
			"Name",
			"the name of new attribute. you can access attribute in other filters through this "
			"name"));
		parlst.addParam(RichString(
			"expr",
			"x",
			"Scalar function =",
			"function to calculate custom scalar attribute value for each vertex"));
		break;

	case FF_DEF_FACE_SCALAR_ATTRIB:
		parlst.addParam(RichString(
			"name",
			"CustomAttrName",
			"Name",
			"the name of new attribute. you can access attribute in other filters through this "
			"name"));
		parlst.addParam(RichString(
			"expr",
			"fi",
			"Scalar function =",
			"function to calculate custom scalar attribute value for each face"));
		break;

	case FF_DEF_VERT_POINT_ATTRIB:
		parlst.addParam(RichString(
			"name",
			"CustomAttrName",
			"Name",
			"the name of new attribute. you can access attribute in other filters through this "
			"name"));
		parlst.addParam(RichString(
			"x_expr",
			"x",
			"x coord function =",
			"function to calculate custom x coord of the point attribute value for each vertex"));
		parlst.addParam(RichString(
			"y_expr",
			"y",
			"y coord function =",
			"function to calculate custom y coord of the point attribute value for each vertex"));
		parlst.addParam(RichString(
			"z_expr",
			"z",
			"z coord function =",
			"function to calculate custom z coord of the point attribute value for each vertex"));
		break;

	case FF_DEF_FACE_POINT_ATTRIB:
		parlst.addParam(RichString(
			"name",
			"CustomAttrName",
			"Name",
			"the name of new attribute. you can access attribute in other filters through this "
			"name"));
		parlst.addParam(RichString(
			"x_expr",
			"x0",
			"x coord function =",
			"function to calculate custom x coord of the point attribute value for each face"));
		parlst.addParam(RichString(
			"y_expr",
			"y0",
			"y coord function =",
			"function to calculate custom y coord of the point attribute value for each face"));
		parlst.addParam(RichString(
			"z_expr",
			"z0",
			"z coord function =",
			"function to calculate custom z coord of the point attribute value for each face"));
		break;

	case FF_GRID:
		parlst.addParam(RichInt(
			"numVertX", 10, "num vertices on x", "number of vertices on x. it must be positive"));
		parlst.addParam(RichInt(
			"numVertY", 10, "num vertices on y", "number of vertices on y. it must be positive"));
		parlst.addParam(RichFloat("absScaleX", 0.3f, "x scale", "absolute scale on x (float)"));
		parlst.addParam(RichFloat("absScaleY", 0.3f, "y scale", "absolute scale on y (float)"));
		parlst.addParam(RichBool(
			"center",
			false,
			"centered on origin",
			"center grid generated by filter on origin.<br>"
			"Grid is first generated and than moved into origin (using muparser lib to perform "
			"fast calc on every vertex)"));
		break;
	case FF_ISOSURFACE:
		parlst.addParam(RichFloat(
			"voxelSize",
			0.05f,
			"Size of Voxel",
			"Size of the voxel that is used by for the grid where the field is sampled. Smaller "
			"this value, higher precision, but higher processing times."));
		parlst.addParam(RichFloat("minX", -1.0f, "Min X", "Range where the field is sampled"));
		parlst.addParam(RichFloat("minY", -1.0f, "Min Y", "Range where the field is sampled"));
		parlst.addParam(RichFloat("minZ", -1.0f, "Min Z", "Range where the field is sampled"));
		parlst.addParam(RichFloat("maxX", 1.0f, "Max X", "Range where the field is sampled"));
		parlst.addParam(RichFloat("maxY", 1.0f, "Max Y", "Range where the field is sampled"));
		parlst.addParam(RichFloat("maxZ", 1.0f, "Max Z", "Range where the field is sampled"));
		parlst.addParam(RichString(
			"expr",
			"x*x+y*y+z*z-0.5",
			"Function =",
			"This expression is evaluated for each voxel of the grid. The surface passing through "
			"the zero valued points of this field is then extracted using marching cube."));

		break;

	case FF_REFINE:
		parlst.addParam(RichString(
			"condSelect",
			"(q0 >= 0 && q1 >= 0)",
			"boolean function",
			"type a boolean function that will be evaluated on every edge"));
		parlst.addParam(RichString(
			"x",
			"(x0+x1)/2",
			"x =",
			"function to generate x coord of new vertex in [x0,x1].<br>For example (x0+x1)/2"));
		parlst.addParam(RichString(
			"y",
			"(y0+y1)/2",
			"y =",
			"function to generate x coord of new vertex in [y0,y1].<br>For example (y0+y1)/2"));
		parlst.addParam(RichString(
			"z",
			"(z0+z1)/2",
			"z =",
			"function to generate x coord of new vertex in [z0,z1].<br>For example (z0+z1)/2"));
		break;

	default: break; // do not add any parameter for the other filters
	}
	return parlst;
}

// The Real Core Function doing the actual mesh processing.
std::map<std::string, QVariant> FilterFunctionPlugin::applyFilter(
	const QAction*           filter,
	const RichParameterList& par,
	MeshDocument&            md,
	unsigned int& /*postConditionMask*/,
	vcg::CallBackPos* cb)
{
	errorMsg = "";
	if (this->getClass(filter) == FilterPlugin::MeshCreation)
		md.addNewMesh("", this->filterName(ID(filter)));
	MeshModel& m = *(md.mm());
	Q_UNUSED(cb);

	//Set values to parser variables related to BBox
	const auto &bbox = m.cm.bbox;
	xmin = bbox.min.X();
	ymin = bbox.min.Y();
	zmin = bbox.min.Z();
	xmax = bbox.max.X();
	ymax = bbox.max.Y();
	zmax = bbox.max.Z();
	xdim = bbox.DimX();
	ydim = bbox.DimY();
	zdim = bbox.DimZ();
	bbdiag = bbox.Diag();
	auto bbCenter = bbox.Center();
	xmid = bbCenter.X();
	ymid = bbCenter.Y();
	zmid = bbCenter.Z();

	switch (ID(filter)) {
	case FF_VERT_SELECTION: {
		std::string expr  = par.getString("condSelect").toStdString();
		auto        wexpr = conversion::fromStringToWString(expr);

		// muparser initialization and explicitly define parser variables
		Parser p;

		setPerVertexVariables(p, m.cm);
		setCustomFunctions(p);

		// set expression inserted by user as string (required by muparser)
		p.SetExpr(wexpr);

		int    numvert = 0;
		time_t start   = clock();

		// every parser variables is related to vertex coord and attributes.
		CMeshO::VertexIterator vi;
		for (vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi)
			if (!(*vi).IsD()) {
				setAttributes(vi, m.cm);

				bool selected = false;

				// use parser to evaluate boolean function specified above
				// in case of fail, error dialog contains details of parser's error
				try {
					selected = p.Eval();
				}
				catch (Parser::exception_type& e) {
					throw MLException(conversion::fromWStringToString(e.GetMsg()).c_str());
				}

				// set vertex as selected or clear selection
				if (selected) {
					(*vi).SetS();
					numvert++;
				}
				else
					(*vi).ClearS();
			}

		// if succeeded log stream contains number of vertices and time elapsed
		log("selected %d vertices in %.2f sec.",
			numvert,
			(clock() - start) / (float) CLOCKS_PER_SEC);
	} break;

	case FF_FACE_SELECTION: {
		QString select = par.getString("condSelect");

		// muparser initialization and explicitly define parser variables
		Parser p;
		setPerFaceVariables(p, m.cm);
		setCustomFunctions(p);
		
		// set expression inserted by user as string (required by muparser)
		p.SetExpr(conversion::fromStringToWString(select.toStdString()));

		int    numface = 0;
		time_t start   = clock();

		// every parser variables is related to face attributes.
		CMeshO::FaceIterator fi;
		for (fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
			if (!(*fi).IsD()) {
				setAttributes(fi, m.cm);

				bool selected = false;

				// use parser to evaluate boolean function specified above
				// in case of fail, error dialog contains details of parser's error
				try {
					selected = p.Eval();
				}
				catch (Parser::exception_type& e) {
					throw MLException(conversion::fromWStringToString(e.GetMsg()).c_str());
				}

				// set face as selected or clear selection
				if (selected) {
					(*fi).SetS();
					numface++;
				}
				else
					(*fi).ClearS();
			}

		// if succeeded log stream contains number of vertices and time elapsed
		log("selected %d faces in %.2f sec.", numface, (clock() - start) / (float) CLOCKS_PER_SEC);

	} break;

	case FF_GEOM_FUNC:
	case FF_VERT_COLOR:
	case FF_VERT_NORMAL: {
		std::string func_x, func_y, func_z, func_a;
		// FF_VERT_COLOR : x = r, y = g, z = b
		// FF_VERT_NORMAL : x = r, y = g, z = b
		func_x = par.getString("x").toStdString();
		func_y = par.getString("y").toStdString();
		func_z = par.getString("z").toStdString();
		if (ID(filter) == FF_VERT_COLOR)
			func_a = par.getString("a").toStdString();

		bool onSelected = par.getBool("onselected");

		if (onSelected && m.cm.svn == 0 && m.cm.sfn == 0) // if no selection at all, fail
		{
			log("Cannot apply only on selection: there is no selection");
			throw MLException("Cannot apply only on selection: there is no selection");
		}
		if (onSelected &&
			(m.cm.svn == 0 &&
			 m.cm.sfn > 0)) // if no vert selected, but some faces selected, use their vertices
		{
			tri::UpdateSelection<CMeshO>::VertexClear(m.cm);
			tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(m.cm);
		}

		// muparser initialization and explicitly define parser variables
		// function for x,y and z must use different parser and variables
		Parser p1, p2, p3, p4;
		setPerVertexVariables(p1, m.cm);
		setPerVertexVariables(p2, m.cm);
		setPerVertexVariables(p3, m.cm);
		setPerVertexVariables(p4, m.cm);

		setCustomFunctions(p1);
		setCustomFunctions(p2);
		setCustomFunctions(p3);
		setCustomFunctions(p4);

		p1.SetExpr(conversion::fromStringToWString(func_x));
		p2.SetExpr(conversion::fromStringToWString(func_y));
		p3.SetExpr(conversion::fromStringToWString(func_z));
		p4.SetExpr(conversion::fromStringToWString(func_a));

		double  newx = 0, newy = 0, newz = 0, newa = 255;
		QString errorMsg = "";

		time_t start = clock();

		// every parser variables is related to vertex coord and attributes.
		CMeshO::VertexIterator vi;
		for (vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi)
			if (!(*vi).IsD())
				if ((!onSelected) || ((*vi).IsS())) {
					setAttributes(vi, m.cm);

					// every function is evaluated by different parser.
					// errorMessage dialog contains errors for func x, func y and func z
					try {
						newx = p1.Eval();
					}
					catch (Parser::exception_type& e) {
						showParserError("1st func : ", e);
					}
					try {
						newy = p2.Eval();
					}
					catch (Parser::exception_type& e) {
						showParserError("2nd func : ", e);
					}
					try {
						newz = p3.Eval();
					}
					catch (Parser::exception_type& e) {
						showParserError("3rd func : ", e);
					}
					if (ID(filter) == FF_VERT_COLOR) {
						try {
							newa = p4.Eval();
						}
						catch (Parser::exception_type& e) {
							showParserError("4th func : ", e);
						}
					}
					if (errorMsg != "")
						throw MLException(errorMsg);

					if (ID(filter) == FF_GEOM_FUNC) // set new vertex coord for this iteration
						(*vi).P() = Point3m(newx, newy, newz);
					if (ID(filter) == FF_VERT_NORMAL) // set new color for this iteration
						(*vi).N() = Point3m(newx, newy, newz);
					if (ID(filter) == FF_VERT_COLOR) // set new color for this iteration
					{
						(*vi).C() = Color4b(newx, newy, newz, newa);
						m.updateDataMask(MeshModel::MM_VERTCOLOR);
					}
				}

		if (ID(filter) == FF_GEOM_FUNC) {
			// update bounding box, normalize normals
			tri::UpdateNormal<CMeshO>::PerVertexNormalizedPerFace(m.cm);
			tri::UpdateNormal<CMeshO>::NormalizePerFace(m.cm);
			tri::UpdateBounding<CMeshO>::Box(m.cm);
		}

		// if succeeded log stream contains number of vertices processed and time elapsed
		log("%d vertices processed in %.2f sec.",
			m.cm.vn,
			(clock() - start) / (float) CLOCKS_PER_SEC);
	} break;

	case FF_VERT_QUALITY: {
		std::string func_q     = par.getString("q").toStdString();
		bool        onSelected = par.getBool("onselected");

		if (onSelected && m.cm.svn == 0 && m.cm.sfn == 0) // if no selection at all, fail
		{
			log("Cannot apply only on selection: there is no selection");
			throw MLException("Cannot apply only on selection: there is no selection");
		}
		if (onSelected &&
			(m.cm.svn == 0 &&
			 m.cm.sfn > 0)) // if no vert selected, but some faces selected, use their vertices
		{
			tri::UpdateSelection<CMeshO>::VertexClear(m.cm);
			tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(m.cm);
		}

		m.updateDataMask(MeshModel::MM_VERTQUALITY);

		// muparser initialization and define custom variables
		Parser p;
		setPerVertexVariables(p, m.cm);

		//Add rnd() and randInt() internal functions
		setCustomFunctions(p);

		// set expression to calc with parser
		p.SetExpr(conversion::fromStringToWString(func_q));

		// every parser variables is related to vertex coord and attributes.
		time_t                 start = clock();
		CMeshO::VertexIterator vi;
		for (vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi)
			if (!(*vi).IsD())
				if ((!onSelected) || ((*vi).IsS())) {
					setAttributes(vi, m.cm);

					// use parser to evaluate function specified above
					// in case of fail, errorMessage dialog contains details of parser's error
					try {
						(*vi).Q() = p.Eval();
					}
					catch (Parser::exception_type& e) {
						throw MLException(conversion::fromWStringToString(e.GetMsg()).c_str());
					}
				}

		// normalize quality with values in [0..1]
		if (par.getBool("normalize"))
			tri::UpdateQuality<CMeshO>::VertexNormalize(m.cm);

		// map quality into per-vertex color
		if (par.getBool("map")) {
			tri::UpdateColor<CMeshO>::PerVertexQualityRamp(m.cm);
			m.updateDataMask(MeshModel::MM_VERTCOLOR);
		}
		// if succeeded log stream contains number of vertices and time elapsed
		log("%d vertices processed in %.2f sec.",
			m.cm.vn,
			(clock() - start) / (float) CLOCKS_PER_SEC);
	} break;
	case FF_VERT_TEXTURE_FUNC: {
		std::string func_u     = par.getString("u").toStdString();
		std::string func_v     = par.getString("v").toStdString();
		bool        onSelected = par.getBool("onselected");

		if (onSelected && m.cm.svn == 0 && m.cm.sfn == 0) // if no selection at all, fail
		{
			log("Cannot apply only on selection: there is no selection");
			throw MLException("Cannot apply only on selection: there is no selection");
		}
		if (onSelected &&
			(m.cm.svn == 0 &&
			 m.cm.sfn > 0)) // if no vert selected, but some faces selected, use their vertices
		{
			tri::UpdateSelection<CMeshO>::VertexClear(m.cm);
			tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(m.cm);
		}

		m.updateDataMask(MeshModel::MM_VERTTEXCOORD);

		// muparser initialization and define custom variables
		Parser pu, pv;
		setPerVertexVariables(pu, m.cm);
		setPerVertexVariables(pv, m.cm);
		setCustomFunctions(pu);
		setCustomFunctions(pv);

		// set expression to calc with parser
#ifdef _UNICODE
		pu.SetExpr(conversion::fromStringToWString(func_u));
		pv.SetExpr(conversion::fromStringToWString(func_v));
#else
		pu.SetExpr(func_u);
		pv.SetExpr(func_v);
#endif

		// every parser variables is related to vertex coord and attributes.
		time_t                 start = clock();
		CMeshO::VertexIterator vi;
		for (vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi)
			if (!(*vi).IsD())
				if ((!onSelected) || ((*vi).IsS())) {
					setAttributes(vi, m.cm);

					// use parser to evaluate function specified above
					// in case of fail, errorMessage dialog contains details of parser's error
					try {
						(*vi).T().U() = pu.Eval();
						(*vi).T().V() = pv.Eval();
					}
					catch (Parser::exception_type& e) {
						throw MLException(conversion::fromWStringToString(e.GetMsg()).c_str());
					}
				}

		log("%d vertices processed in %.2f sec.",
			m.cm.vn,
			(clock() - start) / (float) CLOCKS_PER_SEC);
	} break;
	case FF_WEDGE_TEXTURE_FUNC: {
		std::string func_u0    = par.getString("u0").toStdString();
		std::string func_v0    = par.getString("v0").toStdString();
		std::string func_u1    = par.getString("u1").toStdString();
		std::string func_v1    = par.getString("v1").toStdString();
		std::string func_u2    = par.getString("u2").toStdString();
		std::string func_v2    = par.getString("v2").toStdString();
		bool        onSelected = par.getBool("onselected");

		if (onSelected && m.cm.sfn == 0) // if no selection, fail
		{
			log("Cannot apply only on selection: there is no selection");
			throw MLException("Cannot apply only on selection: there is no selection");
		}

		m.updateDataMask(MeshModel::MM_VERTTEXCOORD);

		// muparser initialization and define custom variables
		Parser pu0, pv0, pu1, pv1, pu2, pv2;
		setPerFaceVariables(pu0, m.cm);
		setPerFaceVariables(pv0, m.cm);
		setPerFaceVariables(pu1, m.cm);
		setPerFaceVariables(pv1, m.cm);
		setPerFaceVariables(pu2, m.cm);
		setPerFaceVariables(pv2, m.cm);
		setCustomFunctions(pu0);
		setCustomFunctions(pv0);
		setCustomFunctions(pu1);
		setCustomFunctions(pv1);
		setCustomFunctions(pu2);
		setCustomFunctions(pv2);

		// set expression to calc with parser
		pu0.SetExpr(conversion::fromStringToWString(func_u0));
		pv0.SetExpr(conversion::fromStringToWString(func_v0));
		pu1.SetExpr(conversion::fromStringToWString(func_u1));
		pv1.SetExpr(conversion::fromStringToWString(func_v1));
		pu2.SetExpr(conversion::fromStringToWString(func_u2));
		pv2.SetExpr(conversion::fromStringToWString(func_v2));

		// every parser variables is related to vertex coord and attributes.
		time_t start = clock();
		for (CMeshO::FaceIterator fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
			if (!(*fi).IsD())
				if ((!onSelected) || ((*fi).IsS())) {
					setAttributes(fi, m.cm);

					// use parser to evaluate function specified above
					// in case of fail, errorMessage dialog contains details of parser's error
					try {
						(*fi).WT(0).U() = pu0.Eval();
						(*fi).WT(0).V() = pv0.Eval();
						(*fi).WT(1).U() = pu1.Eval();
						(*fi).WT(1).V() = pv1.Eval();
						(*fi).WT(2).U() = pu2.Eval();
						(*fi).WT(2).V() = pv2.Eval();
					}
					catch (Parser::exception_type& e) {
						throw MLException(conversion::fromWStringToString(e.GetMsg()).c_str());
					}
				}

		log("%d faces processed in %.2f sec.", m.cm.fn, (clock() - start) / (float) CLOCKS_PER_SEC);
	} break;
		case FF_FACE_NORMAL: {
		std::string func_nx     = par.getString("x").toStdString();
		std::string func_ny     = par.getString("y").toStdString();
		std::string func_nz     = par.getString("z").toStdString();
		bool        onSelected = par.getBool("onselected");
		if (onSelected && m.cm.sfn == 0) // if no selection, fail
		{
			log("Cannot apply only on selection: there is no selection");
			throw MLException("Cannot apply only on selection: there is no selection");
		}
		m.updateDataMask(MeshModel::MM_FACENORMAL);
		Parser p_nx, p_ny, p_nz;
		
		setPerFaceVariables(p_nx, m.cm);
		setPerFaceVariables(p_ny, m.cm);
		setPerFaceVariables(p_nz, m.cm);
		setCustomFunctions(p_nx);
		setCustomFunctions(p_ny);
		setCustomFunctions(p_nz);
			
		p_nx.SetExpr(conversion::fromStringToWString(func_nx));
		p_ny.SetExpr(conversion::fromStringToWString(func_ny));
		p_nz.SetExpr(conversion::fromStringToWString(func_nz));
		
		
		double nx = 0, ny = 0, nz = 0;
		errorMsg = "";
		
		time_t start = clock();
		
		// every parser variables is related to face attributes.
		for (CMeshO::FaceIterator  fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
			if (!(*fi).IsD())
				if ((!onSelected) || ((*fi).IsS())) {
					setAttributes(fi, m.cm);
					
					// evaluate functions to generate new color
					// in case of fail, error dialog contains details of parser's error
					try {
						nx = p_nx.Eval();
					}
					catch (Parser::exception_type& e) {
						showParserError("func nx: ", e);
					}
					try {
						ny = p_ny.Eval();
					}
					catch (Parser::exception_type& e) {
						showParserError("func ny: ", e);
					}
					try {
						nz = p_nz.Eval();
					}
					catch (Parser::exception_type& e) {
						showParserError("func nz: ", e);
					}					
					
					if (errorMsg != "")
						throw MLException(errorMsg);
					
					// set new color for this iteration
					(*fi).N() = Point3m(nx,ny,nz);
				}
		
		// if succeeded log stream contains number of vertices processed and time elapsed
		log("%d faces processed in %.2f sec.", m.cm.fn, (clock() - start) / (float) CLOCKS_PER_SEC);
		
		
	} break;
	case FF_FACE_COLOR: {
		std::string func_r     = par.getString("r").toStdString();
		std::string func_g     = par.getString("g").toStdString();
		std::string func_b     = par.getString("b").toStdString();
		std::string func_a     = par.getString("a").toStdString();
		bool        onSelected = par.getBool("onselected");

		if (onSelected && m.cm.sfn == 0) // if no selection, fail
		{
			log("Cannot apply only on selection: there is no selection");
			throw MLException("Cannot apply only on selection: there is no selection");
		}

		m.updateDataMask(MeshModel::MM_FACECOLOR);

		// muparser initialization and explicitly define parser variables
		// every function must uses own parser and variables
		Parser p1, p2, p3, p4;

		setPerFaceVariables(p1, m.cm);
		setPerFaceVariables(p2, m.cm);
		setPerFaceVariables(p3, m.cm);
		setPerFaceVariables(p4, m.cm);
		setCustomFunctions(p1);
		setCustomFunctions(p2);
		setCustomFunctions(p3);
		setCustomFunctions(p4);

		p1.SetExpr(conversion::fromStringToWString(func_r));
		p2.SetExpr(conversion::fromStringToWString(func_g));
		p3.SetExpr(conversion::fromStringToWString(func_b));
		p4.SetExpr(conversion::fromStringToWString(func_a));

		// RGB is related to every face
		CMeshO::FaceIterator fi;
		double               newr = 0, newg = 0, newb = 0, newa = 255;
		errorMsg = "";

		time_t start = clock();

		// every parser variables is related to face attributes.
		for (fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
			if (!(*fi).IsD())
				if ((!onSelected) || ((*fi).IsS())) {
					setAttributes(fi, m.cm);

					// evaluate functions to generate new color
					// in case of fail, error dialog contains details of parser's error
					try {
						newr = p1.Eval();
					}
					catch (Parser::exception_type& e) {
						showParserError("func r: ", e);
					}
					try {
						newg = p2.Eval();
					}
					catch (Parser::exception_type& e) {
						showParserError("func g: ", e);
					}
					try {
						newb = p3.Eval();
					}
					catch (Parser::exception_type& e) {
						showParserError("func b: ", e);
					}
					try {
						newa = p4.Eval();
					}
					catch (Parser::exception_type& e) {
						showParserError("func a: ", e);
					}

					if (errorMsg != "")
						throw MLException(errorMsg);

					// set new color for this iteration
					(*fi).C() = Color4b(newr, newg, newb, newa);
				}

		// if succeeded log stream contains number of vertices processed and time elapsed
		log("%d faces processed in %.2f sec.", m.cm.fn, (clock() - start) / (float) CLOCKS_PER_SEC);

	} break;

	case FF_FACE_QUALITY: {
		std::string func_q     = par.getString("q").toStdString();
		bool        onSelected = par.getBool("onselected");

		if (onSelected && m.cm.sfn == 0) // if no selection, fail
		{
			log("Cannot apply only on selection: there is no selection");
			throw MLException("Cannot apply only on selection: there is no selection");
		}

		m.updateDataMask(MeshModel::MM_FACEQUALITY);

		// muparser initialization and define custom variables
		Parser pf;
		setPerFaceVariables(pf, m.cm);
		setCustomFunctions(pf);

		// set expression to calc with parser
		pf.SetExpr(conversion::fromStringToWString(func_q));

		time_t start = clock();
		errorMsg     = "";

		// every parser variables is related to face attributes.
		CMeshO::FaceIterator fi;
		for (fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
			if (!(*fi).IsD())
				if ((!onSelected) || ((*fi).IsS())) {
					setAttributes(fi, m.cm);

					// evaluate functions to generate new quality
					// in case of fail, error dialog contains details of parser's error
					try {
						(*fi).Q() = pf.Eval();
					}
					catch (Parser::exception_type& e) {
						showParserError("func q: ", e);
					}
					if (errorMsg != "")
						throw MLException(errorMsg);
				}

		// normalize quality with values in [0..1]
		if (par.getBool("normalize"))
			tri::UpdateQuality<CMeshO>::FaceNormalize(m.cm);

		// map quality into per-vertex color
		if (par.getBool("map")) {
			tri::UpdateColor<CMeshO>::PerFaceQualityRamp(m.cm);
			m.updateDataMask(MeshModel::MM_FACECOLOR);
		}

		// if succeeded log stream contains number of faces processed and time elapsed
		log("%d faces processed in %.2f sec.", m.cm.fn, (clock() - start) / (float) CLOCKS_PER_SEC);

	} break;

	case FF_DEF_VERT_SCALAR_ATTRIB: {
		std::string name = par.getString("name").toStdString();
		std::string expr = par.getString("expr").toStdString();
		checkAttributeName(name);

		// add per-vertex attribute with type float and name specified by user
		CMeshO::PerVertexAttributeHandle<Scalarm> h;
		if (tri::HasPerVertexAttribute(m.cm, name)) {
			h = tri::Allocator<CMeshO>::FindPerVertexAttribute<Scalarm>(m.cm, name);
			if (!tri::Allocator<CMeshO>::IsValidHandle<Scalarm>(m.cm, h)) {
				throw MLException("attribute already exists with a different type");
			}
		}
		else
			h = tri::Allocator<CMeshO>::AddPerVertexAttribute<Scalarm>(m.cm, name);

		Parser p;
		setPerVertexVariables(p, m.cm);
		setCustomFunctions(p);

		p.SetExpr(conversion::fromStringToWString(expr));

		time_t start = clock();

		// perform calculation of attribute's value with function specified by user
		CMeshO::VertexIterator vi;
		for (vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi)
			if (!(*vi).IsD()) {
				setAttributes(vi, m.cm);

				// add new user-defined attribute
				try {
					h[vi] = p.Eval();
				}
				catch (Parser::exception_type& e) {
					throw MLException(conversion::fromWStringToString(e.GetMsg()).c_str());
				}
			}

		// add string, double and handler to vector.
		// vectors keep tracks of new attributes and let muparser use explicit variables
		// it's possible to use custom attributes in other filters
		v_attrNames.push_back(name);
		v_attrValue.push_back(0);
		v_handlers.push_back(h);

		// if succeeded log stream contains number of vertices processed and time elapsed
		log("%d vertices processed in %.2f sec.",
			m.cm.vn,
			(clock() - start) / (float) CLOCKS_PER_SEC);

	} break;

	case FF_DEF_FACE_SCALAR_ATTRIB: {
		std::string name = par.getString("name").toStdString();
		std::string expr = par.getString("expr").toStdString();
		checkAttributeName(name);

		// add per-face attribute with type float and name specified by user
		// add per-vertex attribute with type float and name specified by user
		CMeshO::PerFaceAttributeHandle<Scalarm> h;
		if (tri::HasPerFaceAttribute(m.cm, name)) {
			h = tri::Allocator<CMeshO>::FindPerFaceAttribute<Scalarm>(m.cm, name);
			if (!tri::Allocator<CMeshO>::IsValidHandle<Scalarm>(m.cm, h)) {
				throw MLException("attribute already exists with a different type");
			}
		}
		else
			h = tri::Allocator<CMeshO>::AddPerFaceAttribute<Scalarm>(m.cm, name);
		Parser p;
		setPerFaceVariables(p, m.cm);
		setCustomFunctions(p);		
		p.SetExpr(conversion::fromStringToWString(expr));

		time_t start = clock();

		// every parser variables is related to face attributes.
		CMeshO::FaceIterator fi;
		for (fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
			if (!(*fi).IsD()) {
				setAttributes(fi, m.cm);

				// add new user-defined attribute
				try {
					h[fi] = p.Eval();
				}
				catch (Parser::exception_type& e) {
					throw MLException(conversion::fromWStringToString(e.GetMsg()).c_str());
				}
			}

		// if succeeded log stream contains number of vertices processed and time elapsed
		log("%d faces processed in %.2f sec.", m.cm.fn, (clock() - start) / (float) CLOCKS_PER_SEC);

	} break;

	case FF_DEF_VERT_POINT_ATTRIB: {
		std::string name = par.getString("name").toStdString();
		std::string x_expr = par.getString("x_expr").toStdString();
		std::string y_expr = par.getString("y_expr").toStdString();
		std::string z_expr = par.getString("z_expr").toStdString();
		checkAttributeName(name);

		// add per-vertex attribute with type float and name specified by user
		CMeshO::PerVertexAttributeHandle<Point3m> h;
		if (tri::HasPerVertexAttribute(m.cm, name)) {
			h = tri::Allocator<CMeshO>::FindPerVertexAttribute<Point3m>(m.cm, name);
			if (!tri::Allocator<CMeshO>::IsValidHandle<Point3m>(m.cm, h)) {
				throw MLException("attribute already exists with a different type");
			}
		}
		else
			h = tri::Allocator<CMeshO>::AddPerVertexAttribute<Point3m>(m.cm, name);

		Parser p_x, p_y, p_z;
		setPerVertexVariables(p_x, m.cm);
		setPerVertexVariables(p_y, m.cm);
		setPerVertexVariables(p_z, m.cm);
		setCustomFunctions(p_x);
		setCustomFunctions(p_y);
		setCustomFunctions(p_z);

		p_x.SetExpr(conversion::fromStringToWString(x_expr));
		p_y.SetExpr(conversion::fromStringToWString(y_expr));
		p_z.SetExpr(conversion::fromStringToWString(z_expr));

		time_t start = clock();

		// perform calculation of attribute's value with function specified by user
		CMeshO::VertexIterator vi;
		for (vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi)
			if (!(*vi).IsD()) {
				setAttributes(vi, m.cm);

				try {
					h[vi][0] = p_x.Eval();
					h[vi][1] = p_y.Eval();
					h[vi][2] = p_z.Eval();
				}
				catch (Parser::exception_type& e) {
					throw MLException(conversion::fromWStringToString(e.GetMsg()).c_str());
				}
			}

		// add string, double and handler to vector.
		// vectors keep tracks of new attributes and let muparser use explicit variables
		// it's possible to use custom attributes in other filters

		//v_attrNames.push_back(name);
		//v_attrValue.push_back(0);
		//v_handlers.push_back(h);

		// if succeeded log stream contains number of vertices processed and time elapsed
		log("%d vertices processed in %.2f sec.",
			m.cm.vn,
			(clock() - start) / (float) CLOCKS_PER_SEC);

	} break;

	case FF_DEF_FACE_POINT_ATTRIB: {
		std::string name = par.getString("name").toStdString();
		std::string x_expr = par.getString("x_expr").toStdString();
		std::string y_expr = par.getString("y_expr").toStdString();
		std::string z_expr = par.getString("z_expr").toStdString();
		checkAttributeName(name);

		// add per-face attribute with type float and name specified by user
		// add per-vertex attribute with type float and name specified by user
		CMeshO::PerFaceAttributeHandle<Point3m> h;
		if (tri::HasPerFaceAttribute(m.cm, name)) {
			h = tri::Allocator<CMeshO>::FindPerFaceAttribute<Point3m>(m.cm, name);
			if (!tri::Allocator<CMeshO>::IsValidHandle<Point3m>(m.cm, h)) {
				throw MLException("attribute already exists with a different type");
			}
		}
		else
			h = tri::Allocator<CMeshO>::AddPerFaceAttribute<Point3m>(m.cm, name);
		Parser p_x, p_y, p_z;
		setPerFaceVariables(p_x, m.cm);
		setPerFaceVariables(p_y, m.cm);
		setPerFaceVariables(p_z, m.cm);
		setCustomFunctions(p_x);
		setCustomFunctions(p_y);
		setCustomFunctions(p_z);
		p_x.SetExpr(conversion::fromStringToWString(x_expr));
		p_y.SetExpr(conversion::fromStringToWString(y_expr));
		p_z.SetExpr(conversion::fromStringToWString(z_expr));

		time_t start = clock();

		// every parser variables is related to face attributes.
		CMeshO::FaceIterator fi;
		for (fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
			if (!(*fi).IsD()) {
				setAttributes(fi, m.cm);

				// add new user-defined attribute
				try {
					h[fi][0] = p_x.Eval();
					h[fi][1] = p_y.Eval();
					h[fi][2] = p_z.Eval();
				}
				catch (Parser::exception_type& e) {
					throw MLException(conversion::fromWStringToString(e.GetMsg()).c_str());
				}
			}

		// if succeeded log stream contains number of vertices processed and time elapsed
		log("%d faces processed in %.2f sec.", m.cm.fn, (clock() - start) / (float) CLOCKS_PER_SEC);

	} break;

	case FF_GRID: {
		// obtain parameters to generate 2D Grid
		int     w  = par.getInt("numVertX");
		int     h  = par.getInt("numVertY");
		Scalarm wl = par.getFloat("absScaleX");
		Scalarm hl = par.getFloat("absScaleY");

		if (w <= 0 || h <= 0) {
			throw MLException("number of vertices must be positive");
		}

		// use Grid function to generate Grid
		tri::Grid<CMeshO>(m.cm, w, h, wl, hl);

		// if "centered on origin" is checked than move generated Grid in (0,0,0)
		if (par.getBool("center")) {
			// move x and y
			Scalarm halfw = Scalarm(w - 1) / 2;
			Scalarm halfh = Scalarm(h - 1) / 2;
			Scalarm wld   = wl / Scalarm(w);
			Scalarm hld   = hl / Scalarm(h);

			for (auto vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi) {
				(*vi).P()[0] = (*vi).P()[0] - (wld * halfw);
				(*vi).P()[1] = (*vi).P()[1] - (hld * halfh);
			}
		}
		// update bounding box, normals
		//    Matrix44m rot; rot.SetRotateDeg(180,Point3m(0,1,0));
		Matrix44m rot;
		rot.SetScale(-1, 1, -1);
		tri::UpdatePosition<CMeshO>::Matrix(m.cm, rot, false);
		m.updateBoxAndNormals();
	} break;
	case FF_ISOSURFACE: {
		SimpleVolume<SimpleVoxel<float>> volume;

		typedef vcg::tri::TrivialWalker<CMeshO, SimpleVolume<SimpleVoxel<float>>> MyWalker;
		typedef vcg::tri::MarchingCubes<CMeshO, MyWalker>                         MyMarchingCubes;
		MyWalker                                                                  walker;

		Box3f RangeBBox;
		RangeBBox.min[0] = par.getFloat("minX");
		RangeBBox.min[1] = par.getFloat("minY");
		RangeBBox.min[2] = par.getFloat("minZ");
		RangeBBox.max[0] = par.getFloat("maxX");
		RangeBBox.max[1] = par.getFloat("maxY");
		RangeBBox.max[2] = par.getFloat("maxZ");
		double  step     = par.getFloat("voxelSize");
		Point3i siz      = Point3i::Construct((RangeBBox.max - RangeBBox.min) * (1.0 / step));

		Parser p;
		double x, y, z;
		setCustomFunctions(p);
		
		p.DefineVar(conversion::fromStringToWString("x"), &x);
		p.DefineVar(conversion::fromStringToWString("y"), &y);
		p.DefineVar(conversion::fromStringToWString("z"), &z);
		std::string expr = par.getString("expr").toStdString();
		p.SetExpr(conversion::fromStringToWString(expr));
		log("Filling a Volume of %i %i %i", siz[0], siz[1], siz[2]);
		volume.Init(siz, RangeBBox);
		for (double i = 0; i < siz[0]; i++)
			for (double j = 0; j < siz[1]; j++)
				for (double k = 0; k < siz[2]; k++) {
					x = RangeBBox.min[0] + step * i;
					y = RangeBBox.min[1] + step * j;
					z = RangeBBox.min[2] + step * k;
					try {
						volume.Val(i, j, k) = p.Eval();
					}
					catch (Parser::exception_type& e) {
						throw MLException(conversion::fromWStringToString(e.GetMsg()).c_str());
					}
				}

		// MARCHING CUBES
		log("[MARCHING CUBES] Building mesh...");
		MyMarchingCubes mc(m.cm, walker);
		walker.BuildMesh<MyMarchingCubes>(m.cm, volume, mc, 0);
		//    Matrix44m tr; tr.SetIdentity(); tr.SetTranslate(rbb.min[0],rbb.min[1],rbb.min[2]);
		//    Matrix44m sc; sc.SetIdentity(); sc.SetScale(step,step,step);
		//    tr=tr*sc;

		//    tri::UpdatePosition<CMeshO>::Matrix(m.cm,tr);
		tri::UpdateNormal<CMeshO>::PerVertexNormalizedPerFace(m.cm);
		tri::UpdateBounding<CMeshO>::Box(m.cm); // updates bounding box

	} break;

	case FF_REFINE: {
		std::string condSelect = par.getString("condSelect").toStdString();

		std::string expr1 = par.getString("x").toStdString();
		std::string expr2 = par.getString("y").toStdString();
		std::string expr3 = par.getString("z").toStdString();

		bool        errorMidPoint = false;
		bool        errorEdgePred = false;
		std::string msg           = "";

		// check parsing errors while creating two func obj
		// display error message
		MidPointCustom<CMeshO> mid =
			MidPointCustom<CMeshO>(m.cm, expr1, expr2, expr3, errorMidPoint, msg);
		CustomEdge<CMeshO> edge = CustomEdge<CMeshO>(condSelect, errorEdgePred, msg);
		if (errorMidPoint || errorEdgePred) {
			throw MLException(msg.c_str());
		}

		// Refine current mesh.
		// Only edge specified with CustomEdge pred are selected
		//  and the new vertex is chosen with MidPointCustom created above
		vcg::tri::RefineE<CMeshO, MidPointCustom<CMeshO>, CustomEdge<CMeshO>>(
			m.cm, mid, edge, false, cb);
		m.updateBoxAndNormals();
		m.clearDataMask(MeshModel::MM_VERTMARK);
		// vcg::tri::UpdateNormal<CMeshO>::PerVertexNormalizedPerFace(m.cm);
	} break;

	default: wrongActionCalled(filter);
	}
	return std::map<std::string, QVariant>();
}

// display parsing error in dialog
void FilterFunctionPlugin::showParserError(const QString& s, Parser::exception_type& e)
{
	errorMsg += s;
	errorMsg += conversion::fromWStringToString(e.GetMsg()).c_str();
	errorMsg += "\n";
}

// set per-vertex attributes associated to parser variables
void FilterFunctionPlugin::setAttributes(CMeshO::VertexIterator& vi, CMeshO& m)
{
	x = (*vi).P()[0]; // coord x
	y = (*vi).P()[1]; // coord y
	z = (*vi).P()[2]; // coord z

	nx = (*vi).N()[0]; // normal coord x
	ny = (*vi).N()[1]; // normal coord y
	nz = (*vi).N()[2]; // normal coord z

	r = (*vi).C()[0]; // color R
	g = (*vi).C()[1]; // color G
	b = (*vi).C()[2]; // color B
	a = (*vi).C()[3]; // color ALPHA

	q = (*vi).Q(); // quality

	vsel = ((*vi).IsS()) ? 1.0 : 0.0; // selection

	v = vi - m.vert.begin(); // zero based index of current vertex

	if (tri::HasPerVertexTexCoord(m)) {
		vtu = (*vi).T().U();
		vtv = (*vi).T().V();
		ti  = (*vi).T().N();
	}
	else {
		vtu = vtv = ti = 0;
	}

	// if user-defined attributes exist (vector is not empty)
	//  set variables to explicit value obtained through attribute's handler
	for (int i = 0; i < (int) v_attrValue.size(); i++)
		v_attrValue[i] = v_handlers[i][vi];

	for (int i = 0; i < (int) v3_handlers.size(); i++) {
		v3_attrValue[i * 3 + 0] = v3_handlers[i][vi].X();
		v3_attrValue[i * 3 + 1] = v3_handlers[i][vi].Y();
		v3_attrValue[i * 3 + 2] = v3_handlers[i][vi].Z();
	}
}

// set per-face attributes associated to parser variables
void FilterFunctionPlugin::setAttributes(CMeshO::FaceIterator& fi, CMeshO& m)
{
	// set attributes for First vertex
	// coords, normal coords, quality
	x0 = (*fi).V(0)->P()[0];
	y0 = (*fi).V(0)->P()[1];
	z0 = (*fi).V(0)->P()[2];

	nx0 = (*fi).V(0)->N()[0];
	ny0 = (*fi).V(0)->N()[1];
	nz0 = (*fi).V(0)->N()[2];

	r0 = (*fi).V(0)->C()[0];
	g0 = (*fi).V(0)->C()[1];
	b0 = (*fi).V(0)->C()[2];
	a0 = (*fi).V(0)->C()[3];

	q0 = (*fi).V(0)->Q();

	// set attributes for Second vertex
	// coords, normal coords, quality
	x1 = (*fi).V(1)->P()[0];
	y1 = (*fi).V(1)->P()[1];
	z1 = (*fi).V(1)->P()[2];

	nx1 = (*fi).V(1)->N()[0];
	ny1 = (*fi).V(1)->N()[1];
	nz1 = (*fi).V(1)->N()[2];

	r1 = (*fi).V(1)->C()[0];
	g1 = (*fi).V(1)->C()[1];
	b1 = (*fi).V(1)->C()[2];
	a1 = (*fi).V(1)->C()[3];

	q1 = (*fi).V(1)->Q();

	// set attributes for Third vertex
	// coords, normal coords, quality
	x2 = (*fi).V(2)->P()[0];
	y2 = (*fi).V(2)->P()[1];
	z2 = (*fi).V(2)->P()[2];

	nx2 = (*fi).V(2)->N()[0];
	ny2 = (*fi).V(2)->N()[1];
	nz2 = (*fi).V(2)->N()[2];

	r2 = (*fi).V(2)->C()[0];
	g2 = (*fi).V(2)->C()[1];
	b2 = (*fi).V(2)->C()[2];
	a2 = (*fi).V(2)->C()[3];

	q2 = (*fi).V(2)->Q();

	if (HasPerFaceQuality(m))
		fq = (*fi).Q();
	else
		fq = 0;

	// set face color attributes
	if (HasPerFaceColor(m)) {
		fr = (*fi).C()[0];
		fg = (*fi).C()[1];
		fb = (*fi).C()[2];
		fa = (*fi).C()[3];
	}
	else {
		fr = fg = fb = fa = 255;
	}

	// face normal
	fnx = (*fi).N()[0];
	fny = (*fi).N()[1];
	fnz = (*fi).N()[2];

	// zero based index of face
	f = fi - m.face.begin();

	// zero based index of its vertices
	v0i = ((*fi).V(0) - &m.vert[0]);
	v1i = ((*fi).V(1) - &m.vert[0]);
	v2i = ((*fi).V(2) - &m.vert[0]);

	if (tri::HasPerWedgeTexCoord(m)) {
		wtu0 = (*fi).WT(0).U();
		wtv0 = (*fi).WT(0).V();
		wtu1 = (*fi).WT(1).U();
		wtv1 = (*fi).WT(1).V();
		wtu2 = (*fi).WT(2).U();
		wtv2 = (*fi).WT(2).V();
		ti   = (*fi).WT(0).N();
	}
	else {
		wtu0 = wtv0 = wtu1 = wtv1 = wtu2 = wtv2 = ti = 0;
	}

	// selection
	vsel0 = ((*fi).V(0)->IsS()) ? 1.0 : 0.0;
	vsel1 = ((*fi).V(1)->IsS()) ? 1.0 : 0.0;
	vsel2 = ((*fi).V(2)->IsS()) ? 1.0 : 0.0;
	fsel  = ((*fi).IsS()) ? 1.0 : 0.0;

	// if user-defined attributes exist (vector is not empty)
	//  set variables to explicit value obtained through attribute's handler
	for (int i = 0; i < (int) f_attrValue.size(); i++)
		f_attrValue[i] = f_handlers[i][fi];
	
	for (int i = 0; i < (int) f3_handlers.size(); i++) {
		f3_attrValue[i * 3 + 0] = f3_handlers[i][fi].X();
		f3_attrValue[i * 3 + 1] = f3_handlers[i][fi].Y();
		f3_attrValue[i * 3 + 2] = f3_handlers[i][fi].Z();
	}
	
}

 // Generate a random double in [0.0, 1.0] interval
double FilterFunctionPlugin::random()
{
	std::random_device rd;  // Seed for the random number engine
    std::mt19937 gen(rd()); // Mersenne Twister engine
    std::uniform_real_distribution<double> dis(0.0, 1.0);
    return dis(gen);  // Generate a random double in [0.0, 1.0]
}

// Function explicitly define parser variables to perform per-vertex filter action
// x, y, z for vertex coord, nx, ny, nz for normal coord, r, g ,b for color
// and q for quality
void FilterFunctionPlugin::setPerVertexVariables(Parser& p, CMeshO& m)
{
	p.DefineVar(conversion::fromStringToWString("x"), &x);
	p.DefineVar(conversion::fromStringToWString("y"), &y);
	p.DefineVar(conversion::fromStringToWString("z"), &z);
	p.DefineVar(conversion::fromStringToWString("nx"), &nx);
	p.DefineVar(conversion::fromStringToWString("ny"), &ny);
	p.DefineVar(conversion::fromStringToWString("nz"), &nz);
	p.DefineVar(conversion::fromStringToWString("r"), &r);
	p.DefineVar(conversion::fromStringToWString("g"), &g);
	p.DefineVar(conversion::fromStringToWString("b"), &b);
	p.DefineVar(conversion::fromStringToWString("a"), &a);
	p.DefineVar(conversion::fromStringToWString("q"), &q);
	p.DefineVar(conversion::fromStringToWString("vi"), &v);
	p.DefineVar(conversion::fromStringToWString("vtu"), &vtu);
	p.DefineVar(conversion::fromStringToWString("vtv"), &vtv);
	p.DefineVar(conversion::fromStringToWString("ti"), &ti);
	p.DefineVar(conversion::fromStringToWString("vsel"), &vsel);

    //Add tokens related to mesh bounding box
	p.DefineVar(conversion::fromStringToWString("xmin"), &xmin);
	p.DefineVar(conversion::fromStringToWString("ymin"), &ymin);
	p.DefineVar(conversion::fromStringToWString("zmin"), &zmin);
	p.DefineVar(conversion::fromStringToWString("xmax"), &xmax);
	p.DefineVar(conversion::fromStringToWString("ymax"), &ymax);
	p.DefineVar(conversion::fromStringToWString("zmax"), &zmax);
	p.DefineVar(conversion::fromStringToWString("bbdiag"), &bbdiag);
	p.DefineVar(conversion::fromStringToWString("xdim"), &xdim);
	p.DefineVar(conversion::fromStringToWString("ydim"), &ydim);
	p.DefineVar(conversion::fromStringToWString("zdim"), &zdim);
	p.DefineVar(conversion::fromStringToWString("xmid"), &xmid);
	p.DefineVar(conversion::fromStringToWString("ymid"), &ymid);
	p.DefineVar(conversion::fromStringToWString("zmid"), &zmid);

	//Add function rnd() 
	p.DefineFun("rnd", random);

	// define var for user-defined attributes (if any exists)
	// if vector is empty, code won't be executed
	v_handlers.clear();
	v_attrNames.clear();
	v_attrValue.clear();
	std::vector<std::string> AllVertexAttribName;
	tri::Allocator<CMeshO>::GetAllPerVertexAttribute<Scalarm>(m, AllVertexAttribName);
	v_attrValue.reserve(AllVertexAttribName.size());
	for (int i = 0; i < (int) AllVertexAttribName.size(); i++) {
		CMeshO::PerVertexAttributeHandle<Scalarm> hh =
			tri::Allocator<CMeshO>::GetPerVertexAttribute<Scalarm>(m, AllVertexAttribName[i]);
		v_handlers.push_back(hh);
		v_attrNames.push_back(AllVertexAttribName[i]);
		v_attrValue.push_back(0);
		p.DefineVar(conversion::fromStringToWString(v_attrNames.back()), &v_attrValue.back());
		qDebug("Adding custom per vertex float variable %s", v_attrNames.back().c_str());
	}
	AllVertexAttribName.clear();
	v3_handlers.clear();
	v3_attrNames.clear();
	v3_attrValue.clear();
	tri::Allocator<CMeshO>::GetAllPerVertexAttribute<Point3m>(m, AllVertexAttribName);
	v3_attrValue.reserve(AllVertexAttribName.size());
	for (int i = 0; i < (int) AllVertexAttribName.size(); i++) {
		CMeshO::PerVertexAttributeHandle<Point3m> hh3 =
			tri::Allocator<CMeshO>::GetPerVertexAttribute<Point3m>(m, AllVertexAttribName[i]);

		v3_handlers.push_back(hh3);

		v3_attrValue.push_back(0);
		v3_attrNames.push_back(AllVertexAttribName[i] + "_x");
		p.DefineVar(conversion::fromStringToWString(v3_attrNames.back()), &v3_attrValue.back());

		v3_attrValue.push_back(0);
		v3_attrNames.push_back(AllVertexAttribName[i] + "_y");
		p.DefineVar(conversion::fromStringToWString(v3_attrNames.back()), &v3_attrValue.back());

		v3_attrValue.push_back(0);
		v3_attrNames.push_back(AllVertexAttribName[i] + "_z");
		p.DefineVar(conversion::fromStringToWString(v3_attrNames.back()), &v3_attrValue.back());
		qDebug("Adding custom per vertex Point3f variable %s", v3_attrNames.back().c_str());
	}
}

// Function explicitly define parser variables to perform Per-Face filter action
void FilterFunctionPlugin::setPerFaceVariables(Parser& p, CMeshO& m)
{
	// coord of the three vertices within a face
	p.DefineVar(conversion::fromStringToWString("x0"), &x0);
	p.DefineVar(conversion::fromStringToWString("y0"), &y0);
	p.DefineVar(conversion::fromStringToWString("z0"), &z0);
	p.DefineVar(conversion::fromStringToWString("x1"), &x1);
	p.DefineVar(conversion::fromStringToWString("y1"), &y1);
	p.DefineVar(conversion::fromStringToWString("z1"), &z1);
	p.DefineVar(conversion::fromStringToWString("x2"), &x2);
	p.DefineVar(conversion::fromStringToWString("y2"), &y2);
	p.DefineVar(conversion::fromStringToWString("z2"), &z2);

	// attributes of the vertices
	// normals:
	p.DefineVar(conversion::fromStringToWString("nx0"), &nx0);
	p.DefineVar(conversion::fromStringToWString("ny0"), &ny0);
	p.DefineVar(conversion::fromStringToWString("nz0"), &nz0);

	p.DefineVar(conversion::fromStringToWString("nx1"), &nx1);
	p.DefineVar(conversion::fromStringToWString("ny1"), &ny1);
	p.DefineVar(conversion::fromStringToWString("nz1"), &nz1);

	p.DefineVar(conversion::fromStringToWString("nx2"), &nx2);
	p.DefineVar(conversion::fromStringToWString("ny2"), &ny2);
	p.DefineVar(conversion::fromStringToWString("nz2"), &nz2);

	// colors:
	p.DefineVar(conversion::fromStringToWString("r0"), &r0);
	p.DefineVar(conversion::fromStringToWString("g0"), &g0);
	p.DefineVar(conversion::fromStringToWString("b0"), &b0);
	p.DefineVar(conversion::fromStringToWString("a0"), &a0);

	p.DefineVar(conversion::fromStringToWString("r1"), &r1);
	p.DefineVar(conversion::fromStringToWString("g1"), &g1);
	p.DefineVar(conversion::fromStringToWString("b1"), &b1);
	p.DefineVar(conversion::fromStringToWString("a1"), &a1);

	p.DefineVar(conversion::fromStringToWString("r2"), &r2);
	p.DefineVar(conversion::fromStringToWString("g2"), &g2);
	p.DefineVar(conversion::fromStringToWString("b2"), &b2);
	p.DefineVar(conversion::fromStringToWString("a2"), &a2);

	// quality
	p.DefineVar(conversion::fromStringToWString("q0"), &q0);
	p.DefineVar(conversion::fromStringToWString("q1"), &q1);
	p.DefineVar(conversion::fromStringToWString("q2"), &q2);

	// face color
	p.DefineVar(conversion::fromStringToWString("fr"), &fr);
	p.DefineVar(conversion::fromStringToWString("fg"), &fg);
	p.DefineVar(conversion::fromStringToWString("fb"), &fb);
	p.DefineVar(conversion::fromStringToWString("fa"), &fa);

	// face normal
	p.DefineVar(conversion::fromStringToWString("fnx"), &fnx);
	p.DefineVar(conversion::fromStringToWString("fny"), &fny);
	p.DefineVar(conversion::fromStringToWString("fnz"), &fnz);

	// face quality
	p.DefineVar(conversion::fromStringToWString("fq"), &fq);

	// index
	p.DefineVar(conversion::fromStringToWString("fi"), &f);
	p.DefineVar(conversion::fromStringToWString("vi0"), &v0i);
	p.DefineVar(conversion::fromStringToWString("vi1"), &v1i);
	p.DefineVar(conversion::fromStringToWString("vi2"), &v2i);

	// texture
	p.DefineVar(conversion::fromStringToWString("wtu0"), &wtu0);
	p.DefineVar(conversion::fromStringToWString("wtv0"), &wtv0);
	p.DefineVar(conversion::fromStringToWString("wtu1"), &wtu1);
	p.DefineVar(conversion::fromStringToWString("wtv1"), &wtv1);
	p.DefineVar(conversion::fromStringToWString("wtu2"), &wtu2);
	p.DefineVar(conversion::fromStringToWString("wtv2"), &wtv2);
	p.DefineVar(conversion::fromStringToWString("ti"), &ti);

	// selection
	p.DefineVar(conversion::fromStringToWString("vsel0"), &vsel0);
	p.DefineVar(conversion::fromStringToWString("vsel1"), &vsel1);
	p.DefineVar(conversion::fromStringToWString("vsel2"), &vsel2);
	p.DefineVar(conversion::fromStringToWString("fsel"), &fsel);

    //Add tokens related to mesh bounding box
	p.DefineVar(conversion::fromStringToWString("xmin"), &xmin);
	p.DefineVar(conversion::fromStringToWString("ymin"), &ymin);
	p.DefineVar(conversion::fromStringToWString("zmin"), &zmin);
	p.DefineVar(conversion::fromStringToWString("xmax"), &xmax);
	p.DefineVar(conversion::fromStringToWString("ymax"), &ymax);
	p.DefineVar(conversion::fromStringToWString("zmax"), &zmax);
	p.DefineVar(conversion::fromStringToWString("bbdiag"), &bbdiag);
	p.DefineVar(conversion::fromStringToWString("xdim"), &xdim);
	p.DefineVar(conversion::fromStringToWString("ydim"), &ydim);
	p.DefineVar(conversion::fromStringToWString("zdim"), &zdim);
	p.DefineVar(conversion::fromStringToWString("xmid"), &xmid);
	p.DefineVar(conversion::fromStringToWString("ymid"), &ymid);
	p.DefineVar(conversion::fromStringToWString("zmid"), &zmid);

	//Add function rnd() 
	p.DefineFun("rnd", random);

	// define var for user-defined attributes (if any exists)
	// if vector is empty, code won't be executed
	f_handlers.clear();
	f_attrNames.clear();
	f_attrValue.clear();
	std::vector<std::string> AllFaceAttribName;
	tri::Allocator<CMeshO>::GetAllPerFaceAttribute<Scalarm>(m, AllFaceAttribName);
	f_attrValue.reserve(AllFaceAttribName.size());
	qDebug("Searching for Scalar Face Attributes (%lu)",AllFaceAttribName.size());
	for (int i = 0; i < (int) AllFaceAttribName.size(); i++) {
		CMeshO::PerFaceAttributeHandle<Scalarm> hh =
			tri::Allocator<CMeshO>::GetPerFaceAttribute<Scalarm>(m, AllFaceAttribName[i]);
		f_handlers.push_back(hh);
		f_attrNames.push_back(AllFaceAttribName[i]);
		f_attrValue.push_back(0);
		p.DefineVar(conversion::fromStringToWString(f_attrNames.back()), &f_attrValue.back());
	}
	AllFaceAttribName.clear();
	f3_handlers.clear();
	f3_attrNames.clear();
	f3_attrValue.clear();
	tri::Allocator<CMeshO>::GetAllPerFaceAttribute<Point3m>(m, AllFaceAttribName);
	f3_attrValue.reserve(AllFaceAttribName.size());
	qDebug("Searching for Point3 Face Attributes (%lu)",AllFaceAttribName.size());
	for (int i = 0; i < (int) AllFaceAttribName.size(); i++) {
		CMeshO::PerFaceAttributeHandle<Point3m> hh3 =
			tri::Allocator<CMeshO>::GetPerFaceAttribute<Point3m>(m, AllFaceAttribName[i]);
		
		f3_handlers.push_back(hh3);
		
		f3_attrValue.push_back(0);
		f3_attrNames.push_back(AllFaceAttribName[i] + "_x");
		p.DefineVar(conversion::fromStringToWString(f3_attrNames.back()), &f3_attrValue.back());
		
		f3_attrValue.push_back(0);
		f3_attrNames.push_back(AllFaceAttribName[i] + "_y");
		p.DefineVar(conversion::fromStringToWString(f3_attrNames.back()), &f3_attrValue.back());
		
		f3_attrValue.push_back(0);
		f3_attrNames.push_back(AllFaceAttribName[i] + "_z");
		p.DefineVar(conversion::fromStringToWString(f3_attrNames.back()), &f3_attrValue.back());
		qDebug("Adding custom per face Point3f variable %s", f3_attrNames.back().c_str());
	}
}

void FilterFunctionPlugin::checkAttributeName(const std::string &name) const
{
	static const std::string validChars =
		"0123456789_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	if ( !name.length() ||
		(name.find_first_not_of(validChars)!=string_type::npos) ||
		(name[0]>='0' && name[0]<='9'))
	{
		throw MLException(
			"Invalid Attribute name: only letters, numbers and underscores are allowed in custom "
			"attribute names.");
	}
}

FilterPlugin::FilterArity FilterFunctionPlugin::filterArity(const QAction* filter) const
{
	switch (ID(filter)) {
	case FF_VERT_SELECTION:
	case FF_FACE_SELECTION:
	case FF_GEOM_FUNC:
	case FF_FACE_COLOR:
	case FF_FACE_QUALITY:
	case FF_VERT_COLOR:
	case FF_VERT_QUALITY:
	case FF_VERT_TEXTURE_FUNC:
	case FF_WEDGE_TEXTURE_FUNC:
	case FF_VERT_NORMAL:
	case FF_FACE_NORMAL:
	case FF_DEF_VERT_SCALAR_ATTRIB:
	case FF_DEF_FACE_SCALAR_ATTRIB:
	case FF_REFINE: return FilterPlugin::SINGLE_MESH;
	case FF_GRID:
	case FF_ISOSURFACE: return FilterPlugin::NONE;
	}
	return FilterPlugin::NONE;
}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterFunctionPlugin)
