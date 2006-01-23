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
Revision 1.4  2006/01/23 11:48:48  vannini
bugfix in AreaMix() (voronoi area)

Revision 1.3  2006/01/20 16:25:39  vannini
Added Absolute Curvature colorize

Revision 1.2  2006/01/20 14:46:44  vannini
Code refactoring
Added RMS Curvature colorize

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

#define DEFAULT_HISTO_FRAC 0.1f
#define DEFAULT_HISTO_RANGE 10000

class histoMinMaxQ
{
  public:
    float minQ, maxQ;
};

namespace vcg 
{
 
  template<class MESH_TYPE> void AreaMix(MESH_TYPE &m)
  {
    typename MESH_TYPE::VertexIterator vi;
		typename MESH_TYPE::FaceIterator fi;
    float area0, area1, area2;
    float angle0, angle1, angle2; 

    for(vi=m.vert.begin();vi!=m.vert.end();++vi) if(!(*vi).IsD())
      (*vi).Q() = 0.0;

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
  	  
        area0 = ( e20*(1.0/tan(angle1)) + e01*(1.0/tan(angle2)) ) / 8.0;
	      area1 = ( e01*(1.0/tan(angle2)) + e12*(1.0/tan(angle0)) ) / 8.0;
	      area2 = ( e12*(1.0/tan(angle0)) + e20*(1.0/tan(angle1)) ) / 8.0;
  	  
	      (*fi).V(0)->Q()  += area0;
	      (*fi).V(1)->Q()  += area1;
	      (*fi).V(2)->Q()  += area2;
	    }
      else // triangolo ottuso
	    { 
	      (*fi).V(0)->Q() += vcg::DoubleArea<CFaceO>((*fi)) / 6.0;
	      (*fi).V(1)->Q() += vcg::DoubleArea<CFaceO>((*fi)) / 6.0;
	      (*fi).V(2)->Q() += vcg::DoubleArea<CFaceO>((*fi)) / 6.0;      
	    }
    }
  }

  template<class MESH_TYPE> histoMinMaxQ HK(MESH_TYPE &m, bool computeH, bool useHisto, float histo_frac=DEFAULT_HISTO_FRAC, int histo_range=DEFAULT_HISTO_RANGE) 
  {
    // Calcola la curvatura gaussiana (K) oppure la media (H) in base a computeH
    // e salva il risultato in Q
    // Se useHisto è true approssima il risultato usando l'istogramma

    float *area;
    int i;
    float area0, area1, area2, angle0, angle1, angle2, e01, e12, e20, t;
    typename MESH_TYPE::VertexIterator vi;
		typename MESH_TYPE::FaceIterator fi;
    vcg::Histogram<float> histo;
    histoMinMaxQ hmmq;
    area = new float[m.vn];
    hmmq.minQ = std::numeric_limits<float>::max();
    hmmq.maxQ = -std::numeric_limits<float>::max();
    
    AreaMix(m);   
  
    if (computeH)
      t=0;
    else
      t=(float)(2.0 * M_PI);

    i = 0;
    for(vi=m.vert.begin();vi!=m.vert.end();++vi,++i) if(!(*vi).IsD())
    {
      area[i] = (*vi).Q();
      (*vi).Q() = t;
    }
    
    if (computeH)
    {
      for(fi=m.face.begin();fi!=m.face.end();++fi) if(!(*fi).IsD())
      {    
        angle0 = math::Abs(Angle(	(*fi).P(1)-(*fi).P(0),(*fi).P(2)-(*fi).P(0) ));
        angle1 = math::Abs(Angle(	(*fi).P(0)-(*fi).P(1),(*fi).P(2)-(*fi).P(1) ));
        angle2 = M_PI-(angle0+angle1);

        e01 = Distance( (*fi).V(1)->P() , (*fi).V(0)->P() );
        e12 = Distance( (*fi).V(2)->P() , (*fi).V(1)->P() );
        e20 = Distance( (*fi).V(0)->P() , (*fi).V(2)->P() );
    	  
        area0 = ( e01*(1.0/tan(angle2)) + e20*(1.0/tan(angle1)) )/2;
        area1 = ( e01*(1.0/tan(angle2)) + e12*(1.0/tan(angle0)) )/2;
        area2 = ( e20*(1.0/tan(angle1)) + e20*(1.0/tan(angle0)) )/2;
          
        (*fi).V(0)->Q() += area0;
        (*fi).V(1)->Q() += area1;
        (*fi).V(2)->Q() += area2;    
      }
    }
    else
    {
      for(fi=m.face.begin();fi!=m.face.end();++fi)  if(!(*fi).IsD())
      {
        float angle0 = math::Abs(Angle((*fi).P(1)-(*fi).P(0),(*fi).P(2)-(*fi).P(0)));
        float angle1 = math::Abs(Angle((*fi).P(0)-(*fi).P(1),(*fi).P(2)-(*fi).P(1)));
        float angle2 = M_PI-(angle0+angle1);
          
        (*fi).V(0)->Q() -= angle0;
        (*fi).V(1)->Q() -= angle1;
        (*fi).V(2)->Q() -= angle2;
      }
    }
    
    if (!useHisto)
    {  
      i=0;
      for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi,++i) if(!(*vi).IsD())
      {
        if(area[i]<=std::numeric_limits<float>::epsilon())
          (*vi).Q() = 0;
        else
          (*vi).Q() /= area[i];      
      }
      delete[] area;
      return hmmq;    
    }
    
    i=0;
    for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi,++i) if(!(*vi).IsD())
    {
      if(area[i]<=std::numeric_limits<float>::epsilon())
        (*vi).Q() = 0;
      else
        (*vi).Q() /= area[i];
        
      if ((*vi).Q() < hmmq.minQ) hmmq.minQ = (*vi).Q();
      if ((*vi).Q() > hmmq.maxQ) hmmq.maxQ = (*vi).Q();   
    }
      
    histo.SetRange(hmmq.minQ, hmmq.maxQ, histo_range);
    
    for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi) if(!(*vi).IsD())
      histo.Add((*vi).Q());
    
    hmmq.minQ = histo.Percentile(histo_frac);
    hmmq.maxQ = histo.Percentile(1.0f - histo_frac);
      
    for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi) if(!(*vi).IsD())
      (*vi).Q() = math::Clamp((*vi).Q(), hmmq.minQ, hmmq.maxQ);

    delete[] area;

    return hmmq;
    
  }

  template<class MESH_TYPE> void ColorGaussian(MESH_TYPE &m, GLLogStream *log)
  {
    histoMinMaxQ result = HK(m,false,true);

    if (log)
      log->Log(GLLogStream::Info, "Gaussian Curvature: minQ=%f maxQ=%f", result.minQ, result.maxQ);
    		
  }

  template<class MESH_TYPE> void ColorMean(MESH_TYPE &m, GLLogStream *log)
  {
    histoMinMaxQ result = HK(m,true,true);

    if (log)
      log->Log(GLLogStream::Info, "Mean Curvature: minQ=%f maxQ=%f", result.minQ, result.maxQ);
    
  }

  template<class MESH_TYPE> void ColorRMS(MESH_TYPE &m, GLLogStream *log)
  {
    float *HH4 = new float[m.vn];
    int i;
    typename MESH_TYPE::VertexIterator vi;
    
    vcg::Histogram<float> histo;
    float histo_frac=DEFAULT_HISTO_FRAC;
    int histo_range=DEFAULT_HISTO_RANGE;
    float minQ = std::numeric_limits<float>::max();
    float maxQ = -std::numeric_limits<float>::max();

    //Compute 4*(H^2)
	  HK(m,true,false);
    
    i=0;
    for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi,++i) if(!(*vi).IsD())
      HH4[i] = 4.0f * powf((*vi).Q(),2.0f);

    //Compute K
	  HK(m,false,false);

    //Compute sqrt(HH4-2K)
    i=0;
    for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi,++i) if(!(*vi).IsD())
    {
      (*vi).Q()=math::Sqrt(HH4[i] - ( (*vi).Q() * 2.0) );
      
      if ((*vi).Q() < minQ) minQ = (*vi).Q();
      if ((*vi).Q() > maxQ) maxQ = (*vi).Q();   
    }

    histo.SetRange(minQ, maxQ, histo_range);
    
    for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi) if(!(*vi).IsD())
      histo.Add((*vi).Q());
    
    minQ = histo.Percentile(histo_frac);
    maxQ = histo.Percentile(1.0f - histo_frac);
      
    for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi) if(!(*vi).IsD())
      (*vi).Q() = math::Clamp((*vi).Q(), minQ, maxQ);

    if (log)
      log->Log(GLLogStream::Info, "RMS Curvature: minQ=%f maxQ=%f", minQ, maxQ);
    
    delete[] HH4;

  }

  template<class MESH_TYPE> void ColorAbsolute(MESH_TYPE &m, GLLogStream *log)
  {
    float *H = new float[m.vn];
    int i;
    typename MESH_TYPE::VertexIterator vi;
    
    vcg::Histogram<float> histo;
    float histo_frac=DEFAULT_HISTO_FRAC;
    int histo_range=DEFAULT_HISTO_RANGE;
    float minQ = std::numeric_limits<float>::max();
    float maxQ = -std::numeric_limits<float>::max();

    //Compute H
	  HK(m,true,false);
    
    i=0;
    for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi,++i) if(!(*vi).IsD())
      H[i] = (*vi).Q();

    //Compute K
	  HK(m,false,false);

    //Compute abs(H+sqrt(H*H-K)) + abs(H-sqrt(H*H-K))
    i=0;
    for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi,++i) if(!(*vi).IsD())
    {
      (*vi).Q() = math::Abs(H[i] + math::Sqrt(powf(H[i], 2.0f) - (*vi).Q())) + math::Abs(H[i] - math::Sqrt(powf(H[i], 2.0f) - (*vi).Q()));
      
      if ((*vi).Q() < minQ) minQ = (*vi).Q();
      if ((*vi).Q() > maxQ) maxQ = (*vi).Q();   
    }

    histo.SetRange(minQ, maxQ, histo_range);
    
    for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi) if(!(*vi).IsD())
      histo.Add((*vi).Q());
    
    minQ = histo.Percentile(histo_frac);
    maxQ = histo.Percentile(1.0f - histo_frac);
      
    for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi) if(!(*vi).IsD())
      (*vi).Q() = math::Clamp((*vi).Q(), minQ, maxQ);

    if (log)
      log->Log(GLLogStream::Info, "Absolute Curvature: minQ=%f maxQ=%f", minQ, maxQ);
    
    delete[] H;

  }
}

#endif