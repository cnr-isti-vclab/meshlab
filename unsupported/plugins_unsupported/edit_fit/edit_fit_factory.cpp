/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

#include "edit_fit_factory.h"
#include "editfit.h"

EditFitFactory::EditFitFactory()
{
	editFit =  new QAction(QIcon(":/images/icon_measure.png"),"Fitting Tool", this);

	actionList << editFit;
	
	foreach(QAction *editAction, actionList)
		editAction->setCheckable(true); 	
}
	
//gets a list of actions available from this plugin
QList<QAction *> EditFitFactory::actions() const
{
	return actionList;
}

//get the edit tool for the given action
MeshEditInterface* EditFitFactory::getMeshEditInterface(QAction *action)
{
	if(action == editFit)
	{
		return new EditFitPlugin();
	} else assert(0); //should never be asked for an action that isnt here
}

const QString getEditToolDescription(QAction *)
{
	return EditFitPlugin::Info();
}

Q_EXPORT_PLUGIN(EditFitFactory)
