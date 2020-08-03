/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004-2020                                           \/)\/    *
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

#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>
#include <common/filter_parameter/rich_parameter.h>
#include "../rich_parameter_gui/richparameterlistframe.h"

class SettingDialog : public QDialog
{
Q_OBJECT
public:
	SettingDialog(const RichParameter& curPar, const RichParameter& defPar,QWidget* parent = 0);
	~SettingDialog();
public slots:
	void save();
	void reset();
	void apply();
	void load();

signals:
	void applySettingSignal(const RichParameter&);

private:
	RichParameterListFrame frame;
	RichParameter* curPar;
	RichParameter* tmpPar;
	const RichParameter& defPar;
	QPushButton* savebut;
	QPushButton* applybut;
	QPushButton* resetbut;
	QPushButton* closebut;
	QPushButton* loadbut;
};

#endif // SETTINGDIALOG_H
