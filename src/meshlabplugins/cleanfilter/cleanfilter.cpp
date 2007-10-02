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
 Revision 1.16  2007/10/02 08:13:38  cignoni
 New filter interface. Hopefully more clean and easy to use.

 Revision 1.15  2007/06/11 15:26:43  ponchio
 *** empty log message ***

 Revision 1.14  2007/05/30 15:10:54  ponchio
 *** empty log message ***

 Revision 1.13  2007/05/22 15:26:02  cignoni
 Improved params of ball pivoting (again)

 Revision 1.12  2007/05/22 15:16:43  cignoni
 Improved params of ball pivoting

 Revision 1.11  2007/04/16 09:25:28  cignoni
 ** big change **
 Added Layers managemnt.
 Interfaces are changing again...

 Revision 1.10  2007/03/20 16:23:07  cignoni
 Big small change in accessing mesh interface. First step toward layers

 Revision 1.9  2007/02/08 23:46:15  pirosu
 merged srcpar and par in the GetStdParameters() function

 Revision 1.8  2007/01/11 19:52:25  pirosu
 fixed bug for QT 4.1.0/dotnet2003
 removed the request of the window title to the plugin. The action description is used instead.

 Revision 1.7  2006/12/27 21:41:58  pirosu
 Added improvements for the standard plugin window:
 split of the apply button in two buttons:ok and apply
 added support for parameters with absolute and percentage values

 Revision 1.6  2006/12/13 17:37:27  pirosu
 Added standard plugin window support

 
 Revision 1.5  2006/11/29 00:59:15  cignoni
 Cleaned plugins interface; changed useless help class into a plain string

 Revision 1.4  2006/11/27 06:57:19  cignoni
 Wrong way of using the __DATE__ preprocessor symbol

 Revision 1.3  2006/11/07 17:26:01  cignoni
 small gcc compiling issues

 Revision 1.2  2006/11/07 14:56:23  zifnab1974
 Changes for compilation with gcc 3.4.6 on linux AMD64

 Revision 1.1  2006/11/07 09:09:27  cignoni
 First Working release, moved in from epoch svn

 Revision 1.1  2006/01/20 13:03:27  cignoni
 *** empty log message ***

*****************************************************************************/
#include <Qt>
#include <QtGui>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>
#include <vcg/math/matrix33.h>

#include "cleanfilter.h"
#include "remove_small_cc.h"


//#include <wrap/io_trimesh/io_mask.h>
#include <vcg/complex/trimesh/create/platonic.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/flag.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/stat.h>
#include <vcg/complex/trimesh/create/ball_pivoting.h>

#include <vcg/space/normal_extrapolation.h>

using namespace vcg;

CleanFilter::CleanFilter() 
{
  typeList << FP_REBUILD_SURFACE << FP_REMOVE_WRT_Q << FP_REMOVE_ISOLATED_COMPLEXITY << FP_REMOVE_ISOLATED_DIAMETER;
 
  FilterIDType tt;
  foreach(tt , types())
	    actionList << new QAction(filterName(tt), this);

	maxDiag1=0;
	maxDiag2=-1;
	minCC=25;
	val1=1.0;

}

CleanFilter::~CleanFilter() {
	for (int i = 0; i < actionList.count() ; i++ ) 
		delete actionList.at(i);
}

const QString CleanFilter::filterName(FilterIDType filter) 
{
 switch(filter)
  {
	  case FP_REBUILD_SURFACE :								return QString("Build surface from points");
	  case FP_REMOVE_WRT_Q :									return QString("Remove vertices wrt quality");
	  case FP_REMOVE_ISOLATED_DIAMETER   :		return QString("Remove isolated pieces (wrt diameter)");
	  case FP_REMOVE_ISOLATED_COMPLEXITY :		return QString("Remove isolated pieces (wrt face num)");
  	default: assert(0);
  }
  return QString("error!");
}

const QString CleanFilter::filterInfo(FilterIDType filterId)
{
  switch(filterId)
  {
		case FP_REBUILD_SURFACE :	return QString("Merge"); 
		case FP_REMOVE_ISOLATED_COMPLEXITY:	 return tr("Remove isolated connected components composed by a limited number of triangles"); 
		case FP_REMOVE_ISOLATED_DIAMETER:	 return tr("Remove isolated connected components whose diameter is smaller than the specified constant"); 
		case FP_REMOVE_WRT_Q:	     return tr("Remove all the vertices with a quality lower smaller than the specified constant"); 
  	default: assert(0);
  }
  return QString("error!");
}

const CleanFilter::FilterClass CleanFilter::getClass(QAction *a)
{
  switch(ID(a))
  {
    case FP_REMOVE_WRT_Q :
    case FP_REMOVE_ISOLATED_DIAMETER :
    case FP_REMOVE_ISOLATED_COMPLEXITY :
      return MeshFilterInterface::Cleaning;     
    default : return MeshFilterInterface::Generic;
  }
}


