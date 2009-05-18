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

#ifndef BOX_H
#define BOX_H

#include <vcg/space/obox3.h>
#include <vcg/complex/trimesh/allocate.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <time.h>
#include "primitive.h"

class Box : public Primitive{

private:

	float scaleX;
	float scaleY;
	float scaleZ;

	float oldHeight;
	float oldWidth;
	float oldDepth;

	double										DistanzaOrizzontale;		//Risultato della density serve per la RigidSampling
	double										DistanzaVerticale;			//Risultato della density serve per la RigidSampling
	std::vector<vcg::Point3f>	cloudSelection;					//serve per calcolare la densità
	vcg::Obox3f								BoxSelection;						//E' il box che racchiude il cilindro
	void											Density();							//Calcola la densità e DistanzaOrizzontale e DistanzaVerticale

public:
	Box(std::vector<vcg::Point2i> *_gesture2D,std::vector<vcg::Point3f> *_gesture3D,CMeshO *_TotMesh);
	void Sampling(const double *par);
	void InitSampling(double *par);
	void Draw();
	void DrawNormal();
	//void DrawDebug();
	~Box();
};


inline Box::Box(std::vector<vcg::Point2i> *_gesture2D,std::vector<vcg::Point3f>	*_gesture3D,CMeshO *_TotMesh):Primitive(_gesture2D,_gesture3D,_TotMesh){

	//Inizializzo:
	scaleX=1;
	scaleY=1;
	scaleZ=1;
	oldHeight=0;
	oldWidth=0;
	oldDepth=0;
	//Numero parametri del Box:
	numParameters=3;
	parameters= new double [numParameters];
	//Istanzio una mesh.
	selectionMesh= new CMeshO;
	//Istanzio la griglia.
	grid=new GridType;

	//Caso in cui faccio una gesto nello sfondo
	if(gesture3D->size()!=0){		
		vcg::Point3f a=(*gesture3D)[0];
		vcg::Point3f b=(*gesture3D)[gesture3D->size()-1];
		parameters[0]=vcg::Distance(a,b);								//Altezza
		parameters[1]=vcg::Distance(a,b)/4;							//RaggioUp
		parameters[2]=(vcg::Distance(a,b)/4)+0.5;				//RaggioDown
		float diameter=parameters[0]/2;
		//New frame: costruisco il nuovo sistema di riferimento dal gesto del cilindro
		vcg::Point3f frame[4];
		frame[3]=(a+b)/2.0;																															//centro
		frame[1]=(a-b).Normalize();																											//baseY
		frame[2]=(vcg::Point3f(0,0,1)^frame[1]).Normalize();														//baseZ
		if (frame[2].Norm()<0.01) frame[2]=(vcg::Point3f(1,0,0)^frame[1]).Normalize();				
		frame[0]=(frame[1]^frame[2]).Normalize();																				//baseX
		//Calcolo l'oriented bounding box
		vcg::Point3f min(-diameter/2.0,-parameters[0]/2.0,-diameter/2.0);
		vcg::Point3f max(+diameter/2.0,+parameters[0]/2.0,+diameter/2.0);
		BoxSelection=vcg::Obox3f(min,max,frame);
		//Ho trovato posizione ed orientamento
		orientation=BoxSelection.mi;
		//Trovo tutti i punti allinterno dell BoxSelection serve per la Density.
		for(int i=0;i<(int)TotMesh->vert.size();i++){
			vcg::Point3f p=TotMesh->vert[i].P();
			
			if(BoxSelection.IsIn(p)){
				cloudSelection.push_back(BoxSelection.m*p);
				//Costruisco la sotto Mesh della selezione
				CMeshO::VertexIterator vi;
				vi = vcg::tri::Allocator<CMeshO>::AddVertices(*selectionMesh,1);
				(*vi).P() = BoxSelection.m*p;
				vcg::tri::UpdateBounding<CMeshO>::Box(*selectionMesh);
			}
		}
		//Costruisco la griglia della sotto mesh
		selectionMesh->vert.EnableMark();	
		grid->Set<CMeshO::VertexIterator>(selectionMesh->vert.begin(),selectionMesh->vert.end());
		//Inizializza DistanzaOrizzontale & DistanzaVerticale.
		Density();			
	}
}

