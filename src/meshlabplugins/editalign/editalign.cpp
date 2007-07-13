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
//#include <meshlab/mainwindow.h>
#include "editalign.h"
#include <wrap/gl/pick.h>
#include <wrap/qt/trackball.h>
//#include "align/AlignPair.h" 
#include "align/AlignGlobal.h"

using namespace vcg;

EditAlignPlugin::EditAlignPlugin() {
  alignDialog=0;
  curFacePtr=0;
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

	return tr("Return detailed info about a picked face of the model.");
}

const PluginInfo &EditAlignPlugin::Info() 
{
   static PluginInfo ai; 
   ai.Date=tr(__DATE__);
	 ai.Version = tr("1.0");
	 ai.Author = ("Paolo Cignoni");
   return ai;
} 

void EditAlignPlugin::Pick(MeshModel &m, GLArea * gla)
{
	assert(haveToPick);
	vector<CMeshO::FacePointer> NewSel;  
	GLPickTri<CMeshO>::PickFace(cur.x(), gla->height() - cur.y(), m.cm, NewSel);
	if(NewSel.size()>0)
			curFacePtr=NewSel.front();
	haveToPick=false;
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
			if(haveToPick) Pick(m,gla);
			
			if(curFacePtr)
			{
				glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
				glDisable(GL_DEPTH_TEST); 
				glDisable(GL_LIGHTING);
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
	}
}

void EditAlignPlugin::StartEdit(QAction * /*mode*/, MeshModel &_mm, GLArea *_gla )
{
	md=&_gla->meshDoc;
	gla=_gla;
	//MainWindow *mainW=qobject_cast<MainWindow *> (gla->parentWidget()->parentWidget());
	//assert(mainW);
	gla->setCursor(QCursor(QPixmap(":/images/cur_align.png"),1,1));	
	if(alignDialog==0)
	{
		//alignDialog=new AlignDialog(gla->parentWidget()->parentWidget());
		alignDialog=new AlignDialog(gla->window());
		connect(alignDialog->ui.icpButton,SIGNAL(clicked()),this,SLOT(Process()));
		connect(alignDialog->ui.manualAlignButton,SIGNAL(clicked()),this,SLOT(GlueManual()));
	}
	alignDialog->edit=this;
	alignDialog->updateTree();
	alignDialog->show();
	//alignDialog->adjustSize();
	
	//mainW->addDockWidget(Qt::LeftDockWidgetArea,alignDialog);
	mode=ALIGN_IDLE;
	
		int id=0;

  foreach(MeshModel *mm, md->meshList)
	{
		mm->cm.C()=Color4b::Scatter(100, id,.2f,.7f);
		++id;
	}
	gla->rm.colorMode=GLW::CMPerMesh;

}

  
void EditAlignPlugin::GlueManual()
{
	MeshModel *mm=md->mm();
static QString oldLabelButton;
  if(mode == ALIGN_IDLE)
	{
		mode = ALIGN_MOVE;
		md->mm()->visible=false;
		trackball.Reset();
		trackball.center=mm->Tr*mm->cm.bbox.Center();
		trackball.radius= mm->cm.bbox.Diag()/2.0;
		updateButtons();
		oldLabelButton=	alignDialog->ui.manualAlignButton->text();
		alignDialog->ui.manualAlignButton->setText("Store transformation");
  }
	else if(mode == ALIGN_MOVE)  // stop manual alignment and freeze the mesh
	{
		mode = ALIGN_IDLE;
		updateButtons();
		Matrix44f tran,mtran; 
		tran.SetTranslate(trackball.center);
		mtran.SetTranslate(-trackball.center);
		mm->Tr= (tran) * trackball.track.Matrix()*(mtran) * mm->Tr;
		mm->visible=true;
		alignDialog->ui.manualAlignButton->setText(oldLabelButton);
	
	}
	gla->update();
}

void EditAlignPlugin::GlueHere()
{

}

