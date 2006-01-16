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
Revision 1.7  2006/01/16 05:34:16  cignoni
Added backward qt4.0 compatibility for setAutoFillBackground

Revision 1.6  2006/01/15 15:27:59  glvertex
Added few lines to set background even in qt 4.1

Revision 1.5  2006/01/02 18:54:52  glvertex
added multilevel logging support

Revision 1.4  2006/01/02 17:39:18  glvertex
Added info types in a combobox

Revision 1.3  2005/12/04 16:50:15  glvertex
Removed [using namespace] directive form .h
Renaming in QT style
Adapted method behavior to the new ui interface

Revision 1.2  2005/12/03 22:49:46  cignoni
Added copyright info

****************************************************************************/

#include "customDialog.h"
#include <QPalette>

using namespace vcg;

CustomDialog::CustomDialog(QWidget * parent)
		:QDialog(parent)
{
	ui.setupUi(this);
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ui.pushButtonBottomBg,SIGNAL(clicked()),this,SLOT(setBkgBottomColor()));
	connect(ui.pushButtonTopBg,SIGNAL(clicked()),this,SLOT(setBkgTopColor()));
	connect(ui.pushButtonLogArea,SIGNAL(clicked()),this,SLOT(setLogAreaColor()));
	connect(ui.comboBoxInfoType,SIGNAL(editTextChanged()),this,SLOT(setLogLevel()));
	setFixedSize(260,155);

	// Added for Qt 4.1
#if (QT_VERSION >= 0x040100)
  ui.labelBottmBg->setAutoFillBackground(true);
	ui.labelTopBg->setAutoFillBackground(true);
	ui.labelLogArea->setAutoFillBackground(true);
#endif

	//Error=0, Warning=1, Info=2, Debug=3, Direct=4, OnlyFileLog=5, OnlyConsole=6
	ui.comboBoxInfoType->addItem("Any");
	ui.comboBoxInfoType->addItem("Errors");
	ui.comboBoxInfoType->addItem("Warnigs");
	ui.comboBoxInfoType->addItem("Info");
	ui.comboBoxInfoType->addItem("Debug");
	//ui.comboBoxInfoType->addItem("Direct");
	//ui.comboBoxInfoType->addItem("Only file");
	//ui.comboBoxInfoType->addItem("Only Console");

}


void CustomDialog::loadCurrentSetting(const Color4b& bb,const Color4b& bt,const Color4b& l,short logLevel)
{
	bkgBottomColor=bb;
	bkgTopColor=bt;
 	logAreaColor=l;

	// Changes the palette in the labels
	QPalette pbb(QColor(bb.V(0),bb.V(1),bb.V(2)));
	QPalette pbt(QColor(bt.V(0),bt.V(1),bt.V(2)));
	QPalette pl(QColor(l.V(0),l.V(1),l.V(2)));
	
	ui.comboBoxInfoType->setCurrentIndex(logLevel+1);
	
	
	ui.labelBottmBg->setPalette(pbb);
	ui.labelTopBg->setPalette(pbt);
	ui.labelLogArea->setPalette(pl);
}

void CustomDialog::setBkgBottomColor()
{
	QColor bb=QColorDialog::getColor(QColor(255,255,255,255),this);
	QPalette palette(bb);
	ui.labelBottmBg->setPalette(palette);
	bkgBottomColor.V(0)=bb.red();
	bkgBottomColor.V(1)=bb.green();
	bkgBottomColor.V(2)=bb.blue();
}

void CustomDialog::setBkgTopColor()
{
	QColor bt=QColorDialog::getColor(QColor(255,255,255,255),this);
	QPalette palette(bt);
	ui.labelTopBg->setPalette(palette);
	bkgTopColor.V(0)=bt.red();
	bkgTopColor.V(1)=bt.green();
	bkgTopColor.V(2)=bt.blue();
}

void CustomDialog::setLogAreaColor()
{
	QColor l=QColorDialog::getColor(QColor(255,255,255,255),this);
	QPalette palette(l);
	ui.labelLogArea->setPalette(palette);
	logAreaColor.V(0)=l.red();
	logAreaColor.V(1)=l.green();
	logAreaColor.V(2)=l.blue();
}