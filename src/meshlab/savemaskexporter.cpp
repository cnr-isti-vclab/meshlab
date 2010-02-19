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

#include <Qt>
#include <QtGui>
#include <QFileInfo>

#include "ui_savemaskexporter.h"
#include "savemaskexporter.h"
#include "changetexturename.h"

SaveMaskExporterDialog::SaveMaskExporterDialog(QWidget *parent,MeshModel *m,int capability,int defaultBits, RichParameterSet *_parSet,GLArea* glar):
QDialog(parent),m(m),capability(capability),defaultBits(defaultBits),parSet(_parSet),glar(glar)
{
	ui = new Ui::MaskExporterDialog();
	InitDialog();
}

void SaveMaskExporterDialog::InitDialog()
{
	SaveMaskExporterDialog::ui->setupUi(this);
	connect(ui->okButton, SIGNAL(clicked()), this, SLOT(SlotOkButton()));
	connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(SlotCancelButton()));
	connect(ui->renametextureButton,SIGNAL(clicked()),this,SLOT(SlotRenameTexture()));
	connect(ui->listTextureName,SIGNAL(itemSelectionChanged()),this,SLOT(SlotSelectionTextureName()));
	connect(ui->AllButton,SIGNAL(clicked()),this,SLOT(SlotSelectionAllButton()));
	connect(ui->NoneButton,SIGNAL(clicked()),this,SLOT(SlotSelectionNoneButton()));
	ui->renametextureButton->setDisabled(true);

  stdParFrame = new StdParFrame(this,glar);
    stdParFrame->loadFrameContent(*parSet);
  QVBoxLayout *vbox = new QVBoxLayout(this);
	vbox->addWidget(stdParFrame);
	ui->saveParBox->setLayout(vbox);

	// Show the additional parameters only for formats that have some.
    if(parSet->isEmpty()) ui->saveParBox->hide();
								else ui->saveParBox->show();
	//all - none
	ui->AllButton->setChecked(true);
	//ui->NoneButton->setChecked(true);

	SetTextureName();
	SetMaskCapability();
}

void SaveMaskExporterDialog::SetTextureName()
{
	if( m->cm.textures.size() == 0 )
	{
		ui->check_iom_wedgtexcoord->setDisabled(true);
		ui->check_iom_wedgtexcoord->setChecked(false);
	}

	for(unsigned int i=0;i<m->cm.textures.size();i++)
	{
		QString item(m->cm.textures[i].c_str());
		ui->listTextureName->addItem(item);
	}
}

int SaveMaskExporterDialog::GetNewMask()
{
	return this->mask;
}

/*
 there are three things that are looked when setting the initial states of the checkbox of this dialog
 - this->capabilityBit
 - this->defaultBit


	setDisabled(true): uncheckable
	setDisabled(false) : checkable
	
	true  : when the information is not present or in the Capability or in the MeshModel Mask
	false : when the information is present in the Capability and in the Mask MeshModel

	setChecked(true) : checked
	setChecked(false): unchecked
	
	true  : the information is present both in the Capability and the MeshModel Mask
	false : otherwise.

*/
bool SaveMaskExporterDialog::shouldBeChecked(int bit, int /*capabilityBits*/, int defaultBits)
{
	if(!m->hasDataMask(MeshModel::io2mm(bit))) return false;
	//if( (bit & meshBits) == 0 ) return false;
	if( (bit & defaultBits) == 0 ) return false;
	return true;
}

bool SaveMaskExporterDialog::shouldBeEnabled(int iobit, int capabilityBits, int /*defaultBits*/)
{
	if( (iobit & capabilityBits) == 0 ) return false;
	int mmbit = MeshModel::io2mm(iobit);
	if(!m->hasDataMask(mmbit)) return false;
	return true;
}

void SaveMaskExporterDialog::checkAndEnable(QCheckBox *qcb,int bit, int capabilityBits, int defaultBits)
{
 qcb->setEnabled(shouldBeEnabled (bit,capabilityBits, defaultBits) );
 qcb->setChecked(shouldBeChecked (bit,capabilityBits, defaultBits) );
}

