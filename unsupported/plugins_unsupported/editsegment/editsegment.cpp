/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

#include <editsegment.h>

using namespace std;
using namespace vcg;

inline bool isFront(const QPointF &a, const QPointF &b, const QPointF &c) {
	return (b.x()-a.x())*(c.y()-a.y())-(b.y()-a.y())*(c.x()-a.x())>0;
}

inline bool isIn(const QPointF &pointer, const QPointF &pointer_prev, float X,
								 float Y, float radius, float *dist) {
									 if (pointer != pointer_prev) {
										 float mid_x = (pointer_prev.x() - pointer.x());
										 float mid_y = (pointer_prev.y() - pointer.y());

										 float l_square = mid_x * mid_x + mid_y * mid_y;
										 float r = (X - pointer.x()) * (pointer_prev.x() - pointer.x()) + 
											 (Y - pointer.y()) * (pointer_prev.y() - pointer.y());

										 r /= l_square;

										 float P_x = pointer.x() + r * (pointer_prev.x() - pointer.x()) - X;
										 float P_y = pointer.y() + r * (pointer_prev.y() - pointer.y()) - Y;

										 if (r >= 0 && r <= 1 && (P_x * P_x + P_y * P_y < radius * radius)) {
											 *dist=sqrt(P_x*P_x + P_y*P_y)/radius;
											 return true;
										 }
									 }

									 // there could be some problem when point is nearer p0 or p1 and viceversa
									 // so i have to check both. is only needed with smooth_borders

									 float x1 = (X - pointer_prev.x());
									 float y1 = (Y - pointer_prev.y());
									 float tmp = x1*x1 + y1*y1;
									 if (tmp < radius*radius) {
										 *dist = sqrt(tmp)/radius;
										 return true;
									 }

									 if (pointer == pointer_prev)
										 return false;
									 else {
										 float x0 = (X - pointer.x());
										 float y0 = (Y - pointer.y());
										 tmp = x0*x0 + y0*y0;
										 if (tmp < radius*radius) {
											 *dist = sqrt(tmp)/radius;
											 return true;
										 } else
											 return false;
									 }
}

inline bool lineHitsCircle(QPointF& LineStart, QPointF& LineEnd,
													 QPointF& CircleCenter, float Radius, QPointF* const pOut = 0) {
														 const float RadiusSq = Radius * Radius;
														 QPointF PMinusM =LineStart - CircleCenter;

														 float pm_squ=PMinusM.x()*PMinusM.x()+PMinusM.y()*PMinusM.y();
														 if (pm_squ <= RadiusSq) { /// startpoint in circle
															 if (pOut)
																 *pOut = LineStart;
															 return true;
														 }
														 QPointF LineDir=LineEnd - LineStart; /// line direction
														 // u * (p - m) 
														 const float UDotPMinusM = LineDir.x()*PMinusM.x()+LineDir.y()*PMinusM.y();//Vector2D_Dot(LineDir, PMinusM);
														 // u*u
														 const float LineDirSq = LineDir.x()*LineDir.x()+LineDir.y()*LineDir.y();
														 //   (u * (p - m))^2
														 // - (u*u * ((p - m)^2 - r^2)) 
														 const float d = UDotPMinusM * UDotPMinusM - LineDirSq * (pm_squ - RadiusSq);

														 if (d < 0.0f)
															 return false;
														 else if (d < 0.0001f) {
															 const float s = -UDotPMinusM / LineDirSq;
															 if (s< 0.0f || s> 1.0f)return false;
															 else {
																 if(pOut) *pOut = LineStart + s * LineDir;
																 return true;
															 }
														 }
														 else {
															 const float s = (-UDotPMinusM - sqrtf(d)) / LineDirSq;
															 if(s < 0.0f || s > 1.0f) return false;
															 else {
																 if(pOut) *pOut = LineStart + s * LineDir;
																 return true;
															 }
														 }
}

