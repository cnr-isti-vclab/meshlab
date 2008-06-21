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
//#include "holePatch.h"
//#include <qstring.h>

#include <wrap/gl/pick.h>
#include <vcg/complex/trimesh/hole.h>

using namespace vcg;


EditHolePlugin::EditHolePlugin() {
	isListUpdate = false;
	QAction* editFill = new QAction(QIcon(":/images/icon_filler.png"),"Fill Hole", this);
	actionList << editFill;
	QAction *editAction;
	dialogFiller = 0;

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

/*
 Trackball::Button QT2VCG(Qt::MouseButton qtbt,  Qt::KeyboardModifiers modifiers)
{

	int vcgbt=Trackball::BUTTON_NONE;
	if(qtbt & Qt::LeftButton		) vcgbt |= Trackball::BUTTON_LEFT;
	if(qtbt & Qt::RightButton		) vcgbt |= Trackball::BUTTON_RIGHT;
	if(qtbt & Qt::MidButton			) vcgbt |= Trackball::BUTTON_MIDDLE;
	if(modifiers & Qt::ShiftModifier   ) vcgbt |= Trackball::KEY_SHIFT;
	if(modifiers & Qt::ControlModifier ) vcgbt |= Trackball::KEY_CTRL;
	if(modifiers & Qt::AltModifier     ) vcgbt |= Trackball::KEY_ALT;
	
	return Trackball::Button(vcgbt);
}
*/

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

void EditHolePlugin::mouseMoveEvent     (QAction *,QMouseEvent * e, MeshModel &/*m*/, GLArea * gla)
{
	if( (e->buttons()| Qt::LeftButton) &&
	    !(e->modifiers() & Qt::ShiftModifier))
	{
		//gla->trackball.MouseMove(e->x(),gla->height()-e->y());
		
	}
	gla->update();
}

void EditHolePlugin::StartEdit(QAction * , MeshModel &m, GLArea *gla )
{	
	// if plugin restart with another mesh, recomputing of hole is forced
	if(mesh != &m)
	{
		this->mesh = &m;
		this->gla = gla;
		isListUpdate = false;
	}

	if( !dialogFiller )
	{
		dialogFiller=new FillerDialog(gla->window());
		dialogFiller->show();
		dialogFiller->setAllowedAreas(Qt::NoDockWidgetArea);
		connect(dialogFiller, SIGNAL(SGN_ProcessFilling()), this,SLOT(fill()));
		connect(dialogFiller, SIGNAL(SGN_Apply()), this, SLOT(ApplyFilling()) );
		connect(dialogFiller, SIGNAL(SGN_UpdateGLArea()), this, SLOT(upGlA()));
		connect(dialogFiller, SIGNAL(SGN_RefreshHoles()), this, SLOT(refreshHoles()));
		connect(dialogFiller, SIGNAL(SGN_UpdateHoleSelection()), this, SLOT(refreshSelection()));
		connect(this, SIGNAL(SGN_SuspendEditToggle()),gla,SLOT(suspendEditToggle()) );
		connect(dialogFiller, SIGNAL(SGN_Closing()),gla,SLOT(endEdit()) );		
	}
	markBorders();
	updateUI();
	Decorate(0, m, gla);
		
	SGN_SuspendEditToggle();
}

void EditHolePlugin::Decorate(QAction * ac, MeshModel &m, GLArea * gla)
{
	glPushMatrix();
	glMultMatrix(mesh->cm.Tr);
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT | GL_DEPTH_BUFFER_BIT);
	
	drawHoles();

	if(hasPick )
	{
		hasPick = false;
		pickedFace =0;
		GLPickTri<CMeshO>::PickNearestFace(cur.x(), gla->curSiz.height() - cur.y(), m.cm, pickedFace, 4, 4);
		// guardo se nella faccia più vicina uno dei vertici è di bordo
		if( pickedFace != 0 )
		{
			int selIndex = FgtHole<CMeshO>::FindHoleFromBorderFace(pickedFace, holes);
			if(selIndex>=0) toggleSelection(selIndex);
			pickedFace = 0;
		}
	}

	/*
	if( pickedFace != 0 )
	{	
		glColor3f(0, 1, 1);
		glBegin(GL_TRIANGLES);
		 glVertex( pickedFace->V(0)->P() );
		 glVertex( pickedFace->V(1)->P() );
		 glVertex( pickedFace->V(2)->P() );
		glEnd();
	}
	*/

	glPopAttrib();	
	glPopMatrix();

	gla->update();	
 }

 void EditHolePlugin::EndEdit(QAction * , MeshModel &m, GLArea *gla ){
	 if ( dialogFiller!=0) {
		delete  dialogFiller; 
		dialogFiller=0;
	 }
 }


void EditHolePlugin::upGlA()
{ 
	gla->update();	 
}

void EditHolePlugin::resetHoles()
{
	if(mesh == 0)
		return;

	mesh->clearDataMask( MeshModel::MM_BORDERFLAG | MeshModel::MM_FACETOPO );
	
	CMeshO::FaceIterator fi = mesh->cm.face.begin();
	for( ; fi!=mesh->cm.face.end(); ++fi)
	{
		if( (*fi).V(0)->IsV() )
			(*fi).V(0)->ClearV();
		if( (*fi).V(1)->IsV() )
			(*fi).V(1)->ClearV();
		if( (*fi).V(2)->IsV() )
			(*fi).V(2)->ClearV();
	}
	isListUpdate = false;	
}

void EditHolePlugin::refreshSelection()
{
	for(int i=0; i<dialogFiller->ui.hListW->count(); ++i)
		if( dialogFiller->ui.hListW->item(i)->isSelected() )
			holes.at(i).isSelected = true;
		else
			holes.at(i).isSelected = false;
}


