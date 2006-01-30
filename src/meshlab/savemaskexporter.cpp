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
 Revision 1.4  2006/01/30 10:02:57  fmazzant
 update none selection

 Revision 1.3  2006/01/30 00:26:40  fmazzant
 deleted small bug

 Revision 1.2  2006/01/29 23:52:43  fmazzant
 correct a small bug

 Revision 1.1  2006/01/26 18:39:19  fmazzant
 moved mask dialog exporter from mashio to meshlab

 Revision 1.18  2006/01/19 15:59:00  fmazzant
 moved savemaskexporter to mainwindows

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

SaveMaskExporterDialog::SaveMaskExporterDialog(QWidget *parent,MeshModel *m,int capability): QDialog(parent),m(m),capability(capability)
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
	
	//all - none
	ui.AllButton->setChecked(true);
	//ui.NoneButton->setChecked(true);

	SetTextureName();
	SetMaskCapability();
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

/*
	setDisabled(true): non si puo' checkare
	setDisabled(false) : si puo' checkare
	
	true  : quando l'informazione non e' presente o nella capability oppure nella mask di MeshModel
	false : quando l'informazione e' presente sia nella capability e sia nella mask di MeshModel

	setChecked(true) : checkato
	setChecked(false): non checkato
	
	true  : dato quando l'informazione e' presente sia nella capability sia nella mask di MeshModel
	false : altrimenti.

*/

