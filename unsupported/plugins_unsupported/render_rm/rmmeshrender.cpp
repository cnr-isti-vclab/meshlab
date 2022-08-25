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
/****************************************************************************
History
$Log$
Revision 1.9  2007/12/10 11:46:43  corsini
start new version of rendering cycle

Revision 1.8  2007/12/06 14:47:44  corsini
*** empty log message ***

Revision 1.7  2007/12/03 11:14:55  corsini
*** empty log message ***

Revision 1.6  2007/12/03 11:08:48  corsini
code restyling


****************************************************************************/
#include <GL/glew.h>
#include <QtOpenGL>
#include <QtGui/QImage>
#include "rmmeshrender.h"

static int vp[4];

RmMeshShaderRenderPlugin::~RmMeshShaderRenderPlugin()
{
	QMapIterator<QString, RmXmlParser*> i(rmsources);
	while (i.hasNext()) {
		i.next();
		delete i.value();
	}
}

QList<QAction*> RmMeshShaderRenderPlugin::actions()
{
	if (actionList.isEmpty())
		initActionList();

	return actionList;
}

void RmMeshShaderRenderPlugin::initActionList() {

	QDir shadersDir = QDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
	if (shadersDir.dirName() == "debug" ||
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
			"Unable to find the render monkey shaders directory.\n"
			"No shaders will be loaded.");
		return;
	}

	int errors = 0;
	int successes = 0;
	foreach (QString fileName, shadersDir.entryList(QDir::Files)) {
		if (fileName.endsWith(".rfx")) {
			RmXmlParser *parser = new RmXmlParser(shadersDir.absoluteFilePath(fileName));
			if (parser->parse() == false) {
				qDebug() << "Unable to load RmShader from"
				         << shadersDir.absoluteFilePath(fileName)
				         << ": " << parser -> errorString();
				delete parser;
				errors++;
				continue;
			}

			rmsources.insert(fileName, parser);
			successes++;

			QAction * qa = new QAction(fileName, this);
			qa->setCheckable(false);
			actionList << qa;
		}
	}
}



void RmMeshShaderRenderPlugin::Init(QAction *a, MeshModel &m, RenderMode &rm, QGLWidget *gla)
{
#ifdef DEBUG
	qDebug() << "Init";
#endif
	glGetIntegerv(GL_VIEWPORT, (GLint *)vp);
	if (dialog) {
		dialog->close();
		delete dialog;
	}

	RmXmlParser *parser = rmsources.value(a->text());
	assert(parser);

	gla->makeCurrent();
	GLenum err = glewInit();
	if (GLEW_OK == err) {
		if (GLEW_ARB_vertex_program && GLEW_ARB_fragment_program) {
			holder.reset();
			dialog = new RmShaderDialog(&holder, parser, gla, rm);
			dialog->move(10,100);
			dialog->show();
		}
	}

	// clear errors, if any
	glGetError();
}
void RmMeshShaderRenderPlugin::Render(QAction *a, MeshModel &m, RenderMode &rm, QGLWidget *gla)
{

	if (holder.needUpdateInGLMemory)
		holder.updateUniformVariableValuesInGLMemory();

	if (holder.passNumber() == 1) {
		/*** SINGLE-PASS shader ***/
		/**************************/

		glEnable(GL_TEXTURE_2D);
		holder.updateUniformVariableValuesInGLMemory(0);

		// setup the single-pass shader
		holder.usePassProgram(0);

		// texture generation (just in case)
		holder.genPassTextures();

		// execute it
		holder.executePass(0);
		return;
	} else {
		/*** MULTI-PASS SHADER ***/
		/*************************/

		// ...TODO...
	}

	holder.currentPass++;

	// check errors, if any
	glGetError();
}


Q_EXPORT_PLUGIN(RmMeshShaderRenderPlugin)

