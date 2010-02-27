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
#include <limits>
#include <meshlab/glarea.h>
#include "edit_select.h"
#include <wrap/gl/pick.h>
#include <vcg/complex/trimesh/update/selection.h>


using namespace std;
using namespace vcg;

EditSelectPlugin::EditSelectPlugin(int ConnectedMode) :selectionMode(ConnectedMode) {
  isDragging=false;      
}

QString EditSelectPlugin::Info()
{
	return tr("Interactive selection of faces inside a dragged rectangle in screen space");
}
void EditSelectPlugin::keyReleaseEvent  (QKeyEvent *, MeshModel &/*m*/, GLArea *gla)
{
	gla->setCursor(QCursor(QPixmap(":/images/sel_rect.png"),1,1));	
}

void EditSelectPlugin::keyPressEvent    (QKeyEvent */*event*/, MeshModel &/*m*/, GLArea *gla)
{
	
	if(QApplication::keyboardModifiers() ==  Qt::AltModifier)  gla->setCursor(QCursor(QPixmap(":/images/sel_rect_eye.png"),1,1));	
  if(QApplication::keyboardModifiers() == (Qt::AltModifier+Qt::ControlModifier)) 	gla->setCursor(QCursor(QPixmap(":/images/sel_rect_plus_eye.png"),1,1));
  if(QApplication::keyboardModifiers() == (Qt::AltModifier+Qt::ShiftModifier)) 	gla->setCursor(QCursor(QPixmap(":/images/sel_rect_minus_eye.png"),1,1));
  if(QApplication::keyboardModifiers() ==  Qt::ControlModifier) 	gla->setCursor(QCursor(QPixmap(":/images/sel_rect_plus.png"),1,1));
	if(QApplication::keyboardModifiers() ==  Qt::ShiftModifier) 	gla->setCursor(QCursor(QPixmap(":/images/sel_rect_minus.png"),1,1));	
}

