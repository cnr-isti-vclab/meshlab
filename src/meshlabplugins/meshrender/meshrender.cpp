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
Revision 1.9  2006/02/03 12:27:08  ggangemi
improved shaderDialog support

Revision 1.8  2006/01/25 16:58:05  ggangemi
shaderdialog closed every time the user changes the current shader

Revision 1.7  2006/01/25 02:59:38  ggangemi
added shadersDialog initial support

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

void MeshShaderRenderPlugin::initActionList() {

	
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

	
		QDomDocument doc;
		foreach (QString fileName, shadersDir.entryList(QDir::Files)) {
			if (fileName.endsWith(".gdp")) {
				QFile file(shadersDir.absoluteFilePath(fileName));
				if (file.open(QIODevice::ReadOnly)) {
					if (doc.setContent(&file)) {
						file.close();

						QDomElement root = doc.documentElement();
						if (root.nodeName() == tr("GLSLang")) {

							ShaderInfo si;

							QDomElement elem;

							//Vertex program filename
							elem = root.firstChildElement("VPCount");
							if (!elem.isNull()) {
								//first child of VPCount is "Filenames"
								QDomNode child = elem.firstChild();
								if (!child.isNull()) {
									//first child of "Filenames" is "Filename0"
									child = child.firstChild();
									si.vpFile =	(child.toElement()).attribute("VertexProgram", "");
								}
							}

							//Fragment program filename
							elem = root.firstChildElement("FPCount");
							if (!elem.isNull()) {
								//first child of FPCount is "Filenames"
								QDomNode child = elem.firstChild();
								if (!child.isNull()) {
									//first child of "Filenames" is "Filename0"
									child = child.firstChild();
									si.fpFile =	(child.toElement()).attribute("FragmentProgram", "");
								}
							}	

							//Uniform Variables
							elem = root.firstChildElement("UniformVariables");
							if (!elem.isNull()) {

								QDomNode unif = elem.firstChild();
								while( !unif.isNull() ) {
									
									UniformVariable uv;

									QDomElement unifElem = unif.toElement();
									QString unifVarName = unifElem.attribute("Name", "");
									
									uv.type = (unifElem.attribute("Type", "")).toInt();
									uv.widget = (unifElem.attribute("Widget", "")).toInt();
									uv.min = (unifElem.attribute("Min", "")).toFloat();
									uv.max = (unifElem.attribute("Max", "")).toFloat();
									uv.step = (unifElem.attribute("Step", "")).toFloat();

									QDomNode unifElemValue = unifElem.firstChild();
								
									if (!unifElemValue.isNull()) {

										switch (uv.type) 
										{
										case SINGLE_INT: 
											{
												uv.ival = unifElemValue.toElement().attribute("Value0", 0).toInt();
											} break;
										case SINGLE_FLOAT: 
											{ 
												uv.fval[0] = unifElemValue.toElement().attribute("Value0", 0).toFloat();
											} break; 
										case ARRAY_2_FLOAT: 
											{ 
												uv.fval[0] = unifElemValue.toElement().attribute("Value0", 0).toFloat();			
												uv.fval[1] = unifElemValue.toElement().attribute("Value1", 0).toFloat();	
											} break; 
										case ARRAY_3_FLOAT: 
											{ 
												uv.fval[0] = unifElemValue.toElement().attribute("Value0", 0).toFloat();			
												uv.fval[1] = unifElemValue.toElement().attribute("Value1", 0).toFloat();			
												uv.fval[2] = unifElemValue.toElement().attribute("Value2", 0).toFloat();		
											} break; 
										case ARRAY_4_FLOAT: 
											{ 
												uv.fval[0] = unifElemValue.toElement().attribute("Value0", 0).toFloat();			
												uv.fval[1] = unifElemValue.toElement().attribute("Value1", 0).toFloat();			
												uv.fval[2] = unifElemValue.toElement().attribute("Value2", 0).toFloat();
												uv.fval[3] = unifElemValue.toElement().attribute("Value3", 0).toFloat();		
											} break; 
										default: 
											{ 
												
											} break; 
										}
										
										si.uniformVars[unifVarName] = uv;
									}
								
									unif = unif.nextSibling();
								}
							}					
							
							shaders[fileName] = si;

							QAction * qa = new QAction(fileName, this); 
							qa->setCheckable(false);
							actionList << qa;
						}
					} else {
						file.close();
					}
				}
			}
		}

//		return actionList;
	}

void MeshShaderRenderPlugin::Init(QAction *a, MeshModel &m, GLArea *gla) 
{
	if (sDialog) sDialog->close();

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

				free(fs);
				free(vs);

				glCompileShaderARB(v);
				glCompileShaderARB(f);

				GLint statusV;
				GLint statusF;

				glGetObjectParameterivARB(v, GL_OBJECT_COMPILE_STATUS_ARB, &statusV);
				glGetObjectParameterivARB(f, GL_OBJECT_COMPILE_STATUS_ARB, &statusF);

				if (statusF && statusV) { //successful compile
					shaders[a->text()].shaderProg = glCreateProgramObjectARB();
					glAttachObjectARB(shaders[a->text()].shaderProg,v);
					glAttachObjectARB(shaders[a->text()].shaderProg,f);
					glLinkProgramARB(shaders[a->text()].shaderProg);

					GLint linkStatus;
					glGetObjectParameterivARB(shaders[a->text()].shaderProg, GL_OBJECT_LINK_STATUS_ARB, &linkStatus);

					if (linkStatus) {
						map<QString, UniformVariable>::iterator i = shaders[a->text()].uniformVars.begin();
						while (i != shaders[a->text()].uniformVars.end()) {
							(shaders[a->text()].uniformVars[i->first]).location = glGetUniformLocationARB(shaders[a->text()].shaderProg, (i->first).toLocal8Bit().data());
							++i;
						}
						
					}
				} else {
					QMessageBox::critical(0, "Meshlab",
						QString("An error occurred during shader's linking.\n") +
						"Please check your graphic card's extensions \n"+
						"or the shader's code\n\n");
				}

				sDialog = new ShaderDialog(&shaders[a->text()]);

				/*int okPressed = */sDialog->show();
				//if (okPressed != QDialog::Rejected) return;

			} else {
				QMessageBox::critical(0, "Meshlab",
					QString("An error occurred during shader's compiling.\n") +
					"Please check your graphic card's extensions \n"+
					"or the shader's code\n\n");
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
					glUniform1fARB(i->second.location, i->second.ival);
												 } break;
				case SINGLE_FLOAT: {
					glUniform1fARB(i->second.location, i->second.fval[0]);
													 } break;
				case ARRAY_2_FLOAT: {
					glUniform2fARB(i->second.location, i->second.fval[0], i->second.fval[1]);
														} break;
				case ARRAY_3_FLOAT: {
					glUniform3fARB(i->second.location, i->second.fval[0], i->second.fval[1], i->second.fval[2]);
														} break;
				case ARRAY_4_FLOAT: {
					glUniform4fARB(i->second.location, i->second.fval[0], i->second.fval[1], i->second.fval[2], i->second.fval[3]);
														} break;
				default: {} break;
			}
			++i;
		}
	}
}

Q_EXPORT_PLUGIN(MeshShaderRenderPlugin)
