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
#include <vcg/complex/trimesh/update/selection.h>
#include <vcg/complex/trimesh/create/platonic.h>
#include <vcg/complex/trimesh/allocate.h>

#include <string>

#include "filter_func.h"
#include "muParser.h"

using namespace mu;
using namespace vcg;

// Constructor
FilterFunctionPlugin::FilterFunctionPlugin() 
{ 
	typeList 
		<< FF_VERT_SELECTION
		<< FF_GEOM_FUNC
		<< FF_COLOR_FUNC
		<< FF_GRID;
  
	foreach(FilterIDType tt , types())
		actionList << new QAction(filterName(tt), this);
}

// short string describing each filtering action 
const QString FilterFunctionPlugin::filterName(FilterIDType filterId) 
{
	switch(filterId) {
		case FF_VERT_SELECTION :  return QString("Conditional Vertex Selection"); 
		case FF_GEOM_FUNC :  return QString("Geometric Function");
		case FF_COLOR_FUNC : return QString("Color Function");
		case FF_GRID : return QString("Grid Generator");
		default : assert(0); 
	}
	return QString("error!");
}

// long string describing each filtering action 
const QString FilterFunctionPlugin::filterInfo(FilterIDType filterId)
{
	switch(filterId) {
		case FF_VERT_SELECTION :  return QString("Boolean function using muparser lib to perform vertex selection"); 
		case FF_GEOM_FUNC :  return QString("Geometric function using muparser lib to generate new coord");
		case FF_COLOR_FUNC : return QString("Color function using muparser lib to generate new RGB color for every face");
		case FF_GRID : return QString("2D Grid Generator");
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
	 ai.Version = tr("0.2");
	 ai.Author = ("Giuseppe Alemanno");
   return ai;
 }

const int FilterFunctionPlugin::getRequirements(QAction *action)
{
  switch(ID(action))
  {
    case FF_VERT_SELECTION :
	case FF_GEOM_FUNC :
	case FF_GRID : return 0;
    case FF_COLOR_FUNC : return MeshModel::MM_FACECOLOR;
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
	switch(ID(action))	 {

		case FF_VERT_SELECTION :
			parlst.addString("condSelect"," ", "boolean function",
							 "you can use: ( ) and or < > = \
							 x,y,z for vertex coord, nx, ny, nz for normal coord, r, g, b for color and q for quality");

			parlst.addBool("strictSelect",true,"Strict face selection", "If checked a face is selected if <b>ALL</b> its vertices are selected. <br>"
										 "If unchecked a face is selected if <b>at least one</b> of its vertices is selected");
			break;

		case FF_GEOM_FUNC :
			parlst.addString("x"," ", "func x = ", "insert function to generate new coord for x");
			parlst.addString("y"," ", "func y = ", "insert function to generate new coord for y");
			parlst.addString("z"," ", "func z = ", "insert function to generate new coord for z");
			break;

		case FF_COLOR_FUNC:
			parlst.addString("r"," ", "func r = ", "insert function to generate Red component");
			parlst.addString("g"," ", "func g = ", "insert function to generate Green component");
			parlst.addString("b"," ", "func b = ", "insert function to generate Blue component");
			break;

		case FF_GRID :
			parlst.addInt("numVertX", 10, "num vertices on x", "number of vertices on x");
			parlst.addInt("numVertY", 10, "num vertices on y", "number of vertices on y");
			parlst.addFloat("absScaleX", 0.3, "x scale", "absolute scale on x");
			parlst.addFloat("absScaleY", 0.3, "y scale", "absolute scale on y");
			break;

		default : assert(0); 
	}
}

// The Real Core Function doing the actual mesh processing.
bool FilterFunctionPlugin::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & par, vcg::CallBackPos *cb)
{
	switch(ID(filter)) {
		case FF_VERT_SELECTION :
			{
				QString select = par.getString("condSelect");
			
				// muparser initialization and explicitely define parser variables
				Parser p;
				setParserVariables(p);

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
					// in case of fail, log stream contains details of parser's error
					try { 
						selected = p.Eval();
					} catch(Parser::exception_type &e) {
						Log(GLLogStream::Info, "(error) Conditional Vertex Selection : %s",e.GetMsg().c_str());
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

		case FF_GEOM_FUNC :
			{
				QString func_x = par.getString("x");
				QString func_y = par.getString("y");
				QString func_z = par.getString("z");

				// muparser initialization and explicitely define parser variables
				// function for x,y and z must use different parser and variables
				Parser p1,p2,p3;

				setParserVariables(p1);
				setParserVariables(p2);
				setParserVariables(p3);

				p1.SetExpr(func_x.toStdString());
				p2.SetExpr(func_y.toStdString());
				p3.SetExpr(func_z.toStdString());

				CMeshO::VertexIterator vi;
				double newx,newy,newz;
				bool error = false;

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
					// log stream dump errors for func x, func y and func z
					try { newx = p1.Eval(); } 
					catch(Parser::exception_type &e) {
						Log(GLLogStream::Info, "(error) function x : %s",e.GetMsg().c_str()); 
						error = true;
					}

					try { newy = p2.Eval(); } 
					catch(Parser::exception_type &e) {
						Log(GLLogStream::Info, "(error) function y : %s",e.GetMsg().c_str()); 
						error = true;
					}

					try { newz = p3.Eval(); } 
					catch(Parser::exception_type &e) {
						Log(GLLogStream::Info, "(error) function z : %s",e.GetMsg().c_str()); 
						error = true;
					}
					if(error) return false;

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

		case FF_COLOR_FUNC:
			{
				// NOTE: you have to manually set Render -> Color -> Per Face
				QString func_r = par.getString("r");
				QString func_g = par.getString("g");
				QString func_b = par.getString("b");

				// muparser initialization and explicitely define parser variables
				// every function must uses own parser and variables
				Parser p1,p2,p3;

				setParserVariables(p1);
				setParserVariables(p2);
				setParserVariables(p3);

				p1.SetExpr(func_r.toStdString());
				p2.SetExpr(func_g.toStdString());
				p3.SetExpr(func_b.toStdString());

				// RGB is related to every face
				CMeshO::FaceIterator fi;
				double newr, newg, newb;
				bool error = false;

				time_t start = clock();

				for(fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
				{
					r = (*fi).C()[0];  // face color R
					g = (*fi).C()[1];  // face color G
					b = (*fi).C()[2];  // face color B
					
					// evaluate functions to generate new color
					try { newr = p1.Eval(); }
					catch(Parser::exception_type &e) {
						Log(GLLogStream::Info, "(error) function x : %s",e.GetMsg().c_str()); 
						error = true;
					}

					try { newg = p2.Eval(); }
					catch(Parser::exception_type &e) {
						Log(GLLogStream::Info, "(error) function x : %s",e.GetMsg().c_str()); 
						error = true;
					}

					try { newb = p3.Eval(); }
					catch(Parser::exception_type &e) {
						Log(GLLogStream::Info, "(error) function x : %s",e.GetMsg().c_str()); 
						error = true;
					}
					if(error) return false;

					(*fi).C()[0] = newr;
					(*fi).C()[1] = newg;
					(*fi).C()[2] = newb;
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

				// use Grid function to generate Grid and update bounding box, normals
				std::vector<float> data(w*h,0);
				tri::Grid<CMeshO>(m.cm, w, h, wl, hl, &data[0]);
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

/* Function explicitely define parser variables
   x, y, z for vertex coord, nx, ny, nz for normal coord, r, g ,b for color
   and q for quality */
void FilterFunctionPlugin::setParserVariables(Parser &p)
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

Q_EXPORT_PLUGIN(FilterFunctionPlugin)
