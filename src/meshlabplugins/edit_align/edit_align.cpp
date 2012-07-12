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
#include "edit_align.h"
#include <wrap/gl/pick.h>
#include <wrap/qt/trackball.h>
#include "AlignPairWidget.h"
#include "AlignPairDialog.h"
#include "align/align_parameter.h"
#include <meshlab/stdpardialog.h>
#include <QMetaObject>

using namespace vcg;

EditAlignPlugin::EditAlignPlugin()
{
	alignDialog=0;
	qFont.setFamily("Helvetica");
	qFont.setPixelSize(10);      

	trackball.center=Point3f(0, 0, 0);
	trackball.radius= 1;
}

const QString EditAlignPlugin::Info() 
{
	return tr("Allow to align different layers toghether.");
}

void EditAlignPlugin::Decorate(MeshModel &m, GLArea * gla)
{
	switch(mode)
	{
	case ALIGN_MOVE:
		{
			// Draw the editing mesh
			gla->rm.colorMode=GLW::CMPerMesh;
			m.visible=false;
			glPushMatrix();
			trackball.GetView();
			trackball.Apply(true);
			m.render(GLW::DMFlat,GLW::CMPerMesh,gla->rm.textureMode);
			glPopMatrix();
			break;
		}
	case ALIGN_IDLE:
		{
			m.render(GLW::DMBox,GLW::CMNone,GLW::TMNone);
			if(alignDialog->currentArc!=0)
				DrawArc(alignDialog->currentArc);
		}
	case ALIGN_INSPECT_ARC: 		
		{
			break;

		}
	}
}

bool EditAlignPlugin::StartEdit(MeshDocument &_md, GLArea *_gla )
{
	if (_md.mm() == NULL)
		return false;
	this->md=&_md;
	gla=_gla;

	int id=0;
	int numOfMeshes = md->meshList.size();
	meshTree.clear();
	foreach(MeshModel *mm, md->meshList)
	{
		// assigns random color: if less than 50 meshes, color is truly unique, and the less meshes, the more different they will be
		// if above 50, truly unique color would geenrate too similar colors, so total number of unique color
		// is capped to 50 and the color reused, id that are close will have different color anyway
		if(numOfMeshes < 50)
			mm->cm.C()=Color4b::Scatter(numOfMeshes+1, id, .2f, .7f);
		else
			mm->cm.C()=Color4b::Scatter(51, id%50, .2f, .7f);
		meshTree.nodeList.push_back(new MeshNode(mm,id));
		++id;
	}
	gla->rm.colorMode=GLW::CMPerMesh;

	gla->setCursor(QCursor(QPixmap(":/images/cur_align.png"),1,1));	
	if(alignDialog==0)
	{
        alignDialog=new AlignDialog(gla->window(),this);
		connect(alignDialog->ui.icpParamButton,SIGNAL(clicked()),this,SLOT(alignParam()));
		connect(alignDialog->ui.icpParamCurrentButton,SIGNAL(clicked()),this,SLOT(alignParamCurrent()));
		connect(alignDialog->ui.icpButton,SIGNAL(clicked()),this,SLOT(process()));
		connect(alignDialog->ui.manualAlignButton,SIGNAL(clicked()),this,SLOT(glueManual()));
		connect(alignDialog->ui.pointBasedAlignButton,SIGNAL(clicked()),this,SLOT(glueByPicking()));
		connect(alignDialog->ui.glueHereButton,SIGNAL(clicked()),this,SLOT(glueHere()));
		connect(alignDialog->ui.glueHereAllButton,SIGNAL(clicked()),this,SLOT(glueHereAll()));
		connect(alignDialog->ui.falseColorCB, SIGNAL(stateChanged(int)) , this,  SLOT(toggledColors(int) ));
		connect(alignDialog->ui.recalcButton, SIGNAL(clicked()) , this,  SLOT(recalcCurrentArc() ) );
		connect(alignDialog->ui.hideRevealButton,  SIGNAL(clicked()) , this,  SLOT(hideRevealGluedMesh() ) );
		connect(alignDialog, SIGNAL(updateMeshSetVisibilities() ), this->gla,SLOT(updateMeshSetVisibilities()));
		connect(alignDialog->ui.baseMeshButton, SIGNAL(clicked()) , this,  SLOT(setBaseMesh() ) );
	}

	//alignDialog->setCurrentNode(meshTree.find(gla->mm()) );
	alignDialog->setTree(& meshTree);
	alignDialog->show();
	//alignDialog->adjustSize();

	//mainW->addDockWidget(Qt::LeftDockWidgetArea,alignDialog);
	mode=ALIGN_IDLE;	
	connect(this, SIGNAL(suspendEditToggle()),gla,SLOT(suspendEditToggle()) );
	connect(alignDialog, SIGNAL(closing()),gla,SLOT(endEdit()) );
	suspendEditToggle();
	toggledColors(alignDialog->ui.falseColorCB->checkState());
	return true;
}

