/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

#include <wrap/minpack/minpack.h>
#include <vcg/complex/trimesh/closest.h>
#include "primitive.h"
#include <wrap/gl/math.h>


static int callEvaluateError(void *data,int m_dat,int n,const double* par,double* fvec,int iflag){
	return reinterpret_cast<Primitive *> (data)->EvaluateError(data,m_dat,n,par,fvec,iflag);
}
Primitive::Primitive(std::vector<vcg::Point2i> *_gesture2D,std::vector<vcg::Point3f>	*_gesture3D,CMeshO *_TotMesh):gesture2D(_gesture2D),gesture3D(_gesture3D),TotMesh(_TotMesh){
	parameters=NULL;
	grid=NULL;
	numParameters=0;
	orientation.SetIdentity();
	lastTotDistance=0;
	totDistance=0;
	showCloudNearPoints=false;
	showLines=false;
	showSelectionMesh=false;
	showCloudSampling=true;
	showInObjectSpace=false;
	showStepMode=false;
	indexOfstep=0;
}
/*
inline void Primitive::EvaluateError(double* par,int m_dat,double* fvec,void *data,NLMin<Primitive>::lm_status_type *status){
Sampling(par);
double tmp=0;
totDistance=0;
for(int i=0;i<m_dat;i++){
tmp=(double)vcg::Distance(cloudSampling[i],cloudNearPoints[i]);
//myfvec[i]=tmp;
//fvec[i]=tmp*weight[i];
fvec[i]=tmp;
totDistance+=tmp;
}
}
*/
int Primitive::EvaluateError(void *data,int m_dat,int n,const double* par,double* fvec,int iflag){
	Sampling(par);
	double tmp=0;
	totDistance=0;
	for(int i=0;i<m_dat;i++){
		tmp=(double)vcg::Distance(cloudSampling[i],cloudNearPoints[i]);
		fvec[i]=tmp;
		totDistance+=tmp;
	}
	return 0;
}

void Primitive::run(){
	InitSampling(parameters);											//Inizializza il vettore cloudSampling e cloudNearPoints
	InitDebug();
	double val=selectionMesh->bbox.Diag();
	while(val>selectionMesh->bbox.Diag()/100000){			//100000
		RigidTranformation();												//Modifico solo la posizione e l'orientamento
		NonLinearMinimization();										//Modifico i parametri della primitiva (es. raggio altezza)
		//UpdateWeight();
		val=abs((totDistance-lastTotDistance)/cloudSampling.size());
		lastTotDistance=totDistance;
	}
	emit updateGlArea();
	emit updateDebugStapModeSize((int)DebugCloudSampling.size()-1);
	exec();
}
void Primitive::RigidTranformation(){
	
	//Trovo i punti vicini dal sampling alla sottoMesh.
	for(int i=0;i<(int)cloudSampling.size();i++){
		float error=0;
		CVertexO *p=NULL;
		p=vcg::trimesh::GetClosestVertex<CMeshO,GridType>(*selectionMesh,*grid,cloudSampling[i],10000/*da modificare*/,error);
		if(p!=NULL){cloudNearPoints[i]=p->P();}
		else{cloudNearPoints[i]=vcg::Point3f(0,0,0);}
	}
	vcg::Matrix44f tmp;
	vcg::PointMatching<float>::ComputeRigidMatchMatrix(tmp,cloudSampling/*pfix*/,cloudNearPoints/*pmov*/);
	UpdateMesh_and_Grid_and_Near(tmp);
	orientation=orientation*vcg::Inverse(tmp);
	//Begin_Debug:
		DebugOrientationMatrix.push_back(orientation);
	//End_Debug:
}

