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
Revision 1.1  2006/06/15 13:05:57  cignoni
added Filter History Dialogs

Revision 1.7  2006/01/16 05:34:16  cignoni
Added backward qt4.0 compatibility for setAutoFillBackground

Revision 1.6  2006/01/15 15:27:59  glvertex
Added few lines to set background even in qt 4.1

Revision 1.5  2006/01/02 18:54:52  glvertex
added multilevel logging support

Revision 1.4  2006/01/02 17:39:18  glvertex
Added info types in a combobox

Revision 1.3  2005/12/04 16:50:15  glvertex
Removed [using namespace] directive form .h
Renaming in QT style
Adapted method behavior to the new ui interface

Revision 1.2  2005/12/03 22:49:46  cignoni
Added copyright info

****************************************************************************/

#include "filterScriptDialog.h"

//using namespace vcg;


FilterScriptDialog::FilterScriptDialog(QWidget * parent)
		:QDialog(parent)
{
  FilterScriptDialog::ui.setupUi(this);
  scriptPtr=0;
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
}
