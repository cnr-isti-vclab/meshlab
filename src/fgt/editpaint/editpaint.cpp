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
#include "editpaint.h"

#include <QtGui>
#include <GL/glew.h>

#include <cmath>
#include <cstdlib>
#include <meshlab/glarea.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/color.h>
#include <vcg/complex/trimesh/update/topology.h>
#include <wrap/io_trimesh/io_mask.h>
#include <vcg/math/perlin_noise.h>

using namespace std;
using namespace vcg;

EditPaintPlugin::EditPaintPlugin() 
{
	zbuffer = NULL;
	color_buffer = NULL;
	clone_zbuffer = NULL;
	generateCircle(circle);
	generateCircle(dense_circle, 64);
	generateSquare(square);
	generateSquare(dense_square, 16);
}

EditPaintPlugin::~EditPaintPlugin() {}

const QString EditPaintPlugin::Info() {
	return tr("Improved Painting");
}

bool EditPaintPlugin::StartEdit(MeshModel& m, GLArea * parent) 
{
	dock = new QDockWidget(parent->window());
	paintbox = new Paintbox(dock);
	dock->setAllowedAreas(Qt::NoDockWidgetArea);
	dock->setWidget(paintbox);
	QPoint p=parent->mapToGlobal(QPoint(0,0));
	dock->setGeometry(5 + p.x(), p.y() + 5 , paintbox->width(), parent->height() - 10);
	dock->setFloating(true);
	dock->setVisible(true);

	tri::UpdateBounding<CMeshO>::Box(m.cm);
	
	m.updateDataMask(MeshModel::MM_VERTFACETOPO | MeshModel::MM_FACEMARK|MeshModel::MM_VERTMARK);

	if (!m.hasDataMask(MeshModel::MM_VERTCOLOR))
	{
		m.updateDataMask(MeshModel::MM_VERTCOLOR);
		tri::UpdateColor<CMeshO>::VertexConstant(m.cm,Color4b(150, 150, 150, 255));
	}
  tri::InitFaceIMark(m.cm);
  tri::InitVertexIMark(m.cm);
		
	parent->getCurrentRenderMode().colorMode=vcg::GLW::CMPerVert;
	
	QObject::connect(paintbox, SIGNAL(undo()), this, SLOT(update()));
	QObject::connect(paintbox, SIGNAL(redo()), this, SLOT(update()));
	QObject::connect(paintbox, SIGNAL(typeChange(ToolType)), this, SLOT(setToolType(ToolType)));

	parent->update();
	
	selection = new vector<CMeshO::FacePointer>();
	latest_event.pressure = 0.0;
	
	setToolType(COLOR_PAINT);
	
	glarea = parent;
	buffer_width = glarea->curSiz.width();
	buffer_height = glarea->curSiz.height();
	glarea->setMouseTracking(true);
	
	parent->setCursor(QCursor(QPixmap(":/images/cursor_paint.png"),1,1));
	return true;
}

void EditPaintPlugin::EndEdit(MeshModel &/*m*/, GLArea * /*parent*/) 
{
	QObject::disconnect(paintbox, SIGNAL(undo()), this, SLOT(update()));
	QObject::disconnect(paintbox, SIGNAL(redo()), this, SLOT(update()));
	glarea->setMouseTracking(false);
	if (zbuffer != NULL) delete zbuffer; zbuffer = NULL;
	delete paintbox;
	delete selection;
	delete dock;
}

void EditPaintPlugin::mousePressEvent(QMouseEvent * event, MeshModel &, GLArea * gla) 
{
	if (zbuffer != NULL) delete zbuffer; zbuffer = NULL;
	pushInputEvent(event->type(), event->pos(), event->modifiers(), 1, event->button(), gla);	
	gla->update();
}

void EditPaintPlugin::mouseMoveEvent(QMouseEvent* event, MeshModel & , GLArea * gla) 
{
	pushInputEvent(event->type(), event->pos(), event->modifiers(), latest_event.pressure, latest_event.button, gla);
	gla->update();
}

void EditPaintPlugin::mouseReleaseEvent(QMouseEvent * event, MeshModel &, GLArea * gla) 
{
	pushInputEvent(event->type(), event->pos(), event->modifiers(), 0, event->button(), gla);
	gla->update();
}

void EditPaintPlugin::tabletEvent(QTabletEvent * event, MeshModel & , GLArea * gla)
{
	event->accept();
	pushInputEvent(event->type(), event->pos(), event->modifiers(), event->pressure(), latest_event.button, gla);
	gla->update();
} 

void EditPaintPlugin::setToolType(ToolType t)
{
	current_type = t;
	
	switch(current_type)
	{
		case MESH_SELECT :
			current_options = EPP_PICK_FACES | EPP_DRAW_CURSOR;
			break;
			
		case COLOR_PAINT:
		case COLOR_NOISE:
		case COLOR_CLONE:
		case COLOR_SMOOTH :
		case MESH_SMOOTH :
			current_options = EPP_PICK_VERTICES | EPP_DRAW_CURSOR;
			break;
		
		case MESH_PULL:
		case MESH_PUSH:
			current_options = EPP_PICK_VERTICES | EPP_AVG_NORMAL | EPP_DRAW_CURSOR;
			break;
			
		case COLOR_FILL:
		case COLOR_GRADIENT:
		case COLOR_PICK :
		default:
			current_options = EPP_NONE;
	}
}

void EditPaintPlugin::setBrushSettings(int size, int opacity, int hardness)
{
	current_brush.size = size;
	current_brush.opacity = opacity;
	current_brush.hardness = hardness;
}

