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

MeshNode *AlignDialog::currentNode() {return edit->currentNode();}

// Global function to write on the log in the lower part of the window.
bool AlignCallBackPos(const int pos, const char * message )
{
  assert(globalLogTextEdit);
	
	globalLogTextEdit->insertPlainText(QString(message));
	globalLogTextEdit->ensureCursorVisible();
	globalLogTextEdit->update();
	qApp->processEvents();

	return true;
}
// This signal is used to make a connection between the closure of the align dialog and the end of the editing in the GLArea
void AlignDialog::closeEvent ( QCloseEvent * /*event*/ )
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
	connect(	ui.alignTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem * , int  )) , this,  SLOT(onClickItem(QTreeWidgetItem * , int ) ) );
	
	globalLogTextEdit=ui.logTextEdit;
  currentArc=0;
	meshTree=0;
}


void AlignDialog::setCurrentArc(vcg::AlignPair::Result *_currentArc)
{
	assert(meshTree);
	
	// First clear the backgrund of previously selected arc
	MeshTreeWidgetItem *oldArcF= A2Tf[currentArc];
	MeshTreeWidgetItem *oldArcB= A2Tb[currentArc];
	if(oldArcF!=NULL)
		{ 
			assert(oldArcF->a == currentArc);
			oldArcF->setBackground(3,QBrush());	
			oldArcB->setBackground(3,QBrush());	
		}
	
	// if we clicked twice on the same arc deselect it
	if(_currentArc==currentArc) 		{
			currentArc=0;
			return;
		}
	
	MeshTreeWidgetItem *newArcB= A2Tb[_currentArc];
	MeshTreeWidgetItem *newArcF= A2Tf[_currentArc];
	if(newArcB!=NULL)
		{ 
			assert(newArcB->a == _currentArc);
			newArcB->setBackground(3,QBrush(QColor("#d0ffff")));				
			newArcF->setBackground(3,QBrush(QColor("#c0ffff")));				
		}

	currentArc=_currentArc;
}


void AlignDialog::updateCurrentNodeBackground()
{
  static MeshNode *lastCurrentNode=0;
	assert(meshTree);

	if(lastCurrentNode && M2T[lastCurrentNode])
		 M2T[lastCurrentNode]->setBackground(3,QBrush());	
	
	MeshTreeWidgetItem *newNodeItem= M2T[currentNode()];
	if(newNodeItem!=NULL)
		{ 
			newNodeItem->setBackground(3,QBrush(QColor(Qt::lightGray)));			
			lastCurrentNode=	currentNode();
		}
}

void AlignDialog::setTree(MeshTree *_meshTree)
{
	assert(meshTree==0);
	meshTree=_meshTree;
	meshTree->cb = AlignCallBackPos;
  rebuildTree();
}

void AlignDialog::updateDialog()
{
	assert(meshTree!=0);
	assert(currentNode() == meshTree->find(currentNode()->m));
	updateButtons();
}

void AlignDialog::updateButtons()
{
    if(currentNode()->glued)
         ui.glueHereButton->setText("Unglue Mesh   ");
    else ui.glueHereButton->setText("Glue Here Mesh");

  ui.pointBasedAlignButton->setDisabled(currentNode()->glued);
  ui.manualAlignButton->setDisabled    (currentNode()->glued);
  ui.recalcButton->setDisabled    (currentArc==0);
  ui.icpParamCurrentButton->setDisabled    (currentArc==0);
}

MeshTreeWidgetItem::MeshTreeWidgetItem(MeshNode *meshNode)
{
        QString meshName = meshNode->m->shortName();

		QString labelText;
		setText(0, QString::number(meshNode->id));		
		if(meshNode->glued)  	setText(2, "*");
		if(meshNode->m->visible)  setIcon(1,QIcon(":/layer_eye_open.png"));
											else setIcon(1,QIcon(":/layer_eye_close.png"));
		
		labelText.sprintf("%s",qPrintable(meshName)); 
		setText(3, labelText);

		n=meshNode;
		a=0;
}


