/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

#include <GL/glew.h>
#include <QtGui>
#include <QGLWidget>
#include "AOGLWidget.h"
#include "filter_ao.h"

AOGLWidget::AOGLWidget (QWidget * parent, AmbientOcclusionPlugin *_plugin) :QGLWidget (parent)
{
	plugin=_plugin;
	cb=0;
	m=0;
	QGLFormat qFormat = QGLFormat::defaultFormat();
	qFormat.setAlpha(true);
	qFormat.setDepth(true);
	setFormat(qFormat);
	if(!isValid())
	{
		qDebug("Error: Unable to create a new QGLWidget");
		return;
	}
	
}


void AOGLWidget::initializeGL ()
{
	plugin->initGL(cb,m->cm.vn);

	unsigned int widgetSize = std::min(plugin->maxTexSize, plugin->depthTexSize);

	setFixedSize(widgetSize,widgetSize);
}

void AOGLWidget::paintGL ()
{
	qDebug("Start Painting window size %i %i", width(), height());
	plugin->processGL(this,*m,*viewDirVec);
	hide();
	qDebug("End Painting");
}

