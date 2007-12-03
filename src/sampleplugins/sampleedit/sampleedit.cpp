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
/****************************************************************************
  History
$Log: meshedit.cpp,v $
****************************************************************************/
#include <QtGui>

#include <math.h>
#include <stdlib.h>
#include <meshlab/glarea.h>
#include "sampleedit.h"
#include <wrap/gl/pick.h>
using namespace vcg;

SampleEditPlugin::SampleEditPlugin() {
  curFacePtr=0;
	qFont.setFamily("Helvetica");
	qFont.setPixelSize(10);
	
	actionList << new QAction(QIcon(":/images/icon_info.png"),"Get Info", this);
  foreach(QAction *editAction, actionList)
    editAction->setCheckable(true);      
}

QList<QAction *> SampleEditPlugin::actions() const {
	return actionList;
}


const QString SampleEditPlugin::Info(QAction *action) 
{
  if( action->text() != tr("Get Info") ) assert (0);

	return tr("Return detailed info about a picked face of the model.");
}

const PluginInfo &SampleEditPlugin::Info() 
{
   static PluginInfo ai; 
   ai.Date=tr(__DATE__);
	 ai.Version = tr("1.0");
	 ai.Author = ("Paolo Cignoni");
   return ai;
} 
 
void SampleEditPlugin::mouseReleaseEvent  (QAction *,QMouseEvent * event, MeshModel &/*m*/, GLArea * gla)
{
	gla->update();
	cur=event->pos();
	haveToPick=true;
}
  
void SampleEditPlugin::Decorate(QAction * /*ac*/, MeshModel &m, GLArea * gla)
{
	if(haveToPick)
	{
		vector<CMeshO::FacePointer> NewSel;  
		GLPickTri<CMeshO>::PickFace(cur.x(), gla->height() - cur.y(), m.cm, NewSel);
		if(NewSel.size()>0)
				curFacePtr=NewSel.front();
		haveToPick=false;
	}   
 if(curFacePtr)
 {
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
	glDisable(GL_DEPTH_TEST); 
	glDisable(GL_LIGHTING);
	glColor(Color4b::DarkRed);
	glBegin(GL_LINE_LOOP);
		glVertex(curFacePtr->P(0));
		glVertex(curFacePtr->P(1));
		glVertex(curFacePtr->P(2));
	glEnd();
	for(int i=0;i<3;++i)
		gla->renderText(curFacePtr->P(i)[0],curFacePtr->P(i)[1],curFacePtr->P(i)[2],
										QString("v%1:%2").arg(i).arg(curFacePtr->V(i) - &m.cm.vert[0]), qFont);
	glEnable(GL_DEPTH_TEST); 
	glColor(Color4b::Red);
	glBegin(GL_LINE_LOOP);
		glVertex(curFacePtr->P(0));
		glVertex(curFacePtr->P(1));
		glVertex(curFacePtr->P(2));
	glEnd();
	for(int i=0;i<3;++i)
		gla->renderText(curFacePtr->P(i)[0],curFacePtr->P(i)[1],curFacePtr->P(i)[2],
										QString("v%1:%2").arg(i).arg(curFacePtr->V(i) - &m.cm.vert[0]), qFont);
	glPopAttrib();
 }
}

void SampleEditPlugin::StartEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea *gla )
{
	gla->setCursor(QCursor(QPixmap(":/images/cur_info.png"),1,1));	
}


Q_EXPORT_PLUGIN(SampleEditPlugin)
