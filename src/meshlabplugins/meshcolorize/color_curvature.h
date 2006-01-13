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
Revision 1.1  2006/01/13 16:24:16  vannini
Moved gaussian and mean curvature functions into color_curvature.h


****************************************************************************/

#ifndef EXTRACOLOR_CURVATURE_H
#define EXTRACOLOR_CURVATURE_H

#include <iostream>
#include <QtGlobal>
#include <vcg/complex/trimesh/base.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/space/triangle3.h>
#include <vcg/complex/trimesh/update/topology.h>
#include <vcg/math/histogram.h>

#include "../../meshlab/LogStream.h"

namespace vcg 
{
	template<class MESH_TYPE> void ColorGaussian(MESH_TYPE &m, GLLogStream *log)
  {
		assert(m.HasFFTopology());

    float *area;
    int i;
    float area0, area1, area2;
    float angle0, angle1, angle2; 
  
    float histo_frac = 0.1f;
    int histo_range=100000;
    vcg::Histogram<float> histo;
    float minQ=std::numeric_limits<float>::max(),maxQ = -std::numeric_limits<float>::max();

 		typename MESH_TYPE::VertexIterator vi;
		typename MESH_TYPE::FaceIterator fi;

    area = new float[m.vn];
      
    for(vi=m.vert.begin();vi!=m.vert.end();++vi) if(!(*vi).IsD())
      (*vi).Q() = 0.0;
  
    // AreaMix
    for(fi=m.face.begin();fi!=m.face.end();++fi) if(!(*fi).IsD())
    {
      // angles
      angle0 = math::Abs(Angle(	(*fi).P(1)-(*fi).P(0),(*fi).P(2)-(*fi).P(0) ));
      angle1 = math::Abs(Angle(	(*fi).P(0)-(*fi).P(1),(*fi).P(2)-(*fi).P(1) ));
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
      
    for(fi=m.face.begin();fi!=m.face.end();++fi)  if(!(*fi).IsD())
    {
      float angle0 = math::Abs(Angle((*fi).P(1)-(*fi).P(0),(*fi).P(2)-(*fi).P(0)));
      float angle1 = math::Abs(Angle((*fi).P(0)-(*fi).P(1),(*fi).P(2)-(*fi).P(1)));
      float angle2 = M_PI-(angle0+angle1);
        
      (*fi).V(0)->Q() -= angle0;
      (*fi).V(1)->Q() -= angle1;
      (*fi).V(2)->Q() -= angle2;
    }
       
    i=0;
    for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi,++i) if(!(*vi).IsD())
    {
      if(area[i]<=std::numeric_limits<float>::epsilon())
        (*vi).Q() = 0;
      else
        (*vi).Q() /= area[i];
        
      if ((*vi).Q() < minQ) minQ = (*vi).Q();
      if ((*vi).Q() > maxQ) maxQ = (*vi).Q();   
    }
      
    histo.SetRange(minQ, maxQ, histo_range);
    
    for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi) if(!(*vi).IsD())
      histo.Add((*vi).Q());
    
    minQ = histo.Percentile(histo_frac);
    maxQ = histo.Percentile(1.0f-histo_frac);
      
    for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi) if(!(*vi).IsD())
      (*vi).Q() = math::Clamp((*vi).Q(), minQ, maxQ);

    if (log)
      log->Log(GLLogStream::Info, "Mean Curvature: minQ=%f maxQ=%f range=%d", minQ, maxQ, histo_range);

    delete[] area;
    
  }

  template<class MESH_TYPE> void ColorMean(MESH_TYPE &m, GLLogStream *log)
  {
		assert(m.HasFFTopology());

    float *area;
    int i;
    float area0, area1, area2;
    float angle0, angle1, angle2; 
  
    float histo_frac = 0.1f;
    int histo_range=100000;
    vcg::Histogram<float> histo;
    float minQ=std::numeric_limits<float>::max(),maxQ = -std::numeric_limits<float>::max();

 		typename MESH_TYPE::VertexIterator vi;
		typename MESH_TYPE::FaceIterator fi;

    area = new float[m.vn];
      
    for(vi=m.vert.begin();vi!=m.vert.end();++vi) if(!(*vi).IsD())
      (*vi).Q() = 0.0;
  
    // AreaMix
    for(fi=m.face.begin();fi!=m.face.end();++fi) if(!(*fi).IsD())
    {    
      // angles
      angle0 = math::Abs(Angle(	(*fi).P(1)-(*fi).P(0),(*fi).P(2)-(*fi).P(0) ));
      angle1 = math::Abs(Angle(	(*fi).P(0)-(*fi).P(1),(*fi).P(2)-(*fi).P(1) ));
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
      (*vi).Q() = 0;
    }
          
    for(fi=m.face.begin();fi!=m.face.end();++fi) if(!(*fi).IsD())
    {    
  	  
      angle0 = math::Abs(Angle(	(*fi).P(1)-(*fi).P(0),(*fi).P(2)-(*fi).P(0) ));
      angle1 = math::Abs(Angle(	(*fi).P(0)-(*fi).P(1),(*fi).P(2)-(*fi).P(1) ));
      angle2 = M_PI-(angle0+angle1);

      float e01 = Distance( (*fi).V(1)->P() , (*fi).V(0)->P() );
      float e12 = Distance( (*fi).V(2)->P() , (*fi).V(1)->P() );
      float e20 = Distance( (*fi).V(0)->P() , (*fi).V(2)->P() );
  	  
      area0 = ( e01*(1.0/tan(angle2)) + e20*(1.0/tan(angle1)) )/2;
      area1 = ( e01*(1.0/tan(angle2)) + e12*(1.0/tan(angle0)) )/2;
      area2 = ( e20*(1.0/tan(angle1)) + e20*(1.0/tan(angle0)) )/2;
        
      (*fi).V(0)->Q() += area0;
      (*fi).V(1)->Q() += area1;
      (*fi).V(2)->Q() += area2;
       
    }
       
    i=0;
    for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi,++i) if(!(*vi).IsD())
    {
      if(area[i]<=std::numeric_limits<float>::epsilon())
        (*vi).Q() = 0;
      else
        (*vi).Q() /= area[i];
        
      if ((*vi).Q() < minQ) minQ = (*vi).Q();
      if ((*vi).Q() > maxQ) maxQ = (*vi).Q();   
    }
      
    histo.SetRange(minQ, maxQ, histo_range);
    
    for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi) if(!(*vi).IsD())
      histo.Add((*vi).Q());
    
    minQ = histo.Percentile(histo_frac);
    maxQ = histo.Percentile(1.0f-histo_frac);
      
    for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi) if(!(*vi).IsD())
      (*vi).Q() = math::Clamp((*vi).Q(), minQ, maxQ);

    if (log)
      log->Log(GLLogStream::Info, "Mean Curvature: minQ=%f maxQ=%f range=%d", minQ, maxQ, histo_range);

    delete[] area;
    
  }

}

#endif