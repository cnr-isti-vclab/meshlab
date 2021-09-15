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

#include <GL/glew.h>
#include <QDockWidget>
#include "edit_align.h"

#include "alignDialog.h"
#include <meshlab/glarea.h>

static QTextEdit *globalLogTextEdit = 0;

MeshTreem::MeshNode *AlignDialog::currentNode() { return edit->currentNode(); }

// Global function to write on the log in the lower part of the window.
bool AlignCallBackPos(const int, const char * message)
{
	assert(globalLogTextEdit);

	globalLogTextEdit->insertPlainText(QString(message));
	globalLogTextEdit->ensureCursorVisible();
	globalLogTextEdit->update();
	qApp->processEvents();

	return true;
}
// This signal is used to make a connection between the closure of the align dialog and the end of the editing in the GLArea
void AlignDialog::closeEvent(QCloseEvent * /*event*/)
{
	emit closing();
}

AlignDialog::AlignDialog(QWidget *parent, EditAlignPlugin *_edit) : QDockWidget(parent)
{
	// setWindowFlags( windowFlags() | Qt::WindowStaysOnTopHint | Qt::SubWindow);
	   //setVisible(false);
	AlignDialog::ui.setupUi(this);
	this->setWidget(ui.frame);
	this->setFeatures(QDockWidget::AllDockWidgetFeatures);
	this->setAllowedAreas(Qt::LeftDockWidgetArea);
	QPoint p = parent->mapToGlobal(QPoint(0, 0));
	this->setFloating(true);
	this->setGeometry(p.x() + (parent->width() - width()), p.y() + 40, width(), height());
	this->edit = _edit;
	// The following connection is used to associate the click with the change of the current mesh.
	connect(ui.alignTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(onClickItem(QTreeWidgetItem *, int)));
	globalLogTextEdit = ui.logTextEdit;
	currentArc = nullptr;
	meshTree = nullptr;
}


void AlignDialog::setCurrentArc(vcg::AlignPair::Result *_currentArc)
{
	assert(meshTree);

	// First clear the background of previously selected arc
	MeshTreeWidgetItem *oldArcF = A2Tf[currentArc];
	MeshTreeWidgetItem *oldArcB = A2Tb[currentArc];
	if (oldArcF != NULL)
	{
		assert(oldArcF->a == currentArc);
		oldArcF->setBackground(3, QBrush());
		oldArcB->setBackground(3, QBrush());
	}

	// if we clicked twice on the same arc deselect it
	if (_currentArc == currentArc) {
		currentArc = nullptr;
		return;
	}

	MeshTreeWidgetItem *newArcB = A2Tb[_currentArc];
	MeshTreeWidgetItem *newArcF = A2Tf[_currentArc];
	if (newArcB != NULL)
	{
		assert(newArcB->a == _currentArc);
		newArcB->setBackground(3, QBrush(QColor("#d0ffff")));
		newArcF->setBackground(3, QBrush(QColor("#c0ffff")));
	}
	newArcF->treeWidget()->expandItem(newArcF->parent());
	newArcB->treeWidget()->expandItem(newArcB->parent());
	currentArc = _currentArc;
	updateButtons();
}


void AlignDialog::updateCurrentNodeBackground()
{
	static MeshTreem::MeshNode *lastCurrentNode = nullptr;
	assert(meshTree);

	if (lastCurrentNode && M2T[lastCurrentNode])
		M2T[lastCurrentNode]->setBackground(3, QBrush());

	MeshTreeWidgetItem *newNodeItem = M2T[currentNode()];
	if (newNodeItem != NULL)
	{
		newNodeItem->setBackground(3, QBrush(QColor(Qt::lightGray)));
		lastCurrentNode = currentNode();
	}
}

void AlignDialog::setTree(MeshTreem *_meshTree)
{
	assert(meshTree == nullptr);
	meshTree = _meshTree;
	meshTree->cb = AlignCallBackPos;
	rebuildTree();
}

void AlignDialog::updateDialog()
{
	assert(meshTree != nullptr);
	assert(currentNode() == meshTree->find(currentNode()->m));
	updateButtons();
}

void AlignDialog::updateButtons()
{
	if (currentNode() == nullptr)
		return;
	if (currentNode()->glued)
		ui.glueHereButton->setText("Unglue Mesh   ");
	else ui.glueHereButton->setText("Glue Here Mesh");

	ui.pointBasedAlignButton->setDisabled(currentNode()->glued);
	ui.manualAlignButton->setDisabled(currentNode()->glued);
	ui.recalcButton->setDisabled(currentArc == nullptr);
	ui.icpParamCurrentButton->setDisabled(currentArc == nullptr);
	ui.baseMeshButton->setDisabled(!currentNode()->glued);
}

MeshTreeWidgetItem::MeshTreeWidgetItem(MeshTreem::MeshNode *meshNode)
{
	QString meshName = meshNode->m->label();

	QString labelText;
	setText(0, QString::number(meshNode->Id()));
	if (meshNode->glued)  	setText(2, "*");
	if (meshNode->m->isVisible())  setIcon(1, QIcon(":/layer_eye_open.png"));
	else setIcon(1, QIcon(":/layer_eye_close.png"));

	labelText.sprintf("%s", qUtf8Printable(meshName));
	setText(3, labelText);

	n = meshNode;
	a = nullptr;
}


