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
 Revision 1.1  2006/01/13 23:59:51  fmazzant
 first commit exporter dialog


 ****************************************************************************/

#include "savemaskexporter.h"

SaveMaskExporterDialog::SaveMaskExporterDialog(QWidget *parent) : QDialog(parent)
{
	SaveMaskExporterDialog::ui.setupUi(this);
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(SlotOkButton()));
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(SlotCancelButton()));
}

SaveMaskExporterDialog::SaveMaskExporterDialog(QWidget *parent, int &mask) : QDialog(parent)
{
	SaveMaskExporterDialog::ui.setupUi(this);
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(SlotOkButton()));
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(SlotCancelButton()));
}

//slot
void SaveMaskExporterDialog::SlotOkButton()
{
}

void SaveMaskExporterDialog::SlotCancelButton()
{
}