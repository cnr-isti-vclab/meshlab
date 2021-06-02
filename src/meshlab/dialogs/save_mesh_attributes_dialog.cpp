/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2021                                           \/)\/    *
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

#include "ui_save_mesh_attributes_dialog.h"
#include "save_mesh_attributes_dialog.h"

#include <QInputDialog>
#include <QFileInfo>

SaveMeshAttributesDialog::SaveMeshAttributesDialog(
		QWidget *parent,
		MeshModel *m,
		int capability,
		int defaultBits,
		const RichParameterList& additionalSaveParams,
		GLArea* glar):
	QDialog(parent),
	ui(new Ui::SaveMeshAttributesDialog),
	m(m),
	capability(capability),
	defaultBits(defaultBits),
	mask(0),
	additionalSaveParametrs(additionalSaveParams),
	glar(glar)
{
	ui->setupUi(this);

	ui->renametextureButton->setDisabled(true);

	additionalParametersFrame = new RichParameterListFrame(additionalSaveParametrs, this,glar);
	QVBoxLayout *vbox = new QVBoxLayout(this);
	vbox->addWidget(additionalParametersFrame);
	ui->saveParBox->setLayout(vbox);
	QFileInfo fi(m->fullName());
	this->setWindowTitle("Choose Saving Options for: '"+ fi.baseName() +"'");
	// Show the additional parameters only for formats that have some.
	if(additionalSaveParametrs.isEmpty())
		ui->saveParBox->hide();
	else
		ui->saveParBox->show();
	//all - none
	ui->AllButton->setChecked(true);
	//ui->NoneButton->setChecked(true);

	if( m->cm.textures.size() == 0 )
	{
		ui->check_iom_wedgtexcoord->setDisabled(true);
		ui->check_iom_wedgtexcoord->setChecked(false);
	}

	textureNames.reserve(m->cm.textures.size());
	for(const std::string& tname : m->cm.textures)
	{
		textureNames.push_back(tname);
		QString item(tname.c_str());
		ui->listTextureName->addItem(item);
	}
	setMaskCapability();
}

SaveMeshAttributesDialog::~SaveMeshAttributesDialog()
{
	delete ui;
}

void SaveMeshAttributesDialog::selectAllPossibleBits()
{
	on_AllButton_clicked();
	updateMask();
}

int SaveMeshAttributesDialog::getNewMask() const
{
	return this->mask;
}

RichParameterList SaveMeshAttributesDialog::getNewAdditionalSaveParameters() const
{
	return additionalSaveParametrs;
}

std::vector<std::string> SaveMeshAttributesDialog::getTextureNames() const
{
	return textureNames;
}

void SaveMeshAttributesDialog::on_okButton_clicked()
{
	updateMask();
	additionalParametersFrame->writeValuesOnParameterList(additionalSaveParametrs);
}

void SaveMeshAttributesDialog::on_cancelButton_clicked()
{
	mask=-1;
}

void SaveMeshAttributesDialog::on_check_help_stateChanged(int)
{
	additionalParametersFrame->toggleHelp();
}

void SaveMeshAttributesDialog::on_renametextureButton_clicked()
{
	int row = ui->listTextureName->currentRow();
	//ChangeTextureNameDialog dialog(this, textureNames[row].c_str());
	//dialog.exec();
	bool ok;
	QString newtexture =
			QInputDialog::getText(this, "New Texture Name",
						"Enter the new texture name:", QLineEdit::Normal,
						QString::fromStdString(textureNames[row]), &ok);
	//dialog.close();
	if(ok && newtexture.size()>0 ) {
		textureNames[row] = newtexture.toStdString();
		ui->listTextureName->currentItem()->setText(newtexture);
	}
}

void SaveMeshAttributesDialog::on_listTextureName_itemSelectionChanged()
{
	ui->renametextureButton->setDisabled(false);
}

void SaveMeshAttributesDialog::on_AllButton_clicked()
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

void SaveMeshAttributesDialog::on_NoneButton_clicked()
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