/**
 * Since only on a Decorate call it is possible to obtain correct values
 * from OpenGL, all operations are performed during the execution of this
 * method and not where mouse events are processed.
 *
 */
void EditPaintPlugin::Decorate(MeshModel &m, GLArea * gla) 
{	
	glarea = gla;
	
	if (!latest_event.valid || latest_event.processed) return;

	latest_event.processed = true;
	
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview_matrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projection_matrix);
	
	viewport[0] = viewport[1] = 0;
	viewport[2] = gla->curSiz.width(); viewport[3] = gla->curSiz.height();

	if (zbuffer == NULL)
	{
		zbuffer = new GLfloat[gla->curSiz.width()*gla->curSiz.height()];
		glReadPixels(0,0,gla->curSiz.width(), gla->curSiz.height(), GL_DEPTH_COMPONENT, GL_FLOAT, zbuffer);	
	}
	
	if (current_options & EPP_DRAW_CURSOR)
	{
		//TODO Compute only when needed!!!!!!!!!!!!!!
		current_brush.size = paintbox->getSize() * latest_event.pressure;
		current_brush.radius = (paintbox->getRadius() * m.cm.bbox.Diag() * 0.5) * latest_event.pressure;
		current_brush.opacity = paintbox->getOpacity() * latest_event.pressure;
		current_brush.hardness = paintbox->getHardness() * latest_event.pressure;
		
		if (paintbox->getSizeUnit() == 0) 
			drawSimplePolyLine(gla, latest_event.position, paintbox->getSize(), 
					(paintbox->getBrush() == CIRCLE) ? & circle : & square);
		else
			drawPercentualPolyLine(glarea, latest_event.gl_position, m, zbuffer, modelview_matrix, projection_matrix, viewport, current_brush.radius, 
					(paintbox->getBrush() == CIRCLE) ? & dense_circle : & dense_square );
	}
	
	if (latest_event.pressure > 0)
	{	
		if (current_options & EPP_PICK_VERTICES) 
		{ 
			vertices.clear(); 
			updateSelection(m, & vertices); 
		}else if (current_options & EPP_PICK_FACES) updateSelection(m);
		
		if (previous_event.pressure == 0)
		{
			/*************************
			 *     ON BRUSH DOWN     *
			 *************************/
			
			paintbox->setUndoStack(glarea);
			
			switch (current_type)
			{
				case COLOR_PAINT:
					{	
						painted_vertices.clear();
						QColor newcol = (latest_event.button == Qt::LeftButton) ? 
							paintbox->getForegroundColor() : 
							paintbox->getBackgroundColor();
						color[0] = newcol.red(); color[1] = newcol.green(); 
						color[2] = newcol.blue(); color[3] = newcol.alpha();
						paintbox->getUndoStack()->beginMacro("Color Paint");
						paint(& vertices);
					}
					break;
				
				case COLOR_CLONE :
					
					//Clone Source Acquisition
					if (latest_event.modifiers & Qt::ControlModifier || 
							latest_event.button == Qt::RightButton)
					{
						if (color_buffer != NULL) delete color_buffer;
						if (clone_zbuffer != NULL) delete clone_zbuffer;
						color_buffer = NULL, clone_zbuffer = NULL;
						glarea->getCurrentRenderMode().lighting = false;
						current_options &= ~EPP_DRAW_CURSOR; 
						glarea->update();
					}
					
					//Clone Painting
					else {
						//There's a new image to get
						if (paintbox->isNewPixmapAvailable())
						{
							paintbox->getPixmapBuffer(color_buffer, clone_zbuffer, buffer_width, buffer_height);
					//		source_delta.setX(buffer_width/2);
					//		source_delta.setY(buffer_height/2);
							source_delta = paintbox->getPixmapDelta();
							paintbox->setPixmapOffset(0, 0);
							apply_start = latest_event.position;
							painted_vertices.clear();
							paintbox->getUndoStack()->beginMacro("Color Clone");
														
							paint( & vertices);
						}else if (color_buffer != NULL)
						//There's still something in the buffer
						{
							painted_vertices.clear();
							source_delta = paintbox->getPixmapDelta();
							paintbox->setPixmapOffset(0, 0);
							apply_start = latest_event.position;
							paintbox->getUndoStack()->beginMacro("Color Clone");
							paint( & vertices);
						}
					}
					break;
					
				case COLOR_NOISE :
					painted_vertices.clear();
					noise_scale = paintbox->getNoiseSize() * 10 / m.cm.bbox.Diag();
					paintbox->getUndoStack()->beginMacro("Color Noise");
					break;
					
				case COLOR_GRADIENT:
					gradient_start = latest_event.position;
					break;
					
				case MESH_PULL:
				case MESH_PUSH :
					displaced_vertices.clear();
					paintbox->getUndoStack()->beginMacro("Mesh Sculpting");
					sculpt(m, & vertices);
					break;
					
				case COLOR_SMOOTH:
					paintbox->getUndoStack()->beginMacro("Color Smooth");
					smoothed_vertices.clear();
          tri::UnMarkAll(m.cm);
					break;
				case MESH_SMOOTH:
					paintbox->getUndoStack()->beginMacro("Mesh Smooth");
					smoothed_vertices.clear();
          tri::UnMarkAll(m.cm);
					break;
					
				default :
					break;
			}
		}else
		{
			/*************************
			 *     ON BRUSH MOVE     *
			 *************************/
			
			switch (current_type)
			{
			
				case COLOR_CLONE :
					paintbox->setPixmapOffset(latest_event.position.x() - apply_start.x(), latest_event.position.y() - apply_start.y());
					if (color_buffer != NULL) paint( & vertices);
					break;
				
				case COLOR_PAINT:
				case COLOR_NOISE :
					paint(& vertices);
					break;
					
				case COLOR_GRADIENT:
					drawLine(glarea, gradient_start, latest_event.position);
					break;
				
				case MESH_SELECT:
					for(vector<CMeshO::FacePointer>::iterator fpi = selection->begin(); fpi != selection->end(); ++fpi) 
					{
						if (latest_event.button == Qt::LeftButton)(*fpi)->SetS();
						else (*fpi)->ClearS();
					}
					break;
					
				case MESH_PUSH:
				case MESH_PULL:
					sculpt(m, & vertices);
					break;
				
				case COLOR_SMOOTH:
				case MESH_SMOOTH:
					smooth(& vertices);
					break;
					
				default :
					break;
			}
		}
		
	}else
	{
		if (previous_event.pressure > 0)
		{
			/*************************
			 *      ON BRUSH UP      *
			 *************************/
						
			switch (current_type)
			{
				case COLOR_FILL:
					CFaceO * face;
					if(GLPickTri<CMeshO>::PickNearestFace(latest_event.gl_position.x(), latest_event.gl_position.y(), m.cm, face, 2, 2))
					{
						fill(m, face);
						glarea->update();
					}
					break;

					
				case COLOR_PICK :
					{
						QColor color;
						CVertexO * temp_vert = NULL;
						if (paintbox->getPickMode() == 0) {
							if (getVertexAtMouse(m,temp_vert, latest_event.gl_position, modelview_matrix, projection_matrix, viewport)) 
							{	
								color.setRgb(temp_vert->C()[0], temp_vert->C()[1], temp_vert->C()[2], 255);
								(latest_event.button == Qt::LeftButton) ? paintbox->setForegroundColor(color) : paintbox->setBackgroundColor(color);
							} 
						} else 
						{
							GLubyte pixel[3];
							glReadPixels( latest_event.gl_position.x(), latest_event.gl_position.y(), 1, 1, GL_RGB, GL_UNSIGNED_BYTE,(void *)pixel);
							color.setRgb(pixel[0], pixel[1], pixel[2], 255);
							(latest_event.button == Qt::LeftButton) ? paintbox->setForegroundColor(color) : paintbox->setBackgroundColor(color);
						}
						paintbox->restorePreviousType();
					}
					break;
					
				case COLOR_GRADIENT:
					gradient(m, gla);
					gla->update();
					break;
					
				case COLOR_CLONE:
					if (latest_event.modifiers & Qt::ControlModifier || 
						latest_event.button == Qt::RightButton) {capture(); break;}
					else
						paintbox->movePixmapDelta(-latest_event.position.x() + apply_start.x(), -latest_event.position.y() + apply_start.y());
				case COLOR_SMOOTH :
				case COLOR_NOISE :
				case COLOR_PAINT:
				case MESH_SMOOTH :
				case MESH_PUSH:
				case MESH_PULL:
					paintbox->getUndoStack()->endMacro();
				default :
					break;	
			}
		}
	}
}


