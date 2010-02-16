/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
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
$Log$
Revision 1.3  2006/06/18 20:40:06  cignoni
Completed Open/Save of scripts

Revision 1.2  2006/06/16 01:26:07  cignoni
Added Initial Filter Script Dialog

Revision 1.1  2006/06/15 13:05:57  cignoni
added Filter History Dialogs

****************************************************************************/
#include <QFileDialog>

#include "ui_filterScriptDialog.h"
#include "filterScriptDialog.h"
#include "mainwindow.h"
#include "stdpardialog.h"

//using namespace vcg;


FilterScriptDialog::FilterScriptDialog(QWidget * parent)
		:QDialog(parent)
{
	ui = new Ui::scriptDialog();
  FilterScriptDialog::ui->setupUi(this);
  scriptPtr=0;
  connect(ui->okButton, SIGNAL(clicked()), this, SLOT(applyScript()));
  connect(ui->clearScriptButton,SIGNAL(clicked()), this, SLOT(clearScript()));
  connect(ui->saveScriptButton, SIGNAL(clicked()), this, SLOT(saveScript()));
  connect(ui->openScriptButton, SIGNAL(clicked()), this, SLOT(openScript()));
  connect(ui->moveUpButton,SIGNAL(clicked()), this, SLOT(moveSelectedFilterUp()));
  connect(ui->moveDownButton, SIGNAL(clicked()), this, SLOT(moveSelectedFilterDown()));
  connect(ui->removeFilterButton, SIGNAL(clicked()), this, SLOT(removeSelectedFilter()));
  connect(ui->editParameterButton, SIGNAL(clicked()), this, SLOT(editSelectedFilterParameters()));
}

void FilterScriptDialog::setScript(FilterScript *scr)
{
	scriptPtr=scr;
  FilterScript::iterator li;
  ui->scriptListWidget->clear();
  
  for(li=scr->actionList.begin();li!=scr->actionList.end() ;++li)
     ui->scriptListWidget->addItem((*li).first);
}

void FilterScriptDialog::applyScript()
{
	//get the main window
	MainWindow *mainWindow = qobject_cast<MainWindow*>(parentWidget());
		
	if(NULL == mainWindow){
		qDebug() << "problem casting parent of filterscriptdialog to main window";
	}
	
	//deal with the special case parameter of 
	for(int action = 0; action < scriptPtr->actionList.size(); action++)
	{
	
		RichParameterSet &parameterSet = scriptPtr->actionList[action].second;
	
		for(int i = 0; i < parameterSet.paramList.size(); i++)
		{	
			//get a modifieable reference
        //	RichParameter* parameter = parameterSet.paramList[i];
		
			//if this is a mesh paramter and the pointer not valid
		//	if(parameter.fieldType == FilterParameter::PARMESH &&
		//			!mainWindow->GLA()->meshDoc.meshList.contains((MeshModel*)parameter.pointerVal) )
		//	{
		//		//if the meshmodel pointer is not in the document but not null there must be a problem
		//		if(NULL != parameter.pointerVal )
		//		{
		//			qDebug() << "meshdoc gave us null";
		//			QMessageBox::critical(this, tr("Script Failure"), QString("Failed because you set a script parameter to be a mesh which does not exist anymore."));
		//			return;
		//		} else if(parameter.fieldVal.toInt() >= mainWindow->GLA()->meshDoc.meshList.size() || parameter.fieldVal.toInt() < 0)
		//		{
		//			qDebug() << "integer is out of bounds:" << parameter.fieldVal.toInt();
		//			QMessageBox::critical(this, tr("Script Failure"), QString("One of the filters in the script needs more meshes than you have loaded."));
		//			return;
		//		} else {
		//			qDebug() << "meshdoc has non null value";
		//			parameter.pointerVal = mainWindow->GLA()->meshDoc.getMesh(parameter.fieldVal.toInt());
		//		}
		//	} 
		//	
		}
	}
	
	accept();
}

void FilterScriptDialog::clearScript()
{
  assert(scriptPtr);
  scriptPtr->actionList.clear();
  ui->scriptListWidget->clear();
}

