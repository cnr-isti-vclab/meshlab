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
#include <QtGui>

#include "editalign.h"

#include "alignDialog.h"
#include <meshlab/glarea.h>

static QTextEdit *globalLogTextEdit=0;

// funzione globale che scrive sul 
bool AlignCallBackPos(const int pos, const char * message )
{
  assert(globalLogTextEdit);
	
	globalLogTextEdit->insertPlainText(QString(message));
	globalLogTextEdit->ensureCursorVisible();
	globalLogTextEdit->repaint();

	return true;
};


AlignDialog::AlignDialog(QWidget *parent )    : QDockWidget(parent)    
{ 
 // setWindowFlags( windowFlags() | Qt::WindowStaysOnTopHint | Qt::SubWindow);
	//setVisible(false);
	AlignDialog::ui.setupUi(this);
	this->setWidget(ui.frame);
	this->setFeatures(QDockWidget::AllDockWidgetFeatures);
	this->setAllowedAreas(Qt::LeftDockWidgetArea);
	QPoint p=parent->mapToGlobal(QPoint(0,0));
	this->setFloating(true);
	this->setGeometry(p.x()+(parent->width()-width()),p.y()+40,width(),height() );

	// The following connection is used to associate the click with the change of the current mesh. 
	connect(	ui.alignTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem * , int  )) , this,  SLOT(setCurrent(QTreeWidgetItem * , int ) ) );
	
	globalLogTextEdit=ui.logTextEdit;
  currentNode=0;
	meshTree=0;
}

void AlignDialog::setCurrentNode(MeshNode *_currentNode)
{
	assert(meshTree);
	assert(_currentNode == meshTree->find(_currentNode->m));
	currentNode=_currentNode;
	updateDialog();
}

void AlignDialog::setTree(MeshTree *_meshTree, MeshNode *_node)
{
	assert(meshTree==0);
	meshTree=_meshTree;
	meshTree->cb = AlignCallBackPos;
	setCurrentNode(_node);
}

void AlignDialog::updateDialog()
{
	assert(meshTree!=0);
	assert(currentNode == meshTree->find(currentNode->m));
	
	updateTree();
	updateButtons();
}

void AlignDialog::updateButtons()
{
	if(currentNode->glued) ui.glueHereButton->setText("Unglue Mesh");
										else ui.glueHereButton->setText("Glue Mesh Here");
	
	if(currentNode->glued) ui.pointBasedAlignButton->setDisabled(true);
										else ui.pointBasedAlignButton->setDisabled(false);
}

void AlignDialog::updateTree()
{
	gla=edit->gla;
	QList<MeshNode*> &meshList=meshTree->nodeList;
	//qDebug("Items in list: %d", meshList.size());
	ui.alignTreeWidget->clear();
	//ui.alignTreeWidget->setColumnCount(1);
	M2T.clear();
	for(int i=0;i<meshList.size();++i)
	 {
		QTreeWidgetItem *item;
		//qDebug("Filename %s", meshList.at(i)->fileName.c_str());
		QString meshText = QFileInfo(meshList.at(i)->m->fileName.c_str()).fileName();
		if(meshList.value(i)->glued) 
				meshText=meshText+"*";
		item = new QTreeWidgetItem(QStringList (meshText));
		item->setData(1,Qt::DisplayRole,i);
		M2T[meshList.value(i)]=item;
		
		ui.alignTreeWidget->insertTopLevelItem(0,item);
	}
	
  // Second part add the arcs to the tree
	AlignPair::Result *A;
	for(int i=0;i< meshTree->ResVec.size();++i)
	{
	  A=&(meshTree->ResVec[i]);
		QString buf=QString("Arc: %1 -> %2 A: %3 Err: %4 Sample %5 (%6)")
		.arg((*A).FixName)
		.arg((*A).MovName)
		.arg(meshTree->OG.SVA[i].norm_area, 6,'f',3)
		.arg((*A).err,                  6,'f',3)
		.arg((*A).ap.SampleNum,6)
		.arg((*A).as.LastSampleUsed() );// LPCTSTR lpszItem
		QTreeWidgetItem *parent=M2T[meshList.value((*A).FixName)];
		
		QTreeWidgetItem *item = new QTreeWidgetItem(parent);
		item->setText(0,buf);
		
		parent=M2T[meshList.value((*A).MovName)];
		item = new QTreeWidgetItem(parent);
		item->setText(0,buf);
	}
	
}

void AlignDialog::setCurrent(QTreeWidgetItem * item, int column )
{
  int row = item->data(1,Qt::DisplayRole).toInt();

	setCurrentNode(meshTree->nodeList.value(row));
	
  gla->meshDoc.setCurrentMesh(row);
	gla->update();
}