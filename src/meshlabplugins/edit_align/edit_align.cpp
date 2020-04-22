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

#include "edit_align.h"
#include <common/GLExtensionsManager.h>
#include <meshlab/glarea.h>
#include <meshlab/stdpardialog.h>
#include <wrap/qt/trackball.h>
#include "AlignPairWidget.h"
#include "AlignPairDialog.h"
#include "align/align_parameter.h"
#include <vcg/space/point_matching.h>
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
    return tr("Allows one to align different layers together.");
}

void EditAlignPlugin::suggestedRenderingData(MeshModel & /*m*/, MLRenderingData& dt)
{
	MLPerViewGLOptions opts;
	dt.get(opts);
	for (MLRenderingData::PRIMITIVE_MODALITY pr = MLRenderingData::PRIMITIVE_MODALITY(0); pr < MLRenderingData::PR_ARITY; pr = MLRenderingData::next(pr))
	{
		MLRenderingData::RendAtts atts;
		dt.get(pr, atts);

		atts[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR] = false;
		if (pr == MLRenderingData::PR_SOLID)
			atts[MLRenderingData::ATT_NAMES::ATT_FACECOLOR] = false;
		dt.set(pr, atts);
	}

	opts._perpoint_fixed_color_enabled = false;
	opts._perwire_fixed_color_enabled = false;
	opts._persolid_fixed_color_enabled = false;

	opts._perpoint_mesh_color_enabled = true;
	opts._perwire_mesh_color_enabled = true;
	opts._persolid_mesh_color_enabled = true;

	dt.set(opts);

	/*switch (mode)
	{
		case ALIGN_MOVE:
		{
			_shared->setRenderingDataPerMeshView(m.id(), _gla->context(), dt);
			m.visible = false;
			break;
		}
		case ALIGN_IDLE:
		{

			MLPerViewGLOptions opts;
			dt.get(opts);
			opts._perbbox_enabled = true;
			dt.set(opts);

			_shared->setRenderingDataPerMeshView(m.id(), _gla->context(), dt);
		}
		case ALIGN_INSPECT_ARC:
		{
			break;
		}
	}*/
}

