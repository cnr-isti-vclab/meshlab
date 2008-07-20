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
#include <time.h>


#include <vcg/math/base.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/stat.h>
#include <vcg/complex/trimesh/smooth.h>
#include <vcg/complex/trimesh/update/flag.h>
#include <vcg/complex/trimesh/update/selection.h> 
#include <vcg/complex/trimesh/update/color.h>


using namespace std;
using namespace vcg;

FilterDirt::FilterDirt()
{
	
    typeList << 
    FP_DIRT;
    
	FilterIDType tt;
	foreach(tt , types())
	actionList << new QAction(filterName(tt), this);
}

const QString FilterDirt::filterName(FilterIDType filterId) 
{
	if(filterId!= FP_DIRT)
	{
		return QString("error!");
	}
	return QString("Dirt Maker");
}
const QString FilterDirt::filterInfo(FilterIDType filterId) 
{
	
	if(filterId!= FP_DIRT)
	{
		return QString("error!");
	}
	return QString("Simulate dirt accumolation over the mesh");
}

const PluginInfo &FilterDirt::pluginInfo() 
{
	static PluginInfo ai; 
	ai.Date=tr("July 2008");
	ai.Version = tr("0.2");
	ai.Author = ("Luca Bartoletti");
	return ai;
}

const int FilterDirt::getRequirements(QAction *action)
{
	
	return MeshModel::MM_FACETOPO | MeshModel::MM_FACECOLOR | MeshModel::MM_FACEMARK;	
}

bool FilterDirt::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & par, vcg::CallBackPos *cb)
{
	srand (time(NULL));	
	
	vcg::tri::UpdateColor<CMeshO>::FaceConstant(m.cm,Color4b::White);
	
	CMeshO::FaceIterator fi = m.cm.face.begin();
	
	CFaceO* face = NULL;
	
	//for now it take only a random face and check a casual point over
	do
	{
		face = &fi[rand()%m.cm.fn];
	}
	while (face->IsD());
	
	//find a casual point over face
	float a = (float)(rand()/(double)RAND_MAX);
	float b;
	do
	{
		b=(float)(rand()/(double)RAND_MAX);
	}
	while (b>(1-a));
	
	//to remove
	vcg::Point3f deb0 = face->V(0)->P();
	vcg::Point3f deb1 = face->V(1)->P();
	vcg::Point3f deb2 = face->V(2)->P();
	//end to remove part
	
	vcg::Point3f v1 = face->V(1)->P() - face->V(0)->P();
	vcg::Point3f v2 = face->V(2)->P() - face->V(0)->P();
	
	//find casual point over face with linear span of v1 and v2
	vcg::Point3f casualPoint = (v1*a) + (v2*b);
	
	casualPoint = casualPoint + face->V(0)->P();
	
	
	bool ottuso=false, border=false;
	
	for (int i=0; i<100 && !ottuso && !border; ++i)
	{
		//get dust direction over face
		vcg::Point3f dustDirection = (face->N().Normalize() ^  vcg::Point3f(0,-1,0)) ^ face->N().Normalize();
		//check if selected face angle between normal and -Y axe
		//if (!(vcg::Angle(face->N(),vcg::Point3f(0,-1,0))<M_PI/2))
		//{
			//float mediumDistance = mediumVertexDistance(face);
			//color face for debug
			if (i==0)
				face->C()=vcg::Color4b::Green;
			else
				face->C()=vcg::Color4b::Blue;
		
			vcg::Point3f direction = dustDirection + casualPoint;		
		
			//find wedge incident with dust direction (i'll change this with better function)
			vcg::Point3f vertDist[3];
			
			for (int k=0; k<3; k++)
				vertDist[k] = face->V(k)->P() - direction;
			
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
			//note: mindist1 is the more neighbor vertex to direction
			
			//get point over wedge and select next face
			vcg::Point3f q;
			float radius = vcg::PSDist<float>(casualPoint,face->V(minDist1)->P(),face->V(minDist2)->P(),q);
		
			v1 = (q-casualPoint).Normalize();
			v2 = (direction - casualPoint).Normalize();
			float angle = vcg::math::Acos(v1*v2);
		
			//find intersection point between casualPoint and direction
			float tanValue = (float) radius * (sin(angle)/cos(angle));
			vcg::Point3f v = q - face->V(minDist1)->P();
			vcg::Point3f subVector = -v.Normalize();
			subVector *= tanValue;
			vcg::Point3f intersectionPoint = (v - subVector) + face->V(minDist1)->P();		
			
		
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
			casualPoint = intersectionPoint;
	}
		
	return true;
}



float FilterDirt::mediumVertexDistance(CFaceO* face)
{
	float dist = vcg::Distance<float>(face->V(0)->P(),face->V(1)->P());
	dist +=vcg::Distance<float>(face->V(1)->P(),face->V(2)->P());
	dist += vcg::Distance<float>(face->V(0)->P(),face->V(2)->P());
	return dist/3;
}

/*!!! I don't understand why if i did't return MeshFilterInterface::FaceColoring all my function call
 to FaceConstant (et simila) don't take effect over the mesh. I've read from Interface.c comment getClass is
 only to decide where my plugin label is in Plugin submenu of meshlab menubar.*/
const MeshFilterInterface::FilterClass FilterDirt::getClass(QAction *actionId)
{
	return MeshFilterInterface::FaceColoring;
}


Q_EXPORT_PLUGIN(FilterDirt)