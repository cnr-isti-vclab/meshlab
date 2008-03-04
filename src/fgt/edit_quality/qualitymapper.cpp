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
Revision 1.0 2008/02/20 Alessandro Maione, Federico Bellucci
FIRST RELEASE

****************************************************************************/

#include <QtGui>
#include <limits>
#include <meshlab/glarea.h>
#include "qualitymapper.h"
#include <vcg/space/triangle3.h> //for quality

using namespace vcg;

QualityMapperPlugin::QualityMapperPlugin()
{
	//setting-up plugin
	actionList << new QAction(QIcon(":/images/qualitymapper.png"),"Quality Mapper", this);
	QAction *editAction;
	foreach(editAction, actionList)
		editAction->setCheckable(true);

	//initializing dialog pointer
	_qualityMapperDialog = 0;
}

QList<QAction *> QualityMapperPlugin::actions() const
{	return actionList;	}


const QString QualityMapperPlugin::Info(QAction *action) 
{
	if( action->text() != tr("Get Info") ) assert (0);

	return tr("Colorize mesh vertexes by Quality");
}

const PluginInfo &QualityMapperPlugin::Info() 
{
	static PluginInfo ai; 
	ai.Date=tr("Jan 2008");
	ai.Version = tr("1.0");
	ai.Author = ("Alessandro Maione, Federico Bellucci");
	return ai;
} 

void QualityMapperPlugin::StartEdit(QAction *mode, MeshModel& m, GLArea *gla )
{
	if(_qualityMapperDialog==0)
		_qualityMapperDialog = new QualityMapperDialog(gla->window(), m, gla);

	//drawing histogram
	//bool ret = _qualityMapperDialog->initEqualizerHistogram();
	if ( !_qualityMapperDialog->initEqualizerHistogram() )
	{
		EndEdit(mode, m, gla);
		return;
	}

	//drawing transferFunction
	_qualityMapperDialog->drawTransferFunction();

	//dialog ready to be displayed. Show it now!
	_qualityMapperDialog->show();

	connect(_qualityMapperDialog, SIGNAL(closingDialog()),gla,SLOT(endEdit()) );
}

void QualityMapperPlugin::EndEdit(QAction * , MeshModel &, GLArea * )
{
	//if a dialog exists, it's time to destroy it
	if ( _qualityMapperDialog )
	{
		//disconnecting...
		_qualityMapperDialog->disconnect();
		//...and deleting dialog
		delete _qualityMapperDialog;
		_qualityMapperDialog = 0;
	}
}


Q_EXPORT_PLUGIN(QualityMapperPlugin)

