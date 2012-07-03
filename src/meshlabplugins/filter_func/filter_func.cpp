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

using namespace mu;
using namespace vcg;

// Constructor
FilterFunctionPlugin::FilterFunctionPlugin() 
{ 
	typeList 
		<< FF_VERT_SELECTION
		<< FF_FACE_SELECTION
		<< FF_GEOM_FUNC
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

// short string describing each filtering action 
 QString FilterFunctionPlugin::filterName(FilterIDType filterId) const
{
	switch(filterId) {
		case FF_VERT_SELECTION :  return QString("Conditional Vertex Selection");
		case FF_FACE_SELECTION : return QString("Conditional Face Selection");
		case FF_GEOM_FUNC :  return QString("Geometric Function");
    case FF_FACE_COLOR : return QString("Per Face Color Function");
    case FF_FACE_QUALITY : return QString("Per Face Quality Function");
    case FF_VERT_COLOR : return QString("Per Vertex Color Function");
    case FF_VERT_QUALITY : return QString("Per Vertex Quality Function");
    case FF_VERT_NORMAL : return QString("Per Vertex Normal Function");
    case FF_DEF_VERT_ATTRIB : return QString("Define New Per Vertex Attribute");
    case FF_DEF_FACE_ATTRIB : return QString("Define New Per Face Attribute");
		case FF_GRID : return QString("Grid Generator");
		case FF_REFINE : return QString("Refine User-Defined");
		case FF_ISOSURFACE : return QString("Implicit Surface");
		default : assert(0); 
	}
	return QString("error!");
}
const QString PerVertexAttributeString("It's possibile to use the following per-vertex variables in the expression:<br>"
                                       "x, y, z, nx, ny, nz (normal), r, g, b (color), q (quality), rad, vi (index), vtu, vtv (tex coord ) <br>"
																			 "and all custom <i>vertex attributes</i> already defined by user.<br>");

const QString PerFaceAttributeString("It's possibile to use per-face variables like attributes associated to the three vertex of every face.<br>"
																		"<b>x0,y0,z0</b> for <b>first vertex</b>; x1,y1,z1 for second vertex; x2,y2,z2 for third vertex.<br>"
																		"and also <b>nx0,ny0,nz0</b> nx1,ny1,nz1 etc. for <b>normals</b> and <b>r0,g0,b0</b> for <b>color</b>,"
                                    "<b>q0,q1,q2</b> for <b>quality</b> wtu0, wtv0, wtu1, wtv1,wtu2, wtv2 (per wedge tex coord ).<br>");

// long string describing each filtering action 
 QString FilterFunctionPlugin::filterInfo(FilterIDType filterId) const
{
	switch(filterId) {
		case FF_VERT_SELECTION : return tr("Boolean function using muparser lib to perform vertex selection over current mesh.<br>"
										   "It's possibile to use parenthesis, per-vertex variables and boolean operator:<br>"
										   "<b>(</b>,<b>)</b>,<b>and</b>,<b>or</b>,<b><</b><b>></b>,<b>=</b><br>")
											 +PerVertexAttributeString; 

		case FF_FACE_SELECTION : return tr("Boolean function using muparser lib to perform faces selection over current mesh.<br>"
																			 "It's possibile to use parenthesis, per-vertex variables and boolean operator:<br>"
																			 "<b>(</b>,<b>)</b>,<b>and</b>,<b>or</b>,<b><</b><b>></b>,<b>=</b><br>")+PerFaceAttributeString;   

		case FF_GEOM_FUNC :  return tr("Geometric function using muparser lib to generate new Coord<br>"
									   "You can change x,y,z for every vertex according to the function specified.<br>")
											+PerVertexAttributeString;

		case FF_FACE_COLOR : return tr("Color function using muparser lib to generate new RGB color for every face<br>"
									   "Insert three function each one for red, green and blue channel respectively.<br>")+PerFaceAttributeString;   
		case FF_VERT_COLOR : return tr("Color function using muparser lib to generate new RGB color for every vertex<br>"
									   "Insert three function each one for red, green and blue channel respectively.<br>")
											+PerVertexAttributeString;

  case FF_VERT_QUALITY : return tr("Quality function using muparser to generate new Quality for every vertex<br>")
              +PerVertexAttributeString;
  case FF_VERT_NORMAL : return tr("Normal function using muparser to generate new Normal for every vertex<br>")
              +PerVertexAttributeString;

		case FF_FACE_QUALITY : return tr("Quality function using muparser to generate new Quality for every face<br>"
										 "Insert three function each one for quality of the three vertex of a face<br>")+PerFaceAttributeString;   
		case FF_DEF_VERT_ATTRIB : return tr("Add a new Per-Vertex scalar attribute to current mesh and fill it with the defined function.<br>"
								       "The name specified below can be used in other filter function")+PerVertexAttributeString;

		case FF_DEF_FACE_ATTRIB : return tr("Add a new Per-Face attribute to current mesh.<br>"
									   "You can specify custom name and a function to generate attribute's value<br>"
                     "It's possible to use per-face variables in the expression:<br>")+PerFaceAttributeString+
                     tr("<font color=\"#FF0000\">The attribute name specified below can be used in other filter function</font>");

		case FF_GRID : return tr("Generate a new 2D Grid mesh with number of vertices on X and Y axis specified by user with absolute length/height.<br>"
						         "It's possible to center Grid on origin.");

		case FF_ISOSURFACE : return tr("Generate a new mesh that corresponds to the 0 valued isosurface defined by the scalar field generated by the given expression");

		case FF_REFINE : return tr("Refine current mesh with user defined parameters.<br>"
							       "Specify a Boolean Function needed to select which edges will be cut for refinement purpose.<br>"
								   "Each edge is identified with first and second vertex.<br>"
                   "Arguments accepted are first and second vertex attributes:<br>")+PerVertexAttributeString;

		default : assert(0); 
	}
	return QString("filter not found!");
}

 FilterFunctionPlugin::FilterClass FilterFunctionPlugin::getClass(QAction *a)
{
  switch(ID(a))
  {
    case FF_FACE_SELECTION : 
		case FF_VERT_SELECTION	: return MeshFilterInterface::Selection;
		case FF_FACE_QUALITY		: return MeshFilterInterface::Quality;
		case FF_VERT_QUALITY		: return MeshFilterInterface::Quality;
		case FF_VERT_COLOR			:	return MeshFilterInterface::VertexColoring;
    case FF_VERT_NORMAL			:	return MeshFilterInterface::Normal;
		case FF_FACE_COLOR			: return MeshFilterInterface::FaceColoring;
		case FF_ISOSURFACE			: return MeshFilterInterface::MeshCreation;
		case FF_GRID						: return MeshFilterInterface::MeshCreation;
		case FF_REFINE					: return MeshFilterInterface::Remeshing; 
		case FF_GEOM_FUNC				: return MeshFilterInterface::Smoothing;
		case FF_DEF_VERT_ATTRIB	: return MeshFilterInterface::Layer;
		case FF_DEF_FACE_ATTRIB	: return MeshFilterInterface::Layer; 
		
		default			  : return MeshFilterInterface::Generic;
  }
}
 int FilterFunctionPlugin::postCondition(QAction *action) const
 {
   switch(ID(action))
   {
   case FF_VERT_SELECTION :
   case FF_FACE_SELECTION :
     return MeshModel::MM_VERTFLAGSELECT | MeshModel::MM_FACEFLAGSELECT;
   case FF_FACE_COLOR		:
     return MeshModel::MM_FACECOLOR;
   case FF_GEOM_FUNC :
     return MeshModel::MM_VERTCOORD + MeshModel::MM_VERTNORMAL + MeshModel::MM_FACENORMAL;
   case FF_VERT_COLOR :
     return MeshModel::MM_VERTCOLOR;
   case FF_VERT_NORMAL :
       return MeshModel::MM_VERTNORMAL;
   case FF_VERT_QUALITY :
     return MeshModel::MM_VERTQUALITY+MeshModel::MM_VERTCOLOR;
   case FF_FACE_QUALITY  :
     return MeshModel::MM_FACECOLOR + MeshModel::MM_FACEQUALITY;
   case FF_DEF_VERT_ATTRIB :
   case FF_GRID :
   case FF_ISOSURFACE :
   case FF_DEF_FACE_ATTRIB :
   case FF_REFINE :
     return MeshModel::MM_UNKNOWN;
   }
   return MeshModel::MM_UNKNOWN;
 }
 int FilterFunctionPlugin::getRequirements(QAction *action)
{
  switch(ID(action))
  {
    case FF_VERT_SELECTION :
		case FF_GEOM_FUNC :
    case FF_VERT_COLOR :
    case FF_VERT_NORMAL :
    case FF_VERT_QUALITY :
		case FF_DEF_VERT_ATTRIB :
		case FF_GRID : 
		case FF_ISOSURFACE : 
		case FF_DEF_FACE_ATTRIB :
		case FF_FACE_SELECTION : return 0;
		case FF_FACE_QUALITY  : return MeshModel::MM_FACECOLOR + MeshModel::MM_FACEQUALITY;
    case FF_FACE_COLOR		: return MeshModel::MM_FACECOLOR;
		case FF_REFINE : 
		return MeshModel::MM_FACEFACETOPO | MeshModel::MM_VERTMARK;
    default: assert(0);
  }
  return 0;
}

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parmeter you need to define, 
// - the name of the parameter, 
// - the string shown in the dialog 
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void FilterFunctionPlugin::initParameterSet(QAction *action,MeshModel &m, RichParameterSet & parlst) 
{
	Q_UNUSED(m);
	switch(ID(action))	 {

		case FF_VERT_SELECTION :
			parlst.addParam(new RichString("condSelect","(q < 0)", "boolean function",
							 "type a boolean function that will be evaluated in order to select a subset of vertices<br>"
							 "example: (y > 0) and (ny > 0)"));

			parlst.addParam(new RichBool("strictSelect",true,"Strict face selection", "If checked a face is selected if <b>ALL</b> its vertices are selected. <br>"
										 "If unchecked a face is selected if <b>at least one</b> of its vertices is selected"));
			break;

		case FF_FACE_SELECTION :
			parlst.addParam(new RichString("condSelect","(fi == 0)", "boolean function",
							 "type a boolean function that will be evaluated in order to select a subset of faces<br>"));
			break;

  case FF_GEOM_FUNC :
    parlst.addParam(new RichString("x","x", "func x = ", "insert function to generate new coord for x"));
    parlst.addParam(new RichString("y","y", "func y = ", "insert function to generate new coord for y"));
    parlst.addParam(new RichString("z","sin(x+y)", "func z = ", "insert function to generate new coord for z"));
    break;

  case FF_VERT_NORMAL :
    parlst.addParam(new RichString("x","-nx", "func x = ", "insert function to generate new x for the normal"));
    parlst.addParam(new RichString("y","-ny", "func y = ", "insert function to generate new y for the normal"));
    parlst.addParam(new RichString("z","-nz", "func z = ", "insert function to generate new z for the normal"));
    break;

		case FF_FACE_COLOR:
			parlst.addParam(new RichString("r","255", "func r = ", "function to generate Red component. Expected Range 0-255"));
			parlst.addParam(new RichString("g","0", "func g = ", "function to generate Green component. Expected Range 0-255"));
			parlst.addParam(new RichString("b","255", "func b = ", "function to generate Blue component. Expected Range 0-255"));
      parlst.addParam(new RichString("a","255", "func alpha = ", "function to generate Alpha component. Expected Range 0-255"));
			break;

		case FF_FACE_QUALITY:
			parlst.addParam(new RichString("q","x0+y0+z0", "func q0 = ", "function to generate new Quality foreach face"));
			parlst.addParam(new RichBool("normalize",false,"normalize","if checked normalize all quality values in range [0..1]"));
			parlst.addParam(new RichBool("map",false,"map into color", "if checked map quality generated values into per-vertex color"));
			break;

		case FF_VERT_COLOR:
			parlst.addParam(new RichString("x","255", "func r = ", "function to generate Red component. Expected Range 0-255"));
			parlst.addParam(new RichString("y","255", "func g = ", "function to generate Green component. Expected Range 0-255"));
			parlst.addParam(new RichString("z","0", "func b = ", "function to generate Blue component. Expected Range 0-255"));
      parlst.addParam(new RichString("a","255", "func alpha = ", "function to generate Alpha component. Expected Range 0-255"));
      break;

		case FF_VERT_QUALITY:
			parlst.addParam(new RichString("q","vi", "func q = ", "function to generate new Quality for every vertex"));
			parlst.addParam(new RichBool("normalize",false,"normalize","if checked normalize all quality values in range [0..1]"));
			parlst.addParam(new RichBool("map",false,"map into color", "if checked map quality generated values into per-vertex color"));
			break;

		case FF_DEF_VERT_ATTRIB:
			parlst.addParam(new RichString("name","Radiosity","Name", "the name of new attribute. you can access attribute in other filters through this name"));
			parlst.addParam(new RichString("expr","x","Function =", "function to calculate custom attribute value for each vertex"));
			break;

		case FF_DEF_FACE_ATTRIB:
			parlst.addParam(new RichString("name","Radiosity","Name", "the name of new attribute. you can access attribute in other filters through this name"));
			parlst.addParam(new RichString("expr","fi","Function =", "function to calculate custom attribute value for each vertex"));
			break;

		case FF_GRID :
			parlst.addParam(new RichInt("numVertX", 10, "num vertices on x", "number of vertices on x. it must be positive"));
			parlst.addParam(new RichInt("numVertY", 10, "num vertices on y", "number of vertices on y. it must be positive"));
			parlst.addParam(new RichFloat("absScaleX", 0.3f, "x scale", "absolute scale on x (float)"));
			parlst.addParam(new RichFloat("absScaleY", 0.3f, "y scale", "absolute scale on y (float)"));
			parlst.addParam(new RichBool("center",false,"centered on origin", "center grid generated by filter on origin.<br>"
						   "Grid is first generated and than moved into origin (using muparser lib to perform fast calc on every vertex)"));
			break;
		case FF_ISOSURFACE :
			parlst.addParam(new RichFloat("voxelSize", 0.05, "Size of Voxel", "Size of the voxel that is used by for the grid where the field is sampled. Smaller this value, higher precision, but higher processing times."));
			parlst.addParam(new RichFloat("minX", -1, "Min X", "Range where the field is sampled"));
			parlst.addParam(new RichFloat("minY", -1, "Min Y", "Range where the field is sampled"));
			parlst.addParam(new RichFloat("minZ", -1, "Min Z", "Range where the field is sampled"));
			parlst.addParam(new RichFloat("maxX",  1, "Max X", "Range where the field is sampled"));
			parlst.addParam(new RichFloat("maxY",  1, "Max Y", "Range where the field is sampled"));
			parlst.addParam(new RichFloat("maxZ",  1, "Max Z", "Range where the field is sampled"));
			parlst.addParam(new RichString("expr","x*x+y*y+z*z-0.5","Function =", "This expression is evaluated for each voxel of the grid. The surface passing through the zero valued points of this field is then extracted using marching cube."));

			break;

		case FF_REFINE :
			parlst.addParam(new RichString("condSelect","(q0 >= 0 and q1 >= 0)","boolean function","type a boolean function that will be evaluated on every edge"));
			parlst.addParam(new RichString("x","(x0+x1)/2","x =","function to generate x coord of new vertex in [x0,x1].<br>For example (x0+x1)/2"));
			parlst.addParam(new RichString("y","(y0+y1)/2","y =","function to generate x coord of new vertex in [y0,y1].<br>For example (y0+y1)/2"));
			parlst.addParam(new RichString("z","(z0+z1)/2","z =","function to generate x coord of new vertex in [z0,z1].<br>For example (z0+z1)/2"));
			break;

    default: break; // do not add any parameter for the other filters
  }
}

// The Real Core Function doing the actual mesh processing.
bool FilterFunctionPlugin::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb)
{
  if(this->getClass(filter) == MeshFilterInterface::MeshCreation)
       md.addNewMesh("",this->filterName(ID(filter)));
  MeshModel &m=*(md.mm());
	Q_UNUSED(cb);
	switch(ID(filter)) {
		case FF_VERT_SELECTION :
			{
				std::string expr = par.getString("condSelect").toStdString();
			
				// muparser initialization and explicitely define parser variables
				Parser p;
        setPerVertexVariables(p,m.cm);

				// set expression inserted by user as string (required by muparser)
				p.SetExpr(expr);

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
						errorMessage = e.GetMsg().c_str();
  						return false;
					}

					// set vertex as selected or clear selection
					if(selected) { 
						(*vi).SetS();
						numvert++;
					} else (*vi).ClearS();
				}

				// strict face selection
				if(par.getBool("strictSelect"))
						tri::UpdateSelection<CMeshO>::FaceFromVertexStrict(m.cm); 
				else  
						tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(m.cm); 
		
				// if succeded log stream contains number of vertices and time elapsed
				Log( "selected %d vertices in %.2f sec.", numvert, (clock() - start) / (float) CLOCKS_PER_SEC);
					
				return true;
			}
		break;

		case FF_FACE_SELECTION :
			{
				QString select = par.getString("condSelect");
			
				// muparser initialization and explicitely define parser variables
				Parser p;
        setPerFaceVariables(p,m.cm);

				// set expression inserted by user as string (required by muparser)
				p.SetExpr(select.toStdString());

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
						errorMessage = e.GetMsg().c_str();
  						return false;
					}

					// set face as selected or clear selection
					if(selected) { 
						(*fi).SetS();
						numface++;
					} else (*fi).ClearS();
				}

				// if succeded log stream contains number of vertices and time elapsed
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

				// muparser initialization and explicitely define parser variables
				// function for x,y and z must use different parser and variables
        Parser p1,p2,p3,p4;

        setPerVertexVariables(p1,m.cm);
        setPerVertexVariables(p2,m.cm);
        setPerVertexVariables(p3,m.cm);
        setPerVertexVariables(p4,m.cm);

				p1.SetExpr(func_x);
				p2.SetExpr(func_y);
				p3.SetExpr(func_z);
        p4.SetExpr(func_a);

        double newx=0,newy=0,newz=0,newa=255;
				errorMessage = "";

				time_t start = clock();

				// every parser variables is related to vertex coord and attributes.
				CMeshO::VertexIterator vi;
				for(vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi)if(!(*vi).IsD())
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

          if(ID(filter) == FF_GEOM_FUNC)  // set new vertex coord for this iteration
						(*vi).P() = Point3f(newx,newy,newz); 
          if(ID(filter) == FF_VERT_COLOR) // set new color for this iteration
            (*vi).C() = Color4b(newx,newy,newz,newa);
          if(ID(filter) == FF_VERT_NORMAL) // set new color for this iteration
            (*vi).N() = Point3f(newx,newy,newz);
          }

				if(ID(filter) == FF_GEOM_FUNC) {
					// update bounding box, normalize normals
					tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
					tri::UpdateNormals<CMeshO>::NormalizeFace(m.cm);
					tri::UpdateBounding<CMeshO>::Box(m.cm);
				}

				// if succeded log stream contains number of vertices processed and time elapsed
				Log( "%d vertices processed in %.2f sec.", m.cm.vn, (clock() - start) / (float) CLOCKS_PER_SEC);

				return true;
			}
		break;
		
		case FF_VERT_QUALITY:
			{
				std::string func_q = par.getString("q").toStdString();
				m.updateDataMask(MeshModel::MM_VERTQUALITY);
				
				// muparser initialization and define custom variables
				Parser p;
        setPerVertexVariables(p,m.cm);

				// set expression to calc with parser
				p.SetExpr(func_q);

				// every parser variables is related to vertex coord and attributes.
				time_t start = clock();
				CMeshO::VertexIterator vi;
        for(vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi)
        if(!(*vi).IsD())
				{
					setAttributes(vi,m.cm);

					// use parser to evaluate function specified above
					// in case of fail, errorMessage dialog contains details of parser's error
					try { 
						(*vi).Q() = p.Eval();
					} catch(Parser::exception_type &e) {
						errorMessage = e.GetMsg().c_str();
  						return false;
					}
				}

				// normalize quality with values in [0..1] 
        if(par.getBool("normalize")) tri::UpdateQuality<CMeshO>::VertexNormalize(m.cm);
				
				// map quality into per-vertex color
        if(par.getBool("map")) tri::UpdateColor<CMeshO>::VertexQualityRamp(m.cm);

				// if succeded log stream contains number of vertices and time elapsed
				Log( "%d vertices processed in %.2f sec.", m.cm.vn, (clock() - start) / (float) CLOCKS_PER_SEC);

				return true;
			}
		break;

		case FF_FACE_COLOR:
			{
				std::string func_r = par.getString("r").toStdString();
				std::string func_g = par.getString("g").toStdString();
				std::string func_b = par.getString("b").toStdString();
        std::string func_a = par.getString("a").toStdString();

				// muparser initialization and explicitely define parser variables
				// every function must uses own parser and variables
        Parser p1,p2,p3,p4;

        setPerFaceVariables(p1,m.cm);
        setPerFaceVariables(p2,m.cm);
        setPerFaceVariables(p3,m.cm);
        setPerFaceVariables(p4,m.cm);

				p1.SetExpr(func_r);
				p2.SetExpr(func_g);
				p3.SetExpr(func_b);
        p4.SetExpr(func_a);

				// RGB is related to every face
				CMeshO::FaceIterator fi;
        double newr=0,newg=0,newb=0,newa=255;
				errorMessage = "";

				time_t start = clock();

				// every parser variables is related to face attributes.
				for(fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)if(!(*fi).IsD())
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

				// if succeded log stream contains number of vertices processed and time elapsed
				Log( "%d faces processed in %.2f sec.", m.cm.fn, (clock() - start) / (float) CLOCKS_PER_SEC);

				return true;

			}
		break;

		case FF_FACE_QUALITY:
			{
				std::string func_q = par.getString("q").toStdString();
				m.updateDataMask(MeshModel::MM_FACEQUALITY);

				// muparser initialization and define custom variables
				Parser pf;
        setPerFaceVariables(pf,m.cm);
					
				// set expression to calc with parser
          pf.SetExpr(func_q);

				time_t start = clock();
				errorMessage = "";

				// every parser variables is related to face attributes.
				CMeshO::FaceIterator fi;
				for(fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)if(!(*fi).IsD())
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
        if(par.getBool("map")) tri::UpdateColor<CMeshO>::FaceQualityRamp(m.cm);

				// if succeded log stream contains number of faces processed and time elapsed
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
          h = tri::Allocator<CMeshO>::GetPerVertexAttribute<float>(m.cm, name);
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
        qDebug("Now mesh has %i vertex float attribute",AllVertexAttribName.size());
				Parser p;
        setPerVertexVariables(p,m.cm);
				p.SetExpr(expr);

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
						errorMessage = e.GetMsg().c_str();
  						return false;
					}
				}

				// add string, double and handler to vector.
				// vectors keep tracks of new attributes and let muparser use explicit variables
				// it's possibile to use custom attributes in other filters
				v_attrNames.push_back(name);
				v_attrValue.push_back(0);
        v_handlers.push_back(h);

				// if succeded log stream contains number of vertices processed and time elapsed
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
          h = tri::Allocator<CMeshO>::GetPerFaceAttribute<float>(m.cm, name);
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
				p.SetExpr(expr);

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
						errorMessage = e.GetMsg().c_str();
  						return false;
					}
				}

