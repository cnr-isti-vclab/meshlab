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

#ifndef MESHLAB_RENDER_PLUGIN_H
#define MESHLAB_RENDER_PLUGIN_H

#include "meshlab_plugin.h"
#include "meshlab_plugin_file.h"
#include "../../ml_shared_data_context/ml_scene_gl_shared_data_context.h"

/**
 * @brief The RenderPlugin class is used to customize the rendering 
 * process of the whole MeshDocument.
 *
 * The Render function is called in with the ModelView and Projection Matrices 
 * already set up, screen cleared and background drawn.
 * After the Render call the MeshLab frawework draw on the opengl context other 
 * decorations and the trackball, so it there is the requirement for a rendering 
 * plugin is that it should leave the z-buffer in a coherent state.
 * 
 * The typical rendering loop of a Render plugin is something like, :
 *
 * <your own opengl setup>
 * 
 * foreach(MeshModel * mp, meshDoc.meshList)
 * {
 *     if(mp->visible) mp->Render(rm.drawMode,rm.colorMode,rm.textureMode);
 * }
 */

class GLArea;

class RenderPlugin : virtual public MeshLabPluginFile, virtual public MeshLabPlugin
{
public:
	RenderPlugin() :MeshLabPlugin() {}
	virtual ~RenderPlugin() {}

	virtual bool isSupported() = 0;
	virtual QList<QAction *> actions() = 0;

	virtual void init(QAction *, MeshDocument &, MLSceneGLSharedDataContext::PerMeshRenderingDataMap& /*mp*/, GLArea *) {}
	virtual void render(QAction *, MeshDocument &, MLSceneGLSharedDataContext::PerMeshRenderingDataMap& mp, GLArea *) = 0;
	virtual void finalize(QAction *, MeshDocument *, GLArea *) {}

	/** 
	 * this function is called whenever MeshLab should update GUI's menus filled
	 * with the actions of your plugin. 
	 * Reimplement this if your actions depend on something that may change during
	 * a MeshLab session (e.g. new shader files have been loaded and need to be
	 * added to the MeshLab interface)
	 */
	virtual void refreshActions() {}
};

#define RENDER_PLUGIN_IID  "vcg.meshlab.RenderPlugin/1.0"
Q_DECLARE_INTERFACE(RenderPlugin, RENDER_PLUGIN_IID)

#endif // MESHLAB_RENDER_PLUGIN_INTERFACE_H