MeshTreeWidgetItem::MeshTreeWidgetItem(MeshTree* /*meshTree*/, vcg::AlignPair::Result *A, MeshTreeWidgetItem *parent)
{
		n=0;
		a=A;
		parent->addChild(this);
		QString buf=QString("Arc: %1 -> %2 Area: %3 Err: %4 Sample# %5 (%6)")
			.arg((*A).FixName)
			.arg((*A).MovName)
			.arg((*A).area, 6,'f',3)
			.arg((*A).err,  6,'f',3)
			.arg((*A).ap.SampleNum,6)
			.arg((*A).as.LastSampleUsed() );
			setText(3,buf);
			
			QFont fixedFont("Courier");
			std::vector<vcg::AlignPair::Stat::IterInfo> &I= (*A).as.I;
			QTreeWidgetItem *itemArcIter;
			buf.sprintf("Iter - MinD -  Error - Sample - Used - DistR - BordR - AnglR  ");
			//          " 12   123456  1234567   12345  12345   12345   12345   12345
			itemArcIter = new QTreeWidgetItem(this);
			itemArcIter->setFont(3,fixedFont);
			itemArcIter->setText(3,buf);
			for(int qi=0;qi<I.size();++qi)
			{
				buf.sprintf(" %02i   %6.2f  %7.4f   %05i  %05i  %5i  %5i  %5i",
										qi, I[qi].MinDistAbs, I[qi].pcl50,
										I[qi].SampleTested,I[qi].SampleUsed,I[qi].DistanceDiscarded,I[qi].BorderDiscarded,I[qi].AngleDiscarded );
				itemArcIter = new QTreeWidgetItem(this);
				itemArcIter->setFont(3,fixedFont);
				itemArcIter->setText(3,buf);
			}
}

void AlignDialog::rebuildTree()
{
	currentArc =0;
	gla=edit->gla;
	QList<MeshNode*> &meshList=meshTree->nodeList;
	ui.alignTreeWidget->clear();
	M2T.clear();
	A2Tf.clear();
	A2Tb.clear();
	for(int i=0;i<meshList.size();++i)
	 {
	 		MeshTreeWidgetItem *item=new MeshTreeWidgetItem(meshList.value(i));
		 // if(meshList.value(i)==currentNode) item->setBackground(0,QBrush(QColor(Qt::lightGray)));				
		  M2T[meshList.value(i)]=item;
		  ui.alignTreeWidget->insertTopLevelItem(i,item);
	}
	
  // Second part add the arcs to the tree
	vcg::AlignPair::Result *A;
	MeshTreeWidgetItem *parent;
	MeshTreeWidgetItem *item;
	for(int i=0;i< meshTree->ResVec.size();++i)
	{
	  A=&(meshTree->ResVec[i]);
		// Forward arc
	  parent=M2T[meshList.value((*A).FixName)];
		item = new MeshTreeWidgetItem(meshTree, A, parent);
		A2Tf[A]=item;
		// Backward arc
		parent=M2T[meshList.value((*A).MovName)];
		item = new MeshTreeWidgetItem(meshTree, A, parent);
		A2Tb[A]=item;
	}
	ui.alignTreeWidget->resizeColumnToContents(0);
	ui.alignTreeWidget->resizeColumnToContents(1);
	ui.alignTreeWidget->resizeColumnToContents(2);
	assert(currentNode());
	updateCurrentNodeBackground();
	updateButtons();
}

// Called when a user click over the tree;
void AlignDialog::onClickItem(QTreeWidgetItem * item, int column )
{
//  int row = item->data(1,Qt::DisplayRole).toInt();
	MeshTreeWidgetItem *mItem = dynamic_cast<MeshTreeWidgetItem *>(item);
	if(!mItem) return; // user clicked on a iteration info (neither a node nor an arc) 
	
	MeshNode * nn= mItem->n;
	if(nn) {
		if(column==1)
					{
						nn->m->visible = ! nn->m->visible;
						if(nn->m->visible) mItem->setIcon(1,QIcon(":/layer_eye_open.png"));
													else mItem->setIcon(1,QIcon(":/layer_eye_close.png"));
					}
		else {
						gla->meshDoc.setCurrentMesh(nn->id);
						updateCurrentNodeBackground();
					}
	} else {
		assert(mItem->a);
		setCurrentArc(mItem->a);
	}
	gla->update();
	updateButtons();
}
