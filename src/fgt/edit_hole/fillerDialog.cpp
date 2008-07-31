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

#include "fillerDialog.h"

FillerDialog::FillerDialog(QWidget *parent)
	: QDockWidget(parent)
{	
	ui.setupUi(this);
	this->setWidget(ui.frame);
	
	QPoint p=parent->mapToGlobal(QPoint(0,0));
	this->setGeometry(p.x()+(parent->width()-width()),p.y()+40,width(),height() );
	this->setFloating(true);
}

FillerDialog::~FillerDialog() {}


void FillerDialog::closeEvent ( QCloseEvent * event )
{
  emit SGN_Closing();
}

void FillerDialog::on_fillButton_clicked()
{
	emit SGN_ProcessFilling();
}

void FillerDialog::on_cancelButton_clicked()
{
	emit SGN_CancelFill();
}
