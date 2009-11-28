/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2008                                           \/)\/    *
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

#include "edit_slice_factory.h"
#include "editslice.h"

EditSliceFactory::EditSliceFactory()
{
	editSlice = new QAction(QIcon(":/images/iconslice.png"),"Slice mesh geometry", this);
	
	actionList << editSlice;
	
	foreach(QAction *editAction, actionList)
		editAction->setCheckable(true); 	
}
	
//gets a list of actions available from this plugin
QList<QAction *> EditSliceFactory::actions() const
{
	return actionList;
}

//get the edit tool for the given action
MeshEditInterface* EditSliceFactory::getMeshEditInterface(QAction *action)
{
	if(action == editSlice)
	{
		return new ExtraMeshSlidePlugin();
	} else assert(0); //should never be asked for an action that isnt here
}

QString EditSliceFactory::getEditToolDescription(QAction *)
{
	return ExtraMeshSlidePlugin::Info();
}

Q_EXPORT_PLUGIN(EditSliceFactory)