inline bool pointInTriangle(const QPointF &p, const QPointF &a,
														const QPointF &b, const QPointF &c) {
															float fab=(p.y()-a.y())*(b.x()-a.x()) - (p.x()-a.x())*(b.y()-a.y());
															float fbc=(p.y()-c.y())*(a.x()-c.x()) - (p.x()-c.x())*(a.y()-c.y());
															float fca=(p.y()-b.y())*(c.x()-b.x()) - (p.x()-b.x())*(c.y()-b.y());
															if (fab*fbc>0 && fbc*fca>0)
																return true;
															return false;
}

inline bool pointInTriangle(const float p_x, const float p_y, const float a_x,
														const float a_y, const float b_x, const float b_y, const float c_x,
														const float c_y) {
															float fab=(p_y-a_y)*(b_x-a_x) - (p_x-a_x)*(b_y-a_y);
															float fbc=(p_y-c_y)*(a_x-c_x) - (p_x-c_x)*(a_y-c_y);
															float fca=(p_y-b_y)*(c_x-b_x) - (p_x-b_x)*(c_y-b_y);
															if (fab*fbc>0 && fbc*fca>0)
																return true;
															return false;
}

inline void getSurroundingFacesVF(CFaceO * fac, int vert_pos,
																	vector<CFaceO *> *surround) {
																		CVertexO * vert=fac->V(vert_pos);
																		int pos=vert->VFi();
																		CFaceO * first_fac=vert->VFp();
																		CFaceO * curr_f=first_fac;
																		do {
																			CFaceO * temp=curr_f->VFp(pos);
																			if (curr_f!=0 && !curr_f->IsD()) {
																				surround->push_back(curr_f);
																				pos=curr_f->VFi(pos);
																			}
																			curr_f=temp;
																		} while (curr_f!=first_fac && curr_f!=0);
}

void getInternFaces(MeshModel & m, vector<CMeshO::FacePointer> *actual,	vector<CMeshO::VertexPointer> * risult,
										vector<CMeshO::FacePointer> * face_risult, GLArea * gla, Penn &pen,	QPoint &current_point, QPoint &previous_point, GLfloat * pixels,
										double mvmatrix[16], double projmatrix[16], GLint viewport[4]) {

											QHash <CFaceO *,CFaceO *> selected;
											QHash <CVertexO *,CVertexO *> sel_vert;
											vector<CMeshO::FacePointer>::iterator fpi;
											vector<CMeshO::FacePointer> temp_po;

											if (actual->size()==0) {
												CMeshO::FaceIterator fi;
												for (fi=m.cm.face.begin(); fi!=m.cm.face.end(); ++fi)
													if (!(*fi).IsD()) {
														temp_po.push_back((&*fi));
													}
											} else
												for (fpi=actual->begin(); fpi!=actual->end(); ++fpi) {
													temp_po.push_back(*fpi);
												}

												actual->clear();

												QPointF mid = QPointF(current_point.x(), gla->curSiz.height()
													- current_point.y());
												QPointF mid_prev = QPointF(previous_point.x(), gla->curSiz.height()
													- previous_point.y());

												QPointF p[3], z[3];
												double tx, ty, tz;

												bool backface=pen.backface;
												bool invisible=pen.invisible;

												for (int j=0; j<temp_po.size(); j++) {
													CFaceO * fac=temp_po.at(j);

													int intern=0;
													int checkable=0; /// to avoid problems when the pen is smaller then the polys
													for (int i=0; i<3; i++) {
														if (gluProject((fac)->V(i)->P()[0], (fac)->V(i)->P()[1], (fac)->V(i)->P()[2], mvmatrix, projmatrix, viewport, &tx, &ty, &tz)
															==GL_TRUE)
															checkable++;
														if (tz<0 || tz>1)checkable --;
														p[i].setX(tx); p[i].setY(ty);

														if (tx>=0 && tx<viewport[2] && ty>=0 && ty<viewport[3]) {
															z[i].setX(tz);
															z[i].setY((float)pixels[(int)(((int)ty)*viewport[2]+(int)tx)]);
														}
														else {
															z[i].setX(1); z[i].setY(0);
														}
													}
													if (backface || isFront(p[0],p[1],p[2])) {
														//checkable++;
														/// colud be useful to calc the medium of z in the matrix
														for (int i=0; i<3; i++) {
															if (invisible || (z[i].x()<=z[i].y()+0.003)) {
																float dist;
																if (isIn(mid,mid_prev,p[i].x(),p[i].y(),pen.radius,&dist)==1) {
																	intern=1;
																	if (!sel_vert.contains(fac->V(i))) {

																		risult->push_back(fac->V(i));
																		sel_vert.insert(fac->V(i),fac->V(i));

																	}
																}
																QPointF pos_res;
																if (intern==0 && lineHitsCircle(p[i],p[(i+1)%3],mid,pen.radius,&pos_res)) {
																	intern=1; continue;
																}
															}
														}
														if (checkable==3 && intern==0 && (pointInTriangle(mid,p[0],p[1],p[2]) || pointInTriangle(mid_prev,p[0],p[1],p[2]))) {
															intern=-1;
														}
													}

													if (checkable==3 && intern!=0 && !selected.contains(fac)) {
														selected.insert((fac),(fac));
														actual->push_back(fac);
														vector <CFaceO *> surround;
														for (int i=0; i<3; i++) getSurroundingFacesVF(fac,i,&surround);

														for (int i=0; i<surround.size(); i++) {
															if (!selected.contains(surround[i])) {
																temp_po.push_back(surround[i]);
															}
														}

														if (intern!=0) face_risult->push_back(fac);
													}
												}

}