void FilterScriptDialog::saveScript()
{
  QString fileName = QFileDialog::getSaveFileName(this,tr("Save Filter Script File"),".", "*.mlx");
	if (fileName.isEmpty())	return;
  QFileInfo fi(fileName);
  if(fi.suffix().toLower()!="mlx")
    fileName.append(".mlx");
  scriptPtr->save(fileName);
}

void FilterScriptDialog::openScript()
{
	QString fileName = QFileDialog::getOpenFileName(this,tr("Open Filter Script File"),".", "*.mlx");
	if (fileName.isEmpty())	return;
  scriptPtr->open(fileName);
  setScript(scriptPtr);
}

void FilterScriptDialog::moveSelectedFilterUp()
{
	//NOTE if this class gets to complex using the QT model/view may be a good idea
	//however, i found it to be over complicated and not too helpful for reording
	
	int currentRow = ui->scriptListWidget->currentRow();

	//move item up in list
	QPair<QString, RichParameterSet> pair = scriptPtr->actionList.takeAt(currentRow);
	scriptPtr->actionList.insert(currentRow-1, pair);
	
	//move item up on ui
	QListWidgetItem * item = ui->scriptListWidget->takeItem(currentRow);
	ui->scriptListWidget->insertItem(currentRow-1, item);
	
	//set selected 
	ui->scriptListWidget->setCurrentItem(item);
}

void FilterScriptDialog::moveSelectedFilterDown()
{
	int currentRow = ui->scriptListWidget->currentRow();

	//move item down in list
	QPair<QString, RichParameterSet> pair = scriptPtr->actionList.takeAt(currentRow);
	scriptPtr->actionList.insert(currentRow+1, pair);
	
	//move item down on ui
	QListWidgetItem * item = ui->scriptListWidget->takeItem(currentRow);
	ui->scriptListWidget->insertItem(currentRow+1, item);
	
	//set selected 
	ui->scriptListWidget->setCurrentItem(item);
}

void FilterScriptDialog::removeSelectedFilter()
{
	int currentRow = ui->scriptListWidget->currentRow();
	
	//remove from list and ui
	scriptPtr->actionList.removeAt(currentRow);
	ui->scriptListWidget->takeItem(currentRow);
}

void FilterScriptDialog::editSelectedFilterParameters()
{
	//get the selected item
	int currentRow = ui->scriptListWidget->currentRow();	
	
	//return if no row was selected
	if(currentRow == -1)
		return;
	
	QString actionName = scriptPtr->actionList.at(currentRow).first;
	RichParameterSet oldParameterSet = scriptPtr->actionList.at(currentRow).second;
	
	//get the main window
	MainWindow *mainWindow = qobject_cast<MainWindow*>(parentWidget());
	
	if(NULL == mainWindow){
		qDebug() << "problem casting parent of filterscriptdialog to main window";
		return;
	}

	//get a pointer to this action and filter from the main window so we can get the 
	//description of the parameters from the filter
	QAction *action = mainWindow->pluginManager().actionFilterMap[actionName];
	MeshFilterInterface *iFilter = qobject_cast<MeshFilterInterface *>(action->parent());
	
	if(NULL == iFilter){
		qDebug() << "null filter";
		return;
	}

	//fill the paramter set with all the names and descriptions which are lost in the 
	//filter script
	RichParameterSet newParameterSet;
	iFilter->initParameterSet(action, mainWindow->GLA()->meshDoc, newParameterSet);

	if(newParameterSet.paramList.size() == oldParameterSet.paramList.size())
	{
		//now set values to be the old values
		RichParameterCopyConstructor cc;
		for(int i = 0; i < newParameterSet.paramList.size(); i++)
		{
			oldParameterSet.paramList[i]->accept(cc);
			newParameterSet.paramList[i]->val = cc.lastCreated->val;
		}	
	} else
		qDebug() << "the size of the given list is not the same as the filter suggests it should be.  your filter script may be out of date, or there is a bug in the filter script class";

	//launch the dialog
	GenericParamDialog parameterDialog(this, &newParameterSet, "Edit Parameters", &mainWindow->GLA()->meshDoc);
	int result = parameterDialog.exec();
	if(result == QDialog::Accepted){
		//keep the changes	
		scriptPtr->actionList[currentRow].second = newParameterSet;
	}
	
}

FilterScriptDialog::~FilterScriptDialog()
{
	delete ui;
}


