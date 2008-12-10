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
#ifndef RMSHADERRENDERPLUGIN_H
#define RMSHADERRENDERPLUGIN_H

#include <QDir>
#include <QObject>
#include <QAction>
#include <QList>
#include <QFile>
#include <QString>
#include <QApplication>
#include <QMap>
#include <QMessageBox>
#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include "parser/RmXmlParser.h"
#include "rmshaderdialog.h"
#include "glstateholder.h"


class RmMeshShaderRenderPlugin : public QObject, public MeshRenderInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshRenderInterface)

public:
	RmMeshShaderRenderPlugin() { dialog = NULL; }
	~RmMeshShaderRenderPlugin();

	QList<QAction*> actions();

	virtual bool isSupported() { return holder.isSupported(); }
	virtual void Init(QAction *a, MeshModel &m, RenderMode &rm, QGLWidget *gla);
	virtual void Render(QAction *a, MeshModel &m, RenderMode &rm, QGLWidget *gla);
	virtual int passNum() { return holder.passNumber(); }

private:
	void initActionList();

	/// map between the filename and its parser
	QMap<QString, RmXmlParser*> rmsources;

	QList <QAction *> actionList;

	RmShaderDialog *dialog;
	GLStateHolder holder;
};
#endif /* RMSHADERRENDERPLUGIN_H */
