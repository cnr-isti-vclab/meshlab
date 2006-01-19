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
 Revision 1.17  2006/01/19 12:45:00  fmazzant
 deleted SaveMaskExporterDialog::Initialize()

 Revision 1.16  2006/01/19 09:25:28  fmazzant
 cleaned code & deleted history log

 Revision 1.15  2006/01/18 14:57:25  fmazzant
 added Lib3dsNode in export_3ds

 Revision 1.14  2006/01/18 00:44:27  fmazzant
 added control for unchecked wedgytexcood when textures is empty

 Revision 1.13  2006/01/17 13:48:54  fmazzant
 added capability mask on export file format

 ****************************************************************************/
#include <Qt>
#include <QtGui>

#include "savemaskexporter.h"
#include "changetexturename.h"

SaveMaskExporterDialog::SaveMaskExporterDialog(QWidget *parent,MeshModel *m,int type,int capability): QDialog(parent),m(m),type(type),capability(capability)
{
	InitDialog();
}

void SaveMaskExporterDialog::InitDialog()
{
	SaveMaskExporterDialog::ui.setupUi(this);
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(SlotOkButton()));
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(SlotCancelButton()));
	connect(ui.renametextureButton,SIGNAL(clicked()),this,SLOT(SlotRenameTexture()));
	connect(ui.listTextureName,SIGNAL(itemSelectionChanged()),this,SLOT(SlotSelectionTextureName()));
	connect(ui.AllButton,SIGNAL(clicked()),this,SLOT(SlotSelectionAllButton()));
	connect(ui.NoneButton,SIGNAL(clicked()),this,SLOT(SlotSelectionNoneButton()));
	ui.renametextureButton->setDisabled(true);
	
	//disabled
	//check globali disabilitati
	ui.check_iom_vertquality->setDisabled(true);
	ui.check_iom_facequality->setDisabled(true);

	//all - none
	ui.AllButton->setChecked(false);
	ui.NoneButton->setChecked(true);

	//checked
	ui.check_iom_vertquality->setChecked(true);
	ui.check_iom_facequality->setChecked(true);

	//Initialize();
	SetWindowTitle();
	SetTextureName();
	SetMaskCapability();
}

/*
	Questo metodo serve per specificare le varie personalizzazioni dei vari salvataggi.
*/
void SaveMaskExporterDialog::SetWindowTitle()
{
	switch(type)
	{
	case vcg::tri::io::SaveMaskToExporter::_OBJ:
		{
			this->setWindowTitle("Edit Options OBJ");
			break;
		}
	case vcg::tri::io::SaveMaskToExporter::_PLY:
		{
			this->setWindowTitle("Edit Options PLY");
			break;
		}
	case vcg::tri::io::SaveMaskToExporter::_OFF:
		{
			this->setWindowTitle("Edit Options OFF");
			break;
		}
	case vcg::tri::io::SaveMaskToExporter::_STL:
		{
			this->setWindowTitle("Edit Options STL");
			break;
		}
	case vcg::tri::io::SaveMaskToExporter::_3DS:
		{
			this->setWindowTitle("Edit Options 3DS");
			break;
		}
	default:
		{
			this->mask = 0;
			break;
		}
	}
}

void SaveMaskExporterDialog::SetTextureName()
{
	if( m->cm.textures.size() == 0 )
	{
		ui.check_iom_wedgtexcoord->setDisabled(true);
		ui.check_iom_wedgtexcoord->setChecked(false);
	}

	for(unsigned int i=0;i<m->cm.textures.size();i++)
	{
		QString item(m->cm.textures[i].c_str());
		ui.listTextureName->addItem(item);
	}
}

int SaveMaskExporterDialog::GetNewMask()
{
	return this->mask;
}