Color4b toVcgColor(QColor c) {
	return Color4b(c.red(), c.green(), c.blue(), 255);
}
EditSegment::EditSegment() {
	pixels = 0;
	pen.radius = 5;
	pen.backface = false;
	pen.invisible = false;
	pressed = false;
	dragging = false;
	first = true;
	meshCut = 0;
	meshCutDialog = 0;
	meshcut_dock = 0;

	normalWeight = 5;
	curvatureWeight = 5;

	selectForeground = true;
	doRefine = true;
}
EditSegment::~EditSegment() {
	delete meshCut;
}

const QString EditSegment::Info()
{
	return tr("Segment the mesh by selecting the foreground and background");
}

bool EditSegment::StartEdit(MeshModel & m, GLArea * parent) {
	parent->setCursor(QCursor(
		QPixmap(":/images/editsegment_cursor.png", "PNG"), 1, 1));

	if (!glarea_map.contains(parent)) {
		glarea_map.insert(parent, new MeshCutting<CMeshO>(&m.cm));
	}
	glarea = parent;
	meshCut = glarea_map[glarea];

	if (!meshCutDialog) {
		meshCutDialog = new MeshCutDialog(parent->window());
		meshcut_dock = new QDockWidget(parent->window());
		meshcut_dock->setAllowedAreas(Qt::NoDockWidgetArea);
		meshcut_dock->setWidget(meshCutDialog);
		QPoint pos = parent->window()->mapToGlobal(QPoint(0, 0));
		meshcut_dock->setGeometry(-5+pos.x()+parent->window()->width()-meshCutDialog->width(), pos.y(),
			meshCutDialog->width(), meshCutDialog->height());
		meshcut_dock->setFloating(true);
		QObject::connect(meshCutDialog, SIGNAL(meshCutSignal()), this,
			SLOT(MeshCutSlot()));
			
		QObject::connect(meshCutDialog, SIGNAL(updateCurvatureSignal()), this,
			SLOT(UpdateCurvatureSlot()));
		QObject::connect(meshCutDialog, SIGNAL(selectForegroundSignal(bool)),this, SLOT(SelectForegroundSlot(bool)));

		QObject::connect(meshCutDialog, SIGNAL(doRefineSignal(bool)),this, SLOT(doRefineSlot(bool)));

		QObject::connect(meshCutDialog, SIGNAL(resetSignal()), this,
			SLOT(ResetSlot()));
		QObject::connect(meshCutDialog, SIGNAL(colorizeGaussianSignal()), this,
			SLOT(ColorizeGaussianSlot()));
		QObject::connect(meshCutDialog, SIGNAL(colorizeMeanSignal()), this,
			SLOT(ColorizeMeanSlot()));
		QObject::connect(meshCutDialog, SIGNAL(normalWeightSignal(int)),this, SLOT(changeNormalWeight(int)));
		QObject::connect(meshCutDialog, SIGNAL(curvatureWeightSignal(int)),this, SLOT(changeCurvatureWeight(int)));
		QObject::connect(meshCutDialog, SIGNAL(penRadiusSignal(int)),this, SLOT(changePenRadius(int)));
	}
	meshcut_dock->setVisible(true);
	meshcut_dock->layout()->update();

	m.updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_VERTFACETOPO | MeshModel::MM_VERTMARK );


	parent->getCurrentRenderMode().colorMode=vcg::GLW::CMPerVert;
	parent->mm()->updateDataMask(MeshModel::MM_VERTCOLOR);
	//ioMask|=MeshModel::IOM_VERTCOLOR;

	first = true;
	pressed = false;
	dragging = false;

	parent->update();
	return true;
}

