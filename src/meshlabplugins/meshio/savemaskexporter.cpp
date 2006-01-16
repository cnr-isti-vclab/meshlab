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
 Revision 1.11  2006/01/16 23:53:22  fmazzant
 bux-fix MeshModel &m -> MeshModel *m

 Revision 1.10  2006/01/16 19:45:40  fmazzant
 deleted small error

 Revision 1.9  2006/01/16 16:19:36  fmazzant
 bug-fix

 Revision 1.8  2006/01/16 15:56:00  fmazzant
 bug-fix mask -> newmask

 Revision 1.7  2006/01/16 15:30:26  fmazzant
 added rename texture dialog for exporter
 removed old maskobj

 Revision 1.6  2006/01/16 11:49:48  fmazzant
  added base texture name option.

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
#include <Qt>
#include <QtGui>

#include "savemaskexporter.h"
#include "changetexturename.h"

SaveMaskExporterDialog::SaveMaskExporterDialog(QWidget *parent) : QDialog(parent)
{
	SaveMaskExporterDialog::ui.setupUi(this);
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(SlotOkButton()));
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(SlotCancelButton()));
	connect(ui.renametextureButton,SIGNAL(clicked),this,SLOT(SlotRenameTexture()));
	connect(ui.listTextureName,SIGNAL(itemSelectionChanged()),this,SLOT(SlotSelectionTextureName()));
	ui.renametextureButton->setDisabled(true);
}

SaveMaskExporterDialog::SaveMaskExporterDialog(QWidget *parent, int &mask) : QDialog(parent), mask(mask)
{
	SaveMaskExporterDialog::ui.setupUi(this);
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(SlotOkButton()));
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(SlotCancelButton()));
	connect(ui.renametextureButton,SIGNAL(clicked),this,SLOT(SlotRenameTexture()));
	connect(ui.listTextureName,SIGNAL(itemSelectionChanged()),this,SLOT(SlotSelectionTextureName()));
	ui.renametextureButton->setDisabled(true);
}

SaveMaskExporterDialog::SaveMaskExporterDialog(QWidget *parent, int &mask, int type) : QDialog(parent), mask(mask), type(type)
{
	SaveMaskExporterDialog::ui.setupUi(this);
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(SlotOkButton()));
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(SlotCancelButton()));
	connect(ui.renametextureButton,SIGNAL(clicked),this,SLOT(SlotRenameTexture()));
	connect(ui.listTextureName,SIGNAL(itemSelectionChanged()),this,SLOT(SlotSelectionTextureName()));
	ui.renametextureButton->setDisabled(true);
}

SaveMaskExporterDialog::SaveMaskExporterDialog(QWidget *parent,MeshModel *m,int type): QDialog(parent),m(m),type(type)
{
	SaveMaskExporterDialog::ui.setupUi(this);
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(SlotOkButton()));
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(SlotCancelButton()));
	connect(ui.renametextureButton,SIGNAL(clicked()),this,SLOT(SlotRenameTexture()));
	connect(ui.listTextureName,SIGNAL(itemSelectionChanged()),this,SLOT(SlotSelectionTextureName()));
	ui.renametextureButton->setDisabled(true);
}

void SaveMaskExporterDialog::Initialize()
{
	//disabled
	//check globali disabilitati
	ui.check_iom_vertquality->setDisabled(true);
	ui.check_iom_facequality->setDisabled(true);	
	
	//all - none - camera
	ui.check_iom_all->setDisabled(true);
	ui.check_iom_none->setDisabled(true);
	ui.check_iom_camera->setDisabled(true);
	
	//vert
	ui.check_iom_vertflags->setDisabled(true);
	ui.check_iom_vertcolor->setDisabled(!m->cm.HasPerVertexColor());
	ui.check_iom_verttexcoord->setDisabled(!m->cm.HasPerVertexTexture());
	ui.check_iom_vertnormal->setDisabled(!m->cm.HasPerVertexNormal());

	//face
	ui.check_iom_facenormal->setDisabled(!m->cm.HasPerFaceNormal());
	ui.check_iom_facecolor->setDisabled(!m->cm.HasPerFaceColor());
	ui.check_iom_faceflags->setDisabled(true);

	//wedg
	ui.check_iom_wedgcolor->setDisabled(!m->cm.HasPerWedgeColor());
	ui.check_iom_wedgtexcoord->setDisabled(!m->cm.HasPerWedgeTexture());
	ui.check_iom_wedgnormal->setDisabled(!m->cm.HasPerWedgeNormal());

	//checked
	ui.check_iom_vertquality->setChecked(true);
	ui.check_iom_facequality->setChecked(true);
	
	//vert
	ui.check_iom_vertcolor->setChecked(m->cm.HasPerVertexColor());
	//ui.check_iom_vertquality->setChecked(m.cm.HasPerVertexQuality());
	ui.check_iom_verttexcoord->setChecked(m->cm.HasPerVertexTexture());
	ui.check_iom_vertnormal->setChecked(m->cm.HasPerVertexNormal());

	//face
	ui.check_iom_facecolor->setChecked(m->cm.HasPerFaceColor());
	//ui.check_iom_facequality->setChecked(m.cm.HasPerFaceQuality());
	ui.check_iom_facenormal->setChecked(m->cm.HasPerFaceNormal());

	//wedg
	ui.check_iom_wedgcolor->setChecked(m->cm.HasPerWedgeColor());
	ui.check_iom_wedgtexcoord->setChecked(m->cm.HasPerWedgeTexture());
	ui.check_iom_wedgnormal->setChecked(m->cm.HasPerWedgeNormal());

	//m.cm.HasPerVertexMark();
	//m.cm.HasPerFaceMark();
	//m.cm.HasPerWedgeMark();
	//m.cm.HasPerWedgeQuality();
	
	SetDisableChecks(type);
	SetTextureName();
}

/*
	Questo metodo serve per specificare le varie personalizzazioni dei vari salvataggi.
*/
void SaveMaskExporterDialog::SetDisableChecks(int type)
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

void SaveMaskExporterDialog::SetTextureName()
{
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
	ui.renametextureButton->setDisabled(false);
}