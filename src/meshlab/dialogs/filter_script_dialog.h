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
#ifndef MESHLAB_FILTER_SCRIPT_DIALOG_H
#define MESHLAB_FILTER_SCRIPT_DIALOG_H

#include <QDialog>
#include "../common/filterscript.h"

namespace Ui {
class FilterScriptDialog;
} 

class FilterScriptDialog : public QDialog
{
	Q_OBJECT
public:
	FilterScriptDialog(FilterScript& fs, QWidget *parent = 0);
	~FilterScriptDialog();

private slots:
	void on_saveScriptButton_clicked();
	
	void on_openScriptButton_clicked();
	
	void on_clearScriptButton_clicked();
	
	void on_okButton_clicked();
	
	//moves the filter selected in scriptListWidget up in the script
	void on_moveUpButton_clicked();
	
	//moves the filter selected in scriptListWidget down in the script
	void on_moveDownButton_clicked();
	
	//removes the selected filter from the script
	void on_removeFilterButton_clicked();
	
	//edit the parameters of the selected filter
	void on_editParameterButton_clicked();
	
private:
	void editOldParameters(const int row);
	void updateGui();
	
	Ui::FilterScriptDialog* ui;
	FilterScript& script;
};

#endif //MESHLAB_FILTER_SCRIPT_DIALOG_H
