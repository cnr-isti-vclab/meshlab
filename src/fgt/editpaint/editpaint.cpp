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
#include <iostream>
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
	circle = generateCircle();
	dense_circle = generateCircle(64);
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
	parent->getCurrentRenderMode().colorMode=vcg::GLW::CMPerVert;
	parent->mm()->ioMask|=MeshModel::IOM_VERTCOLOR;
	parent->mm()->ioMask|=MeshModel::IOM_VERTQUALITY;
	
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

void EditPaintPlugin::mouseMoveEvent(QAction *, QMouseEvent* event, MeshModel &, GLArea * gla) 
{
	event_queue.enqueue(*event);
	gla->update();
}

void EditPaintPlugin::mouseReleaseEvent  (QAction *,QMouseEvent * event, MeshModel &, GLArea * gla) 
{
	event_queue.enqueue(*event);
	gla->update();
}

/**
 * Since only on a Decorate call it is possible to obtain correct values
 * from OpenGL, all operations are performed during the execution of this
 * method and not where mouse events are processed.
 */
void EditPaintPlugin::Decorate(QAction*, MeshModel &m, GLArea * gla) 
{
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
		
		prev_cursor = cursor;
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
			
			case COLOR_CLONE :
				if (event.type() == QEvent::MouseButtonPress && button == Qt::RightButton)
				{
					//store position
				}else if (event.type() == QEvent::MouseButtonPress && button == Qt::LeftButton)
				{
					//inizialize cloning
				}else if (event.type() == QEvent::MouseMove && button == Qt::LeftButton)
				{
					//clone
				}else if (event.type() == QEvent::MouseButtonRelease && button == Qt::LeftButton)
				{
					//end cloning
				}
				
			default :
				break;
		}
		
		if (paintbox->getSizeUnit() == 0) drawSimplePolyLine(gla, cursor, paintbox->getSize(), circle);
		if (paintbox->getSizeUnit() == 1) drawPercentualPolyLine(gla, gl_cursor, m, zbuffer, modelview_matrix, projection_matrix, viewport, paintbox->getRadius(), dense_circle);			
			
		vector< pair<CVertexO *, float> > * vertices;
		vertices = new vector< pair<CVertexO *, float> >(); //TODO don't need vertices for face selection!
					
		updateSelection(m, vertices);			
		
		switch (paintbox->getCurrentType())
		{
			case MESH_SELECT:
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
			case MESH_PUSH :
				if (event.type() != QEvent::MouseMove) break;
				drawNormalPercentualCircle(gla, gl_cursor, m, zbuffer, 
					modelview_matrix, projection_matrix, viewport, paintbox->getRadius());
				break;
				
			case COLOR_PAINT :
				if (event.type() == QEvent::MouseButtonPress)
				{
					paintbox->getUndoStack()->beginMacro("Paint");
					visited_vertices.clear();
				}
				else if (event.type() == QEvent::MouseMove) paint(vertices);
				else if (event.type() == QEvent::MouseButtonRelease) paintbox->getUndoStack()->endMacro();
				
				break;
				
			default :
				break;
		}

		delete vertices;
	}
}

