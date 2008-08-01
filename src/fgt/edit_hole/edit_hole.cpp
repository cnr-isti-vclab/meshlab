/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004                                                \/)\/    *
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
#include <QString>
#include <math.h>
#include <stdlib.h>
#include <meshlab/glarea.h>
#include "edit_hole.h"
#include "fgtHole.h"
#include "holeListModel.h"

//#include "holePatch.h"
//#include <qstring.h>

#include <wrap/gl/pick.h>
#include <vcg/complex/trimesh/hole.h>

using namespace vcg;




#include <vcg/complex/trimesh/update/position.h>

//#include <vcg/complex/trimesh/update/bounding.h>
//#include <vcg/complex/trimesh/update/selection.h>
//#include <vcg/space/normal_extrapolation.h>

EditHolePlugin::EditHolePlugin() {
	QAction* editFill = new QAction(QIcon(":/images/icon_filler.png"),"Fill Hole", this);
	actionList << editFill;
	QAction *editAction;
	dialogFiller = 0;
	holesModel = 0;

	foreach(editAction, actionList)
		editAction->setCheckable(true);

	pickedFace = 0;
	hasPick = false;
}

EditHolePlugin::~EditHolePlugin() {
	if ( dialogFiller!=0) { 
		delete  dialogFiller; 
		dialogFiller=0;
	}
}

QList<QAction *> EditHolePlugin::actions() const {
	return actionList;
}

const QString EditHolePlugin::Info(QAction *action) 
{
  if( action->text() != tr("Fill mesh's hole") ) assert (0);

	return tr("Allow fill one or more hole into place");
}
const PluginInfo &EditHolePlugin::Info() 
{
   static PluginInfo ai; 
   ai.Date=tr(__DATE__);
	 ai.Version = tr("0.1");
	 ai.Author = ("Michele Vannoni");
   return ai;
 } 


void EditHolePlugin::mouseReleaseEvent  (QAction *,QMouseEvent * e, MeshModel &/*m*/, GLArea * gla)
{
	 //gla->trackball.MouseUp(e->x(),gla->height()-e->y(),QT2VCG(e->button(), e->modifiers()));
}

void EditHolePlugin::mousePressEvent(QAction *, QMouseEvent * e, MeshModel &m, GLArea * gla)
{
	if ( (e->button()==Qt::LeftButton) )
	{
		cur.setX(e->x());
		cur.setY(e->y());
		hasPick = true;
	}
}

void EditHolePlugin::mouseMoveEvent(QAction *,QMouseEvent * e, MeshModel &/*m*/, GLArea * gla)
{
	/*
	if( (e->buttons()| Qt::LeftButton) &&
	    !(e->modifiers() & Qt::ShiftModifier))
	{
		//gla->trackball.MouseMove(e->x(),gla->height()-e->y());
		
	}
	gla->update();
	*/
}

void EditHolePlugin::StartEdit(QAction * , MeshModel &m, GLArea *gla )
{	
	// necessario per evitare di avere 2 istanze del filtro se si cambia mesh
	// senza chidere il filtro
	if(dialogFiller != 0)
		EndEdit(0, m, gla);
	
	// if plugin restart with another mesh, recomputing of hole is forced
	if(mesh != &m)
	{
		this->mesh = &m;
		this->gla = gla;
	}

	dialogFiller=new FillerDialog(gla->window());
	dialogFiller->show();
	dialogFiller->setAllowedAreas(Qt::NoDockWidgetArea);
	connect(dialogFiller, SIGNAL(SGN_ProcessFilling()), this,SLOT(fill()));
	connect(dialogFiller, SIGNAL(SGN_CancelFill()), this, SLOT(CancelFilling()) );
	connect(this, SIGNAL(SGN_SuspendEditToggle()),gla,SLOT(suspendEditToggle()) );
	connect(dialogFiller, SIGNAL(SGN_Closing()),gla,SLOT(endEdit()) );
	connect(dialogFiller->ui.holeTree->header(), SIGNAL(sectionCountChanged(int, int)), this, SLOT(resizeViewColumn()) );
	if(holesModel != 0)
	{
		delete holeSorter;
		delete holesModel;
	}
	holesModel = new HoleListModel(&m);
	connect(holesModel, SIGNAL(SGN_needUpdateGLA()), this, SLOT(upGlA()) );
	holeSorter = new HoleSorterFilter(); 
	holeSorter->setSourceModel(holesModel);
	dialogFiller->ui.holeTree->setModel( holeSorter );
	
	Decorate(0, m, gla);
	gla->update();	
}

void EditHolePlugin::Decorate(QAction * ac, MeshModel &m, GLArea * gla)
{
	glPushMatrix();
	glMultMatrix(mesh->cm.Tr);
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT | GL_DEPTH_BUFFER_BIT);
	
	holesModel->drawCompenetratingFaces();
	holesModel->drawHoles();
	
	if(hasPick)
	{
		hasPick = false;
		pickedFace =0;
		GLPickTri<CMeshO>::PickNearestFace(cur.x(), gla->curSiz.height() - cur.y(), m.cm, pickedFace, 4, 4);
		// guardo se nella faccia più vicina uno dei vertici è di bordo
		if( pickedFace != 0 )
			holesModel->toggleSelectionHoleFromBorderFace(pickedFace);		
	}

	glPopAttrib();	
	glPopMatrix();
 }

 void EditHolePlugin::EndEdit(QAction * , MeshModel &m, GLArea *gla ){
	 if(holesModel->getState() == HoleListModel::Filled)
		holesModel->acceptFilling(true);
	
	 if ( dialogFiller!=0) {
		delete  dialogFiller;
		delete holesModel;
		dialogFiller = 0;
		holesModel = 0;
		mesh = 0;
	 }
 }


void EditHolePlugin::upGlA()
{ 
	gla->update();
}

void EditHolePlugin::resizeViewColumn()
{

	dialogFiller->ui.holeTree->header()->resizeSections(QHeaderView::ResizeToContents);
	//dialogFiller->ui.holeTree->row
}

void EditHolePlugin::fill()
{
	Qt::CheckState asi = dialogFiller->ui.antiSelfIntersection->checkState();
	if(holesModel->getState() == HoleListModel::Selection)
		holesModel->fill( asi == Qt::Checked);
	else
	{
		holesModel->acceptFilling();
		gla->setWindowModified(true);
	}

	if(holesModel->getState() == HoleListModel::Selection)
		dialogFiller->ui.fillButton->setText("Fill");
	else 
		dialogFiller->ui.fillButton->setText("Accept");

	gla->update();
}


/** Insert into mesh data structure the primiteves used to fill the hole
 */
void EditHolePlugin::CancelFilling()
{
	holesModel->acceptFilling(true);
	dialogFiller->ui.fillButton->setText("Fill");
}
 
 Q_EXPORT_PLUGIN(EditHolePlugin)
