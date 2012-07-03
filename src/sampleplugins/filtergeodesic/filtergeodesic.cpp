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

 filtergeodesic.cpp,v
 Revision 1.1  2007/12/13 00:33:55  cignoni
 New small samples

 Revision 1.1  2007/12/02 07:57:48  cignoni
 Added the new sample filter plugin that removes border faces

 
*****************************************************************************/
#include <Qt>
#include <QtGui>

#include "filtergeodesic.h"

using namespace std;
using namespace vcg;

FilterGeodesic::FilterGeodesic() 
{
  typeList << FP_QUALITY_BORDER_GEODESIC
					 << FP_QUALITY_POINT_GEODESIC;
 
  FilterIDType tt;
  foreach(tt , types())
	    actionList << new QAction(filterName(tt), this);

}

FilterGeodesic::~FilterGeodesic() {
	for (int i = 0; i < actionList.count() ; i++ ) 
		delete actionList.at(i);
}

QString FilterGeodesic::filterName(FilterIDType filter) const
{
 switch(filter)
  {
	  case FP_QUALITY_BORDER_GEODESIC :								return QString("Colorize by border distance");
	  case FP_QUALITY_POINT_GEODESIC :								return QString("Colorize by distance from a given point");
  	default: assert(0);
  }
  return QString("error!");
}

 QString FilterGeodesic::filterInfo(FilterIDType filterId) const
{
  switch(filterId)
  {
		case FP_QUALITY_BORDER_GEODESIC:	   return tr("Store in the quality field the geodesic distance from borders and color the mesh accordingly."); 
		case FP_QUALITY_POINT_GEODESIC:	     return tr("Store in the quality field the geodesic distance from a given point and color the mesh accordingly."); 
  	default: assert(0);
  }
  return QString("error!");
}

 FilterGeodesic::FilterClass FilterGeodesic::getClass(QAction *a)
{
  switch(ID(a))
  {
    case FP_QUALITY_BORDER_GEODESIC :
    case FP_QUALITY_POINT_GEODESIC :
          return FilterGeodesic::FilterClass(MeshFilterInterface::VertexColoring + MeshFilterInterface::Quality);
    default : assert(0);
							return MeshFilterInterface::Generic;
  }
}

 int FilterGeodesic::getRequirements(QAction *action)
{
  switch(ID(action))
  {
		case FP_QUALITY_BORDER_GEODESIC :
	case FP_QUALITY_POINT_GEODESIC :	return MeshModel::MM_VERTFACETOPO ;
    default: assert(0);
  }
  return 0;
}

bool FilterGeodesic::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos * /*cb*/)
{
    MeshModel &m=*(md.mm());
    CMeshO::FaceIterator fi;
	CMeshO::VertexIterator vi;
	switch (ID(filter)) {
		case FP_QUALITY_POINT_GEODESIC:
			{
				m.updateDataMask(MeshModel::MM_VERTFACETOPO);
				m.updateDataMask(MeshModel::MM_VERTMARK);
				m.updateDataMask(MeshModel::MM_VERTQUALITY);
				m.updateDataMask(MeshModel::MM_VERTCOLOR);
				tri::UpdateFlags<CMeshO>::FaceBorderFromVF(m.cm);
				tri::UpdateFlags<CMeshO>::VertexBorderFromFace(m.cm);
				Point3f startPoint = par.getPoint3f("startPoint");
				// first search the closest point on the surface;
				CMeshO::VertexPointer startVertex=0;
				float minDist= std::numeric_limits<float>::max();

				for(vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi) if(!(*vi).IsD())
					if(SquaredDistance(startPoint,(*vi).P()) < minDist) {
						startVertex=&*vi;
						minDist=SquaredDistance(startPoint,(*vi).P());
						}
				
						
				Log("Input point is %f %f %f Closest on surf is %f %f %f",startPoint[0],startPoint[1],startPoint[2],startVertex->P()[0],startVertex->P()[1],startVertex->P()[2]);
						
				// Now actually compute the geodesic distnace from the closest point		
				tri::Geo<CMeshO> g;
				float dist_thr = par.getAbsPerc("maxDistance");
				g.FarthestVertex(m.cm, startVertex,dist_thr);

				// Cleaning Quality value of the unrefernced vertices
				// Unreached vertexes has a quality that is maxfloat
				int unreachedCnt=0;
				float unreached  = std::numeric_limits<float>::max();
				for(vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi) if(!(*vi).IsD())
					if((*vi).Q() == unreached) {
							unreachedCnt++;
							(*vi).Q()=0;
						}
				if(unreachedCnt >0 )
						Log("Warning: %i vertices were unreacheable from the borders, probably your mesh has unreferenced vertices",unreachedCnt);
				
				tri::UpdateColor<CMeshO>::VertexQualityRamp(m.cm);
				
			}
			break;
		case FP_QUALITY_BORDER_GEODESIC:
			{
				m.updateDataMask(MeshModel::MM_VERTFACETOPO);
				m.updateDataMask(MeshModel::MM_VERTMARK);
				m.updateDataMask(MeshModel::MM_VERTQUALITY);
				m.updateDataMask(MeshModel::MM_VERTCOLOR);
				tri::UpdateFlags<CMeshO>::FaceBorderFromVF(m.cm);
				tri::UpdateFlags<CMeshO>::VertexBorderFromFace(m.cm);

				tri::Geo<CMeshO> g;
				bool ret = g.DistanceFromBorder(m.cm);

				// Cleaning Quality value of the unrefernced vertices
				// Unreached vertexes has a quality that is maxfloat
				int unreachedCnt=0;
				float unreached  = std::numeric_limits<float>::max();
				for(vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi) if(!(*vi).IsD())
					if((*vi).Q() == unreached) {
							unreachedCnt++;
							(*vi).Q()=0;
						}
				if(unreachedCnt >0 )
						Log("Warning: %i vertices were unreacheable from the borders, probably your mesh has unreferenced vertices",unreachedCnt);
				
				if(!ret) Log("Mesh Has no borders. No geodesic distance computed");
			    	else tri::UpdateColor<CMeshO>::VertexQualityRamp(m.cm);
				}

			break;
		default: assert(0);
			break;
	}
return true;		
}

void FilterGeodesic::initParameterSet(QAction *action,MeshModel &m, RichParameterSet & parlst)
{
	switch(ID(action))	 
		{
			case FP_QUALITY_POINT_GEODESIC :
					parlst.addParam(new RichPoint3f("startPoint",m.cm.bbox.min,"Starting point","The starting point from which geodesic distance has to be computed. If it is not a surface vertex, the closest vertex to the specified point is used as starting seed point."));
          parlst.addParam(new RichAbsPerc("maxDistance",m.cm.bbox.Diag(),0,m.cm.bbox.Diag()*2,"Max Distance","If not zero it indicates a cut off value to be used during geodesic distance computation."));
					break;
    default: break; // do not add any parameter for the other filters
    }
	return;
}

int FilterGeodesic::postCondition(QAction * filter) const
{
  switch (ID(filter))
  {
    case FP_QUALITY_POINT_GEODESIC:  return MeshModel::MM_VERTCOLOR + MeshModel::MM_VERTQUALITY;
    default                  : return MeshModel::MM_UNKNOWN;
  }
}
Q_EXPORT_PLUGIN(FilterGeodesic)
