
/****************************************************************************
**
** Copyright (C) 2005-2005 Trolltech AS. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
** information about Qt Commercial License Agreements.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QObject>
#include <QStringList>
#include <QList>

#include "meshtree.h"
#include "align/AlignGlobal.h"

MeshTree::MeshTree()
{
	cb=vcg::DummyCallBackPos;
}

int MeshTree::gluedNum()
{
	int cnt=0;
	 foreach(MeshNode *mn, nodeList) 
		if(mn->glued) ++cnt;
	 return cnt;
}

void MeshTree::Process(AlignPair::Param ap)
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
  OG.Init(nodeList.size(), Box3d::Construct(gluedBBox()), 10000);

  vector<Matrix44d> trv(nodeList.size());
  vector<string> names(nodeList.size());
	int id=0;

  foreach(MeshNode *mn, nodeList) 
		if(mn->glued)
			{
				MeshModel *mm=mn->m;
				trv[id]=Matrix44d::Construct(mm->cm.Tr);
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
  QString buf;
	cb(0,qPrintable(buf.sprintf("Computed %i Arcs :\n",OG.SVA.size())));
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
	MM(ii)->cm.Tr.Import(trout[ii]);	
}

