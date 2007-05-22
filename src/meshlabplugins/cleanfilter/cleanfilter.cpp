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
#include <vcg/complex/trimesh/create/ball_pivoting.h>

#include <vcg/space/normal_extrapolation.h>

using namespace vcg;

CleanFilter::CleanFilter() 
{
  typeList << FP_REBUILD_SURFACE << FP_REMOVE_WRT_Q << FP_REMOVE_ISOLATED_COMPLEXITY << FP_REMOVE_ISOLATED_DIAMETER;
 
  FilterType tt;
  foreach(tt , types())
	    actionList << new QAction(ST(tt), this);

    maxDiag1=0;
	maxDiag2=10;
	minCC=25;
	val1=1.0;

}

CleanFilter::~CleanFilter() {
	for (int i = 0; i < actionList.count() ; i++ ) 
		delete actionList.at(i);
}

const QString CleanFilter::ST(FilterType filter) 
{
 switch(filter)
  {
	  case FP_REBUILD_SURFACE :		return QString("Build surface from points");
	  case FP_REMOVE_WRT_Q :		return QString("Remove Faces wrt quality");
	  case FP_REMOVE_ISOLATED_DIAMETER   :		return QString("Remove isolated pieces (wrt diameter)");
	  case FP_REMOVE_ISOLATED_COMPLEXITY :		return QString("Remove isolated pieces (wrt face num)");
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

const QString CleanFilter::Info(FilterType filterId)
{
  switch(filterId)
  {
  case FP_REBUILD_SURFACE :	return QString("Merge"); 
	case FP_REMOVE_ISOLATED_COMPLEXITY:	 return tr("Remove Isolated"); 
	case FP_REMOVE_ISOLATED_DIAMETER:	 return tr("Remove Isolated"); 
	case FP_REMOVE_WRT_Q:	     return tr("Remove all the faces with quality lower than..."); 
  }
}

const PluginInfo &CleanFilter::Info()
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
    case FP_REBUILD_SURFACE :	return 0;
	  case FP_REMOVE_ISOLATED_COMPLEXITY:
    case FP_REMOVE_ISOLATED_DIAMETER:
        return MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG | MeshModel::MM_FACEMARK;
    default: assert(0);
  }
  return 0;
}

bool CleanFilter::getStdFields(QAction *action, MeshModel &m, StdParList &parlst)
{ 
  switch(ID(action))
  {
    case FP_REBUILD_SURFACE :
		  parlst.addFieldFloat("BallRadius","Enter ball size as a diag perc. (0 autoguess))",(float)maxDiag1);
		  parlst.addFieldFloat("CreaseThr","Angle Threshold",(float)maxDiag1);
		  parlst.addFieldBool("ComputeNormal","Compute the per vertex normals using only the point set ",false);
		  parlst.addFieldBool("DeleteFaces","Delete intial set of faces",false);
		  break;
    case FP_REMOVE_ISOLATED_DIAMETER:	 
		  parlst.addFieldFloat("MinComponentDiag","Enter size (as a diag perc 0..100)",(float)maxDiag2);
		  break;
    case FP_REMOVE_ISOLATED_COMPLEXITY:	 
		  parlst.addFieldInt("MinComponentSize","Enter minimum conn. comp size:",(int)minCC);
		  break;
    case FP_REMOVE_WRT_Q:
		  parlst.addFieldFloat("MaxQualityThr","Delete all Vertices with quality under:",(float)val1);
		  break;
	default:
		return false;
  }

  return true;
}

bool CleanFilter::getStdParameters(QAction *action, QWidget *parent, MeshModel &m,FilterParameter &par)
{
 return true;
}
bool CleanFilter::getParameters(QAction *action, QWidget *parent, MeshModel &m,FilterParameter &par)
{
 /*	 switch(ID(action))
  {
 case FP_REBUILD_SURFACE :
		 maxDiag1 = par.getFloat("BallRadius");
     ComputeNormalFlag=
			par.update("BallRadius",float(m.cm.bbox.Diag()*maxDiag1/100.0));
		 return true;
	 case FP_REMOVE_ISOLATED_DIAMETER:	 
		 maxDiag2 = par.getFloat("MinComponentDiag");
				  par.update("MinComponentDiag",float(m.cm.bbox.Diag()*maxDiag2/100.0));
				  return true;
	  case FP_REMOVE_ISOLATED_COMPLEXITY:	 
			  minCC = par.getInt("MinComponentSize");
				 return true;
	  case FP_REMOVE_WRT_Q:
				  val1 = par.getFloat("MaxQualityThr");
				  return true;
	
			}  
 return false;*/
 return false;
  }




bool CleanFilter::applyFilter(QAction *filter, MeshModel &m, FilterParameter & par, vcg::CallBackPos * cb) 
{
	if(filter->text() == ST(FP_REBUILD_SURFACE) )
	  {
      float radius = par.getFloat("BallRadius");		
			bool ComputeNormal = par.getBool("ComputeNormal");
			float CreaseThr = par.getFloat("CreaseThr");
			bool DeleteFaces = par.getBool("DeleteFaces");
      float clustering = 0.1;
      float crease=0;
      if(DeleteFaces) {
				m.cm.fn=0;
				m.cm.face.resize(0);
      }
			
			if(ComputeNormal)
				NormalExtrapolation<vector<CVertexO> >::ExtrapolateNormals(m.cm.vert.begin(), m.cm.vert.end(), 10,-1,NormalExtrapolation<vector<CVertexO> >::IsCorrect,  cb);
      
			tri::UpdateFlags<CMeshO>::FaceBorderFromNone(m.cm);
			tri::UpdateFlags<CMeshO>::VertexBorderFromFace(m.cm);
			
			tri::Pivot<CMeshO> pivot(m.cm, radius, clustering, crease); 
      // the main processing
      pivot.buildMesh(cb);
	  }
    if(filter->text() == ST(FP_REMOVE_ISOLATED_DIAMETER) )
	  {
      float minCC= par.getFloat("MinComponentDiag");		
      RemoveSmallConnectedComponentsDiameter<CMeshO>(m.cm,minCC);
    }  	

    if(filter->text() == ST(FP_REMOVE_ISOLATED_COMPLEXITY) )
	  {
      float minCC= par.getInt("MinComponentSize");		
      RemoveSmallConnectedComponentsSize<CMeshO>(m.cm,minCC);
	  }
	if(filter->text() == ST(FP_REMOVE_WRT_Q) )
	  {
      float val=par.getFloat("MaxQualityThr");		
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
