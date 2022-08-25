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

#ifndef MESHLAB_OPTIONS_DIALOG_H
#define MESHLAB_OPTIONS_DIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <common/parameters/rich_parameter_list.h>

/**
 * @brief This class describes the dialog of the meshlab general options.
 * It creates a QTableWidget starting from the given currentParameterList,
 * and allows to reset to default values using defaulParameterList.
 * Every time that the user selects a RichParameter list, a SettingDialog is
 * created, allowing the user to load, save, modify and reset every option.
 * Every time the user modifies a RichParameter, the given currentParameterList
 * is modified, and a signal called applyCustomSetting is emitted.
 */
class MeshLabOptionsDialog : public QDialog
{
Q_OBJECT
public:
	MeshLabOptionsDialog(
			RichParameterList& currentParameterList,
			const RichParameterList& defaultPrameterList,
			QWidget *parent = 0);
	~MeshLabOptionsDialog();

signals:
	void applyCustomSetting();

private slots:
	void openSubDialog(QTableWidgetItem* itm);

	void updateSingleSetting(const RichParameter& rp);

private:
	void updateSettings();

	RichParameterList& currentParameterList;
	const RichParameterList& defaultParameterList;
	QTableWidget* tw;
	QPushButton* closebut;

	static QTableWidgetItem* createQTableWidgetItemFromRichParameter(const RichParameter& pd);
};

#endif // MESHLAB_OPTIONS_DIALOG_H
