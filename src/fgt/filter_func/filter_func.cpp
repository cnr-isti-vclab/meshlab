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
#include <string>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/selection.h>
#include <vcg/complex/trimesh/create/platonic.h>

#include "filter_func.h"
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
		<< FF_FACE_QUALITY
		<< FF_GRID;
  
	foreach(FilterIDType tt , types())
		actionList << new QAction(filterName(tt), this);
}

// short string describing each filtering action 
const QString FilterFunctionPlugin::filterName(FilterIDType filterId) 
{
	switch(filterId) {
		case FF_VERT_SELECTION :  return QString("Conditional Vertex Selection");
		case FF_FACE_SELECTION : return QString("Conditional Face Selection");
		case FF_GEOM_FUNC :  return QString("Geometric Function");
		case FF_FACE_COLOR : return QString("Per-Face Color Function");
		case FF_FACE_QUALITY : return QString("Per-Face Quality Function");
		case FF_VERT_COLOR : return QString("Per-Vertex Color Function");
		case FF_VERT_QUALITY : return QString("Per-Vertex Quality Function");
		case FF_GRID : return QString("Grid Generator");
		default : assert(0); 
	}
	return QString("error!");
}

// long string describing each filtering action 
const QString FilterFunctionPlugin::filterInfo(FilterIDType filterId)
{
	switch(filterId) {
		case FF_VERT_SELECTION : return tr("Boolean function using muparser lib to perform vertex selection over current mesh.<br>"
										   "It's possibile to use parenthesis, per-vertex variables and boolean operator:<br>"
										   "<b>(</b>,<b>)</b>,<b>and</b>,<b>or</b>,<b><</b><b>></b>,<b>=</b><br>"
									       "<b>x</b>,<b>y</b>,<b>z</b> for vertex coord<br>"
										   "<b>nx</b>,<b>ny</b>,<b>nz</b> for vertex normal<br>"
									       "<b>r</b>,<b>g</b>,<b>b</b> for vertex color <br><b>q</b> for vertex quality."); 

		case FF_FACE_SELECTION : return tr("Boolean function using muparser lib to perform faces selection over current mesh.<br>"
									       "It's possibile to use per-face variables like attributes associated to the three vertex of every face.<br>"
										   "<b>x0,y0,z0</b> for <b>first vertex</b>; x1,y1,z1 for second vertex; x2,y2,z2 for third vertex.<br>"
										   "and also <b>nx0,ny0,nz0</b> nx1,ny1,nz1 etc. for <b>normals</b> and <b>r0,g0,b0</b> for <b>color</b>,"
										   "<b>q0,q1,q2</b> for <b>quality</b>.<br>"
										   "(,),=,<<,>> are allowed.");  

		case FF_GEOM_FUNC :  return tr("Geometric function using muparser lib to generate new Coord<br>"
									   "You can change x,y,z for every vertex according to the function specified.<br>"
									   "It's possibile to use per-vertex variables in the expression:<br>"
									   "x, y, z, nx, ny, nz (normal), r, g, b (color), q (quality).");

		case FF_FACE_COLOR : return tr("Color function using muparser lib to generate new RGB color for every face<br>"
									   "Insert three function each one for red, green and blue channel respectively.<br>"
									   "It's possibile to use per-face variables like attributes associated to the three vertex of every face.<br>"
									   "<b>x0,y0,z0</b> for <b>first vertex</b>; x1,y1,z1 for second vertex; x2,y2,z2 for third vertex.<br>"
									   "and also <b>nx0,ny0,nz0</b> nx1,ny1,nz1 etc. for <b>normals</b> and <b>r0,g0,b0</b> for <b>color</b>,"
									   "<b>q0,q1,q2</b> for <b>quality</b>.<br>"
									   "(,),=,<<,>> are allowed.");

		case FF_VERT_COLOR : return tr("Color function using muparser lib to generate new RGB color for every vertex<br>"
									   "Insert three function each one for red, green and blue channel respectively.<br>"
									   "It's possibile to use per-vertex variables in the expression:<br>"
									   "x, y, z, nx, ny, nz (normal), r, g, b (color), q (quality).");

		case FF_VERT_QUALITY : return tr("Quality function using muparser to generate new Quality for every vertex<br>"
										 "It's possibile to use per-vertex variables in the expression:<br>"
									     "x, y, z, nx, ny, nz (normal), r, g, b (color), q (quality).");

		case FF_FACE_QUALITY : return tr("Quality function using muparser to generate new Quality for every face<br>"
										 "Insert three function each one for quality of the three vertex of a face<br>"
										 "It's possibile to use per-face variables like attributes associated to the three vertex of every face.<br>"
									     "<b>x0,y0,z0</b> for <b>first vertex</b>; x1,y1,z1 for second vertex; x2,y2,z2 for third vertex.<br>"
									     "and also <b>nx0,ny0,nz0</b> nx1,ny1,nz1 etc. for <b>normals</b> and <b>r0,g0,b0</b> for <b>color</b>,"
									     "<b>q0,q1,q2</b> for <b>quality</b>.<br>"
									     "(,),=,<<,>> are allowed.");

		case FF_GRID : return tr("Generate a new 2D Grid mesh with number of vertices on X and Y axis specified by user with absolute length/height.<br>"
						         "It's possibile to center Grid on origin.");
		default : assert(0); 
	}
	return QString("error!");
}