void EditSegment::EndEdit(MeshModel & , GLArea * ) {
	//qDebug() << "Mesh Segmentation End Edit" << endl;
	delete meshCutDialog;
	meshCutDialog = 0;
	delete meshcut_dock;
	meshcut_dock = 0;
}
void EditSegment::Decorate(MeshModel & m, GLArea * gla) {
	if (!glarea_map.contains(gla)) {
		glarea_map.insert(gla, new MeshCutting<CMeshO>(&m.cm));
	}
	glarea = gla;
	meshCut = glarea_map[glarea];

	updateMatrixes();

	QPoint mid=QPoint(current_point.x(), gla->curSiz.height()
		- current_point.y());

	if (first) {
		first=false;
		if (pixels) {
			free(pixels);
		}
		pixels=(GLfloat *)malloc(sizeof(GLfloat)*gla->curSiz.width()
			*gla->curSiz.height());
		glReadPixels(0, 0, gla->curSiz.width(), gla->curSiz.height(),
			GL_DEPTH_COMPONENT, GL_FLOAT, pixels);
	}

	if (dragging) {
		dragging = false;

		DrawXORCircle(m, glarea, false);

		vector<CMeshO::VertexPointer> newSel;
		vector<CMeshO::FacePointer> faceSel;

		getInternFaces(m, &currentSelection, &newSel, &faceSel, gla, pen,
			current_point, previous_point, pixels, mvmatrix, projmatrix,
			viewport);

		vector<CMeshO::VertexPointer>::iterator vi;
		for (vi=newSel.begin(); vi!=newSel.end(); ++vi) {

			if (mouse_button_pressed==Qt::LeftButton) {
				meshCut->Mark(*vi, iF);
				(*vi)->C()
					= toVcgColor(meshCutDialog->getForegroundColor());
			} else if (mouse_button_pressed==Qt::RightButton ){
				meshCut->Mark(*vi, iB);
				(*vi)->C()
					= toVcgColor(meshCutDialog->getBackgroundColor());
			} else if (mouse_button_pressed==Qt::MidButton) {
				meshCut->Mark(*vi, U);
				(*vi)->C()
					= toVcgColor(Qt::white);
			}

		}
	}
}

