/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
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

$Log: stdpardialog.cpp,v $
****************************************************************************/
#ifndef ALIGN_DIALOG_H
#define ALIGN_DIALOG_H
#include <QtGui>

class GLArea;

#include "ui_AlignDialog.h"
#include <QDialog>
class EditAlignPlugin;
class MeshModel;

class AlignDialog : public QDockWidget
{
		Q_OBJECT

	public:
		AlignDialog(QWidget *parent = 0);
		void updateTree();
		Ui::alignDialog ui;
		GLArea *gla; 
		EditAlignPlugin *edit;
		QMap<MeshModel *, QTreeWidgetItem *> M2T; // Model to tree hash
	public slots:
		void setCurrent(QTreeWidgetItem * item, int column );
};


#endif
