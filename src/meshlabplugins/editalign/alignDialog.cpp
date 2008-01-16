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

// Global function to write on the log in the lower part of the window.
bool AlignCallBackPos(const int pos, const char * message )
{
  assert(globalLogTextEdit);
	
	globalLogTextEdit->insertPlainText(QString(message));
	globalLogTextEdit->ensureCursorVisible();
	globalLogTextEdit->repaint();

	return true;
}

void AlignDialog::closeEvent ( QCloseEvent * event )
{
  emit closing();
}

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
	currentArc=0;
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
	if(currentNode->glued) 
			ui.glueHereButton->setText("Unglue Mesh");
	else
		ui.glueHereButton->setText("Glue Mesh Here");
		
	ui.pointBasedAlignButton->setDisabled(currentNode->glued);
	ui.manualAlignButton->setDisabled    (currentNode->glued);
}

MeshTreeWidgetItem::MeshTreeWidgetItem(MeshNode *meshNode)
{
		QString meshName = QFileInfo(meshNode->m->fileName.c_str()).fileName();
		if(meshNode->glued)  	meshName+="*";

		QString labelText;
		labelText.sprintf("%s - %i",qPrintable(meshName),meshNode->id); 
		setText(0, labelText);
		n=meshNode;
		a=0;
}


MeshTreeWidgetItem::MeshTreeWidgetItem(MeshTree* meshTree, AlignPair::Result *A, MeshTreeWidgetItem *parent)
{
		n=0;
		a=A;
		parent->addChild(this);
		QString buf=QString("Arc: %1 -> %2 Area: %3 Err: %4 Sample# %5 (%6)")
			.arg((*A).FixName)
			.arg((*A).MovName)
			.arg(meshTree->OG.SVA[parent->n->id].norm_area, 6,'f',3)
			.arg((*A).err,                  6,'f',3)
			.arg((*A).ap.SampleNum,6)
			.arg((*A).as.LastSampleUsed() );
			setText(0,buf);
			
			QFont fixedFont("Courier");
			vector<AlignPair::Stat::IterInfo> &I= (*A).as.I;
			QTreeWidgetItem *itemArcIter;
			buf.sprintf("Iter - MinD -  Error - Sample - Used - DistR - BordR - AnglR  ");
			//          " 12   123456  1234567   12345  12345   12345   12345   12345
			itemArcIter = new QTreeWidgetItem(this);
			itemArcIter->setFont(0,fixedFont);
			itemArcIter->setText(0,buf);
			for(int qi=0;qi<I.size();++qi)
			{
				buf.sprintf(" %02i   %6.2f  %7.4f   %05i  %05i  %5i  %5i  %5i",
										qi, I[qi].MinDistAbs, I[qi].pcl50,
										I[qi].SampleTested,I[qi].SampleUsed,I[qi].DistanceDiscarded,I[qi].BorderDiscarded,I[qi].AngleDiscarded );
				itemArcIter = new QTreeWidgetItem(this);
				itemArcIter->setFont(0,fixedFont);
				itemArcIter->setText(0,buf);
			}
	
}

void AlignDialog::updateTree()
{
	gla=edit->gla;
	QList<MeshNode*> &meshList=meshTree->nodeList;
	ui.alignTreeWidget->clear();
	M2T.clear();
	for(int i=0;i<meshList.size();++i)
	 {
	 		MeshTreeWidgetItem *item=new MeshTreeWidgetItem(meshList.value(i));

		  if(meshList.value(i)==currentNode) item->setBackground(0,QBrush(QColor(Qt::lightGray)));
				
		  M2T[meshList.value(i)]=item;
		
		  ui.alignTreeWidget->insertTopLevelItem(0,item);
	}
	
  // Second part add the arcs to the tree
	AlignPair::Result *A;
	MeshTreeWidgetItem *parent;
	MeshTreeWidgetItem *item;
	for(int i=0;i< meshTree->ResVec.size();++i)
	{
	  A=&(meshTree->ResVec[i]);
	  parent=M2T[meshList.value((*A).FixName)];
		item = new MeshTreeWidgetItem(meshTree, A, parent);
		parent=M2T[meshList.value((*A).MovName)];
		item = new MeshTreeWidgetItem(meshTree, A, parent);

	}
	
}

// Called when a user click over the tree;
void AlignDialog::setCurrent(QTreeWidgetItem * item, int column )
{
//  int row = item->data(1,Qt::DisplayRole).toInt();
  
	MeshTreeWidgetItem *mItem = dynamic_cast<MeshTreeWidgetItem *>(item);
	if(!mItem) return;
	
	MeshNode * nn= mItem->n;
	if(nn)
	{
	setCurrentNode(nn);
  gla->meshDoc.setCurrentMesh(nn->id);
	gla->update();
	}
}