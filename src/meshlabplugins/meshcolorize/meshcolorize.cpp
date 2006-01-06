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
Revision 1.11  2006/01/06 11:15:26  giec
Added color non manifolt filter.

Revision 1.10  2006/01/04 13:27:53  alemochi
Added help in plugin dialog

Revision 1.9  2005/12/23 19:34:09  glvertex
Removed ColorModes (none,pervert,perface)

Revision 1.8  2005/12/19 16:49:14  cignoni
Added SelfIntersection and Border colorization methods

Revision 1.7  2005/12/12 22:48:42  cignoni
Added plugin info methods

Revision 1.6  2005/12/08 22:52:50  cignoni
Added safer min max search

Revision 1.5  2005/12/05 11:37:13  ggangemi
workaround: added rendermode to compute method

Revision 1.4  2005/12/03 23:23:37  ggangemi
new interface

Revision 1.3  2005/12/03 22:49:46  cignoni
Added copyright info

****************************************************************************/
#include <QtGui>

#include "meshcolorize.h"
#include <limits>
#include <vcg/complex/trimesh/clean.h>
#include "color_manifold.h"

using namespace vcg;


const ActionInfo &MeshColorCurvaturePlugin::Info(QAction *) 
 {
   static ActionInfo ai; 
   ai.Help=tr("Generic Help for an action");
   return ai;
 }

const PluginInfo &MeshColorCurvaturePlugin::Info() 
{
  static PluginInfo ai; 
  ai.Date=tr("__DATE__");
  return ai;
}

static void Gaussian(CMeshO &m){
  
  assert(m.HasPerVertexQuality());
  
  CMeshO::VertexIterator vi;		// iteratore vertice
  CMeshO::FaceIterator fi;		// iteratore facce
  float *area;					// areamix vector
  int i;							// index
  float area0, area1, area2;
  float angle0, angle1, angle2; 
  
  //--- Initialization
  area = new float[m.vn];
  
  //reset the values to 0
  for(vi=m.vert.begin();vi!=m.vert.end();++vi) if(!(*vi).IsD())
    (*vi).Q() = 0.0;
  
  //--- compute Areamix
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
  
  //cout << "Vertex count: " << i << endl;
  
  for(fi=m.face.begin();fi!=m.face.end();++fi)  if(!(*fi).IsD())
    {
      float angle0 = math::Abs(Angle(
				     (*fi).P(1)-(*fi).P(0),(*fi).P(2)-(*fi).P(0) ));
      float angle1 = math::Abs(Angle(
				     (*fi).P(0)-(*fi).P(1),(*fi).P(2)-(*fi).P(1) ));
      float angle2 = M_PI-(angle0+angle1);
      
      (*fi).V(0)->Q() -= angle0;
      (*fi).V(1)->Q() -= angle1;
      (*fi).V(2)->Q() -= angle2;
    }
  
  i=0;
  float histo_frac = 0.1f;
  int histo_range=100000;
  vcg::Histogram<float> histo;
  float minQ =  std::numeric_limits<float>::max(),
    maxQ = -std::numeric_limits<float>::max();
  for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi,++i) if(!(*vi).IsD())
    {
      if(area[i]<=std::numeric_limits<float>::epsilon()) 
	(*vi).Q() = 0;
      else
	(*vi).Q() /= area[i];
      
      if ((*vi).Q() < minQ) minQ = (*vi).Q();
      if ((*vi).Q() > maxQ) maxQ = (*vi).Q();
      
    }
  
  //cout << "min:" << min << " max:" << max << endl;
  
  histo.SetRange(minQ, maxQ, histo_range);
  
  for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi) if(!(*vi).IsD())
    {
      histo.Add((*vi).Q());
    } 
  
  minQ = histo.Percentile(histo_frac);
  maxQ = histo.Percentile(1.0f-histo_frac);
  
  //cout << "Histo: frac=" << histo_frac << " pmin=" << min << " pmax=" << max << "  range=" << histo_range << endl;
  
  for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi) if(!(*vi).IsD())
    {
      (*vi).Q() = math::Clamp((*vi).Q(), minQ, maxQ);
    }
  
  //--- DeInit
  
  delete[] area;
  
}

void MeshColorCurvaturePlugin::Compute(QAction * mode, MeshModel &m, RenderMode &rm, GLArea *parent){
  if(mode->text() == tr("Gaussian Curvature"))
    {
      Gaussian(m.cm);
      vcg::tri::UpdateColor<CMeshO>::VertexQuality(m.cm);
      rm.colorMode = GLW::CMPerVert;
      return;
    }

  if(mode->text() == tr("Self Intersections"))
    {
      vector<CFaceO *> IntersFace;
      tri::Clean<CMeshO>::SelfIntersections(m.cm,IntersFace);
      
      vector<CFaceO *>::iterator fpi;
      for(fpi=IntersFace.begin();fpi!=IntersFace.end();++fpi)
        (*fpi)->C()=Color4b::Red;
      
      rm.colorMode = GLW::CMPerFace;
      return;
    }

  if(mode->text() == tr("Border"))
    {
      vcg::tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
      vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromFF(m.cm);
      vcg::tri::UpdateFlags<CMeshO>::VertexBorderFromFace (m.cm);
      vcg::tri::UpdateColor<CMeshO>::VertexBorderFlag(m.cm);
      rm.colorMode = GLW::CMPerVert;
      return;
    }

  if(mode->text() == tr("Color non Manifold"))
  {
    vcg::tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
    ColorManifold<CMeshO>(m.cm);
  }
}

Q_EXPORT_PLUGIN(MeshColorCurvaturePlugin)
  
