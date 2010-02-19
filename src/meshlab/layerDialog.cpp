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

#include "ui_layerDialog.h"
#include "layerDialog.h"
#include "glarea.h"
#include "mainwindow.h"

using namespace std;

LayerDialog::LayerDialog(QWidget *parent )    : QDockWidget(parent)
{
	ui = new Ui::layerDialog();
  setWindowFlags( windowFlags() | Qt::WindowStaysOnTopHint | Qt::SubWindow);
	setVisible(false);
	LayerDialog::ui->setupUi(this);
	gla=qobject_cast<GLArea *>(parent);
	mw=qobject_cast<MainWindow *>(gla->parentWidget()->parentWidget());

	connect(ui->layerTableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(toggleStatus(int,int)) );
	connect(ui->addButton, SIGNAL(clicked()), mw, SLOT(openIn()) );
	connect(ui->deleteButton, SIGNAL(clicked()), mw, SLOT(delCurrentMesh()) );

	this->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->layerTableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(ui->layerTableWidget, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(showContextMenu(const QPoint&)));
	connect(ui->menuButton, SIGNAL(clicked()), this, SLOT(showLayerMenu()));
    connect(&(gla->meshDoc), SIGNAL(currentMeshChanged(int)),this, SLOT(updateTable()));
	//connect(	ui->deleteButton, SIGNAL(cellClicked(int, int)) , this,  SLOT(openIn(int,int)) );
}
void LayerDialog::toggleStatus(int row, int col)
{
	switch(col)
	{
		case 0 :
			//the user has chosen to switch the layer
			gla->meshDoc.setCurrentMesh(row);
			break;
		case 1 :
		{
			//the user has clicke on one of the eyes
			QList<MeshModel *> &meshList=gla->meshDoc.meshList;
			// NICE TRICK.
			// If the user has pressed ctrl when clicking on the eye icon, only that layer will remain visible
			// Very useful for comparing meshes

			if(QApplication::keyboardModifiers() == Qt::ControlModifier)
					foreach(MeshModel *mp, meshList)
					{
						mp->visible=false;
					}

			if(meshList.at(row)->visible)  meshList.at(row)->visible = false;
			else   meshList.at(row)->visible = true;
		}
	}
	//make sure the right row is colored or that they right eye is drawn (open or closed)
	updateTable();
	gla->update();
}

void LayerDialog::showEvent ( QShowEvent * /* event*/ )
{
	updateTable();
}

void LayerDialog::showLayerMenu()
{
	foreach (QWidget *widget, QApplication::topLevelWidgets()) {
		MainWindow* mainwindow = dynamic_cast<MainWindow*>(widget);
		if (mainwindow)
		{
			mainwindow->layerMenu()->popup(ui->menuButton->mapToGlobal(QPoint(10,10)));
			return;
		}
	}
}

void LayerDialog::showContextMenu(const QPoint& pos)
{
	// switch layer
	int row = ui->layerTableWidget->rowAt(pos.y());
	if (row>=0)
		gla->meshDoc.setCurrentMesh(row);

	foreach (QWidget *widget, QApplication::topLevelWidgets()) {
		MainWindow* mainwindow = dynamic_cast<MainWindow*>(widget);
		if (mainwindow)
		{
			mainwindow->layerMenu()->popup(ui->layerTableWidget->mapToGlobal(pos));
			return;
		}
	}
}

void LayerDialog::updateLog(GLLogStream &log)
{
	QList< pair<int,QString> > &logStringList=log.S;
	ui->logPlainTextEdit->clear();
	//ui->logPlainTextEdit->setFont(QFont("Courier",10));

	pair<int,QString> logElem;
	QString preWarn    = "<font face=\"courier\" size=3 color=\"red\"> Warning: " ;
	QString preSystem  = "<font face=\"courier\" size=2 color=\"grey\"> System:  " ;
	QString preFilter  = "<font face=\"courier\" size=2 color=\"black\"> Filter:  " ;

	QString post   = "</font>";

	foreach(logElem, logStringList){
		QString logText = logElem.second;
		if(logElem.first == GLLogStream::SYSTEM)  logText = preSystem + logText + post;
		if(logElem.first == GLLogStream::WARNING) logText = preWarn + logText + post;
		if(logElem.first == GLLogStream::FILTER)  logText = preFilter + logText + post;
		ui->logPlainTextEdit->appendHtml(logText);
	}
}

void LayerDialog::updateTable()
{
	if(!isVisible()) return;
	QList<MeshModel *> &meshList=gla->meshDoc.meshList;
	//qDebug("Items in list: %d", meshList.size());
	ui->layerTableWidget->clear();
	ui->layerTableWidget->setColumnCount(3);
	ui->layerTableWidget->setRowCount(meshList.size());
	ui->layerTableWidget->horizontalHeader()->hide();
	ui->layerTableWidget->setColumnWidth(1,32);
	ui->layerTableWidget->setColumnWidth(2,32);
	ui->layerTableWidget->setShowGrid(false);
	for(int i=0;i<meshList.size();++i)
	 {
    QTableWidgetItem *item;
		//qDebug("Filename %s", meshList.at(i)->fileName.c_str());

        item = new QTableWidgetItem(meshList.at(i)->shortName());
		if(meshList.at(i)==gla->mm()) {
						item->setBackground(QBrush(Qt::yellow));
						item->setForeground(QBrush(Qt::blue));
						}
  	ui->layerTableWidget->setItem(i,0,item );

		if(meshList.at(i)->visible){
				item = new QTableWidgetItem(QIcon(":/images/layer_eye_open.png"),"");
			}		else		{
				item = new QTableWidgetItem(QIcon(":/images/layer_eye_close.png"),"");
			}
		item->setFlags(Qt::ItemIsEnabled);
  	ui->layerTableWidget->setItem(i,1,item );

		item = new QTableWidgetItem(QIcon(":/images/layer_edit_unlocked.png"),QString());
		item->setFlags(Qt::ItemIsEnabled);
  	ui->layerTableWidget->setItem(i,2,item );

	}
	ui->layerTableWidget->resizeColumnsToContents();
	//ui->layerTableWidget->adjustSize();

	//this->adjustSize();

}

LayerDialog::~LayerDialog()
{
	delete ui;
}
