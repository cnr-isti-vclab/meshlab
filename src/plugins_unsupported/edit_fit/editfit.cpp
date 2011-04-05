/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2008                                                \/)\/    *
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
/****************************************************************************
History
$Log: editfit.cpp,v $
****************************************************************************/
#include "editfit.h"

using namespace vcg;

EditFitPlugin::EditFitPlugin():startGesture(false),pickMode(false)
{
	toolBox=NULL;
	showGesture3D=true;
}

const QString EditFitPlugin::Info()
{
	return tr("Allow to fit some primitive.");
}

void  EditFitPlugin::StartEdit(MeshModel &, GLArea * gla){
	//Disabilito lo swap automatico del double buffering
	//e lo gestisco io direttamente con gla->swapBuffers().
	if(toolBox==NULL){
		toolBox=new FitToolBox(gla->window());
		QObject::connect(toolBox,SIGNAL(click_on_buttonCylinder())			 ,this,SLOT(change_buttonCylinder()));
		QObject::connect(toolBox,SIGNAL(click_on_buttonBox())						 ,this,SLOT(change_buttonBox()));
		QObject::connect(toolBox,SIGNAL(click_on_checkBoxGesture3d(bool)),this,SLOT(change_checkBoxGesture3d(bool)));
		toolBox->show();
	}
	//gla->setDoubleBufferSwap(false);
	gla->setCursor(QCursor(QPixmap(":/images/cur_measure.png"),15,15));
	gla->update();
	glewInit();
}
void  EditFitPlugin::EndEdit(MeshModel &, GLArea *){
	if(toolBox!=NULL){ 
		delete	toolBox; 
		toolBox=NULL;	
	}
	//Riabilito lo swap automatico del double buffering.
	//gla->setDoubleBufferSwap(true);
	gesture3D.clear();
	gesture2D.clear();
	listaPrimitive.clear();
}

