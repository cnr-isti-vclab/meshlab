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

#include "filter_func.h"
#include <vcg/complex/algorithms/create/platonic.h>

#include <vcg/complex/algorithms/create/marching_cubes.h>
#include <vcg/complex/algorithms/create/mc_trivial_walker.h>

#include "muParser.h"
#include "string_conversion.h"

using namespace mu;
using namespace vcg;

// Constructor
FilterFunctionPlugin::FilterFunctionPlugin()
{
    typeList
		<< FF_VERT_SELECTION
		<< FF_FACE_SELECTION
		<< FF_GEOM_FUNC
		<< FF_WEDGE_TEXTURE_FUNC
		<< FF_VERT_TEXTURE_FUNC
		<< FF_FACE_COLOR
		<< FF_VERT_COLOR
		<< FF_VERT_QUALITY
		<< FF_VERT_NORMAL
		<< FF_FACE_QUALITY
		<< FF_DEF_VERT_ATTRIB
		<< FF_DEF_FACE_ATTRIB
		<< FF_GRID
		<< FF_ISOSURFACE
		<< FF_REFINE;

    foreach(FilterIDType tt , types())
        actionList << new QAction(filterName(tt), this);
}

FilterFunctionPlugin::~FilterFunctionPlugin()
{
    for (int i = 0; i < actionList.count() ; i++ )
        delete actionList.at(i);
}

QString FilterFunctionPlugin::pluginName() const
{
    return "FilterFunc";
}

// short string describing each filtering action
QString FilterFunctionPlugin::filterName(FilterIDType filterId) const
{
  switch(filterId) {
	  case FF_VERT_SELECTION:     return QString("Conditional Vertex Selection");
	  case FF_FACE_SELECTION:     return QString("Conditional Face Selection");
	  case FF_GEOM_FUNC:          return QString("Per Vertex Geometric Function");
	  case FF_FACE_COLOR:         return QString("Per Face Color Function");
	  case FF_FACE_QUALITY:       return QString("Per Face Quality Function");
	  case FF_VERT_COLOR:         return QString("Per Vertex Color Function");
	  case FF_VERT_QUALITY:       return QString("Per Vertex Quality Function");
	  case FF_VERT_TEXTURE_FUNC:  return QString("Per Vertex Texture Function");
	  case FF_WEDGE_TEXTURE_FUNC: return QString("Per Wedge Texture Function");
	  case FF_VERT_NORMAL:        return QString("Per Vertex Normal Function");
	  case FF_DEF_VERT_ATTRIB:    return QString("Define New Per Vertex Attribute");
	  case FF_DEF_FACE_ATTRIB:    return QString("Define New Per Face Attribute");
	  case FF_GRID:               return QString("Grid Generator");
	  case FF_REFINE:             return QString("Refine User-Defined");
	  case FF_ISOSURFACE:         return QString("Implicit Surface");

	  default: assert(0);
  }
  return QString("error!");
}

const QString PossibleOperators("<br>It's possible to use parenthesis <b>()</b>, and predefined operators:<br>"
	"<b>&&</b> (logic and), <b>||</b> (logic or), <b>&lt;</b>, <b>&lt;=</b>, <b>></b>, <b>>=</b>, <b>!=</b> (not equal), <b>==</b> (equal), <b>_?_:_</b> (c/c++ ternary operator)<br><br>");

const QString PerVertexAttributeString(	"It's possible to use the following per-vertex variables in the expression:<br>"
										"<b>x,y,z</b> (position), <b>nx,ny,nz</b> (normal), <b>r,g,b,a</b> (color), <b>q</b> (quality), "
										"<b>rad</b> (radius), <b>vi</b> (vertex index), <b>vtu,vtv,ti</b> (texture coords and texture index), <b>vsel</b> (is the vertex selected? 1 yes, 0 no) "
										"and all custom <i>vertex attributes</i> already defined by user.<br>");

const QString PerFaceAttributeString("It's possible to use the following per-face variables, or variables associated to the three vertex of every face:<br>"
										"<b>x0,y0,z0</b> for the first vertex position, <b>x1,y1,z1</b> for the second vertex position, <b>x2,y2,z2</b> for the third vertex position, "
										"<b>nx0,ny0,nz0 nx1,ny1,nz1 nx2,ny2,nz2</b> for vertex normals, <b>r0,g0,b0,a0 r1,g1,b1,a1 r2,g2,b2,a2</b> for vertex colors, "
										"<b>q0,q1,q2</b> for vertex quality, <b>wtu0,wtv0 wtu1,wtv1 wtu2,wtv2</b> for per-wedge texture coords, <b>ti</b> for face texture index, <b>vsel0,vsel1,vsel2</b> for vertex selection (1 yes, 0 no) "
										"<b>fr,fg,fb,fa</b> for face color, <b>fq</b> for face quality, <b>fnx,fny,fnz</b> for face normal, <b>fsel</b> face selection (1 yes, 0 no).<br>");

// long string describing each filtering action
QString FilterFunctionPlugin::filterInfo(FilterIDType filterId) const
{
    switch(filterId) {
		case FF_VERT_SELECTION : return tr(	"Boolean function using muparser lib to perform vertex selection over current mesh.<br>")
			+ PossibleOperators + PerVertexAttributeString;

		case FF_FACE_SELECTION : return tr(	"Boolean function using muparser lib to perform faces selection over current mesh.<br>") 
			+ PossibleOperators + PerFaceAttributeString;

		case FF_GEOM_FUNC :  return tr(	"Geometric function using muparser lib to generate new Coord<br>"
			"You can change x,y,z for every vertex according to the function specified.<br>") + PerVertexAttributeString;

		case FF_FACE_COLOR : return tr(	"Color function using muparser lib to generate new RGBA color for every face<br>"
			"Red, Green, Blue and Alpha channels may be defined specifying a function in their respective fields.<br>") + PerFaceAttributeString;

		case FF_VERT_COLOR : return tr(	"Color function using muparser lib to generate new RGBA color for every vertex<br>"
			"Red, Green, Blue and Alpha channels may be defined specifying a function in their respective fields.<br>") + PerVertexAttributeString;

		case FF_VERT_QUALITY: return tr("Quality function using muparser to generate new Quality for every vertex<br>") + PerVertexAttributeString;

		case FF_VERT_TEXTURE_FUNC: return tr("Texture function using muparser to generate new texture coords for every vertex<br>") + PerVertexAttributeString;

		case FF_VERT_NORMAL: return tr("Normal function using muparser to generate new Normal for every vertex<br>") + PerVertexAttributeString;

        case FF_FACE_QUALITY : return tr("Quality function using muparser to generate new Quality for every face<br>"
			"Insert three function each one for quality of the three vertex of a face<br>") +PerFaceAttributeString;

		case FF_WEDGE_TEXTURE_FUNC : return tr("Texture function using muparser to generate new per wedge tex coords for every face<br>"
			"Insert six functions each u v for each one of the three vertex of a face<br>") +PerFaceAttributeString;

        case FF_DEF_VERT_ATTRIB : return tr("Add a new Per-Vertex scalar attribute to current mesh and fill it with the defined function.<br>"
			"The name specified below can be used in other filter function") +PerVertexAttributeString;

        case FF_DEF_FACE_ATTRIB : return tr("Add a new Per-Face attribute to current mesh.<br>"
			"You can specify custom name and a function to generate attribute's value<br>"
			"It's possible to use per-face variables in the expression:<br>") +PerFaceAttributeString+
			tr("<font color=\"#FF0000\">The attribute name specified below can be used in other filter function</font>");

        case FF_GRID : return tr("Generate a new 2D Grid mesh with number of vertices on X and Y axis specified by user with absolute length/height.<br>"
			"It's possible to center Grid on origin.");

        case FF_ISOSURFACE : return tr("Generate a new mesh that corresponds to the 0 valued isosurface defined by the scalar field generated by the given expression");

        case FF_REFINE : return tr("Refine current mesh with user defined parameters.<br>"
			"Specify a Boolean Function needed to select which edges will be cut for refinement purpose.<br>"
			"Each edge is identified with first and second vertex.<br>"
			"Arguments accepted are first and second vertex attributes:<br>") + PossibleOperators + PerFaceAttributeString;

        default : assert(0);
    }
    return QString("filter not found!");
}