void EditPaintPlugin::paint(vector< pair<CVertexO *, float> > * vertices)
{
	int opac = paintbox->getOpacity();
	int decrease_pos = paintbox->getHardness();
	
	QColor newcol = (button == Qt::LeftButton) ? paintbox->getForegroundColor() : paintbox->getBackgroundColor();
	Color4b color(newcol.red(), newcol.green(), newcol.blue(), newcol.alpha());
		
	for (unsigned int k = 0; k < vertices->size(); k++)
	{
		pair<CVertexO *, float> data = vertices->at(k);
		
		float op = 0.0;
		if (data.second * 100.0 > (float)decrease_pos) 
		{
			op = (data.second * 100.0 - (float)decrease_pos)/ (float) (100 - decrease_pos);
		}
		if (!visited_vertices.contains(data.first)) 
		{
			visited_vertices.insert(data.first, pair<Color4b, float>(
				Color4b(data.first->C()[0], data.first->C()[1], data.first->C()[2], data.first->C()[3]),
				opac-op*opac) );
			
			paintbox->getUndoStack()->push(new SingleColorUndo(data.first, data.first->C()));
			
			applyColor(data.first, color, (int)( opac - op * opac) );
		} else if (visited_vertices[data.first].second < opac - op * opac) 
		{
			visited_vertices[data.first].second = opac - op * opac;
			Color4b temp = visited_vertices[data.first].first;
			data.first->C()[0]=temp[0]; data.first->C()[1]=temp[1]; data.first->C()[2]=temp[2];
			applyColor(data.first, color, (int)(opac - op * opac ));	
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
	
	std::cout << newcol.red() <<" "<< newcol.green() <<" "<< newcol.blue() << std::endl;
	
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
					//r=r/(l*l);
					r=r/l_square;
					
					float px=p0.x()+r*(p1.x()-p0.x());
					float py=p0.y()+r*(p1.y()-p0.y());
				
					px=px-dx;
					py=py-dy;
	
					if (gradient_type==0) {
						if (r>=0 && r<=1 /*&& (px*px+py*py<radius*radius)*/) { 
							mergeColors(r,c1,c2,&merger);
							applyColor(vertex,merger,opac);
						} else if (r>1) {
							applyColor(vertex,c1,opac);
						} else if (r<0) {
							applyColor(vertex,c2,opac);
						}
					} else {
						if (r>=0 && r<=1 /*&& (px*px+py*py<radius*radius)*/) { 
							//mergeColor(r,c1,c2,&merger);
							applyColor(vertex,c1,(int)((opac * 0.01) * r * 100.0));
						} else if (r>1) {
							applyColor(vertex,c1,opac);
						} else if (r<0) {
							//colorize((*fi).V(lauf),c2,opac);
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
 */ 
void EditPaintPlugin::updateSelection(MeshModel &m, vector< pair<CVertexO *, float> > * vertex_result)
{
	QHash <CFaceO *,CFaceO *> visited_faces; /*< */
	QHash <CVertexO *,CVertexO *> visited_vertices;
	vector<CMeshO::FacePointer>::iterator fpi;
	vector<CMeshO::FacePointer> temp;
	
	if (selection->size() == 0) {
		CMeshO::FaceIterator fi;
        for(fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi){ 
        	if(!(*fi).IsD()) {
        		temp.push_back((&*fi));
        	}
        } 
	} else 
	{
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
		int checkable = 0;
		
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
				float dist;
				float radius = percentual ? (paintbox->getRadius() * scale_fac * viewport[3] * fov / distance[j]) : paintbox->getSize();
				
				if (isIn(gl_cursorf, gl_prev_cursorf, p[j].x(), p[j].y(), radius , &dist)) 
				{
					intern = true;
					if (vertex_result == NULL) continue;
					else if(!visited_vertices.contains(fac->V(j))) 
					{
						pair<CVertexO *, float> data(fac->V(j), dist);
						vertex_result->push_back(data);
						visited_vertices.insert(fac->V(j),fac->V(j));
					}
				}
				QPointF pos_res;
				if (vertex_result == NULL && !intern && lineHitsCircle(p[j],p[(j+1)%3], gl_cursorf, radius, &pos_res)) 
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
		
		if (intern && !visited_faces.contains(fac)) 
		{
			visited_faces.insert((fac),(fac));
			selection->push_back(fac);
			
			vector <CFaceO *> surround;
			for (int lauf=0; lauf<3; lauf++) getSurroundingFacesVF(fac,lauf,&surround);

			for (unsigned int lauf3=0; lauf3<surround.size(); lauf3++) {
				if (!visited_faces.contains(surround[lauf3])) 
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
	
	std::cout << "2D: " << sx << " " << sy << " " << sz << std::endl;
		
	if (GL_FALSE == gluUnProject(sx, sy, sz, mvmatrix, prmatrix, vmatrix, &x, &y, &z)) std::cout << "false";
	
	std::cout << "3D: " << x << " " << y << " " << z << std::endl;
	
	gluProject(x, y, z, mvmatrix, prmatrix, vmatrix, &sx, &sy, &sz);
	
	std::cout << "re2D: " << sx << " " << sy << " " << sz << std::endl;
	
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
			std::cout << "displaced point: " << tx << " " << ty << " " << tz << std::endl;
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
			
			//TODO push matrici
		/*	glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(0,gla->curSiz.width(),gla->curSiz.height(),0,-1,1); //TODO troppo
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			gluLookAt(tx, ty, tz,	x, y, z,	 0, 1, 0);
			
			//gluUnProject() di una circonferenza centrata a metà schermo e salva i punti
			GLdouble cx, cy; //centro della circonferenza
			GLdouble xs[4]; //x dei punti
			GLdouble ys[4]; //y dei punti
			GLdouble rad;
			
			cx = gla->curSiz.width()/2; cy = gla->curSiz.height()/2;
//			xs[0] = cx + rad; ys[0] = cy;
//			xs[1] = cx; ys[1] = cy + rad;
//			xs[2] = cx - rad; ys[2] = cy;
//			xs[3] = cx; ys[3] = cy - rad;
			
//			GLUquadric glq();
//			gluDisk(glq, 10, 11, 16, 1);
			
			//TODO algoritmo basturdo:
				//Proietta linee che partono
			
			//Bresenham line rastering beginning from normal apex going through circumpherence points
			//OR a quicktime approach to the problem. (line length == bb.diag)
			//TODO think about it
			
			//TODO pop matrici
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			*/
			//gluProject() dei punti della circonferenza calcolati sopra
		}else
		{
			std::cout << "face doesn't have normal!" <<std::endl;
		}
	}else
	{
		std::cout << "couldn't pick face!" <<std::endl;
	}
	
	
	
	CMeshO::VertexPointer vp;
	
	if (getVertexAtMouse(m, vp, glcur, mvmatrix, prmatrix, vmatrix)){ 
	//	glColor3f(1.0f, 0.0f, 1.0f);
	//	drawVertex(vp);	
		std::cout << "vertex: " << (*vp).P()[0] << " " << (*vp).P()[1] << " " << (*vp).P()[2] << std::endl;
		
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
		std::cout << "vertex2D: " << (int)sx << " " << (int)sy << " " << sz << std::endl;
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

/**
 * Generates the same circle points that Gfrei's algorithm does
 * With the same procedure too.
 */ 
vector<QPointF> * generateCircle(int segments)
{
	vector<QPointF> * result = new vector<QPointF>();
	
	float step = segments / 2.0;
	
	for (int k = 0; k < segments; k++) result->push_back(QPointF(sin(M_PI * (float)k / step), cos(M_PI * (float)k / step)));
	
	return result;
}

vector<QPointF> * generateSquare()
{
	return generateCircle(4);
}