void SaveMaskExporterDialog::SetMaskCapability()
{
	//vert
	if((capability & vcg::tri::io::Mask::IOM_VERTFLAGS)==0)
	{
		ui.check_iom_vertflags->setDisabled(true);
		ui.check_iom_vertflags->setChecked(false);
	}

	if((capability & vcg::tri::io::Mask::IOM_VERTCOLOR)==0)
	{
		ui.check_iom_vertcolor->setDisabled(true);
		ui.check_iom_vertcolor->setChecked(false);
	}

	if((capability & vcg::tri::io::Mask::IOM_VERTQUALITY)==0)
	{
		//none...force!!
	}

	if((capability & vcg::tri::io::Mask::IOM_VERTTEXCOORD)==0)
	{
		ui.check_iom_verttexcoord->setDisabled(true);
		ui.check_iom_verttexcoord->setChecked(false);
	}

	if((capability & vcg::tri::io::Mask::IOM_VERTNORMAL)==0)
	{
		ui.check_iom_vertnormal->setDisabled(true);
		ui.check_iom_vertnormal->setChecked(false);
	}

	//face
	if((capability & vcg::tri::io::Mask::IOM_FACEFLAGS)==0)
	{
		ui.check_iom_faceflags->setDisabled(true);
		ui.check_iom_faceflags->setChecked(false);
	}

	if((capability & vcg::tri::io::Mask::IOM_FACECOLOR)==0)
	{
		ui.check_iom_facecolor->setDisabled(true);
		ui.check_iom_facecolor->setChecked(false);
	}

	if((capability & vcg::tri::io::Mask::IOM_FACEQUALITY)==0)
	{
		//none ....force!!
	}

	if((capability & vcg::tri::io::Mask::IOM_FACENORMAL)==0)
	{
		ui.check_iom_facenormal->setDisabled(true);
		ui.check_iom_facenormal->setChecked(false);
	}

	//wedg
	if((capability & vcg::tri::io::Mask::IOM_WEDGCOLOR)==0)
	{
		ui.check_iom_wedgcolor->setDisabled(true);
		ui.check_iom_wedgcolor->setChecked(false);
	}

	if((capability & vcg::tri::io::Mask::IOM_WEDGTEXCOORD)==0)
	{
		ui.check_iom_wedgtexcoord->setDisabled(true);
		ui.check_iom_wedgtexcoord->setChecked(false);
	}

	if((capability & vcg::tri::io::Mask::IOM_WEDGNORMAL)==0)
	{
		ui.check_iom_wedgnormal->setDisabled(true);
		ui.check_iom_wedgnormal->setChecked(false);
	}

	if((capability & vcg::tri::io::Mask::IOM_CAMERA)==0)
	{
		ui.check_iom_camera->setDisabled(true);
		ui.check_iom_camera->setChecked(false);
	}

	if(capability == 0)
		ui.NoneButton->setChecked(true);
}

//slot
void SaveMaskExporterDialog::SlotOkButton()
{
	int newmask = 0;
	
	newmask |= vcg::tri::io::Mask::IOM_VERTQUALITY;
	newmask |= vcg::tri::io::Mask::IOM_FACEQUALITY;

	if( ui.check_iom_vertflags->isChecked()		) { newmask |= vcg::tri::io::Mask::IOM_VERTFLAGS;}
	if( ui.check_iom_vertcolor->isChecked()		) { newmask |= vcg::tri::io::Mask::IOM_VERTCOLOR;}
	//if( ui.check_iom_vertquality->isChecked()	) { newmask |= vcg::tri::io::Mask::IOM_VERTQUALITY;}
	if( ui.check_iom_verttexcoord->isChecked()	) { newmask |= vcg::tri::io::Mask::IOM_VERTTEXCOORD;}
	if( ui.check_iom_vertnormal->isChecked()	) { newmask |= vcg::tri::io::Mask::IOM_VERTNORMAL;}

	if( ui.check_iom_faceflags->isChecked()		) { newmask |= vcg::tri::io::Mask::IOM_FACEFLAGS;}
	if( ui.check_iom_facecolor->isChecked()		) { newmask |= vcg::tri::io::Mask::IOM_FACECOLOR;}
	//if( ui.check_iom_facequality->isChecked()	) { newmask |= vcg::tri::io::Mask::IOM_FACEQUALITY;}
	if( ui.check_iom_facenormal->isChecked()	) { newmask |= vcg::tri::io::Mask::IOM_FACENORMAL;}

	if( ui.check_iom_wedgcolor->isChecked()		) { newmask |= vcg::tri::io::Mask::IOM_WEDGCOLOR;}
	if( ui.check_iom_wedgtexcoord->isChecked()	) { newmask |= vcg::tri::io::Mask::IOM_WEDGTEXCOORD;}
	if( ui.check_iom_wedgnormal->isChecked()	) { newmask |= vcg::tri::io::Mask::IOM_WEDGNORMAL;}

	if( ui.check_iom_camera->isChecked()		) { newmask |= vcg::tri::io::Mask::IOM_CAMERA;}

	for(unsigned int i=0;i<m->cm.textures.size();i++)
	{
		m->cm.textures[i] = ui.listTextureName->item(i)->text().toStdString();
	}
	this->mask=newmask;
}