FilterFunctionPlugin::FilterClass FilterFunctionPlugin::getClass(const QAction *a) const
{
  switch(ID(a))
  {
	  case FF_FACE_SELECTION:
	  case FF_VERT_SELECTION: return MeshFilterInterface::Selection;
	  case FF_FACE_QUALITY: return FilterClass(Quality + FaceColoring);
	  case FF_VERT_QUALITY: return FilterClass(Quality + VertexColoring);
	  case FF_VERT_TEXTURE_FUNC: return MeshFilterInterface::Texture;
	  case FF_VERT_COLOR:	return MeshFilterInterface::VertexColoring;
	  case FF_VERT_NORMAL:	return MeshFilterInterface::Normal;
	  case FF_FACE_COLOR: return MeshFilterInterface::FaceColoring;
	  case FF_WEDGE_TEXTURE_FUNC: return MeshFilterInterface::Texture;
	  case FF_ISOSURFACE: return MeshFilterInterface::MeshCreation;
	  case FF_GRID: return MeshFilterInterface::MeshCreation;
	  case FF_REFINE: return MeshFilterInterface::Remeshing;
	  case FF_GEOM_FUNC: return MeshFilterInterface::Smoothing;
	  case FF_DEF_VERT_ATTRIB: return MeshFilterInterface::Layer;
	  case FF_DEF_FACE_ATTRIB: return MeshFilterInterface::Layer;

	  default: return MeshFilterInterface::Generic;
  }
}

int FilterFunctionPlugin::postCondition(const QAction *action) const
{
  switch(ID(action))
  {
	case FF_VERT_SELECTION:
	case FF_FACE_SELECTION:
		return MeshModel::MM_VERTFLAGSELECT | MeshModel::MM_FACEFLAGSELECT;
	case FF_FACE_COLOR:
		return MeshModel::MM_FACECOLOR;
	case FF_GEOM_FUNC:
		return MeshModel::MM_VERTCOORD + MeshModel::MM_VERTNORMAL + MeshModel::MM_FACENORMAL;
	case FF_VERT_COLOR:
		return MeshModel::MM_VERTCOLOR;
	case FF_VERT_NORMAL:
		return MeshModel::MM_VERTNORMAL;
	case FF_VERT_TEXTURE_FUNC:
		return MeshModel::MM_VERTTEXCOORD;
	case FF_WEDGE_TEXTURE_FUNC:
		return MeshModel::MM_WEDGTEXCOORD;
	case FF_VERT_QUALITY:
		return MeshModel::MM_VERTQUALITY + MeshModel::MM_VERTCOLOR;
	case FF_FACE_QUALITY:
		return MeshModel::MM_FACECOLOR + MeshModel::MM_FACEQUALITY;

	case FF_DEF_VERT_ATTRIB:
	case FF_DEF_FACE_ATTRIB:
		return MeshModel::MM_NONE;  // none, because they do not change any existing data

	case FF_REFINE:
		return MeshModel::MM_ALL;

	case FF_GRID:
	case FF_ISOSURFACE:
		return MeshModel::MM_NONE;  // none, because they create a new layer, without affecting old one
  }
  
  return MeshModel::MM_NONE;
}

