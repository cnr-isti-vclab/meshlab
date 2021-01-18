/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2020                                           \/)\/    *
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

#ifndef MESHLAB_RENDER_PLUGIN_INTERFACE_H
#define MESHLAB_RENDER_PLUGIN_INTERFACE_H

#include "plugin_interface.h"
#include "../ml_shared_data_context.h"

/**
RenderPluginInterface
Used to customized the rendering process.
Rendering plugins are now responsible of the rendering of the whole MeshDocument and not only of a single MeshModel.

The Render function is called in with the ModelView and Projection Matrices already set up, screen cleared and background drawn.
After the Render call the MeshLab frawework draw on the opengl context other decorations and the trackball, so it there is the
requirement for a rendering plugin is that it should leave the z-buffer in a coherent state.

The typical rendering loop of a Render plugin is something like, :

<your own opengl setup>

foreach(MeshModel * mp, meshDoc.meshList)
{
if(mp->visible) mp->Render(rm.drawMode,rm.colorMode,rm.textureMode);
}

*/

class GLArea;

class RenderPluginInterface : public PluginInterface
{
public:
	RenderPluginInterface() :PluginInterface() {}
	virtual ~RenderPluginInterface() {}

	virtual void Init(QAction *, MeshDocument &, MLSceneGLSharedDataContext::PerMeshRenderingDataMap& /*mp*/, GLArea *) {}
	virtual void Render(QAction *, MeshDocument &, MLSceneGLSharedDataContext::PerMeshRenderingDataMap& mp, GLArea *) = 0;
	virtual void Finalize(QAction *, MeshDocument *, GLArea *) {}
	virtual bool isSupported() = 0;
	virtual QList<QAction *> actions() = 0;
};

#define MESHLAB_PLUGIN_IID_EXPORTER(x) Q_PLUGIN_METADATA(IID x)
#define MESHLAB_PLUGIN_NAME_EXPORTER(x)

#define RENDER_PLUGIN_INTERFACE_IID  "vcg.meshlab.RenderPluginInterface/1.0"
Q_DECLARE_INTERFACE(RenderPluginInterface, RENDER_PLUGIN_INTERFACE_IID)

#endif // MESHLAB_RENDER_PLUGIN_INTERFACE_H
