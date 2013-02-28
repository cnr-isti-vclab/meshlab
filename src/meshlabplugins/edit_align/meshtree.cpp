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

#include <QObject>
#include <QStringList>
#include <QList>

#include "meshtree.h"
#include "align/AlignGlobal.h"
using namespace vcg;

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

//// Assign to each mesh (glued and not glued) an unique id
//void MeshTree::resetID()
//{
//	int cnt=0;
//	 foreach(MeshNode *mn, nodeList)
//		{
//			mn->id= cnt;
//			cnt++;
//		}
//}

void MeshTree::ProcessArc(int fixId, int movId, vcg::AlignPair::Result &result, vcg::AlignPair::Param ap)
{
    // l'allineatore globale cambia le varie matrici di posizione di base delle mesh
    // per questo motivo si aspetta i punti nel sistema di riferimento locale della mesh fix
    // Si fanno tutti i conti rispetto al sistema di riferimento locale della mesh fix
    vcg::Matrix44d FixM=vcg::Matrix44d::Construct(find(fixId)->tr());
    vcg::Matrix44d MovM=vcg::Matrix44d::Construct(find(movId)->tr());
    vcg::Matrix44d MovToFix = Inverse(FixM) * MovM;

    ProcessArc(fixId,movId,MovToFix,result,ap);
}
/** This is the main alignment function.
  It takes a pair of mesh and aling the second on the first one according to the parameters stored in <ap>

  */
void MeshTree::ProcessArc(int fixId, int movId, vcg::Matrix44d &MovM, vcg::AlignPair::Result &result, vcg::AlignPair::Param ap)
{
  vcg::AlignPair::A2Mesh Fix;
  vcg::AlignPair aa;

  // 1) Convert fixed mesh and put it into the grid.
  MM(fixId)->updateDataMask(MeshModel::MM_FACEMARK);
  aa.ConvertMesh<CMeshO>(MM(fixId)->cm,Fix);

  vcg::AlignPair::A2Grid UG;
  vcg::AlignPair::A2GridVert VG;

  if(MM(fixId)->cm.fn==0 || ap.UseVertexOnly)
  {
    Fix.InitVert(vcg::Matrix44d::Identity());
    vcg::AlignPair::InitFixVert(&Fix,ap,VG);
  }
  else
  {
    Fix.Init(vcg::Matrix44d::Identity());
    vcg::AlignPair::InitFix(&Fix, ap, UG);
  }
  // 2) Convert the second mesh and sample a <ap.SampleNum> points on it.

  MM(movId)->updateDataMask(MeshModel::MM_FACEMARK);
//  aa.ConvertMesh<CMeshO>(MM(movId)->cm,Mov);
  std::vector<vcg::AlignPair::A2Vertex> tmpmv;
//  aa.ConvertVertex(Mov.vert,tmpmv);
  aa.ConvertVertex(MM(movId)->cm.vert,tmpmv);
  aa.SampleMovVert(tmpmv, ap.SampleNum, ap.SampleMode);

  aa.mov=&tmpmv;
  aa.fix=&Fix;
  aa.ap = ap;

  vcg::Matrix44d In=MovM;
  // Perform the ICP algorithm
  aa.Align(In,UG,VG,result);

  result.FixName=fixId;
  result.MovName=movId;
  result.as.Dump(stdout);
}