int FilterFunctionPlugin::getRequirements(const QAction *action)
{
  switch(ID(action))
  {
	case FF_VERT_SELECTION :
	case FF_GEOM_FUNC :
	case FF_VERT_COLOR :
	case FF_VERT_NORMAL :
	case FF_VERT_QUALITY :
	case FF_VERT_TEXTURE_FUNC:
	case FF_WEDGE_TEXTURE_FUNC:
	case FF_DEF_VERT_ATTRIB :
	case FF_GRID :
	case FF_ISOSURFACE :
	case FF_DEF_FACE_ATTRIB :
	case FF_FACE_SELECTION  : return 0;
	case FF_FACE_QUALITY    : return MeshModel::MM_FACECOLOR + MeshModel::MM_FACEQUALITY;
	case FF_FACE_COLOR      : return MeshModel::MM_FACECOLOR;
	case FF_REFINE          : return MeshModel::MM_FACEFACETOPO | MeshModel::MM_VERTMARK;
	default: assert(0);
  }
  return 0;
}

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parameter you need to define,
// - the name of the parameter,
// - the string shown in the dialog
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void FilterFunctionPlugin::initParameterSet(QAction *action,MeshModel &m, RichParameterList & parlst)
{
  Q_UNUSED(m);
  switch(ID(action))	 {

  case FF_VERT_SELECTION :
    parlst.addParam(RichString("condSelect","(q < 0)", "boolean function",
                                   "type a boolean function that will be evaluated in order to select a subset of vertices<br>"
                                   "example: (y > 0) and (ny > 0)"));
    break;

  case FF_FACE_SELECTION :
    parlst.addParam(RichString("condSelect","(fi == 0)", "boolean function",
                                   "type a boolean function that will be evaluated in order to select a subset of faces<br>"));
    break;

  case FF_GEOM_FUNC:
      parlst.addParam(RichString("x", "x", "func x = ", "insert function to generate new coord for x"));
      parlst.addParam(RichString("y", "y", "func y = ", "insert function to generate new coord for y"));
      parlst.addParam(RichString("z", "sin(x+y)", "func z = ", "insert function to generate new coord for z"));
      parlst.addParam(RichBool("onselected", false, "only on selection", "if checked, only affects selected vertices"));
	  break;

  case FF_VERT_NORMAL:
      parlst.addParam(RichString("x", "-nx", "func nx = ", "insert function to generate new x for the normal"));
      parlst.addParam(RichString("y", "-ny", "func ny = ", "insert function to generate new y for the normal"));
      parlst.addParam(RichString("z", "-nz", "func nz = ", "insert function to generate new z for the normal"));
      parlst.addParam(RichBool("onselected", false, "only on selection", "if checked, only affects selected vertices"));
	  break;

  case FF_VERT_COLOR:
      parlst.addParam(RichString("x", "255", "func r = ", "function to generate Red component. Expected Range 0-255"));
      parlst.addParam(RichString("y", "255", "func g = ", "function to generate Green component. Expected Range 0-255"));
      parlst.addParam(RichString("z", "0", "func b = ", "function to generate Blue component. Expected Range 0-255"));
      parlst.addParam(RichString("a", "255", "func alpha = ", "function to generate Alpha component. Expected Range 0-255"));
      parlst.addParam(RichBool("onselected", false, "only on selection", "if checked, only affects selected vertices"));
	  break;

  case FF_VERT_TEXTURE_FUNC:
      parlst.addParam(RichString("u", "x", "func u = ", "function to generate u texture coord. Expected Range 0-1"));
      parlst.addParam(RichString("v", "y", "func v = ", "function to generate v texture coord. Expected Range 0-1"));
      parlst.addParam(RichBool("onselected", false, "only on selection", "if checked, only affects selected vertices"));
	  break;

  case FF_VERT_QUALITY:
      parlst.addParam(RichString("q", "vi", "func q = ", "function to generate new Quality for every vertex"));
      parlst.addParam(RichBool("normalize", false, "normalize", "if checked normalize all quality values in range [0..1]"));
      parlst.addParam(RichBool("map", false, "map into color", "if checked map quality generated values into per-vertex color"));
      parlst.addParam(RichBool("onselected", false, "only on selection", "if checked, only affects selected vertices"));
	  break;

  case FF_FACE_COLOR:
      parlst.addParam(RichString("r", "255", "func r = ", "function to generate Red component. Expected Range 0-255"));
      parlst.addParam(RichString("g", "0", "func g = ", "function to generate Green component. Expected Range 0-255"));
      parlst.addParam(RichString("b", "255", "func b = ", "function to generate Blue component. Expected Range 0-255"));
      parlst.addParam(RichString("a", "255", "func alpha = ", "function to generate Alpha component. Expected Range 0-255"));
      parlst.addParam(RichBool("onselected", false, "only on selection", "if checked, only affects selected faces"));
	  break;

  case FF_FACE_QUALITY:
      parlst.addParam(RichString("q", "x0+y0+z0", "func q0 = ", "function to generate new Quality foreach face"));
      parlst.addParam(RichBool("normalize", false, "normalize", "if checked normalize all quality values in range [0..1]"));
      parlst.addParam(RichBool("map", false, "map into color", "if checked map quality generated values into per-vertex color"));
      parlst.addParam(RichBool("onselected", false, "only on selection", "if checked, only affects selected faces"));
	  break;

  case FF_WEDGE_TEXTURE_FUNC:
    parlst.addParam(RichString("u0","x0", "func u0 = ", "function to generate u texture coord. of wedge 0. Expected Range 0-1"));
    parlst.addParam(RichString("v0","y0", "func v0 = ", "function to generate v texture coord. of wedge 0. Expected Range 0-1"));
    parlst.addParam(RichString("u1","x1", "func u1 = ", "function to generate u texture coord. of wedge 1. Expected Range 0-1"));
    parlst.addParam(RichString("v1","y1", "func v1 = ", "function to generate v texture coord. of wedge 1. Expected Range 0-1"));
    parlst.addParam(RichString("u2","x2", "func u2 = ", "function to generate u texture coord. of wedge 2. Expected Range 0-1"));
    parlst.addParam(RichString("v2","y2", "func v2 = ", "function to generate v texture coord. of wedge 2. Expected Range 0-1"));
    parlst.addParam(RichBool("onselected", false, "only on selection", "if checked, only affects selected faces"));
    break;

  case FF_DEF_VERT_ATTRIB:
    parlst.addParam(RichString("name","Radiosity","Name", "the name of new attribute. you can access attribute in other filters through this name"));
    parlst.addParam(RichString("expr","x","Function =", "function to calculate custom attribute value for each vertex"));
    break;

  case FF_DEF_FACE_ATTRIB:
    parlst.addParam(RichString("name","Radiosity","Name", "the name of new attribute. you can access attribute in other filters through this name"));
    parlst.addParam(RichString("expr","fi","Function =", "function to calculate custom attribute value for each face"));
    break;

  case FF_GRID :
    parlst.addParam(RichInt("numVertX", 10, "num vertices on x", "number of vertices on x. it must be positive"));
    parlst.addParam(RichInt("numVertY", 10, "num vertices on y", "number of vertices on y. it must be positive"));
    parlst.addParam(RichFloat("absScaleX", 0.3f, "x scale", "absolute scale on x (float)"));
    parlst.addParam(RichFloat("absScaleY", 0.3f, "y scale", "absolute scale on y (float)"));
    parlst.addParam(RichBool("center",false,"centered on origin", "center grid generated by filter on origin.<br>"
                                 "Grid is first generated and than moved into origin (using muparser lib to perform fast calc on every vertex)"));
    break;
  case FF_ISOSURFACE :
    parlst.addParam(RichFloat("voxelSize", 0.05f, "Size of Voxel", "Size of the voxel that is used by for the grid where the field is sampled. Smaller this value, higher precision, but higher processing times."));
    parlst.addParam(RichFloat("minX", -1.0f, "Min X", "Range where the field is sampled"));
    parlst.addParam(RichFloat("minY", -1.0f, "Min Y", "Range where the field is sampled"));
    parlst.addParam(RichFloat("minZ", -1.0f, "Min Z", "Range where the field is sampled"));
    parlst.addParam(RichFloat("maxX", 1.0f, "Max X", "Range where the field is sampled"));
    parlst.addParam(RichFloat("maxY", 1.0f, "Max Y", "Range where the field is sampled"));
    parlst.addParam(RichFloat("maxZ", 1.0f, "Max Z", "Range where the field is sampled"));
    parlst.addParam(RichString("expr","x*x+y*y+z*z-0.5","Function =", "This expression is evaluated for each voxel of the grid. The surface passing through the zero valued points of this field is then extracted using marching cube."));

    break;

  case FF_REFINE :
    parlst.addParam(RichString("condSelect","(q0 >= 0 && q1 >= 0)","boolean function","type a boolean function that will be evaluated on every edge"));
    parlst.addParam(RichString("x","(x0+x1)/2","x =","function to generate x coord of new vertex in [x0,x1].<br>For example (x0+x1)/2"));
    parlst.addParam(RichString("y","(y0+y1)/2","y =","function to generate x coord of new vertex in [y0,y1].<br>For example (y0+y1)/2"));
    parlst.addParam(RichString("z","(z0+z1)/2","z =","function to generate x coord of new vertex in [z0,z1].<br>For example (z0+z1)/2"));
    break;

  default: break; // do not add any parameter for the other filters
  }
}