inline void Box::Sampling(const double *par){

	double height=par[0];			//altezza		 Y
	double width=par[1];			//larghezza  X
	double depth=par[2];			//profondita Z
		
	if(height!=oldHeight){
		scaleY=((height/2)/(oldHeight/2));
		oldHeight=height;
	}	
	if(width!=oldWidth){
		scaleX= ((width/2)/(oldWidth/2));
		oldWidth=width;
	}
	if(depth!=oldDepth){
		scaleZ= ((depth/2)/(oldDepth/2));
		oldDepth=depth;
	}

	vcg::Matrix44f m;
	m.SetScale(scaleX,scaleY,scaleZ);
	for(int i=0;i<cloudSampling.size();i++){
		cloudSampling[i]=m*cloudSampling[i];
	}
	scaleX=1; scaleY=1; scaleZ=1;
}
inline void Box::Density(){

	//Data una mesh restituisce mediamente quanti punti cadono nel
	//box campione.(come Dots Per Inch (DPI) per le stampe).
	float densita=0;
	float Ncampioni=100;
	vcg::Box3f UnitBox;
	srand(time(0));
	//Calcolo il lato del cubo campione
	//1% della diagonale dell BBbox della Mesh
	float lato=(1* BoxSelection.Diag())/100;
	//Prendo un campione (random) di punti dalla mesh ed in questi ci
	//piazzo il nostro cubo campione, e poi conto quanti punti 
	//dell'intera Mesh cadono nel cubo e ne faccio la media.
	int size=cloudSelection.size()-1;
	for(int i=0;i<Ncampioni;i++){
		UnitBox=vcg::Box3f(cloudSelection[rand()%size],lato);
		int conta=0;	
		for(int i=0;i<(int)cloudSelection.size();i++){
			if(UnitBox.IsIn(cloudSelection[i]))conta++;
		}
		densita+=conta;
	}
	densita=densita/Ncampioni;
	float area=lato*lato;
	float rapporto=area/densita;
	DistanzaOrizzontale=20;
	DistanzaVerticale=sqrt(rapporto)*10;
}

inline void Box::Draw(){
	DrawNormal();
	DrawDebug();
}

inline void Box::DrawNormal(){
	glPushAttrib(GL_DEPTH_BUFFER_BIT|GL_ENABLE_BIT|GL_LINE_BIT|GL_POINT_BIT|GL_CURRENT_BIT|GL_LIGHTING_BIT|GL_COLOR_BUFFER_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_BLEND);

	//Disegno l'oriented boundingBox della selezione
	glPushMatrix();
		vcg::glMultMatrix(BoxSelection.mi);								//<------TRASFORMAZIONI
		glColor3f(1.0,1.0,0);
		vcg::glBoxWire(BoxSelection);
	glPopMatrix();

	glPopAttrib();
}
inline void Box::InitSampling(double *par){
	/*
		|------2°------|
		|							 |
		3°						 4°
		|							 |
		|							 |
		|------1°------|
	*/

	double height=par[0];			//altezza
	double width =par[1];			//larghezza
	double depth =par[2];			//profondita

	oldHeight=height;
	oldWidth=width;
	oldDepth=depth;

	for (double j=(-height/2);j<(+height/2);j+=DistanzaVerticale){
		//1° and 2° side:
		for (double i=-width/2;i<=width/2;i+=DistanzaOrizzontale){ 
			cloudSampling.push_back(vcg::Point3f(i,j,-depth/2)); 
			cloudSampling.push_back(vcg::Point3f(i,j,depth/2)); 
			//inizializzo anche cloudNearPoints
			cloudNearPoints.push_back(vcg::Point3f(0,0,0));
			cloudNearPoints.push_back(vcg::Point3f(0,0,0));
		}
		//3° and 4° side:
		for (double i=(-depth/2);i<=(depth/2);i+=DistanzaOrizzontale){ 
			cloudSampling.push_back(vcg::Point3f(-width/2,j,i)); 
			cloudSampling.push_back(vcg::Point3f(width/2,j,i)); 
			//inizializzo anche cloudNearPoints
			cloudNearPoints.push_back(vcg::Point3f(0,0,0));
			cloudNearPoints.push_back(vcg::Point3f(0,0,0));
		}
	}	
}
inline Box::~Box(){
	delete selectionMesh;
	selectionMesh=NULL;
	delete grid;
	grid=NULL;
}
#endif