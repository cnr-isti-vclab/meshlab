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
#include "ui_savesnapshotDialog.h"
#include "saveSnapshotDialog.h"

SaveSnapshotDialog::SaveSnapshotDialog(QWidget * parent):QDialog(parent)
{
	ui = new Ui::SSDialog();
	SaveSnapshotDialog::ui->setupUi(this);
	connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ui->browseDir, SIGNAL(clicked()),this, SLOT(browseDir()));
	ui->outDirLineEdit->setReadOnly(true);
//	setFixedSize(250,130);
}

void SaveSnapshotDialog::setValues(const SnapshotSetting& ss)
{
	settings=ss;
	ui->baseNameLineEdit->setText(settings.basename);
    ui->outDirLineEdit->setText(settings.outdir);
	ui->resolutionSpinBox->setValue(settings.resolution);
	ui->counterSpinBox->setValue(settings.counter);		
}

SnapshotSetting SaveSnapshotDialog::getValues()
{
	settings.basename=ui->baseNameLineEdit->text();
	settings.outdir=ui->outDirLineEdit->text();
	settings.counter=ui->counterSpinBox->value();
	settings.resolution=ui->resolutionSpinBox->value();
  settings.transparentBackground=ui->backgroundCheckBox->isChecked();
  settings.snapAllLayers=ui->alllayersCheckBox->isChecked();
  settings.tiledSave=ui->tiledSaveCheckBox->isChecked();
	return settings;
}

void SaveSnapshotDialog::browseDir()
{
	QFileDialog fd(0,"Choose output directory");
	fd.setFileMode(QFileDialog::DirectoryOnly);

	QStringList selection;
	if (fd.exec())
	{
		selection = fd.selectedFiles();
		ui->outDirLineEdit->setText(selection.at(0));;	
	}

}

SaveSnapshotDialog::~SaveSnapshotDialog()
{
	delete ui;
}
