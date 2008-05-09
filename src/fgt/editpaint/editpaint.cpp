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
#include <vcg/complex/trimesh/update/topology.h>

using namespace std;
using namespace vcg;

EditPaintPlugin::EditPaintPlugin() 
{
	actionList << new QAction(QIcon(":/images/paintbrush-22.png"),"Z-painting", this);
	QAction *editAction;
	foreach(editAction, actionList)
	editAction->setCheckable(true);
	zbuffer = NULL;
	color_buffer = NULL;
	mark = 0;
	disable_decorate = false;
	circle = generateCircle();
	dense_circle = generateCircle(64);
	square = generateSquare();
	dense_square = generateSquare(16);
}

EditPaintPlugin::~EditPaintPlugin() 
{
	delete circle;
	delete dense_circle;
}

QList<QAction *> EditPaintPlugin::actions() const {
	return actionList;
}

const QString EditPaintPlugin::Info(QAction *action) {
	if( action->text() != tr("Z-painting") ) assert (0);
	return tr("Improved Painting");
}

const PluginInfo &EditPaintPlugin::Info() {
	static PluginInfo ai; 
	ai.Date=tr(__DATE__);
	ai.Version = tr("0.2");
	ai.Author = ("Daniele Bernabei & Andreas Gfrei");
	return ai;
} 

void EditPaintPlugin::StartEdit(QAction *, MeshModel& m, GLArea * parent) 
{
	dock = new QDockWidget(parent->window());
	paintbox = new Paintbox(dock);
	dock->setAllowedAreas(Qt::NoDockWidgetArea);
	dock->setWidget(paintbox);
	QPoint p=parent->window()->mapToGlobal(QPoint(0,0));
	dock->setGeometry(-5+p.x()+parent->window()->width()-paintbox->width(),p.y(),paintbox->width(),200);
	dock->setFloating(true);
	dock->setVisible(true);

	tri::UpdateBounding<CMeshO>::Box(m.cm);
	
	m.updateDataMask(MeshModel::MM_VERTFACETOPO);
	m.updateDataMask(MeshModel::MM_FACEMARK);
	m.updateDataMask(MeshModel::MM_VERTMARK);
	
	if ((m.currentDataMask & MeshModel::IOM_VERTCOLOR) == 0)
	{
		Color4b color(100, 100, 100, 255);
		for (CMeshO::VertexIterator i = m.cm.vert.begin(); i != m.cm.vert.end(); i++) (*i).C() = color;
	}
	
	parent->mm()->ioMask|=MeshModel::IOM_VERTQUALITY;
	parent->mm()->ioMask|=MeshModel::IOM_VERTCOLOR;
	parent->getCurrentRenderMode().colorMode=vcg::GLW::CMPerVert;
	
	QObject::connect(paintbox, SIGNAL(undo()), this, SLOT(update()));
	QObject::connect(paintbox, SIGNAL(redo()), this, SLOT(update()));

	parent->update();
	
	selection = new vector<CMeshO::FacePointer>();
	
	glarea = parent;
}

void EditPaintPlugin::EndEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/) 
{
	QObject::disconnect(paintbox, SIGNAL(undo()), this, SLOT(update()));
	QObject::disconnect(paintbox, SIGNAL(redo()), this, SLOT(update()));
	if (zbuffer != NULL) delete zbuffer; zbuffer = NULL;
	delete paintbox;
	delete selection;
	delete dock;
}

void EditPaintPlugin::mousePressEvent(QAction * , QMouseEvent * event, MeshModel &, GLArea * gla) 
{
	if (zbuffer != NULL) delete zbuffer; zbuffer = NULL;
	event_queue.enqueue(*event);
	gla->update();
}

void EditPaintPlugin::mouseMoveEvent(QAction *, QMouseEvent* event, MeshModel & m, GLArea * gla) 
{
	event_queue.enqueue(*event);
	gla->update();
}

void EditPaintPlugin::mouseReleaseEvent  (QAction *,QMouseEvent * event, MeshModel &, GLArea * gla) 
{
	event_queue.enqueue(*event);
	gla->update();
}

