#include <editsegment.h>

using namespace vcg;


inline bool isFront(const QPointF &a,const QPointF &b,const QPointF &c) {
	return (b.x()-a.x())*(c.y()-a.y())-(b.y()-a.y())*(c.x()-a.x())>0;
}

inline int isIn(const QPointF &p0,const QPointF &p1,float dx,float dy,float radius,float *dist) {
	if (p0!=p1) { /** this must be checked first, because of the color decrease tool */
		float x2=(p1.x()-p0.x());
		float y2=(p1.y()-p0.y());
		//double l=sqrt(x2*x2+y2*y2);
		float l_square=x2*x2+y2*y2;
		float r=(dx-p0.x())*(p1.x()-p0.x())+(dy-p0.y())*(p1.y()-p0.y());
		//r=r/(l*l);
		r=r/l_square;

		float px=p0.x()+r*(p1.x()-p0.x());
		float py=p0.y()+r*(p1.y()-p0.y());

		px=px-dx;
		py=py-dy;

		if (r>=0 && r<=1 && (px*px+py*py<radius*radius)) { *dist=sqrt(px*px+py*py)/radius; return 1; }
	}

	// there could be some problem when point is nearer p0 or p1 and viceversa
	// so i have to check both. is only needed with smooth_borders
	bool found=0;
	float x1=(dx-p1.x());
	float y1=(dy-p1.y());
	float bla1=x1*x1+y1*y1;
	if (bla1<radius*radius) { *dist=sqrt(bla1)/radius; found=1;/*return 1;*/}

	if (p0==p1) return found;

	float x0=(dx-p0.x());
	float y0=(dy-p0.y());
	float bla0=x0*x0+y0*y0;
	if (bla0<radius*radius) { 
		if (found==1) *dist=min((*dist),(float)sqrt(bla0)/radius);
		else *dist=sqrt(bla0)/radius;
		return 1;
	}

	return found;
}

