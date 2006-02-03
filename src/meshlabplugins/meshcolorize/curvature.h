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
Revision 1.7  2006/02/03 17:42:48  vannini
bugfix & code indentation

Revision 1.6  2006/02/01 16:23:09  vannini
Added "smooth color" filter

Revision 1.5  2006/01/31 16:36:13  vannini
Removed two arrays of floats in favour of one SimpleTempData for temporary area values

Revision 1.4  2006/01/31 16:22:10  vannini
Small bugfix

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

  class AreaData
  {
  public:
    float A;
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
    SimpleTempData<VertContainer, CurvData> *TDCurvPtr;
    SimpleTempData<VertContainer, AreaData> *TDAreaPtr;

    void ComputeHK() 
    {
      float area0, area1, area2, angle0, angle1, angle2, e01, e12, e20;
      FaceIterator fi;
      VertexIterator vi;
	
      //Calcola AreaMix in H (vale anche per K)
      for(vi=(*ms).vert.begin(); vi!=(*ms).vert.end(); ++vi) if(!(*vi).IsD())
        (*TDCurvPtr)[*vi].H=0;

      for(fi=(*ms).face.begin();fi!=(*ms).face.end();++fi) if( !(*fi).IsD())
      {
        // angles
        angle0 = math::Abs(Angle(	(*fi).P(1)-(*fi).P(0),(*fi).P(2)-(*fi).P(0) ));
        angle1 = math::Abs(Angle(	(*fi).P(0)-(*fi).P(1),(*fi).P(2)-(*fi).P(1) ));
        angle2 = M_PI-(angle0+angle1);
        
        if((angle0 < M_PI/2) && (angle1 < M_PI/2) && (angle2 < M_PI/2))  // triangolo non ottuso
        { 
	        float e01 = SquaredDistance( (*fi).V(1)->P() , (*fi).V(0)->P() );
	        float e12 = SquaredDistance( (*fi).V(2)->P() , (*fi).V(1)->P() );
	        float e20 = SquaredDistance( (*fi).V(0)->P() , (*fi).V(2)->P() );
      	
          area0 = ( e20*(1.0/tan(angle1)) + e01*(1.0/tan(angle2)) ) / 8.0;
	        area1 = ( e01*(1.0/tan(angle2)) + e12*(1.0/tan(angle0)) ) / 8.0;
	        area2 = ( e12*(1.0/tan(angle0)) + e20*(1.0/tan(angle1)) ) / 8.0;
      	
	        (*TDCurvPtr)[(*fi).V(0)].H  += area0;
	        (*TDCurvPtr)[(*fi).V(1)].H  += area1;
	        (*TDCurvPtr)[(*fi).V(2)].H  += area2;
	      }
        else // triangolo ottuso
	      { 
	        (*TDCurvPtr)[(*fi).V(0)].H += vcg::DoubleArea<CFaceO>((*fi)) / 6.0;
	        (*TDCurvPtr)[(*fi).V(1)].H += vcg::DoubleArea<CFaceO>((*fi)) / 6.0;
	        (*TDCurvPtr)[(*fi).V(2)].H += vcg::DoubleArea<CFaceO>((*fi)) / 6.0;      
	      }
      }   

      for(vi=(*ms).vert.begin(); vi!=(*ms).vert.end(); ++vi) if(!(*vi).IsD())
      {
        (*TDAreaPtr)[*vi].A = (*TDCurvPtr)[*vi].H;      //Areamix è comune a H e a K
        (*TDCurvPtr)[*vi].H = 0;
        (*TDCurvPtr)[*vi].K = (float)(2.0 * M_PI);
      }
      
      for(fi=(*ms).face.begin();fi!=(*ms).face.end();++fi) if( !(*fi).IsD() )
      {    
        angle0 = math::Abs(Angle(	(*fi).P(1)-(*fi).P(0),(*fi).P(2)-(*fi).P(0) ));
        angle1 = math::Abs(Angle(	(*fi).P(0)-(*fi).P(1),(*fi).P(2)-(*fi).P(1) ));
        angle2 = M_PI-(angle0+angle1);
        
        e01 = ( (*fi).V(1)->P() - (*fi).V(0)->P() ) * (*fi).V(0)->N();
        e12 = ( (*fi).V(2)->P() - (*fi).V(1)->P() ) * (*fi).V(1)->N();
        e20 = ( (*fi).V(0)->P() - (*fi).V(2)->P() ) * (*fi).V(2)->N();
        
        area0 = ( e20 * (1.0/tan(angle1)) + e01 * (1.0/tan(angle2)) ) / 4.0;
	      area1 = ( e01 * (1.0/tan(angle2)) + e12 * (1.0/tan(angle0)) ) / 4.0;
	      area2 = ( e12 * (1.0/tan(angle0)) + e20 * (1.0/tan(angle1)) ) / 4.0;
          
        (*TDCurvPtr)[(*fi).V(0)].H  += area0;
	      (*TDCurvPtr)[(*fi).V(1)].H  += area1;
	      (*TDCurvPtr)[(*fi).V(2)].H  += area2;

        (*TDCurvPtr)[(*fi).V(0)].K -= angle0;
        (*TDCurvPtr)[(*fi).V(1)].K -= angle1;
        (*TDCurvPtr)[(*fi).V(2)].K -= angle2;
      }
      
      for(vi=(*ms).vert.begin(); vi!=(*ms).vert.end(); ++vi) if(!(*vi).IsD() && !(*vi).IsB())
      {
        if((*TDAreaPtr)[*vi].A<=std::numeric_limits<float>::epsilon())
        {
          (*TDCurvPtr)[*vi].H = 0;
          (*TDCurvPtr)[*vi].K = 0;
        }
        else
        {
          (*TDCurvPtr)[*vi].H /= (*TDAreaPtr)[*vi].A;
          (*TDCurvPtr)[*vi].K /= (*TDAreaPtr)[*vi].A;
        }
      }
    }

  public:
    Curvature(MESH_TYPE &mt):ms(&mt)
    {
      TDCurvPtr = new SimpleTempData<VertContainer, CurvData>((*ms).vert);
      (*TDCurvPtr).Start(CurvData());
      TDAreaPtr = new SimpleTempData<VertContainer, AreaData>((*ms).vert);
      (*TDAreaPtr).Start(AreaData());

      ComputeHK();
    }

    ~Curvature()
    {
      (*TDCurvPtr).Stop();
      (*TDAreaPtr).Stop();
    }


    void MapGaussianCurvatureIntoQuality()
    { 
      VertexIterator vi;

      for(vi=(*ms).vert.begin(); vi!=(*ms).vert.end(); ++vi) if(!(*vi).IsD() && !(*vi).IsB()) 
        (*vi).Q() = (*TDCurvPtr)[*vi].K;
    }


    void MapMeanCurvatureIntoQuality()
    {
      VertexIterator vi;
      for(vi=(*ms).vert.begin(); vi!=(*ms).vert.end(); ++vi) if(!(*vi).IsD() && !(*vi).IsB()) 
        (*vi).Q() = (*TDCurvPtr)[*vi].H;
    }


    void MapRMSCurvatureIntoQuality()
    {
      VertexIterator vi;

      //Compute sqrt(4*H^2-2K)
	    for(vi=(*ms).vert.begin(); vi!=(*ms).vert.end(); ++vi) if(!(*vi).IsD() && !(*vi).IsB())
        (*vi).Q()=math::Sqrt((4.0f * powf((*TDCurvPtr)[*vi].H, 2.0f)) - ((*TDCurvPtr)[*vi].K * 2.0f));
    }

    void MapAbsoluteCurvatureIntoQuality()
    {
      VertexIterator vi;
      float t;

      //Compute abs(H+sqrt(H*H-K)) + abs(H-sqrt(H*H-K))
	    for(vi=(*ms).vert.begin(); vi!=(*ms).vert.end(); ++vi) if(!(*vi).IsD() && !(*vi).IsB())
      {
        t=math::Sqrt(powf((*TDCurvPtr)[*vi].H, 2.0f) - (*TDCurvPtr)[*vi].K);
        (*vi).Q()= math::Abs((*TDCurvPtr)[*vi].H + t) + math::Abs((*TDCurvPtr)[*vi].H - t);
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