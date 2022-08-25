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

#ifndef SPLATRENDERERPLUGIN_H
#define SPLATRENDERERPLUGIN_H

#include <QObject>
#include <common//interfaces.h>
#include <wrap/gl/splatting_apss/splatrenderer.h>

class QGLFramebufferObject;

class SplatRendererPlugin : public QObject, public MeshRenderInterface
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(MESH_RENDER_INTERFACE_IID)
	Q_INTERFACES(MeshRenderInterface)

	SplatRenderer<CMeshO> splat_renderer;
 
	QList <QAction *> actionList;

public:

	SplatRendererPlugin();

	QList<QAction *> actions ()
	{
		if(actionList.isEmpty()) initActionList();
		return actionList;
	}

	void initActionList();

	bool isSupported() {return splat_renderer.isSupported();}
	void Init(QAction *a, MeshDocument &m, QMap<int,RenderMode>& rm, QGLWidget *gla);
	void Render(QAction *a, MeshDocument &m, QMap<int,RenderMode>& rm, QGLWidget *gla);
	void Finalize(QAction * /*mode*/, MeshDocument */*m*/, GLArea * /*parent*/) ;

};

#endif

