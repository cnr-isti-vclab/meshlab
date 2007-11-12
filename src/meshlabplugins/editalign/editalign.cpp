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
#include <vcg/math/point_matching.h>
//#include <meshlab/mainwindow.h>
#include "editalign.h"
#include <wrap/gl/pick.h>
#include <wrap/qt/trackball.h>
//#include "align/AlignPair.h" 
//#include "align/AlignGlobal.h"
#include "AlignPairWidget.h"
#include "AlignPairDialog.h"

using namespace vcg;

EditAlignPlugin::EditAlignPlugin() {
  alignDialog=0;
//  curFacePtr=0;
	qFont.setFamily("Helvetica");
	qFont.setPixelSize(10);
	
	actionList << new QAction(QIcon(":/images/icon_align.png"), "Align", this);
  foreach(QAction *editAction, actionList)
    editAction->setCheckable(true);      
		
	trackball.center=Point3f(0, 0, 0);
	trackball.radius= 1;
}

QList<QAction *> EditAlignPlugin::actions() const {
	return actionList;
}


const QString EditAlignPlugin::Info(QAction *action) 
{
  if( action->text() != tr("Align") ) assert (0);

	return tr("Allow to align different layers toghether.");
}

const PluginInfo &EditAlignPlugin::Info() 
{
   static PluginInfo ai; 
   ai.Date=tr(__DATE__);
	 ai.Version = tr("1.0");
	 ai.Author = ("Paolo Cignoni");
   return ai;
} 

void EditAlignPlugin::Decorate(QAction * /*ac*/, MeshModel &m, GLArea * gla)
{
	switch(mode)
	 {
		case ALIGN_MOVE:
			// Draw the editing mesh
			gla->rm.colorMode=GLW::CMPerMesh;
			m.visible=false;
			glPushMatrix();
			trackball.GetView();
			trackball.Apply(true);
			m.Render(GLW::DMFlat,GLW::CMPerMesh,gla->rm.textureMode);
			glPopMatrix();
			break;
			
		case ALIGN_IDLE:
		{
			m.Render(GLW::DMBox,GLW::CMPerMesh,gla->rm.textureMode);
		}
		case ALIGN_INSPECT_ARC: 		
		{
			break;
	
		}
	}
}
void EditAlignPlugin::StartEdit(QAction * /*mode*/, MeshModel &_mm, GLArea *_gla )
{
	md=&_gla->meshDoc;
	gla=_gla;

	int id=0;
  meshTree.clear();
  foreach(MeshModel *mm, md->meshList)
	{
		mm->cm.C()=Color4b::Scatter(100, id,.2f,.7f);
		meshTree.nodeList.push_back(new MeshNode(mm));
		++id;
	}
	gla->rm.colorMode=GLW::CMPerMesh;
	
	//MainWindow *mainW=qobject_cast<MainWindow *> (gla->parentWidget()->parentWidget());
	//assert(mainW);
	gla->setCursor(QCursor(QPixmap(":/images/cur_align.png"),1,1));	
	if(alignDialog==0)
	{
		//alignDialog=new AlignDialog(gla->parentWidget()->parentWidget());
		alignDialog=new AlignDialog(gla->window());
		connect(alignDialog->ui.icpButton,SIGNAL(clicked()),this,SLOT(process()));
		connect(alignDialog->ui.manualAlignButton,SIGNAL(clicked()),this,SLOT(glueManual()));
		connect(alignDialog->ui.pointBasedAlignButton,SIGNAL(clicked()),this,SLOT(glueByPicking()));
		connect(alignDialog->ui.glueHereButton,SIGNAL(clicked()),this,SLOT(glueHere()));
		connect(alignDialog->ui.glueHereAllButton,SIGNAL(clicked()),this,SLOT(glueHereAll()));
	}
	alignDialog->edit=this;
	alignDialog->setTree(& meshTree, meshTree.nodeList.front());
	alignDialog->show();
	//alignDialog->adjustSize();
	
	//mainW->addDockWidget(Qt::LeftDockWidgetArea,alignDialog);
	mode=ALIGN_IDLE;	
	connect(this, SIGNAL(suspendEditToggle()),gla,SLOT(suspendEditToggle()) );

	suspendEditToggle();
}