inline void EditPaintPlugin::smooth(vector< pair<CVertexO *, PickingData> > * vertices)
{
	QHash <CVertexO *, pair<Point3f, Color4b> > originals;
	
	Color4b newcol, destCol;
	int opac = paintbox->getOpacity();
	int decrease_pos = paintbox->getSmoothPercentual();
	int c_r, c_g, c_b;
	float p_x, p_y, p_z;
	float newpos[3]; 

	for (unsigned int k = 0; k < vertices->size(); k++) //forach selected vertices
	{
		pair<CVertexO *, PickingData> data = vertices->at(k);
		
		CVertexO * v = data.first;
		PickingData * vd = & data.second;

		pair<Point3f, Color4b> pc_data; //save its color and position
		
		for (int k = 0; k < 4; k++) pc_data.second[k] = v->C()[k];
		for (int k = 0; k < 3; k++) pc_data.first[k] = v->P()[k];

		if (!smoothed_vertices.contains(v))
		{
			if (paintbox->getCurrentType() == COLOR_SMOOTH) paintbox->getUndoStack()->push(new SingleColorUndo(v, v->C()));
			else paintbox->getUndoStack()->push(new SinglePositionUndo(v, v->P(), v->N()));
			smoothed_vertices.insert(v,v);
		} 
	
		if (!originals.contains(v)) originals.insert(v, pc_data); //save original color/position data
			
		CFaceO * one_face = v->VFp(); //one of the faces adjacent to the vertex
		int pos = v->VFi();  //index of vertex v on face one_face
		c_r = c_g = c_b = 0;
		p_x = p_y = p_z = 0;
		int count_me = 0;
		CFaceO * f  = one_face;
		
		do 
		{ /// calc the sum of the surrounding vertexes pos or and vertexes color
			CFaceO * temp = one_face->VFp(pos); //next face in VF list
			if (one_face != 0 && !one_face->IsD()) 
			{
				for (int k = 0; k < 3; k++) 
				{
					if (pos != k) 
					{ 
						CVertexO * v = one_face->V(k);
						Color4b tco = v->C();
						if (originals.contains(v)) 
						{
							pair<Point3f, Color4b> pc_data_k = originals[v];
									
							tco = pc_data_k.second;
							
							p_x += pc_data_k.first[0]; p_y += pc_data_k.first[1]; p_z += pc_data_k.first[2];
						}else if (paintbox->getCurrentType() == MESH_SMOOTH)
						{
							p_x += v->P()[0]; p_y += v->P()[1]; p_z += v->P()[2];		
						}
						c_r += tco[0]; c_g += tco[1]; c_b += tco[2]; 		
					}
				}
				
				pos=one_face->VFi(pos);
				count_me+=2;
			}
			one_face = temp;
		} while (one_face != f && one_face != 0);
			
		if (count_me > 0) /// calc the new color or pos 
		{ 
			float op = brush(paintbox->getBrush(), vd->distance, vd->rel_position.x(), vd->rel_position.y(), decrease_pos);
				
			if (paintbox->getCurrentType() == COLOR_SMOOTH)
			{ 
				newcol[0] = c_r/count_me;
				newcol[1] = c_g/count_me;
				newcol[2] = c_b/count_me;
				
				mergeColors((float)(op*opac)/100.0, newcol, v->C(), &destCol);
				
				v->C()[0] = destCol[0];
				v->C()[1] = destCol[1];
				v->C()[2] = destCol[2];
			} else 
			{		
				newpos[0] = p_x/(float)count_me;
				newpos[1] = p_y/(float)count_me;
				newpos[2] = p_z/(float)count_me;
				float po[3]; for (int lauf=0; lauf<3; lauf++) po[lauf]=v->P()[lauf];
				mergePositions((float)(op*opac)/100.0,newpos,po,newpos);
				
				for (int lauf=0; lauf<3; lauf++) v->P()[lauf]=newpos[lauf];
			}
		}
			
		if (paintbox->getCurrentType() == MESH_SMOOTH)
		{ 
			updateNormal(v);
		}
	}
}

