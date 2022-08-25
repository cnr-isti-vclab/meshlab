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

#include "congrats_dialog.h"
#include "ui_congrats_dialog.h"

#include <common/mlapplication.h>

#include <QSettings>
#include <QDesktopServices>

CongratsDialog::CongratsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CongratsDialog)
{
	ui->setupUi(this);
}

CongratsDialog::~CongratsDialog()
{
	delete ui;
}

void CongratsDialog::on_githubButton_clicked()
{
	QDesktopServices::openUrl(QUrl("https://github.com/cnr-isti-vclab/meshlab/discussions/new"));
}

void CongratsDialog::on_emailButton_clicked()
{
	QDesktopServices::openUrl(QUrl("mailto:paolo.cignoni@isti.cnr.it;alessandro.muntoni@isti.cnr.it?subject=[MeshLab] Reporting Info on MeshLab Usage - V"+MeshLabApplication::appVer()));
}

void CongratsDialog::on_closeButton_clicked()
{
	updateDontShowSetting();
	close();
}

void CongratsDialog::updateDontShowSetting()
{
	const QString dontRemindMeToSendEmailVar("dontRemindMeToSendEmail");
	QSettings settings;
	//This preference values store when you did the last request for a mail
	int congratsMeshCounter = settings.value("congratsMeshCounter", 50).toInt();
	settings.setValue("congratsMeshCounter", congratsMeshCounter * 2);
	if (ui->dontShowCheckBox->isChecked())
		settings.setValue(dontRemindMeToSendEmailVar, true);
	else
		settings.setValue(dontRemindMeToSendEmailVar, false);
}
