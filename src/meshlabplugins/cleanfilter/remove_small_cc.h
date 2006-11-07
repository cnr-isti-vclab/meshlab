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
/****************************************************************************
  History

 $Log$
 Revision 1.1  2006/11/07 09:09:28  cignoni
 First Working release, moved in from epoch svn

 Revision 1.1  2006/01/20 13:03:27  cignoni
 *** empty log message ***

*****************************************************************************/
#ifndef __VCGLIB__REMOVE_SMALL_CC
#define __VCGLIB__REMOVE_SMALL_CC

#include <vcg/space/point3.h>
#include <vcg/complex/trimesh/clean.h>

namespace vcg
{
template<class MeshType>
void RemoveSmallConnectedComponentsSize(MeshType &m, int maxCCSize)
{
  std::vector< std::pair<int, typename MeshType::FacePointer> > CCV;
      int tt=vcg::tri::Clean<MeshType>::ConnectedComponents(m, CCV); 

      tri::ConnectedIterator<MeshType> ci;
      for(unsigned int i=0;i<CCV.size();++i)
      {
        vector<typename MeshType::FacePointer> FPV;
        if(CCV[i].first<maxCCSize)
        {
          for(ci.start(m,CCV[i].second);!ci.completed();++ci)
            FPV.push_back(*ci);
          
          typename vector<typename MeshType::FacePointer>::iterator fpvi;
          for(fpvi=FPV.begin(); fpvi!=FPV.end(); ++fpvi)
            (**fpvi).SetD();
          m.fn-=FPV.size();
        }
      }
}

template<class MeshType>
void RemoveSmallConnectedComponentsDiameter(MeshType &m, typename MeshType::ScalarType maxDiameter)
{
  std::vector< std::pair<int, typename MeshType::FacePointer> > CCV;
      int tt=vcg::tri::Clean<MeshType>::ConnectedComponents(m, CCV); 

      tri::ConnectedIterator<MeshType> ci;
      for(unsigned int i=0;i<CCV.size();++i)
      {
        Box3f bb;
        vector<typename MeshType::FacePointer> FPV;
        for(ci.start(m,CCV[i].second);!ci.completed();++ci)
        {
            FPV.push_back(*ci);
            bb.Add((*ci)->P(0));
            bb.Add((*ci)->P(1));
            bb.Add((*ci)->P(2));
        } 
        if(bb.Diag()<maxDiameter)
        {
          typename vector<typename MeshType::FacePointer>::iterator fpvi;
          for(fpvi=FPV.begin(); fpvi!=FPV.end(); ++fpvi)
            (**fpvi).SetD();
          m.fn-=FPV.size();
        }
      }
}
} //end namespace
#endif