const PluginInfo &CleanFilter::pluginInfo()
{
   static PluginInfo ai;
   ai.Date=tr( __DATE__ );
	 ai.Version = tr("0.1");
	 ai.Author = ("Paolo Cignoni");
   return ai;
}

const int CleanFilter::getRequirements(QAction *action)
{
  switch(ID(action))
  {
    case FP_REMOVE_WRT_Q:
    case FP_REBUILD_SURFACE :	return MeshModel::MM_BORDERFLAG;
	  case FP_REMOVE_ISOLATED_COMPLEXITY:
    case FP_REMOVE_ISOLATED_DIAMETER:
        return MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG | MeshModel::MM_FACEMARK;
    default: assert(0);
  }
  return 0;
}

void CleanFilter::initParameterSet(QAction *action,MeshModel &m, FilterParameterSet & parlst)
{ 
	pair<float,float> qualityRange;
  switch(ID(action))
  {
    case FP_REBUILD_SURFACE :
		  parlst.addFloat("BallRadius",(float)maxDiag1,"Enter ball size as a diag perc. (0 autoguess))");
		  parlst.addFloat("Clustering",30.0f,"Enter clustering radius (as ball size percent)");		  
		  parlst.addFloat("CreaseThr", 90.0f,"Angle Threshold (degrees)");
//		  parlst.addBool("ComputeNormal","Compute the per vertex normals using only the point set ",false);
		  parlst.addBool("DeleteFaces",false,"Delete intial set of faces");
		  break;
    case FP_REMOVE_ISOLATED_DIAMETER:	 
		  parlst.addAbsPerc("MinComponentDiag",m.cm.bbox.Diag()/10.0,0,m.cm.bbox.Diag(),"Enter max diameter of isolated pieces","All the connected components with a smaller diameter will be deleted");
		  break;
    case FP_REMOVE_ISOLATED_COMPLEXITY:	 
		  parlst.addInt("MinComponentSize",(int)minCC,"Enter minimum conn. comp size:");
		  break;
    case FP_REMOVE_WRT_Q:
			 qualityRange=tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(m.cm);
		  parlst.addAbsPerc("MaxQualityThr",(float)val1, qualityRange.first, qualityRange.second,"Delete all vertices with quality under:");
		  break;
	default: assert(0);
  }
}



bool CleanFilter::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & par, vcg::CallBackPos * cb) 
{
	if(filter->text() == filterName(FP_REBUILD_SURFACE) )
	  {
      float Radius = par.getFloat("BallRadius");		
      float Clustering = par.getFloat("Clustering");		      
//			bool ComputeNormal = par.getBool("ComputeNormal");
			float CreaseThr = par.getFloat("CreaseThr");
			bool DeleteFaces = par.getBool("DeleteFaces");

      if(DeleteFaces) {
				m.cm.fn=0;
				m.cm.face.resize(0);
      }
			
/*			if(ComputeNormal)
				NormalExtrapolation<vector<CVertexO> >::ExtrapolateNormals(m.cm.vert.begin(), m.cm.vert.end(), 10,-1,NormalExtrapolation<vector<CVertexO> >::IsCorrect,  cb);
      
		*/
		  CreaseThr *= M_PI/180;
		  Clustering /= 100;
			tri::BallPivoting<CMeshO> pivot(m.cm, Radius, Clustering, CreaseThr); 
      // the main processing
      pivot.BuildMesh(cb);
      m.clearDataMask(MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG);
	  }
    if(filter->text() == filterName(FP_REMOVE_ISOLATED_DIAMETER) )
	  {
      float minCC= par.getAbsPerc("MinComponentDiag");		
      RemoveSmallConnectedComponentsDiameter<CMeshO>(m.cm,minCC);
    }  	

    if(filter->text() == filterName(FP_REMOVE_ISOLATED_COMPLEXITY) )
	  {
      float minCC= par.getInt("MinComponentSize");		
      RemoveSmallConnectedComponentsSize<CMeshO>(m.cm,minCC);
	  }
	if(filter->text() == filterName(FP_REMOVE_WRT_Q) )
	  {
      float val=par.getAbsPerc("MaxQualityThr");		
      CMeshO::VertexIterator vi;
      for(vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi)
        if(!(*vi).IsD() && (*vi).Q()<val)
        {
          (*vi).SetD();
          m.cm.vn--;
        } 
      CMeshO::FaceIterator fi;
      for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi) if(!(*fi).IsD())
               if((*fi).V(0)->IsD() ||(*fi).V(1)->IsD() ||(*fi).V(2)->IsD() ) 
       {
        (*fi).SetD();
        --m.cm.fn;
       }

	  }
	return true;
}


Q_EXPORT_PLUGIN(CleanFilter)