void EditSegment::mousePressEvent(QMouseEvent *event, MeshModel & m,	GLArea * gla) {
	//if (event->button() == Qt::MidButton) {
	//gla->trackball.MouseDown(event->x(),gla->height()-event->y(), QT2VCG(event->button(), event->modifiers() ) );
	//} else {

	if (!glarea_map.contains(gla)) {
		glarea_map.insert(gla, new MeshCutting<CMeshO>(&m.cm));
	}
	glarea = gla;
	meshCut = glarea_map[glarea];

	show_trackball = gla->isTrackBallVisible();
	gla->showTrackBall(false);
	first = true;
	dragging = true;
	mouse_button_pressed = event->button();
	pressed = true;

	current_point = event->pos();
	previous_point = current_point;

	currentSelection.clear();
	//}
}
void EditSegment::mouseReleaseEvent(QMouseEvent *event, MeshModel & m, GLArea * gla) {
	//if (event->button() == Qt::MidButton) {
	//gla->trackball.MouseUp(event->x(),gla->height()-event->y(), QT2VCG(event->button(), event->modifiers() ) );
	//} else {

	if (!glarea_map.contains(gla)) {
		glarea_map.insert(gla, new MeshCutting<CMeshO>(&m.cm));
	}
	glarea = gla;
	meshCut = glarea_map[glarea];

	gla->showTrackBall(show_trackball);
	gla->update();
	previous_point = current_point;
	current_point = event->pos();
	dragging = false;
	pressed = false;
	//}
}
void EditSegment::mouseMoveEvent(QMouseEvent *event, MeshModel & m, GLArea * gla) {
	//if (event->button() == Qt::MidButton) {
	//gla->trackball.MouseMove(event->x(),gla->height()-event->y());
	//gla->setCursorTrack(gla->trackball.current_mode);
	//} else {

	if (!glarea_map.contains(gla)) {
		glarea_map.insert(gla, new MeshCutting<CMeshO>(&m.cm));
	}
	glarea = gla;
	meshCut = glarea_map[glarea];

	if (!dragging)
		previous_point = current_point;
	current_point = event->pos();
	dragging = true;
	//}
	gla->update();
}
void EditSegment::DrawXORCircle(MeshModel &m, GLArea * gla, bool doubleDraw) {
	int PEZ=18;

	if (!glarea_map.contains(gla)) {
		glarea_map.insert(gla, new MeshCutting<CMeshO>(&m.cm));
	}
	glarea = gla;
	meshCut = glarea_map[glarea];

	/** paint the normal circle in pixel-mode */
	//if (paintbox->paintType()==1) { 
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, gla->curSiz.width(), gla->curSiz.height(), 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_LOGIC_OP);
	glLogicOp(GL_XOR);
	glColor3f(1, 1, 1);

	QPoint mid= QPoint(current_point.x(),/*gla->curSiz.height()-*/
		current_point.y());
	if (doubleDraw) {
		glBegin(GL_LINE_LOOP);
		for (int lauf=0; lauf<PEZ; lauf++)
			glVertex2f(mid.x()+sin(M_PI*(float)lauf/9.0)*pen.radius, mid.y()
			+cos(M_PI*(float)lauf/9.0)*pen.radius);
		glEnd();
	}

	glBegin(GL_LINE_LOOP);
	for (int lauf=0; lauf<PEZ; lauf++)
		glVertex2f(mid.x()+sin(M_PI*(float)lauf/9.0)*pen.radius, mid.y()
		+cos(M_PI*(float)lauf/9.0)*pen.radius);
	glEnd();

	glDisable(GL_LOGIC_OP);
	// Closing 2D
	glPopAttrib();
	glPopMatrix(); // restore modelview
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void EditSegment::MeshCutSlot() {
	if (meshCut) {
		meshCut->MeshCut(normalWeight,curvatureWeight);
		meshCut->Colorize(selectForeground, doRefine);
	}
	glarea->update();
}

void EditSegment::UpdateCurvatureSlot() {
	if (meshCut) {
		meshCut->UpdateCurvature();
	}
}

void EditSegment::SelectForegroundSlot(bool value) {
	selectForeground = value;
}

void EditSegment::doRefineSlot(bool value) {
	doRefine = value;
}

void EditSegment::changeCurvatureWeight(int value) {
	curvatureWeight = value;
}

void EditSegment::changeNormalWeight(int value) {
	normalWeight = value;
}

void EditSegment::changePenRadius(int value) {
	pen.radius = value;
}

void EditSegment::ColorizeGaussianSlot() {
	if (meshCut) {
		meshCut->ColorizeCurvature(true);
		glarea->update();
	}
}

void EditSegment::ColorizeMeanSlot() {
	if (meshCut) {
		meshCut->ColorizeCurvature(false);
		glarea->update();
	}
}

void EditSegment::ResetSlot() {
	if (meshCut) {
		meshCut->Reset();
		glarea->update();
	}
}