void MeshTree::Process(vcg::AlignPair::Param &ap, MeshTree::Param &mtp)
{
  QString buf;
  cb(0,qPrintable(buf.sprintf("Starting Processing of %i glued meshes out of %i meshes\n",gluedNum(),nodeList.size())));

  /******* Occupancy Grid Computation *************/
  cb(0,qPrintable(buf.sprintf("Computing Overlaps %i glued meshes...\n",gluedNum() )));
  OG.Init(nodeList.size(), vcg::Box3d::Construct(gluedBBox()), mtp.OGSize);
  foreach(MeshNode *mn, nodeList)
    if(mn->glued)
      OG.AddMesh<CMeshO>(mn->m->cm, vcg::Matrix44d::Construct(mn->tr()), mn->Id());

  OG.Compute();
  OG.Dump(stdout);
  // Note: the s and t of the OG translate into fix and mov, respectively.

  /*************** The long loop of arc computing **************/

  int arcNum=0; // count arc within current thrsh
  while(arcNum<OG.SVA.size() && OG.SVA[arcNum].norm_area > mtp.arcThreshold)
    ++arcNum;

  cb(0,qPrintable(buf.sprintf("Using %i possible Arcs on %lu :\n",arcNum,OG.SVA.size())));
  for(size_t i=0;i<OG.SVA.size() && OG.SVA[i].norm_area > mtp.arcThreshold; ++i)
  {
    fprintf(stdout,"%4i -> %4i Area:%5i NormArea:%5.3f\n",OG.SVA[i].s,OG.SVA[i].t,OG.SVA[i].area,OG.SVA[i].norm_area);
    AlignPair::Result *curResult=0;
    curResult=findResult(OG.SVA[i].s,OG.SVA[i].t);
    if(curResult==0) {
      resultList.push_back(AlignPair::Result());
      curResult= &resultList.back();
      ProcessArc(OG.SVA[i].s, OG.SVA[i].t, *curResult, ap);
      curResult->area= OG.SVA[i].norm_area;
      if( curResult->IsValid() )
      {
        std::pair<double,double> dd=curResult->ComputeAvgErr();
        cb(0,qPrintable(buf.sprintf("(%3i/%3i) %2i -> %2i Aligned AvgErr dd=%f -> dd=%f \n",int(i+1),arcNum,OG.SVA[i].s,OG.SVA[i].t,dd.first,dd.second)));
      }
      else
      {
        cb(0,qPrintable(buf.sprintf( "(%3i/%3i) %2i -> %2i Failed Alignment of one arc %s\n",int(i+1),arcNum,OG.SVA[i].s,OG.SVA[i].t,vcg::AlignPair::ErrorMsg(curResult->status))));
        resultList.pop_back();
      }
    }
  }

  cb(0,qPrintable(buf.sprintf("Completed Mesh-Mesh Alignment\n")));

  //if there are no arcs at all complain and return
  if(arcNum==0) {
    cb(0,qPrintable(buf.sprintf("\n Failure. There are no overlapping meshes?\n No candidate alignment arcs. Nothing Done.\n")));
    return;
  }

  //if there are no valid arcs complain and return
  if(resultList.empty()) {
    cb(0,qPrintable(buf.sprintf("\n Failure. No succesful arc among candidate Alignment arcs. Nothing Done.\n")));
    return;
  }

  ProcessGlobal(ap);
}

void MeshTree::ProcessGlobal(vcg::AlignPair::Param &ap)
{
	QString buf;
	/************** Preparing Matrices for global alignment *************/
	cb(0,qPrintable(buf.sprintf("Starting Global Alignment\n")));

	vcg::Matrix44d Zero44; Zero44.SetZero();
	std::vector<vcg::Matrix44d> PaddedTrVec(nodeList.size(),Zero44);
	// matrix trv[i] is relative to mesh with id IdVec[i]
	// if all the mesh are glued GluedIdVec=={1,2,3,4...}
	std::vector<int> GluedIdVec;
	std::vector<vcg::Matrix44d> GluedTrVec;

	std::map<int,std::string> names;

	foreach(MeshNode *mn, nodeList)
	{
		if(mn->glued)
		{
			GluedIdVec.push_back(mn->Id());
			GluedTrVec.push_back(vcg::Matrix44d::Construct(mn->tr()));
			PaddedTrVec[mn->Id()]=GluedTrVec.back();
			names[mn->Id()]=qPrintable(mn->m->label());
		}
	}

	vcg::AlignGlobal AG;
	std::vector<vcg::AlignPair::Result *> ResVecPtr;
	for(QList<vcg::AlignPair::Result>::iterator li=resultList.begin();li!=resultList.end();++li)
	  ResVecPtr.push_back(&*li);
	AG.BuildGraph(ResVecPtr, GluedTrVec, GluedIdVec);

	float StartGlobErr = 0.001f;
	while(!AG.GlobalAlign(names, 	StartGlobErr, 100, true, stdout)){
		StartGlobErr*=2;
		AG.BuildGraph(ResVecPtr,GluedTrVec, GluedIdVec);
	}

	std::vector<vcg::Matrix44d> GluedTrVecOut(GluedTrVec.size());
	AG.GetMatrixVector(GluedTrVecOut,GluedIdVec);

	//Now get back the results!
	for(int ii=0;ii<GluedTrVecOut.size();++ii)
		MM(GluedIdVec[ii])->cm.Tr.Import(GluedTrVecOut[ii]);

	cb(0,qPrintable(buf.sprintf("Completed Global Alignment (error bound %6.4f)\n",StartGlobErr)));

}

