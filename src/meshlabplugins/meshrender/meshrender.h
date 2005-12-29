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
Revision 1.8  2005/12/29 13:52:31  mariolatronico
gl/glew.h -> GL/glew.h

Revision 1.7  2005/12/24 04:18:46  ggangemi
Added generic .gdp shaders support

Revision 1.6  2005/12/19 16:22:30  davide_portelli
Now "Toon Shader" plugin is checkable

Revision 1.5  2005/12/05 18:11:28  ggangemi
Added toon shader example

Revision 1.4  2005/12/05 16:52:57  ggangemi
new interfaces

Revision 1.3  2005/12/03 22:50:06  cignoni
Added copyright info

****************************************************************************/

#ifndef SHADERRENDERPLUGIN_H
#define SHADERRENDERPLUGIN_H

#include <QDir>
#include <QObject>
#include <QAction>
#include <QList>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>
#include <QFile>
#include <QString>
#include <QApplication>
#include <QMap>
#include <map>

#include <GL/glew.h>
#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include "textfile.h"


struct UniformVariable {
	int type;
	float val;
	GLint location;
	vcg::Point2f val2;
	vcg::Point3f val3;
	vcg::Point4f val4;
};

struct ShaderInfo {
	QString vpFile;
	QString fpFile;
	map<QString, UniformVariable> uniformVars;
	GLhandleARB shaderProg;
};

enum {
	SINGLE_INT = 0,
	SINGLE_FLOAT = 5,
	ARRAY_2_FLOAT = 6,
	ARRAY_3_FLOAT = 7,
	ARRAY_4_FLOAT = 8
};

class MeshShaderRenderPlugin : public QObject, public MeshRenderInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshRenderInterface)

	GLhandleARB v;
	GLhandleARB f;

	map<QString, ShaderInfo> shaders;

	bool supported;
	QList <QAction *> actionList;

public:

	MeshShaderRenderPlugin()
	{
		supported = false;
	}

	QList<QAction *> MeshShaderRenderPlugin::actions () {

	
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
									QDomNode unifElemValue = unifElem.firstChild();
								
									if (!unifElemValue.isNull()) {

										switch (uv.type) 
										{
										case SINGLE_INT: 
											{
												uv.val = unifElemValue.toElement().attribute("Value0", 0).toFloat();
											} break;
										case SINGLE_FLOAT: 
											{ 
												uv.val = unifElemValue.toElement().attribute("Value0", 0).toFloat();
											} break; 
										case ARRAY_2_FLOAT: 
											{ 
												uv.val2[0] = unifElemValue.toElement().attribute("Value0", 0).toFloat();			
												uv.val2[1] = unifElemValue.toElement().attribute("Value1", 0).toFloat();	
											} break; 
										case ARRAY_3_FLOAT: 
											{ 
												uv.val3[0] = unifElemValue.toElement().attribute("Value0", 0).toFloat();			
												uv.val3[1] = unifElemValue.toElement().attribute("Value1", 0).toFloat();			
												uv.val3[2] = unifElemValue.toElement().attribute("Value2", 0).toFloat();		
											} break; 
										case ARRAY_4_FLOAT: 
											{ 
												uv.val4[0] = unifElemValue.toElement().attribute("Value0", 0).toFloat();			
												uv.val4[1] = unifElemValue.toElement().attribute("Value1", 0).toFloat();			
												uv.val4[2] = unifElemValue.toElement().attribute("Value2", 0).toFloat();
												uv.val4[3] = unifElemValue.toElement().attribute("Value3", 0).toFloat();		
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

		return actionList;
	}



	virtual bool isSupported() {return supported;}
	virtual void Init(QAction *a, MeshModel &m, GLArea *gla);
	virtual void Render(QAction *a, MeshModel &m, RenderMode &rm, GLArea *gla);

};

#endif