void SaveMaskExporterDialog::SlotCancelButton()
{
	this->mask=0;
}

void SaveMaskExporterDialog::SlotRenameTexture()
{
	int row = ui.listTextureName->currentRow();
	std::string newtexture = vcg::tri::io::TextureRename::GetNewTextureName(m->cm.textures[row].c_str());
	if(newtexture.size()>0)
	{
		QStringList lists = QString(newtexture.c_str()).split('/');
		(ui.listTextureName->currentItem())->setText(lists[lists.size()-1]);
	}
}

void SaveMaskExporterDialog::SlotSelectionTextureName()
{
	
}

void SaveMaskExporterDialog::SlotSelectionAllButton()
{
	//vert
	ui.check_iom_vertflags->setChecked(ui.check_iom_vertflags->isEnabled());
	ui.check_iom_vertcolor->setChecked(ui.check_iom_vertcolor->isEnabled());
	ui.check_iom_verttexcoord->setChecked(ui.check_iom_verttexcoord->isEnabled());
	ui.check_iom_vertnormal->setChecked(ui.check_iom_vertnormal->isEnabled());

	//face
	ui.check_iom_faceflags->setChecked(ui.check_iom_faceflags->isEnabled());
	ui.check_iom_facenormal->setChecked(ui.check_iom_facenormal->isEnabled());
	ui.check_iom_facecolor->setChecked(ui.check_iom_facecolor->isEnabled());
	
	//wedg
	ui.check_iom_wedgcolor->setChecked(ui.check_iom_wedgcolor->isEnabled());
	ui.check_iom_wedgtexcoord->setChecked(ui.check_iom_wedgtexcoord->isEnabled());
	ui.check_iom_wedgnormal->setChecked(ui.check_iom_wedgnormal->isEnabled());
}

void SaveMaskExporterDialog::SlotSelectionNoneButton()
{
	//vert
	ui.check_iom_vertflags->setChecked((ui.check_iom_vertflags->isEnabled()& ui.check_iom_vertflags->isChecked()) && !ui.check_iom_vertflags->isChecked());
	ui.check_iom_vertcolor->setChecked((ui.check_iom_vertcolor->isEnabled() & ui.check_iom_vertcolor->isChecked())&& !ui.check_iom_vertcolor->isChecked());
	ui.check_iom_verttexcoord->setChecked((ui.check_iom_verttexcoord->isEnabled() & ui.check_iom_verttexcoord->isChecked())&& !ui.check_iom_verttexcoord->isChecked());
	ui.check_iom_vertnormal->setChecked((ui.check_iom_vertnormal->isEnabled() & ui.check_iom_vertnormal->isChecked())&& !ui.check_iom_vertnormal->isChecked());

	//face
	ui.check_iom_faceflags->setChecked((ui.check_iom_faceflags->isEnabled() & ui.check_iom_faceflags->isChecked())&& !ui.check_iom_faceflags->isChecked());
	ui.check_iom_facenormal->setChecked((ui.check_iom_facenormal->isEnabled()& ui.check_iom_facenormal->isChecked())&& !ui.check_iom_facenormal->isChecked());
	ui.check_iom_facecolor->setChecked((ui.check_iom_facecolor->isEnabled()& ui.check_iom_facecolor->isChecked())&& !ui.check_iom_facecolor->isChecked());
	
	//wedg
	ui.check_iom_wedgcolor->setChecked((ui.check_iom_wedgcolor->isEnabled() & ui.check_iom_wedgcolor->isChecked())&& !ui.check_iom_wedgcolor->isChecked());
	ui.check_iom_wedgtexcoord->setChecked((ui.check_iom_wedgtexcoord->isEnabled() & ui.check_iom_wedgtexcoord->isChecked())&& !ui.check_iom_wedgtexcoord->isChecked());
	ui.check_iom_wedgnormal->setChecked((ui.check_iom_wedgnormal->isEnabled() & ui.check_iom_wedgnormal->isChecked())&& !ui.check_iom_wedgnormal->isChecked());
}