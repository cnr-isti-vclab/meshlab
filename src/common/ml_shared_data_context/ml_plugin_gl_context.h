/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2021                                           \/)\/    *
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

#ifndef ML_PLUGIN_GL_CONTEXT_H
#define ML_PLUGIN_GL_CONTEXT_H

#include "ml_scene_gl_shared_data_context.h"

class MLPluginGLContext : public QGLContext
{
public:
	MLPluginGLContext(
			const QGLFormat& frmt,
			QPaintDevice* dvc,
			MLSceneGLSharedDataContext& shared);
	~MLPluginGLContext();

	void initPerViewRenderingData(int meshid, MLRenderingData& dt);
	void removePerViewRenderindData();
	void setRenderingData(int meshid, MLRenderingData& dt);
	void drawMeshModel(int meshid) const;
	void meshAttributesUpdated(
			int meshid,
			bool conntectivitychanged,
			const MLRenderingData::RendAtts& dt);

	static void smoothModalitySuggestedRenderingData(MLRenderingData& dt);
	static void pointModalitySuggestedRenderingData(MLRenderingData& dt);
private:
	MLSceneGLSharedDataContext& _shared;
};

#endif // ML_PLUGIN_GL_CONTEXT_H
