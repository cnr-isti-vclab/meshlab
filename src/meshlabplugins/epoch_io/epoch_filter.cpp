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
 Revision 1.2  2006/11/27 06:57:19  cignoni
 Wrong way of using the __DATE__ preprocessor symbol

 Revision 1.1  2006/11/07 18:14:21  cignoni
 Moved from the epoch svn repository

 Revision 1.1  2006/01/20 13:03:27  cignoni
 *** empty log message ***

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
#include <vcg/math/matrix33.h>
#include<vcg/complex/trimesh/append.h>

#include <QMessageBox>
#include <QFileDialog>
//#include <vcg/complex/trimesh/update/normal.h>

#include "epoch_io.h"

using namespace vcg;

EpochFilter::EpochFilter() {
	actionList << new QAction(ST(FP_MERGE), this);
	actionList << new QAction(ST(FP_REMOVE_ISOLATED), this);
}

const QString EpochFilter::ST(FilterType filter) {

 switch(filter)
  {
	  case FP_MERGE :		return QString("Merge to a single surface");
	  case FP_REMOVE_ISOLATED :		return QString("Remove isolated triangles");
  	default: assert(0);
  }
  return QString("error!");
}

EpochFilter::~EpochFilter() {
	for (int i = 0; i < actionList.count() ; i++ ) {
		delete actionList.at(i);
	}
}

QList<QAction *> EpochFilter::actions() const {
	return actionList;
}


const ActionInfo &EpochFilter::Info(QAction *action)
{
	static ActionInfo ai;

	if( action->text() == ST(FP_MERGE) )
		{
			ai.Help = tr("Merge");
			ai.ShortHelp = tr("Merge");
		}
	if( action->text() == ST(FP_REMOVE_ISOLATED) )
	  {
			ai.Help = tr("Remove Isolated");
			ai.ShortHelp = tr("Remove Isolated");
		}
   return ai;
}

const PluginInfo &EpochFilter::Info()
{
   static PluginInfo ai;
   ai.Date=tr(__DATE__);
	 ai.Version = tr("0.1");
	 ai.Author = ("Paolo Cignoni");
   return ai;
}

bool EpochFilter::applyFilter(QAction *filter, MeshModel &m, QWidget *parent, vcg::CallBackPos *cb)
{
	if(filter->text() == ST(FP_MERGE) )
	  {
	  }
	if(filter->text() == ST(FP_REMOVE_ISOLATED) )
	  {
	  }
	return true;
}

Q_EXPORT_PLUGIN(EpochFilter)