void SaveMaskExporterDialog::SetMaskCapability()
{
	//vert
	checkAndEnable(ui->check_iom_vertquality,  vcg::tri::io::Mask::IOM_VERTQUALITY,  capability, defaultBits );
	checkAndEnable(ui->check_iom_vertflags,    vcg::tri::io::Mask::IOM_VERTFLAGS,    capability, defaultBits);
	checkAndEnable(ui->check_iom_vertcolor,    vcg::tri::io::Mask::IOM_VERTCOLOR,    capability, defaultBits);
	checkAndEnable(ui->check_iom_verttexcoord, vcg::tri::io::Mask::IOM_VERTTEXCOORD, capability, defaultBits);
	checkAndEnable(ui->check_iom_vertnormal,   vcg::tri::io::Mask::IOM_VERTNORMAL,   capability, defaultBits);
	checkAndEnable(ui->check_iom_vertradius,   vcg::tri::io::Mask::IOM_VERTRADIUS,   capability, defaultBits);
	
	//face
	checkAndEnable(ui->check_iom_facequality, vcg::tri::io::Mask::IOM_FACEQUALITY, capability, defaultBits );
	checkAndEnable(ui->check_iom_faceflags,   vcg::tri::io::Mask::IOM_FACEFLAGS,   capability, defaultBits );
	checkAndEnable(ui->check_iom_facecolor,   vcg::tri::io::Mask::IOM_FACECOLOR,   capability, defaultBits );
	checkAndEnable(ui->check_iom_facenormal,  vcg::tri::io::Mask::IOM_FACENORMAL,  capability, defaultBits );

	//wedge
	checkAndEnable(ui->check_iom_wedgcolor,    vcg::tri::io::Mask::IOM_WEDGCOLOR,    capability, defaultBits );
	checkAndEnable(ui->check_iom_wedgtexcoord, vcg::tri::io::Mask::IOM_WEDGTEXCOORD, capability, defaultBits );
	checkAndEnable(ui->check_iom_wedgnormal,   vcg::tri::io::Mask::IOM_WEDGNORMAL,   capability, defaultBits );

	checkAndEnable(ui->check_iom_polygonal,   vcg::tri::io::Mask::IOM_BITPOLYGONAL,   capability, defaultBits );


	//camera THIS ONE HAS TO BE CORRECTED !!!! 
	//bool camval = m->cm.shot.IsValid();
	//int res = capability & vcg::tri::io::Mask::IOM_CAMERA;
	ui->check_iom_camera->setDisabled( ((capability & vcg::tri::io::Mask::IOM_CAMERA)==0) | (m->cm.shot.IsValid() == false));
	ui->check_iom_camera->setChecked ( ((capability & vcg::tri::io::Mask::IOM_CAMERA)!=0) & (m->cm.shot.IsValid()));


	if(capability == 0)
		ui->NoneButton->setChecked(true);
}

//slot
void SaveMaskExporterDialog::SlotOkButton()
{
	int newmask = 0;

	if( ui->check_iom_vertflags->isChecked()		) { newmask |= vcg::tri::io::Mask::IOM_VERTFLAGS;}
	if( ui->check_iom_vertcolor->isChecked()		) { newmask |= vcg::tri::io::Mask::IOM_VERTCOLOR;}
	if( ui->check_iom_vertquality->isChecked()	) { newmask |= vcg::tri::io::Mask::IOM_VERTQUALITY;}
	if( ui->check_iom_verttexcoord->isChecked()	) { newmask |= vcg::tri::io::Mask::IOM_VERTTEXCOORD;}
	if( ui->check_iom_vertnormal->isChecked()	) { newmask |= vcg::tri::io::Mask::IOM_VERTNORMAL;}
	if( ui->check_iom_vertradius->isChecked()	) { newmask |= vcg::tri::io::Mask::IOM_VERTRADIUS;}

	if( ui->check_iom_faceflags->isChecked()		) { newmask |= vcg::tri::io::Mask::IOM_FACEFLAGS;}
	if( ui->check_iom_facecolor->isChecked()		) { newmask |= vcg::tri::io::Mask::IOM_FACECOLOR;}
	if( ui->check_iom_facequality->isChecked()	) { newmask |= vcg::tri::io::Mask::IOM_FACEQUALITY;}
	if( ui->check_iom_facenormal->isChecked()	) { newmask |= vcg::tri::io::Mask::IOM_FACENORMAL;}

	if( ui->check_iom_wedgcolor->isChecked()		) { newmask |= vcg::tri::io::Mask::IOM_WEDGCOLOR;}
	if( ui->check_iom_wedgtexcoord->isChecked()	) { newmask |= vcg::tri::io::Mask::IOM_WEDGTEXCOORD;}
	if( ui->check_iom_wedgnormal->isChecked()	) { newmask |= vcg::tri::io::Mask::IOM_WEDGNORMAL;}

	if( ui->check_iom_camera->isChecked()		) { newmask |= vcg::tri::io::Mask::IOM_CAMERA;}
	if( ui->check_iom_polygonal->isChecked()		) { newmask |= vcg::tri::io::Mask::IOM_BITPOLYGONAL;}

	for(unsigned int i=0;i<m->cm.textures.size();i++)
		m->cm.textures[i] = ui->listTextureName->item(i)->text().toStdString();
	this->mask=newmask;
    stdParFrame->readValues(*parSet);
}

