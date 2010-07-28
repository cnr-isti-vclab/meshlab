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

#ifndef CYLINDER_H
#define CYLINDER_H

#include <vcg/space/obox3.h>
#include <vcg/complex/trimesh/allocate.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <time.h>
#include "primitive.h"

class Cylinder : public Primitive{

private:
	double										DistanzaOrizzontale;		//Risultato della density serve per la RigidSampling
	double										DistanzaVerticale;			//Risultato della density serve per la RigidSampling
	int												numeroCerchi;						//Risultato della RigidSampling serve per il Sampling
	int												numeroPuntiCerchio;			//Risultato della RigidSampling serve per il Sampling
	std::vector<vcg::Point3f>	cloudSelection;					//serve per calcolare la densità
	vcg::Obox3f								BoxSelection;						//E' il box che racchiude il cilindro
	void											Density();							//Calcola la densità e DistanzaOrizzontale e DistanzaVerticale

public:
	Cylinder(std::vector<vcg::Point2i> *_gesture2D,std::vector<vcg::Point3f> *_gesture3D,CMeshO *_TotMesh);
	void Sampling(const double *par);
	void InitSampling(double *par);
	void Draw();
	void DrawNormal();
	~Cylinder();
};


inline Cylinder::Cylinder(std::vector<vcg::Point2i> *_gesture2D,std::vector<vcg::Point3f>	*_gesture3D,CMeshO *_TotMesh):Primitive(_gesture2D,_gesture3D,_TotMesh){
	
	//servono per il metodo Sampling
	numeroCerchi=0;
	numeroPuntiCerchio=0;
	//Numero parametri del cilindro:
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

inline void Cylinder::Sampling(const double *par){

	double altezza=par[0];
	double raggioUp=par[1];
	double raggioDown=par[2];

	double x=0;
	double y=-altezza/2;
	double z=0;
	double alfa=0;
	double raggio=raggioDown;
	int count=numeroPuntiCerchio;

	//Varia l'altezza o i raggi ma il numero dei punti del sampling e fisso
	double intervalloRaggio=raggioDown-raggioUp;						
	double incRaggio=intervalloRaggio/numeroCerchi;
	double DistVerticale=altezza/numeroCerchi;

	for(int i=0;i<(int)cloudSampling.size();i++){
		if(i==count){																			//Ogni "count" punti ho fatto un cerchio quindi azzero "alfa"
			count+=numeroPuntiCerchio;											//e mi sposto di un'unità "DistVerticale" in su e genero il
			y+=DistVerticale;																//successivo cerchio.
			alfa=0;
			raggio-=incRaggio;
		}		
		x=cos(alfa)*raggio;
		z=sin(alfa)*raggio;
		cloudSampling[i]=vcg::Point3f(x,y,z);
		alfa+=DistanzaOrizzontale;
	}
}
inline void Cylinder::Density(){

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
	DistanzaOrizzontale=0.1;
	DistanzaVerticale=sqrt(rapporto)*7;
}

inline void Cylinder::Draw(){
	DrawNormal();
	DrawDebug();
}
inline void Cylinder::DrawNormal(){
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

inline void Cylinder::InitSampling(double *par){

	double PI =3.1415926535897; 

	double altezza=par[0];
	double raggioUp=par[1];
	double raggioDown=par[2];

	double intervalloRaggio=raggioDown-raggioUp;
	//Conto quanti cerchi ho generato per il sampling
	numeroCerchi=altezza/DistanzaVerticale;							
	double incRaggio=intervalloRaggio/numeroCerchi;
	double raggio=raggioDown;

	for (double j=(-altezza/2);j<(+altezza/2);j+=DistanzaVerticale){
		numeroPuntiCerchio=0;
		for (double i=0;i<(2*PI);i+=DistanzaOrizzontale){ 
			cloudSampling.push_back( vcg::Point3f(cos(i)*raggio, (double)j ,sin(i)*raggio) );
			//inizializzo anche cloudNearPoints
			cloudNearPoints.push_back(vcg::Point3f(0,0,0));
			//inizializzo i pesi
			weight.push_back(1.0);
			//inizializzo myfvec
			myfvec.push_back(0.0);			
			//Conto quanti punti ho generato per ogni cerchio
			numeroPuntiCerchio++;
		}
		raggio-=incRaggio;
	}
}
inline Cylinder::~Cylinder(){
	delete selectionMesh;
	selectionMesh=NULL;
	delete grid;
	grid=NULL;
}
#endif