inline void EditPaintPlugin::sculpt(MeshModel & m, vector< pair<CVertexO *, PickingData> > * vertices)
{
//	int opac = 1.0;
	float decrease_pos = paintbox->getHardness() / 100.0;
	float strength = m.cm.bbox.Diag() * paintbox->getDisplacement() / 1000.0;
	
	if (latest_event.button == Qt::RightButton) strength = - strength;
	
	if (normal[0] == normal[1] && normal[1] == normal[2] && normal[2] == 0) {
		return;
	}
	
	for (unsigned int k = 0; k < vertices->size(); k++)
	{
		pair<CVertexO *, PickingData> data = vertices->at(k);
		
		float op = brush(paintbox->getBrush(), data.second.distance, data.second.rel_position.x(), data.second.rel_position.y(), decrease_pos * 100);
		
		//TODO Precalculate this monster!
		float gauss = (strength * exp(-(op - 1.0)*(op - 1.0) * 8.0 ));
		
		if (!displaced_vertices.contains(data.first)) 
		{
			displaced_vertices.insert(data.first, pair<Point3f, float>(
				Point3f(data.first->P()[0], data.first->P()[1], data.first->P()[2]),
				gauss) );

			paintbox->getUndoStack()->push(new SinglePositionUndo(data.first, data.first->P(), data.first->N()));
			displaceAlongVector(data.first, normal, gauss);		
			updateNormal(data.first);

		} else if ((latest_event.button == Qt::RightButton) 
				? displaced_vertices[data.first].second > gauss 
				: displaced_vertices[data.first].second < gauss) 
		{
			displaced_vertices[data.first].second = gauss;
			Point3f temp = displaced_vertices[data.first].first;
			data.first->P()[0]=temp[0]; data.first->P()[1]=temp[1]; data.first->P()[2]=temp[2];
			displaceAlongVector(data.first, normal, gauss);	
			updateNormal(data.first);
						
		}

		delete zbuffer; zbuffer = NULL;
	}
}

inline void EditPaintPlugin::capture()
{
	color_buffer = new GLubyte[glarea->curSiz.width()*glarea->curSiz.height()*4];
	clone_zbuffer = new GLfloat[glarea->curSiz.width()*glarea->curSiz.height()];
	glReadPixels(0,0,glarea->curSiz.width(), glarea->curSiz.height(), GL_RGBA, GL_UNSIGNED_BYTE, color_buffer);
	glReadPixels(0,0,glarea->curSiz.width(), glarea->curSiz.height(), GL_DEPTH_COMPONENT, GL_FLOAT, clone_zbuffer);
	buffer_height = glarea->curSiz.height();
	buffer_width = glarea->curSiz.width();
	
	source_delta = latest_event.position;
	
	QImage image(glarea->width(), glarea->height(), QImage::Format_ARGB32); 
	for (int x = 0; x < glarea->width(); x++){
		for (int y = 0; y < glarea->height(); y++){
			int index = (y * glarea->width() + x)*4;
			image.setPixel(x, glarea->height() - y -1, qRgba((int)color_buffer[index], (int)color_buffer[index + 1], (int)color_buffer[index + 2], (int)color_buffer[index + 3]));
		}
	}
	glarea->getCurrentRenderMode().lighting = true;
	current_options |= EPP_DRAW_CURSOR;
	paintbox->setClonePixmap(image);
	paintbox->setPixmapDelta(source_delta.x(), source_delta.y());
		
//	paintbox->setPixmapCenter(-source_delta.x(), -source_delta.y());
	glarea->update();
}

inline bool EditPaintPlugin::accessCloneBuffer(int vertex_x, int vertex_y, vcg::Color4b & color)
{
	int y =  buffer_height - source_delta.y() +	(vertex_y + apply_start.y() - glarea->curSiz.height());
	int x =  source_delta.x() +	(vertex_x - apply_start.x());
	
	int index = y * buffer_width + x;
			
	if (x < buffer_width && y < buffer_height && x >= 0 && y >= 0)
	{
		if (clone_zbuffer[index] < 1.0)
		{
			index *= 4;
			color[0] = color_buffer[index]; color[1] = color_buffer[index + 1]; color[2] = color_buffer[index + 2], color[3] = color_buffer[index + 3];
			return true;
		}
	}
	return false;
}