bool EditAlignPlugin::StartEdit(MeshDocument& md, GLArea * gla, MLSceneGLSharedDataContext* cont)
{
    _md=&md;
    _gla= gla;
	_shared = cont;

	if ((_gla == NULL) || (_shared == NULL) || (md.meshList.size() < 1))
		return false;

	//mainW->addDockWidget(Qt::LeftDockWidgetArea,alignDialog);
	mode = ALIGN_IDLE;
    int numOfMeshes = _md->meshList.size();
    meshTree.clear();
    foreach(MeshModel *mm, _md->meshList)
    {

        // assigns random color: if less than 50 meshes, color is truly unique, and the less meshes, the more different they will be
        // if above 50, truly unique color would geenrate too similar colors, so total number of unique color
        // is capped to 50 and the color reused, id that are close will have different color anyway
		if (mm != NULL)
		{
			if (numOfMeshes < 50)
				mm->cm.C() = Color4b::Scatter(numOfMeshes + 1, mm->id(), .2f, .7f);
			else
				mm->cm.C() = Color4b::Scatter(51, mm->id() % 50, .2f, .7f);
			mm->updateDataMask(MeshModel::MM_COLOR);
//			meshTree.nodeList.push_back(new MeshNode(mm));
            meshTree.nodeMap[mm->id()]=new MeshNode(mm);
		}
    }

//for(QMap<int,RenderMode>::iterator it = _gla->rendermodemap.begin();it != _gla->rendermodemap.end();++it)
//    it.value().colorMode=GLW::CMPerMesh;

    _gla->setCursor(QCursor(QPixmap(":/images/cur_align.png"),1,1));
    if(alignDialog==0)
    {
        if (!GLExtensionsManager::initializeGLextensions_notThrowing())
			return false;

        alignDialog=new AlignDialog(_gla->window(),this);
        connect(alignDialog->ui.meshTreeParamButton,SIGNAL(clicked()),this,SLOT(meshTreeParam()));
        connect(alignDialog->ui.icpParamButton,SIGNAL(clicked()),this,SLOT(alignParam()));
		connect(alignDialog->ui.icpParamDefMMButton, SIGNAL(clicked()), this, SLOT(setAlignParamMM()));
		connect(alignDialog->ui.icpParamDefMButton, SIGNAL(clicked()), this, SLOT(setAlignParamM()));
        connect(alignDialog->ui.icpParamCurrentButton,SIGNAL(clicked()),this,SLOT(alignParamCurrent()));
        connect(alignDialog->ui.icpButton,SIGNAL(clicked()),this,SLOT(process()));
        connect(alignDialog->ui.manualAlignButton,SIGNAL(clicked()),this,SLOT(glueManual()));
        connect(alignDialog->ui.pointBasedAlignButton,SIGNAL(clicked()),this,SLOT(glueByPicking()));
        connect(alignDialog->ui.glueHereButton,SIGNAL(clicked()),this,SLOT(glueHere()));
        connect(alignDialog->ui.glueHereAllButton,SIGNAL(clicked()),this,SLOT(glueHereVisible()));
        connect(alignDialog->ui.recalcButton, SIGNAL(clicked()) , this,  SLOT(recalcCurrentArc() ) );
        connect(alignDialog->ui.hideRevealButton,  SIGNAL(clicked()) , this,  SLOT(hideRevealGluedMesh() ) );
        connect(alignDialog, SIGNAL(updateMeshSetVisibilities() ), _gla,SLOT(updateMeshSetVisibilities()));
        connect(alignDialog->ui.baseMeshButton, SIGNAL(clicked()) , this,  SLOT(setBaseMesh() ) );
        connect(alignDialog->ui.badArcButton, SIGNAL(clicked()) , this,  SLOT(selectBadArc() ) );
    }

    //alignDialog->setCurrentNode(meshTree.find(gla->mm()) );
    alignDialog->setTree(& meshTree);
    alignDialog->show();
    //alignDialog->adjustSize();
	
    connect(this, SIGNAL(suspendEditToggle()),_gla,SLOT(suspendEditToggle()) );
    connect(alignDialog, SIGNAL(closing()),_gla,SLOT(endEdit()) );
    connect(_md,SIGNAL(currentMeshChanged(int)),alignDialog,SLOT(currentMeshChanged(int)));
    suspendEditToggle();
    return true;
}

void EditAlignPlugin::Decorate(MeshModel & mm, GLArea * gla)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	_gla = gla;
	if (mode == ALIGN_IDLE)
	{
		MLRenderingData tmp;
		MLPerViewGLOptions opts;
		tmp.get(opts);
		opts._perbbox_enabled = true;
		tmp.set(opts);
		_shared->drawAllocatedAttributesSubset(mm.id(), _gla->context(), tmp);
		if ((alignDialog != NULL) && (alignDialog->currentArc != 0))
			DrawArc(alignDialog->currentArc);
	}

	if (mode == ALIGN_MOVE)
	{
		MLRenderingData dt;
		_shared->getRenderInfoPerMeshView(mm.id(), _gla->context(), dt);
		MLPerViewGLOptions opts;
		dt.get(opts);
		opts._perbbox_enabled = false;
		dt.set(opts);

		glPushMatrix();
		trackball.GetView();
		trackball.Apply();
		_shared->drawAllocatedAttributesSubset(mm.id(), _gla->context(),dt);
		glPopMatrix();
	}
	glPopAttrib();
}

void EditAlignPlugin::EndEdit(MeshModel &/*m*/, GLArea * /*parent*/, MLSceneGLSharedDataContext* /*cont*/)
{

    // some cleaning at the end.
    qDebug("EndEdit: cleaning everything");
    meshTree.clear();
    delete alignDialog;
    alignDialog=0;
}

void EditAlignPlugin::hideRevealGluedMesh()
{
  //    foreach(MeshNode *mn, meshTree.nodeList)
  for(auto ni=meshTree.nodeMap.begin();ni!=meshTree.nodeMap.end();++ni)
  { 
    MeshNode *mn=ni->second;
        if(!mn->glued) mn->m->visible=!(mn->m->visible);
  }
    alignDialog->rebuildTree();
    _gla->update();
    alignDialog->updateMeshVisibilities();
}

