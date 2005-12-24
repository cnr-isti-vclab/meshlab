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
/****************************************************************************
  History
$Log$
Revision 1.6  2005/12/24 04:18:46  ggangemi
Added generic .gdp shaders support

Revision 1.5  2005/12/05 18:11:28  ggangemi
Added toon shader example

Revision 1.4  2005/12/05 16:52:57  ggangemi
new interfaces

Revision 1.3  2005/12/03 22:50:06  cignoni
Added copyright info

****************************************************************************/
#include <QtGui>

#include <math.h>
#include <stdlib.h>

#include "meshrender.h"

using namespace vcg;

void MeshShaderRenderPlugin::Init(QAction *a, MeshModel &m, GLArea *gla) 
{
	GLenum err = glewInit();
	if (GLEW_OK == err) {
		if (GLEW_ARB_vertex_program && GLEW_ARB_fragment_program) {
			supported = true;
			if (shaders.find(a->text()) != shaders.end()) {
				
				v = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
				f = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);


				QDir shadersDir = QDir(qApp->applicationDirPath());
			#if defined(Q_OS_WIN)
				if (shadersDir.dirName() == "debug" || shadersDir.dirName() == "release")
					shadersDir.cdUp();
			#elif defined(Q_OS_MAC)
				if (shadersDir.dirName() == "MacOS") {
					shadersDir.cdUp();
					shadersDir.cdUp();
					shadersDir.cdUp();
				}
			#endif
				shadersDir.cd("shaders");
				
				char *fs = textFileRead(shadersDir.absoluteFilePath(shaders[a->text()].fpFile).toLocal8Bit().data());
				char *vs = textFileRead(shadersDir.absoluteFilePath(shaders[a->text()].vpFile).toLocal8Bit().data());
				
				const char * vv = vs;
				const char * ff = fs;
				glShaderSourceARB(v, 1, &vv, NULL);
				glShaderSourceARB(f, 1, &ff, NULL);
						
				glCompileShaderARB(v);
				glCompileShaderARB(f);

				shaders[a->text()].shaderProg = glCreateProgramObjectARB();
				glAttachObjectARB(shaders[a->text()].shaderProg,v);
				glAttachObjectARB(shaders[a->text()].shaderProg,f);
				glLinkProgramARB(shaders[a->text()].shaderProg);

				map<QString, UniformVariable>::iterator i = shaders[a->text()].uniformVars.begin();
				while (i != shaders[a->text()].uniformVars.end()) {
					(shaders[a->text()].uniformVars[i->first]).location = glGetUniformLocationARB(shaders[a->text()].shaderProg, (i->first).toLocal8Bit().data());
					++i;
				}
			}
		}
	}
}

void MeshShaderRenderPlugin::Render(QAction *a, MeshModel &m, RenderMode &rm, GLArea *gla) 
{
	if (shaders.find(a->text()) != shaders.end()) {
		ShaderInfo si = shaders[a->text()];

		glUseProgramObjectARB(si.shaderProg);

		map<QString, UniformVariable>::iterator i = si.uniformVars.begin();
    while (i != si.uniformVars.end()) {
			switch(i->second.type) {
				case SINGLE_INT: {
						glUniform1fARB((i->second.location), i->second.val);
								} break;
				case SINGLE_FLOAT: {
					glUniform1fARB((i->second.location), i->second.val);
								} break;
				case ARRAY_2_FLOAT: {
					glUniform2fARB((i->second).location, i->second.val2[0], i->second.val2[1]);
								} break;
				case ARRAY_3_FLOAT: {
					glUniform3fARB((i->second).location, i->second.val3[0], i->second.val3[1], i->second.val3[2]);
								} break;
				case ARRAY_4_FLOAT: {
					glUniform4fARB((i->second).location, i->second.val4[0], i->second.val4[1], i->second.val4[2], i->second.val4[3]);
								} break;
				default: {} break;
			}
			++i;
    }
	}
}

Q_EXPORT_PLUGIN(MeshShaderRenderPlugin)