/**
 * Painting, Cloning and Noise!
 */
inline void EditPaintPlugin::paint(vector< pair<CVertexO *, PickingData> > * vertices)
{
	int opac = current_brush.opacity; //TODO legacy assignment
	int decrease_pos = current_brush.hardness; //TODO legacy assignment

	for (unsigned int k = 0; k < vertices->size(); k++)
	{
		pair<CVertexO *, PickingData> data = vertices->at(k);
		
		float op = brush(paintbox->getBrush(), data.second.distance, data.second.rel_position.x(), data.second.rel_position.y(), decrease_pos);
		
		if (!painted_vertices.contains(data.first)) 
		{
			if (paintbox->getCurrentType() == COLOR_CLONE) 
				if (!accessCloneBuffer(data.second.position.x(), data.second.position.y(), color)) return;
			
			if (paintbox->getCurrentType() == COLOR_NOISE)
				computeNoiseColor(data.first, color);
			
			painted_vertices.insert(data.first, pair<Color4b, int>(
				Color4b(data.first->C()[0], data.first->C()[1], data.first->C()[2], data.first->C()[3]),
				(int)(op*opac)) );
			
			paintbox->getUndoStack()->push(new SingleColorUndo(data.first, data.first->C()));
			
			applyColor(data.first, color, (int)(op * opac  ));
			
		} else if (painted_vertices[data.first].second < (int)(op * opac)) 
		{
			painted_vertices[data.first].second = (int)(op * opac);
			Color4b temp = painted_vertices[data.first].first;
			data.first->C()[0]=temp[0]; data.first->C()[1]=temp[1]; data.first->C()[2]=temp[2];
			
			if (paintbox->getCurrentType() == COLOR_CLONE) 
				if (!accessCloneBuffer(data.second.position.x(), data.second.position.y(), color)) return;
			
			if (paintbox->getCurrentType() == COLOR_NOISE)
				computeNoiseColor(data.first, color);

			paintbox->getUndoStack()->push(new SingleColorUndo(data.first, data.first->C()));
			
			applyColor(data.first, color, (int)(op * opac) );	
		}
	}
}

inline void EditPaintPlugin::computeNoiseColor(CVertexO * vert, vcg::Color4b & col)
{
	float scaler = noise_scale; //parameter TODO to be cahced
	
	double noise;
	//if "veins on"
//	noise = vcg::math::Abs(vcg::math::Perlin::Noise(vert->P()[0] * scaler, vert->P()[1] * scaler, vert->P()[2] * scaler));
	//else
	noise = (vcg::math::Perlin::Noise(vert->P()[0] * scaler, vert->P()[1] * scaler, vert->P()[2] * scaler)+ 1) /2;

	Color4b forecolor(paintbox->getForegroundColor().red(), paintbox->getForegroundColor().green(), paintbox->getForegroundColor().blue(), paintbox->getForegroundColor().alpha());

	//TODO test code to be refactored 
	if (paintbox->getGradientType() == 0)
	{
		Color4b backcolor(paintbox->getBackgroundColor().red(), paintbox->getBackgroundColor().green(), paintbox->getBackgroundColor().blue(), paintbox->getBackgroundColor().alpha());
		for (int i = 0; i < 4; i ++)
			col[i] = forecolor[i] * noise + backcolor[i] * (1.0 - noise);
	}else 
	{
		for (int i = 0; i < 4; i ++)
			col[i] = forecolor[i] * noise + vert->C()[i] * (1.0 - noise);
	}
}

/** 
 * fills the mesh starting from face.
 * If face is selected, it will fill only the selected area, 
 * otherwise only the non selected area 
 */
inline void EditPaintPlugin::fill(MeshModel & ,CFaceO * face) 
{
	QHash <CFaceO *,CFaceO *> visited;
	QHash <CVertexO *,CVertexO *> temp;
	vector <CFaceO *>temp_po;
	bool who = face->IsS();
	temp_po.push_back(face);
	visited.insert(face,face);
	int opac=paintbox->getOpacity();
	QColor newcol = (latest_event.button == Qt::LeftButton) ? paintbox->getForegroundColor() : paintbox->getBackgroundColor();

	Color4b color(newcol.red(), newcol.green(), newcol.blue(), newcol.alpha());

	paintbox->getUndoStack()->beginMacro("Fill Color");
	
	for (unsigned int lauf2 = 0; lauf2 < temp_po.size(); lauf2++) {
		CFaceO * fac=temp_po.at(lauf2);
		if (who == fac->IsS()) {
			for (int lauf=0; lauf<3; lauf++) {
				if (!temp.contains(fac->V(lauf))) {
					temp.insert(fac->V(lauf),fac->V(lauf));
					
					paintbox->getUndoStack()->push(new SingleColorUndo(fac->V(lauf), fac->V(lauf)->C()));

					applyColor(fac->V(lauf), color , opac);
				}
			}

			vector <CFaceO *> surround;
			for (int lauf=0; lauf<3; lauf++) getSurroundingFacesVF(fac,lauf,&surround);

			for (unsigned int lauf3 = 0; lauf3 < surround.size(); lauf3++) {
				if (!visited.contains(surround[lauf3])) {
					temp_po.push_back(surround[lauf3]);
					visited.insert(surround[lauf3],surround[lauf3]);
				} 
			}
		}
	}
	
	paintbox->getUndoStack()->endMacro();
}

