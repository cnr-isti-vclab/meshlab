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
$Log$
Revision 1.2  2006/06/16 01:26:07  cignoni
Added Initial Filter Script Dialog

Revision 1.1  2006/06/15 13:05:57  cignoni
added Filter History Dialogs

****************************************************************************/

#include "filterScriptDialog.h"

//using namespace vcg;


FilterScriptDialog::FilterScriptDialog(QWidget * parent)
		:QDialog(parent)
{
  FilterScriptDialog::ui.setupUi(this);
  scriptPtr=0;
  connect(ui.clearScriptButton,SIGNAL(clicked()), this, SLOT(clearScript()));
  connect(ui.saveScriptButton, SIGNAL(clicked()), this, SLOT(saveScript()));
  connect(ui.openScriptButton, SIGNAL(clicked()), this, SLOT(openScript()));
}

void FilterScriptDialog::setScript(FilterScript *scr)
{
	scriptPtr=scr;
  FilterScript::iterator li;
  ui.scriptListWidget->clear();
  
  for(li=scr->actionList.begin();li!=scr->actionList.end() ;++li)
     ui.scriptListWidget->addItem((*li).first);
}

void FilterScriptDialog::clearScript()
{
  assert(scriptPtr);
  scriptPtr->actionList.clear();
  ui.scriptListWidget->clear();
 
}


void FilterScriptDialog::saveScript()
{
  scriptPtr->save("Prova.xml");
}

void FilterScriptDialog::openScript()
{
  scriptPtr->open("Prova.xml");
}