inline bool lineHitsCircle(QPointF& LineStart,QPointF& LineEnd,QPointF& CircleCenter, float Radius, QPointF* const pOut = 0) {
	const float RadiusSq = Radius * Radius;
	QPointF PMinusM =LineStart - CircleCenter;

	float pm_squ=PMinusM.x()*PMinusM.x()+PMinusM.y()*PMinusM.y();
	if(pm_squ <= RadiusSq) { /// startpoint in circle
		if(pOut) *pOut = LineStart;
		return true;
	}
	QPointF LineDir=LineEnd - LineStart; /// line direction
	// u * (p - m) 
	const float UDotPMinusM = LineDir.x()*PMinusM.x()+LineDir.y()*PMinusM.y();//Vector2D_Dot(LineDir, PMinusM);
	// u*u
	const float LineDirSq = LineDir.x()*LineDir.x()+LineDir.y()*LineDir.y();
	//   (u * (p - m))^2
	// - (u*u * ((p - m)^2 - r^2)) 
	const float d =   UDotPMinusM * UDotPMinusM - LineDirSq * (pm_squ - RadiusSq);

	if(d < 0.0f) return false;
	else if(d < 0.0001f) {
		const float s = -UDotPMinusM / LineDirSq;
		if(s < 0.0f || s > 1.0f) return false;
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

inline bool pointInTriangle(const QPointF &p,const QPointF &a,const QPointF &b,const QPointF &c) {
	float fab=(p.y()-a.y())*(b.x()-a.x()) - (p.x()-a.x())*(b.y()-a.y());
	float fbc=(p.y()-c.y())*(a.x()-c.x()) - (p.x()-c.x())*(a.y()-c.y());
	float fca=(p.y()-b.y())*(c.x()-b.x()) - (p.x()-b.x())*(c.y()-b.y());
	if (fab*fbc>0 && fbc*fca>0) return true;
	return false;
}

inline bool pointInTriangle(const float p_x,const float p_y,const float a_x,const float a_y,const float b_x,const float b_y,const float c_x,const float c_y) {
	float fab=(p_y-a_y)*(b_x-a_x) - (p_x-a_x)*(b_y-a_y);
	float fbc=(p_y-c_y)*(a_x-c_x) - (p_x-c_x)*(a_y-c_y);
	float fca=(p_y-b_y)*(c_x-b_x) - (p_x-b_x)*(c_y-b_y);
	if (fab*fbc>0 && fbc*fca>0) return true;
	return false;
}

inline void getSurroundingFacesVF(CFaceO * fac,int vert_pos,vector<CFaceO *> *surround) {
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

void getInternFaces(	MeshModel & m,
										vector<CMeshO::FacePointer> *actual,
										vector<CMeshO::VertexPointer> * risult, 
										vector<CMeshO::FacePointer> * face_risult,
										GLArea * gla,
										Penn &pen,
										QPoint &current_point, 
										QPoint &previous_point, 
										GLfloat * pixels,
										double mvmatrix[16],
										double projmatrix[16],
										GLint viewport[4]) {

											QHash <CFaceO *,CFaceO *> selected;
											QHash <CVertexO *,CVertexO *> sel_vert;
											vector<CMeshO::FacePointer>::iterator fpi;
											vector<CMeshO::FacePointer> temp_po;

											if (actual->size()==0) {
												CMeshO::FaceIterator fi;
												for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi) 
													if(!(*fi).IsD()) {
														temp_po.push_back((&*fi));
													}
											} else
												for(fpi=actual->begin();fpi!=actual->end();++fpi) {
													temp_po.push_back(*fpi);
												}

												actual->clear();

												QPointF mid = QPointF(current_point.x(), gla->curSiz.height() - current_point.y());
												QPointF mid_prev = QPointF(previous_point.x(), gla->curSiz.height() - previous_point.y());

												QPointF p[3],z[3];
												double tx,ty,tz;

												bool backface=pen.backface;
												bool invisible=pen.invisible;

												for (int lauf2=0; lauf2<temp_po.size(); lauf2++) {
													CFaceO * fac=temp_po.at(lauf2);

													int intern=0;
													int checkable=0; /// to avoid problems when the pen is smaller then the polys
													for (int lauf=0; lauf<3; lauf++) {
														if (gluProject((fac)->V(lauf)->P()[0],(fac)->V(lauf)->P()[1],(fac)->V(lauf)->P()[2],
															mvmatrix,projmatrix,viewport,&tx,&ty,&tz)==GL_TRUE) checkable++;
														if (tz<0 || tz>1) checkable--;
														p[lauf].setX(tx); p[lauf].setY(ty);
														//qDebug() << tx << " "<< ty << " "<<tz << endl;
														//qDebug() << "zzz: "<<(int)(((int)ty)*old_size.x()+(int)tx)<<" t: "<<tx<<" "<<ty<<" "<<tz<<endl;
														if (tx>=0 && tx<viewport[2] && ty>=0 && ty<viewport[3]) {
															z[lauf].setX(tz);
															z[lauf].setY((float)pixels[(int)(((int)ty)*viewport[2]+(int)tx)]);
														}
														else { 
															z[lauf].setX(1); z[lauf].setY(0);
														}
													}
													if (backface || isFront(p[0],p[1],p[2])) {
														//checkable++;
														/// colud be useful to calc the medium of z in the matrix
														for (int lauf=0; lauf<3; lauf++) {
															if (invisible || (z[lauf].x()<=z[lauf].y()+0.003)){
																float dist;
																if (isIn(mid,mid_prev,p[lauf].x(),p[lauf].y(),pen.radius,&dist)==1) {
																	intern=1;
																	if (!sel_vert.contains(fac->V(lauf))) {

																		risult->push_back(fac->V(lauf));
																		sel_vert.insert(fac->V(lauf),fac->V(lauf));
																		//qDebug() << tx << " " << ty << " " << tz <<"   "<< mid<<endl;
																	}
																} 
																QPointF pos_res;
																if (intern==0 && lineHitsCircle(p[lauf],p[(lauf+1)%3],mid,pen.radius,&pos_res)) {
																	intern=1; continue;
																}
															}
														}
														if (checkable==3 && intern==0 && (pointInTriangle(mid,p[0],p[1],p[2]) || pointInTriangle(mid_prev,p[0],p[1],p[2]))) {
															intern=-1;
														}
													}
													//qDebug() << "ch: "<<checkable << "in: "<<intern <<" bla:"<<bla++<< "  "<< temp_po.size()<< endl;
													if (checkable==3 && intern!=0 && !selected.contains(fac)) {
														selected.insert((fac),(fac));
														actual->push_back(fac);
														vector <CFaceO *> surround;
														for (int lauf=0; lauf<3; lauf++) getSurroundingFacesVF(fac,lauf,&surround);

														for (int lauf3=0; lauf3<surround.size(); lauf3++) {
															if (!selected.contains(surround[lauf3])) {
																temp_po.push_back(surround[lauf3]);
															} 
														}

														if (intern!=0) face_risult->push_back(fac);
													}
												}


}



Color4b toVcgColor(QColor c) {
	return Color4b(c.red(),c.green(),c.blue(),255);
}
EditSegment::EditSegment() {
	QAction* qaction = new QAction(QIcon(":/images/editsegment.png"),"Mesh Segmentation", this);
	qaction->setCheckable(true);
	actionList << qaction;
	pixels = 0;
	pen.radius = 3;
	pen.backface = false;
	pen.invisible = false;
	pressed = false;
	dragging = false;
	first = true;
	meshCut = 0;
	meshCutDialog = 0;
	meshcut_dock = 0;
	//glarea = 0;
	selectForeground = true;
}
EditSegment::~EditSegment(){
	delete meshCut;
}
QList<QAction * > EditSegment::actions() const {
	return actionList;
}

const QString EditSegment::Info(QAction *action) {
	if( action->text() != tr("Mesh Segmentation") ) assert (0);
	return tr("Segment the mesh by selecting the foreground and background");
}

const PluginInfo &EditSegment::Info() {
	static PluginInfo ai; 
	ai.Date=tr(__DATE__);
	ai.Version = tr("0.1");
	ai.Author = ("Giorgio Gangemi");
	return ai;
} 

void EditSegment::StartEdit(QAction * mode, MeshModel & m, GLArea * parent) {
	parent->setCursor(QCursor(QPixmap(":/images/editsegment_cursor.png","PNG"),1,1));

	if (!meshCut) 
		meshCut = new MeshCutting<CMeshO>(&m.cm);

	if (!meshCutDialog) {
		meshCutDialog = new MeshCutDialog(parent->window());
		meshcut_dock = new QDockWidget(parent->window());
		meshcut_dock->setAllowedAreas(Qt::NoDockWidgetArea);
		meshcut_dock->setWidget(meshCutDialog);
		QPoint pos = parent->window()->mapToGlobal(QPoint(0,0));
		meshcut_dock->setGeometry(-5+pos.x()+parent->window()->width()-meshCutDialog->width(),
			pos.y(),meshCutDialog->width(),meshCutDialog->height());
		meshcut_dock->setFloating(true);
		QObject::connect(meshCutDialog, SIGNAL(meshCutSignal()),this, SLOT(MeshCutSlot()));
		QObject::connect(meshCutDialog, SIGNAL(selectForegroundSignal(bool)),this, SLOT(SelectForegroundSlot(bool)));

		QObject::connect(meshCutDialog, SIGNAL(resetSignal()),this,SLOT(ResetSlot()));
		QObject::connect(meshCutDialog, SIGNAL(colorizeGaussianSignal()),this, SLOT(ColorizeGaussianSlot()));
		QObject::connect(meshCutDialog, SIGNAL(colorizeMeanSignal()),this, SLOT(ColorizeMeanSlot()));

	}
	meshcut_dock->setVisible(true);
	meshcut_dock->layout()->update();

	m.updateDataMask(MeshModel::MM_VERTFACETOPO);
	m.updateDataMask(MeshModel::MM_FACETOPO);

	parent->getCurrentRenderMode().colorMode=vcg::GLW::CMPerVert;
	parent->mm()->ioMask|=MeshModel::IOM_VERTCOLOR;

	first = true;
	pressed = false;
	dragging = false;

	glarea = parent;

	parent->update();
}
void EditSegment::EndEdit  (QAction * mode, MeshModel & m, GLArea * parent) {
	qDebug() << "Mesh Segmentation End Edit" << endl;
}
void EditSegment::Decorate (QAction * ac, MeshModel & m, GLArea * gla) {
	updateMatrixes();

	QPoint mid=QPoint(current_point.x(),gla->curSiz.height() -  current_point.y());

	if (first) {
		first=false;
		if (pixels) { free(pixels); }
		pixels=(GLfloat *)malloc(sizeof(GLfloat)*gla->curSiz.width()*gla->curSiz.height());
		glReadPixels(0,0,gla->curSiz.width(),gla->curSiz.height(),GL_DEPTH_COMPONENT,GL_FLOAT,pixels);
	}

	if (dragging) {
		dragging = false;

		DrawXORCircle(m,gla,false);

		vector<CMeshO::VertexPointer> newSel;
		vector<CMeshO::FacePointer> faceSel;

		getInternFaces(m,&currentSelection,&newSel,&faceSel,gla,pen,current_point,previous_point,pixels,mvmatrix,projmatrix,viewport);
		
		vector<CMeshO::FacePointer>::iterator fpo;
		for(fpo=faceSel.begin();fpo!=faceSel.end();++fpo) {
			for (int i=0; i<3; ++i) {
				if (mouse_button_pressed==Qt::LeftButton) {
					meshCut->Mark((*fpo)->V(i), iF);
					(*fpo)->V(i)->C() = toVcgColor(meshCutDialog->getForegroundColor());
				}	else {
					meshCut->Mark((*fpo)->V(i), iB);
					(*fpo)->V(i)->C() = toVcgColor(meshCutDialog->getBackgroundColor());
				}
			}
		}
	}
}

void EditSegment::mousePressEvent    (QAction *, QMouseEvent *event, MeshModel & m, GLArea * gla){

	show_trackball = gla->isTrackBallVisible();
	gla->showTrackBall(false);
	first = true;
	dragging = true;
	mouse_button_pressed = event->button();
	pressed = true;

	current_point = event->pos();
	previous_point = current_point;

	currentSelection.clear();
}
void EditSegment::mouseMoveEvent     (QAction *, QMouseEvent *event, MeshModel & m, GLArea * gla){
	if (!dragging) previous_point = current_point;
	current_point = event->pos();
	dragging = true;
	gla->update();	
}
void EditSegment::mouseReleaseEvent  (QAction *, QMouseEvent *event, MeshModel & m, GLArea * gla){
	gla->showTrackBall(show_trackball);
	gla->update();
	previous_point = current_point;
	current_point = event->pos();
	dragging = false;
	pressed = false;
}
void EditSegment::DrawXORCircle(MeshModel &m,GLArea * gla, bool doubleDraw) {
	int PEZ=18;
	/** paint the normal circle in pixel-mode */
	//if (paintbox->paintType()==1) { 
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

	QPoint mid= QPoint(current_point.x(),/*gla->curSiz.height()-*/current_point.y());
	if(doubleDraw) {
		glBegin(GL_LINE_LOOP);
		for (int lauf=0; lauf<PEZ; lauf++) 
			glVertex2f(mid.x()+sin(M_PI*(float)lauf/9.0)*pen.radius,mid.y()+cos(M_PI*(float)lauf/9.0)*pen.radius);
		glEnd();
	}

	glBegin(GL_LINE_LOOP);
	for (int lauf=0; lauf<PEZ; lauf++) 
		glVertex2f(mid.x()+sin(M_PI*(float)lauf/9.0)*pen.radius,mid.y()+cos(M_PI*(float)lauf/9.0)*pen.radius);
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
		meshCut->MeshCut();
		meshCut->Colorize(selectForeground);
	}
	glarea->update();
}

void EditSegment::SelectForegroundSlot(bool value) {
	selectForeground = value;
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

Q_EXPORT_PLUGIN(EditSegment)
