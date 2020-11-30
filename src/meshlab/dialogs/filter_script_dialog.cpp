/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2020                                           \/)\/    *
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

#include "ui_filter_script_dialog.h"
#include "filter_script_dialog.h"
#include "mainwindow.h"
#include "../common/mlexception.h"
#include "rich_parameter_gui/richparameterlistdialog.h"

FilterScriptDialog::FilterScriptDialog(FilterScript& fs, QWidget * parent):
	QDialog(parent),
	ui(new Ui::FilterScriptDialog),
	script(fs)
{
	ui->setupUi(this);
	
	updateGui();
}

FilterScriptDialog::~FilterScriptDialog()
{
	delete ui;
}

void FilterScriptDialog::on_saveScriptButton_clicked()
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
	if (filt.contains(mlxast)) {
		if(suf != mlx)
			fileName.append("." + mlx);
	}
	else {
		if(suf != xml)
			fileName.append("." + xml);
	}	
	script.save(fileName);
}

void FilterScriptDialog::on_openScriptButton_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(
				this, 
				tr("Open Filter Script File"),
				".", 
				"MeshLab Scripting File Format (*.mlx);;Extensible Markup Language (*.xml)");
	if (fileName.isEmpty())	
		return;
	script.open(fileName);
	updateGui();
}

void FilterScriptDialog::on_clearScriptButton_clicked()
{
	script.clear();
	ui->scriptListWidget->clear();
}

void FilterScriptDialog::on_okButton_clicked()
{
	MainWindow *mainWindow = qobject_cast<MainWindow*>(parentWidget());
	
	if(NULL == mainWindow){
		qDebug() << "problem casting parent of filterscriptdialog to main window";
	}
	accept();
}

void FilterScriptDialog::on_moveUpButton_clicked()
{
	//NOTE if this class gets to complex using the QT model/view may be a good idea
	//however, i found it to be over complicated and not too helpful for reording
	
	int currentRow = ui->scriptListWidget->currentRow();
	if ((currentRow == -1) || (currentRow == 0))
		return;
	
	//move item up in list
	FilterNameParameterValuesPair pair = script.takeAt(currentRow);
	QString filtername = ui->scriptListWidget->currentItem()->text();
	if (pair.filterName() == filtername)
		script.insert(currentRow-1, pair);
	else {
		throw MLException("Something bad happened: A filter item has been selected in filterScriptDialog being NOT a XML filter or old-fashioned c++ filter.");
	}
	
	//move item up on ui
	QListWidgetItem * item = ui->scriptListWidget->takeItem(currentRow);
	ui->scriptListWidget->insertItem(currentRow-1, item);
	
	//set selected
	ui->scriptListWidget->setCurrentItem(item);
}

void FilterScriptDialog::on_moveDownButton_clicked()
{
	int currentRow = ui->scriptListWidget->currentRow();
	if ((currentRow == -1) || (currentRow == script.size() - 1))
		return;
	
	//move item up in list
	FilterNameParameterValuesPair pair = script.takeAt(currentRow);
	QString filtername = ui->scriptListWidget->currentItem()->text();
	if (pair.filterName() == filtername)
		script.insert(currentRow+1, pair);
	else {
		throw MLException("Something bad happened: A filter item has been selected in filterScriptDialog being NOT a XML filter or old-fashioned c++ filter.");
	}
	
	//move item up on ui
	QListWidgetItem * item = ui->scriptListWidget->takeItem(currentRow);
	ui->scriptListWidget->insertItem(currentRow+1, item);
	
	//set selected 
	ui->scriptListWidget->setCurrentItem(item);
}

void FilterScriptDialog::on_removeFilterButton_clicked()
{
	int currentRow = ui->scriptListWidget->currentRow();
	if(currentRow == -1)
		return;
	
	const FilterNameParameterValuesPair& pair = script[currentRow];
	QString filtername = ui->scriptListWidget->currentItem()->text();
	if (pair.filterName() == filtername)
	{
		ui->scriptListWidget->takeItem(currentRow);
		script.removeAt(currentRow);
	}
	else {
		throw MLException("Something bad happened: A filter item has been selected in filterScriptDialog being NOT a XML filter or old-fashioned c++ filter.");
	}
}

void FilterScriptDialog::on_editParameterButton_clicked()
{
	//get the selected item
	int currentRow = ui->scriptListWidget->currentRow();
	
	//return if no row was selected
	if(currentRow == -1)
		return;
	
	QString filtername = ui->scriptListWidget->currentItem()->text();
	const FilterNameParameterValuesPair& pair = script.at(currentRow);
	if (pair.filterName() == filtername) {
		editOldParameters(currentRow);
	}
	else {
		throw MLException("Something bad happened: A filter item has been selected in filterScriptDialog being NOT a XML filter or old-fashioned c++ filter.");
	}
}

void FilterScriptDialog::editOldParameters( const int row )
{
	if(row == -1)
		return;
	QString actionName = ui->scriptListWidget->currentItem()->text();
	
	FilterNameParameterValuesPair& old = script[row];
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

void FilterScriptDialog::updateGui()
{
	ui->scriptListWidget->clear();
	
	for (const FilterNameParameterValuesPair& pair : script)
		ui->scriptListWidget->addItem(pair.filterName());
}
