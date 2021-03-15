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

#include "edit_align_factory.h"
#include "edit_align.h"

EditAlignFactory::EditAlignFactory()
{
	editAlign = new QAction(QIcon(":/images/icon_align.png"), "Align", this);

	actionList.push_back(editAlign);
	
	foreach(QAction *editAction, actionList)
		editAction->setCheckable(true); 	
}

QString EditAlignFactory::pluginName() const
{
	return "EditAlign";
}

//get the edit tool for the given action
EditPlugin* EditAlignFactory::getEditTool(const QAction *action)
{
	assert(action == editAlign); (void) action;
	return new EditAlignPlugin();
}

QString EditAlignFactory::getEditToolDescription(const QAction *)
{
	return EditAlignPlugin::info();
}

MESHLAB_PLUGIN_NAME_EXPORTER(EditAlignFactory)
