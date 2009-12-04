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

#ifndef SHADERDIALOG_H
#define SHADERDIALOG_H

#include <map>
#include <GL/glew.h>
#include <common/meshmodel.h>
#include <QtGui>
#include <QGLWidget>
#include "shaderStructs.h"
#include "ui_shaderDialog.h"

class ShaderDialog : public QDockWidget
{
    Q_OBJECT

public:
    ShaderDialog(ShaderInfo *sInfo, QGLWidget* gla, RenderMode &rm, QWidget *parent = 0);
    ~ShaderDialog();

private:
		QGLWidget* glarea;
		RenderMode * rendMode;
		ShaderInfo * shaderInfo;
		QSignalMapper *colorSignalMapper;
		QSignalMapper *valueSignalMapper;
		QSignalMapper *textLineSignalMapper;
		QSignalMapper *textButtonSignalMapper;
    std::map<QString, QLabel*> labels;
		std::map<QString, QSlider*> sliders;
		std::vector<QLineEdit*> textLineEdits;
		std::map<QString, QLineEdit*> lineEdits;
    Ui::ShaderDialogClass ui;

private slots:
	void valuesChanged(const QString &);
	void setColorValue(const QString &);
	void setColorMode(int);
	void changeTexturePath(int);
	void browseTexturePath(int);
	void reloadTexture(int i);
		
};

#endif // SHADERDIALOG_H
