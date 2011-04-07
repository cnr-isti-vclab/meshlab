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
#include <common/pluginmanager.h>

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
	QDir shadersDir = PluginManager::getBaseDirPath();

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

	if (!shadersDir.cd("shaders/shadersrm")) {
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
			if (theParser.isValidDoc()) {
				QAction *action = new QAction(fileName, this);
				action->setCheckable(false);
				actionList.append(action);
				delete theParser.GetShader();
			}
		}
	}
}

void RenderRFX::Init(QAction *action, MeshDocument &md, RenderMode &rmode, QGLWidget *parent)
{

	assert(actionList.contains(action));
	if (activeShader) {
		delete activeShader;
		activeShader = NULL;
	}

	// parse shader file we're going to use
	RfxParser theParser(QDir(shaderDir).absoluteFilePath(action->text()));

	// Small hack that allow to use the current mesh textures for the shaders.
	foreach(MeshModel * mp, md.meshList)
			{
					if(mp->cm.textures.size()>0)
					{
                        QFileInfo meshBaseDir(mp->fullName());
						theParser.setMeshTexture(meshBaseDir.absolutePath()+"/"+QString(mp->cm.textures[0].c_str()));
					}			
			}

	theParser.Parse(md);
  RfxShader *tmp = theParser.GetShader();
  //activeShader = theParser.GetShader();
  //assert(activeShader);

  if (dialog) {
    dialog->close();
    delete dialog;
  }

	//verifies if there's some special attributes in the shader.
  if(!tmp->checkSpecialAttributeDataMask(&md))
  {
    if (activeShader) {
      delete activeShader;
      activeShader = NULL;
    }
		return;
  }
  activeShader = tmp;

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

void RenderRFX::Render(QAction *action, MeshDocument &md,  RenderMode &rm, QGLWidget *parent)
{

  if(!activeShader) return;
	rm.textureMode = vcg::GLW::TMPerWedge;
	
	
	for(shaderPass=0;shaderPass<totPass;shaderPass++)
	{
		activeShader->Start(shaderPass);
			glGetError();
			foreach(MeshModel * mp, md.meshList)
			{
				
				if(mp->visible && activeShader->GetPass(shaderPass)->hasSpecialAttribute()){
					GLuint *program = activeShader->GetPass(shaderPass)->getProgram();
								
					Draw(&md, program, activeShader->GetPass(shaderPass)->AttributesList());
				

				}
				else{
					if(mp->visible) 
						mp->Render(rm.drawMode,rm.colorMode,rm.textureMode);
				}
				
			}
	}
	glUseProgramObjectARB(0);

}

void RenderRFX::Finalize(QAction * /*act*/, MeshDocument &/*md*/, GLArea */*gla*/)
{
	// close any open dialog
	if (dialog) {
		dialog->close();
		delete dialog;
		dialog = NULL;
	}

	// ... and delete any active shader
	if (activeShader) {
		delete activeShader;
		activeShader = NULL;
	}
}

Q_EXPORT_PLUGIN(RenderRFX)
