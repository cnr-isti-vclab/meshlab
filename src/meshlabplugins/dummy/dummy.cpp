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
Revision 1.6  2005/12/12 11:22:19  cignoni
commented a parameter name to remove a warning

Revision 1.5  2005/12/04 10:51:30  glvertex
Minor changes

Revision 1.4  2005/12/03 22:49:46  cignoni
Added copyright info

****************************************************************************/
#include <QtGui>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <meshlab/glarea.h>

#include "dummy.h"

using namespace vcg;

void DummyPlugin::Render(QAction *a, MeshModel &/*m*/, RenderMode &rm, GLArea * /*gla*/)
{
	if(a->text() == "action 1")
	{
		rm.colorMode = GLW::CMNone;
		return;
	}
	
	if(a->text() == "action 2")
	{
		rm.colorMode = GLW::CMPerVert;
		return;
	}

	if(a->text() == "action 3")
	{
		rm.colorMode = GLW::CMPerFace;
		return;
	}

}

Q_EXPORT_PLUGIN(DummyPlugin)