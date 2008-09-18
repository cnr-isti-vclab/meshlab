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

#include "render_rfx.h"

RenderRFX::RenderRFX()
{
	shadersSupported = false;
	shaderPass = -1;
	totPass = -1;
	dialog = NULL;
	activeShader = NULL;
}

RenderRFX::~RenderRFX()
{
	if (dialog) {
		dialog->close();
		delete dialog;
	}

	foreach (QAction *a, actionList)
		delete a;
	actionList.clear();
}

QList<QAction*> RenderRFX::actions()
{
	if (actionList.isEmpty())
		initActionList();

	return actionList;
}

void RenderRFX::initActionList()
{
	QDir shadersDir = QDir(qApp->applicationDirPath());

#if defined(Q_OS_WIN)
	if (shadersDir.dirName() == "debug"   ||
	    shadersDir.dirName() == "release" ||
	    shadersDir.dirName() == "plugins")
		shadersDir.cdUp();
#elif defined(Q_OS_MAC)
	if (shadersDir.dirName() == "MacOS") {
		for (int i = 0; i < 6; ++i) {
			shadersDir.cdUp();
			if (shadersDir.exists("shadersrm"))
				break;
		}
	}
#endif

	if (!shadersDir.cd("shadersrm")) {
		QMessageBox::information(0, "MeshLab",
		                            "Unable to find the render monkey "
		                            "shaders directory.\n"
		                            "No shaders will be loaded.");
		return;
	}
	shaderDir = shadersDir.path();
	qDebug("Shader directory found '%s', and it contains %i rfx files",
	       qPrintable(shadersDir.path()),
	       shadersDir.entryList(QStringList("*.rfx")).size());

	// validate each .rfx file found and add to actionList
	// (this is actually just a sanity check, shaders created are deleted immediately)
	foreach (QString fileName, shadersDir.entryList(QDir::Files)) {
		if (fileName.endsWith(".rfx")) {
			RfxParser theParser(shadersDir.absoluteFilePath(fileName));
			if (theParser.Parse()) {
				QAction *action = new QAction(fileName, this);
				action->setCheckable(false);
				actionList.append(action);
				delete theParser.GetShader();
			}
		}
	}
}

void RenderRFX::Init(QAction *action, MeshModel &mesh,
                     RenderMode &rmode, QGLWidget *parent)
{
	Q_UNUSED(mesh)
	Q_UNUSED(rmode)

	assert(actionList.contains(action));
	if (activeShader) {
		delete activeShader;
		activeShader = NULL;
	}

	// parse shader file we're going to use
	RfxParser theParser(QDir(shaderDir).absoluteFilePath(action->text()));
	assert(theParser.Parse());
	activeShader = theParser.GetShader();
	assert(activeShader);

	if (dialog) {
		dialog->close();
		delete dialog;
	}

	parent->makeCurrent();
	GLenum err = glewInit();
	if (GLEW_OK == err) {
		if (GLEW_ARB_vertex_program && GLEW_ARB_fragment_program) {
			shadersSupported = true;

			activeShader->CompileAndLink();
			totPass = activeShader->GetTotalPasses();
			shaderPass = 0;

			dialog = new RfxDialog(activeShader, action, parent);
			dialog->move(0, 100);
			dialog->show();
		}
	}

	glGetError();
}

void RenderRFX::Render(QAction *action, MeshModel &mesh,
                       RenderMode &rmode, QGLWidget *parent)
{
	Q_UNUSED(action)
	Q_UNUSED(mesh)
	Q_UNUSED(parent)

	assert(activeShader);

	activeShader->Start(shaderPass++);
	rmode.textureMode = vcg::GLW::TMPerWedge;

	if (shaderPass >= totPass)
		shaderPass = 0;

	glGetError();
}

const PluginInfo& RenderRFX::Info()
{
	static PluginInfo ai;
	ai.Date    = tr("July 2008");
	ai.Version = tr("0.1");
	ai.Author  = "Carlo Casta";
	return ai;
}

Q_EXPORT_PLUGIN(RenderRFX)
