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
 Revision 1.3  2005/12/06 15:54:59  fmazzant
 update dialog export obj

 Revision 1.2  2005/12/03 09:45:42  fmazzant
 adding to mask how much we save a obj file format. the mask taken from the dialogue window SaveMaskDialog.

 Revision 1.1  2005/12/02 17:40:26  fmazzant
 added dialog obj exporter.


*****************************************************************************/

#include "savemaskdialog.h"
#include <wrap/ply/io_mask.h>
#include <QMessageBox>
#include <fstream>
#include <iostream>

SaveMaskDialog::SaveMaskDialog(QWidget *parent): QDialog(parent)
{
	SaveMaskDialog::ui.setupUi(this);
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(SlotOkButton()));
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(SlotCancelButton()));
}

//slot
void SaveMaskDialog::SlotOkButton()
{
	Mask mymask;
	mymask.binary = SaveMaskDialog::ui.binaryButton->isChecked();
	mymask.faces = SaveMaskDialog::ui.faceCheck->isChecked();
	mymask.normal = SaveMaskDialog::ui.normalCheck->isChecked();
	mymask.texture = SaveMaskDialog::ui.textureCheck->isChecked();
	mymask.vertexs = SaveMaskDialog::ui.vertexCheck->isChecked();
	mymask.colorV = SaveMaskDialog::ui.vertexCheck->isChecked();
	if(WriteMask(&mymask))
		accept();
	else
		QMessageBox::warning(new QWidget(),"Error","Parameters not valid!");
}

void SaveMaskDialog::SlotCancelButton()
{
	
}

//member
bool SaveMaskDialog::ReadMask()
{
	Mask mask;
	std::ifstream in("./mask.ini");
	if (in.fail()){return false;} 
	std::string line;
	std::getline(in, line, '\n');	
	bool ok = false;

	if(line.size()==Mask::args)
	{
		mask.binary = QString(line.at(0)).toInt(&ok, 10); 
		mask.faces = QString(line.at(1)).toInt(&ok, 10); 
		mask.normal = QString(line.at(2)).toInt(&ok, 10); 
		mask.texture = QString(line.at(3)).toInt(&ok, 10); 
		mask.vertexs = QString(line.at(4)).toInt(&ok, 10);
		mask.colorV = QString(line.at(5)).toInt(&ok, 10);
	}
	else
	{
		mask.binary = 0;
		mask.faces = 0;
		mask.normal = 0;
		mask.texture = 0;
		mask.vertexs = 0;
		mask.colorV = 0;
	}

	SaveMaskDialog::ui.binaryButton->setChecked((bool)mask.binary);
	SaveMaskDialog::ui.asciiButton->setChecked(!((bool)mask.binary));
	SaveMaskDialog::ui.faceCheck->setChecked((bool)mask.faces);
	SaveMaskDialog::ui.normalCheck->setChecked((bool)mask.normal);
	SaveMaskDialog::ui.textureCheck->setChecked((bool)mask.texture);
	SaveMaskDialog::ui.vertexCheck->setChecked((bool)mask.vertexs);
	SaveMaskDialog::ui.colorVertexCheck->setChecked((bool)mask.colorV);

	in.close();
	return true;
}


//static
Mask SaveMaskDialog::GetMask()
{
	Mask mask;
	std::ifstream in("./mask.ini");
	if (in.fail()){return mask;} 
	std::string line;
	std::getline(in, line, '\n');
	bool ok = false;
	mask.binary = QString(line.at(0)).toInt(&ok, 10); 
	mask.faces = QString(line.at(1)).toInt(&ok, 10); 
	mask.normal = QString(line.at(2)).toInt(&ok, 10); 
	mask.texture = QString(line.at(3)).toInt(&ok, 10); 
	mask.vertexs = QString(line.at(4)).toInt(&ok, 10); 
	mask.colorV = QString(line.at(5)).toInt(&ok, 10);
	return mask;
}

bool SaveMaskDialog::WriteMask(Mask *mask)
{
	std::ofstream out("./mask.ini");
	if (out.fail()){return false;}
	out << mask->binary << mask->faces << mask->normal << mask->texture << mask->vertexs << mask->colorV << std::endl;
	out.close();
	return true;
}
int SaveMaskDialog::MaskToInt(Mask *mymask)
{
	int mask;
	
	if(mymask->texture)	{mask |= vcg::ply::PLYMask::PM_WEDGTEXCOORD;}
	if(mymask->normal)	{mask |= vcg::ply::PLYMask::PM_WEDGNORMAL;}
	if(mymask->colorV)	{mask |= vcg::ply::PLYMask::PM_VERTCOLOR; mask |= vcg::ply::PLYMask::PM_FACECOLOR;}
	if(mymask->faces)	{mask |= vcg::ply::PLYMask::PM_FACEQUALITY;}
	if(mymask->vertexs)	{mask |= vcg::ply::PLYMask::PM_VERTQUALITY;}

	return mask;
}