//				// add string, double and handler to vector.
//				// vectors keep tracks of new attributes and let muparser use explicit variables
//				// it's possibile to use custom attributes in other filters
//				f_attrNames.push_back(name);
//				f_attrValue.push_back(0);
//				fhandlers.push_back(h);

				// if succeded log stream contains number of vertices processed and time elapsed
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
				std::vector<float> data(w*h,0);
				tri::Grid<CMeshO>(m.cm, w, h, wl, hl, &data[0]);

				// if "centered on origin" is checked than move generated Grid in (0,0,0)
				if(par.getBool("center")) 
				{
					// move x and y
					double halfw = double(w-1)/2;
					double halfh = double(h-1)/2;
					double wld = wl/double(w);
					double hld = hl/float(h);

					CMeshO::VertexIterator vi;
					for(vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi)
					{
						(*vi).P()[0] = (*vi).P()[0] - (wld * halfw);
						(*vi).P()[1] = (*vi).P()[1] - (hld * halfh);
					}
				}
				// update bounding box, normals
        Matrix44f rot; rot.SetRotateDeg(180,Point3f(0,1,0));
        tri::UpdatePosition<CMeshO>::Matrix(m.cm,rot,false);
				tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);	
				tri::UpdateNormals<CMeshO>::NormalizeFace(m.cm);
				tri::UpdateBounding<CMeshO>::Box(m.cm);
				
				return true;
			}
		break;
		case FF_ISOSURFACE :
			{
					SimpleVolume<SimpleVoxel> 	volume;
		
					typedef vcg::tri::TrivialWalker<CMeshO, SimpleVolume<SimpleVoxel> >	MyWalker;
					typedef vcg::tri::MarchingCubes<CMeshO, MyWalker>	MyMarchingCubes;
					MyWalker walker;
		
					Box3d rbb;
					rbb.min[0]=par.getFloat("minX");
					rbb.min[1]=par.getFloat("minY");
					rbb.min[2]=par.getFloat("minZ");
					rbb.max[0]=par.getFloat("maxX");
					rbb.max[1]=par.getFloat("maxY");
					rbb.max[2]=par.getFloat("maxZ");
					double step=par.getFloat("voxelSize");
					Point3i siz= Point3i::Construct((rbb.max-rbb.min)*(1.0/step));
					
					Parser p;
					double x,y,z;
					p.DefineVar("x", &x);
					p.DefineVar("y", &y);
					p.DefineVar("z", &z);
					std::string expr = par.getString("expr").toStdString();
					p.SetExpr(expr);
					Log("Filling a Volume of %i %i %i",siz[0],siz[1],siz[2]);
					volume.Init(siz);
					for(double i=0;i<siz[0];i++)
						for(double j=0;j<siz[1];j++)
							for(double k=0;k<siz[2];k++)
							{
							 x = rbb.min[0]+step*i;
							 y = rbb.min[1]+step*j;
							 z = rbb.min[2]+step*k;
							 	try {
										volume.Val(i,j,k)=p.Eval();
										} catch(Parser::exception_type &e) {
												errorMessage = e.GetMsg().c_str();
												return false;
											}
							}
		
		// MARCHING CUBES
		Log("[MARCHING CUBES] Building mesh...");
		MyMarchingCubes					mc(m.cm, walker);
		walker.BuildMesh<MyMarchingCubes>(m.cm, volume, mc, 0);
		Matrix44f tr; tr.SetIdentity(); tr.SetTranslate(rbb.min[0],rbb.min[1],rbb.min[2]);
		Matrix44f sc; sc.SetIdentity(); sc.SetScale(step,step,step);
		tr=tr*sc;
		
		tri::UpdatePosition<CMeshO>::Matrix(m.cm,tr);
		tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);																																			 
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
				//  and the new vertex is choosen with MidPointCustom created above
				RefineE<CMeshO, MidPointCustom<CMeshO>, CustomEdge<CMeshO> >
					(m.cm, mid, edge, false, cb);

				m.clearDataMask( MeshModel::MM_VERTMARK);
				vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);

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
	errorMessage += e.GetMsg().c_str();
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

	q = (*vi).Q();     // quality
	
	if(tri::HasPerVertexRadius(m)) rad = (*vi).R();
		 else rad=0;

	v = vi - m.vert.begin(); // zero based index of current vertex

  if(tri::HasPerVertexTexCoord(m))
  {
    vtu=(*vi).T().U();
    vtv=(*vi).T().V();
  }
  else { vtu=vtv=0; }

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

	q0 = (*fi).V(0)->Q();
	q1 = (*fi).V(1)->Q();
	q2 = (*fi).V(2)->Q();

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
	
  if(HasPerFaceQuality(m))
         fq=(*fi).Q();
    else fq=0;

	// set face color attributes
	if(HasPerFaceColor(m)){
		r = (*fi).C()[0];
		g = (*fi).C()[1];
		b = (*fi).C()[2];
	} else {
		r=g=b=255;
	}
	
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
  }
  else { wtu0=wtv0=wtu1=wtv1=wtu2=wtv2=0; }


	// if user-defined attributes exist (vector is not empty) 
	//  set variables to explicit value obtained through attribute's handler
	for(int i = 0; i < (int) f_attrValue.size(); i++)
    f_attrValue[i] = f_handlers[i][fi];
}