MeshTreeWidgetItem::MeshTreeWidgetItem(MeshTreem* /*meshTree*/, vcg::AlignPair::Result *A, MeshTreeWidgetItem *parent)
{
	n = nullptr;
	a = A;
	parent->addChild(this);
	QString buf = QString("Arc: %1 -> %2 Area: %3 Err: %4 Sample# %5 (%6)")
		.arg((*A).FixName)
		.arg((*A).MovName)
		.arg((*A).area, 6, 'f', 3)
		.arg((*A).err, 6, 'f', 3)
		.arg((*A).ap.SampleNum, 6)
		.arg((*A).as.lastSampleUsed());
	setText(3, buf);

	QFont fixedFont("Courier");
	std::vector<vcg::AlignPair::Stat::IterInfo> &I = (*A).as.I;
	QTreeWidgetItem *itemArcIter;
	buf.sprintf("Iter - MinD -  Error - Sample - Used - DistR - BordR - AnglR  ");
	//          " 12   123456  1234567   12345  12345   12345   12345   12345
	itemArcIter = new QTreeWidgetItem(this);
	itemArcIter->setFont(3, fixedFont);
	itemArcIter->setText(3, buf);
	for (size_t qi = 0; qi < I.size(); ++qi)
	{
		buf.sprintf(" %02zu   %6.2f  %7.4f   %05i  %05i  %5i  %5i  %5i",
			qi, I[qi].MinDistAbs, I[qi].pcl50,
			I[qi].SampleTested, I[qi].SampleUsed, I[qi].DistanceDiscarded, I[qi].BorderDiscarded, I[qi].AngleDiscarded);
		itemArcIter = new QTreeWidgetItem(this);
		itemArcIter->setFont(3, fixedFont);
		itemArcIter->setText(3, buf);
	}
}

void AlignDialog::rebuildTree()
{
	currentArc = nullptr;
	gla = edit->_gla;
	ui.alignTreeWidget->clear();
	M2T.clear();
	A2Tf.clear();
	A2Tb.clear();
    //  QList<MeshTreem::MeshNode*> &meshList = meshTree->nodeList;
    //	for (int i = 0; i < meshList.size(); ++i)
    for(auto ni=meshTree->nodeMap.begin();ni!=meshTree->nodeMap.end();++ni)
      { 
        MeshTreem::MeshNode *mn=ni->second;
//		MeshTreeWidgetItem *item = new MeshTreeWidgetItem(meshList.value(i));
        MeshTreeWidgetItem *item = new MeshTreeWidgetItem(mn);
//		 if(meshList.value(i)==currentNode) item->setBackground(0,QBrush(QColor(Qt::lightGray)));
//        M2T[meshList.value(i)] = item;
        M2T[mn] = item;
		ui.alignTreeWidget->addTopLevelItem(item);
	}

	// Second part add the arcs to the tree
	vcg::AlignPair::Result *A;
	MeshTreeWidgetItem *parent;
	MeshTreeWidgetItem *item;
	for (int i = 0; i < meshTree->resultList.size(); ++i)
	{
		A = &(meshTree->resultList[i]);
		// Forward arc
//		parent = M2T[meshList.at((*A).FixName)];
        parent = M2T[meshTree->nodeMap[(*A).FixName] ];
		item = new MeshTreeWidgetItem(meshTree, A, parent);
		A2Tf[A] = item;
		// Backward arc
//		parent = M2T[meshList.at((*A).MovName)];
        parent = M2T[meshTree->nodeMap [(*A).MovName] ];
		item = new MeshTreeWidgetItem(meshTree, A, parent);
		A2Tb[A] = item;
	}
	ui.alignTreeWidget->resizeColumnToContents(0);
	ui.alignTreeWidget->resizeColumnToContents(1);
	ui.alignTreeWidget->resizeColumnToContents(2);
	assert(currentNode());
	updateCurrentNodeBackground();
	updateButtons();
}
void AlignDialog::currentMeshChanged(int)
{
	this->updateDialog();
	this->rebuildTree();
}

// Called when a user click over the tree;
void AlignDialog::onClickItem(QTreeWidgetItem * item, int column)
{
	//  int row = item->data(1,Qt::DisplayRole).toInt();
	MeshTreeWidgetItem *mItem = dynamic_cast<MeshTreeWidgetItem *>(item);
	if (!mItem) 
		return; // user clicked on a iteration info (neither a node nor an arc)

	MeshTreem::MeshNode * nn = mItem->n;
	if (nn) {
		if (column == 1)
		{
			nn->m->setVisible(!nn->m->isVisible());
			emit updateMeshSetVisibilities();
			if (nn->m->isVisible()) mItem->setIcon(1, QIcon(":/layer_eye_open.png"));
			else mItem->setIcon(1, QIcon(":/layer_eye_close.png"));
		}
		else {
			this->edit->_md->setCurrentMesh(nn->Id());
			updateCurrentNodeBackground();
		}
	}
	else {
		assert(mItem->a);
		setCurrentArc(mItem->a);
	}
	gla->update();
	updateButtons();
}
