/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004                                                \/)\/    *
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
Revision 1.4  2006/12/06 21:24:00  cignoni
Completed NonManifoldVertex

Revision 1.3  2006/12/05 15:37:27  cignoni
Added rough version of non manifold vertex coloring

Revision 1.2  2006/02/15 05:32:34  cignoni
Now it colors also non manifold faces

Revision 1.1  2006/01/06 11:14:44  giec
Change location meshfilter to meshcolorize.
----------------------------------------------------------------------
----------------------------------------------------------------------

Revision 1.1  2005/12/30 10:15:41  giec
Filter that it colors the edge non manifold



****************************************************************************/
#ifndef __VCGLIB_COLOR_MANIFOLD
#define __VCGLIB_COLOR_MANIFOLD

#include<vcg/complex/trimesh/base.h>
#include <vcg/complex/trimesh/clean.h>
#include<vcg/space/triangle3.h>
#include<vcg/complex/trimesh/update/topology.h>
#include<vcg/complex/trimesh/update/color.h>

#include <iostream>
#include <QtGlobal>

//using std::cout;
//using std::endl;

namespace vcg{

	template<class MESH_TYPE>
		void ColorManifoldFace(MESH_TYPE &m)
	{	
		assert(m.HasFFTopology());

		typename MESH_TYPE::VertexIterator vi;
		typename MESH_TYPE::FaceIterator fi;
   
    tri::UpdateColor<MESH_TYPE>::VertexConstant(m, Color4b::White);

		// Test per gli edge manifold
		for (fi = m.face.begin(); fi != m.face.end(); ++fi)	if (!fi->IsD())
			{
        for(int i=0;i<3;++i)
        if(!IsManifold(*fi,i)){
					(*fi).V0(i)->C().SetRGB(255,0,0);
					(*fi).V1(i)->C().SetRGB(255,0,0);
          }
		  }
	}

  template<class MESH_TYPE>
		void ColorManifoldVertex(MESH_TYPE &m)
	{	
		assert(m.HasFFTopology());

		typename MESH_TYPE::VertexIterator vi;
		typename MESH_TYPE::FaceIterator fi;
   
    tri::UpdateColor<MESH_TYPE>::VertexConstant(m, Color4b::White);

    SimpleTempData<typename MESH_TYPE::VertContainer, int > TD(m.vert);

  	// primo loop, si conta quanti facce incidono su ogni vertice...

    TD.Start(0);
    for (fi = m.face.begin(); fi != m.face.end(); ++fi)	if (!fi->IsD())
    {
      TD[(*fi).V(0)]++;
      TD[(*fi).V(1)]++;
      TD[(*fi).V(2)]++;
    }

    for (vi = m.vert.begin(); vi != m.vert.end(); ++vi)	if (!vi->IsD())
      (*vi).ClearV();
    
    for (fi = m.face.begin(); fi != m.face.end(); ++fi)	if (!fi->IsD())
    {
      
      for(int i=0;i<3;i++) if(!(*fi).V(i)->IsV()){
        (*fi).V(i)->SetV();
        face::Pos<typename MESH_TYPE::FaceType> sp(&(*fi),i);
        face::Pos<typename MESH_TYPE::FaceType> ip=sp;
        bool borderfound = false;
        int facenumber = 0;
        do 
        {
          if(!ip.IsManifold()) break;
          if(ip.IsBorder())
            borderfound = true;
          ip.FlipF();
          ip.FlipE();
          facenumber++;
        } while(ip!=sp);
        
        if(borderfound && ip.IsManifold()){
          assert((facenumber%2)==0);
          facenumber=facenumber/2;
        }
        if(TD[(*fi).V(i)]!=facenumber || !(ip.IsManifold())){
          (*fi).V(i)->C()=Color4b::Red;
        
        }
      
      }
    }

    
    TD.Stop();
	}


} // end namespace
#endif