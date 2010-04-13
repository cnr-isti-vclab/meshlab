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
Revision 1.3  2007/07/10 06:49:19  cignoni
correct resizing behaviour

Revision 1.2  2005/12/06 10:42:03  vannini
Snapshot dialog now works

Revision 1.1  2005/12/05 18:15:27  vannini
Added snapshot save dialog (not used yet)


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
