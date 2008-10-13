/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2008                                           \/)\/    *
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

#ifndef RENDERRFX_H
#define RENDERRFX_H

#include <GL/glew.h>
#include <QGLWidget>
#include <QMapIterator>
#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include "rfx_uniform.h"
#include "rfx_shader.h"
#include "rfx_parser.h"
#include "rfx_dialog.h"

class RenderRFX : public QObject, public MeshRenderInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshRenderInterface)

public:
	RenderRFX();
	~RenderRFX();
	void Init(QAction*, MeshModel&, RenderMode&, QGLWidget*);
	void Render(QAction*, MeshModel&, RenderMode&, QGLWidget*);
	void Finalize(QAction*, MeshModel&, GLArea*);
	bool isSupported()         { return shadersSupported; }
	QList<QAction*> actions();
	int passNum()              { return totPass; }

private:
	void initActionList();

	bool shadersSupported;
	QList<QAction*> actionList;
	int shaderPass;
	int totPass;
	QString shaderDir;
	RfxDialog *dialog;
	RfxShader *activeShader;
};
#endif /* RENDERRFX_H */
