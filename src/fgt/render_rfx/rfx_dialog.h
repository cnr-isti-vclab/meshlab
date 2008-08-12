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
#include <QMap>
#include <QLabel>
#include <QPicture>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include "ui_rfx_dialog.h"
#include "rfx_shader.h"

class RfxDialog : public QDialog
{
	Q_OBJECT

public:
	RfxDialog(RfxShader*, QAction*, QWidget *parent = 0);
	virtual ~RfxDialog();

	enum DialogTabs { ALL_TABS = -1, UNIFORM_TAB, TEXTURE_TAB, GLSTATE_TAB };

public slots:
	void UniformSelected(int);
	void TextureSelected(int);

private:
	void DrawIFace(RfxUniform*, int rows, int columns);
	void CleanTab(int);

	Ui::Dialog ui;
	QMultiMap<int, QWidget*> widgetsByTab;
	RfxShader *shader;
	GLSLSynHlighter *vertHL;
	GLSLSynHlighter *fragHL;
};
#endif /* RFX_DIALOG_H_ */
