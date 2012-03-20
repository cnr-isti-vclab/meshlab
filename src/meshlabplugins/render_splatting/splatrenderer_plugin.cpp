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

#include <QtGui>

#include <math.h>
#include <stdlib.h>
#include <iostream>
#include "splatrenderer_plugin.h"

#include <QGLWidget>
#include <QTextStream>

#include <QGLFramebufferObject>


SplatRendererPlugin::SplatRendererPlugin()
{
	splat_renderer.Clear();
	
}

void SplatRendererPlugin::initActionList()
{
	actionList << new QAction("Splatting", this);
}


void SplatRendererPlugin::Init(QAction * a, MeshDocument & md, RenderMode &, QGLWidget *gla)
{
	if (!(md.mm()->hasDataMask(MeshModel::MM_VERTRADIUS)))
	{
		QMessageBox::warning(0, tr("Splat Render Failure"), QString("Warning the current mesh <font color=red>'" + md.mm()->fullName() + "'</font> cannot not be rendered.<br><i>Radius per vertex attribute</i> must be present."));
		return;
	}
	splat_renderer.Init(gla);
}

void SplatRendererPlugin::Render(QAction *, MeshDocument &md, RenderMode &rm, QGLWidget * /* gla */)
{
	GL_TEST_ERR

		std::vector<CMeshO*> meshes_to_render;
		foreach(MeshModel * mp, md.meshList)
			{
				meshes_to_render.push_back( &(*mp).cm );
			}
		splat_renderer.Render(meshes_to_render,rm.colorMode,rm.textureMode);


}

void SplatRendererPlugin::Finalize(QAction * /*mode*/, MeshDocument */* &*//*m*/, GLArea * /*parent*/) {
  splat_renderer.Destroy();
}

#if 0
void SplatRendererPlugin::Draw(QAction *a, MeshModel &m, RenderMode &rm, QGLWidget * gla)
{
	if (m.cm.vert.RadiusEnabled)
	{
		if (mCurrentPass==2)
			return;

		enablePass(mCurrentPass);
		/*if (mCurrentPass==1)*/ drawSplats(m, rm);
	}
	else if (mCurrentPass==2)
	{
		MeshRenderInterface::Draw(a, m, rm, gla);
	}
}
#endif


Q_EXPORT_PLUGIN(SplatRendererPlugin)
