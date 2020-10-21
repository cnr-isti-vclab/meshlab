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
#include <meshlab/glarea.h>
#include <wrap/qt/trackball.h>
#include <PhotoTexturingWidget.h>
#include "photoTexturing.h"

using namespace std;
using namespace vcg;

/**
 * The PhotoTexturingPlugin is an edit plugin, which allows to texture a mesh using
 * a calibrated camera and a corresponding image. It is also possible to use more 
 * then one camera to generate a texture for the mesh. The texture of each individuell
 * camera can be merged with the other camera to one texture file.
 */
PhotoTexturingPlugin::PhotoTexturingPlugin() {
	qFont.setFamily("Helvetica");
	qFont.setPixelSize(10);
		
	ptDialog = NULL;
	glArea = NULL;
	// inits the PhotoTexturer
	photoTexturer = new PhotoTexturer();
}

const QString PhotoTexturingPlugin::Info() {
	return tr("Generates a texture for a mesh using a calibrated camera and a photo made by this camera.");
}

void PhotoTexturingPlugin::StartEdit(MeshModel &m, GLArea *gla)
{	
	//Creat GUI window
	if(ptDialog == NULL)
	{
		ptDialog = new PhotoTexturingWidget::PhotoTexturingWidget(this,photoTexturer,m, gla);
		//ptDialog->setWindowFlags(Qt::WindowStaysOnTopHint);
		//qDebug()<<"new PhotoTexturingDialog";
	}
	glArea = gla;
	
	//connect(this,SIGNAL(suspendEditToggle()),gla,SLOT(suspendEditToggle()));
	//emit suspendEditToggle();
	
	//show the dialog
	ptDialog->show();
}

void PhotoTexturingPlugin::mousePressEvent(QMouseEvent *e, MeshModel &mm, GLArea *glArea)
{
	//just passes throgh the mouse events to the GLArea
	e->accept();
	glArea->setFocus();     
	if (glArea->isDefaultTrackBall())
		glArea->trackball.MouseDown(e->x(),glArea->height()-e->y(), QT2VCG(e->button(), e->modifiers() ) );
	else glArea->trackball_light.MouseDown(e->x(),glArea->height()-e->y(), QT2VCG(e->button(), Qt::NoModifier ) );
	glArea->update();
}

void PhotoTexturingPlugin::mouseMoveEvent(QMouseEvent *e, MeshModel &, GLArea *glArea)
{ 
	//just passes throgh the mouse move events to the GLArea
	if(e->buttons() | Qt::LeftButton) 
	{
	    if (glArea->isDefaultTrackBall()) 
		{
	    	glArea->trackball.MouseMove(e->x(),glArea->height()-e->y());
	    	//glArea->setCursorTrack(glArea->trackball.current_mode);
		}
	    else glArea->trackball_light.MouseMove(e->x(),glArea->height()-e->y());
	    glArea->update();
	}
}

// When mouse is released we set the correct mouse curson
void PhotoTexturingPlugin::mouseReleaseEvent(QMouseEvent *e, MeshModel &, GLArea *glArea)
{
          if (glArea->isDefaultTrackBall()) glArea->trackball.MouseUp(e->x(),glArea->height()-e->y(), QT2VCG(e->button(), e->modifiers() ) );
	        else glArea->trackball_light.MouseUp(e->x(),glArea->height()-e->y(), QT2VCG(e->button(),e->modifiers()) );
          //glArea->setCursorTrack(glArea->trackball.current_mode);
        
	
          glArea->update();
}

void PhotoTexturingPlugin::EndEdit(MeshModel &/*m*/, GLArea *gla){
	//gla->reloadTexture();
	ptDialog->hide();
	//delete ptDialog;
	//ptDialog = NULL;
}
