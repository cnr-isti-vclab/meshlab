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
Revision 1.1  2006/02/01 16:23:09  vannini
Added "smooth color" filter


****************************************************************************/

#ifndef SMOOTHCOLOR_H
#define SMOOTHCOLOR_H

#include<vcg/complex/trimesh/base.h>
#include<vcg/complex/trimesh/clean.h>
#include<vcg/complex/trimesh/update/topology.h>
#include<math.h>
#include <QtGlobal>

namespace vcg{

  class ColorSmoothInfo 
  {
  public:
	  unsigned int r;
    unsigned int g;
    unsigned int b;
    unsigned int a;
    int cnt;
  };

  template<class MESH_TYPE> void LaplacianSmoothColor(MESH_TYPE &m, int step, bool SmoothSelected=false)
  { 
	  SimpleTempData<typename MESH_TYPE::VertContainer, ColorSmoothInfo> TD(m.vert);
    ColorSmoothInfo csi;
	  csi.r=0; csi.g=0; csi.b=0; csi.cnt=0;

	  TD.Start(csi);
	  for(int i=0;i<step;++i)
	  {
		  typename MESH_TYPE::VertexIterator vi;
		  for(vi=m.vert.begin();vi!=m.vert.end();++vi)
			  TD[*vi]=csi;

		  typename MESH_TYPE::FaceIterator fi;
		  for(fi=m.face.begin();fi!=m.face.end();++fi)
			  if(!(*fi).IsD()) 
				  for(int j=0;j<3;++j)
					  if(!(*fi).IsB(j)) 
						  {
							  TD[(*fi).V(j)].r+=(*fi).V1(j)->C()[0];
                TD[(*fi).V(j)].g+=(*fi).V1(j)->C()[1];
                TD[(*fi).V(j)].b+=(*fi).V1(j)->C()[2];
                TD[(*fi).V(j)].a+=(*fi).V1(j)->C()[3];

                TD[(*fi).V1(j)].r+=(*fi).V(j)->C()[0];
                TD[(*fi).V1(j)].g+=(*fi).V(j)->C()[1];
                TD[(*fi).V1(j)].b+=(*fi).V(j)->C()[2];		  
                TD[(*fi).V1(j)].a+=(*fi).V(j)->C()[3];		  

							  ++TD[(*fi).V(j)].cnt;
							  ++TD[(*fi).V1(j)].cnt;
					  }

			  // si azzaera i dati per i vertici di bordo
			  for(fi=m.face.begin();fi!=m.face.end();++fi)
				  if(!(*fi).IsD()) 
					  for(int j=0;j<3;++j)
						  if((*fi).IsB(j))
							  {
								  TD[(*fi).V(j)]=csi;
								  TD[(*fi).V1(j)]=csi;
							  }

			  // se l'edge j e' di bordo si deve mediare solo con gli adiacenti
			  for(fi=m.face.begin();fi!=m.face.end();++fi)
				  if(!(*fi).IsD()) 
					  for(int j=0;j<3;++j)
						  if((*fi).IsB(j)) 
							  {
								  TD[(*fi).V(j)].r+=(*fi).V1(j)->C()[0];
                  TD[(*fi).V(j)].g+=(*fi).V1(j)->C()[1];
                  TD[(*fi).V(j)].b+=(*fi).V1(j)->C()[2];
                  TD[(*fi).V(j)].a+=(*fi).V1(j)->C()[3];

                  TD[(*fi).V1(j)].r+=(*fi).V(j)->C()[0];
                  TD[(*fi).V1(j)].g+=(*fi).V(j)->C()[1];
                  TD[(*fi).V1(j)].b+=(*fi).V(j)->C()[2];		  
                  TD[(*fi).V1(j)].a+=(*fi).V(j)->C()[3];

							    ++TD[(*fi).V(j)].cnt;
							    ++TD[(*fi).V1(j)].cnt;
						  }

	  for(vi=m.vert.begin();vi!=m.vert.end();++vi)
		  if(!(*vi).IsD() && TD[*vi].cnt>0 )
			  if(!SmoothSelected || (*vi).IsS())
        {
          (*vi).C()[0] = (unsigned int) ceil((double) (TD[*vi].r / TD[*vi].cnt));
          (*vi).C()[1] = (unsigned int) ceil((double) (TD[*vi].g / TD[*vi].cnt));
          (*vi).C()[2] = (unsigned int) ceil((double) (TD[*vi].b / TD[*vi].cnt));
          (*vi).C()[3] = (unsigned int) ceil((double) (TD[*vi].a / TD[*vi].cnt));
        }
	  }
  		 	
	  TD.Stop();
  };
}
#endif