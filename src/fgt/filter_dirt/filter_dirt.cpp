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
	return MeshModel::MM_FACECOLOR;
}


bool FilterDirt::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & par, vcg::CallBackPos * cb) 
{
	/*Select a random face and check only angle between face normal and mesh's -Y axe for now*/
	/* 
		Problema: Non sono riuscito a capire perchè (*di)->C()=vcg::Color4b::Blue; (come le funzioni vcg::tri::UpdateColor) 
		non funziona se prima non applico un filtro di tipo Colorize che modifica il colore delle facce
		(triangle intersection, triangle qualitity, etc...). Ritorno in getRequest MM_FACECOLOR, per sicurezza ho
		provato anche con MM_ALL.
		La stessa cosa succede se provo a cambiare il colore dei vertici,
		prima devo applicare un filtro che modifica il colore dei vertici (come ad esempio Border).
		Ho letto più volte il plugin meshcolorize senza trovare soluzione, ho provato con il debugger 
		a guardare cosa succede ma sono solo riuscito a vedere che il colore della faccia viene effettivamente modificato.

		TODO:
		 *Muovere i ton lungo la superfice per un certo numero di iterazioni
		 *fornire interfaccia per selezionare numero di ton e numero di iterazioni
	     *Modificare flag vertici per tenere traccia della quantità di dust
		 e per modificarne il colore alla fine delle iterazioni
	*/

	//pointer to face where are dust ton 
	vector<CFaceO*> dustTon;

	vector<CFaceO*>::iterator di;

	CMeshO::FaceIterator fi = m.cm.face.begin();

	int randomFace=0;

	//select random face 
	for (int cont=0; cont<defaultDustTon; ++cont)
	{
		randomFace = (int)rand()%m.cm.fn;
		if (!fi[randomFace].IsD())
			dustTon.push_back(&fi[randomFace]);
		else
			--cont;
	}
	
	vcg::Point3f dustDirection;

	for (di=dustTon.begin(); di!=dustTon.end(); ++di)
	{
		//color face for debug
		(*di)->C()=vcg::Color4b::Blue;
		//get vector direction to move dust over face
		dustDirection = ((*di)->N() ^ vcg::Point3f(0,-1,0)) ^ (*di)->N();
		//check if selected face angle between normal and -Y axe
		if (!(vcg::Angle((*di)->N(),vcg::Point3f(0,-1,0))<M_PI/2))
			Log(0,"angolo tra vettore normale e -Y acuto");
		else
			Log(0,"angolo tra vettore normale e -Y ottuso");
	}

	return true;
}


Q_EXPORT_PLUGIN(FilterDirt)