void SaveMaskExporterDialog::SetMaskCapability()
{
	//vert
	ui.check_iom_vertquality->setDisabled( ((capability & MeshModel::IOM_VERTQUALITY)==0) /*| ((m->mask & MeshModel::IOM_VERTQUALITY)==0)*/ );
	ui.check_iom_vertquality->setChecked ( ((capability & MeshModel::IOM_VERTQUALITY)!=0) /*& ((m->mask & MeshModel::IOM_VERTQUALITY)!=0)*/);
	
	ui.check_iom_vertflags->setDisabled( ((capability & MeshModel::IOM_VERTFLAGS)==0) /*| ((m->mask & MeshModel::IOM_VERTFLAGS)==0)*/ );
	ui.check_iom_vertflags->setChecked ( ((capability & MeshModel::IOM_VERTFLAGS)!=0) /*& ((m->mask & MeshModel::IOM_VERTFLAGS)!=0)*/ );

	ui.check_iom_vertcolor->setDisabled( ((capability & MeshModel::IOM_VERTCOLOR)==0) /*| ((m->mask & MeshModel::IOM_VERTCOLOR)==0) */);
	ui.check_iom_vertcolor->setChecked ( ((capability & MeshModel::IOM_VERTCOLOR)!=0) /*& ((m->mask & MeshModel::IOM_VERTCOLOR)!=0)*/ );
	
	ui.check_iom_verttexcoord->setDisabled( ((capability & MeshModel::IOM_VERTTEXCOORD)==0) /*| ((m->mask & MeshModel::IOM_VERTTEXCOORD)==0)*/ );
	ui.check_iom_verttexcoord->setChecked ( ((capability & MeshModel::IOM_VERTTEXCOORD)!=0) /*& ((m->mask & MeshModel::IOM_VERTTEXCOORD)!=0) */);

	ui.check_iom_vertnormal->setDisabled( ((capability & MeshModel::IOM_VERTNORMAL)==0) /*| ((m->mask & MeshModel::IOM_VERTNORMAL)==0)*/ );
	ui.check_iom_vertnormal->setChecked ( ((capability & MeshModel::IOM_VERTNORMAL)!=0) /*& ((m->mask & MeshModel::IOM_VERTNORMAL)!=0)*/ );

	//face
	ui.check_iom_facequality->setDisabled( ((capability & MeshModel::IOM_FACEQUALITY)==0) /*| ((m->mask & MeshModel::IOM_FACEQUALITY)==0)*/);
	ui.check_iom_facequality->setChecked ( ((capability & MeshModel::IOM_FACEQUALITY)!=0) /*& ((m->mask & MeshModel::IOM_FACEQUALITY)!=0)*/);

	ui.check_iom_faceflags->setDisabled( ((capability & MeshModel::IOM_FACEFLAGS)==0) /*| ((m->mask & MeshModel::IOM_FACEFLAGS)==0) */);
	ui.check_iom_faceflags->setChecked ( ((capability & MeshModel::IOM_FACEFLAGS)!=0) /*& ((m->mask & MeshModel::IOM_FACEFLAGS)!=0) */);

	ui.check_iom_facecolor->setDisabled( ((capability & MeshModel::IOM_FACECOLOR)==0) /*| ((m->mask & MeshModel::IOM_FACECOLOR)==0) */);
	ui.check_iom_facecolor->setChecked ( ((capability & MeshModel::IOM_FACECOLOR)!=0) /*& ((m->mask & MeshModel::IOM_FACECOLOR)!=0) */);
	
	ui.check_iom_facenormal->setDisabled( ((capability & MeshModel::IOM_FACENORMAL)==0) /*| ((m->mask & MeshModel::IOM_FACENORMAL)==0) */);
	ui.check_iom_facenormal->setChecked ( ((capability & MeshModel::IOM_FACENORMAL)!=0) /*& ((m->mask & MeshModel::IOM_FACENORMAL)!=0) */);

	//wedg
	ui.check_iom_wedgcolor->setDisabled( ((capability & MeshModel::IOM_WEDGCOLOR)==0) /*| ((m->mask & MeshModel::IOM_WEDGCOLOR)==0) */);
	ui.check_iom_wedgcolor->setChecked ( ((capability & MeshModel::IOM_WEDGCOLOR)!=0) /*& ((m->mask & MeshModel::IOM_WEDGCOLOR)!=0) */);

	ui.check_iom_wedgtexcoord->setDisabled( ((capability & MeshModel::IOM_WEDGTEXCOORD)==0) /*| ((m->mask & MeshModel::IOM_WEDGTEXCOORD)==0) */);
	ui.check_iom_wedgtexcoord->setChecked ( ((capability & MeshModel::IOM_WEDGTEXCOORD)!=0) /*& ((m->mask & MeshModel::IOM_WEDGTEXCOORD)!=0) */);

	ui.check_iom_wedgnormal->setDisabled( ((capability & MeshModel::IOM_WEDGNORMAL)==0) /*| ((m->mask & MeshModel::IOM_WEDGNORMAL)==0) */);
	ui.check_iom_wedgnormal->setChecked ( ((capability & MeshModel::IOM_WEDGNORMAL)!=0) /*& ((m->mask & MeshModel::IOM_WEDGNORMAL)!=0) */);

	//camera
	ui.check_iom_camera->setDisabled( ((capability & MeshModel::IOM_CAMERA)==0) /*| ((m->mask & MeshModel::IOM_CAMERA)==0) */);
	ui.check_iom_camera->setChecked ( ((capability & MeshModel::IOM_CAMERA)!=0) /*& ((m->mask & MeshModel::IOM_CAMERA)!=0) */);


	if(capability == 0)
		ui.NoneButton->setChecked(true);
}