void  EditFitPlugin::mousePressEvent(QMouseEvent *ev, MeshModel &, GLArea *){
	if(ev->button()==Qt::LeftButton){
		startGesture=true;
		vcg::Point2<int> tmp(ev->x(),ev->y());
		gesture2D.push_back(tmp);
		lastPoint=tmp;
		gesture3D.clear();
		listaPrimitive.clear();
	}
}
void  EditFitPlugin::mouseMoveEvent(QMouseEvent *ev, MeshModel &, GLArea * gla){
	if(startGesture){
		vcg::Point2<int> newPoint(ev->x(),ev->y());
		LineRasterDDA(lastPoint.X(),lastPoint.Y(),newPoint.X(),newPoint.Y());
		lastPoint=newPoint;
	}
	gla->update();
}
void  EditFitPlugin::mouseReleaseEvent(QMouseEvent *ev, MeshModel &, GLArea * gla){
	if(ev->button()==Qt::LeftButton){
		startGesture=false;
		pickMode=true;
	}
	//DrawInPickingMode(gla);
	//ProcessPick(gla);
}
void  EditFitPlugin::Decorate(MeshModel &, GLArea * gla){
	if(pickMode){
		DrawInPickingMode(gla);
		//ProcessPick(gla);
		pickMode=false;
		
		if(gesture3D.size()!=0){
			switch(selectedPrimitive){

				case CYLINDER:
					Cylinder *c;
					c = new Cylinder(&gesture2D,&gesture3D,&gla->mm()->cm);
					QObject::connect(c,SIGNAL(updateGlArea()),this,SLOT(slot_UpdateGlArea()));
					QObject::connect(c,SIGNAL(updateDebugStapModeSize(int)),toolBox,SLOT(slot_InitspinBoxStepMode(int)));
					QObject::connect(toolBox,SIGNAL(click_on_checkBoxCloudNearPoints(bool)),c,SLOT(change_checkBoxCloudNearPoints(bool)));
					QObject::connect(toolBox,SIGNAL(click_on_checkBoxLines(bool)),					c,SLOT(change_checkBoxLines(bool)));
					QObject::connect(toolBox,SIGNAL(click_on_checkBoxSelectionMesh(bool)),	c,SLOT(change_checkBoxSelectionMesh(bool)));
					QObject::connect(toolBox,SIGNAL(click_on_checkBoxCloudSampling(bool)),	c,SLOT(change_checkBoxCloudSampling(bool)));
					QObject::connect(toolBox,SIGNAL(click_on_checkBoxObjectSpace(bool)),		c,SLOT(change_checkBoxObjectSpace(bool)));
					QObject::connect(toolBox,SIGNAL(click_on_checkBoxStepMode(bool)),				c,SLOT(change_checkBoxStepMode(bool)));
					QObject::connect(toolBox,SIGNAL(click_on_spinBoxStepMode(int)),					c,SLOT(change_spinBoxStepMode(int)));
					listaPrimitive.push_back(c);
					break;
				
				case BOX:
					Box *s; 
					s = new Box(&gesture2D,&gesture3D,&gla->mm()->cm);
					QObject::connect(s,SIGNAL(updateGlArea()),this,SLOT(slot_UpdateGlArea()));
					QObject::connect(s,SIGNAL(updateDebugStapModeSize(int)),toolBox,SLOT(slot_InitspinBoxStepMode(int)));
					QObject::connect(toolBox,SIGNAL(click_on_checkBoxCloudNearPoints(bool)),s,SLOT(change_checkBoxCloudNearPoints(bool)));
					QObject::connect(toolBox,SIGNAL(click_on_checkBoxLines(bool)),					s,SLOT(change_checkBoxLines(bool)));
					QObject::connect(toolBox,SIGNAL(click_on_checkBoxSelectionMesh(bool)),	s,SLOT(change_checkBoxSelectionMesh(bool)));
					QObject::connect(toolBox,SIGNAL(click_on_checkBoxCloudSampling(bool)),	s,SLOT(change_checkBoxCloudSampling(bool)));
					QObject::connect(toolBox,SIGNAL(click_on_checkBoxObjectSpace(bool)),		s,SLOT(change_checkBoxObjectSpace(bool)));
					QObject::connect(toolBox,SIGNAL(click_on_checkBoxStepMode(bool)),				s,SLOT(change_checkBoxStepMode(bool)));
					QObject::connect(toolBox,SIGNAL(click_on_spinBoxStepMode(int)),					s,SLOT(change_spinBoxStepMode(int)));
					listaPrimitive.push_back(s);
					break;
			}
			std::vector<Primitive*>::iterator i;
			for(i=listaPrimitive.begin();i<listaPrimitive.end();++i){
				(*i)->start();
			}
		}		
		gesture2D.clear();
	}else{

		//DrawInPickingMode(gla);
		DrawGesture2D(gla);
		DrawGesture3D();
		
		
		//Disegno le primitive:
		std::vector<Primitive*>::iterator i;
		for(i=listaPrimitive.begin();i<listaPrimitive.end();++i){
			(*i)->Draw();
		}	
		
		//gla->swapBuffers();
		gla->update();
	}
}
/*
void  EditFitPlugin::DrawInPickingMode(GLArea * gla){
	glDrawBuffer(GL_BACK);
	//glDrawBuffer(GL_AUX1); 
	glClearColor(255,255,255,255);
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);	
	glEnable(GL_DEPTH_TEST); 
	glDisable(GL_DITHER);
	glEnable(GL_CULL_FACE);
	glDisable(GL_LIGHTING); 

	//glewInit();
	static GLfloat linear[3] = {0.0,0.0,0.001};
	static GLfloat cost[3]   = {1.0,0.0,0.0};
	glPointParameterf(GL_POINT_SIZE_MIN, -100.0);
	glPointParameterf(GL_POINT_SIZE_MAX, 100.0);

	for(unsigned int i=0;i<gla->mm()->glw.m->vert.size();i++){
		glColor4ubv((const GLubyte *)&i);
		glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, linear);
		glPointSize(1);
		glBegin(GL_POINTS);
		glVertex(gla->mm()->glw.m->vert[i].P());
		glEnd();
	}
	glEnable(GL_DITHER);
	glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, cost);
	glPointSize(1);
	glDrawBuffer(GL_BACK);
	/*Disegno ogni punto con un colore diverso partendo da RGBA==(0,0,0,0)
	fino al massimo RGBA==(255,255,255,254) perch� (255,255,255,255) � 
	lo sfondo.Quindi al massimo posso distingure 4294967295 punti*
}
*/