// The Real Core Function doing the actual mesh processing.
bool FilterFunctionPlugin::applyFilter(const QAction *filter, MeshDocument &md, const RichParameterList & par, vcg::CallBackPos *cb)
{
  if(this->getClass(filter) == MeshFilterInterface::MeshCreation)
    md.addNewMesh("",this->filterName(ID(filter)));
  MeshModel &m=*(md.mm());
  Q_UNUSED(cb);
  switch(ID(filter)) {
  case FF_VERT_SELECTION :
  {
    std::string expr = par.getString("condSelect").toStdString();
	  auto wexpr = conversion::fromStringToWString(expr);

    // muparser initialization and explicitly define parser variables
    Parser p;
    setPerVertexVariables(p,m.cm);

    // set expression inserted by user as string (required by muparser)
    p.SetExpr(wexpr);

    int numvert = 0;
    time_t start = clock();

    // every parser variables is related to vertex coord and attributes.
    CMeshO::VertexIterator vi;
    for(vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi)if(!(*vi).IsD())
    {
      setAttributes(vi,m.cm);

      bool selected = false;

      // use parser to evaluate boolean function specified above
      // in case of fail, error dialog contains details of parser's error
      try {
        selected = p.Eval();
      } catch(Parser::exception_type &e) {
        errorMessage = conversion::fromWStringToString(e.GetMsg()).c_str();
        return false;
      }

      // set vertex as selected or clear selection
      if(selected) {
        (*vi).SetS();
        numvert++;
      } else (*vi).ClearS();
    }

    // if succeeded log stream contains number of vertices and time elapsed
    Log( "selected %d vertices in %.2f sec.", numvert, (clock() - start) / (float) CLOCKS_PER_SEC);

    return true;
  }
    break;

  case FF_FACE_SELECTION :
  {
    QString select = par.getString("condSelect");

    // muparser initialization and explicitly define parser variables
    Parser p;
    setPerFaceVariables(p,m.cm);

    // set expression inserted by user as string (required by muparser)
    p.SetExpr(conversion::fromStringToWString(select.toStdString()));

    int numface = 0;
    time_t start = clock();

    // every parser variables is related to face attributes.
    CMeshO::FaceIterator fi;
    for(fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)if(!(*fi).IsD())
    {
      setAttributes(fi,m.cm);

      bool selected = false;

      // use parser to evaluate boolean function specified above
      // in case of fail, error dialog contains details of parser's error
      try {
        selected = p.Eval();
      } catch(Parser::exception_type &e) {
        errorMessage = conversion::fromWStringToString(e.GetMsg()).c_str();
        return false;
      }

      // set face as selected or clear selection
      if(selected) {
        (*fi).SetS();
        numface++;
      } else (*fi).ClearS();
    }

    // if succeeded log stream contains number of vertices and time elapsed
    Log( "selected %d faces in %.2f sec.", numface, (clock() - start) / (float) CLOCKS_PER_SEC);

    return true;
  }
    break;

  case FF_GEOM_FUNC :
  case FF_VERT_COLOR:
  case FF_VERT_NORMAL:
  {
    std::string func_x,func_y,func_z,func_a;
    // FF_VERT_COLOR : x = r, y = g, z = b
    // FF_VERT_NORMAL : x = r, y = g, z = b
    func_x = par.getString("x").toStdString();
    func_y = par.getString("y").toStdString();
    func_z = par.getString("z").toStdString();
    if(ID(filter) == FF_VERT_COLOR) func_a = par.getString("a").toStdString();

	bool onSelected = par.getBool("onselected");

	if (onSelected && m.cm.svn == 0 && m.cm.sfn == 0) // if no selection at all, fail
	{
		Log("Cannot apply only on selection: there is no selection");
		errorMessage = "Cannot apply only on selection: there is no selection";
		return false;
	}
	if (onSelected && (m.cm.svn == 0 && m.cm.sfn > 0)) // if no vert selected, but some faces selected, use their vertices
	{
		tri::UpdateSelection<CMeshO>::VertexClear(m.cm);
		tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(m.cm);
	}

    // muparser initialization and explicitly define parser variables
    // function for x,y and z must use different parser and variables
    Parser p1,p2,p3,p4;

    setPerVertexVariables(p1,m.cm);
    setPerVertexVariables(p2,m.cm);
    setPerVertexVariables(p3,m.cm);
    setPerVertexVariables(p4,m.cm);

    p1.SetExpr(conversion::fromStringToWString(func_x));
    p2.SetExpr(conversion::fromStringToWString(func_y));
    p3.SetExpr(conversion::fromStringToWString(func_z));
    p4.SetExpr(conversion::fromStringToWString(func_a));

    double newx=0,newy=0,newz=0,newa=255;
    errorMessage = "";

    time_t start = clock();

    // every parser variables is related to vertex coord and attributes.
    CMeshO::VertexIterator vi;
    for(vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi)
		if(!(*vi).IsD())
			if ((!onSelected) || ((*vi).IsS()))
				{
				  setAttributes(vi,m.cm);

				  // every function is evaluated by different parser.
				  // errorMessage dialog contains errors for func x, func y and func z
				  try { newx = p1.Eval(); }  catch(Parser::exception_type &e) { showParserError("1st func : ",e); }
				  try { newy = p2.Eval(); }  catch(Parser::exception_type &e) { showParserError("2nd func : ",e); }
				  try { newz = p3.Eval(); }  catch(Parser::exception_type &e) { showParserError("3rd func : ",e); }
				  if(ID(filter) == FF_VERT_COLOR)
				  {
					try { newa = p4.Eval(); } catch(Parser::exception_type &e) { showParserError("4th func : ",e); }
				  }
				  if(errorMessage != "") return false;

				  if (ID(filter) == FF_GEOM_FUNC)  // set new vertex coord for this iteration
					  (*vi).P() = Point3m(newx, newy, newz);
				  if (ID(filter) == FF_VERT_NORMAL) // set new color for this iteration
					  (*vi).N() = Point3m(newx, newy, newz);
				  if (ID(filter) == FF_VERT_COLOR) // set new color for this iteration
				  {
					  (*vi).C() = Color4b(newx, newy, newz, newa);
					  m.updateDataMask(MeshModel::MM_VERTCOLOR);
				  }

				}

    if(ID(filter) == FF_GEOM_FUNC) {
      // update bounding box, normalize normals
      tri::UpdateNormal<CMeshO>::PerVertexNormalizedPerFace(m.cm);
      tri::UpdateNormal<CMeshO>::NormalizePerFace(m.cm);
      tri::UpdateBounding<CMeshO>::Box(m.cm);
    }

    // if succeeded log stream contains number of vertices processed and time elapsed
    Log( "%d vertices processed in %.2f sec.", m.cm.vn, (clock() - start) / (float) CLOCKS_PER_SEC);

    return true;
  }
    break;

  case FF_VERT_QUALITY:
  {
    std::string func_q = par.getString("q").toStdString();
	bool onSelected = par.getBool("onselected");

	if (onSelected && m.cm.svn == 0 && m.cm.sfn == 0) // if no selection at all, fail
	{
		Log("Cannot apply only on selection: there is no selection");
		errorMessage = "Cannot apply only on selection: there is no selection";
		return false;
	}
	if (onSelected && (m.cm.svn == 0 && m.cm.sfn > 0)) // if no vert selected, but some faces selected, use their vertices
	{
		tri::UpdateSelection<CMeshO>::VertexClear(m.cm);
		tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(m.cm);
	}

    m.updateDataMask(MeshModel::MM_VERTQUALITY);

    // muparser initialization and define custom variables
    Parser p;
    setPerVertexVariables(p,m.cm);

    // set expression to calc with parser
    p.SetExpr(conversion::fromStringToWString(func_q));

    // every parser variables is related to vertex coord and attributes.
    time_t start = clock();
    CMeshO::VertexIterator vi;
    for(vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi)
      if(!(*vi).IsD())
		  if ((!onSelected) || ((*vi).IsS()))
		  {
			setAttributes(vi,m.cm);

			// use parser to evaluate function specified above
			// in case of fail, errorMessage dialog contains details of parser's error
			try {
			  (*vi).Q() = p.Eval();
			} catch(Parser::exception_type &e) {
			  errorMessage = conversion::fromWStringToString(e.GetMsg()).c_str();
			  return false;
			}
		  }

    // normalize quality with values in [0..1]
    if(par.getBool("normalize")) tri::UpdateQuality<CMeshO>::VertexNormalize(m.cm);

    // map quality into per-vertex color
    if(par.getBool("map"))
    {
        tri::UpdateColor<CMeshO>::PerVertexQualityRamp(m.cm);
        m.updateDataMask(MeshModel::MM_VERTCOLOR);
    }
    // if succeeded log stream contains number of vertices and time elapsed
    Log( "%d vertices processed in %.2f sec.", m.cm.vn, (clock() - start) / (float) CLOCKS_PER_SEC);

    return true;
  }
    break;
  case FF_VERT_TEXTURE_FUNC:
  {
    std::string func_u = par.getString("u").toStdString();
    std::string func_v = par.getString("v").toStdString();
	bool onSelected = par.getBool("onselected");

	if (onSelected && m.cm.svn == 0 && m.cm.sfn == 0) // if no selection at all, fail
	{
		Log("Cannot apply only on selection: there is no selection");
		errorMessage = "Cannot apply only on selection: there is no selection";
		return false;
	}
	if (onSelected && (m.cm.svn == 0 && m.cm.sfn > 0)) // if no vert selected, but some faces selected, use their vertices
	{
		tri::UpdateSelection<CMeshO>::VertexClear(m.cm);
		tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(m.cm);
	}

    m.updateDataMask(MeshModel::MM_VERTTEXCOORD);

    // muparser initialization and define custom variables
    Parser pu,pv;
    setPerVertexVariables(pu,m.cm);
    setPerVertexVariables(pv,m.cm);

    // set expression to calc with parser
#ifdef _UNICODE
    pu.SetExpr(conversion::fromStringToWString(func_u));
    pv.SetExpr(conversion::fromStringToWString(func_v));
#else
    pu.SetExpr(func_u);
    pv.SetExpr(func_v);
#endif

    // every parser variables is related to vertex coord and attributes.
    time_t start = clock();
    CMeshO::VertexIterator vi;
    for(vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi)
      if(!(*vi).IsD())
		  if ((!onSelected) || ((*vi).IsS()))
		  {
			setAttributes(vi,m.cm);

			// use parser to evaluate function specified above
			// in case of fail, errorMessage dialog contains details of parser's error
			try {
			  (*vi).T().U() = pu.Eval();
			  (*vi).T().V() = pv.Eval();
			} catch(Parser::exception_type &e) {
			  errorMessage = conversion::fromWStringToString(e.GetMsg()).c_str();
			  return false;
			}
		  }

    Log( "%d vertices processed in %.2f sec.", m.cm.vn, (clock() - start) / (float) CLOCKS_PER_SEC);
    return true;
  }
    break;
  case FF_WEDGE_TEXTURE_FUNC:
  {
    std::string func_u0 = par.getString("u0").toStdString();
    std::string func_v0 = par.getString("v0").toStdString();
    std::string func_u1 = par.getString("u1").toStdString();
    std::string func_v1 = par.getString("v1").toStdString();
    std::string func_u2 = par.getString("u2").toStdString();
    std::string func_v2 = par.getString("v2").toStdString();
	bool onSelected = par.getBool("onselected");

	if (onSelected && m.cm.sfn == 0) // if no selection, fail
	{
		Log("Cannot apply only on selection: there is no selection");
		errorMessage = "Cannot apply only on selection: there is no selection";
		return false;
	}

    m.updateDataMask(MeshModel::MM_VERTTEXCOORD);

    // muparser initialization and define custom variables
    Parser pu0,pv0,pu1,pv1,pu2,pv2;
    setPerFaceVariables(pu0,m.cm); setPerFaceVariables(pv0,m.cm);
    setPerFaceVariables(pu1,m.cm); setPerFaceVariables(pv1,m.cm);
    setPerFaceVariables(pu2,m.cm); setPerFaceVariables(pv2,m.cm);

    // set expression to calc with parser
    pu0.SetExpr(conversion::fromStringToWString(func_u0)); pv0.SetExpr(conversion::fromStringToWString(func_v0));
    pu1.SetExpr(conversion::fromStringToWString(func_u1)); pv1.SetExpr(conversion::fromStringToWString(func_v1));
    pu2.SetExpr(conversion::fromStringToWString(func_u2)); pv2.SetExpr(conversion::fromStringToWString(func_v2));

    // every parser variables is related to vertex coord and attributes.
    time_t start = clock();
    CMeshO::VertexIterator vi;
    for(CMeshO::FaceIterator fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
      if(!(*fi).IsD())
		  if ((!onSelected) || ((*fi).IsS()))
		  {
			setAttributes(fi,m.cm);

			// use parser to evaluate function specified above
			// in case of fail, errorMessage dialog contains details of parser's error
			try {
			  (*fi).WT(0).U() = pu0.Eval(); (*fi).WT(0).V() = pv0.Eval();
			  (*fi).WT(1).U() = pu1.Eval(); (*fi).WT(1).V() = pv1.Eval();
			  (*fi).WT(2).U() = pu2.Eval(); (*fi).WT(2).V() = pv2.Eval();
			} catch(Parser::exception_type &e) {
				errorMessage = conversion::fromWStringToString(e.GetMsg()).c_str();
			  return false;
			}
		  }

    Log( "%d faces processed in %.2f sec.", m.cm.fn, (clock() - start) / (float) CLOCKS_PER_SEC);
    return true;
  }
    break;
  case FF_FACE_COLOR:
  {
    std::string func_r = par.getString("r").toStdString();
    std::string func_g = par.getString("g").toStdString();
    std::string func_b = par.getString("b").toStdString();
    std::string func_a = par.getString("a").toStdString();
	bool onSelected = par.getBool("onselected");

	if (onSelected && m.cm.sfn == 0) // if no selection, fail
	{
		Log("Cannot apply only on selection: there is no selection");
		errorMessage = "Cannot apply only on selection: there is no selection";
		return false;
	}

	m.updateDataMask(MeshModel::MM_FACECOLOR);

    // muparser initialization and explicitly define parser variables
    // every function must uses own parser and variables
    Parser p1,p2,p3,p4;

    setPerFaceVariables(p1,m.cm);
    setPerFaceVariables(p2,m.cm);
    setPerFaceVariables(p3,m.cm);
    setPerFaceVariables(p4,m.cm);

    p1.SetExpr(conversion::fromStringToWString(func_r));
    p2.SetExpr(conversion::fromStringToWString(func_g));
    p3.SetExpr(conversion::fromStringToWString(func_b));
    p4.SetExpr(conversion::fromStringToWString(func_a));

    // RGB is related to every face
    CMeshO::FaceIterator fi;
    double newr=0,newg=0,newb=0,newa=255;
    errorMessage = "";

    time_t start = clock();

    // every parser variables is related to face attributes.
    for(fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
		if(!(*fi).IsD())
			if ((!onSelected) || ((*fi).IsS()))
			{
			  setAttributes(fi,m.cm);

			  // evaluate functions to generate new color
			  // in case of fail, error dialog contains details of parser's error
			  try { newr = p1.Eval(); } catch(Parser::exception_type &e) { showParserError("func r: ",e); }
			  try { newg = p2.Eval(); } catch(Parser::exception_type &e) { showParserError("func g: ",e); }
			  try { newb = p3.Eval(); } catch(Parser::exception_type &e) { showParserError("func b: ",e); 	}
			  try { newa = p4.Eval(); } catch(Parser::exception_type &e) { showParserError("func a: ",e); 	}

			  if(errorMessage != "") return false;

			  // set new color for this iteration
			  (*fi).C() = Color4b(newr,newg,newb,newa);
			}

    // if succeeded log stream contains number of vertices processed and time elapsed
    Log( "%d faces processed in %.2f sec.", m.cm.fn, (clock() - start) / (float) CLOCKS_PER_SEC);

    return true;

  }
    break;

  case FF_FACE_QUALITY:
  {
    std::string func_q = par.getString("q").toStdString();
	bool onSelected = par.getBool("onselected");

	if (onSelected && m.cm.sfn == 0) // if no selection, fail
	{
		Log("Cannot apply only on selection: there is no selection");
		errorMessage = "Cannot apply only on selection: there is no selection";
		return false;
	}

    m.updateDataMask(MeshModel::MM_FACEQUALITY);

    // muparser initialization and define custom variables
    Parser pf;
    setPerFaceVariables(pf,m.cm);

    // set expression to calc with parser
    pf.SetExpr(conversion::fromStringToWString(func_q));

    time_t start = clock();
    errorMessage = "";

    // every parser variables is related to face attributes.
    CMeshO::FaceIterator fi;
    for(fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
		if(!(*fi).IsD())
			if ((!onSelected) || ((*fi).IsS()))
			{
			  setAttributes(fi,m.cm);

			  // evaluate functions to generate new quality
			  // in case of fail, error dialog contains details of parser's error
			  try { (*fi).Q() = pf.Eval(); }
			  catch(Parser::exception_type &e) {
				showParserError("func q: ",e);
			  }
			  if(errorMessage != "") return false;
			}

    // normalize quality with values in [0..1]
    if(par.getBool("normalize")) tri::UpdateQuality<CMeshO>::FaceNormalize(m.cm);

    // map quality into per-vertex color
    if(par.getBool("map"))
    {
        tri::UpdateColor<CMeshO>::PerFaceQualityRamp(m.cm);
        m.updateDataMask(MeshModel::MM_FACECOLOR);
    }

    // if succeeded log stream contains number of faces processed and time elapsed
    Log( "%d faces processed in %.2f sec.", m.cm.fn, (clock() - start) / (float) CLOCKS_PER_SEC);

    return true;
  }
    break;

  case FF_DEF_VERT_ATTRIB :
  {
    std::string name = par.getString("name").toStdString();
    std::string expr = par.getString("expr").toStdString();

    // add per-vertex attribute with type float and name specified by user
    CMeshO::PerVertexAttributeHandle<float> h;
    if(tri::HasPerVertexAttribute(m.cm,name))
    {
      h = tri::Allocator<CMeshO>::FindPerVertexAttribute<float>(m.cm, name);
      if(!tri::Allocator<CMeshO>::IsValidHandle<float>(m.cm,h))
      {
        errorMessage = "attribute already exists with a different type";
        return false;
      }
    }
    else
      h = tri::Allocator<CMeshO>::AddPerVertexAttribute<float> (m.cm,name);

    std::vector<std::string> AllVertexAttribName;
    tri::Allocator<CMeshO>::GetAllPerVertexAttribute< float >(m.cm,AllVertexAttribName);
    qDebug("Now mesh has %lu vertex float attribute",AllVertexAttribName.size());
    Parser p;
    setPerVertexVariables(p,m.cm);
    p.SetExpr(conversion::fromStringToWString(expr));

    time_t start = clock();

    // perform calculation of attribute's value with function specified by user
    CMeshO::VertexIterator vi;
    for(vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi)if(!(*vi).IsD())
    {
      setAttributes(vi,m.cm);

      // add new user-defined attribute
      try {
        h[vi] = p.Eval();
      } catch(Parser::exception_type &e) {
        errorMessage = conversion::fromWStringToString(e.GetMsg()).c_str();
        return false;
      }
    }

    // add string, double and handler to vector.
    // vectors keep tracks of new attributes and let muparser use explicit variables
    // it's possible to use custom attributes in other filters
    v_attrNames.push_back(name);
    v_attrValue.push_back(0);
    v_handlers.push_back(h);

    // if succeeded log stream contains number of vertices processed and time elapsed
    Log( "%d vertices processed in %.2f sec.", m.cm.vn, (clock() - start) / (float) CLOCKS_PER_SEC);

    return true;
  }
    break;

  case FF_DEF_FACE_ATTRIB :
  {
    std::string name = par.getString("name").toStdString();
    std::string expr = par.getString("expr").toStdString();

    // add per-face attribute with type float and name specified by user
    // add per-vertex attribute with type float and name specified by user
    CMeshO::PerFaceAttributeHandle<float> h;
    if(tri::HasPerFaceAttribute(m.cm,name))
    {
      h = tri::Allocator<CMeshO>::FindPerFaceAttribute<float>(m.cm, name);
      if(!tri::Allocator<CMeshO>::IsValidHandle<float>(m.cm,h))
      {
        errorMessage = "attribute already exists with a different type";
        return false;
      }
    }
    else
      h = tri::Allocator<CMeshO>::AddPerFaceAttribute<float> (m.cm,name);
    Parser p;
    setPerFaceVariables(p,m.cm);
    p.SetExpr(conversion::fromStringToWString(expr));

    time_t start = clock();

    // every parser variables is related to face attributes.
    CMeshO::FaceIterator fi;
    for(fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)if(!(*fi).IsD())
    {
      setAttributes(fi,m.cm);

      // add new user-defined attribute
      try {
        h[fi] = p.Eval();
      } catch(Parser::exception_type &e) {
        errorMessage = conversion::fromWStringToString(e.GetMsg()).c_str();
        return false;
      }
    }

    //				// add string, double and handler to vector.
    //				// vectors keep tracks of new attributes and let muparser use explicit variables
    //				// it's possible to use custom attributes in other filters
    //				f_attrNames.push_back(name);
    //				f_attrValue.push_back(0);
    //				fhandlers.push_back(h);

    // if succeeded log stream contains number of vertices processed and time elapsed
    Log( "%d faces processed in %.2f sec.", m.cm.fn, (clock() - start) / (float) CLOCKS_PER_SEC);

    return true;
  }
    break;

  case FF_GRID :
  {
    // obtain parameters to generate 2D Grid
    int w = par.getInt("numVertX");
    int h = par.getInt("numVertY");
    float wl = par.getFloat("absScaleX");
    float hl = par.getFloat("absScaleY");

    if(w <= 0 || h <= 0) {
      errorMessage = "number of vertices must be positive";
      return false;
    }

    // use Grid function to generate Grid
    tri::Grid<CMeshO>(m.cm, w, h, wl, hl);

    // if "centered on origin" is checked than move generated Grid in (0,0,0)
    if(par.getBool("center"))
    {
      // move x and y
      Scalarm halfw = Scalarm(w-1)/2;
      Scalarm halfh = Scalarm(h-1)/2;
      Scalarm wld = wl/Scalarm(w);
      Scalarm hld = hl/Scalarm(h);

      for(auto vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi)
      {
        (*vi).P()[0] = (*vi).P()[0] - (wld * halfw);
        (*vi).P()[1] = (*vi).P()[1] - (hld * halfh);
      }
    }
    // update bounding box, normals
//    Matrix44m rot; rot.SetRotateDeg(180,Point3m(0,1,0));
    Matrix44m rot; rot.SetScale(-1,1,-1);
    tri::UpdatePosition<CMeshO>::Matrix(m.cm,rot,false);
    m.UpdateBoxAndNormals();
    return true;
  }
    break;
  case FF_ISOSURFACE :
  {
    SimpleVolume<SimpleVoxel <float > > 	volume;

    typedef vcg::tri::TrivialWalker<CMeshO, SimpleVolume<SimpleVoxel<float> > >	MyWalker;
    typedef vcg::tri::MarchingCubes<CMeshO, MyWalker>	MyMarchingCubes;
    MyWalker walker;

    Box3f RangeBBox;
    RangeBBox.min[0]=par.getFloat("minX");
    RangeBBox.min[1]=par.getFloat("minY");
    RangeBBox.min[2]=par.getFloat("minZ");
    RangeBBox.max[0]=par.getFloat("maxX");
    RangeBBox.max[1]=par.getFloat("maxY");
    RangeBBox.max[2]=par.getFloat("maxZ");
    double step=par.getFloat("voxelSize");
    Point3i siz= Point3i::Construct((RangeBBox.max-RangeBBox.min)*(1.0/step));

    Parser p;
    double x,y,z;
    p.DefineVar(conversion::fromStringToWString("x"), &x);
    p.DefineVar(conversion::fromStringToWString("y"), &y);
    p.DefineVar(conversion::fromStringToWString("z"), &z);
    std::string expr = par.getString("expr").toStdString();
    p.SetExpr(conversion::fromStringToWString(expr));
    Log("Filling a Volume of %i %i %i",siz[0],siz[1],siz[2]);
    volume.Init(siz,RangeBBox);
    for(double i=0;i<siz[0];i++)
      for(double j=0;j<siz[1];j++)
        for(double k=0;k<siz[2];k++)
        {
          x = RangeBBox.min[0]+step*i;
          y = RangeBBox.min[1]+step*j;
          z = RangeBBox.min[2]+step*k;
          try {
            volume.Val(i,j,k)=p.Eval();
          } catch(Parser::exception_type &e) {
            errorMessage = conversion::fromWStringToString(e.GetMsg()).c_str();
            return false;
          }
        }

    // MARCHING CUBES
    Log("[MARCHING CUBES] Building mesh...");
    MyMarchingCubes					mc(m.cm, walker);
    walker.BuildMesh<MyMarchingCubes>(m.cm, volume, mc, 0);
//    Matrix44m tr; tr.SetIdentity(); tr.SetTranslate(rbb.min[0],rbb.min[1],rbb.min[2]);
//    Matrix44m sc; sc.SetIdentity(); sc.SetScale(step,step,step);
//    tr=tr*sc;

//    tri::UpdatePosition<CMeshO>::Matrix(m.cm,tr);
    tri::UpdateNormal<CMeshO>::PerVertexNormalizedPerFace(m.cm);
    tri::UpdateBounding<CMeshO>::Box(m.cm);					// updates bounding box
    return true;

  }
    break;

  case FF_REFINE :
  {
    std::string condSelect = par.getString("condSelect").toStdString();

    std::string expr1 = par.getString("x").toStdString();
    std::string expr2 = par.getString("y").toStdString();
    std::string expr3 = par.getString("z").toStdString();

    bool errorMidPoint = false;
    bool errorEdgePred = false;
    std::string msg = "";

    // check parsing errors while creating two func obj
    // display error message
    MidPointCustom<CMeshO> mid = MidPointCustom<CMeshO>(m.cm,expr1,expr2,expr3,errorMidPoint,msg);
    CustomEdge<CMeshO> edge = CustomEdge<CMeshO>(condSelect,errorEdgePred,msg);
    if(errorMidPoint || errorEdgePred)
    {
      errorMessage = msg.c_str();
      return false;
    }

    // Refine current mesh.
    // Only edge specified with CustomEdge pred are selected
    //  and the new vertex is chosen with MidPointCustom created above
    vcg::tri::RefineE<CMeshO, MidPointCustom<CMeshO>, CustomEdge<CMeshO> >
        (m.cm, mid, edge, false, cb);
    m.UpdateBoxAndNormals();
    m.clearDataMask( MeshModel::MM_VERTMARK);
    //vcg::tri::UpdateNormal<CMeshO>::PerVertexNormalizedPerFace(m.cm);

    return true;
  }
    break;

  default : assert (0);
  }
  return false;
}

// display parsing error in dialog
void FilterFunctionPlugin::showParserError(const QString &s, Parser::exception_type &e)
{
  errorMessage += s;
  errorMessage += conversion::fromWStringToString(e.GetMsg()).c_str();
  errorMessage += "\n";
}

// set per-vertex attributes associated to parser variables
void FilterFunctionPlugin::setAttributes(CMeshO::VertexIterator &vi, CMeshO &m)
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
  a = (*vi).C()[3];  // color ALPHA

  q = (*vi).Q();     // quality

  vsel = ((*vi).IsS()) ? 1.0 : 0.0;    //selection

  if(tri::HasPerVertexRadius(m)) rad = (*vi).R();
  else rad=0;

  v = vi - m.vert.begin(); // zero based index of current vertex

  if(tri::HasPerVertexTexCoord(m))
  {
    vtu=(*vi).T().U();
    vtv=(*vi).T().V();
	ti = (*vi).T().N();
  }
  else { vtu=vtv=ti=0; }

  // if user-defined attributes exist (vector is not empty)
  //  set variables to explicit value obtained through attribute's handler
  for(int i = 0; i < (int) v_attrValue.size(); i++)
    v_attrValue[i] = v_handlers[i][vi];

  for(int i = 0; i < (int) v3_handlers.size(); i++)
  {
    v3_attrValue[i*3+0] = v3_handlers[i][vi].X();
    v3_attrValue[i*3+1] = v3_handlers[i][vi].Y();
    v3_attrValue[i*3+2] = v3_handlers[i][vi].Z();
  }
}

// set per-face attributes associated to parser variables
void FilterFunctionPlugin::setAttributes(CMeshO::FaceIterator &fi, CMeshO &m)
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

	if(HasPerFaceQuality(m))
		fq=(*fi).Q();
	else fq=0;

    // set face color attributes
    if(HasPerFaceColor(m)){
        fr = (*fi).C()[0];
        fg = (*fi).C()[1];
        fb = (*fi).C()[2];
		fa = (*fi).C()[3];
    } else {
        fr=fg=fb=fa=255;
    }

	//face normal
	fnx = (*fi).N()[0];
	fny = (*fi).N()[1];
	fnz = (*fi).N()[2];

    // zero based index of face
    f = fi - m.face.begin();

    // zero based index of its vertices
    v0i = ((*fi).V(0) - &m.vert[0]);
    v1i = ((*fi).V(1) - &m.vert[0]);
    v2i = ((*fi).V(2) - &m.vert[0]);

	if(tri::HasPerWedgeTexCoord(m))
	{
		wtu0=(*fi).WT(0).U();
		wtv0=(*fi).WT(0).V();
		wtu1=(*fi).WT(1).U();
		wtv1=(*fi).WT(1).V();
		wtu2=(*fi).WT(2).U();
		wtv2=(*fi).WT(2).V();
		ti = (*fi).WT(0).N();
	}
	else { wtu0=wtv0=wtu1=wtv1=wtu2=wtv2=ti=0; }

	//selection
	vsel0 = ((*fi).V(0)->IsS()) ? 1.0 : 0.0;
	vsel1 = ((*fi).V(1)->IsS()) ? 1.0 : 0.0;
	vsel2 = ((*fi).V(2)->IsS()) ? 1.0 : 0.0;
	fsel = ((*fi).IsS()) ? 1.0 : 0.0;

    // if user-defined attributes exist (vector is not empty)
    //  set variables to explicit value obtained through attribute's handler
    for(int i = 0; i < (int) f_attrValue.size(); i++)
    f_attrValue[i] = f_handlers[i][fi];
}

// Function explicitly define parser variables to perform per-vertex filter action
// x, y, z for vertex coord, nx, ny, nz for normal coord, r, g ,b for color
// and q for quality
void FilterFunctionPlugin::setPerVertexVariables(Parser &p, CMeshO &m)
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
	p.DefineVar(conversion::fromStringToWString("vi"),&v);
	p.DefineVar(conversion::fromStringToWString("rad"),&rad);
	p.DefineVar(conversion::fromStringToWString("vtu"),&vtu);
	p.DefineVar(conversion::fromStringToWString("vtv"),&vtv);
	p.DefineVar(conversion::fromStringToWString("ti"), &ti);
	p.DefineVar(conversion::fromStringToWString("vsel"), &vsel);

    // define var for user-defined attributes (if any exists)
    // if vector is empty, code won't be executed
	v_handlers.clear();
	v_attrNames.clear();
	v_attrValue.clear();
	v3_handlers.clear();
	v3_attrNames.clear();
	v3_attrValue.clear();
	std::vector<std::string> AllVertexAttribName;
	tri::Allocator<CMeshO>::GetAllPerVertexAttribute< float >(m,AllVertexAttribName);
	for(int i = 0; i < (int) AllVertexAttribName.size(); i++)
	{
		CMeshO::PerVertexAttributeHandle<float> hh = tri::Allocator<CMeshO>::GetPerVertexAttribute<float>(m, AllVertexAttribName[i]);
		v_handlers.push_back(hh);
		v_attrNames.push_back(AllVertexAttribName[i]);
		v_attrValue.push_back(0);
		p.DefineVar(conversion::fromStringToWString(v_attrNames.back()), &v_attrValue.back());
		qDebug("Adding custom per vertex float variable %s",v_attrNames.back().c_str());
	}
	AllVertexAttribName.clear();
	tri::Allocator<CMeshO>::GetAllPerVertexAttribute< Point3f >(m,AllVertexAttribName);
	for(int i = 0; i < (int) AllVertexAttribName.size(); i++)
	{
		CMeshO::PerVertexAttributeHandle<Point3f> hh3 = tri::Allocator<CMeshO>::GetPerVertexAttribute<Point3f>(m, AllVertexAttribName[i]);

		v3_handlers.push_back(hh3);

		v3_attrValue.push_back(0);
		v3_attrNames.push_back(AllVertexAttribName[i]+"_x");
		p.DefineVar(conversion::fromStringToWString(v3_attrNames.back()), &v3_attrValue.back());

		v3_attrValue.push_back(0);
		v3_attrNames.push_back(AllVertexAttribName[i]+"_y");
		p.DefineVar(conversion::fromStringToWString(v3_attrNames.back()), &v3_attrValue.back());

		v3_attrValue.push_back(0);
		v3_attrNames.push_back(AllVertexAttribName[i]+"_z");
		p.DefineVar(conversion::fromStringToWString(v3_attrNames.back()), &v3_attrValue.back());
		qDebug("Adding custom per vertex Point3f variable %s",v3_attrNames.back().c_str());
	}
}