void Primitive::UpdateMesh_and_Grid_and_Near(const vcg::Matrix44f &m){

	//Aggiorno la sotto Mesh:
	std::vector<vcg::Point3f> tmp;
	for(int i=0;i<(int)selectionMesh->vert.size();i++){
		selectionMesh->vert[i].P()=m*selectionMesh->vert[i].P();
		//Begin_Debug:
		tmp.push_back(selectionMesh->vert[i].P());
		//End_Debug:
	}
	//Aggiorno la cloudNearPoints:
	for(int i=0;i<(int)cloudNearPoints.size();i++){
		cloudNearPoints[i]=m*cloudNearPoints[i];
	}
	//Aggiorno la griglia:
	delete grid;
	grid= new GridType;
	grid->Set<CMeshO::VertexIterator>(selectionMesh->vert.begin(),selectionMesh->vert.end());
	/*Il sampling rimane fisso si muove solo la sottoMesh*/

	//Begin_Debug:
	DebugCloudNearPoints.push_back(cloudNearPoints);
	DebugCloudSampling.push_back(cloudSampling);
	DebugCloudMesh.push_back(tmp);
	//End_Debug:
}

/*
inline void Primitive::NonLinearMinimization(){
NLMin<Primitive> nlm;
nlm.InitControl();
nlm.control.maxcall  = 5;				//così è molto più veloce
nlm.control.gtol = 0.0001;			//così è molto più veloce
nlm.Fit(cloudSampling.size()/*codominio/,numParameters/*dominio/,parameters,this,&Primitive::EvaluateError,0);
}
*/

void Primitive::NonLinearMinimization(){
	LMDiff nlm;
	nlm.InitControl();
	nlm.control.maxcall  = 5;				//così è molto più veloce
	nlm.control.gtol = 0.0001;			//così è molto più veloce
	nlm.Run(cloudSampling.size()/*codominio*/,numParameters/*dominio*/,parameters,&callEvaluateError,this);
}

void Primitive::UpdateWeight(){
	for(int i=0;i<(int)weight.size();i++){
		weight[i]=1/pow((myfvec[i]+1),2.0);
	}
}

