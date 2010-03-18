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
#include <time.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/append.h>
#include <vcg/complex/trimesh/autoalign_4pcs.h>

#include "filter_autoalign.h"
#include "../../meshlabplugins/editalign/align/Guess.h"

using namespace vcg;
using namespace std;
// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

FilterAutoalign::FilterAutoalign() 
{ 
	typeList << FP_AUTOALIGN 
					 //<< FP_BEST_ROTATION
					 ;
  
  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
QString FilterAutoalign::filterName(FilterIDType filterId) const
{
  switch(filterId) {
		case FP_AUTOALIGN :  return QString("Automatic pair Alignement"); 
		case FP_BEST_ROTATION :  return QString("Automatic Alignement (Brute)"); 
		default : assert(0); 
	}
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
QString FilterAutoalign::filterInfo(FilterIDType filterId) const
{
  switch(filterId) {
		case FP_AUTOALIGN     :  return QString("Automatic Rough Alignment of two meshes. Based on the paper <b> 4-Points Congruent Sets for Robust Pairwise Surface Registration</b>, by Aiger,Mitra, Cohen-Or. Siggraph 2008  "); 
		case FP_BEST_ROTATION :  return QString("Automatic Rough Alignment of two meshes. Brute Force Approach"); 
		default : assert(0); 
	}
}

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parmeter you need to define, 
// - the name of the parameter, 
// - the string shown in the dialog 
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void FilterAutoalign::initParameterSet(QAction *action,MeshDocument & md/*m*/, RichParameterSet & parlst) 
{
	 MeshModel *target;
	 switch(ID(action))	 {
		case FP_AUTOALIGN :  
 				target= md.mm();
				foreach (target, md.meshList) 
						if (target != md.mm())  break;
		    
				parlst.addParam(new RichMesh ("FirstMesh", md.mm(),&md, "First Mesh",
												"The mesh were the coplanar bases are sampled (it will contain the trasformation)"));
				parlst.addParam(new RichMesh ("SecondMesh", target,&md, "Second Mesh",
												"The mesh were similar coplanar based are searched."));
				parlst.addParam(new RichFloat("overlapping",0.5f,"Estimated fraction of the\n first mesh overlapped by the second"));
				parlst.addParam(new RichFloat("tolerance [0.0,1.0]",0.3f,"Error tolerance"));
		 break;		
			 case FP_BEST_ROTATION :  
			 target= md.mm();
			 foreach (target, md.meshList) 
			 if (target != md.mm())  break;
			 
			 parlst.addParam(new RichMesh ("FirstMesh", md.mm(),&md, "First Mesh",
											 "The mesh that will be moved"));
			 parlst.addParam(new RichMesh ("SecondMesh", target,&md, "Second Mesh",
											 "The mesh that will be kept fixed."));
			 parlst.addParam(new RichInt("GridSize",10,"Grid Size", "The size of the uniform grid that is used for searching the best translation for a given rotation"));
			 parlst.addParam(new RichInt("Rotation Num",64,"RotationNumber", "sss"));
			 break;		
			 
   default: break; // do not add any parameter for the other filters
  }
}

// The Real Core Function doing the actual mesh processing.
bool FilterAutoalign::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb)
{
	vcg::tri::FourPCS<CMeshO> *fpcs ;
	switch(ID(filter)) {
		case FP_AUTOALIGN :
		{
			
			MeshModel *firstMesh= par.getMesh("FirstMesh");
			MeshModel *secondMesh= par.getMesh("SecondMesh");
			fpcs = new vcg::tri::FourPCS<CMeshO>();
			fpcs->prs.Default();
			fpcs->prs.f =  par.getFloat("overlapping");
			firstMesh->updateDataMask(MeshModel::MM_VERTMARK);
			secondMesh->updateDataMask(MeshModel::MM_VERTMARK);
			fpcs->Init(firstMesh->cm,secondMesh->cm);
			bool res = fpcs->Align(0,firstMesh->cm.Tr,cb);
			firstMesh->clearDataMask(MeshModel::MM_VERTMARK);
			secondMesh->clearDataMask(MeshModel::MM_VERTMARK);

			// Log function dump textual info in the lower part of the MeshLab screen. 
			Log(GLLogStream::FILTER,(res)?" Automatic Rough Alignment Done":"Automatic Rough Alignment Failed");
				delete fpcs;
		} break;
		case FP_BEST_ROTATION :
		{	
			MeshModel *firstMesh= par.getMesh("FirstMesh");
			MeshModel *secondMesh= par.getMesh("SecondMesh");
			tri::Guess GG;
			GG.pp.MatrixNum = 100;
			GG.GenRotMatrix();
			GG.Init<CMeshO>(firstMesh->cm, secondMesh->cm);
		  static int counterMV=0;
			static int step=0;
			//for(int i=0;i<GG.MV.size();++i)
			int i=counterMV;
			step = (step +1)%4;
//			if(step==0) counterMV++;
			
			qDebug("Counter %i step %i",counterMV,step);
				{
						Point3f baseTran =  GG.InitBaseTranslation(GG.MV[i]);		
						Point3f bestTran;
																
						int res = GG.SearchBestTranslation(GG.u[0],GG.MV[i],4,1,baseTran,bestTran);
						if(step==0) firstMesh->cm.Tr.SetIdentity();
						if(step==1) firstMesh->cm.Tr = GG.MV[i];
						if(step==2) firstMesh->cm.Tr = GG.BuildTransformation(GG.MV[i],baseTran);												
						if(step==3) firstMesh->cm.Tr = GG.BuildTransformation(GG.MV[i],bestTran);																									
				}			
			//Log(0,(res)?" Automatic Rough Alignment Done":"Automatic Rough Alignment Failed");			
		} break;
		default: assert (0);
	}
	return true;
}

FilterAutoalign::FilterClass FilterAutoalign::getClass(QAction *a)
{
  switch(ID(a))
  {
    case FP_AUTOALIGN :
		case FP_BEST_ROTATION :	
      return FilterClass(MeshFilterInterface::Layer+MeshFilterInterface::RangeMap);     
    default : 
			return MeshFilterInterface::Generic;
  }
}

Q_EXPORT_PLUGIN(FilterAutoalign)