inline void EditPaintPlugin::gradient(MeshModel & m,GLArea * gla) {
	
	QPoint p = gradient_start - latest_event.position;

	QHash <CVertexO *,CVertexO *> temp;

	int opac = paintbox->getOpacity();
	QColor qc1 = paintbox->getForegroundColor();
	QColor qc2 = paintbox->getBackgroundColor();

	Color4b c1(qc1.red(), qc1.green(), qc1.blue(), qc1.alpha());
	Color4b c2(qc2.red(), qc2.green(), qc2.blue(), qc2.alpha());
	
	QPointF p1(gradient_start.x(),gla->curSiz.height() - gradient_start.y());
	QPointF p0(latest_event.gl_position);
	
	float x2=(p1.x()-p0.x());
	float y2=(p1.y()-p0.y());
	
	float l_square=x2*x2+y2*y2;

	CVertexO * vertex;
	CMeshO::FaceIterator fi;
	double dx,dy,dz;
	Color4b merger;
	bool tutti = !hasSelected(m);
	float radius=sqrt((float)(p.x()*p.x()+p.y()*p.y()));
	
	paintbox->getUndoStack()->beginMacro("Gradient");
	
	int gradient_type=paintbox->getGradientType();
	int gradient_form=paintbox->getGradientForm();
	for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi) 
		if (!(*fi).IsD() && (tutti || (*fi).IsS()))
		for (int lauf=0; lauf<3; lauf++) {
			if (!temp.contains((*fi).V(lauf))) {
				vertex=(*fi).V(lauf);
				temp.insert(vertex,vertex);
				gluProject(vertex->P()[0],vertex->P()[1],vertex->P()[2],modelview_matrix, projection_matrix, viewport,&dx,&dy,&dz);
			
				paintbox->getUndoStack()->push(new SingleColorUndo(vertex, vertex->C()));
				
				if (gradient_form==0) {
					double r=(dx-p0.x())*(p1.x()-p0.x())+(dy-p0.y())*(p1.y()-p0.y());
					r=r/l_square;
					
					float px=p0.x()+r*(p1.x()-p0.x());
					float py=p0.y()+r*(p1.y()-p0.y());
				
					px=px-dx;
					py=py-dy;
	
					if (gradient_type==0) {
						if (r>=0 && r<=1) { 
							mergeColors(r,c1,c2,&merger);
							applyColor(vertex,merger,opac);
						} else if (r>1) {
							applyColor(vertex,c1,opac);
						} else if (r<0) {
							applyColor(vertex,c2,opac);
						}
					} else {
						if (r>=0 && r<=1) { 
							applyColor(vertex,c1,(int)((opac * 0.01) * r * 100.0));
						} else if (r>1) {
							applyColor(vertex,c1,opac);
						}
					}
				} else {
					float x0=(dx-p1.x());
					float y0=(dy-p1.y());
					float bla0=x0*x0+y0*y0;
					if (bla0<radius*radius && radius>0) {
						float r=1.0-sqrt(bla0)/sqrt(radius*radius);
						if (gradient_type==0) {
							mergeColors(r,c1,c2,&merger);
							applyColor(vertex,merger,opac);
						} else {
							applyColor(vertex,c1,(int)((opac * 0.01) * r * 100.0));
						}
					}
				}
			}
		}
	paintbox->getUndoStack()->endMacro();
}


/**
 * Updates currently selected faces by using one of the
 * two methods. Just a new version of computeBrushedFaces
 * 
 * It's inlined because it's only called once, inside Decorate.
 * Should it be called in other places, the inlining must be removed!
 */ 