void EditHolePlugin::markBorders()
{
	if(isListUpdate || mesh==0)
		return; // evito di ricalcolare gli hole quando non necessario, visto che costa

	holes.clear();
	mesh->updateDataMask(MeshModel::MM_FACETOPO);
	mesh->updateDataMask(MeshModel::MM_BORDERFLAG);

	FgtHole<CMeshO>::GetMeshHoles(mesh->cm, holes);
	//HoleFinder<CMeshO>::GetBoundHoles(mesh->cm, holeList);
	isListUpdate = true;
}




void EditHolePlugin::updateUI()
{
	//Si aggiorna nel dialogo la lista dei buchi
	HoleVector::const_iterator it = holes.begin();

	dialogFiller->ui.hListW->clear();
	int i = 0;
	for( ; it != holes.end(); ++it) {
		++i;
		dialogFiller->ui.hListW->addItem( QString("Hole_%1").arg( i ));
	}
}

void EditHolePlugin::toggleSelection(int holeIndex)
{
	QListWidgetItem* itm = dialogFiller->ui.hListW->item(holeIndex);
	itm->setSelected(!itm->isSelected() );
}


void EditHolePlugin::drawHoles() const
{
	glLineWidth(2.0f);
	glDepthFunc(GL_ALWAYS);
	glDisable(GL_DEPTH_TEST); 
	glDepthMask(GL_FALSE);
	glDisable(GL_LIGHTING);

	HoleVector::const_iterator it = holes.begin();
	// scorro tutti i buchi
	for( ; it != holes.end(); ++it)
	{
		if( it->isSelected )
			glColor(Color4b::DarkGreen);
		else
			glColor(Color4b::DarkRed);
		it->Draw();
	}

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LESS);

	for(it = holes.begin(); it != holes.end(); ++it)
	{
		if( it->isSelected )
			glColor(Color4b::Green);
		else
			glColor(Color4b::Red);
		it->Draw();
	}	
}


void EditHolePlugin::refreshHoles()
{
	resetHoles();
	markBorders();
	updateUI();
}

/** Insert into mesh data structure the primiteves used to fill the hole
 */
void EditHolePlugin::ApplyFilling()
{
	// TO DO
}


//void EditHolePlugin::DrawPlane(GLArea * gla, MeshModel &m){
//	 
//	b=m.cm.bbox; //Boundig Box
//	Point3f mi=b.min;
//	Point3f ma=b.max;
//	Point3f centre=b.Center() ;
//	edgeMax=0;
//	float LX= ma[0]-mi[0];
//	float LY= ma[1]-mi[1];
//	float LZ= ma[2]-mi[2];
//	edgeMax= max(LX, LY);
//	edgeMax=max(edgeMax, LZ); //edgeMax: the longest side of BBox
//	glPushMatrix();
//	glPushAttrib(GL_COLOR_BUFFER_BIT|GL_LIGHTING_BIT);
//	trackball_slice.GetView();
//	trackball_slice.Apply(true);
//	trackball_slice.center=centre;
//	trackball_slice.radius=edgeMax;
//	glColor4f(1.0,0.0,0.0,0.8);
//
//	glEnable(GL_BLEND); 
//	glEnable(GL_COLOR_MATERIAL);
//	point_Vector.clear();
//	/*
//	for(int i=1; i<=(plane); i++){
//	  
//		glEnable(GL_CULL_FACE);
//		glColor4f(0,1,0,0.5);
//		glBegin(GL_QUADS);
//			glNormal3f(1,0,0);
//			
//			float assi_x[4];
//			assi_x[0]=mi[0]+(layer*i);
//			assi_x[1]=mi[0]+(layer*((plane+1)-i));
//			assi_x[2]=centre[0]-((dialogFiller->getDistance()*(plane+1))/2)+(dialogFiller->getDistance()*i);
//			assi_x[3]=centre[0]-((dialogFiller->getDistance()*(plane+1))/2)+(dialogFiller->getDistance()*((plane+1)-i));	
//			
//			this->point_Vector.push_back(Point3f(assi_x[in_ass], centre[1], centre[2]));
//			
//			glVertex3f(assi_x[in_ass], centre[1]-edgeMax, centre[2]-edgeMax);
//			glVertex3f(assi_x[in_ass], centre[1]+edgeMax, centre[2]-edgeMax);
//			glVertex3f(assi_x[in_ass], centre[1]+edgeMax, centre[2]+edgeMax);
//			glVertex3f(assi_x[in_ass], centre[1]-edgeMax, centre[2]+edgeMax);
//		glEnd();
//		
//		glColor4f(1,0,0,0.5);
//		glBegin(GL_QUADS);
//		glNormal3f(-1,0,0);
//		glVertex3f(assi_x[in_ass+1], centre[1]-edgeMax, centre[2]-edgeMax);
//		glVertex3f(assi_x[in_ass+1], centre[1]-edgeMax, centre[2]+edgeMax);
//		glVertex3f(assi_x[in_ass+1], centre[1]+edgeMax, centre[2]+edgeMax);
//		glVertex3f(assi_x[in_ass+1], centre[1]+edgeMax, centre[2]-edgeMax);
//		glEnd();
//		}
//		*/
//
//	  glPopAttrib();
//	  glPopMatrix();
//	  if(isDragging){
//		 isDragging=false;
//		 gla->update();
//	  }
//	
// }

 void EditHolePlugin::fill()
 {	 
	dialogFiller->hide();
	HoleVector::iterator it = holes.begin();
	for( ; it != holes.end(); it++ )
	{
		if( it->isSelected )
		{

			// TO DO: RIEMPIMENTO USANDO INFO DEL FGTHOLE
		}
	}

    dialogFiller->show();
 }


 
 Q_EXPORT_PLUGIN(EditHolePlugin)