// Function explicitly define parser variables to perform Per-Face filter action
void FilterFunctionPlugin::setPerFaceVariables(Parser &p, CMeshO &m)
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
    p.DefineVar(conversion::fromStringToWString("fi"),&f);
    p.DefineVar(conversion::fromStringToWString("vi0"),&v0i);
    p.DefineVar(conversion::fromStringToWString("vi1"),&v1i);
    p.DefineVar(conversion::fromStringToWString("vi2"),&v2i);

	// texture
	p.DefineVar(conversion::fromStringToWString("wtu0"),&wtu0);
	p.DefineVar(conversion::fromStringToWString("wtv0"),&wtv0);
	p.DefineVar(conversion::fromStringToWString("wtu1"),&wtu1);
	p.DefineVar(conversion::fromStringToWString("wtv1"),&wtv1);
	p.DefineVar(conversion::fromStringToWString("wtu2"),&wtu2);
	p.DefineVar(conversion::fromStringToWString("wtv2"),&wtv2);
	p.DefineVar(conversion::fromStringToWString("ti"), &ti);

	//selection
	p.DefineVar(conversion::fromStringToWString("vsel0"), &vsel0);
	p.DefineVar(conversion::fromStringToWString("vsel1"), &vsel1);
	p.DefineVar(conversion::fromStringToWString("vsel2"), &vsel2);
	p.DefineVar(conversion::fromStringToWString("fsel"), &fsel);

    // define var for user-defined attributes (if any exists)
    // if vector is empty, code won't be executed
	std::vector<std::string> AllFaceAttribName;
	tri::Allocator<CMeshO>::GetAllPerFaceAttribute< float >(m,AllFaceAttribName);
	f_handlers.clear();
	f_attrNames.clear();
	f_attrValue.clear();
	for(int i = 0; i < (int) AllFaceAttribName.size(); i++)
	{
		CMeshO::PerFaceAttributeHandle<float> hh = tri::Allocator<CMeshO>::GetPerFaceAttribute<float>(m, AllFaceAttribName[i]);
		f_handlers.push_back(hh);
		f_attrNames.push_back(AllFaceAttribName[i]);
		f_attrValue.push_back(0);
		p.DefineVar(conversion::fromStringToWString(f_attrNames.back()), &f_attrValue.back());
	}

}

MeshFilterInterface::FILTER_ARITY FilterFunctionPlugin::filterArity( QAction* filter ) const
{
    switch(ID(filter)) 
    {
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
    case FF_DEF_VERT_ATTRIB:    
    case FF_DEF_FACE_ATTRIB:    
    case FF_REFINE:
        return MeshFilterInterface::SINGLE_MESH;
    case FF_GRID:                            
    case FF_ISOSURFACE:         
        return MeshFilterInterface::NONE;
    }
    return MeshFilterInterface::NONE;
}


MESHLAB_PLUGIN_NAME_EXPORTER(FilterFunctionPlugin)
