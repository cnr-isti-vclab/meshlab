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

#include "edit_paint_factory.h"
#include "edit_paint.h"

EditPaintFactory::EditPaintFactory()
{
	editPaint = new QAction(QIcon(":/images/paintbrush-22.png"), "Z-painting", this);

	actionList.push_back(editPaint);
	
	foreach(QAction *editAction, actionList)
		editAction->setCheckable(true); 	
}

QString EditPaintFactory::pluginName() const
{
	return "EditPaint";
}

//get the edit tool for the given action
EditTool* EditPaintFactory::getEditTool(const QAction *action)
{
	if(action == editPaint) {
		return new EditPaintPlugin();
	}
	else
		assert(0); //should never be asked for an action that isn't here
	return NULL;
}

QString EditPaintFactory::getEditToolDescription(const QAction *)
{
	return EditPaintPlugin::info();
}

MESHLAB_PLUGIN_NAME_EXPORTER(EditPaintFactory)