void EditAlignPlugin::glueByPicking()
{
	if(meshTree.gluedNum()<1)
		{
			QMessageBox::warning(0,"Align tool", "Point based aligning  requires at least one glued  mesh");
			return;
		}

 Matrix44f oldTr = md->mm()->cm.Tr;
 md->mm()->cm.Tr.SetIdentity();
 AlignPairDialog *dd=new AlignPairDialog();
 dd->aa->initMesh(meshTree.find(md->mm()), &meshTree);
 dd->exec();	
 
 // i picked points sono in due sistemi di riferimento.
 
 vector<Point3f> freePnt = dd->aa->freePickedPointVec; 
 vector<Point3f> gluedPnt= dd->aa->gluedPickedPointVec; 
 
 	if(freePnt.size() != gluedPnt.size())
		{
			QMessageBox::warning(0,"Align tool", "Point based aligning requires at least one glued  mesh");
			return;
		}

 Matrix44f res;
//			A3PntFree.push_back((A3PntFree[0]+A3PntFree[1])/2.0);
//			A3PntGlue.push_back((A3PntGlue[0]+A3PntGlue[1])/2.0);
 PointMatching<float>::ComputeRigidMatchMatrix(res,gluedPnt,freePnt);
	md->mm()->cm.Tr=res;
	QString buf;
  for(int i=0;i<freePnt.size();++i)
		meshTree.cb(0,qPrintable(buf.sprintf("%f %f %f -- %f %f %f \n",freePnt[i][0],freePnt[i][1],freePnt[i][2],gluedPnt[i][0],gluedPnt[i][1],gluedPnt[i][2])));
	
//			Matrix44d old=MSel().M;
//			//old.Invert();
//			MSel().M=old*res;
//			MSel().glued=true;
//			UpdateAllViews(NULL,UPDTreeChange);		

} 

 
void EditAlignPlugin::glueManual()
{
	assert(alignDialog->currentNode->glued==false); 
	MeshModel *mm=md->mm();
	static QString oldLabelButton;
	Matrix44f tran,mtran; 
  
	switch(mode)
	{
		case ALIGN_IDLE:
				suspendEditToggle();
				mode = ALIGN_MOVE;
				md->mm()->visible=false;
				trackball.Reset();
				trackball.center= mm->cm.trBB().Center();
				trackball.radius= mm->cm.trBB().Diag()/2.0;
				toggleButtons();
				oldLabelButton=	alignDialog->ui.manualAlignButton->text();
				alignDialog->ui.manualAlignButton->setText("Store transformation");
				break;
	
		case ALIGN_MOVE:  // stop manual alignment and freeze the mesh
				mode = ALIGN_IDLE;
				toggleButtons();
				tran.SetTranslate(trackball.center);
				mtran.SetTranslate(-trackball.center);
				mm->cm.Tr= (tran) * trackball.track.Matrix()*(mtran) * mm->cm.Tr;
				mm->visible=true;
				alignDialog->currentNode->glued=true;
				alignDialog->ui.manualAlignButton->setText(oldLabelButton);
			 break;
			
		default : assert("entered in the GlueManual slot in the wrong state");
		}
	gla->update();
}




void EditAlignPlugin::glueHere()
{ 
	MeshNode *mn=meshTree.find(md->mm());
	mn->glued = !mn->glued;
	alignDialog->updateDialog();
}

void EditAlignPlugin::glueHereAll()
{
	foreach(MeshNode *mn, meshTree.nodeList)
				mn->glued=true;
	
	alignDialog->updateDialog();
}

void EditAlignPlugin::process()
{
	if(meshTree.gluedNum()<2)
		{
			QMessageBox::warning(0,"Align tool", "Process can work only when more than two meshes have been glued");
			return;
		}
	meshTree.Process(ap);
	alignDialog->updateDialog();
	gla->update();
}


void EditAlignPlugin::mousePressEvent    (QAction *, QMouseEvent *e, MeshModel &, GLArea * )
{
	if(mode==ALIGN_MOVE)
	{
		trackball.MouseDown(e->x(),gla->height()-e->y(), QT2VCG(e->button(), e->modifiers() ) );
		gla->update();
	}
}

