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
Revision 1.19  2006/06/08 08:54:43  zifnab1974
Do not use classname in class definition

Revision 1.18  2006/05/25 04:57:45  cignoni
Major 0.7 release. A lot of things changed. Colorize interface gone away, Editing and selection start to work.
Optional data really working. Clustering decimation totally rewrote. History start to work. Filters organized in classes.

Revision 1.17  2006/02/27 05:02:01  ggangemi
Added texture support

Revision 1.16  2006/02/21 17:26:38  ggangemi
RenderMode is now passed to MeshRender::Init()

Revision 1.14  2006/02/19 02:57:49  ggangemi
Now each shader can change the opengl status

Revision 1.13  2006/02/03 12:27:08  ggangemi
improved shaderDialog support

Revision 1.12  2006/01/25 16:58:05  ggangemi
shaderdialog closed every time the user changes the current shader

Revision 1.11  2006/01/25 02:59:38  ggangemi
added shadersDialog initial support

Revision 1.10  2006/01/19 11:41:42  ggangemi
Reduced memory occupation of "UniformVariable" struct

Revision 1.9  2006/01/17 11:04:14  cignoni
Removed bug due to multiple creation of list of action

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
#include <QtXml/QDomNamedNodeMap>
#include <QFile>
#include <QString>
#include <QApplication>
#include <QMap>
#include <map>
#include <vector>
#include <QImage>

#include <GL/glew.h>
#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include "textfile.h"
#include "shaderStructs.h"
#include "shaderDialog.h"


class MeshShaderRenderPlugin : public QObject, public MeshRenderInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshRenderInterface)

	GLhandleARB v;
	GLhandleARB f;

	map<QString, ShaderInfo> shaders;

	bool supported;
	QList <QAction *> actionList;
	
	ShaderDialog *sDialog;

public:

	MeshShaderRenderPlugin()
	{
    initActionList();
		supported = false;
		sDialog = 0;
	}

	QList<QAction *> actions () const {
    return actionList;
  }

  void initActionList();

	virtual const PluginInfo &Info();
	virtual bool isSupported() {return supported;}
	virtual void Init(QAction *a, MeshModel &m, RenderMode &rm, QGLWidget *gla);
	virtual void Render(QAction *a, MeshModel &m, RenderMode &rm, QGLWidget *gla);

};

#endif