void EditAlignPlugin::EndEdit(MeshModel &/*m*/, GLArea * /*parent*/)
{
	// some cleaning at the end.
	qDebug("EndEdit: cleaning everything");
	toggledColors(Qt::Unchecked);
	meshTree.clear();
	assert(alignDialog);
	delete alignDialog;
	alignDialog=0;
}    

void EditAlignPlugin::hideRevealGluedMesh()
{
	foreach(MeshNode *mn, meshTree.nodeList)
		if(!mn->glued) mn->m->visible=!(mn->m->visible);

	alignDialog->rebuildTree();
	gla->update();
}

void EditAlignPlugin::setBaseMesh()
{
	Matrix44d oldTr = Matrix44d::Construct(md->mm()->cm.Tr);
	Matrix44d inv = Inverse(oldTr);
	md->mm()->cm.Tr.SetIdentity();

	foreach(MeshNode *mn, meshTree.nodeList)
		if(mn->glued && (mn->m != md->mm()) )
			mn->m->cm.Tr.Import(inv*Matrix44d::Construct(mn->m->cm.Tr));

	alignDialog->rebuildTree();
	gla->update();
}


void EditAlignPlugin::glueByPicking()
{
	if(meshTree.gluedNum()<1)
	{
		QMessageBox::warning(0,"Align tool", "Point based aligning requires at least one glued  mesh");
		return;
	}

	Matrix44f oldTr = md->mm()->cm.Tr;
	md->mm()->cm.Tr.SetIdentity();
	AlignPairDialog *dd=new AlignPairDialog();
	dd->aa->initMesh(currentNode(), &meshTree);
	dd->aa->isUsingVertexColor = !alignDialog->ui.falseColorCB->isChecked();
	dd->aa->usePointRendering = alignDialog->ui.pointRenderingCB->isChecked();
	dd->exec();	

	if(dd->result()==QDialog::Rejected) return;

	// i picked points sono in due sistemi di riferimento.

	std::vector<vcg::Point3f>freePnt = dd->aa->freePickedPointVec; 
	std::vector<vcg::Point3f>gluedPnt= dd->aa->gluedPickedPointVec; 

	if( (freePnt.size() != gluedPnt.size())	|| (freePnt.size()==0) )	{
		QMessageBox::warning(0,"Align tool", "require the same number of chosen points");
		return;
	}

	Matrix44f res;
	if(dd->allowScalingCB->isChecked())
		PointMatching<float>::ComputeSimilarityMatchMatrix(res,gluedPnt,freePnt);
	else 
		PointMatching<float>::ComputeRigidMatchMatrix(res,gluedPnt,freePnt);

	//md->mm()->cm.Tr=res;
	currentNode()->tr()=res;
	QString buf;
	// for(size_t i=0;i<freePnt.size();++i)
	//		meshTree.cb(0,qPrintable(buf.sprintf("%f %f %f -- %f %f %f \n",freePnt[i][0],freePnt[i][1],freePnt[i][2],gluedPnt[i][0],gluedPnt[i][1],gluedPnt[i][2])));

	assert(currentNode()->glued==false);

	currentNode()->glued=true;
	alignDialog->rebuildTree();	
	gla->update();
} 


void EditAlignPlugin::glueManual()
{
	assert(currentNode()->glued==false); 
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
		currentNode()->glued=true;
		alignDialog->ui.manualAlignButton->setText(oldLabelButton);
		break;

	default : assert("entered in the GlueManual slot in the wrong state"==0);
	}
	gla->update();
}

void EditAlignPlugin:: alignParamCurrent()
{
	assert(currentArc());

	RichParameterSet alignParamSet;
	QString titleString=QString("Current Arc (%1 -> %2) Alignment Parameters").arg(currentArc()->MovName).arg(currentArc()->FixName);
	AlignParameter::buildRichParameterSet(currentArc()->ap, alignParamSet);

	GenericParamDialog ad(alignDialog,&alignParamSet,titleString);
	ad.setWindowFlags(Qt::Dialog);
	ad.setWindowModality(Qt::WindowModal);
	int result=ad.exec();
	if(result != QDialog::Accepted) return;

	// Dialog accepted. get back the values
	AlignParameter::buildAlignParameters(alignParamSet, currentArc()->ap);
}

void EditAlignPlugin:: alignParam()
{
	RichParameterSet alignParamSet;
	AlignParameter::buildRichParameterSet(defaultAP, alignParamSet);
	GenericParamDialog ad(alignDialog,&alignParamSet,"Default Alignment Parameters");
	ad.setWindowFlags(Qt::Dialog);
	ad.setWindowModality(Qt::WindowModal);
    int result=ad.exec();
	if(result != QDialog::Accepted) return;
	// Dialog accepted. get back the values
	AlignParameter::buildAlignParameters(alignParamSet, defaultAP);
}

