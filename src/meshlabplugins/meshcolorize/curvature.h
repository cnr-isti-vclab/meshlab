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
Revision 1.3  2006/01/31 15:59:00  vannini
Severe bugfix in mean curvature computation

Revision 1.2  2006/01/31 10:54:28  vannini
curvature<>color mapping now ignores border vertex

Revision 1.1  2006/01/27 18:27:53  vannini
code refactoring for curvature colorize
added colorize equalizer dialog and
"Colorize by Quality" filter
some small bugfixes
removed color_curvature.h in favour of curvature.h


****************************************************************************/
#ifndef CURVATURE_H
#define CURVATURE_H

#include <vcg/container/simple_temporary_data.h>
#include <vcg/complex/trimesh/base.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/space/triangle3.h>
#include <vcg/complex/trimesh/update/topology.h>
#include <vcg/math/histogram.h>

#define DEFAULT_HISTO_FRAC 0.05f
#define DEFAULT_HISTO_RANGE 10000

namespace vcg 
{
  class CurvData
  {
  public:
    float H;
    float K;
  };

  class Frange
  {
  public:
    float min;
    float max;
  };

  template <class MESH_TYPE> class Curvature
  {

    typedef typename MESH_TYPE::FaceIterator FaceIterator;
    typedef typename MESH_TYPE::VertexIterator VertexIterator;
    typedef typename MESH_TYPE::VertContainer VertContainer;

  private:
    MESH_TYPE *ms;
    SimpleTempData<VertContainer, CurvData> *TDPtr;

    void ComputeHK() 
    {
      float *areaH,*areaK;
      int i;
      float area0, area1, area2, angle0, angle1, angle2, e01, e12, e20, t;
      FaceIterator fi;
      VertexIterator vi;
	
      //Calcola AreaMix in H (vale anche per K)
      for(vi=(*ms).vert.begin(); vi!=(*ms).vert.end(); ++vi) if(!(*vi).IsD() && !(*vi).IsB())
        (*TDPtr)[*vi].H=0;

      for(fi=(*ms).face.begin();fi!=(*ms).face.end();++fi) if( !(*fi).IsD())
      {

        // angles
        angle0 = math::Abs(Angle(	(*fi).P(1)-(*fi).P(0),(*fi).P(2)-(*fi).P(0) ));
        angle1 = math::Abs(Angle(	(*fi).P(0)-(*fi).P(1),(*fi).P(2)-(*fi).P(1) ));
        angle2 = M_PI-(angle0+angle1);
        
        if((angle0 > M_PI/2) || (angle1 > M_PI/2) || (angle2 > M_PI/2))  // triangolo non ottuso
        { 
	        float e01 = SquaredDistance( (*fi).V(1)->P() , (*fi).V(0)->P() );
	        float e12 = SquaredDistance( (*fi).V(2)->P() , (*fi).V(1)->P() );
	        float e20 = SquaredDistance( (*fi).V(0)->P() , (*fi).V(2)->P() );
      	
          area0 = ( e20*(1.0/tan(angle1)) + e01*(1.0/tan(angle2)) ) / 8.0;
	        area1 = ( e01*(1.0/tan(angle2)) + e12*(1.0/tan(angle0)) ) / 8.0;
	        area2 = ( e12*(1.0/tan(angle0)) + e20*(1.0/tan(angle1)) ) / 8.0;
      	
	        (*TDPtr)[(*fi).V(0)].H  += area0;
	        (*TDPtr)[(*fi).V(1)].H  += area1;
	        (*TDPtr)[(*fi).V(2)].H  += area2;
	      }
        else // triangolo ottuso
	      { 
	        (*TDPtr)[(*fi).V(0)].H += vcg::DoubleArea<CFaceO>((*fi)) / 6.0;
	        (*TDPtr)[(*fi).V(1)].H += vcg::DoubleArea<CFaceO>((*fi)) / 6.0;
	        (*TDPtr)[(*fi).V(2)].H += vcg::DoubleArea<CFaceO>((*fi)) / 6.0;      
	      }
      }   

      i = 0;
      areaH = new float[(*ms).vn];
      areaK = new float[(*ms).vn];
      for(vi=(*ms).vert.begin(); vi!=(*ms).vert.end(); ++vi,++i) if(!(*vi).IsD() && !(*vi).IsB())
      {
        areaH[i] = areaK[i] = (*TDPtr)[*vi].H;    //Areamix è comune a H e a K
        (*TDPtr)[*vi].H = 0;
        (*TDPtr)[*vi].K = (float)(2.0 * M_PI);

      }
      
      for(fi=(*ms).face.begin();fi!=(*ms).face.end();++fi) if( !(*fi).IsD() )
      {    
        angle0 = math::Abs(Angle(	(*fi).P(1)-(*fi).P(0),(*fi).P(2)-(*fi).P(0) ));
        angle1 = math::Abs(Angle(	(*fi).P(0)-(*fi).P(1),(*fi).P(2)-(*fi).P(1) ));
        angle2 = M_PI-(angle0+angle1);
        
        e01 = ( (*fi).V(1)->P() - (*fi).V(0)->P() ) * (*fi).V(0)->N();
        e12 = ( (*fi).V(2)->P() - (*fi).V(1)->P() ) * (*fi).V(1)->N();
        e20 = ( (*fi).V(0)->P() - (*fi).V(2)->P() ) * (*fi).V(2)->N();
        
        area0 = ( e20 * (1.0/tan(angle1)) + e01 * (1.0/tan(angle2)) ) / 2.0;
	      area1 = ( e01*(1.0/tan(angle2)) + e12*(1.0/tan(angle0)) ) / 2.0;
	      area2 = ( e12*(1.0/tan(angle0)) + e20*(1.0/tan(angle1)) ) / 2.0;
          
        (*TDPtr)[(*fi).V(0)].H  += area0;
	      (*TDPtr)[(*fi).V(1)].H  += area1;
	      (*TDPtr)[(*fi).V(2)].H  += area2;

        (*TDPtr)[(*fi).V(0)].K -= angle0;
        (*TDPtr)[(*fi).V(1)].K -= angle1;
        (*TDPtr)[(*fi).V(2)].K -= angle2;
      }
      
      i=0;
      for(vi=(*ms).vert.begin(); vi!=(*ms).vert.end(); ++vi,++i) if(!(*vi).IsD() && !(*vi).IsB())
      {
        if(areaH[i]<=std::numeric_limits<float>::epsilon())
          (*TDPtr)[*vi].H = 0;
        else
          (*TDPtr)[*vi].H /= areaH[i];

        if(areaK[i]<=std::numeric_limits<float>::epsilon())
          (*TDPtr)[*vi].K = 0;
        else
          (*TDPtr)[*vi].K /= areaK[i];
      }

      delete[] areaH;
      delete[] areaK;
    }

  public:
    Curvature(MESH_TYPE &mt):ms(&mt)
    {
      TDPtr = new SimpleTempData<VertContainer, CurvData>((*ms).vert);
      (*TDPtr).Start(CurvData());
      ComputeHK();
    }

    ~Curvature()
    {
      (*TDPtr).Stop();
    }


    void MapGaussianCurvatureIntoQuality()
    { 
      VertexIterator vi;

      for(vi=(*ms).vert.begin(); vi!=(*ms).vert.end(); ++vi) if(!(*vi).IsD() && !(*vi).IsB()) 
        (*vi).Q() = (*TDPtr)[*vi].K;
    }


    void MapMeanCurvatureIntoQuality()
    {
      VertexIterator vi;
      for(vi=(*ms).vert.begin(); vi!=(*ms).vert.end(); ++vi) if(!(*vi).IsD() && !(*vi).IsB()) 
        (*vi).Q() = (*TDPtr)[*vi].H;
    }


    void MapRMSCurvatureIntoQuality()
    {
      VertexIterator vi;

      //Compute sqrt(4*H^2-2K)
	    for(vi=(*ms).vert.begin(); vi!=(*ms).vert.end(); ++vi) if(!(*vi).IsD() && !(*vi).IsB())
        (*vi).Q()=math::Sqrt((4.0f * powf((*TDPtr)[*vi].H, 2.0f)) - ((*TDPtr)[*vi].K * 2.0f));
    }

    void MapAbsoluteCurvatureIntoQuality()
    {
      VertexIterator vi;
      float t;

      //Compute abs(H+sqrt(H*H-K)) + abs(H-sqrt(H*H-K))
	    for(vi=(*ms).vert.begin(); vi!=(*ms).vert.end(); ++vi) if(!(*vi).IsD() && !(*vi).IsB())
      {
        t=math::Sqrt(powf((*TDPtr)[*vi].H, 2.0f) - (*TDPtr)[*vi].K);
        (*vi).Q()= math::Abs((*TDPtr)[*vi].H + t) + math::Abs((*TDPtr)[*vi].H - t);
      }
    }

    Frange minMaxQ()
    {
      VertexIterator vi;
      Frange r;
      r.min=std::numeric_limits<float>::max();
      r.max=-std::numeric_limits<float>::max();

      for(vi=(*ms).vert.begin(); vi!=(*ms).vert.end(); ++vi) if(!(*vi).IsD() && !(*vi).IsB())
      {
        if ((*vi).Q() < r.min) r.min = (*vi).Q();
        if ((*vi).Q() > r.max) r.max = (*vi).Q();   
      }
    
      return r;

    }

    Frange histoPercentile(Frange Q, float histo_frac=DEFAULT_HISTO_FRAC, int histo_range=DEFAULT_HISTO_RANGE)
    {
      VertexIterator vi;
      vcg::Histogram<float> histo;
      
      histo.SetRange(Q.min, Q.max, histo_range);
      
      for(vi=(*ms).vert.begin(); vi!=(*ms).vert.end(); ++vi) if(!(*vi).IsD() && !(*vi).IsB()) 
        histo.Add((*vi).Q());
        
      Q.min = histo.Percentile(histo_frac);
      Q.max = histo.Percentile(1.0f - histo_frac);

      return Q;
    }


    void ColorizeByEqualizedQuality(Frange P)
    {
      VertexIterator vi;
              
      for(vi=(*ms).vert.begin(); vi!=(*ms).vert.end(); ++vi) if(!(*vi).IsD() && !(*vi).IsB()) 
        (*vi).C().ColorRamp(P.min, P.max, (*vi).Q());
      
    }

  };
}
#endif // CURVATURE_H