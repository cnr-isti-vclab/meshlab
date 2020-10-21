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
	someBridge = false;
	ui.setupUi(this);
	this->setWidget(ui.frame_main);
	ui.bridgeTab->setEnabled(false);

	QPoint p=parent->mapToGlobal(QPoint(0,0));
	this->setGeometry(p.x()+(parent->width()-width()),p.y()+58,width(),height() );
	this->setFloating(true);
}

FillerDialog::~FillerDialog() {}


void FillerDialog::closeEvent ( QCloseEvent * /*event*/ )
{
	emit SGN_Closing();
}

void FillerDialog::clickStartBridging()
{
	ui.manualBridgeBtn->setText("End");
	ui.acceptBridgeBtn->setEnabled(false);
	ui.autoBridgeBtn->setEnabled(false);
	ui.clearBridgeBtn->setEnabled(false);
	ui.selfHoleChkB->setEnabled(false);
	ui.nmHoleClosureBtn->setEnabled(false);
	ui.label_sld_dx->setEnabled(false);
	ui.label_sld_sx->setEnabled(false);
	ui.bridgeParamSld->setEnabled(false);
}

void FillerDialog::clickEndBridging()
{
	ui.manualBridgeBtn->setText("Start Manual Bridging");
	ui.autoBridgeBtn->setEnabled(true);
	ui.selfHoleChkB->setEnabled(true);
	ui.nmHoleClosureBtn->setEnabled(true);
	
	ui.clearBridgeBtn->setEnabled(someBridge);
	ui.acceptBridgeBtn->setEnabled(someBridge);

	if(ui.selfHoleChkB->isChecked())
	{
		ui.bridgeParamSld->setEnabled(true);
		ui.label_sld_dx->setEnabled(true);
		ui.label_sld_sx->setEnabled(true);
	}
}

void FillerDialog::clickSingleHoleBridgeOpt()
{
	bool check = ui.selfHoleChkB->isChecked();
	ui.label_sld_sx->setEnabled(check);
	ui.label_sld_dx->setEnabled(check);
	ui.bridgeParamSld->setEnabled(check);
}

void FillerDialog::SLOT_ExistBridge(bool exist)
{
	someBridge = exist;
	ui.clearBridgeBtn->setEnabled(exist);
	ui.acceptBridgeBtn->setEnabled(exist);
}