void EditAlignPlugin::Process()
{


//	ap.MaxIterNum=100;
//	ap.MinDistAbs = aln.MinDist;
//	ap.TrgDistAbs = aln.TrgDist;;
//	ap.SampleNum = aln.SampleNum;
//    ap.SampleMode = AlignPair::Param::SMRandom;
//	ap.MaxPointNum = aln.MaxPointNum;
//	ap.MinPointNum = aln.MinPointNum;
//    ap.MaxAngleRad=math::ToRad(aln.MaxAngleDeg);
//	if(aln.RigidFlag) ap.MatchMode = AlignPair::Param::MMRigid;
//				   else ap.MatchMode = AlignPair::Param::MMFast;
//	ap.ReduceFactor = aln.ReduceFactor;
//    ap.PassLoFilter=0;
//	ap.EndStepNum=aln.EndStepNum;

  //Minimo esempio di codice per l'uso della funzione di allineamento.
	//OccupancyGrid OG;
  OG.Init(md->meshList.size(), Box3d::Construct(md->bbox()), 10000);

  vector<Matrix44d> trv(md->meshList.size());
  vector<string> names(md->meshList.size());
	int id=0;

  foreach(MeshModel *mm, md->meshList)
	{
		trv[id]=Matrix44d::Construct(mm->Tr);
		names[id]=mm->fileName;
		OG.AddMesh<CMeshO>(mm->cm,trv[id],id);
		++id;
	}
	
  OG.Compute();
  OG.Dump(stdout);

  
	ResVec.clear();
	ResVec.resize(OG.SVA.size());
	ResVecPtr.clear();
	AlignPair::A2Mesh Fix;
	AlignPair::A2Mesh Mov;

	fprintf(stdout,"Computed %i Arcs :\n",OG.SVA.size());
	int i=0;
	for(i=0;i<OG.SVA.size() && OG.SVA[i].norm_area > .1; ++i)
  {	
    fprintf(stdout,"%4i -> %4i Area:%5i NormArea:%5.3f\n",OG.SVA[i].s,OG.SVA[i].t,OG.SVA[i].area,OG.SVA[i].norm_area);

    Matrix44d Ident; Ident.SetIdentity();

    // l'allineatore globale cambia le varie matrici di posizione di base delle mesh
    // per questo motivo si aspetta i punti nel sistema di riferimento locale della mesh fix
    // Si fanno tutti i conti rispetto al sistema di riferimento locale della mesh fix
    Matrix44d FixM=trv[OG.SVA[i].s];
    Matrix44d InvFixM=Inverse(FixM);
		
		Matrix44d MovM=InvFixM * trv[OG.SVA[i].t];
    Matrix44d InvMovM=Inverse(MovM);

	  AlignPair aa;

	  aa.ConvertMesh<CMeshO>(MM(OG.SVA[i].s)->cm,Fix);
		Fix.Init(Matrix44d::Identity(),false);
	  aa.ConvertMesh<CMeshO>(MM(OG.SVA[i].t)->cm,Mov);
  
    printf("Loaded Fix Mesh %32s vn:%8i fn:%8i\n",MM(OG.SVA[i].s)->fileName.c_str(),Fix.vn,Fix.fn);
	  printf("Loaded Mov Mesh %32s vn:%8i fn:%8i\n",MM(OG.SVA[i].t)->fileName.c_str(),Mov.vn,Mov.fn);
  

    AlignPair::A2Grid UG;
	  AlignPair::InitFix(&Fix, ap, UG);
	  //
	  vector<AlignPair::A2Vertex> tmpmv;     

	  aa.ConvertVertex(Mov.vert,tmpmv);
	  aa.SampleMovVert(tmpmv, ap.SampleNum, ap.SampleMode);
   
	  aa.mov=&tmpmv;
	  aa.fix=&Fix;
    aa.ap = ap;
	  Matrix44d In;
	  In.SetIdentity();
    In=MovM;
    printf("Starting Alingment with an initial matrix :\n");
    //AlignWrapper::FileMatrixWrite(stdout,In);
    aa.Align(In,UG,ResVec[i]);
    ResVec[i].FixName=OG.SVA[i].s;
    ResVec[i].MovName=OG.SVA[i].t;
	  ResVec[i].as.Dump(stdout); 
    ResVecPtr.push_back(&ResVec[i]);
    std::pair<double,double> dd=ResVec[i].ComputeAvgErr();
    printf("Avg dd=%f -> dd=%f \n",dd.first,dd.second);
}

  vector<int> IdVec;
  for(int i=0;i<trv.size();++i) 
    IdVec.push_back(i);
  
  AlignGlobal AG;
  
  AG.BuildGraph(ResVecPtr,trv,IdVec);
  
 
 int maxiter = 1000;
 float StartGlobErr = 0.001f;
 while(!AG.GlobalAlign(names, 	StartGlobErr, 100, true, stdout)){
	StartGlobErr*=2;
		AG.BuildGraph(ResVecPtr,trv,IdVec);
	}

 vector<Matrix44d> trout(trv.size());
 AG.GetMatrixVector(trout,IdVec);

//Now get back the results!
for(int ii=0;ii<trout.size();++ii)
	MM(ii)->Tr.Import(trout[ii]);
	
	alignDialog->updateTree();
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
	if(mode==ALIGN_PICK) 
	{
		gla->update();
		cur=e->pos();
		haveToPick=true;
	}
}

void EditAlignPlugin::updateButtons()
{
switch(mode)
	{
	case	ALIGN_MOVE:
			alignDialog->ui.manualAlignButton->setEnabled(true);
			alignDialog->ui.inspectButton->setEnabled(false);
			alignDialog->ui.icpButton->setEnabled(false);
			alignDialog->ui.icpParamButton->setEnabled(false);
			alignDialog->ui.exportButton->setEnabled(false);
			alignDialog->ui.alignTreeWidget->setEnabled(false);
			
		break;
	case	ALIGN_IDLE:
			alignDialog->ui.manualAlignButton->setEnabled(true);
			alignDialog->ui.inspectButton->setEnabled(true);
			alignDialog->ui.icpButton->setEnabled(true);
			alignDialog->ui.icpParamButton->setEnabled(true);
			alignDialog->ui.exportButton->setEnabled(true);
			alignDialog->ui.alignTreeWidget->setEnabled(true);
			break;
	}
}


Q_EXPORT_PLUGIN(EditAlignPlugin)