//slot
void SaveMaskExporterDialog::SlotOkButton()
{
	int newmask = 0;

	if( ui.check_iom_vertflags->isChecked()		) { newmask |= MeshModel::IOM_VERTFLAGS;}
	if( ui.check_iom_vertcolor->isChecked()		) { newmask |= MeshModel::IOM_VERTCOLOR;}
	if( ui.check_iom_vertquality->isChecked()	) { newmask |= MeshModel::IOM_VERTQUALITY;}
	if( ui.check_iom_verttexcoord->isChecked()	) { newmask |= MeshModel::IOM_VERTTEXCOORD;}
	if( ui.check_iom_vertnormal->isChecked()	) { newmask |= MeshModel::IOM_VERTNORMAL;}

	if( ui.check_iom_faceflags->isChecked()		) { newmask |= MeshModel::IOM_FACEFLAGS;}
	if( ui.check_iom_facecolor->isChecked()		) { newmask |= MeshModel::IOM_FACECOLOR;}
	if( ui.check_iom_facequality->isChecked()	) { newmask |= MeshModel::IOM_FACEQUALITY;}
	if( ui.check_iom_facenormal->isChecked()	) { newmask |= MeshModel::IOM_FACENORMAL;}

	if( ui.check_iom_wedgcolor->isChecked()		) { newmask |= MeshModel::IOM_WEDGCOLOR;}
	if( ui.check_iom_wedgtexcoord->isChecked()	) { newmask |= MeshModel::IOM_WEDGTEXCOORD;}
	if( ui.check_iom_wedgnormal->isChecked()	) { newmask |= MeshModel::IOM_WEDGNORMAL;}

	if( ui.check_iom_camera->isChecked()		) { newmask |= MeshModel::IOM_CAMERA;}

	for(unsigned int i=0;i<m->cm.textures.size();i++)
		m->cm.textures[i] = ui.listTextureName->item(i)->text().toStdString();
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

void SaveMaskExporterDialog::SlotSelectionAllButton()
{
	//vert
	ui.check_iom_vertquality->setChecked(ui.check_iom_vertquality->isEnabled());
	ui.check_iom_vertflags->setChecked(ui.check_iom_vertflags->isEnabled());
	ui.check_iom_vertcolor->setChecked(ui.check_iom_vertcolor->isEnabled());
	ui.check_iom_verttexcoord->setChecked(ui.check_iom_verttexcoord->isEnabled());
	ui.check_iom_vertnormal->setChecked(ui.check_iom_vertnormal->isEnabled());

	//face
	ui.check_iom_facequality->setChecked(ui.check_iom_facequality->isEnabled());
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
	ui.check_iom_vertquality->setChecked(false);//(ui.check_iom_vertquality->isEnabled() & ui.check_iom_vertquality->isChecked()) && !ui.check_iom_vertquality->isChecked());
	ui.check_iom_vertflags->setChecked(false);//(ui.check_iom_vertflags->isEnabled()& ui.check_iom_vertflags->isChecked())&& !ui.check_iom_vertflags->isChecked() );
	ui.check_iom_vertcolor->setChecked(false);//(ui.check_iom_vertcolor->isEnabled() & ui.check_iom_vertcolor->isChecked()) && !ui.check_iom_vertcolor->isChecked());
	ui.check_iom_verttexcoord->setChecked(false);//(ui.check_iom_verttexcoord->isEnabled() & ui.check_iom_verttexcoord->isChecked())&& !ui.check_iom_verttexcoord->isChecked());
	ui.check_iom_vertnormal->setChecked(false);//(ui.check_iom_vertnormal->isEnabled() & ui.check_iom_vertnormal->isChecked())&& !ui.check_iom_vertnormal->isChecked());

	//face
	ui.check_iom_facequality->setChecked(false);//(ui.check_iom_facequality->isEnabled() & ui.check_iom_facequality->isChecked()) && !ui.check_iom_facequality->isChecked());
	ui.check_iom_faceflags->setChecked(false);//(ui.check_iom_faceflags->isEnabled() & ui.check_iom_faceflags->isChecked()) && !ui.check_iom_faceflags->isChecked());
	ui.check_iom_facenormal->setChecked(false);//(ui.check_iom_facenormal->isEnabled()& ui.check_iom_facenormal->isChecked()) && !ui.check_iom_facenormal->isChecked());
	ui.check_iom_facecolor->setChecked(false);//(ui.check_iom_facecolor->isEnabled()& ui.check_iom_facecolor->isChecked()) && !ui.check_iom_facecolor->isChecked());
	
	//wedg
	ui.check_iom_wedgcolor->setChecked(false);//(ui.check_iom_wedgcolor->isEnabled() & ui.check_iom_wedgcolor->isChecked()) && !ui.check_iom_wedgcolor->isChecked());
	ui.check_iom_wedgtexcoord->setChecked(false);//(ui.check_iom_wedgtexcoord->isEnabled() & ui.check_iom_wedgtexcoord->isChecked()) && !ui.check_iom_wedgtexcoord->isChecked());
	ui.check_iom_wedgnormal->setChecked(false);//(ui.check_iom_wedgnormal->isEnabled() & ui.check_iom_wedgnormal->isChecked()) && !ui.check_iom_wedgtexcoord->isChecked());
}