void SaveMeshAttributesDialog::setMaskCapability()
{
	//vert
	checkAndEnable(ui->check_iom_vertquality,  vcg::tri::io::Mask::IOM_VERTQUALITY,  capability, defaultBits );
	checkAndEnable(ui->check_iom_vertflags,    vcg::tri::io::Mask::IOM_VERTFLAGS,    capability, defaultBits);
	checkAndEnable(ui->check_iom_vertcolor,    vcg::tri::io::Mask::IOM_VERTCOLOR,    capability, defaultBits);
	checkAndEnable(ui->check_iom_verttexcoord, vcg::tri::io::Mask::IOM_VERTTEXCOORD, capability, defaultBits);
	checkAndEnable(ui->check_iom_vertnormal,   vcg::tri::io::Mask::IOM_VERTNORMAL,   capability, defaultBits);
	checkAndEnable(ui->check_iom_vertradius,   vcg::tri::io::Mask::IOM_VERTRADIUS,   capability, defaultBits);

	// point cloud fix: if a point cloud, probably you'd want to save vertex normals
	if ((m->cm.fn == 0) && (m->cm.en == 0))
		ui->check_iom_vertnormal->setChecked(true);

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
	ui->check_iom_camera->setDisabled( ((capability & vcg::tri::io::Mask::IOM_CAMERA)==0) || (m->cm.shot.IsValid() == false));
	ui->check_iom_camera->setChecked ( ((capability & vcg::tri::io::Mask::IOM_CAMERA)!=0) && (m->cm.shot.IsValid()));

	if(capability == 0)
		ui->NoneButton->setChecked(true);
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
void SaveMeshAttributesDialog::checkAndEnable(QCheckBox *qcb,int bit, int capabilityBits, int defaultBits)
{
	qcb->setEnabled(shouldBeEnabled (bit,capabilityBits, defaultBits) );
	qcb->setChecked(shouldBeChecked (bit,capabilityBits, defaultBits) );
}

bool SaveMeshAttributesDialog::shouldBeEnabled(int iobit, int capabilityBits, int /*defaultBits*/)
{
	if( (iobit & capabilityBits) == 0 ) return false;
	int mmbit = MeshModel::io2mm(iobit);
	if(!m->hasDataMask(mmbit)) return false;
	return true;
}

bool SaveMeshAttributesDialog::shouldBeChecked(int bit, int /*capabilityBits*/, int defaultBits)
{
	if(!m->hasDataMask(MeshModel::io2mm(bit))) return false;
	//if( (bit & meshBits) == 0 ) return false;
	if( (bit & defaultBits) == 0 ) return false;
	return true;
}

void SaveMeshAttributesDialog::updateMask()
{
	int newmask = 0;

	if( ui->check_iom_vertflags->isChecked()    ) { newmask |= vcg::tri::io::Mask::IOM_VERTFLAGS;}
	if( ui->check_iom_vertcolor->isChecked()    ) { newmask |= vcg::tri::io::Mask::IOM_VERTCOLOR;}
	if( ui->check_iom_vertquality->isChecked()  ) { newmask |= vcg::tri::io::Mask::IOM_VERTQUALITY;}
	if( ui->check_iom_verttexcoord->isChecked() ) { newmask |= vcg::tri::io::Mask::IOM_VERTTEXCOORD;}
	if( ui->check_iom_vertnormal->isChecked()   ) { newmask |= vcg::tri::io::Mask::IOM_VERTNORMAL;}
	if( ui->check_iom_vertradius->isChecked()   ) { newmask |= vcg::tri::io::Mask::IOM_VERTRADIUS;}

	if( ui->check_iom_faceflags->isChecked()    ) { newmask |= vcg::tri::io::Mask::IOM_FACEFLAGS;}
	if( ui->check_iom_facecolor->isChecked()    ) { newmask |= vcg::tri::io::Mask::IOM_FACECOLOR;}
	if( ui->check_iom_facequality->isChecked()  ) { newmask |= vcg::tri::io::Mask::IOM_FACEQUALITY;}
	if( ui->check_iom_facenormal->isChecked()   ) { newmask |= vcg::tri::io::Mask::IOM_FACENORMAL;}

	if( ui->check_iom_wedgcolor->isChecked()    ) { newmask |= vcg::tri::io::Mask::IOM_WEDGCOLOR;}
	if( ui->check_iom_wedgtexcoord->isChecked() ) { newmask |= vcg::tri::io::Mask::IOM_WEDGTEXCOORD;}
	if( ui->check_iom_wedgnormal->isChecked()   ) { newmask |= vcg::tri::io::Mask::IOM_WEDGNORMAL;}

	if( ui->check_iom_camera->isChecked()       ) { newmask |= vcg::tri::io::Mask::IOM_CAMERA;}
	if( ui->check_iom_polygonal->isChecked()    ) { newmask |= vcg::tri::io::Mask::IOM_BITPOLYGONAL;}

	this->mask=newmask;
}