//Debug:
void Primitive::DrawDebug(){

	DrawDebugInWorldSpace();

	if(showInObjectSpace && !showStepMode){
		DrawDebugInObjectSpace();
	}
	if(showStepMode){
		DrawDebugInStapMode();
	}

}
void Primitive::DrawDebugInWorldSpace(){

	glPushAttrib(GL_DEPTH_BUFFER_BIT|GL_ENABLE_BIT|GL_LINE_BIT|GL_POINT_BIT|GL_CURRENT_BIT|GL_LIGHTING_BIT|GL_COLOR_BUFFER_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_BLEND);

	if(showCloudSampling && !showStepMode){
		//Disegno la cloudSampling
		glPushMatrix();
		vcg::glMultMatrix(orientation);										//<------TRASFORMAZIONI
		for(int i=0;i<(int)cloudSampling.size();i++){
			glColor3f(1,0,1);
			glPointSize(3);
			glBegin(GL_POINTS);
			glVertex(cloudSampling[i]);
			glEnd();
		}	
		glPopMatrix();
	}

	if(showCloudNearPoints && !showStepMode){
		//Disegno la cloudNearPoints
		glPushMatrix();
		vcg::glMultMatrix(orientation);										//<------TRASFORMAZIONI
		for(int i=0;i<(int)cloudNearPoints.size();i++){
			glColor3f(0,1,1);
			glPointSize(3);
			glBegin(GL_POINTS);
			glVertex(cloudNearPoints[i]);				
			glEnd();
		}
		glPopMatrix();
	}

	if(showLines && !showStepMode){
		//Disegno Le linee:
		glPushMatrix();
		vcg::glMultMatrix(orientation);										//<------TRASFORMAZIONI
		for(int i=0;i<(int)cloudNearPoints.size();i++){
			glPointSize(1);
			glBegin(GL_LINES);
			glColor3f(0,1,1);
			glVertex(cloudNearPoints[i]);
			glColor3f(1,0,1);
			glVertex(cloudSampling[i]);
			glEnd();
		}
		glPopMatrix();
	}	

	if(showSelectionMesh && !showStepMode){
		//Disegno la selectionMesh
		glPushMatrix();
		vcg::glMultMatrix(orientation);										//<------TRASFORMAZIONI
		for(int i=0;i<(int)selectionMesh->vert.size();i++){
			glColor3f(1,0.5,0.25);
			glPointSize(3);
			glBegin(GL_POINTS);
			glVertex(selectionMesh->vert[i].P());
			glEnd();
		}	
		glPopMatrix();
	}
	glPopAttrib();
}
void Primitive::DrawDebugInObjectSpace(){

	glPushAttrib(GL_DEPTH_BUFFER_BIT|GL_ENABLE_BIT|GL_LINE_BIT|GL_POINT_BIT|GL_CURRENT_BIT|GL_LIGHTING_BIT|GL_COLOR_BUFFER_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_BLEND);

	if(showCloudSampling){
		//Disegno la cloudSampling
		for(int i=0;i<(int)cloudSampling.size();i++){
			glColor3f(1,0,1);
			glPointSize(3);
			glBegin(GL_POINTS);
			glVertex(cloudSampling[i]);
			glEnd();
		}	
	}

	if(showCloudNearPoints){
		//Disegno la cloudNearPoints
		for(int i=0;i<(int)cloudNearPoints.size();i++){
			glColor3f(0,1,1);
			glPointSize(3);
			glBegin(GL_POINTS);
			glVertex(cloudNearPoints[i]);				
			glEnd();
		}
	}

	if(showLines){
		//Disegno Le linee:
		for(int i=0;i<(int)cloudNearPoints.size();i++){
			glPointSize(1);
			glBegin(GL_LINES);
			glColor3f(0,1,1);
			glVertex(cloudNearPoints[i]);
			glColor3f(1,0,1);
			glVertex(cloudSampling[i]);
			glEnd();
		}
	}	

	if(showSelectionMesh){
		//Disegno la selectionMesh
		for(int i=0;i<(int)selectionMesh->vert.size();i++){
			glColor3f(1,0.5,0.25);
			glPointSize(3);
			glBegin(GL_POINTS);
			glVertex(selectionMesh->vert[i].P());
			glEnd();
		}	
	}

	glPopAttrib();

}
void Primitive::DrawDebugInStapMode(){
	glPushAttrib(GL_DEPTH_BUFFER_BIT|GL_ENABLE_BIT|GL_LINE_BIT|GL_POINT_BIT|GL_CURRENT_BIT|GL_LIGHTING_BIT|GL_COLOR_BUFFER_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_BLEND);

	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////

	if(showCloudSampling){
		//Disegno la cloudSampling
		if(showInObjectSpace){
			for(int i=0;i<(int)DebugCloudSampling[indexOfstep].size();i++){
				glColor3f(1,0,1);
				glPointSize(3);
				glBegin(GL_POINTS);
				glVertex(DebugCloudSampling[indexOfstep][i]);
				glEnd();
			}	
		}else{
			glPushMatrix();
			vcg::glMultMatrix(DebugOrientationMatrix[indexOfstep]);										//<------TRASFORMAZIONI
			for(int i=0;i<(int)DebugCloudSampling[indexOfstep].size();i++){
				glColor3f(1,0,1);
				glPointSize(3);
				glBegin(GL_POINTS);
				glVertex(DebugCloudSampling[indexOfstep][i]);
				glEnd();
			}
			glPopMatrix();
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////

	if(showCloudNearPoints){
		//Disegno la cloudNearPoints
		if(showInObjectSpace){
			for(int i=0;i<(int)DebugCloudNearPoints[indexOfstep].size();i++){
				glColor3f(0,1,1);
				glPointSize(3);
				glBegin(GL_POINTS);
				glVertex(DebugCloudNearPoints[indexOfstep][i]);				
				glEnd();
			}
		}else{
			glPushMatrix();
			vcg::glMultMatrix(DebugOrientationMatrix[indexOfstep]);										//<------TRASFORMAZIONI
			for(int i=0;i<(int)DebugCloudNearPoints[indexOfstep].size();i++){
				glColor3f(0,1,1);
				glPointSize(3);
				glBegin(GL_POINTS);
				glVertex(DebugCloudNearPoints[indexOfstep][i]);				
				glEnd();
			}
			glPopMatrix();
			}
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////

	if(showLines){
		//Disegno Le linee:
		if(showInObjectSpace){
			for(int i=0;i<(int)DebugCloudNearPoints[indexOfstep].size();i++){
				glPointSize(1);
				glBegin(GL_LINES);
				glColor3f(0,1,1);
				glVertex(DebugCloudNearPoints[indexOfstep][i]);
				glColor3f(1,0,1);
				glVertex(DebugCloudSampling[indexOfstep][i]);
				glEnd();
			}
		}else{
			glPushMatrix();
			vcg::glMultMatrix(DebugOrientationMatrix[indexOfstep]);										//<------TRASFORMAZIONI
			for(int i=0;i<(int)DebugCloudNearPoints[indexOfstep].size();i++){
				glPointSize(1);
				glBegin(GL_LINES);
				glColor3f(0,1,1);
				glVertex(DebugCloudNearPoints[indexOfstep][i]);
				glColor3f(1,0,1);
				glVertex(DebugCloudSampling[indexOfstep][i]);
				glEnd();
			}
			glPopMatrix();
		}
	}	

	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////

	if(showSelectionMesh){
		//Disegno la selectionMesh
		if(showInObjectSpace){
			for(int i=0;i<(int)DebugCloudMesh[indexOfstep].size();i++){
				glColor3f(1,0.5,0.25);
				glPointSize(3);
				glBegin(GL_POINTS);
				glVertex(DebugCloudMesh[indexOfstep][i]);
				glEnd();
			}	
		}else{
			glPushMatrix();
			vcg::glMultMatrix(DebugOrientationMatrix[indexOfstep]);										//<------TRASFORMAZIONI
			for(int i=0;i<(int)DebugCloudMesh[indexOfstep].size();i++){
				glColor3f(1,0.5,0.25);
				glPointSize(3);
				glBegin(GL_POINTS);
				glVertex(DebugCloudMesh[indexOfstep][i]);
				glEnd();
			}	
			glPopMatrix();
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////

	glPopAttrib();
}
void Primitive::InitDebug(){
	//Begin_Debug:
	DebugOrientationMatrix.push_back(orientation);
	/////////////////////////////////////////////////////////////////
	DebugCloudSampling.push_back(cloudSampling);
	/////////////////////////////////////////////////////////////////
	std::vector<vcg::Point3f> prova;
	for(int i=0;i<(int)cloudSampling.size();i++){
		float error=0;
		CVertexO *p=NULL;
		p=vcg::trimesh::GetClosestVertex<CMeshO,GridType>(*selectionMesh,*grid,cloudSampling[i],10000/*da modificare*/,error);
		if(p!=NULL){prova.push_back(p->P());}
		else{prova.push_back(vcg::Point3f(0,0,0));}
	}
	DebugCloudNearPoints.push_back(prova);
	/////////////////////////////////////////////////////////////////
	std::vector<vcg::Point3f> tmp;
	for(int i=0;i<(int)selectionMesh->vert.size();i++){	
		tmp.push_back(selectionMesh->vert[i].P());
	}
	DebugCloudMesh.push_back(tmp);
	//End_Debug:
}
Primitive::~Primitive(){
	delete [] parameters;
	parameters=NULL;
}


//slots:
void Primitive::change_checkBoxCloudNearPoints(bool b){
	showCloudNearPoints=b;
}
void Primitive::change_checkBoxLines(bool b){
	showLines=b;
}
void Primitive::change_checkBoxSelectionMesh(bool b){
	showSelectionMesh=b;
}
void Primitive::change_checkBoxCloudSampling(bool b){
	showCloudSampling=b;
}
void Primitive::change_checkBoxObjectSpace(bool b){
	showInObjectSpace=b;
}
void Primitive::change_checkBoxStepMode(bool b){
	showStepMode=b;
}
void Primitive::change_spinBoxStepMode(int i){
	indexOfstep=i;
}