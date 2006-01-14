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
 Revision 1.2  2006/01/14 11:23:24  fmazzant
 update savemask exporter with init a mask [base type]

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

SaveMaskExporterDialog::SaveMaskExporterDialog(QWidget *parent, int &mask) : QDialog(parent), mask(mask)
{
	SaveMaskExporterDialog::ui.setupUi(this);
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(SlotOkButton()));
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(SlotCancelButton()));
}

SaveMaskExporterDialog::SaveMaskExporterDialog(QWidget *parent, int &mask, QString type) : QDialog(parent), mask(mask), type(type)
{
	SaveMaskExporterDialog::ui.setupUi(this);
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(SlotOkButton()));
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(SlotCancelButton()));
}

void SaveMaskExporterDialog::Initialize()
{
	if( mask & vcg::tri::io::Mask::IOM_VERTFLAGS    ) {}
	if( mask & vcg::tri::io::Mask::IOM_VERTCOLOR    ) {}
	if( mask & vcg::tri::io::Mask::IOM_VERTQUALITY  ) {}
	if( mask & vcg::tri::io::Mask::IOM_VERTTEXCOORD ) {}
	if( mask & vcg::tri::io::Mask::IOM_VERTNORMAL   ) {}

	if( mask & vcg::tri::io::Mask::IOM_FACEFLAGS    ) {}
	if( mask & vcg::tri::io::Mask::IOM_FACECOLOR    ) {}
	if( mask & vcg::tri::io::Mask::IOM_FACEQUALITY  ) {}
	if( mask & vcg::tri::io::Mask::IOM_FACENORMAL   ) {}

	if( mask & vcg::tri::io::Mask::IOM_WEDGCOLOR    ) {}
	if( mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD ) {}
	if( mask & vcg::tri::io::Mask::IOM_WEDGNORMAL   ) {}

	if( mask & vcg::tri::io::Mask::IOM_CAMERA       ) {}
}

int SaveMaskExporterDialog::GetNewMask()
{
	return this->mask;
}

//slot
void SaveMaskExporterDialog::SlotOkButton()
{
	int newmask = 0;
	
	newmask |= vcg::tri::io::Mask::IOM_VERTQUALITY;
	newmask |= vcg::tri::io::Mask::IOM_FACEQUALITY;

	this->mask=newmask;
}

void SaveMaskExporterDialog::SlotCancelButton()
{
	this->mask=0;
}