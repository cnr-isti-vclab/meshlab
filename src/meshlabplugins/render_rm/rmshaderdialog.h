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
Revision 1.3  2007/12/10 14:21:02  corsini
new version with correct layout

Revision 1.2  2007/12/03 11:35:24  corsini
code restyling


****************************************************************************/
#ifndef RMSHADERDIALOG_H
#define RMSHADERDIALOG_H

// Qt headers
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QList>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QSignalMapper>
#include <QCheckBox>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <GL/glew.h>
#include <QGLWidget>
#include "parser/RmXmlParser.h"
#include "parser/RmEffect.h"
#include "parser/RmPass.h"
#include "ui_rmShadowDialog.h"
#include "glstateholder.h"
#include <meshlab/meshmodel.h>

class RmShaderDialog : public QDialog
{
	Q_OBJECT

public:
	RmShaderDialog(GLStateHolder *holder, RmXmlParser *parser, QGLWidget *gla,
	               RenderMode &rm, QWidget *parent = NULL);
	~RmShaderDialog();

public slots:
	void fillDialogWithEffect( int index );
	void fillTabsWithPass( int index );
	void clearTabs();
	void valuesChanged(const QString & varNameAndIndex );

private:
	Ui_RmShaderDialogClass ui;

	QGridLayout *layoutUniform;
	QGridLayout *layoutTextures;
	QGridLayout *layoutOpengl;

	QGLWidget *glarea;
	RenderMode *rendMode;
	RmXmlParser *parser;

	RmEffect *eff_selected;
	RmPass *pass_selected;

	QList<QWidget*> shown;
	GLStateHolder *holder;

	QSignalMapper *signaler;
};
#endif /* RMSHADERDIALOG_H */
