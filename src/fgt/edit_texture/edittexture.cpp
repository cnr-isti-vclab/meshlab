
#include <QtGui>
#include <QString>

#include <math.h>
#include <stdlib.h>
#include <meshlab/glarea.h>
#include "edittexture.h"
#include "renderarea.h"
#include <wrap/gl/pick.h>
#include <wrap/gl/picking.h>
#include<limits>

#include <vcg/complex/trimesh/update/topology.h>
#include <vcg/complex/trimesh/update/flag.h>

using namespace vcg;

EditTexturePlugin::EditTexturePlugin() 
{
	isDragging = false;
	widget = 0;
	qFont.setFamily("Helvetica");
	qFont.setPixelSize(14);
	actionList << new QAction(QIcon(":/images/edit_texture.png"),"Edit UV Map", this);
	QAction *editAction;
	foreach(editAction, actionList) editAction->setCheckable(true);
}

EditTexturePlugin::~EditTexturePlugin() 
{
	if (widget != 0) 
	{
		delete widget;
		widget = 0;
	}
}

QList<QAction *> EditTexturePlugin::actions() const 
{
	return actionList;
}

const QString EditTexturePlugin::Info(QAction *action) 
{
	if(action->text() != tr("Edit UV Map")) assert (0);
	return tr("Edit texture coordinates of the selected area");
}

const PluginInfo &EditTexturePlugin::Info() 
{
	static PluginInfo ai; 
	ai.Date=tr(__DATE__);
	ai.Version = tr("0.7.3");
	ai.Author = ("Riccardo Dini");
    return ai;
}

void EditTexturePlugin::mousePressEvent (QAction *, QMouseEvent * event, MeshModel &m, GLArea * gla)
{
	isDragging = true;

	if(event->modifiers() == Qt::ControlModifier) selMode = SMAdd;
    else if(event->modifiers() == Qt::ShiftModifier) selMode = SMSub;
	else selMode = SMClear;
	// Change the appearance of the cursor
	switch(selMode)	// (in StartEdit func this doesn't work...)
	{
		case SMAdd:	// CTRL + Mouse
			gla->setCursor(QCursor(QPixmap(":/images/sel_rect_plus.png"),1,1));
			break;
		case SMSub: // SHIFT + Mouse
			gla->setCursor(QCursor(QPixmap(":/images/sel_rect_minus.png"),1,1));
			break;
		case SMClear: // Mouse
			gla->setCursor(QCursor(QPixmap(":/images/sel_rect.png"),1,1));
			for (unsigned i = 0; i < FaceSel.size(); i++) FaceSel[i]->ClearS();
			FaceSel.clear();
			break;
	}
    if(event->modifiers() == Qt::ControlModifier || event->modifiers() == Qt::ShiftModifier )
	{
		CMeshO::FaceIterator fi;
		for(fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi) 
			if(!(*fi).IsD() && (*fi).IsS()) FaceSel.push_back(&*fi);        
	}

	start = event->pos();
    cur = start;
	gla->update();
	return;
}
  
void EditTexturePlugin::mouseMoveEvent (QAction *,QMouseEvent * event, MeshModel &/*m*/, GLArea * gla)
{
    prev = cur;
    cur = event->pos();
	int curT;
	static int lastRendering;
	if (isDragging)
	{
	    // The user is selecting an area: management of the update 
		lastRendering = clock();
		curT = clock();
	    if(gla->lastRenderingTime() < 50 || (curT - lastRendering) > 1000 )
		{
			lastRendering = curT;
			gla->update();
	    }
		else
		{
			gla->makeCurrent();
			glDrawBuffer(GL_FRONT);
			DrawXORRect(gla);
			glDrawBuffer(GL_BACK);
			glFlush();
	    }
	}

}
  
void EditTexturePlugin::mouseReleaseEvent(QAction *,QMouseEvent * event, MeshModel &m, GLArea * gla)
{
    prev = cur;
    cur = event->pos();
	gla->setCursor(QCursor(QPixmap(":/images/sel_rect.png"),1,1));
	isDragging = false;
	widget->Reset(); 
	InitTexture(m);
	gla->update();
}

