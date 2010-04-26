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

#include "edit_vase_factory.h"
#include "edit_vase.h"

EditVaseFactory::EditVaseFactory()
{
    editVase = new QAction(QIcon(":/images/vase.png"),"Volume Aware Surface Extraction", this);

    actionList << editVase;
	
	foreach(QAction *editAction, actionList)
		editAction->setCheckable(true); 	
}
	
//gets a list of actions available from this plugin
QList<QAction *> EditVaseFactory::actions() const
{
	return actionList;
}

//get the edit tool for the given action
MeshEditInterface* EditVaseFactory::getMeshEditInterface(QAction *action)
{
  if(action == editVase)
      return new EditVasePlugin();
	 
	assert(0); //should never be asked for an action that isnt here
  return 0;
}

QString EditVaseFactory::getEditToolDescription(QAction */*a*/)
{
  return EditVasePlugin::Info();
}

Q_EXPORT_PLUGIN(EditVaseFactory)