void EditPaintPlugin::projectCursor(MeshModel & m, GLArea * gla)
{
	if (paintbox->getSizeUnit() == 0){ 
		if (paintbox->getBrush() == CIRCLE) drawSimplePolyLine(gla, cursor, paintbox->getSize(), circle);
		if (paintbox->getBrush() == SQUARE) drawSimplePolyLine(gla, cursor, paintbox->getSize(), square);
	}
	if (paintbox->getSizeUnit() == 1) { 
		if (paintbox->getBrush() == CIRCLE) drawPercentualPolyLine(gla, gl_cursor, m, zbuffer, modelview_matrix, projection_matrix, viewport, paintbox->getRadius(), dense_circle);				
		if (paintbox->getBrush() == SQUARE) drawPercentualPolyLine(gla, gl_cursor, m, zbuffer, modelview_matrix, projection_matrix, viewport, paintbox->getRadius(), dense_square);
	}
}

void EditPaintPlugin::setToolType(ToolType t)
{
	current_type = t;
	switch(current_type)
	{
		case COLOR_PAINT:
			current_settings = EPP_PICK_VERTICES | EPP_DRAW_CURSOR;
		default:
			current_settings = EPP_NONE;
	}
}

/**
 * Since only on a Decorate call it is possible to obtain correct values
 * from OpenGL, all operations are performed during the execution of this
 * method and not where mouse events are processed.
 * 
 * TODO If update() is called from within the loop on events, there will be a "read ahead" of
 * events that will disrupt the status
 */