void  EditFitPlugin::DrawInPickingMode(GLArea * gla){
	
	/*Disegno ogni punto con un colore diverso partendo da RGBA==(0,0,0,0)
	fino al massimo RGBA==(255,255,255,254) perch� (255,255,255,255) � 
	lo sfondo.Quindi al massimo posso distingure 4294967295 punti*/

	QGLFramebufferObject framebuffer(gla->width(),gla->height(),QGLFramebufferObject::Depth,GL_TEXTURE_RECTANGLE_EXT);
		if(!framebuffer.isValid())QMessageBox::critical(gla,"Error","Invalid frame buffer");;
		if(!framebuffer.bind())		QMessageBox::critical(gla,"Error","Error binding the frame buffer");
		//framebuffer.setAutoBufferSwap(false);
		glClearColor(1,1,1,1);
		glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);	
		glEnable(GL_DEPTH_TEST); 
		glDisable(GL_DITHER);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_CULL_FACE);
		glDisable(GL_LIGHTING); 

		glewInit();
		static GLfloat linear[3] = {0.0,0.0,0.001};
		static GLfloat cost[3]   = {1.0,0.0,0.0};
		glPointParameterf(GL_POINT_SIZE_MIN, -100.0);
		glPointParameterf(GL_POINT_SIZE_MAX, 100.0);
		
		for(unsigned int i=0;i<gla->mm()->glw.m->vert.size();i++){
			glColor4ubv((const GLubyte *)&i);
			glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, linear);
			glPointSize(1);
			glBegin(GL_POINTS);
				glVertex(gla->mm()->glw.m->vert[i].P());
			glEnd();
		}
		glEnable(GL_DITHER);
		glEnable(GL_TEXTURE_2D);
		glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, cost);
		glPointSize(1);
		QImage img=framebuffer.toImage();
		//img.save("debug.bmp");
		if(img.isNull())QMessageBox::critical(gla,"Error","Error by building the image frame buffer");
		ProcessPick(img,gla);
	framebuffer.release();
}
void  EditFitPlugin::ProcessPick  (const QImage &img,GLArea * gla){

	unsigned int z;
	//Visto che in drawInPickingMode disegno pixel grossi 5, inserirei sempre lo stesso punto3D
	//perch� per piu pixel ha sempre lo stesso colore, per questo uso last.
	unsigned int last=std::numeric_limits<unsigned int>::max();		//sarebbe 4294967295;
	for(int i=0;i<(int)gesture2D.size();i++){
		QRgb k=img.pixel(gesture2D[i].X(),gesture2D[i].Y());
		int red  =qRed(k);
		int green=qGreen(k);
		int blue =qBlue(k);
		int alpha=qAlpha(k);
		z=qRgba(blue,green,red,alpha);
		/* 
		//Bedin_Debug:
				char c[100];
				std::string r;
				sprintf(c,"\n%d\n",z);
				r.append("r"+std::bitset<32>(red).to_string()+"\n");
				r.append("g"+std::bitset<32>(green).to_string()+"\n");
				r.append("b"+std::bitset<32>(blue).to_string()+"\n");
				r.append("a"+std::bitset<32>(alpha).to_string()+"\n");
				r.append(c);
				r.append("X"+std::bitset<32>(z).to_string()+"\n");
				QMessageBox::information(0,"Messaggio:",r.c_str());
		//End_Debug
		*/
		if(z!=std::numeric_limits<unsigned int>::max() && z!=last){
			gesture3D.push_back(vcg::Point3f(gla->mm()->glw.m->vert[z].P()));
			last=z;
		}	
	}
}


