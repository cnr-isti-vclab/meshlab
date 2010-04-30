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
Revision 1.25  2008/04/04 14:16:05  cignoni
Solved namespace ambiguities caused by the removal of a silly 'using namespace' in meshmodel.h

Revision 1.24  2007/09/09 17:56:13  ldpmatic
Minor changes to avoid memory leak.
Added two glGetError() to avoid MeshLab to crash when the plugin encounters some error on its way

Revision 1.23  2007/03/12 15:24:00  cignoni
Safer dir search for plugins for mac

Revision 1.22  2007/02/28 00:02:57  cignoni
Added casts for mac compiling

Revision 1.21  2007/02/20 13:05:50  corsini
*** empty log message ***

Revision 1.20  2007/02/20 13:05:23  corsini
add log file for shader compilation and linking error

Revision 1.19  2006/12/24 22:46:34  cignoni
Corrected bug about a wrong glUniform1fARB  (thanks Clement Menier!)

Revision 1.18  2006/05/26 04:09:52  cignoni
Still debugging 0.7

Revision 1.17  2006/05/25 04:57:45  cignoni
Major 0.7 release. A lot of things changed. Colorize interface gone away, Editing and selection start to work.
Optional data really working. Clustering decimation totally rewrote. History start to work. Filters organized in classes.

Revision 1.16  2006/03/08 17:26:13  ggangemi
added texture tab

Revision 1.15  2006/02/27 05:02:01  ggangemi
Added texture support

Revision 1.14  2006/02/25 13:44:45  ggangemi
Action "None" is now exported from MeshRenderPlugin

Revision 1.13  2006/02/21 17:26:38  ggangemi
RenderMode is now passed to MeshRender::Init()

Revision 1.11  2006/02/19 02:57:49  ggangemi
Now each shader can change the opengl status

Revision 1.10  2006/02/09 00:42:40  ggangemi
now GLArea is passed to the shaderDialog

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
#include <QGLWidget>
#include <QTextStream>

using namespace std;
using namespace vcg;

void MeshShaderRenderPlugin::initActionList() {

	QAction * qaNone = new QAction("None", this); 
	qaNone->setCheckable(false);
	actionList << qaNone;

	QDir shadersDir = QDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
	if (shadersDir.dirName() == "debug" || shadersDir.dirName() == "release" || shadersDir.dirName() == "plugins"  )
		shadersDir.cdUp();
#elif defined(Q_OS_MAC)
	if (shadersDir.dirName() == "MacOS") {
		for(int i=0;i<6;++i){
			if(shadersDir.exists("shaders")) break;
			shadersDir.cdUp();
		}
	}
#endif
	bool ret=shadersDir.cd("shaders");
  if(!ret) 
		{
			QMessageBox::information(0, "MeshLab",
															 "Unable to find the shaders directory.\n"
															 "No shaders will be loaded.");
		}
	qDebug("Shader directory found '%s', and it contains %i gdp files",qPrintable(shadersDir.path()),shadersDir.entryList(QStringList("*.gdp")).size());


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
								si.vpFile =	shadersDir.absoluteFilePath((child.toElement()).attribute("VertexProgram", ""));
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
								si.fpFile =	shadersDir.absoluteFilePath((child.toElement()).attribute("FragmentProgram", ""));
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
											uv.ival[0] = unifElemValue.toElement().attribute("Value0", 0).toInt();
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


						//OpenGL Status
						elem = root.firstChildElement("FragmentProcessor");
						if (!elem.isNull()) {
							if (elem.hasAttribute("Shade"))					si.glStatus[SHADE] = elem.attribute("Shade", "0");
							if (elem.hasAttribute("AlphaTest"))			si.glStatus[ALPHA_TEST] = elem.attribute("AlphaTest", "False");
							if (elem.hasAttribute("AlphaFunc"))			si.glStatus[ALPHA_FUNC] = elem.attribute("AlphaFunc", "0");
							if (elem.hasAttribute("AlphaClamp"))		si.glStatus[ALPHA_CLAMP] = elem.attribute("AlphaClamp", "0");
							if (elem.hasAttribute("Blending"))			si.glStatus[BLENDING] = elem.attribute("Blending", "False");
							if (elem.hasAttribute("BlendFuncSRC"))	si.glStatus[BLEND_FUNC_SRC] = elem.attribute("BlendFuncSRC", "0");
							if (elem.hasAttribute("BlendFuncDST"))	si.glStatus[BLEND_FUNC_DST] = elem.attribute("BlendFuncDST", "0");
							if (elem.hasAttribute("BlendEquation")) si.glStatus[BLEND_EQUATION] = elem.attribute("BlendEquation", "0");
							if (elem.hasAttribute("DepthTest"))			si.glStatus[DEPTH_TEST] = elem.attribute("DepthTest", "False");
							if (elem.hasAttribute("DepthFunc"))			si.glStatus[DEPTH_FUNC] = elem.attribute("DepthFunc", "0");
							if (elem.hasAttribute("ClampNear"))			si.glStatus[CLAMP_NEAR] = elem.attribute("ClampNear", "0");
							if (elem.hasAttribute("ClampFar"))			si.glStatus[CLAMP_FAR] = elem.attribute("ClampFar", "0");
							if (elem.hasAttribute("ClearColorR"))		si.glStatus[CLEAR_COLOR_R] = elem.attribute("ClearColorR", "0");
							if (elem.hasAttribute("ClearColorG"))		si.glStatus[CLEAR_COLOR_G] = elem.attribute("ClearColorG", "0");
							if (elem.hasAttribute("ClearColorB"))		si.glStatus[CLEAR_COLOR_B] = elem.attribute("ClearColorB", "0");
							if (elem.hasAttribute("ClearColorA"))		si.glStatus[CLEAR_COLOR_A] = elem.attribute("ClearColorA", "0");
						}

						
						//Textures
						
						shadersDir.cdUp();
						shadersDir.cd("textures");
						elem = root.firstChildElement("TexturedUsed");
						if (!elem.isNull()) {
							QDomNode unif = elem.firstChild();
							while( !unif.isNull() ) {
								QDomElement unifElem = unif.toElement();
								TextureInfo tInfo;

								tInfo.path = shadersDir.absoluteFilePath((unifElem.attribute("Filename", "")));								
								tInfo.MinFilter = (unifElem.attribute("MinFilter", 0)).toInt();
								tInfo.MagFilter = (unifElem.attribute("MagFilter", 0)).toInt();
								tInfo.Target = (unifElem.attribute("Target", 0)).toInt();
								tInfo.WrapS = (unifElem.attribute("WrapS", 0)).toInt();
								tInfo.WrapT = (unifElem.attribute("WrapT", 0)).toInt();
								tInfo.WrapR = (unifElem.attribute("WrapR", 0)).toInt();
							
								si.textureInfo.push_back(tInfo);
								unif = unif.nextSibling();
							}
						}
						shadersDir.cdUp();
						shadersDir.cd("shaders");

						//End Textures

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
}