void EditPaintPlugin::Decorate(QAction*, MeshModel &m, GLArea * gla) 
{
	if (disable_decorate) return;
	
	glarea = gla;
	
	if (event_queue.isEmpty()) return;
	
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview_matrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projection_matrix);
	
	viewport[0] = viewport[1] = 0;
	viewport[2] = gla->curSiz.width(); viewport[3] = gla->curSiz.height();

	//TODO We maybe can avoid this, I don't know
	if (zbuffer == NULL)
	{
		zbuffer = new GLfloat[gla->curSiz.width()*gla->curSiz.height()];
		glReadPixels(0,0,gla->curSiz.width(), gla->curSiz.height(), GL_DEPTH_COMPONENT, GL_FLOAT, zbuffer);	
	}

	while (!event_queue.isEmpty()) 
	{
		QMouseEvent event = event_queue.dequeue();
		
		if (event.type() == QEvent::MouseButtonPress) button = event.button();
		
		prev_cursor = cursor; //TODO what if the cursor was not initialized??? 
		cursor = event.pos();
		gl_cursor.setX(cursor.x()); gl_cursor.setY(gla->curSiz.height() - cursor.y());
		
		switch (paintbox->getCurrentType())
		{
			case COLOR_FILL :
				if (event.type() == QEvent::MouseButtonRelease) 
				{
					CFaceO * face;
					if(GLPickTri<CMeshO>::PickNearestFace(gl_cursor.x(), 
						gl_cursor.y(), m.cm, face, 2, 2))
					{
						fill(m, face);
						gla->update();
					}
				}
				continue;
				
			case COLOR_PICK :
				if (event.type() == QEvent::MouseButtonRelease)
				{
					QColor color;
					CVertexO * temp_vert=0;
					if (paintbox->getPickMode() == 0) {
						if (getVertexAtMouse(m,temp_vert, gl_cursor, modelview_matrix, projection_matrix, viewport)) 
						{	
							color.setRgb(temp_vert->C()[0], temp_vert->C()[1], temp_vert->C()[2], 255);
							(button == Qt::LeftButton) ? paintbox->setForegroundColor(color) : paintbox->setBackgroundColor(color);
						} 
					} else 
					{
						GLubyte pixel[3];
						glReadPixels( gl_cursor.x(), gl_cursor.y(), 1, 1, GL_RGB, GL_UNSIGNED_BYTE,(void *)pixel);
						color.setRgb(pixel[0], pixel[1], pixel[2], 255);
						(button == Qt::LeftButton) ? paintbox->setForegroundColor(color) : paintbox->setBackgroundColor(color);
					}
					paintbox->restorePreviousType(); 
				}			 
				continue;
				
			case COLOR_GRADIENT :
				if (event.type() == QEvent::MouseButtonPress) {start_cursor.setX(cursor.x()); start_cursor.setY(cursor.y()); }
				else if (event.type() == QEvent::MouseMove ) drawLine(gla, start_cursor, cursor);
				else if (event.type() == QEvent::MouseButtonRelease){
					gradient(m, gla);
					gla->update();
				}
				continue;
				
			default :
				break;
		}
		
		
		vertices.clear(); //TODO don't need vertices for face selection!
		
		switch (paintbox->getCurrentType())
		{
			case MESH_SELECT:
				projectCursor(m, glarea);
				updateSelection(m);			
						
				if (event.type() == QEvent::MouseButtonPress)
				{
					paintbox->getUndoStack()->beginMacro("Select");
				}else if (event.type() == QEvent::MouseButtonRelease) 
				{
					paintbox->getUndoStack()->endMacro(); 
				} else if (event.type() == QEvent::MouseMove){
					for(vector<CMeshO::FacePointer>::iterator fpi = selection->begin(); fpi != selection->end(); ++fpi) 
					{
						paintbox->getUndoStack()->push(new SelectionUndo((*fpi), (*fpi)->IsS()));
						if (button == Qt::LeftButton)(*fpi)->SetS();
						else (*fpi)->ClearS();
					}
				}
				break;
			
			case MESH_PULL :
				projectCursor(m, glarea);
				updateSelection(m, & vertices);			
				
				if (event.type() == QEvent::MouseButtonPress) { displaced_vertices.clear(); paintbox->getUndoStack()->beginMacro("Pull"); }
				else if (event.type() == QEvent::MouseMove){ 
					sculpt(m, & vertices);}
				else if (event.type() == QEvent::MouseButtonRelease) paintbox->getUndoStack()->endMacro();
				break;
				
			case MESH_PUSH : // glarea->setLight(false);
			
				projectCursor(m, glarea);
				updateSelection(m, & vertices);			
				
				if (event.type() == QEvent::MouseButtonPress) { displaced_vertices.clear(); paintbox->getUndoStack()->beginMacro("Push"); }
				else if (event.type() == QEvent::MouseMove) sculpt(m, & vertices);
				else if (event.type() == QEvent::MouseButtonRelease) paintbox->getUndoStack()->endMacro();
				break;
				
				
			case COLOR_PAINT :
				projectCursor(m, glarea);
				updateSelection(m, & vertices);
				
				if (event.type() == QEvent::MouseButtonPress)
				{
					paintbox->getUndoStack()->beginMacro("Paint");
					visited_vertices.clear();
					paint(& vertices);
				}
				else if (event.type() == QEvent::MouseMove) paint(& vertices);
				else if (event.type() == QEvent::MouseButtonRelease) paintbox->getUndoStack()->endMacro();
				
				break;
			
			case COLOR_SMOOTH:
			case MESH_SMOOTH:
				projectCursor(m, glarea);
				updateSelection(m, & vertices);
				
				if (event.type() == QEvent::MouseButtonPress)
				{
					paintbox->getUndoStack()->beginMacro("Smooth");
					mark++; //TODO BAD mark usage
				}
				else if (event.type() == QEvent::MouseMove) smooth(& vertices);
				else if (event.type() == QEvent::MouseButtonRelease) paintbox->getUndoStack()->endMacro();				
				break;
				
			case COLOR_CLONE :
				if (event.type() == QEvent::MouseButtonPress && button == Qt::RightButton)
				{
					if (color_buffer != NULL) delete color_buffer;
					disable_decorate = true;
					glarea->update();
					color_buffer = new GLubyte[gla->curSiz.width()*gla->curSiz.height()*3];
					clone_zbuffer = new GLfloat[gla->curSiz.width()*gla->curSiz.height()];
					glReadPixels(0,0,gla->curSiz.width(), gla->curSiz.height(), GL_RGB, GL_UNSIGNED_BYTE, color_buffer);
					glReadPixels(0,0,gla->curSiz.width(), gla->curSiz.height(), GL_DEPTH_COMPONENT, GL_FLOAT, clone_zbuffer);
					start_cursor.setX(gl_cursor.x()); start_cursor.setY(gl_cursor.y()); //storing now source click position
					
					//TODO refactor
					QImage image(glarea->width(), glarea->height(), QImage::Format_RGB32); 
					for (int x = 0; x < glarea->width(); x++){
						for (int y = 0; y < glarea->height(); y++){
							int index = (y * glarea->width() + x)*3;
							image.setPixel(x, glarea->height() - y, qRgb((int)color_buffer[index], (int)color_buffer[index + 1], (int)color_buffer[index + 2]));
						}
					}
					
					paintbox->setClonePixmap(image);
					paintbox->setPixmapCenter(-cursor.x(), -cursor.y());
					disable_decorate = false;
				}else if (event.type() == QEvent::MouseButtonPress && button == Qt::LeftButton)
				{
					projectCursor(m, glarea);
					updateSelection(m, & vertices);
					paintbox->getUndoStack()->beginMacro("Clone");
					visited_vertices.clear();
					start_cursor.setX(start_cursor.x() - gl_cursor.x()); start_cursor.setY(start_cursor.y() - gl_cursor.y());
				
				}else if (event.type() == QEvent::MouseMove && button == Qt::LeftButton)
				{
					projectCursor(m, glarea);
					updateSelection(m, & vertices);
					paintbox->setPixmapCenter(-cursor.x() - start_cursor.x(), -cursor.y() + start_cursor.y() );
					paint( & vertices);
				}else if (event.type() == QEvent::MouseButtonRelease && button == Qt::LeftButton)
				{
					projectCursor(m, glarea);
					updateSelection(m, & vertices);
					paintbox->getUndoStack()->endMacro();
				}
				break;
			case COLOR_NOISE :
				//TODO Perlin noise application
				
			default :
				break;
		}
	}
}