void EditAlignPlugin::mouseMoveEvent     (QAction *, QMouseEvent *e, MeshModel &, GLArea * ) 
{
	if(mode==ALIGN_MOVE)
	{
		trackball.MouseMove(e->x(),gla->height()-e->y() );
		gla->update();
	}
	
}

void EditAlignPlugin::mouseReleaseEvent  (QAction *,QMouseEvent * e, MeshModel &/*m*/, GLArea * gla)
{
	if(mode==ALIGN_MOVE)
	{
		trackball.MouseUp(e->x(),gla->height()-e->y(), QT2VCG(e->button(), e->modifiers() ) );
		gla->update();
	}
}


// this function toggles on and off all the buttons (according to the "modal" states of the interface),
// do not confuse it with the updatebuttons function of the alignDialog class.
void EditAlignPlugin::toggleButtons()
{
switch(mode)
	{
	case	ALIGN_MOVE:
			alignDialog->ui.manualAlignButton->setEnabled(true);
			alignDialog->ui.inspectButton->setEnabled(false);
			alignDialog->ui.icpButton->setEnabled(false);
			alignDialog->ui.icpParamButton->setEnabled(false);
			alignDialog->ui.alignTreeWidget->setEnabled(false);
			
		break;
	case	ALIGN_IDLE:
			alignDialog->ui.manualAlignButton->setEnabled(true);
			alignDialog->ui.inspectButton->setEnabled(true);
			alignDialog->ui.icpButton->setEnabled(true);
			alignDialog->ui.icpParamButton->setEnabled(true);
			alignDialog->ui.alignTreeWidget->setEnabled(true);
			break;
	}
}



// Se <relative> e ' true disegna i punti di allineamento dell'arco 
// nelle posizioni delle mesh DOPO l'allineamento globale (e quindi matchano con la surf)
// altrimenti disegna i punti di mov nella posizione data dall'allinemaneto
// (e che quindi potrebbe non matchare con quella dopo l'allineamento globale); 

void EditAlignPlugin::DrawArc( /*ArcPtr a, bool relative, const Point3d &Center, double Size */)
{
/*	AlignPair::Result &r=*a;
	NodePtr fix=FindMesh(r.FixName);
	NodePtr mov=FindMesh(r.MovName);
	double nl=2.0*(*fix).bb.Diag()/100.0;
	glPushAttrib(GL_ENABLE_BIT );
	
	glDisable(GL_LIGHTING);
	glPushMatrix();
	(*WorkingGroup).UnitTransform(Center, Size);
	
	glPushMatrix();
	glMultMatrix((*fix).A);
		glPointSize(5.0f);
		glColor3f(1,0,0);
		glBegin(GL_POINTS);
		for(int i=0;i<r.Pfix.size();i++) glVertex(r.Pfix[i]);
		glEnd();
		glPointSize(1.0f);
		glColor((*fix).mi.c);
		if(r.Nfix.size()==r.Pfix.size()) 
		{
			glBegin(GL_LINES);
			for(i=0;i<r.Pfix.size();i++) {
				glVertex(r.Pfix[i]);
				glVertex(r.Pfix[i]+r.Nfix[i]*nl);
			}
			glEnd();
		}
		if(!relative){
			glPopMatrix();
			glPushMatrix();
			glMultMatrix((*mov).A);
		}
		else	glMultMatrix(r.Tr);
		
		glPointSize(5.0f);
		glColor3f(0,0,1);
		glBegin(GL_POINTS);
		for(i=0;i<r.Pmov.size();i++) glVertex(r.Pmov[i]);
		glEnd();
		glPointSize(1.0f);
		glColor((*mov).mi.c);
		if(r.Nmov.size()==r.Pmov.size()) 
		{
			glBegin(GL_LINES);
			for(i=0;i<r.Pmov.size();i++) {
				glVertex(r.Pmov[i]);
				glVertex(r.Pmov[i]+r.Nmov[i]*nl);
			}
			glEnd();
		}
		glPopMatrix();
		glPopMatrix();
		// Now Draw the histogram	
		
		int HSize = ViewPort[2]-100;
		r.H.glDraw(Point4i(20,80,HSize,100),dlFont,r.as.I[0].MinDistAbs,1);
		
		glPopAttrib(); 
	*/
}

Q_EXPORT_PLUGIN(EditAlignPlugin)
