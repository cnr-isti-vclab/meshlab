/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

#ifndef MESHCUTDIALOG_H
#define MESHCUTDIALOG_H

#include <QWidget>
#include "ui_meshcutdialog.h"
#include <colorpicker.h>

class MeshCutDialog : public QWidget
{
	Q_OBJECT

public:
	MeshCutDialog(QWidget *parent = 0);
	~MeshCutDialog();

	inline bool SelectForeground() {return ui.foreRadioButton->isChecked();}
	inline QColor getForegroundColor() {return ui.foreColorPicker->getColor();}
	inline QColor getBackgroundColor() {return ui.backColorPicker->getColor();}

private:
	Ui::MeshCutDialogClass ui;

	public slots:
		void on_meshSegmentButton_clicked();
		void on_updateCurvatureButton_clicked();
		
		void on_foreRadioButton_clicked();
		void on_backRadioButton_clicked();

		void on_refineCheckBox_clicked();

		void on_gaussianButton_clicked();
		void on_meanButton_clicked();
		void on_resetButton_clicked();

		void on_normalWeightSlider_valueChanged();
		void on_curvatureWeightSlider_valueChanged();		
		
		void on_penRadiusSlider_valueChanged();
		
	signals:
		void meshCutSignal();
		void updateCurvatureSignal();
		
		void selectForegroundSignal(bool);
		void doRefineSignal(bool);
		void normalWeightSignal(int);
		void curvatureWeightSignal(int);
		
		void penRadiusSignal(int);

		void colorizeGaussianSignal();
		void colorizeMeanSignal();
		void resetSignal();
};

#endif // MESHCUTDIALOG_H
