/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2007                                          \/)\/    *
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
 Revision 1.1  2007/02/09 13:50:10  cignoni
 First Non Working version

*****************************************************************************/
#include <Qt>
#include <QtGui>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>


// temporaneamente prendo la versione corrente dalla cartella test
#include<vcg/complex/trimesh/update/bounding.h>
#include <wrap/io_trimesh/io_mask.h>
#include <vcg/complex/trimesh/create/platonic.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/math/matrix33.h>
#include <vcg/complex/trimesh/create/ball_pivoting.h>

#include <QMessageBox>
#include <QFileDialog>

#include "cleanfilter.h"
#include "remove_small_cc.h"
#include <vcg/complex/trimesh/update/normal.h>
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

const QString CleanFilter::Info(QAction *action)
{
  switch(ID(action))
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
		  parlst.addField("BallRadius","Enter ball size as a diag perc. (0 autoguess))",(float)maxDiag1);
		  break;
    case FP_REMOVE_ISOLATED_DIAMETER:	 
		  parlst.addField("MinComponentDiag","Enter size (as a diag perc 0..100)",(float)maxDiag2);
		  break;
    case FP_REMOVE_ISOLATED_COMPLEXITY:	 
		  parlst.addField("MinComponentSize","Enter minimum conn. comp size:",(int)minCC);
		  break;
    case FP_REMOVE_WRT_Q:
		  parlst.addField("MaxQualityThr","Delete all Vertices with quality under:",(float)val1);
		  break;
	default:
		return false;
  }

  return true;
}

bool CleanFilter::getParameters(QAction *action, QWidget *parent, MeshModel &m,FilterParameter &par)
{
 switch(ID(action))
  {
	 case FP_REBUILD_SURFACE :
		 maxDiag1 = par.getFloat("BallRadius");
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
 return false;
  }




bool CleanFilter::applyFilter(QAction *filter, MeshModel &m, FilterParameter & par, vcg::CallBackPos * cb) 
{
	if(filter->text() == ST(FP_REBUILD_SURFACE) )
	  {
  	  }
 	return true;
}


Q_EXPORT_PLUGIN(CleanFilter)
