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

#ifndef MESHLAB_CONGRATS_DIALOG_H
#define MESHLAB_CONGRATS_DIALOG_H

#include <QDialog>

namespace Ui {
class CongratsDialog;
}

class CongratsDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit CongratsDialog(QWidget *parent = nullptr);
	~CongratsDialog();
	
private slots:
	void on_githubButton_clicked();
	
	void on_emailButton_clicked();
	
	void on_closeButton_clicked();
	
private:
	void updateDontShowSetting();
	Ui::CongratsDialog *ui;
};

#endif // MESHLAB_CONGRATS_DIALOG_H