void MeshShaderRenderPlugin::Init(QAction *a, MeshDocument &md, RenderMode &rm, QGLWidget *gla) 
{
	if (sDialog) {
		sDialog->close();
		delete sDialog;
		sDialog=0;
	}

  gla->makeCurrent();
	GLenum err = glewInit();
	if (GLEW_OK == err) {
		if (GLEW_ARB_vertex_program && GLEW_ARB_fragment_program) {
			supported = true;			
			if (shaders.find(a->text()) != shaders.end()) {

				v = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
				f = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

				char *fs = textFileRead((shaders[a->text()].fpFile).toLocal8Bit().data());
				char *vs = textFileRead((shaders[a->text()].vpFile).toLocal8Bit().data());

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
					else 
					{
						QFile file("shaders.log");
						if (file.open(QFile::Append))
						{
							char proglog[2048];
							GLsizei length;
							QTextStream out(&file);

							glGetProgramiv(v, GL_LINK_STATUS, &statusV);
							glGetProgramInfoLog(v, 2048, &length, proglog);
							out << "VERTEX SHADER LINK INFO:" << endl;
							out << proglog << endl << endl;

							glGetProgramiv(f, GL_LINK_STATUS, &statusF);
							glGetProgramInfoLog(f, 2048, &length, proglog);
							out << "FRAGMENT SHADER LINK INFO:" << endl << endl;
							out << proglog << endl << endl;

							file.close();
						}

						QMessageBox::critical(0, "Meshlab",
							QString("An error occurred during shader's linking.\n") +
							"See shaders.log for further details about this error.\n");
					}

					//Textures
				

					std::vector<TextureInfo>::iterator tIter = shaders[a->text()].textureInfo.begin();
					while (tIter != shaders[a->text()].textureInfo.end()) {
						glEnable(tIter->Target);
						QImage img, imgScaled, imgGL;
						img.load(tIter->path);
						// image has to be scaled to a 2^n size. We choose the first 2^N <= picture size.
						int bestW=pow(2.0,floor(::log(double(img.width() ))/::log(2.0)));
						int bestH=pow(2.0,floor(::log(double(img.height()))/::log(2.0)));
						imgScaled=img.scaled(bestW,bestH,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
						imgGL=QGLWidget::convertToGLFormat(imgScaled);

						glGenTextures( 1, &(tIter->tId) );
						glBindTexture( tIter->Target, tIter->tId );
						glTexImage2D( tIter->Target, 0, 3, imgGL.width(), imgGL.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imgGL.bits() );
						glTexParameteri( tIter->Target, GL_TEXTURE_MIN_FILTER, tIter->MinFilter );
						glTexParameteri( tIter->Target, GL_TEXTURE_MAG_FILTER, tIter->MagFilter ); 
						glTexParameteri( tIter->Target, GL_TEXTURE_WRAP_S, tIter->WrapS ); 
						glTexParameteri( tIter->Target, GL_TEXTURE_WRAP_T, tIter->WrapT ); 
						glTexParameteri( tIter->Target, GL_TEXTURE_WRAP_R, tIter->WrapR ); 


						++tIter;
					}


					sDialog = new ShaderDialog(&shaders[a->text()], gla, rm);
					sDialog->move(10,100);
					sDialog->show();

				} 
				else 
				{	
					QFile file("shaders.log");
					if (file.open(QFile::WriteOnly))
					{
						char shlog[2048];
						GLsizei length;
						QTextStream out(&file);

						glGetShaderiv(v, GL_COMPILE_STATUS, &statusV);
						glGetShaderInfoLog(v, 2048, &length, shlog);
						out << "VERTEX SHADER COMPILE INFO:" << endl << endl;
						out << shlog << endl << endl;

						glGetShaderiv(f, GL_COMPILE_STATUS, &statusF);
						glGetShaderInfoLog(f, 2048, &length, shlog);
						out << "FRAGMENT SHADER COMPILE INFO:" << endl << endl;
						out << shlog << endl << endl;

						file.close();
					}

					QMessageBox::critical(0, "Meshlab",
						QString("An error occurred during shader's compiling.\n"
						"See shaders.log for further details about this error."));
				}
			}
		}
	}

	// * clear the errors, if any
	glGetError();
}