inline void EditPaintPlugin::updateSelection(MeshModel &m, vector< pair<CVertexO *, PickingData> > * vertex_result)
{
	vector<CMeshO::FacePointer>::iterator fpi;
	vector<CMeshO::FacePointer> temp; //TODO maybe temp can be placed inside the class for better performance
	
	vector <CFaceO *> surround; /*< surrounding faces of a given face*/
	surround.reserve(6);
	
	if (current_options & EPP_AVG_NORMAL ) normal = Point3f(0.0, 0.0, 0.0);
	
  tri::UnMarkAll(m.cm);
	
	if (selection->size() == 0) {
		CMeshO::FaceIterator fi;
		temp.reserve(m.cm.fn); //avoid unnecessary expansions
        for(fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi){ 
        	if(!(*fi).IsD()) {
        		temp.push_back((&*fi));
        	}
        } 
	} else 
	{
		temp.reserve(selection->size()); //avoid unnecessary expansions
		vertex_result->reserve(selection->size());
		for(fpi = selection->begin();fpi != selection->end(); ++fpi) 
		{
			temp.push_back(*fpi);
		}
	}
	
	selection->clear();

	QPointF gl_cursorf = QPointF(latest_event.gl_position);
	QPointF gl_prev_cursorf = QPointF(previous_event.gl_position);

	QPointF p[3],z[3]; //p stores vertex coordinates in screen space, z the corresponding depth value
	double tx,ty,tz;

	bool backface = paintbox->getPaintBackFace();
	bool invisible = paintbox->getPaintInvisible();
	bool percentual = paintbox->getSizeUnit() == 1;
	
	double EPSILON = 0.003;
	
	double scale_fac = 1.0;
	float fov = 1.0;
	double distance[3];
	
	if (percentual)
	{
		double dX, dY, dZ;

		fastMultiply(0,0,0,modelview_matrix,&dX,&dY,&dZ);
		fastMultiply(0,1,0,modelview_matrix,&tx,&ty,&tz);
		
		scale_fac=sqrt((ty-dY)*(ty-dY)+(tx-dX)*(tx-dX)+(tz-dZ)*(tz-dZ));

		/** to get the correct radius depending on distance but on fov too */
		float fo = glarea->getFov()*0.5;
		fov = 1.0/tan(fo*M_PI/180.0)*0.5;
	}
		
	for (unsigned int k = 0; k < temp.size(); k++) //for each face 
	{
		CFaceO * fac = temp.at(k);
		bool intern = false;
		int checkable = 0; /*< counts how many valid vertices there are in this face*/
		
		for (int i = 0; i < 3; i++) //for each vertex defining the face
		{	
			if (gluProject((fac)->V(i)->P()[0], (fac)->V(i)->P()[1], (fac)->V(i)->P()[2],
					modelview_matrix, projection_matrix, viewport, &tx, &ty, &tz) == GL_TRUE) checkable++; //if gluProjection works
			
			if (tz < 0 || tz > 1) checkable--; //but if outside depth bounds
			
			p[i].setX(tx); p[i].setY(ty); //store the position of the vertex on the screen
			
			//if the vertex is projected within the screen (i.e. it's visible)
			if (tx>=0 && tx<viewport[2] && ty>=0 && ty<viewport[3])
			{
				z[i].setX(tz); //the screen depth of the point
				z[i].setY((float)zbuffer[(int)(((int)ty)*viewport[2]+(int)tx)]); //the screen depth of the closest point at the same coors
			}else 
			{ 
				z[i].setX(1); //far
				z[i].setY(0); //near
			}
			
			if (percentual){
				double dx,dy,dz; // distance
				fastMultiply((fac)->V(i)->P()[0],(fac)->V(i)->P()[1],(fac)->V(i)->P()[2], modelview_matrix, &dx, &dy, &dz);
				distance[i]=dz;
			}
		}

		if (backface || isFront(p[0],p[1],p[2]))
		{
			for (int j=0; j<3; j++) if (invisible || (z[j].x() <= z[j].y() + EPSILON))
			{
				PickingData vd; //TODO make it a pointer
				
				float radius = percentual ? vcg::math::Abs(current_brush.radius * scale_fac * viewport[3] * fov / distance[j]) : current_brush.size;
				
				if (isIn(gl_cursorf, gl_prev_cursorf, p[j].x(), p[j].y(), radius , & vd.distance, vd.rel_position)) 
				{
					intern = true;
					if (vertex_result == NULL) continue;
          else if (!tri::IsMarked(m.cm,fac->V(j)))
					{
						vd.position.setX((int)p[j].x()); vd.position.setY((int)p[j].y());
						pair<CVertexO *, PickingData> data(fac->V(j), vd);
						vertex_result->push_back(data);
						
						if (current_options & EPP_AVG_NORMAL ) normal += fac->V(j)->N();
						
            tri::Mark(m.cm,fac->V(j));
					}
				}
				
				if (vertex_result == NULL && !intern && lineHitsCircle(p[j],p[(j+1)%3], gl_cursorf, radius)) 
				{
					intern = true; 
					continue;
				}
			}
		
			if (checkable==3 && !intern && (pointInTriangle(gl_cursorf,p[0],p[1],p[2]) || pointInTriangle(gl_prev_cursorf,p[0],p[1],p[2]))) 
			{
				intern = true;
			} 
		}
		
    if (intern && !tri::IsMarked(m.cm,fac))
		{
      tri::Mark(m.cm,fac);
			selection->push_back(fac);
			surround.clear();
			for (int lauf=0; lauf<3; lauf++) getSurroundingFacesVF(fac,lauf,&surround);

			for (unsigned int lauf3=0; lauf3<surround.size(); lauf3++)
			{
        if (!tri::IsMarked(m.cm,surround[lauf3]))
				{
						temp.push_back(surround[lauf3]);
				} 
			}
		}
	} //end of for each face loop
	
	if (current_options & EPP_AVG_NORMAL ) normal /= vertex_result->size();
}

/**
 * Request an async repainting of the glarea
 * 
 * This slot is connected to undo and redo
 */
void EditPaintPlugin::update()
{
	glarea->update();
}


/*********OpenGL Drawing Routines************/


/** 
 * draws the xor-line 
 */