void SaveMaskExporterDialog::SlotCancelButton()
{
	this->mask=-1;
}

void SaveMaskExporterDialog::SlotRenameTexture()
{
	int row = ui->listTextureName->currentRow();
	std::string newtexture = vcg::tri::io::TextureRename::GetNewTextureName(m->cm.textures[row].c_str());
	if(newtexture.size()>0)
	{
		QStringList lists = QString(newtexture.c_str()).split('/');
		(ui->listTextureName->currentItem())->setText(lists[lists.size()-1]);
	}
}

void SaveMaskExporterDialog::SlotSelectionTextureName()
{
	ui->renametextureButton->setDisabled(false);
}

void SaveMaskExporterDialog::SlotSelectionAllButton()
{
	//vert
	ui->check_iom_vertquality->setChecked(ui->check_iom_vertquality->isEnabled());
	ui->check_iom_vertflags->setChecked(ui->check_iom_vertflags->isEnabled());
	ui->check_iom_vertcolor->setChecked(ui->check_iom_vertcolor->isEnabled());
	ui->check_iom_verttexcoord->setChecked(ui->check_iom_verttexcoord->isEnabled());
	ui->check_iom_vertnormal->setChecked(ui->check_iom_vertnormal->isEnabled());
	ui->check_iom_vertradius->setChecked(ui->check_iom_vertradius->isEnabled());

	//face
	ui->check_iom_facequality->setChecked(ui->check_iom_facequality->isEnabled());
	ui->check_iom_faceflags->setChecked(ui->check_iom_faceflags->isEnabled());
	ui->check_iom_facenormal->setChecked(ui->check_iom_facenormal->isEnabled());
	ui->check_iom_facecolor->setChecked(ui->check_iom_facecolor->isEnabled());
	
	//wedg
	ui->check_iom_wedgcolor->setChecked(ui->check_iom_wedgcolor->isEnabled());
	ui->check_iom_wedgtexcoord->setChecked(ui->check_iom_wedgtexcoord->isEnabled());
	ui->check_iom_wedgnormal->setChecked(ui->check_iom_wedgnormal->isEnabled());

	//camera
	ui->check_iom_camera->setChecked(ui->check_iom_camera->isEnabled());
}

void SaveMaskExporterDialog::SlotSelectionNoneButton()
{
	//vert
	ui->check_iom_vertquality->setChecked(false);
	ui->check_iom_vertflags->setChecked(false);
	ui->check_iom_vertcolor->setChecked(false);
	ui->check_iom_verttexcoord->setChecked(false);
	ui->check_iom_vertnormal->setChecked(false);
	ui->check_iom_vertradius->setChecked(false);

	//face
	ui->check_iom_facequality->setChecked(false);
	ui->check_iom_faceflags->setChecked(false);
	ui->check_iom_facenormal->setChecked(false);
	ui->check_iom_facecolor->setChecked(false);
	
	//wedg
	ui->check_iom_wedgcolor->setChecked(false);
	ui->check_iom_wedgtexcoord->setChecked(false);
	ui->check_iom_wedgnormal->setChecked(false);
	
	//camera
	ui->check_iom_camera->setChecked(false);
}

void SaveMaskExporterDialog::SetTitleDialog( const QString& filename )
{
	this->setWindowTitle("Exporting Parameters Options for " + filename);
}

void SaveMaskExporterDialog::on_check_help_stateChanged(int)
{
	stdParFrame->toggleHelp();
}

SaveMaskExporterDialog::~SaveMaskExporterDialog()
{
	delete ui;
}