void EditAlignPlugin::setBaseMesh()
{
    Matrix44d oldTr = Matrix44d::Construct(_md->mm()->cm.Tr);
    Matrix44d inv = Inverse(oldTr);
    _md->mm()->cm.Tr.SetIdentity();

    //foreach(MeshNode *mn, meshTree.nodeList)
    for(auto ni=meshTree.nodeMap.begin();ni!=meshTree.nodeMap.end();++ni)
    { 
      MeshNode *mn=ni->second;
      if(mn->glued && (mn->m != _md->mm()) )
            mn->m->cm.Tr.Import(inv*Matrix44d::Construct(mn->m->cm.Tr));
    }

    alignDialog->rebuildTree();
    _gla->update();
}


void EditAlignPlugin::glueByPicking()
{
    if(meshTree.gluedNum()<1)
    {
        QMessageBox::warning(0,"Align tool", "Point-based aligning requires at least one glued  mesh");
        return;
    }

    //Matrix44f oldTr = md->mm()->cm.Tr;
    AlignPairDialog *dd=new AlignPairDialog(_gla,this->alignDialog);
    dd->aa->initMesh(currentNode(), &meshTree);
    dd->exec();

    if(dd->result()==QDialog::Rejected) 
		return;

    // i picked points sono in due sistemi di riferimento.

    std::vector<vcg::Point3f>freePnt = dd->aa->freePickedPointVec;
    std::vector<vcg::Point3f>gluedPnt= dd->aa->gluedPickedPointVec;

    if( (freePnt.size() != gluedPnt.size())	|| (freePnt.size()==0) )	{
        QMessageBox::warning(0,"Align tool", "ERROR: alignment requires the same number of chosen points");
        return;
    }

    Matrix44f res;
    if ((dd != NULL) && (dd->aa != NULL) && (dd->aa->allowscaling))
        ComputeSimilarityMatchMatrix(gluedPnt,freePnt,res);
    else
        ComputeRigidMatchMatrix(gluedPnt,freePnt,res);

    //md->mm()->cm.Tr=res;
	currentNode()->tr() = currentNode()->tr() * res;
    QString buf;
    // for(size_t i=0;i<freePnt.size();++i)
    //		meshTree.cb(0,qUtf8Printable(buf.sprintf("%f %f %f -- %f %f %f \n",freePnt[i][0],freePnt[i][1],freePnt[i][2],gluedPnt[i][0],gluedPnt[i][1],gluedPnt[i][2])));

    assert(currentNode()->glued==false);

    currentNode()->glued=true;
    alignDialog->rebuildTree();
    _gla->update();
}


void EditAlignPlugin::glueManual()
{
    assert(currentNode()->glued==false);
    
	if ((_md == NULL) || (_md->mm() == NULL) || (_gla == NULL) || (_gla->mvc() == NULL))
		return;

	MeshModel *mm=_md->mm();

	static QString oldLabelButton;
    Matrix44f tran,mtran, tmp;
	
    switch(mode)
    {
		case ALIGN_IDLE:
		{
			emit suspendEditToggle();
			mode = ALIGN_MOVE;
			mm->visible = false;
			trackball.Reset();
			trackball.center.Import(mm->cm.trBB().Center());
			trackball.radius = mm->cm.trBB().Diag() / 2.0;
			toggleButtons();
			oldLabelButton = alignDialog->ui.manualAlignButton->text();
			alignDialog->ui.manualAlignButton->setText("Accept Transformation");
			break;
		}

		case ALIGN_MOVE:  // stop manual alignment and freeze the mesh
		{
			emit suspendEditToggle();
			mode = ALIGN_IDLE;
			toggleButtons();
			tran.SetTranslate(trackball.center);
			mtran.SetTranslate(-trackball.center);
			tmp.Import(mm->cm.Tr);
			mm->cm.Tr.Import((tran)* trackball.track.Matrix()*(mtran)* tmp);
			mm->visible = true;
			alignDialog->ui.manualAlignButton->setText(oldLabelButton);
			currentNode()->glued = true;
			alignDialog->rebuildTree();
			break;
		}
		default : assert("entered in the GlueManual slot in the wrong state"==0);
	}

    _gla->update();
}