void EditPaintPlugin::smooth(vector< pair<CVertexO *, VertexDistance> > * vertices)
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
		pair<CVertexO *, VertexDistance> data = vertices->at(k);
		
		CVertexO * v = data.first;
		VertexDistance * vd = & data.second;

		pair<Point3f, Color4b> pc_data; //save its color and position
		
		for (int k = 0; k < 4; k++) pc_data.second[k] = v->C()[k];
		for (int k = 0; k < 3; k++) pc_data.first[k] = v->P()[k];

		if (v->IMark() != mark)
		{
			if (paintbox->getCurrentType() == COLOR_SMOOTH) paintbox->getUndoStack()->push(new SingleColorUndo(v, v->C()));
			else paintbox->getUndoStack()->push(new SinglePositionUndo(v, v->P()));
			v->IMark() = mark;
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

//TODO Begin modularization of paint to allow different tools to leverage the same architecture
void EditPaintPlugin::sculpt(MeshModel & m, vector< pair<CVertexO *, VertexDistance> > * vertices)
{
	int opac = 1.0;
	int decrease_pos = paintbox->getHardness();
	float strength = m.cm.bbox.Diag() * paintbox->getDisplacement() / 1000.0;
	
	Point3f normal(0.0, 0.0, 0.0);
	
	//TODO Normal should be calculated during updateSelection
	for (unsigned int k = 0; k < vertices->size(); k++)
	{
		normal += vertices->at(k).first->N() / vertices->size();
	}
	
	if (normal[0] == normal[1] && normal[1] == normal[2] && normal[2] == 0) {
		qDebug() << "Prayer culo";
		return;
	}
	
	qDebug() << strength;
		
	qDebug() << normal[0] << " " << normal[1] << " " << normal[2];
	
	for (unsigned int k = 0; k < vertices->size(); k++)
	{
		pair<CVertexO *, VertexDistance> data = vertices->at(k);
		
		float op = brush(paintbox->getBrush(), data.second.distance, data.second.rel_position.x(), data.second.rel_position.y(), decrease_pos);
		
		if (!displaced_vertices.contains(data.first)) 
		{
			displaced_vertices.insert(data.first, pair<Point3f, float>(
				Point3f(data.first->P()[0], data.first->P()[1], data.first->P()[2]),
				opac * op) );
			
			paintbox->getUndoStack()->push(new SinglePositionUndo(data.first, data.first->P()));

			qDebug() << "Position before push" << data.first->P()[0] << " " << data.first->P()[1] << " " << data.first->P()[2];
			qDebug() << "strength" << strength;
			
			displaceAlongVector(data.first, normal, (opac * op) * strength);
			
			qDebug() << "Position after push" << data.first->P()[0] << " " << data.first->P()[1] << " " << data.first->P()[2];
			
		} else if (displaced_vertices[data.first].second < opac * op) 
		{
			displaced_vertices[data.first].second = opac * op;
			Point3f temp = displaced_vertices[data.first].first;
			data.first->P()[0]=temp[0]; data.first->P()[1]=temp[1]; data.first->P()[2]=temp[2];
			displaceAlongVector(data.first, normal, (opac * op)* strength);	
		}
	}
	
	for (unsigned int k = 0; k < vertices->size(); k++) updateNormal(vertices->at(k).first);
}

/**
 * Painting and Cloning
 */
void EditPaintPlugin::paint(vector< pair<CVertexO *, VertexDistance> > * vertices)
{
	int opac = paintbox->getOpacity();
	int decrease_pos = paintbox->getHardness();
	
	int index = 0;
	Color4b color;
	
	if (paintbox->getCurrentType() == COLOR_PAINT){
		QColor newcol = (button == Qt::LeftButton) ? paintbox->getForegroundColor() : paintbox->getBackgroundColor();
		color[0] = newcol.red(); color[1] = newcol.green(); color[2] = newcol.blue(); color[3] = newcol.alpha();
	}
	
	for (unsigned int k = 0; k < vertices->size(); k++)
	{
		pair<CVertexO *, VertexDistance> data = vertices->at(k);
		
		float op = brush(paintbox->getBrush(), data.second.distance, data.second.rel_position.x(), data.second.rel_position.y(), decrease_pos);
		
		if (!visited_vertices.contains(data.first)) 
		{
			visited_vertices.insert(data.first, pair<Color4b, float>(
				Color4b(data.first->C()[0], data.first->C()[1], data.first->C()[2], data.first->C()[3]),
				op*opac) );
			
			paintbox->getUndoStack()->push(new SingleColorUndo(data.first, data.first->C()));
			
			if (paintbox->getCurrentType() == COLOR_CLONE)
			{
				index = ((data.second.position.y() + start_cursor.y()) * viewport[2] +
								data.second.position.x() + start_cursor.x())*3;
				 			
				//TODO Optimize the division by three
				if (index < glarea->curSiz.width()*glarea->curSiz.height()*3 - 3 && index > 0)
				{
					if (clone_zbuffer[index / 3] < 1.0){
						color[0] = color_buffer[index]; color[1] = color_buffer[index + 1]; color[2] = color_buffer[index + 2];		
						applyColor(data.first, color, (int)(op * opac));
				
					}
				}
			}else applyColor(data.first, color, (int)(op * opac) );
			
		} else if (visited_vertices[data.first].second < op * opac) 
		{
			visited_vertices[data.first].second = op * opac;
			Color4b temp = visited_vertices[data.first].first;
			data.first->C()[0]=temp[0]; data.first->C()[1]=temp[1]; data.first->C()[2]=temp[2];
			
			if (paintbox->getCurrentType() == COLOR_CLONE) //TODO refactor in a method
			{
				index = ((data.second.position.y() + start_cursor.y()) * viewport[2] +
							data.second.position.x() + start_cursor.x())*3;
							 			
				if (index < glarea->curSiz.width()*glarea->curSiz.height()*3 - 3 && index > 0)
				{
					if (clone_zbuffer[index / 3] < 1.0){
						color[0] = color_buffer[index]; color[1] = color_buffer[index + 1]; color[2] = color_buffer[index + 2];		
						applyColor(data.first, color, (int)(op * opac));
					}
				}
			}else applyColor(data.first, color, (int)(op * opac) );	
		}
	}
}

/** 
 * fills the mesh starting from face.
 * If face is selected, it will fill only the selected area, 
 * otherwise only the non selected area 
 */
void EditPaintPlugin::fill(MeshModel & ,CFaceO * face) 
{
	QHash <CFaceO *,CFaceO *> visited;
	QHash <CVertexO *,CVertexO *> temp;
	vector <CFaceO *>temp_po;
	bool who = face->IsS();
	temp_po.push_back(face);
	visited.insert(face,face);
	int opac=paintbox->getOpacity();
	QColor newcol = (button == Qt::LeftButton) ? paintbox->getForegroundColor() : paintbox->getBackgroundColor();

	Color4b color(newcol.red(), newcol.green(), newcol.blue(), newcol.alpha());
	
//	std::cout << newcol.red() <<" "<< newcol.green() <<" "<< newcol.blue() << std::endl;
	
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

void EditPaintPlugin::gradient(MeshModel & m,GLArea * gla) {
	
	QPoint p = start_cursor - cursor;

	QHash <CVertexO *,CVertexO *> temp;

	int opac = paintbox->getOpacity();
	QColor qc1 = paintbox->getForegroundColor();
	QColor qc2 = paintbox->getBackgroundColor();

	Color4b c1(qc1.red(), qc1.green(), qc1.blue(), qc1.alpha());
	Color4b c2(qc2.red(), qc2.green(), qc2.blue(), qc2.alpha());
	
	QPointF p1(start_cursor.x(),gla->curSiz.height() - start_cursor.y());
	QPointF p0(gl_cursor);
	
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
inline void EditPaintPlugin::updateSelection(MeshModel &m, vector< pair<CVertexO *, VertexDistance> > * vertex_result)
{
	vector<CMeshO::FacePointer>::iterator fpi;
	vector<CMeshO::FacePointer> temp; //TODO maybe temp can be placed inside the class for better performance
	
	vector <CFaceO *> surround; /*< surrounding faces of a given face*/
	surround.reserve(6);
	
	mark++;
	
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

	QPointF gl_cursorf = QPointF(gl_cursor);
	QPointF gl_prev_cursorf = QPointF(prev_cursor.x(),glarea->curSiz.height() - prev_cursor.y());

	QPointF p[3],z[3]; //p stores vertex coordinates in screen space, z the corresponding depth value
	double tx,ty,tz;

	bool backface = paintbox->getPaintBackFace();
	bool invisible = paintbox->getPaintInvisible();
	bool percentual = paintbox->getSizeUnit() == 1;
	float pen_radius = paintbox->getRadius();
	int pen_size = paintbox->getSize();
	
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
				VertexDistance vd; //TODO make it a pointer

				float radius = percentual ? (pen_radius * scale_fac * viewport[3] * fov / distance[j]) : pen_size;
				
				if (isIn(gl_cursorf, gl_prev_cursorf, p[j].x(), p[j].y(), radius , & vd.distance, vd.rel_position)) 
				{
					intern = true;
					if (vertex_result == NULL) continue;
					else if (fac->V(j)->IMark() != mark)
					{
						vd.position.setX((int)p[j].x()); vd.position.setY((int)p[j].y());
						pair<CVertexO *, VertexDistance> data(fac->V(j), vd);
						vertex_result->push_back(data);
						fac->V(j)->IMark() = mark;
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
		
		if (intern && fac->IMark() != mark) 
		{
			fac->IMark() = mark;
			selection->push_back(fac);
			surround.clear();
			for (int lauf=0; lauf<3; lauf++) getSurroundingFacesVF(fac,lauf,&surround);

			for (unsigned int lauf3=0; lauf3<surround.size(); lauf3++)
			{
				if (surround[lauf3]->IMark() != mark) 
				{
						temp.push_back(surround[lauf3]);
				} 
			}
		}
	} //end of for each face loop
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


Q_EXPORT_PLUGIN(EditPaintPlugin)



/*********OpenGL Drawing Routines************/

/**
 * Draw a red vertex
 */
void drawVertex(CVertexO* vp)
{
	glPushAttrib(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glColor3f(1.0f, 0.0f, 1.0f);
	glPointSize(6.0);
	glBegin(GL_POINTS);
		glVertex((*vp).P());
	glEnd();
	glEnable(GL_DEPTH_TEST);
	glPopAttrib();
}

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


/**
 * Draw a circle projected on the surface of the mesh oriented with
 * the avarage normal. This will eventually substitute drawPercentualCircle
 */
void drawNormalPercentualCircle(GLArea * , QPoint &glcur, MeshModel &m, GLfloat* ,
		double* mvmatrix, double* prmatrix, GLint* vmatrix, float )
{
	glPushAttrib(GL_ENABLE_BIT);
	glEnable(GL_DEPTH_TEST);
	
	GLdouble x, y, z;
	GLdouble sx, sy, sz;
	sx = glcur.x();
	sy = glcur.y();
	GLfloat _z;
	
	glReadPixels((int)sx, (int)sy, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &_z); sz = _z;
	
//	std::cout << "2D: " << sx << " " << sy << " " << sz << std::endl;
		
//	if (GL_FALSE == gluUnProject(sx, sy, sz, mvmatrix, prmatrix, vmatrix, &x, &y, &z)) std::cout << "false";
	
//	std::cout << "3D: " << x << " " << y << " " << z << std::endl;
	
	gluProject(x, y, z, mvmatrix, prmatrix, vmatrix, &sx, &sy, &sz);
	
//	std::cout << "re2D: " << sx << " " << sy << " " << sz << std::endl;
	
	glPushAttrib(GL_COLOR_BUFFER_BIT);  //---Begin of Point Drawing
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glPointSize(6.0);
	glBegin(GL_POINTS);
		glColor3f(1.0f, 0.0f, 0.0f); //red
		glVertex3d(x, y, z);
	glEnd();
	glEnable(GL_DEPTH_TEST);
	glPopAttrib(); //---End of Point Drawing
		
	CFaceO * fp = NULL;

	CFaceO::NormalType normal;
	
	GLdouble tx, ty, tz;

	if (GLPickTri<CMeshO>::PickNearestFace(glcur.x(), glcur.y(), m.cm, fp)) 
	{
		double MULT = 0.1 * m.cm.bbox.Diag();
		if (fp->HasFaceNormal()){
			normal = fp->N();
			MULT /= normal.Norm();
			tx = x + MULT * normal.Ext(0); ty = y + MULT * normal.Ext(1); tz = z + MULT * normal.Ext(2);
		//	std::cout << "displaced point: " << tx << " " << ty << " " << tz << std::endl;
			glPushAttrib(GL_COLOR_BUFFER_BIT);  //---Begin of Point Drawing
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_LIGHTING);
			glEnable(GL_LINE_SMOOTH);
			glBegin(GL_LINES);
				glColor3f(1.0f, 0.0f, 0.0f); //red
				glVertex3d(x, y, z);
				glColor3f(1.0f, 1.0f, 0.0f); //red
				glVertex3d(tx, ty, tz);
			glEnd();
			glEnable(GL_DEPTH_TEST);
			glPopAttrib(); //---End of Point Drawing	
			
		}else
		{
	//		std::cout << "face doesn't have normal!" <<std::endl;
		}
	}else
	{
	//	std::cout << "couldn't pick face!" <<std::endl;
	}
	
	
	
	CMeshO::VertexPointer vp;
	
	if (getVertexAtMouse(m, vp, glcur, mvmatrix, prmatrix, vmatrix)){ 
	//	glColor3f(1.0f, 0.0f, 1.0f);
	//	drawVertex(vp);	
	//	std::cout << "vertex: " << (*vp).P()[0] << " " << (*vp).P()[1] << " " << (*vp).P()[2] << std::endl;
		
		glPushAttrib(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glPointSize(6.0);
		glBegin(GL_POINTS);
			glColor3f(1.0f, 0.0f, 1.0f); //pink
			glVertex3d((*vp).P()[0], (*vp).P()[1], (*vp).P()[2]);
		glEnd();
		glEnable(GL_DEPTH_TEST);
		glPopAttrib();

	//	calcCoord((*vp).P()[0], (*vp).P()[1], (*vp).P()[2], inv_mvmatrix, &x, &y, &z);
	
	//	std::cout << "vertex*inv_matrix3D: " << x << " " << y << " " << z << std::endl;
		
		gluProject((*vp).P()[0], (*vp).P()[1], (*vp).P()[2], mvmatrix, prmatrix, vmatrix, &sx, &sy, &sz);
	//	std::cout << "vertex2D: " << (int)sx << " " << (int)sy << " " << sz << std::endl;
	}
	
	glPopAttrib();
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

	float radius = scale * m.cm.bbox.Diag() * 0.5;
	
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
vector<QPointF> * generatePolygon(int sides, int segments)
{
	vector<QPointF> * temp = new vector<QPointF>();
		
	float step = sides / 2.0;
	
	float start_angle = M_PI / sides;
	
	for (int k = 0; k < sides; k++)
	{
		temp->push_back(QPointF(sin((M_PI * (float)k / step) + start_angle), cos((M_PI * (float)k / step) + start_angle)));
	}
	
	if (segments > 1)
	{
		vector<QPointF> * result = new vector<QPointF>();
		int sk;
		for (int k = 0; k < sides; k++  )
		{
			sk = (k + 1) % sides;
			QPointF kv = temp->at(k);
			QPointF skv = temp->at(sk);
			QPointF d = (skv - kv)/segments;
			result->push_back(kv);
			for (int j = 1; j < segments; j++)
			{
				result->push_back(kv + d * j);
			}
		}
		return result;
	}else return temp;
}

/**
 * Generates the same circle points that Gfrei's algorithm does
 */ 
vector<QPointF> * generateCircle(int segments)
{
	return generatePolygon(segments, 1);
}

vector<QPointF> * generateSquare(int segments)
{
	return generatePolygon(4, segments);	
}