void MeshShaderRenderPlugin::Render(QAction *a, MeshDocument &md, RenderMode &rm, QGLWidget * /* gla */) 
{
//  MeshModel &mm
	if (shaders.find(a->text()) != shaders.end()) {
		ShaderInfo si = shaders[a->text()];

		glUseProgramObjectARB(si.shaderProg);

		map<QString, UniformVariable>::iterator i = si.uniformVars.begin();
		while (i != si.uniformVars.end()) {
			switch(i->second.type) {
				case SINGLE_INT: {
					glUniform1iARB(i->second.location, i->second.ival[0]);
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

		std::map<int, QString>::iterator j = si.glStatus.begin();
		while (j != si.glStatus.end()) {
			switch (j->first) {
				case SHADE: glShadeModel(j->second.toInt()); break;
				case ALPHA_TEST: if (j->second == "True") glEnable(GL_ALPHA_TEST); else glDisable(GL_ALPHA_TEST); break;
				case ALPHA_FUNC: glAlphaFunc(j->second.toInt(), (si.glStatus[ALPHA_CLAMP]).toFloat()); break;
					//case ALPHA_CLAMP: used in ALPHA_FUNC
				case BLENDING: if (j->second == "True") glEnable(GL_BLEND); else glDisable(GL_BLEND); break;
				case BLEND_FUNC_SRC: glBlendFunc(j->second.toInt(), (si.glStatus[BLEND_FUNC_DST]).toInt()); break;
					//case BLEND_FUNC_DST: used in BLEND_FUNC_SRC
				case BLEND_EQUATION: glBlendEquation(j->second.toInt()); break;
				case DEPTH_TEST: if (j->second == "True") glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST); break;
				case DEPTH_FUNC: glDepthFunc(j->second.toInt()); break;
					//case CLAMP_NEAR:
					//case CLAMP_FAR:
				case CLEAR_COLOR_R: glClearColor(j->second.toFloat(), 
															(si.glStatus[CLEAR_COLOR_G]).toFloat(),
															(si.glStatus[CLEAR_COLOR_B]).toFloat(),
															(si.glStatus[CLEAR_COLOR_A]).toFloat()); break;
					//case CLEAR_COLOR_G: used in CLEAR_COLOR_R
					//case CLEAR_COLOR_B: used in CLEAR_COLOR_R
					//case CLEAR_COLOR_A: used in CLEAR_COLOR_R
			}
			++j;
		}
	
		int n = GL_TEXTURE0_ARB;
		std::vector<TextureInfo>::iterator tIter = shaders[a->text()].textureInfo.begin();
		while (tIter != shaders[a->text()].textureInfo.end()) {
			glActiveTexture(n);
			glEnable(tIter->Target);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);	

			glBindTexture( tIter->Target, tIter->tId );
      rm.textureMode = GLW::TMPerWedge;
			
			++tIter;
			++n;
		}

		
	}
	// * clear the errors, if any
	glGetError();
	foreach(MeshModel * mp, md.meshList)
				{
					if(mp->visible) mp->Render(rm.drawMode,rm.colorMode,rm.textureMode);
				}
	glUseProgramObjectARB(0);
}

Q_EXPORT_PLUGIN(MeshShaderRenderPlugin)
