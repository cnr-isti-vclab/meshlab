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
 Revision 1.5  2006/01/15 08:51:29  fmazzant
 added mask specificy in 3ds code

 Revision 1.4  2006/01/15 00:45:39  fmazzant
 extend mask exporter for all type file format +

 Revision 1.3  2006/01/14 14:12:07  fmazzant
 sample for use save's mask exporter. ony 3ds.

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

SaveMaskExporterDialog::SaveMaskExporterDialog(QWidget *parent, int &mask, int type) : QDialog(parent), mask(mask), type(type)
{
	SaveMaskExporterDialog::ui.setupUi(this);
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(SlotOkButton()));
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(SlotCancelButton()));
}

void SaveMaskExporterDialog::Initialize()
{
	if( mask & vcg::tri::io::Mask::IOM_VERTFLAGS    ) {ui.check_iom_vertflags->setChecked(true);}
	if( mask & vcg::tri::io::Mask::IOM_VERTCOLOR    ) {ui.check_iom_vertcolor->setChecked(true);}
	if( mask & vcg::tri::io::Mask::IOM_VERTQUALITY  ) {ui.check_iom_vertquality->setChecked(true);}
	if( mask & vcg::tri::io::Mask::IOM_VERTTEXCOORD ) {ui.check_iom_verttexcoord->setChecked(true);}
	if( mask & vcg::tri::io::Mask::IOM_VERTNORMAL   ) {ui.check_iom_vertnormal->setChecked(true);}

	if( mask & vcg::tri::io::Mask::IOM_FACEFLAGS    ) {ui.check_iom_faceflags->setChecked(true);}
	if( mask & vcg::tri::io::Mask::IOM_FACECOLOR    ) {ui.check_iom_facecolor->setChecked(true);}
	if( mask & vcg::tri::io::Mask::IOM_FACEQUALITY  ) {ui.check_iom_facequality->setChecked(true);}
	if( mask & vcg::tri::io::Mask::IOM_FACENORMAL   ) {ui.check_iom_facenormal->setChecked(true);}

	if( mask & vcg::tri::io::Mask::IOM_WEDGCOLOR    ) {ui.check_iom_wedgcolor->setChecked(true);}
	if( mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD ) {ui.check_iom_verttexcoord->setChecked(true);}
	if( mask & vcg::tri::io::Mask::IOM_WEDGNORMAL   ) {ui.check_iom_wedgnormal->setChecked(true);}

	if( mask & vcg::tri::io::Mask::IOM_CAMERA       ) {ui.check_iom_camera->setChecked(true);}

	SetDisableChecks(type);
}

/*
	Questo metodo serve per specificare le varie personalizzazioni dei vari salvataggi.
*/
void SaveMaskExporterDialog::SetDisableChecks(int type)
{
	//check globali disabilitati
	ui.check_iom_vertquality->setDisabled(true);
	ui.check_iom_facequality->setDisabled(true);	
	
	//all - none - camera
	ui.check_iom_all->setDisabled(true);
	ui.check_iom_none->setDisabled(true);
	ui.check_iom_camera->setDisabled(true);
	
	//vert
	ui.check_iom_vertflags->setDisabled(true);
	ui.check_iom_vertcolor->setDisabled(true);
	ui.check_iom_verttexcoord->setDisabled(true);
	ui.check_iom_vertnormal->setDisabled(true);

	//face
	ui.check_iom_facenormal->setDisabled(true);
	ui.check_iom_facecolor->setDisabled(true);
	ui.check_iom_faceflags->setDisabled(true);

	//wedg
	ui.check_iom_wedgcolor->setDisabled(true);
	ui.check_iom_wedgtexcoord->setDisabled(true);
	ui.check_iom_wedgnormal->setDisabled(true);

	switch(type)
	{
	case vcg::tri::io::SaveMaskToExporter::_OBJ:
		{
			this->setWindowTitle("Edit Options OBJ");
			
			ui.check_iom_vertnormal->setDisabled(false);
			ui.check_iom_facecolor->setDisabled(false);
			ui.check_iom_wedgtexcoord->setDisabled(false);
			
			break;
		}
	case vcg::tri::io::SaveMaskToExporter::_PLY:
		{
			this->setWindowTitle("Edit Options PLY");

			ui.check_iom_wedgtexcoord->setDisabled(false);
			ui.check_iom_wedgtexcoord->setChecked(true);
			
			break;
		}
	case vcg::tri::io::SaveMaskToExporter::_OFF:
		{
			this->setWindowTitle("Edit Options OFF");

			ui.check_iom_wedgtexcoord->setDisabled(false);

			break;
		}
	case vcg::tri::io::SaveMaskToExporter::_STL:
		{
			this->setWindowTitle("Edit Options STL");

			ui.check_iom_wedgtexcoord->setDisabled(false);

			break;
		}
	case vcg::tri::io::SaveMaskToExporter::_3DS:
		{
			this->setWindowTitle("Edit Options 3DS");

			ui.check_iom_facenormal->setDisabled(false);
			ui.check_iom_facecolor->setDisabled(false);
			ui.check_iom_wedgtexcoord->setDisabled(false);
			ui.check_iom_faceflags->setDisabled(false);

			break;
		}
	default:
		{
			this->mask = 0;
			break;
		}
	}
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

	if( ui.check_iom_vertflags->isChecked()		) { mask |= vcg::tri::io::Mask::IOM_VERTFLAGS;}
	if( ui.check_iom_vertcolor->isChecked()		) { mask |= vcg::tri::io::Mask::IOM_VERTCOLOR;}
	if( ui.check_iom_vertquality->isChecked()	) { mask |= vcg::tri::io::Mask::IOM_VERTQUALITY;}
	if( ui.check_iom_verttexcoord->isChecked()	) { mask |= vcg::tri::io::Mask::IOM_VERTTEXCOORD;}
	if( ui.check_iom_vertnormal->isChecked()	) { mask |= vcg::tri::io::Mask::IOM_VERTNORMAL;}

	if( ui.check_iom_faceflags->isChecked()		) { mask |= vcg::tri::io::Mask::IOM_FACEFLAGS;}
	if( ui.check_iom_facecolor->isChecked()		) { mask |= vcg::tri::io::Mask::IOM_FACECOLOR;}
	if( ui.check_iom_facequality->isChecked()	) { mask |= vcg::tri::io::Mask::IOM_FACEQUALITY;}
	if( ui.check_iom_facenormal->isChecked()	) { mask |= vcg::tri::io::Mask::IOM_FACENORMAL;}

	if( ui.check_iom_wedgcolor->isChecked()		) { mask |= vcg::tri::io::Mask::IOM_WEDGCOLOR;}
	if( ui.check_iom_wedgtexcoord->isChecked()	) { mask |= vcg::tri::io::Mask::IOM_WEDGTEXCOORD;}
	if( ui.check_iom_wedgnormal->isChecked()	) { mask |= vcg::tri::io::Mask::IOM_WEDGNORMAL;}

	if( ui.check_iom_camera->isChecked()		) { mask |= vcg::tri::io::Mask::IOM_CAMERA;}

	this->mask=newmask;
}

void SaveMaskExporterDialog::SlotCancelButton()
{
	this->mask=0;
}