const FilterFunctionPlugin::FilterClass FilterFunctionPlugin::getClass(QAction *a)
{
  switch(ID(a))
  {
    case FF_GRID      : return MeshFilterInterface::MeshCreation;
    default			  : return MeshFilterInterface::Generic;
  }
}

const PluginInfo &FilterFunctionPlugin::pluginInfo()
{
   static PluginInfo ai;
   ai.Date=tr(__DATE__);
	 ai.Version = tr("0.3");
	 ai.Author = ("Giuseppe Alemanno");
   return ai;
 }

const int FilterFunctionPlugin::getRequirements(QAction *action)
{
  switch(ID(action))
  {
    case FF_VERT_SELECTION :
	case FF_GEOM_FUNC :
	case FF_VERT_COLOR :
	case FF_VERT_QUALITY :
	case FF_GRID : return 0;
	case FF_FACE_SELECTION :
	case FF_FACE_QUALITY :
    case FF_FACE_COLOR : return MeshModel::MM_FACECOLOR;
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
void FilterFunctionPlugin::initParameterSet(QAction *action,MeshModel &m, FilterParameterSet & parlst) 
{
	Q_UNUSED(m);
	switch(ID(action))	 {

		case FF_VERT_SELECTION :
			parlst.addString("condSelect","", "boolean function",
							 "type a boolean function that will be evaluated in order to select a subset of vertices<br>"
							 "example: (y > 0) and (ny > 0)");

			parlst.addBool("strictSelect",true,"Strict face selection", "If checked a face is selected if <b>ALL</b> its vertices are selected. <br>"
										 "If unchecked a face is selected if <b>at least one</b> of its vertices is selected");
			break;

		case FF_FACE_SELECTION :
			parlst.addString("condSelect","", "boolean function",
							 "type a boolean function that will be evaluated in order to select a subset of faces<br>");
			break;

		case FF_GEOM_FUNC :
			parlst.addString("x","", "func x = ", "insert function to generate new coord for x");
			parlst.addString("y","", "func y = ", "insert function to generate new coord for y");
			parlst.addString("z","", "func z = ", "insert function to generate new coord for z");
			break;

		case FF_FACE_COLOR:
			parlst.addString("r","", "func r = ", "function to generate Red component. Expected Range 0-255");
			parlst.addString("g","", "func g = ", "function to generate Green component. Expected Range 0-255");
			parlst.addString("b","", "func b = ", "function to generate Blue component. Expected Range 0-255");
			break;

		case FF_FACE_QUALITY:
			parlst.addString("q0","", "func q0 = ", "function to generate new quality for <b>first</b> vertex of every face");
			parlst.addString("q1","", "func q1 = ", "function to generate new quality for <b>second</b> vertex of every face");
			parlst.addString("q2","", "func q2 = ", "function to generate new quality for <b>third</b> vertex of every face");
			break;

		case FF_VERT_COLOR:
			parlst.addString("r","", "func r = ", "function to generate Red component. Expected Range 0-255");
			parlst.addString("g","", "func g = ", "function to generate Green component. Expected Range 0-255");
			parlst.addString("b","", "func b = ", "function to generate Blue component. Expected Range 0-255");
			break;

		case FF_VERT_QUALITY:
			parlst.addString("q","", "func q = ", "function to evaluate to generate new Quality for every vertex");
			break;

		case FF_GRID :
			parlst.addInt("numVertX", 10, "num vertices on x", "number of vertices on x. it must be positive");
			parlst.addInt("numVertY", 10, "num vertices on y", "number of vertices on y. it must be positive");
			parlst.addFloat("absScaleX", 0.3f, "x scale", "absolute scale on x (float)");
			parlst.addFloat("absScaleY", 0.3f, "y scale", "absolute scale on y (float)");
			parlst.addBool("center",false,"centered on origin", "center grid generated by filter on origin.<br>"
						   "Grid is first generated and than moved into origin (using muparser lib to perform fast calc on every vertex)");
			break;

		default : assert(0); 
	}
}

// The Real Core Function doing the actual mesh processing.
bool FilterFunctionPlugin::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & par, vcg::CallBackPos *cb)
{
	Q_UNUSED(cb);
	switch(ID(filter)) {
		case FF_VERT_SELECTION :
			{
				QString select = par.getString("condSelect");
			
				// muparser initialization and explicitely define parser variables
				Parser p;
				setPerVertexVariables(p);

				// set expression inserted by user as string (required by muparser)
				std::string expr = select.toStdString();
				p.SetExpr(expr);

				int numvert = 0;
				time_t start = clock();

				// every parser variables is related to vertex coord and attributes.
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
				Log(GLLogStream::Info, "selected %d vertices in %.2f sec.", numvert, (clock() - start) / (float) CLOCKS_PER_SEC);
					
				return true;
			}
		break;

		case FF_FACE_SELECTION :
			{
				QString select = par.getString("condSelect");
			
				// muparser initialization and explicitely define parser variables
				Parser p;
				setPerFaceVariables(p);

				// set expression inserted by user as string (required by muparser)
				p.SetExpr(select.toStdString());

				int numface = 0;
				time_t start = clock();

				// every parser variables is related to face attributes.
				CMeshO::FaceIterator fi;
				for(fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
				{
					x0 = (*fi).V(0)->P()[0];
					y0 = (*fi).V(0)->P()[1];
					z0 = (*fi).V(0)->P()[2];

					x1 = (*fi).V(1)->P()[0];
					y1 = (*fi).V(1)->P()[1];
					z1 = (*fi).V(1)->P()[2];

					x2 = (*fi).V(2)->P()[0];
					y2 = (*fi).V(2)->P()[1];
					z2 = (*fi).V(2)->P()[2];

					nx0 = (*fi).V(0)->N()[0];
					ny0 = (*fi).V(0)->N()[1];
					nz0 = (*fi).V(0)->N()[2];

					nx1 = (*fi).V(1)->N()[0];
					ny1 = (*fi).V(1)->N()[1];
					nz1 = (*fi).V(1)->N()[2];

					nx2 = (*fi).V(2)->N()[0];
					ny2 = (*fi).V(2)->N()[1];
					nz2 = (*fi).V(2)->N()[2];

					r0 = (*fi).V(0)->C()[0];
					g0 = (*fi).V(0)->C()[1];
					b0 = (*fi).V(0)->C()[2];

					r1 = (*fi).V(1)->C()[0];
					g1 = (*fi).V(1)->C()[1];
					b1 = (*fi).V(1)->C()[2];

					r2 = (*fi).V(2)->C()[0];
					g2 = (*fi).V(2)->C()[1];
					b2 = (*fi).V(2)->C()[2];

					q0 = (*fi).V(0)->Q();
					q1 = (*fi).V(1)->Q();
					q2 = (*fi).V(2)->Q();
					
					r = (*fi).C()[0];
					g = (*fi).C()[1];
					b = (*fi).C()[2];

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
				Log(GLLogStream::Info, "selected %d faces in %.2f sec.", numface, (clock() - start) / (float) CLOCKS_PER_SEC);
					
				return true;
			}
		break;

		case FF_GEOM_FUNC :
			{
				QString func_x = par.getString("x");
				QString func_y = par.getString("y");
				QString func_z = par.getString("z");

				// muparser initialization and explicitely define parser variables
				// function for x,y and z must use different parser and variables
				Parser p1,p2,p3;

				setPerVertexVariables(p1);
				setPerVertexVariables(p2);
				setPerVertexVariables(p3);

				p1.SetExpr(func_x.toStdString());
				p2.SetExpr(func_y.toStdString());
				p3.SetExpr(func_z.toStdString());

				CMeshO::VertexIterator vi;
				double newx,newy,newz;
				errorMessage = "";

				time_t start = clock();

				// every parser variables is related to vertex coord and attributes.
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

					// every function is evaluated by different parser.
					// errorMessage dialog contains errors for func x, func y and func z
					try { newx = p1.Eval(); } 
					catch(Parser::exception_type &e) {
						errorMessage += "func x: ";
						errorMessage += e.GetMsg().c_str();
						errorMessage += "\n";
					}

					try { newy = p2.Eval(); } 
					catch(Parser::exception_type &e) {
						errorMessage += "func y: ";
						errorMessage += e.GetMsg().c_str();
						errorMessage += "\n";
					}

					try { newz = p3.Eval(); } 
					catch(Parser::exception_type &e) {
						errorMessage += "func z: ";
						errorMessage += e.GetMsg().c_str();
						errorMessage += "\n";
					}
					if(errorMessage != "") return false;

					// set new vertex coord for this iteration
					(*vi).P() = Point3f(newx,newy,newz);
			    }

				// update bounding box, normalize normals
				tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
				tri::UpdateNormals<CMeshO>::NormalizeFace(m.cm);
				tri::UpdateBounding<CMeshO>::Box(m.cm);

				// if succeded log stream contains number of vertices processed and time elapsed
				Log(GLLogStream::Info, "%d vertices processed in %.2f sec.", m.cm.vn, (clock() - start) / (float) CLOCKS_PER_SEC);

				return true;
			}
		break;

		case FF_VERT_COLOR:
			{
				QString func_r = par.getString("r");
				QString func_g = par.getString("g");
				QString func_b = par.getString("b");

				// muparser initialization and explicitely define parser variables
				// function for x,y and z must use different parser and variables
				Parser p1,p2,p3;

				setPerVertexVariables(p1);
				setPerVertexVariables(p2);
				setPerVertexVariables(p3);

				p1.SetExpr(func_r.toStdString());
				p2.SetExpr(func_g.toStdString());
				p3.SetExpr(func_b.toStdString());

				CMeshO::VertexIterator vi;
				double newr,newg,newb;
				errorMessage = "";

				time_t start = clock();

				// every parser variables is related to vertex coord and attributes.
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

					// every function is evaluated by different parser.
					// errorMessage dialog contains errors for func r, func g and func b
					try { newr = p1.Eval(); } 
					catch(Parser::exception_type &e) {
						errorMessage += "func r: ";
						errorMessage += e.GetMsg().c_str();
						errorMessage += "\n";
					}

					try { newg = p2.Eval(); } 
					catch(Parser::exception_type &e) {
						errorMessage += "func g: ";
						errorMessage += e.GetMsg().c_str();
						errorMessage += "\n";
					}

					try { newb = p3.Eval(); } 
					catch(Parser::exception_type &e) {
						errorMessage += "func b: ";
						errorMessage += e.GetMsg().c_str();
						errorMessage += "\n";
					}
					if(errorMessage != "") return false;

					// set new color for this iteration
					(*vi).C() = Color4b(newr,newg,newb,255);
			    }

				// if succeded log stream contains number of vertices processed and time elapsed
				Log(GLLogStream::Info, "%d vertices processed in %.2f sec.", m.cm.vn, (clock() - start) / (float) CLOCKS_PER_SEC);

				return true;
			}
		break;

		case FF_VERT_QUALITY:
			{
				QString func_q = par.getString("q");
				
				// muparser initialization and define custom variables
				Parser p;
				setPerVertexVariables(p);

				// set expression to calc with parser
				p.SetExpr(func_q.toStdString());

				time_t start = clock();

				// every parser variables is related to vertex coord and attributes.
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

					// use parser to evaluate function specified above
					// in case of fail, errorMessage dialog contains details of parser's error
					try { 
						(*vi).Q() = p.Eval();
					} catch(Parser::exception_type &e) {
						errorMessage = e.GetMsg().c_str();
  						return false;
					}
				}

				// if succeded log stream contains number of vertices and time elapsed
				Log(GLLogStream::Info, "%d vertices processed in %.2f sec.", m.cm.vn, (clock() - start) / (float) CLOCKS_PER_SEC);

				return true;
			}
		break;

		case FF_FACE_COLOR:
			{
				// NOTE: you have to manually set Render -> Color -> Per Face
				QString func_r = par.getString("r");
				QString func_g = par.getString("g");
				QString func_b = par.getString("b");

				// muparser initialization and explicitely define parser variables
				// every function must uses own parser and variables
				Parser p1,p2,p3;

				setPerFaceVariables(p1);
				setPerFaceVariables(p2);
				setPerFaceVariables(p3);

				p1.SetExpr(func_r.toStdString());
				p2.SetExpr(func_g.toStdString());
				p3.SetExpr(func_b.toStdString());

				// RGB is related to every face
				CMeshO::FaceIterator fi;
				double newr,newg,newb;
				errorMessage = "";

				time_t start = clock();

				// every parser variables is related to face attributes.
				for(fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
				{
					x0 = (*fi).V(0)->P()[0];
					y0 = (*fi).V(0)->P()[1];
					z0 = (*fi).V(0)->P()[2];

					x1 = (*fi).V(1)->P()[0];
					y1 = (*fi).V(1)->P()[1];
					z1 = (*fi).V(1)->P()[2];

					x2 = (*fi).V(2)->P()[0];
					y2 = (*fi).V(2)->P()[1];
					z2 = (*fi).V(2)->P()[2];

					nx0 = (*fi).V(0)->N()[0];
					ny0 = (*fi).V(0)->N()[1];
					nz0 = (*fi).V(0)->N()[2];

					nx1 = (*fi).V(1)->N()[0];
					ny1 = (*fi).V(1)->N()[1];
					nz1 = (*fi).V(1)->N()[2];

					nx2 = (*fi).V(2)->N()[0];
					ny2 = (*fi).V(2)->N()[1];
					nz2 = (*fi).V(2)->N()[2];

					r0 = (*fi).V(0)->C()[0];
					g0 = (*fi).V(0)->C()[1];
					b0 = (*fi).V(0)->C()[2];

					r1 = (*fi).V(1)->C()[0];
					g1 = (*fi).V(1)->C()[1];
					b1 = (*fi).V(1)->C()[2];

					r2 = (*fi).V(2)->C()[0];
					g2 = (*fi).V(2)->C()[1];
					b2 = (*fi).V(2)->C()[2];

					q0 = (*fi).V(0)->Q();
					q1 = (*fi).V(1)->Q();
					q2 = (*fi).V(2)->Q();
					
					r = (*fi).C()[0];
					g = (*fi).C()[1];
					b = (*fi).C()[2];


					// evaluate functions to generate new color
					// in case of fail, error dialog contains details of parser's error
					try { newr = p1.Eval(); }
					catch(Parser::exception_type &e) {
						errorMessage += "func r: ";
						errorMessage += e.GetMsg().c_str();
						errorMessage += "\n";
					}

					try { newg = p2.Eval(); }
					catch(Parser::exception_type &e) {
						errorMessage += "func g: ";
						errorMessage += e.GetMsg().c_str();
						errorMessage += "\n";
					}

					try { newb = p3.Eval(); }
					catch(Parser::exception_type &e) {
						errorMessage += "func b: ";
						errorMessage += e.GetMsg().c_str();
						errorMessage += "\n";
					}

					if(errorMessage != "") return false;

					// set new color for this iteration
					(*fi).C() = Color4b(newr,newg,newb,255);
				}

				// if succeded log stream contains number of vertices processed and time elapsed
				Log(GLLogStream::Info, "%d faces processed in %.2f sec.", m.cm.fn, (clock() - start) / (float) CLOCKS_PER_SEC);

				return true;

			}
		break;

		case FF_FACE_QUALITY:
			{
				QString func_q0 = par.getString("q0");
				QString func_q1 = par.getString("q1");
				QString func_q2 = par.getString("q2");

				// muparser initialization and define custom variables
				Parser p1,p2,p3;
				setPerFaceVariables(p1);
				setPerFaceVariables(p2);
				setPerFaceVariables(p3);
				
				// set expression to calc with parser
				p1.SetExpr(func_q0.toStdString());
				p2.SetExpr(func_q1.toStdString());
				p3.SetExpr(func_q2.toStdString());

				time_t start = clock();
				
				// three different function for quality of every vertex of a face
				CMeshO::FaceIterator fi;
				double newq0, newq1, newq2;

				// every parser variables is related to face attributes.
				for(fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
				{
					x0 = (*fi).V(0)->P()[0];
					y0 = (*fi).V(0)->P()[1];
					z0 = (*fi).V(0)->P()[2];

					x1 = (*fi).V(1)->P()[0];
					y1 = (*fi).V(1)->P()[1];
					z1 = (*fi).V(1)->P()[2];

					x2 = (*fi).V(2)->P()[0];
					y2 = (*fi).V(2)->P()[1];
					z2 = (*fi).V(2)->P()[2];

					nx0 = (*fi).V(0)->N()[0];
					ny0 = (*fi).V(0)->N()[1];
					nz0 = (*fi).V(0)->N()[2];

					nx1 = (*fi).V(1)->N()[0];
					ny1 = (*fi).V(1)->N()[1];
					nz1 = (*fi).V(1)->N()[2];

					nx2 = (*fi).V(2)->N()[0];
					ny2 = (*fi).V(2)->N()[1];
					nz2 = (*fi).V(2)->N()[2];

					r0 = (*fi).V(0)->C()[0];
					g0 = (*fi).V(0)->C()[1];
					b0 = (*fi).V(0)->C()[2];

					r1 = (*fi).V(1)->C()[0];
					g1 = (*fi).V(1)->C()[1];
					b1 = (*fi).V(1)->C()[2];

					r2 = (*fi).V(2)->C()[0];
					g2 = (*fi).V(2)->C()[1];
					b2 = (*fi).V(2)->C()[2];

					q0 = (*fi).V(0)->Q();
					q1 = (*fi).V(1)->Q();
					q2 = (*fi).V(2)->Q();
					
					r = (*fi).C()[0];
					g = (*fi).C()[1];
					b = (*fi).C()[2];

					// evaluate functions to generate new quality
					// in case of fail, error dialog contains details of parser's error
					try { newq0 = p1.Eval(); }
					catch(Parser::exception_type &e) {
						errorMessage += "func q0: ";
						errorMessage += e.GetMsg().c_str();
						errorMessage += "\n";
					}

					try { newq1 = p2.Eval(); }
					catch(Parser::exception_type &e) {
						errorMessage += "func q1: ";
						errorMessage += e.GetMsg().c_str();
						errorMessage += "\n";
					}

					try { newq2 = p3.Eval(); }
					catch(Parser::exception_type &e) {
						errorMessage += "func q2: ";
						errorMessage += e.GetMsg().c_str();
						errorMessage += "\n";
					}

					if(errorMessage != "") return false;

					//set new quality for every vertex of a face
					(*fi).V(0)->Q() = newq0;
					(*fi).V(1)->Q() = newq1;
					(*fi).V(2)->Q() = newq2;
				}

				// if succeded log stream contains number of vertices processed and time elapsed
				Log(GLLogStream::Info, "%d faces processed in %.2f sec.", m.cm.fn, (clock() - start) / (float) CLOCKS_PER_SEC);

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
					// move x and y using parser
					Parser p1,p2;
					double x,y;
					double halfw = double(w-1)/2;
					double halfh = double(h-1)/2;
					double wld = wl/double(w);
					double hld = hl/float(h);

					p1.DefineVar("x",&x);
					p1.DefineVar("y",&y);
					p1.DefineVar("wld",&wld);
					p1.DefineVar("hld",&hld);
					p1.DefineVar("halfw",&halfw);
					p1.DefineVar("halfh",&halfh);

					p2.DefineVar("x",&x);
					p2.DefineVar("y",&y);
					p2.DefineVar("wld",&wld);
					p2.DefineVar("hld",&hld);
					p2.DefineVar("halfw",&halfw);
					p2.DefineVar("halfh",&halfh);

					// calc new x and new y
					p1.SetExpr("x - (wld * halfw)");
					p2.SetExpr("y - (hld * halfh)");
					
					CMeshO::VertexIterator vi;
					for(vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi)
					{
						x = (*vi).P()[0];
						y = (*vi).P()[1];

						try {
							(*vi).P()[0] = p1.Eval();
						} catch(Parser::exception_type &e) {
							errorMessage = e.GetMsg().c_str();
  							return false;
						}
						try {
							(*vi).P()[1] = p2.Eval();
						} catch(Parser::exception_type &e) {
							errorMessage = e.GetMsg().c_str();
							return false;
						}
					}
				}

				// update bounding box, normals
				tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);	
				tri::UpdateNormals<CMeshO>::NormalizeFace(m.cm);
				tri::UpdateBounding<CMeshO>::Box(m.cm);
				return true;
			}
		break;

		default : assert (0);
	}
	return false;
}

/* Function explicitely define parser variables to perform per-vertex filter action
   x, y, z for vertex coord, nx, ny, nz for normal coord, r, g ,b for color
   and q for quality */
void FilterFunctionPlugin::setPerVertexVariables(Parser &p)
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
}

// Function explicitely define parser variables to perform Per-Face filter action
void FilterFunctionPlugin::setPerFaceVariables(Parser &p)
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
}

Q_EXPORT_PLUGIN(FilterFunctionPlugin)