void EditTexturePlugin::Decorate(QAction *, MeshModel &m, GLArea *gla)
{
	QPoint mid, wid;
	vector<CMeshO::FacePointer> NewFaceSel;
	vector<CMeshO::FacePointer>::iterator fpi;
	CMeshO::FaceIterator fi;
	
	if (isDragging)
	{
		DrawXORRect(gla);
		mid = (start + cur)/2;
	    mid.setY(gla->curSiz.height() - mid.y());
		wid = (start - cur);
	    if(wid.x()<0) wid.setX(-wid.x());
		if(wid.y()<0) wid.setY(-wid.y());

		for(fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi) 
			if(!(*fi).IsD()) (*fi).ClearS();

		GLPickTri<CMeshO>::PickFace(mid.x(), mid.y(), m.cm, NewFaceSel, wid.x(), wid.y());

		switch(selMode)
		{
			case SMSub:
				for(fpi = FaceSel.begin(); fpi != FaceSel.end(); ++fpi)
					(*fpi)->SetS();
				for(fpi = NewFaceSel.begin(); fpi != NewFaceSel.end(); ++fpi)
					(*fpi)->ClearS();
				break;
			case SMAdd:
				for(fpi = FaceSel.begin(); fpi != FaceSel.end(); ++fpi)
					(*fpi)->SetS();
			case SMClear:
				for(fpi = NewFaceSel.begin(); fpi != NewFaceSel.end(); ++fpi)
					(*fpi)->SetS();
				break;
		}
	}
}

void EditTexturePlugin::StartEdit(QAction * /*mode*/, MeshModel &m, GLArea *gla )
{
	FaceSel.clear();
	CMeshO::FaceIterator ff;
	for(ff = m.cm.face.begin(); ff != m.cm.face.end(); ++ff)
		if(!(*ff).IsD() && (*ff).IsS()) FaceSel.push_back(&*ff);
	// Reset the flag
	//m.cm.face.EnableFFAdjacency();
	//tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
	CMeshO::FaceIterator fi;
	for(fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi) (*fi).ClearS();

	gla->setCursor(QCursor(QPixmap(":/images/sel_rect.png"),1,1));
	
	connect(this, SIGNAL(setSelectionRendering(bool)),gla,SLOT(setSelectionRendering(bool)));
	setSelectionRendering(true);

	// Create an istance of the interface
	if (widget == 0) 
	{ 
		widget = new TextureEditor(gla->window());
		dock = new QDockWidget(gla->window());
		dock->setAllowedAreas(Qt::NoDockWidgetArea);
		dock->setWidget(widget);
		QPoint p = gla->window()->mapToGlobal(QPoint(0,0));
		dock->setGeometry(-5+p.x()+gla->window()->width()-widget->width(),p.y(),widget->width(),widget->height());
		dock->setFloating(true);
		QObject::connect(widget, SIGNAL(changeSelectMode(int)), this, SLOT(SelectMode(int)));
	}
	dock->setVisible(true);
	dock->layout()->update();	
	widget->model = &m;
	widget->area = gla;
	
	// Initialize the texture using the intere model
	for (unsigned i = 0; i < m.cm.face.size(); i++) m.cm.face[i].SetS();
	InitTexture(m);

	gla->update();
}

void EditTexturePlugin::EndEdit(QAction * , MeshModel &m , GLArea * )
{
	// Delete the widget
	if (widget != 0) 
	{ 
		delete widget; 
		delete dock; 
		widget = 0;
		dock = 0;
	 }
	// Clear the selection
	for (unsigned i = 0; i < m.cm.face.size(); i++) m.cm.face[i].ClearS();
}

void EditTexturePlugin::DrawXORRect(GLArea *gla)
{	
/*	Draw the rectangle of the selection area */
    glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, gla->curSiz.width(), gla->curSiz.height(),0,-1,1);
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
      glVertex2f(start.x(),start.y());
      glVertex2f(cur.x(),start.y());
      glVertex2f(cur.x(),cur.y());
      glVertex2f(start.x(),cur.y());
    glEnd();

    glDisable(GL_LOGIC_OP);
  	glPopAttrib();
	glPopMatrix(); // restore modelview
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void EditTexturePlugin::InitTexture(MeshModel &m)
{
	// Get the textures folder
	QString s = QString(m.fileName.c_str());
	// Add the tab
    for(unsigned i = 0; i < m.cm.textures.size(); i++)
		widget->AddRenderArea(m.cm.textures[i].c_str(), &m, i);
	vcg::Trackball tb;
}

Q_EXPORT_PLUGIN(EditTexturePlugin)