void EditAlignPlugin:: alignParamCurrent()
{
    assert(currentArc());

    RichParameterSet alignParamSet;
    QString titleString=QString("Current Arc (%1 -> %2) Alignment Parameters").arg(currentArc()->MovName).arg(currentArc()->FixName);
    AlignParameter::AlignPairParamToRichParameterSet(currentArc()->ap, alignParamSet);

    GenericParamDialog ad(alignDialog,&alignParamSet,titleString);
    ad.setWindowFlags(Qt::Dialog);
    ad.setWindowModality(Qt::WindowModal);
    int result=ad.exec();
    if(result != QDialog::Accepted) return;

    // Dialog accepted. get back the values
    AlignParameter::RichParameterSetToAlignPairParam(alignParamSet, currentArc()->ap);
}

void EditAlignPlugin:: meshTreeParam()
{
  RichParameterSet  meshTreeParamSet;
  AlignParameter::MeshTreeParamToRichParameterSet(defaultMTP, meshTreeParamSet);
  GenericParamDialog ad(alignDialog,&meshTreeParamSet,"Default Alignment Parameters");
  ad.setWindowFlags(Qt::Dialog);
  ad.setWindowModality(Qt::WindowModal);
  int result=ad.exec();
  if(result != QDialog::Accepted) return;
  // Dialog accepted. get back the values
  AlignParameter::RichParameterSetToMeshTreeParam(meshTreeParamSet, defaultMTP);

}

void EditAlignPlugin:: alignParam()
{
    RichParameterSet alignParamSet;
    AlignParameter::AlignPairParamToRichParameterSet(defaultAP, alignParamSet);
    GenericParamDialog ad(alignDialog,&alignParamSet,"Default Alignment Parameters");
    ad.setWindowFlags(Qt::Dialog);
    ad.setWindowModality(Qt::WindowModal);
    int result=ad.exec();
    if(result != QDialog::Accepted) return;
    // Dialog accepted. get back the values
    AlignParameter::RichParameterSetToAlignPairParam(alignParamSet, defaultAP);
}

void EditAlignPlugin::setAlignParamMM()
{
	defaultAP.SampleNum = 2000;
	defaultAP.MinDistAbs = 10.0;
	defaultAP.TrgDistAbs = 0.005;
	defaultAP.MaxIterNum = 75;
	defaultAP.ReduceFactorPerc = 0.8;
	defaultAP.PassHiFilter = 0.75;
	defaultAP.MatchMode = AlignPair::Param::MMRigid;
	QMessageBox::warning(0, "Align tool", "ICP Default Parameters set for MILLIMETERS");
}

void EditAlignPlugin::setAlignParamM()
{
	defaultAP.SampleNum = 2000;
	defaultAP.MinDistAbs = 0.3;
	defaultAP.TrgDistAbs = 0.0005;
	defaultAP.MaxIterNum = 75;
	defaultAP.ReduceFactorPerc = 0.8;
	defaultAP.PassHiFilter = 0.75;
	defaultAP.MatchMode = AlignPair::Param::MMRigid;
	QMessageBox::warning(0, "Align tool", "ICP Default Parameters set for METERS");
}

void EditAlignPlugin::glueHere()
{
    MeshNode *mn=currentNode();
    if(mn->glued)
      meshTree.deleteResult(mn);

    mn->glued = !mn->glued;
    alignDialog->rebuildTree();
}

void EditAlignPlugin::glueHereVisible()
{
  for(auto ni=meshTree.nodeMap.begin();ni!=meshTree.nodeMap.end();++ni)
//    foreach(MeshNode *mn, meshTree.nodeList)
      if(ni->second->m->visible) ni->second->glued=true;

    alignDialog->rebuildTree();
}

void EditAlignPlugin::selectBadArc()
{
  float maxErr=0;
  AlignPair::Result *worseArc=0;
  for(QList<vcg::AlignPair::Result>::iterator li=meshTree.resultList.begin();li!=meshTree.resultList.end();++li)
  {
    if(li->err > maxErr)
    {
      maxErr=li->err;
      worseArc=&*li;
    }
  }
  if(worseArc)
    alignDialog->setCurrentArc(worseArc);
}