void drawLine(GLArea * gla, QPoint & start, QPoint & cur) {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0,gla->curSiz.width(),gla->curSiz.height(),0,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_LOGIC_OP);
	float wi;
	glGetFloatv(GL_LINE_WIDTH,&wi);
	glLineWidth(4);
	glLogicOp(GL_XOR);
	glColor3f(1,1,1);
	glBegin(GL_LINES);
		glVertex2f(start.x(),start.y());
		glVertex2f(cur.x(),cur.y());
	glEnd();
	glPopAttrib();
	glPopMatrix(); // restore modelview
	glLineWidth(wi);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void drawSimplePolyLine(GLArea * gla, QPoint & cur, float scale, vector<QPointF> * points)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0,gla->curSiz.width(),gla->curSiz.height(),0,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_LOGIC_OP);
	glLogicOp(GL_XOR);
	glColor3f(1,1,1);
	
	glBegin(GL_LINE_LOOP);
	
	for (unsigned int k = 0; k < points->size(); k++)
	{
		glVertex2f(cur.x() + ( points->at(k).x() * scale ), cur.y() + ( points->at(k).y() * scale ));
	}
	
	glEnd();

	glDisable(GL_LOGIC_OP);
	glPopAttrib();
	glPopMatrix(); // restore modelview
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void drawPercentualPolyLine(GLArea * gla, QPoint &mid, MeshModel &m, GLfloat* pixels,
		double* modelview_matrix, double* projection_matrix, GLint* viewport, float scale, vector<QPointF> * points)
{
	double dX, dY, dZ; //near
	double dX2, dY2, dZ2; //far
			
	gluUnProject ((double) mid.x(), mid.y(), 0.0, modelview_matrix, projection_matrix, viewport, &dX, &dY, &dZ);
	gluUnProject ((double) mid.x(), mid.y(), 1.0, modelview_matrix, projection_matrix, viewport, &dX2, &dY2, &dZ2);

	glPushMatrix();
	glLoadIdentity();
	gluLookAt(dX,dY,dZ, dX2,dY2,dZ2, 1,0,0);
	
	double mvmatrix2[16];
	glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix2);
	glPopMatrix();
		
	double tx,ty,tz;
	double tx2,ty2,tz2;
		
	double inv_mvmatrix[16];
		
	Matrix44d temp(mvmatrix2);
	Invert(temp);
	
	for (int k=0; k<16; k++) 
		inv_mvmatrix[k]=temp[k/4][k%4];
			
	double a,b,c;
	double a2,b2,c2;

	int STEPS = 30;
	int DSCALER = -7;
	
	float diag = m.cm.bbox.Diag() * DSCALER;

	float radius = scale; //TODO leftover
	
	QPointF * proj_points = new QPointF[points->size()]; //TODO hope it works on windows!
	
	for (unsigned int i = 0; i < points->size(); i++)
	{
		/** calcs the far point of the line */
		fastMultiply(points->at(i).x() * radius, points->at(i).y() * radius, diag, inv_mvmatrix, &tx, &ty, &tz);
		
		gluProject(tx,ty,tz,modelview_matrix, projection_matrix, viewport, &a,&b,&c);
		
		/** calcs the near point of the line */
		fastMultiply(points->at(i).x() * radius, points->at(i).y() * radius, 0, inv_mvmatrix, &tx2, &ty2, &tz2);
		
		gluProject(tx2,ty2,tz2,modelview_matrix, projection_matrix, viewport, &a2,&b2,&c2);
		double da=(a-a2);// /(double)STEPS;
		double db=(b-b2);// /(double)STEPS;
		double dc=(c-c2);// /(double)STEPS;
		double pix_x=a2;
		double pix_y=b2;
		double pix_z=c2;
		
		/** to search with quicksearch the nearset zbuffer value in the line */
		for (int k = 0; k < STEPS; k++)
		{
			double inv_yy = gla->curSiz.height()-pix_y;
			double zz=999;
			
			if ((int)pix_x>=0 && (int)pix_x<gla->curSiz.width() && (int)pix_y>=0 && (int)pix_y<gla->curSiz.height()) 
				zz=(GLfloat)pixels[(int)(((int)pix_y)*gla->curSiz.width()+(int)pix_x)];
			da=da/2.0;
			db=db/2.0;
			dc=dc/2.0;
			if (fabsf(zz-pix_z)<0.001) 
			{
				proj_points[i] = QPointF(pix_x, inv_yy);
				break;
			} else if (zz>pix_z) 
			{
				pix_x+=da;
				pix_y+=db;
				pix_z+=dc;
			} else
			{
				pix_x-=da;
				pix_y-=db;
				pix_z-=dc;
			}
			
			if (k == STEPS-1) 
			{ 
				proj_points[i] = QPointF(pix_x,inv_yy);
				break; 
			}
		}
	}
		
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0,gla->curSiz.width(),gla->curSiz.height(),0,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_LOGIC_OP);
	glLogicOp(GL_XOR);
	glColor3f(1,1,1);
	
	/** draws the circle */
	glBegin(GL_LINE_LOOP);
	
	for (unsigned int i = 0; i < points->size(); i++)
	{
		glVertex2f(proj_points[i].x(), proj_points[i].y());
	}
	
	glEnd();
	
	glDisable(GL_COLOR_LOGIC_OP);
	glPopAttrib();
	glPopMatrix(); // restore modelview
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	delete proj_points;
}

//each side is divided in given segments
void generatePolygon(std::vector<QPointF> & vertices, int sides, int segments)
{	
	float step = sides / 2.0;
	
	float start_angle = M_PI / sides;
	
	for (int k = 0; k < sides; k++)
	{
		vertices.push_back(QPointF(sin((M_PI * (float)k / step) + start_angle), cos((M_PI * (float)k / step) + start_angle)));
	}
	
	if (segments > 1)
	{
		int sk;
		for (int k = 0; k < sides; k++  )
		{
			sk = (k + 1) % sides;
			QPointF kv = vertices.at(k);
			QPointF skv =vertices.at(sk);
			QPointF d = (skv - kv)/segments;
			vertices.push_back(kv);
			for (int j = 1; j < segments; j++)
			{
				vertices.push_back(kv + d * j);
			}
		}
		vertices.erase(vertices.begin(), vertices.begin() + sides);
	}
}

//TODO This should be done statically

/**
 * Generates the same circle points that Gfrei's algorithm does
 */ 
void generateCircle(std::vector<QPointF> & vertices, int segments)
{
	return generatePolygon(vertices, segments, 1);
}

void generateSquare(std::vector<QPointF> & vertices, int segments)
{
	return generatePolygon(vertices, 4, segments);	
}