void EditAlignPlugin::glueHere()
{ 
	MeshNode *mn=currentNode();
	mn->glued = !mn->glued;
	alignDialog->rebuildTree();	
}

void EditAlignPlugin::glueHereAll()
{
	foreach(MeshNode *mn, meshTree.nodeList)
		mn->glued=true;

	alignDialog->rebuildTree();	
}

void EditAlignPlugin::process()
{
	if(meshTree.gluedNum()<2)
	{
		QMessageBox::warning(0,"Align tool", "Process can work only when more than two meshes have been glued");
		return;
	}
	meshTree.Process(defaultAP);
	alignDialog->rebuildTree();	
	gla->update();
}

void EditAlignPlugin::recalcCurrentArc()
{
	assert(currentArc());

	meshTree.ProcessArc(currentArc()->FixName,currentArc()->MovName,*currentArc(),currentArc()->ap);
	meshTree.ProcessGlobal(currentArc()->ap);

	alignDialog->rebuildTree();	
	gla->update();
}


void EditAlignPlugin::mousePressEvent(QMouseEvent *e, MeshModel &, GLArea * )
{
	if(mode==ALIGN_MOVE)
	{
		trackball.MouseDown(e->x(),gla->height()-e->y(), QT2VCG(e->button(), e->modifiers() ) );
		gla->update();
	}
}

void EditAlignPlugin::mouseMoveEvent(QMouseEvent *e, MeshModel &, GLArea * ) 
{
	if(mode==ALIGN_MOVE)
	{
		trackball.MouseMove(e->x(),gla->height()-e->y() );
		gla->update();
	}

}

void EditAlignPlugin::mouseReleaseEvent(QMouseEvent * e, MeshModel &/*m*/, GLArea * gla)
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
		alignDialog->ui.recalcButton->setEnabled(false);
		alignDialog->ui.icpButton->setEnabled(false);
		alignDialog->ui.icpParamButton->setEnabled(false);
		alignDialog->ui.alignTreeWidget->setEnabled(false);
		alignDialog->ui.baseMeshButton->setEnabled(false);


		break;
	case	ALIGN_IDLE:
		alignDialog->ui.manualAlignButton->setEnabled(true);
		alignDialog->ui.recalcButton->setEnabled(true);
		alignDialog->ui.icpButton->setEnabled(true);
		alignDialog->ui.icpParamButton->setEnabled(true);
		alignDialog->ui.alignTreeWidget->setEnabled(true);
		alignDialog->ui.baseMeshButton->setEnabled(true);
		break;
	}
}

void EditAlignPlugin::DrawArc(vcg::AlignPair::Result *A )
{	
	unsigned int i;
	AlignPair::Result &r=*A;
	MeshNode *fix=meshTree.find(r.FixName);
	MeshNode *mov=meshTree.find(r.MovName);
	//int mov=FindMesh(r.MovName);
	double nl=2.0*(*fix).bbox().Diag()/100.0;
	glPushAttrib(GL_ENABLE_BIT );

	glDisable(GL_LIGHTING);

	glPushMatrix();
	glMultMatrix(fix->tr());
	glPointSize(5.0f);
	glColor3f(1,0,0);
	glBegin(GL_POINTS);
	for(i=0;i<r.Pfix.size();i++) glVertex(r.Pfix[i]);
	glEnd();
	glPointSize(1.0f);
	//glColor((*fix).mi.c);
	if(r.Nfix.size()==r.Pfix.size()) 
	{
		glBegin(GL_LINES);
		for(i=0;i<r.Pfix.size();i++) {
			glVertex(r.Pfix[i]);
			glVertex(r.Pfix[i]+r.Nfix[i]*nl);
		}
		glEnd();
	}

	glPopMatrix();
	glPushMatrix();
	glMultMatrix(mov->tr());

	glPointSize(5.0f);
	glColor3f(0,0,1);
	glBegin(GL_POINTS);
	for(i=0;i<r.Pmov.size();i++) glVertex(r.Pmov[i]);
	glEnd();
	glPointSize(1.0f);
	//glColor((*mov).mi.c);
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
	/*
	// Now Draw the histogram	

	int HSize = ViewPort[2]-100;
	r.H.glDraw(Point4i(20,80,HSize,100),dlFont,r.as.I[0].MinDistAbs,1);
	*/		
	glPopAttrib(); 

}

void EditAlignPlugin::toggledColors(int colorstate)
{
	if(colorstate == Qt::Checked)
		gla->rm.colorMode=GLW::CMPerMesh;
	else
		gla->rm.colorMode=GLW::CMPerVert;
	gla->update();
}
