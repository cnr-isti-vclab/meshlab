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

#ifndef RFX_DIALOG_H_
#define RFX_DIALOG_H_

/* syntax highlighter for vertex and fragment editor */
#include <QSyntaxHighlighter>
class GLSLSynHlighter : public QSyntaxHighlighter
{
public:
	GLSLSynHlighter(QTextDocument *parent = 0);

protected:
	void highlightBlock(const QString &text);

private:
	struct HighlightingRule
	{
		QRegExp pattern;
		QTextCharFormat format;
	};
	QVector<HighlightingRule> highlightingRules;

	QTextCharFormat kwordsFormat;
	QTextCharFormat builtinsFormat;
	QTextCharFormat functionFormat;
	QTextCharFormat singleLineCommentFormat;
	QTextCharFormat multiLineCommentFormat;

	QRegExp commentStartExpression;
	QRegExp commentEndExpression;
};


/* Shader Properties dialog */
#include <cassert>
#include <climits>
#include <cfloat>
#include <QMap>
#include <typeinfo>
#include <QLabel>
#include <QPicture>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QSignalMapper>
#include <QFileDialog>
#include <QColor>
#include <QDockWidget>
#include "ui_rfx_dialog.h"
#include "rfx_shader.h"
#include "rfx_specialuniform.h"
#include "rfx_colorbox.h"

class RfxDialog : public QDockWidget
{
	Q_OBJECT

public:
	RfxDialog(RfxShader*, QAction*, QGLWidget *parent = 0);
	virtual ~RfxDialog();

     
     static const float DECTOINT;
     static const float INTTODEC;

	enum DialogTabs { UNIFORM_TAB, TEXTURE_TAB, ALL_TABS };

public slots:
	void PassSelected(int);
	void TextureSelected(int);
	void ChangeTexture(int);
	void ChangeValue(const QString&);
	void extendRange(double);
	void extendRange(int);
	void mapSliderLineEdit(QWidget*);
     
private:
	void setupTabs();
	void AddUniformBox(RfxUniform*, int);
	void DrawIFace(QGridLayout*, RfxUniform*, int idx, int rows, int columns);
	void CleanTab(int);

     Ui::RfxDock ui;
	QGLWidget *mGLWin;
	QMultiMap<int, QWidget*> widgetsByTab;
	int selPass;
	RfxShader *shader;
	GLSLSynHlighter *vertHL;
	GLSLSynHlighter *fragHL;
};
#endif /* RFX_DIALOG_H_ */
