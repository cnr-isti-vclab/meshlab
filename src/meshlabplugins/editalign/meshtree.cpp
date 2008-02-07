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

// Assign to each mesh (glued and not glued) an unique id
void MeshTree::resetID()
{
	int cnt=0;
	 foreach(MeshNode *mn, nodeList) 
		{
			mn->id= cnt;
			cnt++;
		}
}

void MeshTree::Process(AlignPair::Param ap)
{
	resetID(); 	// Assign to each mesh (glued and not glued) an unique id
	OG.Init(nodeList.size(), Box3d::Construct(gluedBBox()), 10000);
  
	Matrix44d Zero44; Zero44.SetZero();
  vector<Matrix44d> PaddedTrVec(nodeList.size(),Zero44);
	// matrix trv[i] is relative to mesh with id IdVec[i]
	// if all the mesh are glued GluedIdVec=={1,2,3,4...}
	vector<int> GluedIdVec;
	vector<Matrix44d> GluedTrVec;
		
  vector<string> names(nodeList.size());
		  
  foreach(MeshNode *mn, nodeList) 
	{
		if(mn->glued)
			{
				MeshModel *mm=mn->m;
				GluedIdVec.push_back(mn->id);							
				GluedTrVec.push_back(Matrix44d::Construct(mm->cm.Tr));
				OG.AddMesh<CMeshO>(mm->cm, GluedTrVec.back(), GluedIdVec.back());
				
				PaddedTrVec[mn->id]=GluedTrVec.back();
				names[mn->id]=mm->fileName;
				printf("Added to the OG the mesh with id %i at the position %i\n",mn->id,GluedIdVec.size()-1);
			}
			else printf("Mesh with id %i is not glued\n",mn->id);
	//	++id;
  }		
	
	QString buf;
	cb(0,qPrintable(buf.sprintf("Starting Processing of %i glued meshes out of %i meshes\n",gluedNum(),nodeList.size())));
  
  OG.Compute();
  OG.Dump(stdout);

  
	ResVec.clear();
	ResVec.resize(OG.SVA.size());
	ResVecPtr.clear();
	AlignPair::A2Mesh Fix;
	AlignPair::A2Mesh Mov;

	cb(0,qPrintable(buf.sprintf("Computed %i Arcs :\n",OG.SVA.size())));
	int i=0;
	for(i=0;i<OG.SVA.size() && OG.SVA[i].norm_area > .1; ++i)
  {	
    fprintf(stdout,"%4i -> %4i Area:%5i NormArea:%5.3f\n",OG.SVA[i].s,OG.SVA[i].t,OG.SVA[i].area,OG.SVA[i].norm_area);

    Matrix44d Ident; Ident.SetIdentity();

    // l'allineatore globale cambia le varie matrici di posizione di base delle mesh
    // per questo motivo si aspetta i punti nel sistema di riferimento locale della mesh fix
    // Si fanno tutti i conti rispetto al sistema di riferimento locale della mesh fix
    Matrix44d FixM=PaddedTrVec[OG.SVA[i].s];
    Matrix44d InvFixM=Inverse(FixM);
		
		Matrix44d MovM=InvFixM * PaddedTrVec[OG.SVA[i].t];
    Matrix44d InvMovM=Inverse(MovM);

	  AlignPair aa;

	  aa.ConvertMesh<CMeshO>(MM(OG.SVA[i].s)->cm,Fix);
		Fix.Init(Matrix44d::Identity(),false);
	  aa.ConvertMesh<CMeshO>(MM(OG.SVA[i].t)->cm,Mov);
  
    cb(0,qPrintable(buf.sprintf("Loaded Fix Mesh %32s vn:%8i fn:%8i\n",qPrintable(QFileInfo(MM(OG.SVA[i].s)->fileName.c_str()).fileName()),Fix.vn,Fix.fn)));
	  cb(0,qPrintable(buf.sprintf("Loaded Mov Mesh %32s vn:%8i fn:%8i\n",qPrintable(QFileInfo(MM(OG.SVA[i].t)->fileName.c_str()).fileName()),Mov.vn,Mov.fn)));
  

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
    aa.Align(In,UG,ResVec[i]);
    ResVec[i].FixName=OG.SVA[i].s;
    ResVec[i].MovName=OG.SVA[i].t;
	  ResVec[i].as.Dump(stdout);
		ResVec[i].area= OG.SVA[i].norm_area;
    if( ResVec[i].IsValid() )
			ResVecPtr.push_back(&ResVec[i]);
    std::pair<double,double> dd=ResVec[i].ComputeAvgErr();
    if( ResVec[i].IsValid() ) 
			cb(0,qPrintable(buf.sprintf(" %2i -> %2i Aligned AvgErr dd=%f -> dd=%f \n",OG.SVA[i].s,OG.SVA[i].t,dd.first,dd.second)));
		else 
			cb(0,qPrintable(buf.sprintf(" %2i -> %2i Failed Alignment\n",OG.SVA[i].s,OG.SVA[i].t)));
		
}
  // now cut the ResVec vector to the only computed result (the arcs with area > .1)
	ResVec.resize(i);
  
	cb(0,qPrintable(buf.sprintf("Completed Mesh-Mesh Alignment\n")));
	cb(0,qPrintable(buf.sprintf("Starting Global Alignment\n")));
		
	AlignGlobal AG;
  
  AG.BuildGraph(ResVecPtr, GluedTrVec, GluedIdVec);
   
 int maxiter = 1000;
 float StartGlobErr = 0.001f;
 while(!AG.GlobalAlign(names, 	StartGlobErr, 100, true, stdout)){
  	StartGlobErr*=2;
		AG.BuildGraph(ResVecPtr,GluedTrVec, GluedIdVec);
	}

 vector<Matrix44d> GluedTrVecOut(GluedTrVec.size());
 AG.GetMatrixVector(GluedTrVecOut,GluedIdVec);

//Now get back the results!
for(int ii=0;ii<GluedTrVecOut.size();++ii)
	MM(GluedIdVec[ii])->cm.Tr.Import(GluedTrVecOut[ii]);	

cb(0,qPrintable(buf.sprintf("Completed Global Alignment (error bound %6.4f\n",StartGlobErr)));

}

