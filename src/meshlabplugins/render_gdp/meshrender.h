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

#ifndef SHADERRENDERPLUGIN_H
#define SHADERRENDERPLUGIN_H

#include <common/gl_defs.h>
#include <common/interfaces.h>
#include "textfile.h"
#include "shaderStructs.h"
#include "shaderDialog.h"


class MeshShaderRenderPlugin : public QObject, public MeshRenderInterface
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(MESH_RENDER_INTERFACE_IID)
	Q_INTERFACES(MeshRenderInterface)

	GLhandleARB v;
	GLhandleARB f;

	std::map<QString, ShaderInfo> shaders;

	bool supported;
	QList <QAction *> actionList;

	ShaderDialog *sDialog;

public:

	MeshShaderRenderPlugin()
	{

		supported = false;
		sDialog = 0;
	}

	QList<QAction *> actions() 
	{
		if (actionList.isEmpty()) initActionList();
		return actionList;
	}

	void initActionList();

	bool isSupported() { return supported; }

	void Init(QAction *, MeshDocument &, MLSceneGLSharedDataContext::PerMeshRenderingDataMap& , GLArea *);
	void Render(QAction *, MeshDocument &, MLSceneGLSharedDataContext::PerMeshRenderingDataMap&, GLArea *);
	void Finalize(QAction*, MeshDocument*, GLArea*);

};

#endif

