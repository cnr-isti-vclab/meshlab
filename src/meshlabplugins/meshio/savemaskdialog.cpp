/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
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
 Revision 1.8  2006/01/19 09:36:28  fmazzant
 cleaned up history log

 Revision 1.7  2006/01/10 16:52:19  fmazzant
 update ply::PlyMask -> io::Mask

 Revision 1.6  2005/12/13 14:02:50  fmazzant
 added the rescue of the materials of the obj

 Revision 1.5  2005/12/09 16:37:20  fmazzant
 maskobj for select element to save

 Revision 1.4  2005/12/07 07:52:25  fmazzant
 export obj generic(base)

*****************************************************************************/

#include "savemaskdialog.h"
#include <wrap/io_trimesh/io_mask.h>
#include <QMessageBox>
#include <fstream>
#include <iostream>

SaveMaskDialog::SaveMaskDialog(QWidget *parent): mask(new MaskObj()), QDialog(parent)
{
	SaveMaskDialog::ui.setupUi(this);
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(SlotOkButton()));
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(SlotCancelButton()));
}

SaveMaskDialog::SaveMaskDialog(MaskObj *mask, QWidget *parent): mask(mask), QDialog(parent)
{
	SaveMaskDialog::ui.setupUi(this);
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(SlotOkButton()));
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(SlotCancelButton()));
}

//slot
void SaveMaskDialog::SlotOkButton()
{
	mask->isfirst = false;
	mask->binary = ui.binaryButton->isChecked();
	mask->normal = ui.normalCheck->isChecked();
	mask->texture = ui.textureCheck->isChecked();
	mask->colorF = ui.colorFaceCheck->isChecked();
	accept();
}

void SaveMaskDialog::SlotCancelButton()
{
	
}


bool SaveMaskDialog::ReadMask()
{
	if(mask->binary)
		ui.binaryButton->setChecked(true);
	else
		ui.asciiButton->setChecked(true);
	
	ui.faceCheck->setChecked(true);
	ui.vertexCheck->setChecked(true);

	ui.normalCheck->setChecked(mask->normal);
	ui.textureCheck->setChecked(mask->texture);
	ui.colorFaceCheck->setChecked(mask->colorF);

	return true;
}