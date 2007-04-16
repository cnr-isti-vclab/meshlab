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

#include "layerDialog.h"
#include "glarea.h"

LayerDialog::LayerDialog(QWidget *parent )    : QDialog(parent)    
{ 
  setWindowFlags( windowFlags() | Qt::WindowStaysOnTopHint | Qt::SubWindow);
	setVisible(false);
	LayerDialog::ui.setupUi(this);
	gla=static_cast<GLArea *>(parent);
	connect(	ui.layerTableWidget, SIGNAL(cellClicked(int, int)) , this,  SLOT(toggleStatus(int,int)) );
}

void LayerDialog::toggleStatus(int row, int col)
{
	if(col==1) 
	{
	  QList<MeshModel *> &meshList=gla->meshList;
		if(meshList.at(row)->visible)
		{
			ui.layerTableWidget->item(row,col)->setIcon(QIcon(":/images/layer_eye_close.png"));
			meshList.at(row)->visible=false;
		}
		else
		{
			ui.layerTableWidget->item(row,col)->setIcon(QIcon(":/images/layer_eye_open.png"));
			meshList.at(row)->visible=true;
		}		
		gla->update();
	}
}

void LayerDialog::updateTable()
{
	QList<MeshModel *> &meshList=gla->meshList;
	qDebug("Items in list: %d", meshList.size());
	ui.layerTableWidget->setColumnCount(3);
	ui.layerTableWidget->setRowCount(meshList.size());
	ui.layerTableWidget->horizontalHeader()->hide();
	ui.layerTableWidget->setColumnWidth(1,32);
	ui.layerTableWidget->setColumnWidth(2,32);
	ui.layerTableWidget->setShowGrid(false);
	for(int i=0;i<meshList.size();++i)
	 {
    QTableWidgetItem *item;
		qDebug("Filename %s", meshList.at(i)->fileName.c_str());
		
		item = new QTableWidgetItem(QFileInfo(meshList.at(i)->fileName.c_str()).fileName());
  	ui.layerTableWidget->setItem(i,0,item );
		item = new QTableWidgetItem(QIcon(":/images/layer_eye_open.png"),"");
		item->setFlags(Qt::ItemIsEnabled);
  	ui.layerTableWidget->setItem(i,1,item ); 
		item = new QTableWidgetItem(QIcon(":/images/layer_edit_unlocked.png"),QString());
		item->setFlags(Qt::ItemIsEnabled);
  	ui.layerTableWidget->setItem(i,2,item );
	}
	ui.layerTableWidget->adjustSize();

	this->adjustSize();

}

