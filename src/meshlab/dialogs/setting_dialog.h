/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2020                                           \/)\/    *
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

#ifndef MESHLAB_SETTING_DIALOG_H
#define MESHLAB_SETTING_DIALOG_H

#include <QDialog>
#include <common/parameters/rich_parameter.h>
#include "../rich_parameter_gui/richparameterlistframe.h"

/**
 * @brief This class describes a dialog to load, save, modify and reset a
 * RichParameter. Every time that a RichParameter is modified, a signal called
 * "applySettingSignal" is emitted, with the given RichParameter changed.
 */
class SettingDialog : public QDialog
{
Q_OBJECT
public:
	SettingDialog(
			const RichParameter& currentParameter,
			const RichParameter& defaultParameter,
			QWidget* parent = 0);
	~SettingDialog();
public slots:
	void save();
	void reset();
	void apply();
	void load();

signals:
	void applySettingSignal(const RichParameter&);

private:
	RichParameter* currentParameter;
	RichParameter* savedParameter; //used in case of load button pressed
	const RichParameter& defaultParameter;
	RichParameterListFrame frame;
	QPushButton* savebut;
	QPushButton* applybut;
	QPushButton* resetbut;
	QPushButton* closebut;
	QPushButton* loadbut;
};

#endif // MESHLAB_SETTING_DIALOG_H
