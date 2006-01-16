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
 Revision 1.2  2006/01/16 16:19:36  fmazzant
 bug-fix

 Revision 1.1  2006/01/16 15:30:26  fmazzant
 added rename texture dialog for exporter
 removed old maskobj



 ****************************************************************************/

#include "changetexturename.h"
#include <QtGui>

ChangeTextureNameDialog::ChangeTextureNameDialog(QWidget *parent) : QDialog(parent)
{
	ChangeTextureNameDialog::ui.setupUi(this);
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(SlotOkButton()));
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(SlotCancelButton()));
	connect(ui.searchButton,SIGNAL(clicked()),this,SLOT(SlotSearchTextureName()));
	ui.newtexturename->setWindowTitle("Rename Texture");
}

ChangeTextureNameDialog::ChangeTextureNameDialog(QWidget *parent,std::string oldtexture) : QDialog(parent), texture(oldtexture)
{
	ChangeTextureNameDialog::ui.setupUi(this);
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(SlotOkButton()));
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(SlotCancelButton()));
	connect(ui.searchButton,SIGNAL(clicked()),this,SLOT(SlotSearchTextureName()));
	ui.newtexturename->setWindowTitle("Rename Texture");
	ui.newtexturename->setText(QString(texture.c_str()));
}


void ChangeTextureNameDialog::SlotOkButton()
{
	this->texture = ui.newtexturename->text().toStdString();
	ui.newtexturename->setText(QString(texture.c_str()));
}

void ChangeTextureNameDialog::SlotCancelButton()
{
	this->texture = "";
}

void ChangeTextureNameDialog::SlotSearchTextureName()
{
	this->texture = QFileDialog::getOpenFileName(new QWidget(),tr("Open Image File"),".").toStdString();
	if(this->texture.size() > 0)
		ui.newtexturename->setText(QString(texture.c_str()));
}