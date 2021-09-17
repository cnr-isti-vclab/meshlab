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
#include <QMenu>
#include <QDockWidget>
#include <vcg/complex/algorithms/meshtree.h>

typedef vcg::MeshTree<MeshModel, Scalarm> MeshTreem;

class GLArea;

#include "ui_alignDialog.h"
#include <QDialog>
class EditAlignPlugin;
class MeshModel;

class MeshTreeWidgetItem : public QTreeWidgetItem
{
public:
	MeshTreeWidgetItem(MeshTreem::MeshNode *n);
	MeshTreeWidgetItem(MeshTreem* meshTree,vcg::AlignPair::Result *A,MeshTreeWidgetItem *parent);

    MeshTreem::MeshNode *n;
	vcg::AlignPair::Result *a;
};

class AlignDialog : public QDockWidget
{
	Q_OBJECT

public:
	AlignDialog(QWidget *parent, EditAlignPlugin *_edit);
	void rebuildTree();
	void updateButtons();
	void updateDialog();
	void updateMeshVisibilities() {emit updateMeshSetVisibilities();}
	void setTree(MeshTreem *);
	void updateCurrentNodeBackground();
	void setCurrentArc(vcg::AlignPair::Result *currentArc);

	Ui::alignDialog ui;
	GLArea *gla;
private:
	EditAlignPlugin *edit;
public:

    MeshTreem *meshTree;
    MeshTreem::MeshNode *currentNode();
	vcg::AlignPair::Result *currentArc;

	QMap<MeshTreem::MeshNode *,           MeshTreeWidgetItem *> M2T; // MeshNode to treeitem hash
	QMap<vcg::AlignPair::Result  *, MeshTreeWidgetItem *> A2Tf; // Arc to treeitem hash  (forward)
	QMap<vcg::AlignPair::Result  *, MeshTreeWidgetItem *> A2Tb; // Arc to treeitem hash  (backward)

	QMenu popupMenu;

	virtual void closeEvent ( QCloseEvent * event )	;
signals:
	void closing();
	void updateMeshSetVisibilities();

public slots:
	void onClickItem(QTreeWidgetItem * item, int column );
	void currentMeshChanged(int );
};

#endif
