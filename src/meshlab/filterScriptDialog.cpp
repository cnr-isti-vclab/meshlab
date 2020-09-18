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

#include <QFileDialog>

#include "ui_filterScriptDialog.h"
#include "filterScriptDialog.h"
#include "mainwindow.h"
#include "../common/mlexception.h"
#include "rich_parameter_gui/richparameterlistdialog.h"

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
  ui->scriptListWidget->clear();
  
  for (const FilterNameParameterValuesPair& pair : *scr)
     ui->scriptListWidget->addItem(pair.filterName());
}

void FilterScriptDialog::applyScript()
{
	//get the main window
	MainWindow *mainWindow = qobject_cast<MainWindow*>(parentWidget());
		
	if(NULL == mainWindow){
		qDebug() << "problem casting parent of filterscriptdialog to main window";
	}
	accept();
}

void FilterScriptDialog::clearScript()
{
  assert(scriptPtr);
  scriptPtr->clear();
  ui->scriptListWidget->clear();
}

void FilterScriptDialog::saveScript()
{
	QString filt;
	QString mlx("mlx");
	QString xml("xml");
	QString mlxast("(*." + mlx + ")");
	QString xmlast("(*." + xml + ")");
	QString fileName = QFileDialog::getSaveFileName(this,tr("Save Filter Script File"),".", "MeshLab Scripting File Format " + mlxast + ";;Extensible Markup Language " + xmlast,&filt);
	if (fileName.isEmpty())	return;
	
	QFileInfo fi(fileName);
	QString suf(fi.suffix().toLower());
	if (filt.contains(mlxast))
	{
		if(suf != mlx)
			fileName.append("." + mlx);
	}
	else 
	{
		if(suf != xml)
			fileName.append("." + xml);
	}	
	scriptPtr->save(fileName);
}

void FilterScriptDialog::openScript()
{
	QString fileName = QFileDialog::getOpenFileName(this,tr("Open Filter Script File"),".", "MeshLab Scripting File Format (*.mlx);;Extensible Markup Language (*.xml)");
	if (fileName.isEmpty())	return;
  scriptPtr->open(fileName);
  setScript(scriptPtr);
}

void FilterScriptDialog::moveSelectedFilterUp()
{
    //NOTE if this class gets to complex using the QT model/view may be a good idea
    //however, i found it to be over complicated and not too helpful for reording

    int currentRow = ui->scriptListWidget->currentRow();
    if ((currentRow == -1) || (currentRow == 0))
        return;

    //move item up in list
    FilterNameParameterValuesPair pair = scriptPtr->takeAt(currentRow);
    QString filtername = ui->scriptListWidget->currentItem()->text();
    if (pair.filterName() == filtername)
        scriptPtr->insert(currentRow-1, pair);
    else
        throw MLException("Something bad happened: A filter item has been selected in filterScriptDialog being NOT a XML filter or old-fashioned c++ filter.");

    //move item up on ui
    QListWidgetItem * item = ui->scriptListWidget->takeItem(currentRow);
    ui->scriptListWidget->insertItem(currentRow-1, item);

    //set selected
    ui->scriptListWidget->setCurrentItem(item);
}

void FilterScriptDialog::moveSelectedFilterDown()
{
    int currentRow = ui->scriptListWidget->currentRow();
    if ((currentRow == -1) || (currentRow == scriptPtr->size() - 1))
        return;

    //move item up in list
    FilterNameParameterValuesPair pair = scriptPtr->takeAt(currentRow);
    QString filtername = ui->scriptListWidget->currentItem()->text();
    if (pair.filterName() == filtername)
        scriptPtr->insert(currentRow+1, pair);
    else
        throw MLException("Something bad happened: A filter item has been selected in filterScriptDialog being NOT a XML filter or old-fashioned c++ filter.");

    //move item up on ui
    QListWidgetItem * item = ui->scriptListWidget->takeItem(currentRow);
    ui->scriptListWidget->insertItem(currentRow+1, item);

    //set selected 
    ui->scriptListWidget->setCurrentItem(item);
}

void FilterScriptDialog::removeSelectedFilter()
{
	int currentRow = ui->scriptListWidget->currentRow();
    if(currentRow == -1)
        return;

    const FilterNameParameterValuesPair& pair = (*scriptPtr)[currentRow];
    QString filtername = ui->scriptListWidget->currentItem()->text();
    if (pair.filterName() == filtername)
    {
        ui->scriptListWidget->takeItem(currentRow);
        scriptPtr->removeAt(currentRow);
    }
    else
        throw MLException("Something bad happened: A filter item has been selected in filterScriptDialog being NOT a XML filter or old-fashioned c++ filter.");
}

void FilterScriptDialog::editSelectedFilterParameters()
{
	//get the selected item
	int currentRow = ui->scriptListWidget->currentRow();
	
	//return if no row was selected
	if(currentRow == -1)
		return;
	
	QString filtername = ui->scriptListWidget->currentItem()->text();
	const FilterNameParameterValuesPair& pair = scriptPtr->at(currentRow);
	if (pair.filterName() == filtername) {
		editOldParameters(currentRow);
	}
	else {
		throw MLException("Something bad happened: A filter item has been selected in filterScriptDialog being NOT a XML filter or old-fashioned c++ filter.");
	}
}

FilterScriptDialog::~FilterScriptDialog()
{
	delete ui;
}

void FilterScriptDialog::editOldParameters( const int row )
{
	if(row == -1)
		return;
	QString actionName = ui->scriptListWidget->currentItem()->text();

	FilterNameParameterValuesPair& old = (*scriptPtr)[row];
	RichParameterList oldParameterSet = old.second;
	//get the main window
	MainWindow *mainWindow = qobject_cast<MainWindow*>(parentWidget());

	if(NULL == mainWindow)
		throw MLException("FilterScriptDialog::editXMLParameters : problem casting parent of filterscriptdialog to main window");

	//get a pointer to this action and filter from the main window so we can get the
	//description of the parameters from the filter
	QAction *action = mainWindow->pluginManager().actionFilterMap[actionName];
	FilterPluginInterface *iFilter = qobject_cast<FilterPluginInterface *>(action->parent());

	if(NULL == iFilter){
		qDebug() << "null filter";
		return;
	}

	//fill the parameter set with all the names and descriptions which are lost in the
	//filter script
	RichParameterList newParameterSet;
	iFilter->initParameterList(action, *(mainWindow->meshDoc()), newParameterSet);

	if(newParameterSet.size() == oldParameterSet.size()) {
		RichParameterList::iterator i = newParameterSet.begin();
		RichParameterList::iterator j = oldParameterSet.begin();
		//now set values to be the old values
		for (; i != newParameterSet.end(); ++i, ++j){
			i->setValue(j->value());
		}
	} else {
		qDebug() << "the size of the given list is not the same as the filter suggests it should be.  your filter script may be out of date, or there is a bug in the filter script class";
	}

	//launch the dialog
	RichParameterListDialog parameterDialog(this, newParameterSet, "Edit Parameters");
	int result = parameterDialog.exec();
	if(result == QDialog::Accepted) {
		//keep the changes
		old.second = newParameterSet;
	}
}