// Function explicitely define parser variables to perform per-vertex filter action
// x, y, z for vertex coord, nx, ny, nz for normal coord, r, g ,b for color
// and q for quality 
void FilterFunctionPlugin::setPerVertexVariables(Parser &p, CMeshO &m)
{
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
	p.DefineVar("vi",&v);
	p.DefineVar("rad",&rad);
  p.DefineVar("vtu",&vtu);
  p.DefineVar("vtv",&vtv);

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
    if(tri::Allocator<CMeshO>::IsValidHandle<float>(m,hh))
    {
      v_handlers.push_back(hh);
      v_attrNames.push_back(AllVertexAttribName[i]);
      v_attrValue.push_back(0);
      p.DefineVar(v_attrNames.back(), &v_attrValue.back());
      qDebug("Adding custom per vertex float variable %s",v_attrNames.back().c_str());
    }
  }
  AllVertexAttribName.clear();
  tri::Allocator<CMeshO>::GetAllPerVertexAttribute< Point3f >(m,AllVertexAttribName);
  for(int i = 0; i < (int) AllVertexAttribName.size(); i++)
  {
    CMeshO::PerVertexAttributeHandle<Point3f> hh3 = tri::Allocator<CMeshO>::GetPerVertexAttribute<Point3f>(m, AllVertexAttribName[i]);
    if(tri::Allocator<CMeshO>::IsValidHandle<Point3f>(m,hh3))
    {
      v3_handlers.push_back(hh3);

      v3_attrValue.push_back(0);
      v3_attrNames.push_back(AllVertexAttribName[i]+"_x");
      p.DefineVar(v3_attrNames.back(), &v3_attrValue.back());

      v3_attrValue.push_back(0);
      v3_attrNames.push_back(AllVertexAttribName[i]+"_y");
      p.DefineVar(v3_attrNames.back(), &v3_attrValue.back());

      v3_attrValue.push_back(0);
      v3_attrNames.push_back(AllVertexAttribName[i]+"_z");
      p.DefineVar(v3_attrNames.back(), &v3_attrValue.back());
      qDebug("Adding custom per vertex Point3f variable %s",v3_attrNames.back().c_str());
    }
  }
}