void EditSelectPlugin::mousePressEvent(QMouseEvent * event, MeshModel &m, GLArea *)
{
    LastSelVert.clear();
    LastSelFace.clear();

    if((event->modifiers() & Qt::ControlModifier) || 
       (event->modifiers() & Qt::ShiftModifier)  )
      {
        CMeshO::FaceIterator fi;
        for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi) 
            if(!(*fi).IsD() && (*fi).IsS()) 
                    LastSelFace.push_back(&*fi);

        CMeshO::VertexIterator vi;
        for(vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi)
            if(!(*vi).IsD() && (*vi).IsS())
                    LastSelVert.push_back(&*vi);
      }
      
    composingSelMode=SMClear;
    if(event->modifiers() & Qt::ControlModifier) composingSelMode=SMAdd;
    if(event->modifiers() & Qt::ShiftModifier) composingSelMode=SMSub;
		
    if(event->modifiers() & Qt::AltModifier)		selectFrontFlag=true;
																				else	selectFrontFlag=false;

    start=event->pos();
    cur=start;
    return;
  }
  
  void EditSelectPlugin::mouseMoveEvent(QMouseEvent * event, MeshModel &/*m*/, GLArea * gla)
  {
    prev=cur;
    cur=event->pos();
    isDragging = true;

    // to avoid too frequent rendering 
    if(gla->lastRenderingTime() < 200 )
    {
      gla->update();
    }
    else{
      gla->makeCurrent();
      glDrawBuffer(GL_FRONT);
      DrawXORRect(gla,true);
      glDrawBuffer(GL_BACK);
      glFlush();
    }
  }
  
  void EditSelectPlugin::mouseReleaseEvent(QMouseEvent * event, MeshModel &/*m*/, GLArea * gla)
  {
    gla->update();
    prev=cur;
    cur=event->pos();
  }

  void EditSelectPlugin::DrawXORRect(GLArea * gla, bool doubleDraw)
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
    if(doubleDraw)
    {
      glBegin(GL_LINE_LOOP);
        glVertex2f(start.x(),start.y());
        glVertex2f(prev.x(),start.y());
        glVertex2f(prev.x(),prev.y());
        glVertex2f(start.x(),prev.y());
      glEnd();
    }
    glBegin(GL_LINE_LOOP);
      glVertex2f(start.x(),start.y());
      glVertex2f(cur.x(),start.y());
      glVertex2f(cur.x(),cur.y());
      glVertex2f(start.x(),cur.y());
    glEnd();
    glDisable(GL_LOGIC_OP);
    
    // Closing 2D
  	glPopAttrib();
	  glPopMatrix(); // restore modelview
	  glMatrixMode(GL_PROJECTION);
	  glPopMatrix();
	  glMatrixMode(GL_MODELVIEW);

  }

  void EditSelectPlugin::Decorate(MeshModel &m, GLArea * gla)
  {
    if(isDragging)
    {
    DrawXORRect(gla,false);
    vector<CMeshO::FacePointer>::iterator fpi;
    // Starting Sel 
    vector<CMeshO::FacePointer> NewSelFace;
    vector<CMeshO::VertexPointer> NewSelVert;
    QPoint mid=(start+cur)/2;
    mid.setY(gla->curSiz.height()-  mid.y());
    QPoint wid=(start-cur);
    if(wid.x()<0)  wid.setX(-wid.x());
    if(wid.y()<0)  wid.setY(-wid.y());

		glPushMatrix();
		glMultMatrix(m.cm.Tr);
    if(selectionMode == SELECT_VERT_MODE)
    {
        vector<CMeshO::VertexPointer>::iterator vpi;

      GLPickTri<CMeshO>::PickVert(mid.x(), mid.y(), m.cm, NewSelVert, wid.x(), wid.y());
      glPopMatrix();
      tri::UpdateSelection<CMeshO>::ClearVertex(m.cm);

      switch(composingSelMode)
      {
        case SMSub :  // Subtract mode : The faces in the rect must be de-selected
            for(vpi=LastSelVert.begin();vpi!=NewSelVert.end();++vpi)
                (*vpi)->SetS();
            for(vpi=NewSelVert.begin();vpi!=NewSelVert.end();++vpi)
                (*vpi)->ClearS();
              break;
        case SMAdd :  // Subtract mode : The faces in the rect must be de-selected
              for(vpi=LastSelVert.begin();vpi!=LastSelVert.end();++vpi)
                  (*vpi)->SetS();
        case SMClear :  // Subtract mode : The faces in the rect must be de-selected
              for(vpi=NewSelVert.begin();vpi!=NewSelVert.end();++vpi)
                  (*vpi)->SetS();
                break;
      }
    }
    else
    {
    if(selectFrontFlag)	GLPickTri<CMeshO>::PickFaceVisible(mid.x(), mid.y(), m.cm, NewSelFace, wid.x(), wid.y());
                   else GLPickTri<CMeshO>::PickFace(mid.x(), mid.y(), m.cm, NewSelFace, wid.x(), wid.y());

    qDebug("Pickface: rect %i %i - %i %i",mid.x(),mid.y(),wid.x(),wid.y());
    qDebug("Pickface: Got  %i on %i",int(NewSelFace.size()),int(m.cm.face.size()));
		glPopMatrix();
    tri::UpdateSelection<CMeshO>::ClearFace(m.cm);
    switch(composingSelMode)
    { 
      case SMSub :  // Subtract mode : The faces in the rect must be de-selected
        if(selectionMode == SELECT_CONN_MODE)
					{
            for(fpi=NewSelFace.begin();fpi!=NewSelFace.end();++fpi)
								(*fpi)->SetS();
						tri::UpdateSelection<CMeshO>::FaceConnectedFF(m.cm);
						
						CMeshO::FaceIterator fi;
            NewSelFace.clear();
						for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi)
                if(!(*fi).IsD() && (*fi).IsS()) NewSelFace.push_back(&*fi);
					}
				// Normal case: simply deselect what has been selected. 
        for(fpi=LastSelFace.begin();fpi!=LastSelFace.end();++fpi)
					(*fpi)->SetS();
        for(fpi=NewSelFace.begin();fpi!=NewSelFace.end();++fpi)
					(*fpi)->ClearS(); 
      break;
      case SMAdd :
        for(fpi=LastSelFace.begin();fpi!=LastSelFace.end();++fpi)
          (*fpi)->SetS(); 
      case SMClear :
        for(fpi=NewSelFace.begin();fpi!=NewSelFace.end();++fpi)
          (*fpi)->SetS();
          if(selectionMode == SELECT_CONN_MODE) tri::UpdateSelection<CMeshO>::FaceConnectedFF(m.cm);
        break;
    }
     isDragging=false;
    }   
}
  }

bool EditSelectPlugin::StartEdit(MeshModel &m, GLArea *gla )
{  
 gla->setCursor(QCursor(QPixmap(":/images/sel_rect.png"),1,1));	

 connect(this, SIGNAL(setSelectionRendering(bool)),gla,SLOT(setSelectFaceRendering(bool)) );
 connect(this, SIGNAL(setSelectionRendering(bool)),gla,SLOT(setSelectVertRendering(bool)) );

 setSelectionRendering(true);

 if(selectionMode)
	m.updateDataMask(MeshModel::MM_FACEFACETOPO);
 return true;
}
