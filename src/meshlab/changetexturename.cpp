/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004                                                \/)\/    *
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
 Revision 1.2  2007/03/26 08:25:09  zifnab1974
 added eol at the end of the files

 Revision 1.1  2006/01/26 18:39:19  fmazzant
 moved mask dialog exporter from mashio to meshlab

 Revision 1.4  2006/01/19 09:51:04  fmazzant
 cleaned code

 Revision 1.3  2006/01/16 19:45:40  fmazzant
 deleted small error

 Revision 1.2  2006/01/16 16:19:36  fmazzant
 bug-fix

 Revision 1.1  2006/01/16 15:30:26  fmazzant
 added rename texture dialog for exporter
 removed old maskobj



 ****************************************************************************/

#include "ui_renametexture.h"
#include "changetexturename.h"
#include <QtGui>

ChangeTextureNameDialog::ChangeTextureNameDialog(QWidget *parent) : QDialog(parent)
{
	ui = new Ui::RenameTextureDialog();
	InitDialog();
}

ChangeTextureNameDialog::ChangeTextureNameDialog(QWidget *parent,std::string oldtexture) : QDialog(parent), texture(oldtexture)
{
	InitDialog();
	ui->newtexturename->setText(QString(texture.c_str()));
}

void ChangeTextureNameDialog::InitDialog()
{
	ChangeTextureNameDialog::ui->setupUi(this);
	connect(ui->okButton, SIGNAL(clicked()), this, SLOT(SlotOkButton()));
	connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(SlotCancelButton()));
	connect(ui->searchButton,SIGNAL(clicked()),this,SLOT(SlotSearchTextureName()));
	ui->newtexturename->setWindowTitle("Rename Texture");
}

void ChangeTextureNameDialog::SlotOkButton()
{
	this->texture = ui->newtexturename->text().toStdString();
	ui->newtexturename->setText(QString(texture.c_str()));
}

void ChangeTextureNameDialog::SlotCancelButton()
{
	this->texture = "";
}

void ChangeTextureNameDialog::SlotSearchTextureName()
{
	this->texture = QFileDialog::getOpenFileName(new QWidget(),tr("Open Image File"),".").toStdString();
	if(this->texture.size() > 0)
	{
		QStringList lists = QString(texture.c_str()).split('/');
		ui->newtexturename->setText(lists[lists.size()-1]);
	}
}

ChangeTextureNameDialog::~ChangeTextureNameDialog()
{
	delete ui;
}