/*
void  EditFitPlugin::ProcessPick  (GLArea * gla){
	unsigned int k=0;
	//Visto che in drawInPickingMode disegno pixel grossi 5, inserirei sempre lo stesso punto3D
	//perch� per piu pixel ha sempre lo stesso colore, per questo uso last.
	unsigned int last=std::numeric_limits<unsigned int>::max();		//sarebbe 4294967295;
	glReadBuffer(GL_BACK);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	
	/*
	//Debug:
	//unsigned char *data=(unsigned char *)malloc(4*gla->width()*gla->height()*sizeof(unsigned char));
	//glReadPixels(0,0,gla->width(),gla->height(),GL_RGBA,GL_UNSIGNED_BYTE,data);
	//QImage pippo(data,gla->width(),gla->height(),QImage::Format_ARGB32);
	//pippo.save("debug.bmp");
	//fine Debug.
	

	for(int i=0;i<(int)gesture2D.size();i++){
		glReadPixels(gesture2D[i].X(),(gla->height()-1)-gesture2D[i].Y(),1,1,GL_RGBA,GL_UNSIGNED_BYTE,&k);
		if(k!=std::numeric_limits<unsigned int>::max() && k!=last){
			gesture3D.push_back(vcg::Point3f(gla->mm()->glw.m->vert[k].P()));
			last=k;
		}
	}
}
*/
void  EditFitPlugin::DrawGesture2D  (GLArea * gla){
	//2d mode
	glPushAttrib(GL_DEPTH_BUFFER_BIT|GL_ENABLE_BIT|GL_LINE_BIT|GL_POINT_BIT|GL_CURRENT_BIT|GL_LIGHTING_BIT|GL_COLOR_BUFFER_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_BLEND);
	glDepthMask(false);
	glPointSize(2.0);
	glColor3f(0,0,1.0);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0,gla->width(),gla->height(),0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	//////////////////////////////////////////
	std::vector<vcg::Point2i>::iterator i;
	for(i=gesture2D.begin();i<gesture2D.end();++i){
		glBegin(GL_POINTS);
		glVertex2i(i->X(),i->Y());
		glEnd();
	}
	//Out 2d mode
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glPopAttrib();
}


void  EditFitPlugin::DrawGesture3D(){
	if(showGesture3D){
		glPushAttrib(GL_DEPTH_BUFFER_BIT|GL_ENABLE_BIT|GL_LINE_BIT|GL_POINT_BIT|GL_CURRENT_BIT|GL_LIGHTING_BIT|GL_COLOR_BUFFER_BIT);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_BLEND);

		std::vector<vcg::Point3f>::iterator i;
		for(i=gesture3D.begin();i<gesture3D.end();++i){
			glColor3f(0,1.0,0);
			glPointSize(3);
			glBegin(GL_POINTS);
			glVertex(*i);
			glEnd();
		}	
		glPointSize(1);
		glPopAttrib();
	}
}
void  EditFitPlugin::LineRasterDDA			 (float x1,float y1,float x2,float y2){
	float x=x1,y=y1,m;
	float dx=x2-x1;
	float dy=y2-y1;
	if(abs(dx)>=abs(dy)){
		m=abs(dx);
	}else{
		m=abs(dy);
	}
	for(int i=1;i<=m;i++){
		x=x+dx/m;
		y=y+dy/m;
		gesture2D.push_back(vcg::Point2<int>((int)x,(int)y));
	}
}



EditFitPlugin::~EditFitPlugin(){
	actionList.clear();
	listaPrimitive.clear();
}
//Slots:
void EditFitPlugin::change_buttonCylinder(){
	selectedPrimitive=CYLINDER;
}
void EditFitPlugin::change_buttonBox(){
	//QMessageBox::information(0,"Box","Box");
	selectedPrimitive=BOX;
}
void EditFitPlugin::change_checkBoxGesture3d(bool b){
	showGesture3D=b;
}
void EditFitPlugin::slot_UpdateGlArea(){
	//gla->update();
}
