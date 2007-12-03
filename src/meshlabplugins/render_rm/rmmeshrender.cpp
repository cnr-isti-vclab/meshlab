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
Revision 1.7  2007/12/03 11:14:55  corsini
*** empty log message ***

Revision 1.6  2007/12/03 11:08:48  corsini
code restyling


****************************************************************************/

// Local headers
#include "rmmeshrender.h"

// Qt headers
#include <QtOpenGL>
#include <QtGui/QImage>


static int vp[4];

const PluginInfo& RmMeshShaderRenderPlugin::Info() {
	static PluginInfo ai; 
	ai.Date=tr("September 2007");
	ai.Version = "1.0";
	ai.Author = "Fusco Francesco, Giacomo Galilei";
	return ai;
}

void RmMeshShaderRenderPlugin::initActionList() {

	QDir shadersDir = QDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
	if (shadersDir.dirName() == "debug" || shadersDir.dirName() == "release" || shadersDir.dirName() == "plugins"  )
		shadersDir.cdUp();
#elif defined(Q_OS_MAC)
	if (shadersDir.dirName() == "MacOS") {
		for(int i=0;i<4;++i){
			shadersDir.cdUp();
			if(shadersDir.exists("shadersrm")) break;
		}
	}
#endif

	if(!shadersDir.cd("shadersrm"))
	{
		QMessageBox::information(0, "MeshLab", "Unable to find the render monkey shaders directory.\n" "No shaders will be loaded.");
		return;
	}


	int errors = 0;
	int successes = 0;

	foreach (QString fileName, shadersDir.entryList(QDir::Files)) {
		if (fileName.endsWith(".rfx")) {
			RmXmlParser * parser = new RmXmlParser( shadersDir.absoluteFilePath(fileName) );
			if( parser -> parse() == false ) {
				qDebug() << "Unable to load RmShader from" << shadersDir.absoluteFilePath(fileName) << ": " << parser -> errorString();
				delete parser;
				errors += 1;
				continue;
			}

			rmsources.insert(fileName, parser);
			successes += 1;

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
	if( dialog ) {
		dialog->close();
		delete dialog;
	}

	RmXmlParser * parser = rmsources.value(a->text());
	assert(parser);

	gla->makeCurrent();
  GLenum err = glewInit();
	if (GLEW_OK == err) 
	{
		if (GLEW_ARB_vertex_program && GLEW_ARB_fragment_program) 
		{

			holder.reset();
			dialog = new RmShaderDialog(&holder, parser, gla, rm);
			dialog->move(10,100);
			dialog->show();
		}
	}

	// check errors, if any
	glGetError();
}
void RmMeshShaderRenderPlugin::Render(QAction *a, MeshModel &m, RenderMode &rm, QGLWidget *gla)
{

	if (holder.needUpdateInGLMemory)
		holder.updateUniformVariableValuesInGLMemory();

	if(holder.isSupported())
	{

		/* Handle single pass filters */
		if (holder.passNumber() == 1)
		{
			glEnable(GL_TEXTURE_2D);
			holder.updateUniformVariableValuesInGLMemory(0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			holder.usePassProgram(0);
			return;
		}

		glViewport(0,0,FBO_SIZE,FBO_SIZE); /* FIXME */

		if (holder.currentPass >= holder.passNumber())
			holder.currentPass = -1;

		if (holder.currentPass < 0)
			holder.currentPass = 0;

		if (holder.currentPass >= 0)
		{
			holder.executePass(holder.currentPass);
		}

	}

	holder.currentPass++;

	// check errors, if any
	glGetError();
}


Q_EXPORT_PLUGIN(RmMeshShaderRenderPlugin)

