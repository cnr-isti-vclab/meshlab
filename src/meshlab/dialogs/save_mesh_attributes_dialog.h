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

#ifndef __VCGLIB_SAVEMASK_EXPORT
#define __VCGLIB_SAVEMASK_EXPORT

#include <QDialog>
#include <QListWidgetItem>

#include <wrap/io_trimesh/io_mask.h>

#include "rich_parameter_gui/richparameterlistframe.h"
#include <common/parameters/rich_parameter_list.h>
#include "glarea.h"
//
// Each file format exposes:
//  a capability bit vector with all the things that it can save (the bits are the one indicated in the IOM_XXX bit mask) ; 
//  a default bit vector that indicate what things are saved by defaults (eg. by default normals and flags are not saved in ply)
//  a vector of optional parameters (like for example a bool for ascii/binary, a bool for choosing what  
// This dialog allows one to select what things actually save. 
// 
// 
// 

namespace Ui 
{
	class SaveMeshAttributesDialog;
}

class SaveMeshAttributesDialog : public QDialog
{
	Q_OBJECT
public:
	SaveMeshAttributesDialog(
			QWidget* parent,
			MeshModel* m,
			int capability,
			int defaultBits,
			const RichParameterList& additionalSaveParams,
			GLArea* glar = NULL);
	~SaveMeshAttributesDialog();

	void selectAllPossibleBits();

	int getNewMask() const;
	RichParameterList getNewAdditionalSaveParameters() const;
	std::vector<std::string> getTextureNames() const;
	bool saveTextures() const;
	int getTextureQuality() const;

private slots:
	void on_okButton_clicked();
	void on_cancelButton_clicked();
	void on_check_help_stateChanged(int);
	void on_listTextureName_itemDoubleClicked(QListWidgetItem *item);
	void on_AllButton_clicked();
	void on_NoneButton_clicked();

	void on_saveTextureCheckBox_stateChanged(int arg1);

	void on_textureQualitySpinBox_valueChanged(int arg1);

private:
	void setMaskCapability();
	void checkAndEnable(QCheckBox *qcb,int bit, int capabilityBits, int defaultBits);
	bool shouldBeEnabled(int bit, int capabilityBits, int defaultBits);
	bool shouldBeChecked(int bit, int capabilityBits, int defaultBits);
	void updateMask();

	Ui::SaveMeshAttributesDialog* ui;
	MeshModel *m;
	const int capability;
	const int defaultBits;

	int mask;
	int textureQuality;

	RichParameterList additionalSaveParametrs;
	std::vector<std::string> textureNames;

	RichParameterListFrame *additionalParametersFrame;
	GLArea* glar;
};//end class

#endif
