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
	dialog = NULL;
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

	QMapIterator<QString, RfxShader*> it(shaderList);
	while (it.hasNext())
		delete it.value();
	shaderList.clear();
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
		for (int i = 0; i < 4; ++i) {
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

	// validate each .rfx file found and add to actionList
	// (this is actually just a sanity check)
	foreach (QString fileName, shadersDir.entryList(QDir::Files)) {
		if (fileName.endsWith(".rfx")) {
			RfxParser theParser(shadersDir.absoluteFilePath(fileName));
			if (theParser.Parse()) {
				QAction *action = new QAction(fileName, this);
				action->setCheckable(false);
				actionList.append(action);
				shaderList[fileName] = theParser.GetShader();
			}
		}
	}
}

void RenderRFX::Init(QAction *action, MeshModel &/*mesh*/,
                     RenderMode &/*rmode*/, QGLWidget *parent)
{
	assert(actionList.contains(action));
	RfxShader *shader = shaderList[action->text()];

	if (dialog) {
		dialog->close();
		delete dialog;
	}

	parent->makeCurrent();
	GLenum err = glewInit();
	if (GLEW_OK == err) {
		if (GLEW_ARB_vertex_program && GLEW_ARB_fragment_program) {
			shadersSupported = true;

			shader->CompileAndLink((QGLContext *)parent);

			dialog = new RfxDialog(shader, action, parent);
			dialog->move(0, 100);
			dialog->show();
		}
	}

	// clear errors, if any
	glGetError();
}

void RenderRFX::Render(QAction *action, MeshModel &/*mesh*/,
                       RenderMode &rmode, QGLWidget */*parent*/)
{
	assert(actionList.contains(action));
	RfxShader *shader = shaderList[action->text()];

	shader->Start();
	rmode.textureMode = vcg::GLW::TMPerVert;

	// clear errors, if any
	glGetError();
}

const PluginInfo &RenderRFX::Info()
{
	static PluginInfo ai;
	ai.Date    = tr("July 2008");
	ai.Version = tr("0.1");
	ai.Author  = "Carlo Casta";
	return ai;
}

Q_EXPORT_PLUGIN(RenderRFX)
