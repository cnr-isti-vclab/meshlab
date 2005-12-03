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
$Log$
Revision 1.3  2005/12/03 22:49:46  cignoni
Added copyright info

****************************************************************************/
#include <QtGui>

#include <math.h>
#include <stdlib.h>

#include "meshcolorize.h"

using namespace vcg;

static void Gaussian(CMeshO &m){
	float min, max = 0.0;
	float histo_frac = 0.10f;
	int histo_range=1000;
	vcg::Histogram<float> histo;

	assert(m.HasPerVertexQuality());

	CMeshO::VertexIterator vi;		// iteratore vertice
	CMeshO::FaceIterator fi;		// iteratore facce
	double *area;					// areamix vector
	int i;							// index
	double area0, area1, area2;
	double angle0, angle1, angle2; 
	
	//--- Initialization
	area = new double[m.vn];

	//reset the values to 0
	for(vi=m.vert.begin();vi!=m.vert.end();++vi) if(!(*vi).IsD())
		(*vi).Q() = 0.0;

	//--- compute Areamix
	for(fi=m.face.begin();fi!=m.face.end();++fi) if(!(*fi).IsD())
	{
		
		// angles
			 angle0 = math::Abs(Angle(	(*fi).V(1)->P()-(*fi).V(0)->P(),(*fi).V(2)->P()-(*fi).V(0)->P() ));
			 angle1 = math::Abs(Angle(	(*fi).V(0)->P()-(*fi).V(1)->P(),(*fi).V(2)->P()-(*fi).V(1)->P() ));
 			 angle2 = M_PI-(angle0+angle1);
		
		if((angle0 < M_PI/2) || (angle1 < M_PI/2) || (angle2 < M_PI/2))  // triangolo non ottuso
		{ 
			float e01 = SquaredDistance( (*fi).V(1)->P() , (*fi).V(0)->P() );
			float e12 = SquaredDistance( (*fi).V(2)->P() , (*fi).V(1)->P() );
			float e20 = SquaredDistance( (*fi).V(0)->P() , (*fi).V(2)->P() );
			
			// voronoi area v[0]
			area0 = ( e01*(1.0/tan(angle2)) + e20*(1.0/tan(angle1)) ) /8;
			// voronoi area v[1]
			area1 = ( e01*(1.0/tan(angle2)) + e12*(1.0/tan(angle0)) ) /8;
			// voronoi area v[2]
			area2 = ( e20*(1.0/tan(angle1)) + e20*(1.0/tan(angle0)) ) /8;
			
			(*fi).V(0)->Q()  += area0;
			(*fi).V(1)->Q()  += area1;
			(*fi).V(2)->Q()  += area2;
		}
		else // triangolo ottuso
		{ 
			(*fi).V(0)->Q() += vcg::Area<CFaceO>((*fi)) / 3.0;
			(*fi).V(1)->Q() += vcg::Area<CFaceO>((*fi)) / 3.0;
			(*fi).V(2)->Q() += vcg::Area<CFaceO>((*fi)) / 3.0;      
		}
	}

	i = 0;
	for(vi=m.vert.begin();vi!=m.vert.end();++vi,++i) if(!(*vi).IsD())
	{
		area[i] = (*vi).Q();
		(*vi).Q() = (float)(2.0 * M_PI);
	}
	
	//cout << "Vertex count: " << i << endl;

	for(fi=m.face.begin();fi!=m.face.end();++fi)  if(!(*fi).IsD())
	{
		float angle0 = math::Abs(Angle(
			(*fi).V(1)->P()-(*fi).V(0)->P(),(*fi).V(2)->P()-(*fi).V(0)->P() ));
		float angle1 = math::Abs(Angle(
			(*fi).V(0)->P()-(*fi).V(1)->P(),(*fi).V(2)->P()-(*fi).V(1)->P() ));
		float angle2 = M_PI-(angle0+angle1);
		
		(*fi).V(0)->Q() -= angle0;
		(*fi).V(1)->Q() -= angle1;
		(*fi).V(2)->Q() -= angle2;
	}
	
	i=0;
	for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi,++i) if(!(*vi).IsD())
	{
		if(area[i]==0) 
			(*vi).Q() = 0;
		else
			(*vi).Q() /= area[i];
	
		if ((*vi).Q() < min) min = (*vi).Q();
		if ((*vi).Q() > max) max = (*vi).Q();

	}

	//cout << "min:" << min << " max:" << max << endl;

	histo.SetRange(min, max, histo_range);
	
	for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi) if(!(*vi).IsD())
	{
		histo.Add((*vi).Q());
	} 

	min = histo.Percentile(histo_frac);
	max = histo.Percentile(1.0f-histo_frac);

	//cout << "Histo: frac=" << histo_frac << " pmin=" << min << " pmax=" << max << "  range=" << histo_range << endl;
	
	for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi) if(!(*vi).IsD())
	{
		(*vi).Q() = math::Clamp((*vi).Q(), min, max);
	}

	//--- DeInit
	
	delete[] area;

}



QStringList MeshColorCurvaturePlugin::colorsFrom() const{
	return QStringList() << tr("Gaussian Curvature");
}

void MeshColorCurvaturePlugin::Compute(const QString &mode, MeshModel &m, QWidget *parent){
	if(mode == tr("Gaussian Curvature"))
	{
		Gaussian(m.cm);
		vcg::tri::UpdateColor<CMeshO>::VertexQuality(m.cm);
	}
};


Q_EXPORT_PLUGIN(MeshColorCurvaturePlugin)
