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
Revision 1.4  2005/12/03 22:49:46  cignoni
Added copyright info

****************************************************************************/
#include <QtGui>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <meshlab/glarea.h>

#include "dummy.h"

using namespace vcg;

void DummyPlugin::Render(QAction *a, MeshModel &m, RenderMode &rm, GLArea *gla)
{
	if(a->text() == "action 1")
	{
		rm.drawColor = GLW::CMNone;
		return;
	}
	
	if(a->text() == "action 2")
	{
		rm.drawColor = GLW::CMPerVert;
		return;
	}

	if(a->text() == "action 2")
	{
		rm.drawColor = GLW::CMPerFace;
		return;
	}

}

Q_EXPORT_PLUGIN(DummyPlugin)