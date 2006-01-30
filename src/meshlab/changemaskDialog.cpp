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
 Revision 1.1  2006/01/30 06:32:33  buzzelli
 added a dialog used to select type of data being imported


 ****************************************************************************/
#include "changeMaskDialog.h"
#include "MeshModel.h"
//#include <wrap/io_trimesh/io_mask.h>

ChangeMaskDialog::ChangeMaskDialog(int capability, QWidget *parent) : capability(capability), QDialog(parent)
{
	setupUi(this);
	mask = capability;

	connect(okButton, SIGNAL(clicked()), this, SLOT(slotOkButtonPressed()));
	connect(AllButton,SIGNAL(clicked()),this,SLOT(slotSelectionAllButtonPressed()));
	connect(NoneButton,SIGNAL(clicked()),this,SLOT(slotSelectionNoneButtonPressed()));

	AllButton->setChecked(true);
	setMaskCapability();
}

void ChangeMaskDialog::setMaskCapability()
{
	//vertices
	(capability & MeshModel::IOM_VERTQUALITY)		? check_iom_vertquality->setChecked(true) : check_iom_vertquality->setEnabled(false);
	(capability & MeshModel::IOM_VERTFLAGS)			?	check_iom_vertflags->setChecked(true) : check_iom_vertflags->setEnabled(false);
	(capability & MeshModel::IOM_VERTCOLOR)			? check_iom_vertcolor->setChecked(true) : check_iom_vertcolor->setEnabled(false);
	(capability & MeshModel::IOM_VERTTEXCOORD)	? check_iom_verttexcoord->setChecked(true) :check_iom_verttexcoord->setEnabled(false);
	(capability & MeshModel::IOM_VERTNORMAL)		? check_iom_vertnormal->setChecked(true) : check_iom_vertnormal->setEnabled(false);
	
	// faces
	(capability & MeshModel::IOM_FACEQUALITY)		? check_iom_facequality->setChecked(true) : check_iom_facequality->setEnabled(false);
	(capability & MeshModel::IOM_FACEFLAGS)			? check_iom_faceflags->setChecked(true) : check_iom_faceflags->setEnabled(false);
	(capability & MeshModel::IOM_FACECOLOR)			? check_iom_facecolor->setChecked(true) : check_iom_facecolor->setEnabled(false);
	(capability & MeshModel::IOM_FACENORMAL)		? check_iom_facenormal->setChecked(true) : check_iom_facenormal->setEnabled(false);
	
	// wedges
	(capability & MeshModel::IOM_WEDGCOLOR)			? check_iom_wedgcolor->setChecked(true) : check_iom_wedgcolor->setEnabled(false);
	(capability & MeshModel::IOM_WEDGTEXCOORD)	? check_iom_wedgtexcoord->setChecked(true) :check_iom_wedgtexcoord->setEnabled(false);
	(capability & MeshModel::IOM_WEDGNORMAL)		? check_iom_wedgnormal->setChecked(true) : check_iom_wedgnormal->setEnabled(false);
	
	// camera
	(capability & MeshModel::IOM_CAMERA)				? check_iom_camera->setChecked(true) : check_iom_camera->setEnabled(false);
}

int ChangeMaskDialog::getNewMask()
{
	return mask;
}

