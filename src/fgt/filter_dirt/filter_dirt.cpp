/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
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

#include <Qt>
#include <QtGui>
#include "filter_dirt.h"

#include <vcg/complex/trimesh/update/flag.h>
#include <vcg/complex/trimesh/update/color.h>

#include <stdlib.h>
#include <time.h>

using namespace std;
using namespace vcg;

FilterDirt::FilterDirt():
	defaultDustTon(1)
{
  typeList << FP_DIRT;
 
  FilterIDType tt;
  foreach(tt , types())
	    actionList << new QAction(filterName(tt), this);

}

FilterDirt::~FilterDirt() {
	for (int i = 0; i < actionList.count() ; i++ ) 
		delete actionList.at(i);
}

const QString FilterDirt::filterName(FilterIDType filter) 
{
 switch(filter)
  {
	  case FP_DIRT :								return QString("Simulate dust");
  	default: assert(0);
  }
  return QString("error!");
}

const QString FilterDirt::filterInfo(FilterIDType filterId)
{
  switch(filterId)
  {
		case FP_DIRT:	     return tr("Simulate dust over mesh."); 
  	default: assert(0);
  }
  return QString("error!");
}


const PluginInfo &FilterDirt::pluginInfo()
{
   static PluginInfo ai;
   ai.Date=tr( __DATE__ );
	 ai.Version = tr("0.1");
	 ai.Author = ("Luca Bartoletti");
   return ai;
}

const int FilterDirt::getRequirements(QAction *action)
{
	return MeshModel::MM_FACECOLOR | MeshModel::MM_VERTFACETOPO | MeshModel::MM_FACETOPO;
}


bool FilterDirt::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & par, vcg::CallBackPos * cb) 
{
	/*Select a random face and check only angle between face normal and mesh's -Y, try to move dust over face for 100 iteration*/
	/* 
		Problema: Non sono riuscito a capire perchè face->C()=vcg::Color4b::Blue; (come le funzioni vcg::tri::UpdateColor) 
		non funziona se prima non applico un filtro di tipo Colorize che modifica il colore delle facce
		(triangle intersection, triangle qualitity, etc...). Ritorno in getRequest MM_FACECOLOR, per sicurezza ho
		provato anche con MM_ALL.
		La stessa cosa succede se provo a cambiare il colore dei vertici,
		prima devo applicare un filtro che modifica il colore dei vertici (come ad esempio Border).
		Ho letto più volte il plugin meshcolorize senza trovare soluzione, ho provato con il debugger 
		a guardare cosa succede ma sono solo riuscito a vedere che il colore della faccia viene effettivamente modificato.

		TODO:
		 *Muovere i ton lungo la superfice per un certo numero di iterazioni (per ora questo avviene muovendosi una faccia per iterazione)
		 *La quantità di movimento deve dipendere dalla pendenza (della faccia) e dall'attrito della superfice (parametro utente)
		 *fornire interfaccia per selezionare numero di ton e numero di iterazioni
	     *Modificare flag vertici per tenere traccia della quantità di dust
		 e per modificarne il colore alla fine delle iterazioni
	*/
	//init rand seed
	srand ( time(NULL) );

	//pointer to face where are dust ton 
	vector<CFaceO*> dustTon;

	vector<CFaceO*>::iterator di;

	CMeshO::FaceIterator fi = m.cm.face.begin();

	int randomFace=0;

	/*
	//select random face 
	for (int cont=0; cont<defaultDustTon; ++cont)
	{
		randomFace = (int)rand()%m.cm.fn;
		if (!fi[randomFace].IsD())
			dustTon.push_back(&fi[randomFace]);
		else
			--cont;
	}*/

	CFaceO* face = NULL;

	//for now it take only a random face and check a casual point over
	do
	{
		face = &fi[rand()%m.cm.fn];
	}
	while (face->IsD());

	assert (face!=NULL);

	vcg::tri::UpdateColor<CMeshO>::FaceConstant(m.cm);

	bool ottuso = false, border= false;

	//find a casual point over face
	float a = (float)(rand()%100);
	float b = (float)(rand()%(int)(100-a))/100;
	a /= 100;
	float c = 1.0f - (a + b);
	vcg::Point3f casualPoint = (face->V(0)->P()*a) + (face->V(1)->P()*b) + (face->V(2)->P()*c);	

	for (int i=0; i<100 && !ottuso && !border; ++i)
	{
		//get dust direction over face
		vcg::Point3f dustDirection = (face->N().Normalize() ^  vcg::Point3f(0,-1,0)) ^ face->N().Normalize();
		//check if selected face angle between normal and -Y axe
		if (!(vcg::Angle(face->N(),vcg::Point3f(0,-1,0))<M_PI/2))
		{
			vcg::Point3f casualDirection = casualPoint + dustDirection*100;
			//color face for debug
			if (i==0)
				face->C()=vcg::Color4b::Green;
			else
				face->C()=vcg::Color4b::Blue;

			//find wedge incident with dust direction
			vcg::Point3f vertDist[3];
			
			for (int k=0; k<3; k++)
				vertDist[k] = face->V(k)->P() - casualDirection;

			int minDist1=0;		
			int minDist2=1;
			for (int k=1; k<3; ++k)
			{
				if (vertDist[k].Norm()<vertDist[minDist1].Norm())
				{
					minDist2=minDist1;
					minDist1=k;
				}
			}				
			for (int k=2; k>=0; --k)
			{
				if ((k!=minDist1) && (vertDist[k].Norm()<vertDist[minDist2].Norm()))
					minDist2=k;
			}

			//get point over wedge and select next face
			vcg::Point3f q;
			vcg::PSDist<float>(casualPoint,face->V(minDist1)->P(),face->V(minDist2)->P(),q);

			int nextFace;
			if ((minDist1 == 0 && minDist2 == 1) || (minDist1 == 1 && minDist2 == 0) )
				nextFace = 0;
			else if ((minDist1 == 1 && minDist2 == 2) || (minDist1 == 2 && minDist2 == 1))
				nextFace = 1;
			else
				nextFace = 2;

			if (face == face->FFp(nextFace))
			{
				border = true;
				Log (0, "[%d] trovato bordo", i);
				face->C()=vcg::Color4b::Red;
			}
			else
				face = face->FFp(nextFace);
			
			//for now i get q, this point isn't right because isn't point incident between dustDirection and face wedge 
			//but is min distance between p and wedge
			casualPoint = q;
		}
		else
		{
			Log(0,"[%d] angolo tra vettore normale e -Y ottuso", i);
			face->C()=vcg::Color4b::Red;
			ottuso = true;
		}
	}

	return true;
}


Q_EXPORT_PLUGIN(FilterDirt)