// Function explicitely define parser variables to perform Per-Face filter action
void FilterFunctionPlugin::setPerFaceVariables(Parser &p, CMeshO &m)
{
	// coord of the three vertices within a face
	p.DefineVar("x0", &x0);
	p.DefineVar("y0", &y0);
	p.DefineVar("z0", &z0);
	p.DefineVar("x1", &x1);
	p.DefineVar("y1", &y1);
	p.DefineVar("z1", &z1);
	p.DefineVar("x2", &x2);
	p.DefineVar("y2", &y2);
	p.DefineVar("z2", &z2);

	// attributes of the vertices
	// normals:
	p.DefineVar("nx0", &nx0);
	p.DefineVar("ny0", &ny0);
	p.DefineVar("nz0", &nz0);
		
	p.DefineVar("nx1", &nx1);
	p.DefineVar("ny1", &ny1);
	p.DefineVar("nz1", &nz1);

	p.DefineVar("nx2", &nx2);
	p.DefineVar("ny2", &ny2);
	p.DefineVar("nz2", &nz2);

	// colors:
	p.DefineVar("r0", &r0);
	p.DefineVar("g0", &g0);
	p.DefineVar("b0", &b0);

	p.DefineVar("r1", &r1);
	p.DefineVar("g1", &g1);
	p.DefineVar("b1", &b1);
	
	p.DefineVar("r2", &r2);
	p.DefineVar("g2", &g2);
	p.DefineVar("b2", &b2);

	// quality
	p.DefineVar("q0", &q0);
	p.DefineVar("q1", &q1);
	p.DefineVar("q2", &q2);

	// face color
	p.DefineVar("r", &r);
	p.DefineVar("g", &g);
	p.DefineVar("b", &b);

  // face quality
  p.DefineVar("q", &fq);

	// index
	p.DefineVar("fi",&f);
	p.DefineVar("vi0",&v0i);
	p.DefineVar("vi1",&v1i);
	p.DefineVar("vi2",&v2i);

  p.DefineVar("wtu0",&wtu0);
  p.DefineVar("wtv0",&wtv0);
  p.DefineVar("wtu1",&wtu1);
  p.DefineVar("wtv1",&wtv1);
  p.DefineVar("wtu2",&wtu2);
  p.DefineVar("wtv2",&wtv2);

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
    if(tri::Allocator<CMeshO>::IsValidHandle<float>(m,hh))
    {
      f_handlers.push_back(hh);
      f_attrNames.push_back(AllFaceAttribName[i]);
      f_attrValue.push_back(0);
      p.DefineVar(f_attrNames.back(), &f_attrValue.back());
    }
  }

}

Q_EXPORT_PLUGIN(FilterFunctionPlugin)