void EditAlignPlugin::process()
{
    if(meshTree.gluedNum()<2)
    {
        QMessageBox::warning(0,"Align tool", "ICP Process can only work when at least two layers have been glued");
        return;
    }
    alignDialog->setEnabled(false);
    meshTree.Process(defaultAP, defaultMTP);
    alignDialog->rebuildTree();
    _gla->update();
    alignDialog->setEnabled(true);
}

void EditAlignPlugin::recalcCurrentArc()
{
    assert(currentArc());

    alignDialog->setEnabled(false);
    meshTree.ProcessArc(currentArc()->FixName,currentArc()->MovName,*currentArc(),currentArc()->ap);
    meshTree.ProcessGlobal(currentArc()->ap);
    AlignPair::Result *recomputedArc = currentArc();
    alignDialog->rebuildTree();
    alignDialog->setCurrentArc(recomputedArc);
    alignDialog->setEnabled(true);
    _gla->update();
}


void EditAlignPlugin::mousePressEvent(QMouseEvent *e, MeshModel &, GLArea * )
{
    if(mode==ALIGN_MOVE)
    {
        trackball.MouseDown(e->x(),_gla->height()-e->y(), QT2VCG(e->button(), e->modifiers() ) );
        _gla->update();
    }
}

void EditAlignPlugin::mouseMoveEvent(QMouseEvent *e, MeshModel &, GLArea * )
{
    if(mode==ALIGN_MOVE)
    {
        trackball.MouseMove(e->x(),_gla->height()-e->y() );
        _gla->update();
    }

}

void EditAlignPlugin::mouseReleaseEvent(QMouseEvent * e, MeshModel &/*m*/, GLArea * )
{
    if(mode==ALIGN_MOVE)
    {
        trackball.MouseUp(e->x(),_gla->height()-e->y(), QT2VCG(e->button(), e->modifiers() ) );
        _gla->update();
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
		alignDialog->ui.glueHereButton->setEnabled(false);
		alignDialog->ui.glueHereAllButton->setEnabled(false);
		alignDialog->ui.pointBasedAlignButton->setEnabled(false);
		alignDialog->ui.baseMeshButton->setEnabled(false);
		alignDialog->ui.hideRevealButton->setEnabled(false);
		alignDialog->ui.icpButton->setEnabled(false);
		alignDialog->ui.icpParamButton->setEnabled(false);
		alignDialog->ui.icpParamDefMMButton->setEnabled(false);
		alignDialog->ui.icpParamDefMButton->setEnabled(false);
		alignDialog->ui.meshTreeParamButton->setEnabled(false);
		alignDialog->ui.badArcButton->setEnabled(false);
		alignDialog->ui.icpParamCurrentButton->setEnabled(false);
		alignDialog->ui.recalcButton->setEnabled(false);
        alignDialog->ui.alignTreeWidget->setEnabled(false);

        break;
    case	ALIGN_IDLE:
        alignDialog->ui.manualAlignButton->setEnabled(true);
		alignDialog->ui.glueHereButton->setEnabled(true);
		alignDialog->ui.glueHereAllButton->setEnabled(true);
		alignDialog->ui.pointBasedAlignButton->setEnabled(true);
		alignDialog->ui.baseMeshButton->setEnabled(true);
		alignDialog->ui.hideRevealButton->setEnabled(true);
		alignDialog->ui.icpButton->setEnabled(true);
		alignDialog->ui.icpParamButton->setEnabled(true);
		alignDialog->ui.icpParamDefMMButton->setEnabled(true);
		alignDialog->ui.icpParamDefMButton->setEnabled(true);
		alignDialog->ui.meshTreeParamButton->setEnabled(true);
		alignDialog->ui.badArcButton->setEnabled(true);
		alignDialog->ui.icpParamCurrentButton->setEnabled(true);
		alignDialog->ui.recalcButton->setEnabled(true);
		alignDialog->ui.alignTreeWidget->setEnabled(true);
		alignDialog->updateButtons();
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