void ChangeMaskDialog::slotOkButtonPressed()
{
	int newmask = 0;

	if( check_iom_vertflags->isChecked() )		{ newmask |= MeshModel::IOM_VERTFLAGS; }
	if( check_iom_vertcolor->isChecked() )		{ newmask |= MeshModel::IOM_VERTCOLOR; }
	if( check_iom_vertquality->isChecked() )	{ newmask |= MeshModel::IOM_VERTQUALITY; }
	if( check_iom_verttexcoord->isChecked() ) { newmask |= MeshModel::IOM_VERTTEXCOORD; }
	if( check_iom_vertnormal->isChecked() )		{ newmask |= MeshModel::IOM_VERTNORMAL; }

	if( check_iom_faceflags->isChecked() )		{ newmask |= MeshModel::IOM_FACEFLAGS; }
	if( check_iom_facecolor->isChecked() )		{ newmask |= MeshModel::IOM_FACECOLOR; }
	if( check_iom_facequality->isChecked() )	{ newmask |= MeshModel::IOM_FACEQUALITY; }
	if( check_iom_facenormal->isChecked()	)		{ newmask |= MeshModel::IOM_FACENORMAL; }

	if( check_iom_wedgcolor->isChecked() )		{ newmask |= MeshModel::IOM_WEDGCOLOR; }
	if( check_iom_wedgtexcoord->isChecked() ) { newmask |= MeshModel::IOM_WEDGTEXCOORD; }
	if( check_iom_wedgnormal->isChecked()	)		{ newmask |= MeshModel::IOM_WEDGNORMAL; }

	if( check_iom_camera->isChecked() )				{ newmask |= MeshModel::IOM_CAMERA; }

	mask = newmask;
}

void ChangeMaskDialog::slotSelectionAllButtonPressed()
{
	//vertices
	if (capability & MeshModel::IOM_VERTQUALITY)	check_iom_vertquality->setChecked(true);
	if (capability & MeshModel::IOM_VERTFLAGS)		check_iom_vertflags->setChecked(true);
	if (capability & MeshModel::IOM_VERTCOLOR)		check_iom_vertcolor->setChecked(true);
	if (capability & MeshModel::IOM_VERTTEXCOORD)	check_iom_verttexcoord->setChecked(true);
	if (capability & MeshModel::IOM_VERTNORMAL)		check_iom_vertnormal->setChecked(true);
	
	// faces
	if (capability & MeshModel::IOM_FACEQUALITY)	check_iom_facequality->setChecked(true);
	if (capability & MeshModel::IOM_FACEFLAGS)		check_iom_faceflags->setChecked(true);
	if (capability & MeshModel::IOM_FACECOLOR)		check_iom_facecolor->setChecked(true);
	if (capability & MeshModel::IOM_FACENORMAL)		check_iom_facenormal->setChecked(true);
	
	// wedges
	if (capability & MeshModel::IOM_WEDGCOLOR)		check_iom_wedgcolor->setChecked(true);
	if (capability & MeshModel::IOM_WEDGTEXCOORD)	check_iom_wedgtexcoord->setChecked(true);
	if (capability & MeshModel::IOM_WEDGNORMAL)		check_iom_wedgnormal->setChecked(true);
	
	// camera
	if (capability & MeshModel::IOM_CAMERA)				check_iom_camera->setChecked(true);
}

void ChangeMaskDialog::slotSelectionNoneButtonPressed()
{
	//vertices
	if (capability & MeshModel::IOM_VERTQUALITY)	check_iom_vertquality->setChecked(false);
	if (capability & MeshModel::IOM_VERTFLAGS)		check_iom_vertflags->setChecked(false);
	if (capability & MeshModel::IOM_VERTCOLOR)		check_iom_vertcolor->setChecked(false);
	if (capability & MeshModel::IOM_VERTTEXCOORD)	check_iom_verttexcoord->setChecked(false);
	if (capability & MeshModel::IOM_VERTNORMAL)		check_iom_vertnormal->setChecked(false);
	
	// faces
	if (capability & MeshModel::IOM_FACEQUALITY)	check_iom_facequality->setChecked(false);
	if (capability & MeshModel::IOM_FACEFLAGS)		check_iom_faceflags->setChecked(false);
	if (capability & MeshModel::IOM_FACECOLOR)		check_iom_facecolor->setChecked(false);
	if (capability & MeshModel::IOM_FACENORMAL)		check_iom_facenormal->setChecked(false);
	
	// wedges
	if (capability & MeshModel::IOM_WEDGCOLOR)		check_iom_wedgcolor->setChecked(false);
	if (capability & MeshModel::IOM_WEDGTEXCOORD)	check_iom_wedgtexcoord->setChecked(false);
	if (capability & MeshModel::IOM_WEDGNORMAL)		check_iom_wedgnormal->setChecked(false);
	
	// camera
	if (capability & MeshModel::IOM_CAMERA)				check_iom_camera->setChecked(false);
}